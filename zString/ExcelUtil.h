#pragma once
#include <ActiveQt/QAxObject>
#include <ObjBase.h>

class ExcelUtil
{
public:
	ExcelUtil();
	~ExcelUtil();

	//用指定的Excel表自动更新错误码
	void UpdateErrorCode(const QString& strPathExcel, const QString& strPathErrorCode);

	void DeleteErrorCode(const QString& strPathExcel, const QString& strPathErrorCode);

	//将目录下所有Excel文件先整合，再分为普通表和矛盾表（两表路径需要生动输入）
	void PreHandelExcel(const QString& strDir, const QString& strPathExcelGeneral, const QString& strPathExcelConflict);
	//将普通表，分为安全表和风险表（两表路径自动生成）
	void HandelGeneralExcel(const QString& strPathExcelGeneral);
	//用安全表自动替换代码中的cpp文件和UI文件
	void ReplaceStringCppsAndUIs(const QString& strPathExcelSafe, const QString& strDir);
	//用安全表自动替换字典
	void ReplaceStringDic(const QString& strPathExcelSafe, const QString& strPathExcelDic);

private:
	void CastVariant2ListList(const QVariant& varData, QList<QList<QVariant>>& varListList);
	void CastListList2Variant(const QList<QList<QVariant>>& varListList, QVariant& varData);
	void ReadFromExcel(const QString& strPathExcel, QList<QList<QVariant>>& varListList);
	void WriteToNewExcel(const QString& strPathExcel, const QList<QList<QVariant>>& varList);
	//提取一个目录下所有Excel的路径（不深入）
	void ExtractAllExcelPaths(const QString& strDir, QStringList& strExcelPathList);
	//读取路径下所有的Excel表
	void ReadAllExcels(const QString& strDir, QList<QList<QVariant>>& varListListTotal);
	//将建议一栏为空（不需要修改）的过滤掉
	void HandleEmpty(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListRes);
	//不完全去重（保留一次）
	void HandleDuplicate(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListRes);
	//拆分为普通与矛盾
	void HandleConflict(QList<QList<QVariant>>& varListList, QList<QList<QVariant>>& varListListConflict, QList<QList<QVariant>>& varListListGeneral);
	//一个句子中的单词个数
	int WordCountofSentence(const QString& strSentence);
	//判断词条修改是否安全
	void ClassifySafe(const QString& strSrc, const QString& strSuggest, bool& bflag);
	//提取一个目录及其子目录下的所有文件路径
	int FindFile(const QString& _filePath);
	//从总路径集中提取cpp文件和UI文件路径集
	void ExtractCppAndUIPaths(const QStringList& paths, QStringList& cppAndUIPaths);
	//替换一个文件中的字符串
	void ReplaceStringOneFile(const QString& filePath, QString strSrc, QString strTarget);

	QStringList m_depPathList;
};
