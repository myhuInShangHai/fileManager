

#include "AutoParse.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QDataStream>
#include <QtCore/QIODevice>

#include "libxl.h"

using namespace libxl;

bool getNumerusInfo(QLocale::Language language, QLocale::Country country, QByteArray *rules);

struct SItem {
	QString m_context;
	QString m_sourcetext;
	QString m_comment;
	QString m_translator;

	SItem(QString context = "", QString source = "", QString comment = "", QString translator = "")
		:m_context(context), m_sourcetext(source), m_comment(comment), m_translator(translator){}

	bool operator <(const SItem &m) const
	{
		if (m_context != m.m_context)
			return m_context < m.m_context;
		if (m_sourcetext != m.m_sourcetext)
			return m_sourcetext < m.m_sourcetext;
		return m_comment < m.m_comment;
	}

	QByteArray context() const
	{
		return m_context.toLatin1();
	}
	QByteArray sourceText() const
	{
		return m_sourcetext.toLatin1();
	}
	QString comment() const
	{
		return m_comment;
	}
	const QString& translations() const { return m_translator; }
};


// magic number for the file
static const int MagicLength = 16;
static const uchar magic[MagicLength] = {
	0x3c, 0xb8, 0x64, 0x18, 0xca, 0xef, 0x9c, 0x95,
	0xcd, 0x21, 0x1c, 0xbf, 0x60, 0xa1, 0xbd, 0xdd
};

enum { Contexts = 0x2f, Hashes = 0x42, Messages = 0x69, NumerusRules = 0x88 };

enum Tag {
	Tag_End = 1,
	Tag_SourceText16 = 2,
	Tag_Translation = 3,
	Tag_Context16 = 4,
	Tag_Obsolete1 = 5,
	Tag_SourceText = 6,
	Tag_Context = 7,
	Tag_Comment = 8,
	Tag_Obsolete2 = 9
};

enum Prefix {
	NoPrefix,
	Hash,
	HashContext,
	HashContextSourceText,
	HashContextSourceTextComment
};


static uint elfHash(const QByteArray &ba)
{
	const uchar *k = (const uchar *)ba.data();
	uint h = 0;
	uint g;

	if (k) {
		while (*k) {
			h = (h << 4) + *k++;
			if ((g = (h & 0xf0000000)) != 0)
				h ^= g >> 24;
			h &= ~g;
		}
	}
	if (!h)
		h = 1;
	return h;
}

class Releaser
{
public:
	struct Offset {
		Offset()
			: h(0), o(0)
		{}
		Offset(uint hash, uint offset)
			: h(hash), o(offset)
		{}

		bool operator<(const Offset &other) const {
			return (h != other.h) ? h < other.h : o < other.o;
		}
		bool operator==(const Offset &other) const {
			return h == other.h && o == other.o;
		}
		uint h;
		uint o;
	};

	Releaser() : m_codec(0) {}

	void setCodecName(const QByteArray &codecName)
	{
		m_codec = QTextCodec::codecForName(codecName);
	}

	bool save(QIODevice *iod);

	void squeeze();

	void setNumerusRules(const QByteArray &rules);

	void insertInternal(const QString &context, const QString &source, const QString &translator);

private:
	Q_DISABLE_COPY(Releaser)

		static Prefix commonPrefix(const SItem &m1, const SItem &m2);

	static uint msgHash(const SItem &msg);

	void writeMessage(const SItem & msg, QDataStream & stream, Prefix prefix) const;

	// for squeezed but non-file data, this is what needs to be deleted
	QByteArray m_messageArray;
	QByteArray m_offsetArray;
	QMap<SItem, void *> m_messages;
	QByteArray m_numerusRules;

	// Used to reproduce the original bytes
	QTextCodec *m_codec;
};

uint Releaser::msgHash(const SItem &msg)
{
	return elfHash(msg.sourceText());
}

Prefix Releaser::commonPrefix(const SItem &m1, const SItem &m2)
{
	if (msgHash(m1) != msgHash(m2))
		return NoPrefix;
	if (m1.context() != m2.context())
		return Hash;
	if (m1.sourceText() != m2.sourceText())
		return HashContext;
	if (m1.comment() != m2.comment())
		return HashContextSourceText;
	return HashContextSourceTextComment;
}

void Releaser::writeMessage(const SItem &msg, QDataStream &stream, Prefix prefix) const
{
	stream << quint8(Tag_Translation) << msg.translations();

	prefix = HashContextSourceTextComment;

	switch (prefix) {
	default:
	case HashContextSourceTextComment:
		stream << quint8(Tag_Comment) << msg.comment();
		// fall through
	case HashContextSourceText:
		stream << quint8(Tag_SourceText) << msg.sourceText();
		// fall through
	case HashContext:
		stream << quint8(Tag_Context) << msg.context();
		break;
	}

	//QDataStream streamtest;
	//streamtest << quint8(Tag_Context) << msg.context();

	stream << quint8(Tag_End);
}


