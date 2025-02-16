/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QDebug>
#include <QDomDocument>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QTableWidgetItem>
#include <QTextStream>


#include "downloadmanager/scdlmgr.h"
#include "langmgr.h"
#include "prefs_spelling.h"
#include "prefsstructs.h"
#include "scpaths.h"
#include "scribusapp.h"
#include "scribusdoc.h"

Prefs_Spelling::Prefs_Spelling(QWidget* parent, ScribusDoc* /*doc*/)
	: Prefs_Pane(parent)
{
	setupUi(this);
	languageChange();

	m_caption = tr("Spelling");
	m_icon = "pref-hyphenator";

	updateDictList();
	downloadLocation = ScPaths::downloadDir();
	spellDownloadButton->setEnabled(false);
	setAvailDictsXMLFile(downloadLocation + "scribus_spell_dicts.xml");
	downloadProgressBar->setVisible(false);
	dlLabel->setVisible(false);
	connect(spellDownloadButton, SIGNAL(clicked()), this, SLOT(downloadSpellDicts()));
	connect(availListDownloadButton, SIGNAL(clicked()), this, SLOT(updateAvailDictList()));
}

Prefs_Spelling::~Prefs_Spelling() = default;

void Prefs_Spelling::languageChange()
{
	// No need to do anything here, the UI language cannot change while prefs dialog is opened
}

void Prefs_Spelling::restoreDefaults(struct ApplicationPrefs *prefsData)
{
}

void Prefs_Spelling::saveGuiToPrefs(struct ApplicationPrefs *prefsData) const
{
}

void Prefs_Spelling::downloadSpellDicts()
{
	int rows = availDictTableWidget->rowCount();
	QStringList dlLangs;
	for (int i = 0; i < rows; ++i)
	{
		QTableWidgetItem *dlItem = availDictTableWidget->item(i, 3);
		if (dlItem->checkState() == Qt::Checked)
			dlLangs << availDictTableWidget->item(i, 1)->text();
	}
	if (dlLangs.isEmpty())
		return;
	spellDownloadButton->setEnabled(false);
	downloadList.clear();
	downloadProgressBar->setValue(0);
	downloadProgressBar->setVisible(true);
	dlLabel->setVisible(true);
	int i = 0;
	QString userDictDir(ScPaths::userDictDir(ScPaths::Spell, true));
	for (const DownloadItem& d : dictList)
	{
		if (dlLangs.contains(d.lang))
		{
			if (d.filetype == "zip")
			{
				ScQApp->dlManager()->addURL(d.url, true, downloadLocation, userDictDir);
				++i;
			}
			if (d.filetype == "plain")
			{
				//qDebug()<<d.url<<d.files;
				const QStringList plainURLs(d.files.split(";", Qt::SkipEmptyParts));
				for (const QString& s : plainURLs)
				{
					ScQApp->dlManager()->addURL(d.url + "/" + s, true, downloadLocation, userDictDir);
					++i;
				}
				downloadList.append(d);
			}
		}
	}
	if (i > 0)
	{
		downloadProgressBar->setRange(0, i);
		connect(ScQApp->dlManager(), SIGNAL(finished()), this, SLOT(downloadSpellDictsFinished()));
		connect(ScQApp->dlManager(), SIGNAL(fileReceived(QString)), this, SLOT(updateProgressBar()));
		connect(ScQApp->dlManager(), SIGNAL(fileFailed(QString)), this, SLOT(updateProgressBar()));
		ScQApp->dlManager()->startDownloads();
	}
}

void Prefs_Spelling::updateDictList()
{
	bool dictsFound = LanguageManager::instance()->findSpellingDictionaries(dictionaryPaths);
	if (!dictsFound)
		return;
	dictionaryMap.clear();
	LanguageManager::instance()->findSpellingDictionarySets(dictionaryPaths, dictionaryMap);

	dictTableWidget->clear();
	dictTableWidget->setRowCount(dictionaryMap.count());
	dictTableWidget->setColumnCount(3);
	QMapIterator<QString, QString> i(dictionaryMap);
	int row=0;
	while (i.hasNext())
	{
		 i.next();
		 int column = 0;
		 //qDebug()<<i.key()<<i.value()<<LanguageManager::instance()->getLangFromAbbrev(i.key(), false);
		 QTableWidgetItem *newItem1 = new QTableWidgetItem(LanguageManager::instance()->getLangFromAbbrev(i.key()));
		 newItem1->setFlags(newItem1->flags() & ~Qt::ItemIsEditable);
		 dictTableWidget->setItem(row, column++, newItem1);
		 QTableWidgetItem *newItem2 = new QTableWidgetItem(i.key());
		 newItem2->setFlags(newItem1->flags());
		 dictTableWidget->setItem(row, column++, newItem2);
		 QTableWidgetItem *newItem3 = new QTableWidgetItem(i.value());
		 newItem3->setFlags(newItem1->flags());
		 newItem3->setToolTip(i.value());
		 dictTableWidget->setItem(row, column++, newItem3);
		 ++row;
	}
	QStringList headers;
	headers << tr("Language") << tr("Code") << tr("Location");
	dictTableWidget->setHorizontalHeaderLabels(headers);
	dictTableWidget->resizeColumnsToContents();
}

