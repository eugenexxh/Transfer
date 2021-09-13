#pragma once
#include <ActiveQt/QAxObject>
#include <ObjBase.h>

class ExcelUtil
{
public:
	ExcelUtil();
	~ExcelUtil();

	//��ָ����Excel���Զ����´�����
	void UpdateErrorCode(const QString& strPathExcel, const QString& strPathErrorCode);

	void DeleteErrorCode(const QString& strPathExcel, const QString& strPathErrorCode);

	//��Ŀ¼������Excel�ļ������ϣ��ٷ�Ϊ��ͨ���ì�ܱ�����·����Ҫ�������룩
	void PreHandelExcel(const QString& strDir, const QString& strPathExcelGeneral, const QString& strPathExcelConflict);
	//����ͨ����Ϊ��ȫ��ͷ��ձ�����·���Զ����ɣ�
	void HandelGeneralExcel(const QString& strPathExcelGeneral);
	//�ð�ȫ���Զ��滻�����е�cpp�ļ���UI�ļ�
	void ReplaceStringCppsAndUIs(const QString& strPathExcelSafe, const QString& strDir);
	//�ð�ȫ���Զ��滻�ֵ�
	void ReplaceStringDic(const QString& strPathExcelSafe, const QString& strPathExcelDic);

private:
	void CastVariant2ListList(const QVariant& varData, QList<QList<QVariant>>& varListList);
	void CastListList2Variant(const QList<QList<QVariant>>& varListList, QVariant& varData);
	void ReadFromExcel(const QString& strPathExcel, QList<QList<QVariant>>& varListList);
	void WriteToNewExcel(const QString& strPathExcel, const QList<QList<QVariant>>& varList);
	//��ȡһ��Ŀ¼������Excel��·���������룩
	void ExtractAllExcelPaths(const QString& strDir, QStringList& strExcelPathList);
	//��ȡ·�������е�Excel��
	void ReadAllExcels(const QString& strDir, QList<QList<QVariant>>& varListListTotal);
	//������һ��Ϊ�գ�����Ҫ�޸ģ��Ĺ��˵�
	void HandleEmpty(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListRes);
	//����ȫȥ�أ�����һ�Σ�
	void HandleDuplicate(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListRes);
	//���Ϊ��ͨ��ì��
	void HandleConflict(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListConflict, QList<QList<QVariant>>& varListListGeneral);
	//һ�������еĵ��ʸ���
	int WordCountofSentence(const QString& strSentence);
	//�жϴ����޸��Ƿ�ȫ
	void ClassifySafe(const QString& strSrc, const QString& strSuggest, bool& bflag);
	//��ȡһ��Ŀ¼������Ŀ¼�µ������ļ�·��
	int FindFile(const QString& _filePath);
	//����·��������ȡcpp�ļ���UI�ļ�·����
	void ExtractCppAndUIPaths(const QStringList& paths, QStringList& cppAndUIPaths);
	//�滻һ���ļ��е��ַ���
	void ReplaceStringOneFile(const QString& filePath, QString strSrc, QString strTarget);

	QStringList m_depPathList;
};
