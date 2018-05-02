/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef PREFS_OPERATORTOOLS_H
#define PREFS_OPERATORTOOLS_H

#include "ui_prefs_operatortoolsbase.h"
#include "prefs_pane.h"
#include "scribusapi.h"

class ScribusDoc;

class SCRIBUS_API Prefs_OperatorTools : public Prefs_Pane, Ui::Prefs_OperatorTools
{
	Q_OBJECT

	public:
		Prefs_OperatorTools(QWidget* parent, ScribusDoc* doc=nullptr);
		~Prefs_OperatorTools();
		virtual void restoreDefaults(struct ApplicationPrefs *prefsData);
		virtual void saveGuiToPrefs(struct ApplicationPrefs *prefsData) const;

	public slots:
		void languageChange();
		void unitChange(int);
};

#endif // PREFS_OPERATORTOOLS_H
