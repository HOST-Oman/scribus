/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef AUTOFORM_H
#define AUTOFORM_H

#include <QMenu>
#include <QPixmap>
#include <QToolButton>
#include <QWidgetAction>

#include "scribusapi.h"
class AutoformButtonGroup;

class SCRIBUS_API Autoforms : public QToolButton
{
    Q_OBJECT

public:
	Autoforms( QWidget* parent );
	~Autoforms() {};

	QPixmap getIconPixmap(int nr);

public slots:
	void selForm(int a);
	void iconSetChange();

signals:
	void FormSel(int, int, qreal *);

protected:
	AutoformButtonGroup* buttonGroup1 { nullptr };
};

#endif

