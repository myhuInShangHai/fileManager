#include "convertWidget.h"
#include "AutoParse.h"

#include "QFileDialog"
#include "QMessageBox"
#include "QDebug"
#include "QList"
#include "QStringList"
#include "QRegExpValidator"
#include "QRegExp"
#include "QHBoxLayout"
#include "QIcon"
#include "QToolButton"
#include "QCheckBox"
#include "QSettings"


ConvertWidget::ConvertWidget(QWidget* parent /* = 0 */, Qt::WindowFlags flag /* = 0 */)
	: QWidget(parent, flag),
	m_strFile(""),
	m_iToolFlag(TOOL_SLN_TO_EXCEL)
{
	QCoreApplication::setOrganizationName("FE-CLIENT");
	QCoreApplication::setApplicationName("FILECONVERT");
	ui.setupUi(this);
	initLayout();
}

ConvertWidget::~ConvertWidget()
{

}

void ConvertWidget::initLayout()
{
	QToolButton *button1 = new QToolButton;
	QIcon openicon = QIcon(":/images/open.png");
	button1->setFixedSize(25, 24);
	button1->setIcon(openicon);
	button1->setIconSize(QSize(16, 16));
	button1->setAutoRaise(true);
	button1->setCursor(Qt::ArrowCursor);
	button1->setStyleSheet("background:transparent;");

	QHBoxLayout *search_layout = new QHBoxLayout;
	search_layout->addStretch();
	search_layout->addWidget(button1);
	search_layout->setSpacing(0);
	search_layout->setContentsMargins(0, 0, 0, 0);
	ui.sourceFile->setLayout(search_layout);
	ui.sourceFile->setTextMargins(0, 0, button1->width(), 0);
	connect(button1, SIGNAL(clicked()), this, SLOT(OnSelectFile()));
	ui.sourceFile->setReadOnly(true);

	QToolButton *button2 = new QToolButton;
	button2->setFixedSize(25, 24);
	button2->setIcon(openicon);
	button2->setIconSize(QSize(16, 16));
	button2->setAutoRaise(true);
	button2->setCursor(Qt::ArrowCursor);
	button2->setStyleSheet("background:transparent;");

	QHBoxLayout *destLayout = new QHBoxLayout;
	destLayout->addStretch();
	destLayout->addWidget(button2);
	destLayout->setSpacing(0);
	destLayout->setContentsMargins(0, 0, 0, 0);
	ui.destDir->setLayout(destLayout);
	ui.destDir->setTextMargins(0, 0, button2->width(), 0);
	connect(button2, SIGNAL(clicked()), this, SLOT(outputPath()));
	ui.destDir->setReadOnly(true);

	connect(ui.typeSelect, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(OnCurrentIndexChanged(const QString&)));
	ui.typeSelect->addItem(tr("sln转化为excel"));
	ui.typeSelect->addItem(tr("excel转化为ts"));
	ui.typeSelect->addItem(tr("excel转化为qm"));

	ui.typeSelect->setCurrentText(tr("excel转化为qm"));

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OnConvert()));

	QStringList languageSet;
	languageSet << "中文";
	languageSet << "英语";
	languageSet << "德语";
	languageSet << "日语";
	languageSet << "韩语";

	for (int i = 0; i < languageSet.size(); ++i)
	{
		QListWidgetItem *item = new QListWidgetItem(ui.languageList);
		QCheckBox* language = new QCheckBox(languageSet.at(i));
		ui.languageList->setItemWidget(item, language);
		connect(language, SIGNAL(stateChanged(int)), this, SLOT(OnLanguageSelect(int)));
	}

	//fill ui from read QSettings
	QSettings settings;
	m_strFile = settings.value("sourcefile").toString();
	ui.sourceFile->setText(m_strFile);
	m_strPrefix = settings.value("prefix").toString();
	ui.fileName->setText(m_strPrefix);
	m_strOutPutPath = settings.value("outputpath").toString();
	ui.destDir->setText(m_strOutPutPath);
	QStringList languagelist = settings.value("language").toStringList();
	for (int i = 0; i < languagelist.size(); ++i)
	{
		if (!languagelist.at(i).isEmpty())
			dynamic_cast<QCheckBox *>(ui.languageList->itemWidget(ui.languageList->item(i)))->setChecked(true);
	}
}

void ConvertWidget::OnSelectFile()
{
	QString tmp;
	if (m_iToolFlag == TOOL_SLN_TO_EXCEL)
	{
		tmp = QFileDialog::getOpenFileName(this, tr("打开VS项目描述文件"), "", tr("TS(*.vcxproj)"));
	}
	else if (m_iToolFlag == TOOL_EXCEL_TO_TS)
	{
		tmp = QFileDialog::getOpenFileName(this, tr("打开excel文件"), "", tr("EXCEL(*.xls *.xlsx)"));
	}
	else if (m_iToolFlag == TOOL_EXCEL_TO_QM)
	{
		tmp = QFileDialog::getOpenFileName(this, tr("打开excel文件"), "", tr("EXCEL(*.xls *.xlsx)"));
	}

	if (!tmp.isEmpty())
	{
		m_strFile = tmp;
		ui.sourceFile->setText(m_strFile);
	}
}