bool Releaser::save(QIODevice *iod)
{
	QDataStream s(iod);
	s.writeRawData((const char *)magic, MagicLength);

	if (!m_offsetArray.isEmpty()) {
		quint32 oas = quint32(m_offsetArray.size());
		s << quint8(Hashes) << oas;
		s.writeRawData(m_offsetArray.constData(), oas);
	}
	if (!m_messageArray.isEmpty()) {
		quint32 mas = quint32(m_messageArray.size());
		s << quint8(Messages) << mas;

		s.writeRawData(m_messageArray.constData(), mas);
	}
	if (!m_numerusRules.isEmpty()) {
		quint32 nrs = m_numerusRules.size();
		s << quint8(NumerusRules) << nrs;
		s.writeRawData(m_numerusRules.constData(), nrs);
	}
	return true;
}

void Releaser::squeeze()
{
	if (m_messages.isEmpty())
	{
		//clear
		m_messageArray.clear();
		m_offsetArray.clear();
		return;
	}

	QMap<SItem, void *> messages = m_messages;

	// re-build contents
	m_messageArray.clear();
	m_offsetArray.clear();
	m_messages.clear();

	QMap<Offset, void *> offsets;

	QDataStream ms(&m_messageArray, QIODevice::WriteOnly);
	QMap<SItem, void *>::const_iterator it, next;
	int cpPrev = 0, cpNext = 0;
	for (it = messages.constBegin(); it != messages.constEnd(); ++it) {
		cpPrev = cpNext;
		next = it;
		++next;
		if (next == messages.constEnd())
			cpNext = 0;
		else
			cpNext = commonPrefix(it.key(), next.key());

		offsets.insert(Offset(msgHash(it.key()), ms.device()->pos()), (void *)0);
		writeMessage(it.key(), ms, Prefix(qMax(cpPrev, cpNext + 1)));
	}

	QMap<Offset, void *>::Iterator offset;
	offset = offsets.begin();
	QDataStream ds(&m_offsetArray, QIODevice::WriteOnly);
	while (offset != offsets.end()) {
		Offset k = offset.key();
		++offset;
		ds << quint32(k.h) << quint32(k.o);
	}
}

void Releaser::insertInternal(const QString &context, const QString &source, const QString &translator)
{
	SItem bmsg(context,
		source,
		QString(""),
		translator);

	m_messages.insert(bmsg, 0);
}

void Releaser::setNumerusRules(const QByteArray &rules)
{
	m_numerusRules = rules;
}

void languageAndCountry(const QString &languageCode, QLocale::Language *lang, QLocale::Country *country)
{
	QLocale locale(languageCode);
	if (lang)
		*lang = locale.language();

	if (country) {
		*country = locale.country();
	}
}


int saveQM(const QString &source, const QStringList &dest)
{

	Book	*book;
	bool	isXlsx = false;
	if (source.endsWith("xlsx"))
	{
		book = xlCreateXMLBook();
		isXlsx = true;
	}
	else
		book = xlCreateBook();


	if (!book->load((const wchar_t *)source.constData()))
	{
		return EXCEL_READ_ERROR;
	}

	int count = book->sheetCount();

	if (count == 0)
		return SHEET_COUNT_ZERO;

	Releaser releaser;
	QLocale::Language l;
	QLocale::Country c;


	for (int k = 0; k < dest.size(); ++k)
	{
		if (dest[k].isEmpty())
			continue;

		QString language = LANGUAGEABBRE.at(k);

		languageAndCountry(language, &l, &c);

		QByteArray rules;
		if (getNumerusInfo(l, c, &rules))
			releaser.setNumerusRules(rules);

		Sheet *sheet;
		const wchar_t *translation;
		const wchar_t *sourcetext;

		if (isXlsx)
		{
			for (int i = 0; i < count; ++i)
			{
				sheet = book->getSheet(i);

				//need optimize,we don't need read sheet name and key every time,next version optimize
				for (int j = 1; j < sheet->lastRow(); ++j)
				{
					if (sourcetext = sheet->readStr(j, 1))
					{
						QString source = QString::fromWCharArray(sourcetext);
						if (translation = sheet->readStr(j, k + 2))
						{
							releaser.insertInternal(QString::fromWCharArray(sheet->name()),
								source, QString::fromWCharArray(translation));
						}
					}
				}
			}
		}
		else{
			for (int i = 0; i < count; ++i)
			{
				sheet = book->getSheet(i);

				//need optimize,we don't need read sheet name and key every time,next version optimize
				for (int j = 1; j < sheet->lastRow(); ++j)
				{
					if (sourcetext = sheet->readStr(j, 1))
						if (translation = sheet->readStr(j, k + 2))
						{
							releaser.insertInternal(QString::fromWCharArray(sheet->name()),
								QString::fromWCharArray(sourcetext), QString::fromWCharArray(translation));
						}
				}
			}
		}

		releaser.squeeze();

		QFile file(dest.at(k) + ".qm");
		if (!file.open(QIODevice::ReadWrite))
			return CREATE_QM_PERMISSION_ERROR;

		releaser.save(&file);
	}
	//release resource
	book->release();
	return SUCCESS;
}

