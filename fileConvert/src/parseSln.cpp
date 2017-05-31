#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <QtXml/QDomDocument>


bool parseSln(const QString &filename, QStringList &sourcefile)
{
	QFile vssln(filename);
	if (!vssln.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	QString errorStr;
	int errorLine;
	int errorColumn;

	QDomDocument doc;
	if (!doc.setContent(&vssln, false, &errorStr, &errorLine,
		&errorColumn))
		return false;

	QDomElement itemgroup = doc.documentElement().firstChildElement("ItemGroup");
	QDomElement clcompile;
	QDomElement custombuild;
	QString name;

	while (!itemgroup.isNull())
	{
		clcompile = itemgroup.firstChildElement("ClCompile");

		custombuild = itemgroup.firstChildElement("CustomBuild");

		//add cpp into result list
		while (!clcompile.isNull())
		{
			name = clcompile.attribute("Include");
			if (!name.contains("GeneratedFiles"))
			{
				sourcefile << name;
			}

			clcompile = clcompile.nextSiblingElement("ClCompile");
		}

		//add .h .ui file into result list
		while (!custombuild.isNull())
		{
			sourcefile << custombuild.attribute("Include");
			custombuild = custombuild.nextSiblingElement("CustomBuild");
		}
		itemgroup = itemgroup.nextSiblingElement("ItemGroup");
	}
	return sourcefile.size();
}