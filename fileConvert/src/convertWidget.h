#ifndef CONVERTWIDGET_H
#define CONVERTWIDGET_H

#include "QtWidgets/QWidget"
#include "ui_convertWidgetClass.h"
#include "QtCore/QVector"
#include "QtCore/QMap"
#include "QStringList"
#include "QString"



#include "AutoParse.h"


#pragma execution_character_set("utf-8")     //

#define TOOL_SLN_TO_EXCEL			1		//sln to excel(now support xls format)
#define TOOL_EXCEL_TO_TS			2		//excel to ts
#define TOOL_EXCEL_TO_QM			3		//excel to qm


class ConvertWidget : public QWidget
{
	Q_OBJECT
public:
	ConvertWidget(QWidget* parent = 0, Qt::WindowFlags flag = 0);
	~ConvertWidget();

private:
	void							initLayout();
	void							readDataFromXml(const QString& strXml);
	void							readDataFromExcel(const QString& strExcel);

private slots:
	void							OnConvert();
	void							OnSelectFile();
	void							OnCurrentIndexChanged(const QString&);
	void							outputPath();
	void							OnLanguageSelect(int);

private:
	Ui::convertWidgetClass			ui;
	QString							m_strFile;
	QString							m_strPrefix;
	QString                         m_strOutPutPath;
	int								m_iToolFlag;
	QStringList						mLanguageList;
	QStringList						mConvertList;
};


#endif