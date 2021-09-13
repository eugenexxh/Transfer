//解决Qt中的中文乱码
#pragma execution_character_set("utf-8")

#include <QtCore/QCoreApplication>
#include <QDebug>
#include "ExcelUtil.h"

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	//use ExcelUtil do something

	qDebug() << "End";
	return a.exec();
}

