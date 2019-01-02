/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
 *   Riku Leino, tsoots@gmail.com                                          *
 ***************************************************************************/
#ifndef SATDIALOG_H
#define SATDIALOG_H

#include "ui_satdialog.h"
#include <QMap>
#include <prefscontext.h>

class SATDialog : public QDialog, public Ui::SATDialogBase
{
	Q_OBJECT

public:
	SATDialog(QWidget* parent, const QString& tmplName = "", int pageW = 0, int pageH = 0);
	~SATDialog();

	QMap<QString, QString> cats;
private slots:
	void detailClicked(int);

private:
	PrefsContext* prefs;
	QString author;
	QString email;
	bool isFullDetail;

	QString findTemplateXml(const QString& dir);
	void    readPrefs();
	void    writePrefs();
	void    addCategories(const QString& dir);
	void    readCategories(const QString& fileName);
	void    setupCategories();
	void    setupPageSize(int w, int h);
};

#endif

