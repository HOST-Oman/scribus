/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef PREFS_SPELLING_H
#define PREFS_SPELLING_H

#include "ui_prefs_spellingbase.h"
#include "prefs_pane.h"
#include "scribusapi.h"
#include "scribusstructs.h"

class ScribusDoc;

class SCRIBUS_API Prefs_Spelling : public Prefs_Pane, Ui::Prefs_Spelling
{


	Q_OBJECT

	public:
		Prefs_Spelling(QWidget* parent, ScribusDoc* doc=nullptr);
		~Prefs_Spelling();

		void restoreDefaults(struct ApplicationPrefs *prefsData) override;
		void saveGuiToPrefs(struct ApplicationPrefs *prefsData) const override;

	public slots:
		void languageChange();

	protected slots:
		void downloadSpellDicts();
		void updateDictList();
		void updateAvailDictList();
		void downloadDictListFinished();
		void downloadSpellDictsFinished();
		void updateProgressBar();

	protected:
		QString affixFileName(const QStringList& files);
		QString dictFileName(const QStringList& files);
		void setAvailDictsXMLFile(const QString& availDictsXMLDataFile);
		QMap<QString, QString> dictionaryMap;
		QStringList dictionaryPaths;
		QString downloadLocation;
		QList <DownloadItem> dictList;
		QList <DownloadItem> downloadList;
};

#endif // PREFS_SPELLING_H
