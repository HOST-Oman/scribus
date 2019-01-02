/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/**************************************************************************
*   Copyright (C) 2007 by Franz Schmid                                    *
*   franz.schmid@altmuehlnet.de                                           *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
***************************************************************************/

#include "pathdialog.h"

PathDialog::PathDialog(QWidget* parent, int unitIndex, double len, bool group) : QDialog(parent)
{
	setupUi(this);
	setModal(true);
	effectType = 0;
	gap = 0.0;
	offset = 0.0;
	offsetY = 0.0;
	rotate = 0;
	offsetYSpin->setMinimum(-len);
	offsetYSpin->setMaximum(len);
	offsetXSpin->setMinimum(-len);
	offsetXSpin->setMaximum(len);
	offsetXSpin->setNewUnit(unitIndex);
	offsetYSpin->setNewUnit(unitIndex);
	gapSpin->setNewUnit(unitIndex);
	if (group)
	{
		label_3->hide();
		gapSpin->hide();
		typeCombo->removeItem(3);
		typeCombo->removeItem(2);
		resize(minimumSizeHint());
	}
	typeCombo->setCurrentIndex(0);
	label_3->setEnabled(false);
	gapSpin->setEnabled(false);
	connect(offsetXSpin, SIGNAL(valueChanged(double)), this, SLOT(newOffset(double)));
	connect(offsetYSpin, SIGNAL(valueChanged(double)), this, SLOT(newOffsetY(double)));
	connect(gapSpin, SIGNAL(valueChanged(double)), this, SLOT(newGap(double)));
	connect(typeCombo, SIGNAL(activated(int)), this, SLOT(newType(int)));
	connect(previewCheck, SIGNAL(clicked()), this, SLOT(togglePreview()));
	connect(rotationCombo, SIGNAL(activated(int)), this, SLOT(toggleRotate(int)));
}

void PathDialog::toggleRotate(int rot)
{
	rotate = rot;
	if (previewCheck->isChecked())
		emit updateValues(effectType, offset, offsetY, gap, rotate);
}

void PathDialog::newOffsetY(double val)
{
	offsetY = val;
	if (previewCheck->isChecked())
		emit updateValues(effectType, offset, offsetY, gap, rotate);
}

void PathDialog::newOffset(double val)
{
	offset = val;
	if (previewCheck->isChecked())
		emit updateValues(effectType, offset, offsetY, gap, rotate);
}

void PathDialog::newGap(double val)
{
	gap = val;
	if (previewCheck->isChecked())
		emit updateValues(effectType, offset, offsetY, gap, rotate);
}

void PathDialog::newType(int val)
{
	effectType = val;
	if (previewCheck->isChecked())
		emit updateValues(effectType, offset, offsetY, gap, rotate);
	bool setter = effectType >= 2;
	label_3->setEnabled(setter);
	gapSpin->setEnabled(setter);
}

void PathDialog::togglePreview()
{
	int t = (previewCheck->isChecked()) ? effectType : -1;
	emit updateValues(t, offset, offsetY, gap, rotate);
}
