/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef PREFS_DOCUMENTSETUP_H
#define PREFS_DOCUMENTSETUP_H

#include "ui_prefs_documentsetupbase.h"
#include "prefs_pane.h"
#include "scribusapi.h"
#include "scribusstructs.h"

class ScribusDoc;

class SCRIBUS_API Prefs_DocumentSetup : public Prefs_Pane, Ui::Prefs_DocumentSetup
{
	Q_OBJECT

	public:
		Prefs_DocumentSetup(QWidget* parent, ScribusDoc* doc = nullptr);
		~Prefs_DocumentSetup() = default;

		void restoreDefaults(struct ApplicationPrefs *prefsData) override;
		void saveGuiToPrefs(struct ApplicationPrefs *prefsData) const override;

		void getResizeDocumentPages(bool &resizePages, bool &resizeMasterPages, bool &resizePageMargins, bool &resizeMasterPageMargins);
		void setupPageSizes(struct ApplicationPrefs *prefsData);

	public slots:
		void languageChange();
		void pageLayoutChanged(int);

	protected slots:
		void setPageWidth(double);
		/*!
		\author Franz Schmid
		\brief Preferences (Document / Page Size), sets Page height values
		\param v Height value
		 */
		void setPageHeight(double);
		/*!
		\author Franz Schmid
		\brief Preferences (Document / Page Size), sets Page orientation value and page dimensions
		\param ori Orientation value
		 */
		void setPageOrientation(int);
		/*!
		\author Franz Schmid
		\brief Preferences (Document / Page Size), sets Page size values. Connects signals for setting page dimensions.
		\param gr Standard page size value (eg A4)
		 */
		void setSize(const QString & gr);
		void setPageSize();
		void slotUndo(bool);
		void unitChange();
		void changeAutoDocDir();
		void emitSectionChange();

	protected:
		void setupPageSets();
		ScribusDoc* m_doc { nullptr };

		double unitRatio { 1.0 };
		double pageW { 1.0 };
		double pageH { 1.0 };
		QString prefsPageSizeName;
		QList<PageSet> pageSets;

	signals:
		void changeToOtherSection(const QString&);
		void prefsChangeUnits(int unit);
	};

#endif // PREFS_DOCUMENTSETUP_H