void ConvertWidget::OnCurrentIndexChanged(const QString& strText)
{
	if (strText == tr("sln转化为excel"))
	{
		m_iToolFlag = TOOL_SLN_TO_EXCEL;
	}
	else if (strText == tr("excel转化为ts"))
	{
		m_iToolFlag = TOOL_EXCEL_TO_TS;
	}
	else if (strText == tr("excel转化为qm"))
	{
		m_iToolFlag = TOOL_EXCEL_TO_QM;
	}
}

void ConvertWidget::outputPath()
{
	QString tmp = QFileDialog::getExistingDirectory(this, tr("选择输出文件夹"), ".");
	if (!tmp.isEmpty())
	{
		m_strOutPutPath = tmp;
		ui.destDir->setText(m_strOutPutPath);
	}
}

void ConvertWidget::OnLanguageSelect(int state)
{
	mLanguageList.clear();
	for (int i = 0; i < LANGUAGEABBRE.size(); ++i)   //5 represent languageSet.size in line77
	{
		QCheckBox *language = dynamic_cast<QCheckBox *>(ui.languageList->itemWidget(ui.languageList->item(i)));
		if (language->checkState())
			mLanguageList << LANGUAGEABBRE.at(i);
		else
			mLanguageList << "";
	}

}

void ConvertWidget::OnConvert()
{
	if (m_strFile.isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("请选择待转化文件！"));
		return;
	}

	m_strOutPutPath = ui.destDir->text();
	if (m_strOutPutPath.isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("目标路径不能为空！"));
		return;
	}

	m_strPrefix = ui.fileName->text();
	if (m_strPrefix.isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("目标文件名称前缀不能为空！"));
		return;
	}

	QRegExp rx("[\\\\/:*?\"<>|]");
	int match = m_strPrefix.indexOf(rx);
	if (match >= 0)
	{
		QMessageBox::warning(this, tr("警告"), tr("文件名不能包含下列任何字符\n     \\/:*?<>|"));
		return;
	}

	if (mLanguageList.isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("请选择需要转化的语言！"));
		return;
	}

	mConvertList.clear();
	//catenat path,prefix,language abbreviation
	for (int i = 0; i < mLanguageList.size(); ++i)
	{
		if (mLanguageList.at(i).isEmpty())
			mConvertList << "";
		else
			mConvertList << m_strOutPutPath + "/" + m_strPrefix + "_" + mLanguageList[i];
	}

	switch (m_iToolFlag) {
	case TOOL_SLN_TO_EXCEL:
		/*{
		if (parseSln(m_strFile, mSourceList))
		{
		foreach (const QString &file, mSourceList)
		{
		QString source = m_strFile.left(m_strFile.lastIndexOf('/')) + "/" + file;
		qDebug() << source;
		if (file.endsWith(QLatin1String(".cpp"), Qt::CaseInsensitive))
		{
		}
		else if (file.endsWith(QLatin1String(".ui"), Qt::CaseInsensitive))
		{
		loadUI(source,mResult);
		}
		//other suffix java/jui/js/qs/qml not support now
		}

		writeResultToExcel("AA", mResult);
		}
		else
		{
		QMessageBox::information(this, tr("通知"), tr("文件无法读取或不含有任何需要解析文件"));
		}
		break;
		}*/
		break;
	case TOOL_EXCEL_TO_TS:
	{
		//parseExcelTOTs(m_strFile, m_strConvertFile);
		//QStringList list;
		//list << "D:/Work/Tools/excel2ts/en.ts";
		//parseExcelTOTs(m_strFile, list);
		break;
	}
	case TOOL_EXCEL_TO_QM:
	{
		int result = saveQM(m_strFile, mConvertList);
		switch (result)
		{
			case EXCEL_READ_ERROR:
				QMessageBox::information(this, tr("通知"), tr("EXCEL文件读取失败"));
				break;
			case SHEET_COUNT_ZERO:
				QMessageBox::information(this, tr("通知"), tr("EXCEL表单为空"));
				break;
			case CREATE_QM_PERMISSION_ERROR:
				QMessageBox::information(this, tr("通知"), tr("QM文件不能生成，检查导出目录权限"));
				break;
			default:
				QMessageBox::information(this, tr("通知"), tr("生成QM文件成功"));
				QSettings settings;
				settings.setValue("sourcefile", m_strFile);
				settings.setValue("prefix", m_strPrefix);
				settings.setValue("outputpath", m_strOutPutPath);
				settings.setValue("language", mLanguageList);
				break;
		}
		break;
	}
	default:
		break;
	}
}