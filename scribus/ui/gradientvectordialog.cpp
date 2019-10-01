/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          gradientvectordialog.cpp  -  description
                             -------------------
    begin                : Tue Nov 17 2009
    copyright            : (C) 2009 by Franz Schmid
    email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gradientvectordialog.h"
#include "units.h"
#include "iconmanager.h"

GradientVectorDialog::GradientVectorDialog(QWidget* parent) : ScrPaletteBase( parent, "GradientVectorPalette", false, nullptr )
{
	m_unitRatio = 1.0;

	setupUi(this);
	gSk->setNewUnit(6);
	gSk->setValues(-89, 89, 2, 0);
	gSk_2->setNewUnit(6);
	gSk->setValues(-89, 89, 2, 0);
	gSc->setSuffix(" %");
	gSc->setValue( 100 );
	connect(gX1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gX2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gY1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gY2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gSk,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gX1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gX2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gY1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gY2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gFX,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gFY,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gSk_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gSc,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gC1X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC1Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC2X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC2Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC3X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC3Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC4X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC4Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC1XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC1YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC2XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC2YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC3XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC3YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC4XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC4YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC5XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC5YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(editPoints, SIGNAL(clicked()), this, SLOT(handleEditButton()));
	connect(editControlPoints, SIGNAL(clicked()), this, SLOT(handleEditControlButton()));
	connect(buttonNewGrid, SIGNAL(clicked()), this, SIGNAL(createNewMesh()));
	connect(buttonResetGrid, SIGNAL(clicked()), this, SIGNAL(resetMesh()));
	connect(buttonPoLIne, SIGNAL(clicked()), this, SIGNAL(meshToShape()));
	connect(resetControlPoint, SIGNAL(clicked()), this, SIGNAL(reset1Control()));
	connect(resetAllControlPoints, SIGNAL(clicked()), this, SIGNAL(resetAllControl()));
	connect(editPPoint, SIGNAL(clicked()), this, SLOT(handlePEditButton()));
	connect(editPControlPoints, SIGNAL(clicked()), this, SLOT(handlePEditControlButton()));
	connect(buttonAddPatch, SIGNAL(clicked()), this, SLOT(handlePAddButton()));
	connect(buttonRemovePatch, SIGNAL(clicked()), this, SIGNAL(removePatch()));
	connect(resetPControlPoint, SIGNAL(clicked()), this, SIGNAL(reset1Control()));
	connect(resetAllPControlPoints, SIGNAL(clicked()), this, SIGNAL(resetAllControl()));
	connect(snapToGrid, SIGNAL(clicked()), this, SLOT(handleSnapToGridBox()));
	QSize iconSize = QSize(22, 22);
	IconManager& im=IconManager::instance();
	editPoints->setIcon(im.loadIcon("MoveNode.png"));
	editPoints->setIconSize(iconSize);
	editControlPoints->setIcon(im.loadIcon("MoveKontrol.png"));
	editControlPoints->setIconSize(iconSize);
	resetControlPoint->setIcon(im.loadIcon("Reset1Node.png"));
	resetControlPoint->setIconSize(iconSize);
	resetAllControlPoints->setIcon(im.loadIcon("ResetNode.png"));
	resetAllControlPoints->setIconSize(iconSize);
	editPPoint->setIcon(im.loadIcon("MoveNode.png"));
	editPPoint->setIconSize(iconSize);
	editPControlPoints->setIcon(im.loadIcon("MoveKontrol.png"));
	editPControlPoints->setIconSize(iconSize);
	resetPControlPoint->setIcon(im.loadIcon("Reset1Node.png"));
	resetPControlPoint->setIconSize(iconSize);
	resetAllPControlPoints->setIcon(im.loadIcon("ResetNode.png"));
	resetAllPControlPoints->setIconSize(iconSize);
	languageChange();
	selectLinear();
}

void GradientVectorDialog::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange)
	{
		languageChange();
	}
	else
		QWidget::changeEvent(e);
}

void GradientVectorDialog::selectLinear()
{
	stackedWidget->setCurrentIndex(0);
	resize(minimumSizeHint());
}

void GradientVectorDialog::selectRadial()
{
	stackedWidget->setCurrentIndex(1);
	label_7->show();
	gSk_2->show();
	resize(minimumSizeHint());
}

void GradientVectorDialog::selectConical()
{
	stackedWidget->setCurrentIndex(1);
	label_7->hide();
	gSk_2->hide();
	resize(minimumSizeHint());
}