void Prefs_Spelling::updateAvailDictList()
{
	availListDownloadButton->setEnabled(false);
	ScQApp->dlManager()->addURL("http://services.scribus.net/scribus_spell_dicts.xml", true, downloadLocation, downloadLocation);
	connect(ScQApp->dlManager(), SIGNAL(finished()), this, SLOT(downloadDictListFinished()));
	ScQApp->dlManager()->startDownloads();
}

void Prefs_Spelling::downloadDictListFinished()
{
	disconnect(ScQApp->dlManager(), SIGNAL(finished()), this, SLOT(downloadDictListFinished()));
	setAvailDictsXMLFile(downloadLocation + "scribus_spell_dicts.xml");
	availListDownloadButton->setEnabled(true);
}

void Prefs_Spelling::downloadSpellDictsFinished()
{
	disconnect(ScQApp->dlManager(), SIGNAL(finished()), this, SLOT(downloadDictListFinished()));

	updateDictList();
	downloadProgressBar->setValue(0);
	downloadProgressBar->setVisible(false);
	dlLabel->setVisible(false);
	spellDownloadButton->setEnabled(true);
}

void Prefs_Spelling::updateProgressBar()
{
	downloadProgressBar->setValue(downloadProgressBar->value()+1);
}

void Prefs_Spelling::setAvailDictsXMLFile(const QString& availDictsXMLDataFile)
{
	QFile dataFile(availDictsXMLDataFile);
	if (!dataFile.exists())
		return;
	if (!dataFile.open(QIODevice::ReadOnly))
		return;

	QTextStream ts(&dataFile);
	ts.setEncoding(QStringConverter::Utf8);

	QDomDocument doc( "scribus_spell_dicts" );
	QString data(ts.readAll());
	dataFile.close();

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
	QDomDocument::ParseResult parseResult = doc.setContent(data);
	if (!parseResult)
	{
		if (data.contains("404 not found", Qt::CaseInsensitive))
			qDebug() << "File not found on server";
		else
			qDebug() << "Could not open file" << availDictsXMLDataFile;
		return;
	}
#else
	QString errorMsg;
	int eline;
	int ecol;
	if (!doc.setContent(data, &errorMsg, &eline, &ecol))
	{
		if (data.contains("404 not found", Qt::CaseInsensitive))
			qDebug()<<"File not found on server";
		else
			qDebug()<<"Could not open file"<<availDictsXMLDataFile;
		return;
	}
#endif

	dictList.clear();
	QDomElement docElem = doc.documentElement();
	for (QDomNode n = docElem.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement e = n.toElement();
		if (e.isNull())
			continue;

		if (e.tagName() != "dictionary")
			continue;

		if (e.hasAttribute("type") && e.hasAttribute("filetype"))
		{
			if (e.attribute("type") == "spell")
			{
				struct DownloadItem d;
				d.desc = e.attribute("description");
				d.files = e.attribute("files");
				d.url = e.attribute("URL");
				d.version = e.attribute("version");
				d.lang = e.attribute("language");
				d.license = e.attribute("license");
				d.filetype = e.attribute("filetype");
				QUrl url(d.url);
				if (url.isValid() && !url.isEmpty() && !url.host().isEmpty())
					dictList.append(d);
				//else
				//	qDebug()<<"hysettings : availDicts : invalid URL"<<d.url;
			}
		}
	}
	availDictTableWidget->clear();
	if(dictList.isEmpty())
	{
		spellDownloadButton->setEnabled(false);
		return;
	}
	availDictTableWidget->setRowCount(dictList.count());
	availDictTableWidget->setColumnCount(4);
	int row = 0;
	for (const DownloadItem& d : dictList)
	{
		int column = 0;
		//qDebug()<<d.version<<d.files<<d.url<<d.desc<<d.license;
		QTableWidgetItem *newItem1 = new QTableWidgetItem(d.desc);
		newItem1->setFlags(newItem1->flags() & ~Qt::ItemIsEditable);
		availDictTableWidget->setItem(row, column++, newItem1);
		QTableWidgetItem *newItem2 = new QTableWidgetItem(d.lang);
		newItem2->setFlags(newItem1->flags());
		availDictTableWidget->setItem(row, column++, newItem2);
		QTableWidgetItem *newItem3 = new QTableWidgetItem();
		newItem3->setCheckState(dictionaryMap.contains(d.lang) ? Qt::Checked : Qt::Unchecked);
		newItem3->setFlags(newItem1->flags() & ~Qt::ItemIsUserCheckable);
		availDictTableWidget->setItem(row, column++, newItem3);
		QTableWidgetItem *newItem4 = new QTableWidgetItem();
		newItem4->setCheckState(d.download ? Qt::Checked : Qt::Unchecked);
		availDictTableWidget->setItem(row, column++, newItem4);
		++row;
	}
	QStringList headers;
	headers << tr("Language") << tr("Code") << tr("Installed") << tr("Download");
	availDictTableWidget->setHorizontalHeaderLabels(headers);
	availDictTableWidget->resizeColumnsToContents();
	spellDownloadButton->setEnabled(true);
}

QString Prefs_Spelling::affixFileName(const QStringList& files) const
{
	for (int i = 0; i < files.count(); ++i)
	{
		const QString& file = files.at(i);
		if (file.endsWith(".aff", Qt::CaseInsensitive))
			return file;
	}
	return QString();
}

QString Prefs_Spelling::dictFileName(const QStringList& files) const
{
	for (int i = 0; i < files.count(); ++i)
	{
		const QString& file = files.at(i);
		if (file.endsWith(".dic", Qt::CaseInsensitive))
			return file;
	}
	return QString();
}
