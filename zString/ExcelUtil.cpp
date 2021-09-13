#include "ExcelUtil.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QList>
#include <map>

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

ExcelUtil::ExcelUtil()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED); //���QAxObject�ڶ��߳��б���
}

ExcelUtil::~ExcelUtil()
{

}

void ExcelUtil::UpdateErrorCode(const QString& strPathExcel, const QString& strPathErrorCode)
{
	//1.����Excel����
	QList<QList<QVariant>> varListList;
	ReadFromExcel(strPathExcel, varListList);

	//2.�������ݿ�
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
	QSqlQuery sqlQuery;
	database.setDatabaseName(strPathErrorCode);
	if (!database.open())
	{
		qDebug() << "Error: Failed to connect database." << database.lastError();
		return;
	}

	//3.�������ݿ�
	int bflag = false;
	int iRow = varListList.size();
	for (int i = 0; i < iRow; ++i)
	{
		QList<QVariant> lineList = varListList[i];
		int tmpId = lineList[0].toInt();
		QString tmpEN = lineList[1].toString();
		QString tmpZH = lineList[2].toString();
		QString tmpDE = lineList[3].toString();
		QString tmpFI = lineList[4].toString();
		QString tmpJP = lineList[5].toString();

		sqlQuery.prepare("UPDATE error_code SET en=?,zh=?,de=?,fi=?,jp=? WHERE code_id=?");
		sqlQuery.addBindValue(tmpEN);
		sqlQuery.addBindValue(tmpZH);
		sqlQuery.addBindValue(tmpDE);
		sqlQuery.addBindValue(tmpFI);
		sqlQuery.addBindValue(tmpJP);
		sqlQuery.addBindValue(tmpId);
		bflag = sqlQuery.exec();
	}

	if (bflag)
	{
		qDebug() << "updated errorDB success!";
	}
	else
	{
		qDebug() << "updated errorDB error!";
	}
}

void ExcelUtil::PreHandelExcel(const QString& strDir, const QString& strPathExcelGeneral, const QString& strPathExcelConflict)
{
	//1.�������е�Excel
	QList<QList<QVariant>> varListList;
	ReadAllExcels(strDir, varListList);
	//2.ȥ��
	QList<QList<QVariant>> varListListNotEmpty;
	HandleEmpty(varListList, varListListNotEmpty);
	//3.ȥ��
	QList<QList<QVariant>> varListListNotDuplicate;
	HandleDuplicate(varListListNotEmpty, varListListNotDuplicate);
	//4.��Ϊ��ͨ��ì��
	QList<QList<QVariant>> varListListGeneral;
	QList<QList<QVariant>> varListListConflict;
	HandleConflict(varListListNotDuplicate, varListListGeneral, varListListConflict);
	//5.�ֱ�д����ͨ����ì�ܱ�
	WriteToNewExcel(strPathExcelGeneral, varListListGeneral);
	WriteToNewExcel(strPathExcelConflict, varListListConflict);
}

void ExcelUtil::HandelGeneralExcel(const QString& strPathExcelGeneral)
{
	//1.���ɰ�ȫ������ձ��·��
	QString strPathExcelSafe = strPathExcelGeneral;
	QString strPathExcelRisk = strPathExcelGeneral;
	QString strSrc = ".xlsx";
	int num = strSrc.size();
	int pos = strPathExcelGeneral.indexOf(strSrc);
	QString strTarget1 = "_safe.xlsx";
	QString strTarget2 = "_risk.xlsx";
	strPathExcelSafe.replace(pos, num, strTarget1);
	strPathExcelRisk.replace(pos, num, strTarget2);
	//2.������ͨ�������
	QList<QList<QVariant>> varListList;
	ReadFromExcel(strPathExcelGeneral, varListList);
	//3.����ͨ��Ϊ��ȫ�����
	QList<QList<QVariant>> varListListSafe;
	QList<QList<QVariant>> varListListRisk;
	int iRow = varListList.size();
	for (int i = 0; i < iRow; ++i)
	{
		bool bSafeflag = false;
		QString tmpSrc = varListList[i][0].toString();
		QString tmpSuggest = varListList[i][1].toString();
		ClassifySafe(tmpSrc, tmpSuggest, bSafeflag);

		if (bSafeflag)
		{
			varListListSafe.append(varListList[i]);
		}
		else
		{
			varListListRisk.append(varListList[i]);
		}
	}
	//4.�ֱ�д�밲ȫ������ձ�
	WriteToNewExcel(strPathExcelSafe, varListListSafe);
	WriteToNewExcel(strPathExcelRisk, varListListRisk);
}