void GradientVectorDialog::selectFourColor()
{
	stackedWidget->setCurrentIndex(2);
	resize(minimumSizeHint());
}

void GradientVectorDialog::selectDiamond()
{
	stackedWidget->setCurrentIndex(3);
	resize(minimumSizeHint());
}

void GradientVectorDialog::selectMesh()
{
	stackedWidget->setCurrentIndex(4);
	editPoints->setChecked(true);
	resize(minimumSizeHint());
}

void GradientVectorDialog::selectPatchMesh()
{
	stackedWidget->setCurrentIndex(5);
	editPPoint->setChecked(true);
	snapToGrid->setChecked(false);
	snapToGrid->setEnabled(true);
	resize(minimumSizeHint());
}

void GradientVectorDialog::languageChange()
{
	retranslateUi(this);
	resize(minimumSizeHint());
}

void GradientVectorDialog::handleEditButton()
{
	if (editPoints->isChecked())
	{
		resetAllControlPoints->setEnabled(false);
		resetControlPoint->setEnabled(false);
		emit editGradient(5);
	}
}

void GradientVectorDialog::handleEditControlButton()
{
	if (editControlPoints->isChecked())
	{
		resetAllControlPoints->setEnabled(true);
		resetControlPoint->setEnabled(true);
		emit editGradient(7);
	}
}

void GradientVectorDialog::handlePEditButton()
{
	if (editPPoint->isChecked())
	{
		snapToGrid->setEnabled(true);
		resetAllPControlPoints->setEnabled(false);
		resetPControlPoint->setEnabled(false);
		emit editGradient(9);
	}
}

void GradientVectorDialog::handlePEditControlButton()
{
	if (editPControlPoints->isChecked())
	{
		snapToGrid->setEnabled(false);
		resetAllPControlPoints->setEnabled(true);
		resetPControlPoint->setEnabled(true);
		emit editGradient(10);
	}
}

void GradientVectorDialog::handleSnapToGridBox()
{
	emit snapToMGrid(snapToGrid->isChecked());
}

void GradientVectorDialog::handlePAddButton()
{
	editPPoint->setChecked(true);
	editPPoint->setEnabled(false);
	editPControlPoints->setEnabled(false);
	resetAllPControlPoints->setEnabled(false);
	resetPControlPoint->setEnabled(false);
	buttonAddPatch->setEnabled(false);
	emit editGradient(11);
}

void GradientVectorDialog::endPAddButton()
{
	editPPoint->setChecked(true);
	editPPoint->setEnabled(true);
	editPControlPoints->setEnabled(true);
	resetAllPControlPoints->setEnabled(false);
	resetPControlPoint->setEnabled(false);
	buttonAddPatch->setChecked(false);
	buttonAddPatch->setEnabled(true);
}

void GradientVectorDialog::changebuttonRemovePatch(bool val)
{
	buttonRemovePatch->setEnabled(val);
}

void GradientVectorDialog::setValues(double x1, double y1, double x2, double y2, double fx, double fy, double sg, double sk, double cx, double cy)
{
	disconnect(gX1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gX2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gY1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gY2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gSk,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gX1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gX2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gY1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gY2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gFX,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gFY,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gSk_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gSc,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gC1X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC1Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC2X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC2Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC3X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC3Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC4X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC4Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC1XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC1YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC2XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC2YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC3XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC3YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC4XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC4YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC5XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC5YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));

	gX1->setValue(x1 * m_unitRatio);
	gX2->setValue(x2 * m_unitRatio);
	gY1->setValue(y1 * m_unitRatio);
	gY2->setValue(y2 * m_unitRatio);
	gFX->setValue(fx * m_unitRatio);
	gFY->setValue(fy * m_unitRatio);
	gSk->setValue(sk);
	gSc->setValue(sg * 100.0);
	gX1_2->setValue(x1 * m_unitRatio);
	gX2_2->setValue(x2 * m_unitRatio);
	gY1_2->setValue(y1 * m_unitRatio);
	gY2_2->setValue(y2 * m_unitRatio);
	gSk_2->setValue(sk);
	gC1X->setValue(x1 * m_unitRatio);
	gC1Y->setValue(y1 * m_unitRatio);
	gC2X->setValue(x2 * m_unitRatio);
	gC2Y->setValue(y2 * m_unitRatio);
	gC3X->setValue(fx * m_unitRatio);
	gC3Y->setValue(fy * m_unitRatio);
	gC4X->setValue(sg * m_unitRatio);
	gC4Y->setValue(sk * m_unitRatio);
	gC1XD->setValue(x1 * m_unitRatio);
	gC1YD->setValue(y1 * m_unitRatio);
	gC2XD->setValue(x2 * m_unitRatio);
	gC2YD->setValue(y2 * m_unitRatio);
	gC3XD->setValue(fx * m_unitRatio);
	gC3YD->setValue(fy * m_unitRatio);
	gC4XD->setValue(sg * m_unitRatio);
	gC4YD->setValue(sk * m_unitRatio);
	gC5XD->setValue(cx * m_unitRatio);
	gC5YD->setValue(cy * m_unitRatio);

	connect(gX1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gX2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gY1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gY2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gSk,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gX1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gX2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gY1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gY2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gFX,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gFY,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gSk_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gSc,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gC1X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC1Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC2X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC2Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC3X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC3Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC4X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC4Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC1XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC1YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC2XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC2YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC3XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC3YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC4XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC4YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC5XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC5YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
}

