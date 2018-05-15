/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "autoform.h"
#include "autoformbuttongroup.h"

Autoforms::Autoforms( QWidget* parent ) : QToolButton( parent )
{
	buttonGroup1 = new AutoformButtonGroup( nullptr );
	setMenu(buttonGroup1);
	setPopupMode(QToolButton::InstantPopup);
	setIcon(QIcon(buttonGroup1->getIconPixmap(0)));
	connect(buttonGroup1, SIGNAL(buttonClicked(int)), this, SLOT(selForm(int)));
}

void Autoforms::selForm(int a)
{
	setIcon(QIcon(buttonGroup1->getIconPixmap(a)));
	int n;
	qreal *AutoShapes = buttonGroup1->getShapeData(a, &n);
	emit FormSel(a, n, AutoShapes);
}

QPixmap Autoforms::getIconPixmap(int nr)
{
	return buttonGroup1->getIconPixmap(nr);
}

