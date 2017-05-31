#include "AutoParse.h"


#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QString>

#include <QtXml/QXmlAttributes>
#include <QtXml/QXmlDefaultHandler>
#include <QtXml/QXmlLocator>
#include <QtXml/QXmlParseException>


class UiReader : public QXmlDefaultHandler
{
public:
	UiReader(QMap<QString, QString> &result)
		:m_lineNumber(-1), m_isTrString(false),
		m_needUtf8(true),
		mResult(result)
	{}

	bool startElement(const QString &namespaceURI, const QString &localName,
		const QString &qName, const QXmlAttributes &atts);
	bool endElement(const QString &namespaceURI, const QString &localName,
		const QString &qName);
	bool characters(const QString &ch);
	bool fatalError(const QXmlParseException &exception);

	void setDocumentLocator(QXmlLocator *locator) { m_locator = locator; }

private:
	void flush();

	QString m_context;
	QString m_source;
	QString m_comment;
	QString m_extracomment;
	QXmlLocator *m_locator;

	QMap<QString, QString> &mResult;
	QString m_accum;
	int m_lineNumber;
	bool m_isTrString;
	bool m_needUtf8;
};

bool UiReader::startElement(const QString &namespaceURI,
	const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
	Q_UNUSED(namespaceURI);
	Q_UNUSED(localName);

	if (qName == QLatin1String("item")) { // UI3 menu entries
		flush();
		if (!atts.value(QLatin1String("text")).isEmpty()) {
			m_source = atts.value(QLatin1String("text"));
			m_isTrString = true;

			m_lineNumber = m_locator->lineNumber();
		}
	}
	else if (qName == QLatin1String("string")) {
		flush();
		if (atts.value(QLatin1String("notr")).isEmpty() ||
			atts.value(QLatin1String("notr")) != QLatin1String("true")) {
			m_isTrString = true;
			m_comment = atts.value(QLatin1String("comment"));
			m_extracomment = atts.value(QLatin1String("extracomment"));

			m_lineNumber = m_locator->lineNumber();
		}
		else {
			m_isTrString = false;
		}
	}
	m_accum.clear();
	return true;
}

bool UiReader::endElement(const QString &namespaceURI,
	const QString &localName, const QString &qName)
{
	Q_UNUSED(namespaceURI);
	Q_UNUSED(localName);

	m_accum.replace(QLatin1String("\r\n"), QLatin1String("\n"));

	if (qName == QLatin1String("class")) { // UI "header"
		if (m_context.isEmpty())
			m_context = m_accum;
	}
	else if (qName == QLatin1String("string") && m_isTrString) {
		m_source = m_accum;
	}
	else if (qName == QLatin1String("comment")) { // FIXME: what's that?
		m_comment = m_accum;
		flush();
	}
	else if (qName == QLatin1String("function")) { // UI3 embedded code
		//fetchtrInlinedCpp(m_accum, m_translator, m_context);
		qDebug() << "temporary not used";
	}
	else {
		flush();
	}
	return true;
}

bool UiReader::characters(const QString &ch)
{
	m_accum += ch;
	return true;
}

bool UiReader::fatalError(const QXmlParseException &exception)
{
	QString msg = QString("XML error: Parse error at line %1, column %2 (%3).")
		.arg(exception.lineNumber()).arg(exception.columnNumber())
		.arg(exception.message());
	return false;
}

void UiReader::flush()
{
	if (!m_context.isEmpty() && !m_source.isEmpty()) {
		mResult.insert(m_source, m_context);
	}

	m_source.clear();
	m_comment.clear();
	m_extracomment.clear();
}

bool loadUI(const QString &filename, QMap<QString, QString> &dataset)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	QXmlInputSource in(&file);
	QXmlSimpleReader reader;

	UiReader handler(dataset);

	reader.setContentHandler(&handler);
	reader.setErrorHandler(&handler);
	bool result = reader.parse(in);
	if (!result)
		qDebug() << "Parse error in UI file";

	reader.setContentHandler(0);
	reader.setErrorHandler(0);
	return result;
}