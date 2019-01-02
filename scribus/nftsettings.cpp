/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
 *   Riku Leino, tsoots@gmail.com                                          *
 ***************************************************************************/

#include "nftsettings.h"
#include "prefsmanager.h"
#include "scpaths.h"

nftsettings::nftsettings(const QString& guilang)
{
	lang = guilang;
	read();
}

void nftsettings::read()
{
	nftrcreader reader(&templates, QDir::toNativeSeparators(ScPaths::applicationDataDir()));

	addTemplates(reader, ScPaths::instance().templateDir());
	addTemplates(reader, ScPaths::instance().userTemplateDir(true));
}

void nftsettings::addTemplates(nftrcreader& reader, const QString& dir) // dir will be searched for a sub folder called templates
{
	if (dir.isEmpty())
		return;
	// Add templates from the dir itself
	QString tmplFile = findTemplateXml(dir);
	QString tmplFilePath = QDir::toNativeSeparators(tmplFile);
	reader.setSourceDir(dir);
	reader.setSourceFile(tmplFile);
	if (QFile::exists(tmplFilePath))
		reader.parse(tmplFilePath);
	
	// And from all the subdirectories. template.xml file is only searched one dir level deeper than the dir
	QDir tmpldir(dir);
	if (tmpldir.exists())
	{
		tmpldir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
		QStringList dirs = tmpldir.entryList();
		for (int i = 0; i < dirs.size(); ++i)
		{
			tmplFile = findTemplateXml(dir + "/" + dirs[i]);
			tmplFilePath = QDir::toNativeSeparators(tmplFile);
			reader.setSourceDir(dir+"/"+dirs[i]);
			reader.setSourceFile(tmplFile);
			if (QFile::exists(tmplFilePath))
				reader.parse(tmplFilePath);
		}
	}
}

QString nftsettings::findTemplateXml(const QString& dir)
{
	QString tmp = dir + "/template." + lang + ".xml";
	if (QFile(tmp).exists())
		return tmp;

	if (lang.length() > 2)
	{
		tmp = dir + "/template." + lang.left(2) + ".xml";
		if (QFile(tmp).exists())
			return tmp;
	}
	return dir + "/template.xml";	
}

nftsettings::~ nftsettings()
{
	for (uint i = 0; i < templates.size(); ++i)
	{
		if (templates[i] != nullptr)
			delete templates[i];
	}
}