void ExcelUtil::ReplaceStringCppsAndUIs(const QString& strPathExcelSafe, const QString& strDir)
{
	//1.����Excel����
	QList<QList<QVariant>> varListList;
	ReadFromExcel(strPathExcelSafe, varListList);

	//2.��ȡ���е�cpp�ļ�·����UI�ļ�·��
	QStringList cppAndUIPathList;
	m_depPathList.clear();
	FindFile(strDir);
	ExtractCppAndUIPaths(m_depPathList, cppAndUIPathList);
	m_depPathList.clear();

	//3.�滻�����е��ַ���
	int iRow = varListList.size();
	int iNumFile = cppAndUIPathList.size();
	for (int i = 0; i < iRow; ++i)
	{
		QString tmpSrc = varListList[i][0].toString();
		QString tmpSuggest = varListList[i][1].toString();

		for (int j = 0; j < iNumFile; ++j)
		{
			QString filePath = cppAndUIPathList[j];
			ReplaceStringOneFile(filePath, tmpSrc, tmpSuggest);
		}
	}
}

void ExcelUtil::ReplaceStringDic(const QString& strPathExcelSafe, const QString& strPathExcelDic)
{
	//1.����Excel����
	QList<QList<QVariant>> varListListSafe;
	QList<QList<QVariant>> varListListDic;
	ReadFromExcel(strPathExcelSafe, varListListSafe);
	ReadFromExcel(strPathExcelDic, varListListDic);

	//2.�滻�ַ���
	int iRowSafe = varListListSafe.size();
	int iRowDic = varListListDic.size();
	for (int i = 0; i < iRowSafe; ++i)
	{
		QString tmpSrcSafe = varListListSafe[i][0].toString();
		QString tmpSuggestSafe = varListListSafe[i][1].toString();

		for (int j = 1; j < iRowDic; ++j)
		{
			QString tmpSrcDic = varListListDic[j][0].toString();
			if (tmpSrcDic == tmpSrcSafe)
			{
				varListListDic[j][0] = QVariant(tmpSuggestSafe);
				varListListDic[j][1] = QVariant(tmpSuggestSafe);
			}
		}
	}
}

void ExcelUtil::CastVariant2ListList(const QVariant& varData, QList<QList<QVariant> >& varListList)
{
	QVariantList varList = varData.toList();
	int iRow = varList.size();
	for (int i = 0; i < iRow; ++i)
	{
		QVariantList singleLineList = varList[i].toList();
		varListList.append(singleLineList);
	}
}

void ExcelUtil::CastListList2Variant(const QList<QList<QVariant>>& varListList, QVariant& varData)
{
	QVariantList vars;
	const int iRow = varListList.size();
	for (int i = 0; i < iRow; ++i)
	{
		vars.append(QVariant(varListList[i]));
	}
	varData = QVariant(vars);
}

void ExcelUtil::ReadFromExcel(const QString& strPathExcel, QList<QList<QVariant>>& varListList)
{
	QAxObject* pExcel = new QAxObject("Excel.Application");		//����Excel�������󣬲�����Excel�ؼ�
	pExcel->setProperty("Visible", false);						//����ʾ����
	pExcel->setProperty("DisplayAlerts", false);				//����ʾ������Ϣ
	QAxObject* pWorkBooks = pExcel->querySubObject("WorkBooks");		//��ȡ����������
	QAxObject* pWorkBook = pWorkBooks->querySubObject("Open(QString, QVariant)", QDir::toNativeSeparators(strPathExcel)); //��һ�����й�����
	QAxObject* pWorkSheet = pWorkBook->querySubObject("WorkSheets(int)", 1); //���ʵ�һ��������

	QAxObject* pUsedRange = pWorkSheet->querySubObject("UsedRange");
	QVariant varData = pUsedRange->dynamicCall("Value");
	CastVariant2ListList(varData, varListList);
	varData.clear();

	pWorkBook->dynamicCall("Close()");
	pExcel->dynamicCall("Quit()");
	SAFE_DELETE(pUsedRange);
	SAFE_DELETE(pWorkSheet);
	SAFE_DELETE(pWorkBook);
	SAFE_DELETE(pWorkBooks);
	SAFE_DELETE(pExcel);
}