void GradientVectorDialog::changeSpecialL()
{
	emit NewSpecial(gX1->value(), gY1->value(), gX2->value(), gY2->value(), gFX->value(), gFY->value(), 1, gSk->value(), 0, 0);
}

void GradientVectorDialog::changeSpecialR()
{
	emit NewSpecial(gX1_2->value(), gY1_2->value(), gX2_2->value(), gY2_2->value(), gFX->value(), gFY->value(), gSc->value() / 100.0, gSk_2->value(), 0, 0);
}

void GradientVectorDialog::changeSpecialF()
{
	emit NewSpecial(gC1X->value(), gC1Y->value(), gC2X->value(), gC2Y->value(), gC3X->value(), gC3Y->value(), gC4X->value(), gC4Y->value(), 0, 0);
}

void GradientVectorDialog::changeSpecialD()
{
	emit NewSpecial(gC1XD->value(), gC1YD->value(), gC2XD->value(), gC2YD->value(), gC3XD->value(), gC3YD->value(), gC4XD->value(), gC4YD->value(), gC5XD->value(), gC5YD->value());
}

void GradientVectorDialog::unitChange(int unitIndex)
{
	m_unitRatio = unitGetRatioFromIndex(unitIndex);

	disconnect(gX1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gX2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gY1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gY2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	disconnect(gX1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gX2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gY1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gY2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gFX,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gFY,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	disconnect(gC1X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC1Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC2X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC2Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC3X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC3Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC4X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC4Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	disconnect(gC1XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC1YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC2XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC2YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC3XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC3YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC4XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC4YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC5XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	disconnect(gC5YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	gX1->setNewUnit(unitIndex);
	gY1->setNewUnit(unitIndex);
	gX2->setNewUnit(unitIndex);
	gY2->setNewUnit(unitIndex);
	gFX->setNewUnit(unitIndex);
	gFY->setNewUnit(unitIndex);
	gX1_2->setNewUnit(unitIndex);
	gY1_2->setNewUnit(unitIndex);
	gX2_2->setNewUnit(unitIndex);
	gY2_2->setNewUnit(unitIndex);
	gC1X->setNewUnit(unitIndex);
	gC1Y->setNewUnit(unitIndex);
	gC2X->setNewUnit(unitIndex);
	gC2Y->setNewUnit(unitIndex);
	gC3X->setNewUnit(unitIndex);
	gC3Y->setNewUnit(unitIndex);
	gC4X->setNewUnit(unitIndex);
	gC4Y->setNewUnit(unitIndex);
	gC1XD->setNewUnit(unitIndex);
	gC1YD->setNewUnit(unitIndex);
	gC2XD->setNewUnit(unitIndex);
	gC2YD->setNewUnit(unitIndex);
	gC3XD->setNewUnit(unitIndex);
	gC3YD->setNewUnit(unitIndex);
	gC4XD->setNewUnit(unitIndex);
	gC4YD->setNewUnit(unitIndex);
	gC5XD->setNewUnit(unitIndex);
	gC5YD->setNewUnit(unitIndex);
	connect(gX1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gX2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gY1,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gY2,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialL()));
	connect(gX1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gX2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gY1_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gY2_2, SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gFX,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gFY,   SIGNAL(valueChanged(double)), this, SLOT(changeSpecialR()));
	connect(gC1X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC1Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC2X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC2Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC3X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC3Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC4X,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC4Y,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialF()));
	connect(gC1XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC1YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC2XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC2YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC3XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC3YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC4XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC4YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC5XD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
	connect(gC5YD,  SIGNAL(valueChanged(double)), this, SLOT(changeSpecialD()));
}
