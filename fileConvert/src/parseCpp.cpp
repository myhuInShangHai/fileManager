#include "AutoParse.h"

#include "QtCore/QVector"
#include "QtCore/QTextStream"
#include "QtCore/QFile"
#include "QtCore/QSet"
#include "QtCore/QString"

enum {
	Tok_Eof, Tok_class, Tok_friend, Tok_namespace, Tok_using, Tok_return,
	Tok_tr, Tok_trUtf8, Tok_translate, Tok_translateUtf8, Tok_trid,
	Tok_Q_OBJECT, Tok_Q_DECLARE_TR_FUNCTIONS,
	Tok_Ident, Tok_Comment, Tok_String, Tok_Arrow, Tok_Colon, Tok_ColonColon,
	Tok_Equals, Tok_LeftBracket, Tok_RightBracket,
	Tok_LeftBrace, Tok_RightBrace, Tok_LeftParen, Tok_RightParen, Tok_Comma, Tok_Semicolon,
	Tok_Null, Tok_Integer,
	Tok_QuotedInclude, Tok_AngledInclude,
	Tok_Other
};



class CppParser {
public:
	CppParser();

	void setInput(QTextStream &ts, const QString &fileName);

	void parse();

private:
	uint getChar();
	uint getToken();

private:
	QString yyInStr;
	const ushort *yyInPtr;

	QString yyFileName;

	QString yyWord;

	int yyCh;
	uint yyTok;


	int yyLineNo;
	int yyCurLineNo;

	bool yyAtNewline;

};

CppParser::CppParser()
{
	yyCurLineNo = 1;
	yyAtNewline = true;
}

void CppParser::setInput(QTextStream &ts, const QString &fileName)
{
	yyInStr = ts.readAll();
	yyFileName = fileName;
}

uint CppParser::getChar()
{
	const ushort *uc = yyInPtr;
	forever{
		ushort c = *uc;
		if (!c) {
			yyInPtr = uc;
			return EOF;
		}
		++uc;
		if (c == '\\') {
			ushort cc = *uc;
			if (cc == '\n') {
				++yyCurLineNo;
				++uc;
				continue;
			}
			if (cc == '\r') {
				++yyCurLineNo;
				++uc;
				if (*uc == '\n')
					++uc;
				continue;
			}
		}
		if (c == '\r') {
			if (*uc == '\n')
				++uc;
			c = '\n';
			++yyCurLineNo;
			yyAtNewline = true;
		}
		else if (c == '\n') {
			++yyCurLineNo;
			yyAtNewline = true;
		}
		else if (c != ' ' && c != '\t' && c != '#') {
			yyAtNewline = false;
		}
		yyInPtr = uc;
		return c;
	}
}