void ExcelUtil::WriteToNewExcel(const QString& strPathExcel, const QList<QList<QVariant>>& varList)
{
	int iRow = varList.size();
	QString strRange = QString("A1:") + QString("C") + QString::number(iRow);
	QVariant varData;
	CastListList2Variant(varList, varData);

	QAxObject* pExcel = new QAxObject("Excel.Application");
	pExcel->setProperty("Visible", false);
	pExcel->setProperty("DisplayAlerts", false);
	QAxObject* pWorkBooks = pExcel->querySubObject("WorkBooks");
	pWorkBooks->dynamicCall("Add");
	QAxObject* pWorkBook = pExcel->querySubObject("ActiveWorkBook");
	QAxObject* pWorkSheet = pWorkBook->querySubObject("Worksheets(int)", 1);

	QAxObject* pUsedRange = pWorkSheet->querySubObject("Range(const QString&)", strRange);
	pUsedRange->setProperty("Value", varData);
	pWorkBook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(strPathExcel));

	pWorkBook->dynamicCall("Close()");
	pExcel->dynamicCall("Quit()");
	SAFE_DELETE(pUsedRange);
	SAFE_DELETE(pWorkSheet);
	SAFE_DELETE(pWorkBook);
	SAFE_DELETE(pWorkBooks);
	SAFE_DELETE(pExcel);
}

void ExcelUtil::ExtractAllExcelPaths(const QString& strDir, QStringList& strExcelPathList)
{
	QDir dir(strDir);
	QList<QFileInfo> fileInfoList = dir.entryInfoList();
	int num = fileInfoList.count();
	for (int i = 0; i < num; i++)
	{
		QString strFilePath = fileInfoList.at(i).filePath();
		QString strFileName = fileInfoList.at(i).fileName();
		QString strTarget = ".xlsx";
		if (strFileName.contains(strTarget, Qt::CaseSensitive))
		{
			strExcelPathList.append(strFilePath);
		}
	}
}

void ExcelUtil::ReadAllExcels(const QString& strDir, QList<QList<QVariant>>& varListListTotal)
{
	QStringList excelPaths;
	ExtractAllExcelPaths(strDir, excelPaths);

	int num = excelPaths.size();
	for (int i = 0; i < num; ++i)
	{
		QList<QList<QVariant>> varListList;
		ReadFromExcel(excelPaths.at(i), varListList);

		for (int j = 0; j < varListList.size(); ++j)
		{
			QList<QVariant> lineList = varListList[j];
			varListListTotal.append(lineList);
		}
	}
}

void ExcelUtil::HandleEmpty(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListRes)
{
	int iRow = varListList.size();
	for (int i = 0; i < iRow; ++i)
	{
		QString tmpSuggest = varListList[i][1].toString();
		if (tmpSuggest != "")
		{
			varListListRes.append(varListList[i]);
		}
	}

	varListList.clear();
}

void ExcelUtil::HandleDuplicate(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListRes)
{
	std::map<QString, QList<QVariant>> map;

	int iRow = varListList.size();
	for (int i = 0; i < iRow; ++i)
	{
		QString tmpSrc = varListList[i][0].toString();
		QString tmpSuggest = varListList[i][1].toString();
		QString tmpKey = tmpSrc + tmpSuggest;
		map[tmpKey] = varListList[i];
	}

	for (auto e : map)
	{
		QList<QVariant> lineList = e.second;
		varListListRes.append(lineList);
	}

	varListList.clear();
}

