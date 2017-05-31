#include "libxl.h"

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtCore/QString>

#include <QtCore/QTextCodec>
#include <QtCore/QLocale>
#include <QtCore/QIODevice>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>

#include <string>

using namespace libxl;

bool writeResultToExcel(const QString &filename, const QMap<QString, QString> &result)
{
	Book* book = xlCreateBook();

	QMap<QString, int> multisheet;

	Sheet  *sheet;

	QMap<QString, QString>::const_iterator i = result.constBegin();

	int j = 1;
	int k = 0;

	while (i != result.constEnd())
	{
		if (multisheet.contains(i.value()))
		{
			sheet = book->getSheet(multisheet[i.value()]);
			sheet->writeStr(sheet->lastRow(), 1, (const wchar_t *)i.key().constData());
		}
		else
		{
			sheet = book->addSheet((const wchar_t *)i.value().constData());
			multisheet[i.value()] = k;
			sheet->writeStr(sheet->lastRow(), 1, (const wchar_t *)i.key().constData());
			++k;
		}
		++i;
		++j;
	}

	for (int index = 0; index < book->sheetCount(); ++index)
	{
		Sheet *sheet = book->getSheet(index);
		sheet->setCol(1, 1, 100);
	}

	book->save(L"D:/Work/Tools/excel2ts/en.xls");
	book->release();
	return true;
}

static QString numericEntity(int ch)
{
	return QString(ch <= 0x20 ? QLatin1String("<byte value=\"x%1\"/>")
		: QLatin1String("&#x%1;")).arg(ch, 0, 16);
}

static QString protect(const QString &str)
{
	QString result;
	result.reserve(str.length() * 12 / 10);
	for (int i = 0; i != str.size(); ++i) {
		uint c = str.at(i).unicode();
		switch (c) {
		case '\"':
			result += QLatin1String("&quot;");
			break;
		case '&':
			result += QLatin1String("&amp;");
			break;
		case '>':
			result += QLatin1String("&gt;");
			break;
		case '<':
			result += QLatin1String("&lt;");
			break;
		case '\'':
			result += QLatin1String("&apos;");
			break;
		default:
			if (c < 0x20 && c != '\r' && c != '\n' && c != '\t')
				result += numericEntity(c);
			else // this also covers surrogates
				result += QChar(c);
		}
	}
	return result;
}


bool parseExcelTOTs(const QString &source, const QStringList &dest)
{
	Book	*book = xlCreateBook();
	//if (!book->load(source.toStdString().c_str()))
	if (!book->load((const wchar_t *)source.constData()))
	{
		qDebug() << "load error";
		return false;
	}
	int count = book->sheetCount();

	qDebug() << count;

	if (count == 0)
		return false;
	else
	{
		QString language;
		Sheet *tempSheet = book->getSheet(0);
		for (int index = 2; index < tempSheet->lastCol(); ++index)
		{
			language = QString((const QChar *)tempSheet->readStr(0, index));
			QLocale local("en");
			qDebug() << language;
		}
	}

	Sheet	*sheet;

	for (int k = 0; k < dest.size(); ++k)
	{
		QFile file(dest.at(k));

		if (!file.open(QIODevice::ReadWrite))
			return false;

		qDebug() << "Create file success";

		QTextStream stream(&file);
		stream.setCodec(QTextCodec::codecForName("UTF-8"));

		stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE TS>\n";

		stream << "<TS version=\"2.0\"";

		//stream << " language=\"" << languageCode << "\"";   //set language

		stream << ">\n";

		const wchar_t *translation;

		qDebug() << sizeof(wchar_t);

		for (int i = 0; i < count; ++i)
		{
			sheet = book->getSheet(i);

			stream << "<context>\n";

			stream << "    <name>" << protect(QString((const QChar *)sheet->name())) << "</name>\n";

			//need 
			for (int j = 1; j < sheet->lastRow(); ++j)
			{
				stream << "    <message>\n";
				stream << "        <source>" << protect(QString((const QChar *)sheet->readStr(j, 1))) << "</source>\n";

				stream << "        <translation";
				if (translation = sheet->readStr(j, k + 2))
					stream << ">" << protect(QString((const QChar *)translation)) << "</translation>\n";
				else
					stream << " type=\"unfinished\"" << "></translation>\n";

				stream << "    </message>\n";
			}
			stream << "</context>\n";
		}
		stream << "</TS>\n";
	}
	qDebug() << "success";
	return true;
}