/*uint CppParser::getToken()
{

restart:
	// Failing this assertion would mean losing the preallocated buffer.
	Q_ASSERT(yyWord.isDetached());
	yyWord.resize(0);

	while (yyCh != EOF) {
		yyLineNo = yyCurLineNo;

		if (yyCh == '#' && yyAtNewline) {
			
// 			Early versions of lupdate complained about
// 			unbalanced braces in the following code:
// 
// 			#ifdef ALPHA
// 			while (beta) {
// 			#else
// 			while (gamma) {
// 			#endif
// 			delta;
// 			}
// 
// 			The code contains, indeed, two opening braces for
// 			one closing brace; yet there's no reason to panic.
// 
// 			The solution is to remember yyBraceDepth as it was
// 			when #if, #ifdef or #ifndef was met, and to set
// 			yyBraceDepth to that value when meeting #elif or
// 			#else.
			
			do {
				yyCh = getChar();
			} while (isspace(yyCh) && yyCh != '\n');

			switch (yyCh) {
			case 'd': // define
				// Skip over the name of the define to avoid it being interpreted as c++ code
				do { // Rest of "define"
					yyCh = getChar();
					if (yyCh == EOF)
						return Tok_Eof;
					if (yyCh == '\n')
						goto restart;
				} while (!isspace(yyCh));
				do { // Space between "define" and macro name
					yyCh = getChar();
					if (yyCh == EOF)
						return Tok_Eof;
					if (yyCh == '\n')
						goto restart;
				} while (isspace(yyCh));
				do { // Macro name
					if (yyCh == '(') {
						// Argument list. Follows the name without a space, and no
						// paren nesting is possible.
						do {
							yyCh = getChar();
							if (yyCh == EOF)
								return Tok_Eof;
							if (yyCh == '\n')
								goto restart;
						} while (yyCh != ')');
						break;
					}
					yyCh = getChar();
					if (yyCh == EOF)
						return Tok_Eof;
					if (yyCh == '\n')
						goto restart;
				} while (!isspace(yyCh));
				do { // Shortcut the immediate newline case if no comments follow.
					yyCh = getChar();
					if (yyCh == EOF)
						return Tok_Eof;
					if (yyCh == '\n')
						goto restart;
				} while (isspace(yyCh));

				saveState(&savedState);
				yyMinBraceDepth = yyBraceDepth;
				inDefine = true;
				goto restart;
			case 'i':
				yyCh = getChar();
				if (yyCh == 'f') {
					// if, ifdef, ifndef
					yyIfdefStack.push(IfdefState(yyBracketDepth, yyBraceDepth, yyParenDepth));
					yyCh = getChar();
				}
				else if (yyCh == 'n') {
					// include
					do {
						yyCh = getChar();
					} while (yyCh != EOF && !isspace(yyCh));
					do {
						yyCh = getChar();
					} while (isspace(yyCh));
					int tChar;
					if (yyCh == '"')
						tChar = '"';
					else if (yyCh == '<')
						tChar = '>';
					else
						break;
					ushort *ptr = (ushort *)yyWord.unicode();
					forever{
						yyCh = getChar();
						if (yyCh == EOF || yyCh == '\n')
							break;
						if (yyCh == tChar) {
							yyCh = getChar();
							break;
						}
						*ptr++ = yyCh;
					}
					yyWord.resize(ptr - (ushort *)yyWord.unicode());
					return (tChar == '"') ? Tok_QuotedInclude : Tok_AngledInclude;
				}
				break;
			case 'e':
				yyCh = getChar();
				if (yyCh == 'l') {
					// elif, else
					if (!yyIfdefStack.isEmpty()) {
						IfdefState &is = yyIfdefStack.top();
						if (is.elseLine != -1) {
							if (yyBracketDepth != is.bracketDepth1st
								|| yyBraceDepth != is.braceDepth1st
								|| yyParenDepth != is.parenDepth1st)
								yyMsg(is.elseLine)
								<< qPrintable(LU::tr("Parenthesis/bracket/brace mismatch between "
								"#if and #else branches; using #if branch\n"));
						}
						else {
							is.bracketDepth1st = yyBracketDepth;
							is.braceDepth1st = yyBraceDepth;
							is.parenDepth1st = yyParenDepth;
							saveState(&is.state);
						}
						is.elseLine = yyLineNo;
						yyBracketDepth = is.bracketDepth;
						yyBraceDepth = is.braceDepth;
						yyParenDepth = is.parenDepth;
					}
					yyCh = getChar();
				}
				else if (yyCh == 'n') {
					// endif
					if (!yyIfdefStack.isEmpty()) {
						IfdefState is = yyIfdefStack.pop();
						if (is.elseLine != -1) {
							if (yyBracketDepth != is.bracketDepth1st
								|| yyBraceDepth != is.braceDepth1st
								|| yyParenDepth != is.parenDepth1st)
								yyMsg(is.elseLine)
								<< qPrintable(LU::tr("Parenthesis/brace mismatch between "
								"#if and #else branches; using #if branch\n"));
							yyBracketDepth = is.bracketDepth1st;
							yyBraceDepth = is.braceDepth1st;
							yyParenDepth = is.parenDepth1st;
							loadState(&is.state);
						}
					}
					yyCh = getChar();
				}
				break;
			}
			// Optimization: skip over rest of preprocessor directive
			do {
				if (yyCh == '/') {
					yyCh = getChar();
					if (yyCh == '/') {
						do {
							yyCh = getChar();
						} while (yyCh != EOF && yyCh != '\n');
						break;
					}
					else if (yyCh == '*') {
						bool metAster = false;

						forever{
							yyCh = getChar();
							if (yyCh == EOF) {
								yyMsg() << qPrintable(LU::tr("Unterminated C++ comment\n"));
								break;
							}

							if (yyCh == '*') {
								metAster = true;
							}
							else if (metAster && yyCh == '/') {
								yyCh = getChar();
								break;
							}
							else {
								metAster = false;
							}
						}
					}
				}
				else {
					yyCh = getChar();
				}
			} while (yyCh != '\n' && yyCh != EOF);
			yyCh = getChar();
		}
		else if ((yyCh >= 'A' && yyCh <= 'Z') || (yyCh >= 'a' && yyCh <= 'z') || yyCh == '_') {
			ushort *ptr = (ushort *)yyWord.unicode();
			do {
				*ptr++ = yyCh;
				yyCh = getChar();
			} while ((yyCh >= 'A' && yyCh <= 'Z') || (yyCh >= 'a' && yyCh <= 'z')
				|| (yyCh >= '0' && yyCh <= '9') || yyCh == '_');
			yyWord.resize(ptr - (ushort *)yyWord.unicode());

			//qDebug() << "IDENT: " << yyWord;

			switch (yyWord.unicode()[0].unicode()) {
			case 'Q':
				if (yyWord == strQ_OBJECT)
					return Tok_Q_OBJECT;
				if (yyWord == strQ_DECLARE_TR_FUNCTIONS)
					return Tok_Q_DECLARE_TR_FUNCTIONS;
				if (yyWord == strQT_TR_NOOP)
					return Tok_tr;
				if (yyWord == strQT_TRID_NOOP)
					return Tok_trid;
				if (yyWord == strQT_TRANSLATE_NOOP)
					return Tok_translate;
				if (yyWord == strQT_TRANSLATE_NOOP3)
					return Tok_translate;
				if (yyWord == strQT_TR_NOOP_UTF8)
					return Tok_trUtf8;
				if (yyWord == strQT_TRANSLATE_NOOP_UTF8)
					return Tok_translateUtf8;
				if (yyWord == strQT_TRANSLATE_NOOP3_UTF8)
					return Tok_translateUtf8;
				break;
			case 'T':
				// TR() for when all else fails
				if (yyWord == strTR || yyWord == strTr)
					return Tok_tr;
				break;
			case 'c':
				if (yyWord == strclass)
					return Tok_class;
				break;
			case 'f':
				
// 				QTranslator::findMessage() has the same parameters as
// 				QApplication::translate().
				
				if (yyWord == strfindMessage)
					return Tok_translate;
				if (yyWord == strfriend)
					return Tok_friend;
				break;
			case 'n':
				if (yyWord == strnamespace)
					return Tok_namespace;
				break;
			case 'o':
				if (yyWord == stroperator) {
					// Operator overload declaration/definition.
					// We need to prevent those characters from confusing the followup
					// parsing. Actually using them does not add value, so just eat them.
					while (isspace(yyCh))
						yyCh = getChar();
					while (yyCh == '+' || yyCh == '-' || yyCh == '*' || yyCh == '/' || yyCh == '%'
						|| yyCh == '=' || yyCh == '<' || yyCh == '>' || yyCh == '!'
						|| yyCh == '&' || yyCh == '|' || yyCh == '~' || yyCh == '^'
						|| yyCh == '[' || yyCh == ']')
						yyCh = getChar();
				}
				break;
			case 'q':
				if (yyWord == strqtTrId)
					return Tok_trid;
				break;
			case 'r':
				if (yyWord == strreturn)
					return Tok_return;
				break;
			case 's':
				if (yyWord == strstruct)
					return Tok_class;
				break;
			case 't':
				if (yyWord == strtr)
					return Tok_tr;
				if (yyWord == strtrUtf8)
					return Tok_trUtf8;
				if (yyWord == strtranslate)
					return Tok_translate;
				break;
			case 'u':
				if (yyWord == strusing)
					return Tok_using;
				break;
			}
			return Tok_Ident;
		}
		else {
			switch (yyCh) {
			case '\n':
				if (inDefine) {
					loadState(&savedState);
					prospectiveContext.clear();
					yyBraceDepth = yyMinBraceDepth;
					yyMinBraceDepth = 0;
					inDefine = false;
				}
				yyCh = getChar();
				break;
			case '/':
				yyCh = getChar();
				if (yyCh == '/') {
					ushort *ptr = (ushort *)yyWord.unicode() + yyWord.length();
					do {
						yyCh = getChar();
						if (yyCh == EOF)
							break;
						*ptr++ = yyCh;
					} while (yyCh != '\n');
					yyWord.resize(ptr - (ushort *)yyWord.unicode());
				}
				else if (yyCh == '*') {
					bool metAster = false;
					ushort *ptr = (ushort *)yyWord.unicode() + yyWord.length();

					forever{
						yyCh = getChar();
						if (yyCh == EOF) {
							yyMsg() << qPrintable(LU::tr("Unterminated C++ comment\n"));
							break;
						}
						*ptr++ = yyCh;

						if (yyCh == '*')
							metAster = true;
						else if (metAster && yyCh == '/')
							break;
						else
							metAster = false;
					}
					yyWord.resize(ptr - (ushort *)yyWord.unicode() - 2);

					yyCh = getChar();
				}
				return Tok_Comment;
			case '"': {
				ushort *ptr = (ushort *)yyWord.unicode() + yyWord.length();
				yyCh = getChar();
				while (yyCh != EOF && yyCh != '\n' && yyCh != '"') {
					if (yyCh == '\\') {
						yyCh = getChar();
						if (yyCh == EOF || yyCh == '\n')
							break;
						*ptr++ = '\\';
					}
					*ptr++ = yyCh;
					yyCh = getChar();
				}
				yyWord.resize(ptr - (ushort *)yyWord.unicode());

				if (yyCh != '"')
					yyMsg() << qPrintable(LU::tr("Unterminated C++ string\n"));
				else
					yyCh = getChar();
				return Tok_String;
			}
			case '-':
				yyCh = getChar();
				if (yyCh == '>') {
					yyCh = getChar();
					return Tok_Arrow;
				}
				break;
			case ':':
				yyCh = getChar();
				if (yyCh == ':') {
					yyCh = getChar();
					return Tok_ColonColon;
				}
				return Tok_Colon;
				// Incomplete: '<' might be part of '<=' or of template syntax.
				// The main intent of not completely ignoring it is to break
				// parsing of things like   std::cout << QObject::tr()  as
				// context std::cout::QObject (see Task 161106)
			case '=':
				yyCh = getChar();
				return Tok_Equals;
			case '>':
			case '<':
				yyCh = getChar();
				return Tok_Other;
			case '\'':
				yyCh = getChar();
				if (yyCh == '\\')
					yyCh = getChar();

				forever{
					if (yyCh == EOF || yyCh == '\n') {
						yyMsg() << "Unterminated C++ character\n";
						break;
					}
					yyCh = getChar();
					if (yyCh == '\'') {
						yyCh = getChar();
						break;
					}
				}
				break;
			case '{':
				if (yyBraceDepth == 0)
					yyBraceLineNo = yyCurLineNo;
				yyBraceDepth++;
				yyCh = getChar();
				return Tok_LeftBrace;
			case '}':
				if (yyBraceDepth == yyMinBraceDepth) {
					if (!inDefine)
						yyMsg(yyCurLineNo)
						<< qPrintable(LU::tr("Excess closing brace in C++ code"
						" (or abuse of the C++ preprocessor)\n"));
					// Avoid things getting messed up even more
					yyCh = getChar();
					return Tok_Semicolon;
				}
				yyBraceDepth--;
				yyCh = getChar();
				return Tok_RightBrace;
			case '(':
				if (yyParenDepth == 0)
					yyParenLineNo = yyCurLineNo;
				yyParenDepth++;
				yyCh = getChar();
				return Tok_LeftParen;
			case ')':
				if (yyParenDepth == 0)
					yyMsg(yyCurLineNo)
					<< qPrintable(LU::tr("Excess closing parenthesis in C++ code"
					" (or abuse of the C++ preprocessor)\n"));
				else
					yyParenDepth--;
				yyCh = getChar();
				return Tok_RightParen;
			case '[':
				if (yyBracketDepth == 0)
					yyBracketLineNo = yyCurLineNo;
				yyBracketDepth++;
				yyCh = getChar();
				return Tok_LeftBracket;
			case ']':
				if (yyBracketDepth == 0)
					yyMsg(yyCurLineNo)
					<< qPrintable(LU::tr("Excess closing bracket in C++ code"
					" (or abuse of the C++ preprocessor)\n"));
				else
					yyBracketDepth--;
				yyCh = getChar();
				return Tok_RightBracket;
			case ',':
				yyCh = getChar();
				return Tok_Comma;
			case ';':
				yyCh = getChar();
				return Tok_Semicolon;
			case '0':
				yyCh = getChar();
				if (yyCh == 'x') {
					do {
						yyCh = getChar();
					} while ((yyCh >= '0' && yyCh <= '9')
						|| (yyCh >= 'a' && yyCh <= 'f') || (yyCh >= 'A' && yyCh <= 'F'));
					return Tok_Integer;
				}
				if (yyCh < '0' || yyCh > '9')
					return Tok_Null;
				// Fallthrough
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				do {
					yyCh = getChar();
				} while (yyCh >= '0' && yyCh <= '9');
				return Tok_Integer;
			default:
				yyCh = getChar();
				break;
			}
		}
	}
	return Tok_Eof;
}*/

void CppParser::parse()
{
	yyWord.reserve(yyInStr.size());
	yyInPtr = (const ushort *)yyInStr.unicode();

	yyCh = getChar();
	//yyTok = getToken();

}


/*void parseCpp(const QString &cppfile, QVector<SItem> &cppResult)
{
	QFile file(cppfile);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	CppParser parser;
	QTextStream ts(&file);

	//init set 
	parser.setInput(ts, cppfile);
	QSet<QString> inclusions;

	//key procedure
	//parser.parse(cd.m_defaultContext, cd, QStringList(), inclusions);
	//parser.recordResults(isHeader(filename));
}*/