//��νì�ܣ���ָһ���ַ���A�����޸�ΪB�����޸�ΪC
void ExcelUtil::HandleConflict(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListConflict, QList<QList<QVariant>>& varListListGeneral)
{
	QList<QString> suggestList;

	int iRow = varListList.size();
	for (int i = 0; i < iRow; ++i)
	{
		QString tmpSrc = varListList[i][0].toString();
		QString tmpSuggest = varListList[i][1].toString();
		bool bConflictFlag = false;

		for (int j = i + 1; j < iRow; ++j)
		{
			QString tmpSrc1 = varListList[j][0].toString();
			QString tmpSuggest1 = varListList[j][1].toString();
			if (tmpSrc == tmpSrc1)
			{
				bConflictFlag = true;
				varListListConflict.append(varListList[j]);
				suggestList.append(tmpSuggest1);
			}
		}

		if (bConflictFlag)
		{
			varListListConflict.append(varListList[i]);
			suggestList.append(tmpSuggest);
		}
	}

	for (int i = 0; i < iRow; ++i)
	{
		QString tmpSuggest = varListList[i][1].toString();
		bool bflag = false;

		for (int j = 0; j < suggestList.size(); ++j)
		{
			if (tmpSuggest == suggestList[j])
			{
				bflag = true;
				break;
			}
		}

		if (bflag == false)
		{
			varListListGeneral.append(varListList[i]);
		}
	}

	varListList.clear();
}

int ExcelUtil::WordCountofSentence(const QString& strSentence)
{
	int count = 0;
	foreach(QChar qc, strSentence)
	{
		if (qc.isSpace())
		{
			count += 1;
		}
	}
	count += 1;
	return count;
}

void ExcelUtil::ClassifySafe(const QString& strSrc, const QString& strSuggest, bool& bflagSafe)
{
	//Todo, Ӧ�û�Ҫ�ٿ��������ַ���ʡ�Ժ�
	int n1 = WordCountofSentence(strSrc);
	int n2 = WordCountofSentence(strSuggest);
	if (n1 >= 4 && n2 >= 4)
	{
		bflagSafe = true;
	}
	else
	{
		bflagSafe = false;
	}
}

int ExcelUtil::FindFile(const QString& _filePath)
{
	QDir dir(_filePath);   //QDir��·��һ��Ҫ��ȫ·�������·�����д���

	if (!dir.exists())
		return -1;

	//ȡ�����е��ļ����ļ�����ȥ��.��..�ļ���
	dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
	dir.setSorting(QDir::DirsFirst);
	//����ת��Ϊһ��list
	QFileInfoList list = dir.entryInfoList();
	if (list.size() < 1)
		return -1;

	int i = 0;
	//���õݹ��㷨
	do {
		QFileInfo fileInfo = list.at(i);
		bool bisDir = fileInfo.isDir();
		if (bisDir)
		{
			FindFile(fileInfo.filePath());
		}
		else
		{
			m_depPathList.append(fileInfo.filePath());
			//qDebug() << fileInfo.filePath() << ":" << fileInfo.fileName();
		}

		++i;
	} while (i < list.size());

	return 0;
}

void ExcelUtil::ExtractCppAndUIPaths(const QStringList& paths, QStringList& cppAndUIPaths)
{
	int num = paths.size();
	for (int i = 0; i < num; i++)
	{
		QString strFilePath = paths.at(i);
		QString strTargetCpp = ".cpp";
		QString strTargetUI = ".ui";

		if (strFilePath.contains(strTargetCpp, Qt::CaseSensitive) || strFilePath.contains(strTargetUI, Qt::CaseSensitive))
		{
			cppAndUIPaths.append(strFilePath);
		}
	}
}

void ExcelUtil::ReplaceStringOneFile(const QString& filePath, QString strSrc, QString strTarget)
{
	//1.�ж����ͣ��������ʹ���Դ�ַ����ͽ����ַ���
	QString strCpp = ".cpp";
	QString strtUI = ".ui";
	if (filePath.contains(strCpp, Qt::CaseSensitive))
	{
		strSrc = "tr(" + strSrc + ")";
		strTarget = "tr(" + strTarget + ")";
	}
	else if (filePath.contains(strtUI, Qt::CaseSensitive))
	{
		strSrc = "<string>" + strSrc + "</string>";
		strTarget = "<string>" + strTarget + "</string>";
	}
	int num = strSrc.size();

	//2.�滻�ַ���
	QString Alltemp = "";
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		stream.setCodec("utf-8");

		while (!stream.atEnd())
		{
			QString line = stream.readLine();
			if (line.contains(strSrc, Qt::CaseSensitive))
			{
				int pos = line.indexOf(strSrc);
				line.replace(pos, num, strTarget);
			}

			Alltemp = Alltemp + line;
			Alltemp += QString('\n');
		}

		file.close();
	}

	//3.�����滻�õ�����
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream stream1(&file);
		stream1.setCodec("utf-8");
		stream1 << Alltemp;
		file.close();
	}
}
