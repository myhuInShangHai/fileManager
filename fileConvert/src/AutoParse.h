#ifndef AUTOPARSE_H
#define AUTOPARSE_H

#include <QtCore/QString>
#include <QtCore/QMap>

#include <QtCore/QStringList>

enum QMFailure {
	EXCEL_READ_ERROR,
	SHEET_COUNT_ZERO,
	CREATE_QM_PERMISSION_ERROR,
	SUCCESS,
};

const QStringList LANGUAGEABBRE = (QStringList() << "zh_cn" << "en" << "de" << "ja" << "ko");


bool parseSln(const QString &filename, QStringList &sourcefile);
bool loadUI(const QString &filename, QMap<QString, QString> &result);
bool writeResultToExcel(const QString &filename, const QMap<QString, QString> &result);


bool parseExcelTOTs(const QString &source, const QStringList &dest);
int saveQM(const QString &source, const QStringList &dest);


#endif