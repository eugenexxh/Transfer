//解决Qt中的中文乱码
#pragma execution_character_set("utf-8")

#include <QtCore/QCoreApplication>
#include <QDebug>
#include "ExcelUtil.h"

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	//use ExcelUtil do something
	QString strExcel = "D:/newE/Learn_MySQL/delete.xlsx";
	QString strDB = "D:/newE/Learn_MySQL/localDB/test.db";
	ExcelUtil excelUtil;
	excelUtil.DeleteErrorCode(strExcel, strDB);


	qDebug() << "End";
	return a.exec();
}

