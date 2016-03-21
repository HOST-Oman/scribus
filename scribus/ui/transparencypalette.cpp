/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          transparencypalette.cpp  -  description
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

#include "transparencypalette.h"
#include "sccolorengine.h"
#include "scpainter.h"
#include "scpattern.h"
#include "scribus.h"
#include "iconmanager.h"
#include "util.h"
#include "util_math.h"

Tpalette::Tpalette(QWidget* parent) : QWidget(parent)
{
	currentItem = NULL;
	patternList = NULL;
	TGradDia = NULL;
	TGradDia = new GradientVectorDialog(this->parentWidget());
	TGradDia->hide();
	setupUi(this);
	editLineSelector->setIcon(IconManager::instance()->loadIcon("16/color-stroke.png"));
	editFillSelector->setIcon(IconManager::instance()->loadIcon("16/color-fill.png"));
	editFillSelector->setChecked(true);
	strokeOpacity->setDecimals(0);
	fillOpacity->setDecimals(0);
	editFillSelectorButton();
	connect(editLineSelector, SIGNAL(clicked()), this, SLOT(editLineSelectorButton()));
	connect(editFillSelector, SIGNAL(clicked()), this, SLOT(editFillSelectorButton()));
	connect(strokeOpacity, SIGNAL(valueChanged(double)), this, SLOT(slotTransS(double)));
	connect(fillOpacity, SIGNAL(valueChanged(double)), this, SLOT(slotTransF(double)));
	connect(blendModeFill, SIGNAL(activated(int)), this, SIGNAL(NewBlend(int)));
	connect(blendModeStroke, SIGNAL(activated(int)), this, SIGNAL(NewBlendS(int)));
	connect(namedGradient, SIGNAL(activated(const QString &)), this, SLOT(setNamedGradient(const QString &)));
	connect(gradEdit, SIGNAL(gradientChanged()), this, SIGNAL(gradientChanged()));
	connect(gradEditButton, SIGNAL(clicked()), this, SLOT(editGradientVector()));
	connect(TGradDia, SIGNAL(NewSpecial(double, double, double, double, double, double, double, double, double, double)), this, SIGNAL(NewSpecial(double, double, double, double, double, double, double, double, double, double)));
	connect(TGradDia, SIGNAL(paletteShown(bool)), this, SLOT(setActiveGradDia(bool)));
	connect(gradientType, SIGNAL(activated(int)), this, SLOT(slotGradType(int)));
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotGrad(int)));
	connect(patternBox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectPattern(QListWidgetItem*)));
	connect(editPatternProps, SIGNAL(clicked()), this, SLOT(changePatternProps()));
	connect(transpCalcGradient, SIGNAL(clicked()), this, SLOT(switchGradientMode()));
	connect(transpCalcPattern, SIGNAL(clicked()), this, SLOT(switchPatternMode()));
	connect(usePatternInverted, SIGNAL(clicked()), this, SLOT(switchPatternMode()));
}

void Tpalette::connectSignals()
{
	connect(gradEdit, SIGNAL(gradientChanged()), this, SIGNAL(gradientChanged()));
	connect(namedGradient, SIGNAL(activated(const QString &)), this, SLOT(setNamedGradient(const QString &)));
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotGrad(int)));
	connect(gradientType, SIGNAL(activated(int)), this, SLOT(slotGradType(int)));
	connect(transpCalcGradient, SIGNAL(clicked()), this, SLOT(switchGradientMode()));
	connect(transpCalcPattern, SIGNAL(clicked()), this, SLOT(switchPatternMode()));
	connect(usePatternInverted, SIGNAL(clicked()), this, SLOT(switchPatternMode()));
}

void Tpalette::disconnectSignals()
{
	disconnect(gradEdit, SIGNAL(gradientChanged()), this, SIGNAL(gradientChanged()));
	disconnect(namedGradient, SIGNAL(activated(const QString &)), this, SLOT(setNamedGradient(const QString &)));
	disconnect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotGrad(int)));
	disconnect(gradientType, SIGNAL(activated(int)), this, SLOT(slotGradType(int)));
	disconnect(transpCalcGradient, SIGNAL(clicked()), this, SLOT(switchGradientMode()));
	disconnect(transpCalcPattern, SIGNAL(clicked()), this, SLOT(switchPatternMode()));
	disconnect(usePatternInverted, SIGNAL(clicked()), this, SLOT(switchPatternMode()));
}

void Tpalette::setCurrentItem(PageItem* item)
{
	currentItem = item;
	disconnectSignals();

	if (!currentItem || !currentDoc)
		return;

	setActTrans(currentItem->fillTransparency(), currentItem->lineTransparency());
	setActBlend(currentItem->fillBlendmode(), currentItem->lineBlendmode());
	gradEdit->setGradient(currentItem->mask_gradient);
	if (!currentItem->gradientMask().isEmpty())
	{
		setCurrentComboItem(namedGradient, currentItem->gradientMask());
		gradEdit->setGradientEditable(false);
	}
	else
	{
		namedGradient->setCurrentIndex(0);
		gradEdit->setGradientEditable(true);
	}
	if (currentItem->maskType() == 0)
		tabWidget->setCurrentIndex(0);
	else if ((currentItem->maskType() == 1) || (currentItem->maskType() == 2) || (currentItem->maskType() == 4) || (currentItem->maskType() == 5))
		tabWidget->setCurrentIndex(1);
	else
		tabWidget->setCurrentIndex(2);
	if (patternList->count() == 0)
		tabWidget->setTabEnabled(2, false);
	else
		tabWidget->setTabEnabled(2, true);
	transpCalcGradient->setChecked(false);
	transpCalcPattern->setChecked(false);
	usePatternInverted->setChecked(false);
	if ((currentItem->maskType() == 4) || (currentItem->maskType() == 5))
		transpCalcGradient->setChecked(true);
	if ((currentItem->maskType() == 6) || (currentItem->maskType() == 7))
		transpCalcPattern->setChecked(true);
	if ((currentItem->maskType() == 7) || (currentItem->maskType() == 8))
		usePatternInverted->setChecked(true);
	if ((currentItem->maskType() == 1) || (currentItem->maskType() == 4))
		gradientType->setCurrentIndex(0);
	else if ((currentItem->maskType() == 2) || (currentItem->maskType() == 5))
		gradientType->setCurrentIndex(1);
	if(TGradDia && gradEditButton->isChecked())
		TGradDia->setValues(currentItem->GrMaskStartX, currentItem->GrMaskStartY, currentItem->GrMaskEndX, currentItem->GrMaskEndY, currentItem->GrMaskFocalX, currentItem->GrMaskFocalY, currentItem->GrMaskScale, currentItem->GrMaskSkew, 0, 0);
	double patternScaleX, patternScaleY, patternOffsetX, patternOffsetY, patternRotation, patternSkewX, patternSkewY;
	bool mirrorX, mirrorY;
	currentItem->maskTransform(patternScaleX, patternScaleY, patternOffsetX, patternOffsetY, patternRotation, patternSkewX, patternSkewY);
	currentItem->maskFlip(mirrorX, mirrorY);
	setActPattern(currentItem->patternMask(), patternScaleX, patternScaleY, patternOffsetX, patternOffsetY, patternRotation, patternSkewX, patternSkewY, mirrorX, mirrorY);

	connectSignals();
}

void Tpalette::setDocument(ScribusDoc* doc)
{
	disconnect(this, SIGNAL(NewTrans(double)), 0, 0);
	disconnect(this, SIGNAL(NewTransS(double)), 0, 0);
	disconnect(this, SIGNAL(NewGradient(int)), 0, 0);
	disconnect(this, SIGNAL(NewBlend(int)), 0, 0);
	disconnect(this, SIGNAL(NewBlendS(int)), 0, 0);
	disconnect(this, SIGNAL(NewPattern(QString)), 0, 0);
	disconnect(this, SIGNAL(NewPatternProps(double, double, double, double, double, double, double, bool, bool)), 0, 0);
	
	if (currentDoc)
	{
		disconnect(currentDoc->scMW(), SIGNAL(UpdateRequest(int)), this, 0);
	}

	currentDoc = doc;

	if (doc != NULL)
	{
		gradEdit->setColors(doc->PageColors);
		currentUnit = doc->unitIndex();

		updateColorList();

		connect(this, SIGNAL(NewTrans(double)), doc, SLOT(itemSelection_SetItemFillTransparency(double)));
		connect(this, SIGNAL(NewTransS(double)), doc, SLOT(itemSelection_SetItemLineTransparency(double)));
		connect(this, SIGNAL(NewBlend(int)), doc, SLOT(itemSelection_SetItemFillBlend(int)));
		connect(this, SIGNAL(NewBlendS(int)), doc, SLOT(itemSelection_SetItemLineBlend(int)));
		connect(this, SIGNAL(NewGradient(int)), doc, SLOT(itemSelection_SetItemGradMask(int)));
		connect(this, SIGNAL(NewPattern(QString)), doc, SLOT(itemSelection_SetItemPatternMask(QString)));
		connect(this, SIGNAL(NewPatternProps(double, double, double, double, double, double, double, bool, bool)), doc, SLOT(itemSelection_SetItemPatternMaskProps(double, double, double, double, double, double, double, bool, bool)));
		connect(doc->scMW(), SIGNAL(UpdateRequest(int)), this, SLOT(handleUpdateRequest(int)));
	}
}

void Tpalette::handleUpdateRequest(int updateFlags)
{
	if (updateFlags & reqColorsUpdate)
		updateColorList();
}

void Tpalette::updateColorList()
{
	if (!currentDoc)
		return;

	if (currentItem)
		disconnectSignals();

	this->setColors(currentDoc->PageColors);
	this->setPatterns(&currentDoc->docPatterns);
	this->setGradients(&currentDoc->docGradients);

	if (currentItem)
		setCurrentItem(currentItem);
}

void Tpalette::hideSelectionButtons()
{
	editLineSelector->hide();
	editLineSelector->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
	editFillSelector->hide();
	editFillSelector->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
}

void Tpalette::updateFromItem()
{
	setCurrentItem(currentItem);
}

void Tpalette::updateCList()
{
	gradEdit->setColors(colorList);
}

void Tpalette::updateGradientList()
{
	disconnect(namedGradient, SIGNAL(activated(const QString &)), this, SLOT(setNamedGradient(const QString &)));
	namedGradient->clear();
	namedGradient->setIconSize(QSize(48, 12));
	namedGradient->addItem( tr("Custom"));
	QStringList patK = gradientList->keys();
	qSort(patK);
	for (int a = 0; a < patK.count(); a++)
	{
		VGradient gr = gradientList->value(patK[a]);
		QImage pixm(48, 12, QImage::Format_ARGB32_Premultiplied);
		QPainter pb;
		QBrush b(QColor(205,205,205), IconManager::instance()->loadPixmap("testfill.png"));
		pb.begin(&pixm);
		pb.fillRect(0, 0, 48, 12, b);
		pb.end();
		ScPainter *p = new ScPainter(&pixm, 48, 12);
		p->setPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
		p->setFillMode(2);
		p->fill_gradient = gr;
		p->setGradient(VGradient::linear, FPoint(0,6), FPoint(48, 6), FPoint(0,0), 1, 0);
		p->drawRect(0, 0, 48, 12);
		p->end();
		delete p;
		QPixmap pm;
		pm = QPixmap::fromImage(pixm);
		namedGradient->addItem(pm, patK[a]);
	}
	connect(namedGradient, SIGNAL(activated(const QString &)), this, SLOT(setNamedGradient(const QString &)));
}

void Tpalette::setGradients(QHash<QString, VGradient> *docGradients)
{
	gradientList = docGradients;
	updateGradientList();
}

void Tpalette::setColors(ColorList newColorList)
{
	colorList.clear();
	colorList = newColorList;
	updateCList();
}

void Tpalette::slotGrad(int number)
{
	if (number == 1)
	{
		disconnect(namedGradient, SIGNAL(activated(const QString &)), this, SLOT(setNamedGradient(const QString &)));
		if (!currentItem->gradientMask().isEmpty())
		{
			setCurrentComboItem(namedGradient, currentItem->gradientMask());
			gradEdit->setGradient(gradientList->value(currentItem->gradientMask()));
			gradEdit->setGradientEditable(false);
		}
		else
		{
			namedGradient->setCurrentIndex(0);
			gradEdit->setGradient(currentItem->mask_gradient);
			gradEdit->setGradientEditable(true);
		}
		if (gradientType->currentIndex() == 0)
		{
			if (transpCalcGradient->isChecked())
				emit NewGradient(4);
			else
				emit NewGradient(1);
		}
		else
		{
			if (transpCalcGradient->isChecked())
				emit NewGradient(5);
			else
				emit NewGradient(2);
		}
		connect(namedGradient, SIGNAL(activated(const QString &)), this, SLOT(setNamedGradient(const QString &)));
	}
	else if (number == 2)
	{
		if (transpCalcPattern->isChecked())
		{
			if (usePatternInverted->isChecked())
				emit NewGradient(7);
			else
				emit NewGradient(6);
		}
		else
		{
			if (usePatternInverted->isChecked())
				emit NewGradient(8);
			else
				emit NewGradient(3);
		}
	}
	else
		emit NewGradient(0);
}

void Tpalette::slotGradType(int type)
{
	if (type == 0)
	{
		if (transpCalcGradient->isChecked())
			emit NewGradient(4);
		else
			emit NewGradient(1);
	}
	else
	{
		if (transpCalcGradient->isChecked())
			emit NewGradient(5);
		else
			emit NewGradient(2);
	}
}

void Tpalette::setNamedGradient(const QString &name)
{
	if (namedGradient->currentIndex() == 0)
	{
		gradEdit->setGradient(currentItem->mask_gradient);
		currentItem->setGradientMask("");
		gradEdit->setGradientEditable(true);
	}
	else
	{
		gradEdit->setGradient(gradientList->value(name));
		gradEdit->setGradientEditable(false);
		currentItem->setGradientMask(name);
	}
	if (gradientType->currentIndex() == 0)
	{
		if (transpCalcGradient->isChecked())
			emit NewGradient(4);
		else
			emit NewGradient(1);
	}
	else
	{
		if (transpCalcGradient->isChecked())
			emit NewGradient(5);
		else
			emit NewGradient(2);
	}
}

void Tpalette::switchGradientMode()
{
	if (gradientType->currentIndex() == 0)
	{
		if (transpCalcGradient->isChecked())
			emit NewGradient(4);
		else
			emit NewGradient(1);
	}
	else
	{
		if (transpCalcGradient->isChecked())
			emit NewGradient(5);
		else
			emit NewGradient(2);
	}
}

void Tpalette::switchPatternMode()
{
	if (transpCalcPattern->isChecked())
	{
		if (usePatternInverted->isChecked())
			emit NewGradient(7);
		else
			emit NewGradient(6);
	}
	else
	{
		if (usePatternInverted->isChecked())
			emit NewGradient(8);
		else
			emit NewGradient(3);
	}
}

void Tpalette::editGradientVector()
{
	if (gradEditButton->isChecked())
	{
		TGradDia->unitChange(currentDoc->unitIndex());
		TGradDia->setValues(currentItem->GrMaskStartX, currentItem->GrMaskStartY, currentItem->GrMaskEndX, currentItem->GrMaskEndY, currentItem->GrMaskFocalX, currentItem->GrMaskFocalY, currentItem->GrMaskScale, currentItem->GrMaskSkew, 0, 0);
		if ((currentItem->GrMask == 1) || (currentItem->GrMask == 4))
			TGradDia->selectLinear();
		else
			TGradDia->selectRadial();
		TGradDia->show();
	}
	else
	{
		TGradDia->hide();
	}
	emit editGradient();
}

void Tpalette::setActiveGradDia(bool active)
{
	if (!active)
	{
		gradEditButton->setChecked(false);
		emit editGradient();
	}
}

void Tpalette::setSpecialGradient(double x1, double y1, double x2, double y2, double fx, double fy, double sg, double sk)
{
	if (TGradDia)
		TGradDia->setValues(x1, y1, x2, y2, fx, fy, sg, sk, 0, 0);
}

void Tpalette::hideEditedPatterns(QStringList names)
{
	for (int a = 0; a < names.count(); a++)
	{
		QList<QListWidgetItem*> items = patternBox->findItems(names[a], Qt::MatchExactly);
		if (items.count() > 0)
			items[0]->setHidden(true);
	}
}

void Tpalette::updatePatternList()
{
	disconnect(patternBox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectPattern(QListWidgetItem*)));
	patternBox->clear();
	patternBox->setIconSize(QSize(48, 48));
	QStringList patK = patternList->keys();
	qSort(patK);
	for (int a = 0; a < patK.count(); a++)
	{
		ScPattern sp = patternList->value(patK[a]);
		QPixmap pm;
		if (sp.getPattern()->width() >= sp.getPattern()->height())
			pm=QPixmap::fromImage(sp.getPattern()->scaledToWidth(48, Qt::SmoothTransformation));
		else
			pm=QPixmap::fromImage(sp.getPattern()->scaledToHeight(48, Qt::SmoothTransformation));
		QPixmap pm2(48, 48);
		pm2.fill(palette().color(QPalette::Base));
		QPainter p;
		p.begin(&pm2);
		p.drawPixmap(24 - pm.width() / 2, 24 - pm.height() / 2, pm);
		p.end();
		QListWidgetItem *item = new QListWidgetItem(pm2, patK[a], patternBox);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	}
	patternBox->clearSelection();
	connect(patternBox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectPattern(QListWidgetItem*)));
}

void Tpalette::setPatterns(QHash<QString, ScPattern> *docPatterns)
{
	patternList = docPatterns;
	updatePatternList();
}

void Tpalette::selectPattern(QListWidgetItem *c)
{
	if (c == NULL)
		return;
	emit NewPattern(c->text());
}

void Tpalette::setActPattern(QString pattern, double scaleX, double scaleY, double offsetX, double offsetY, double rotation, double skewX, double skewY, bool mirrorX, bool mirrorY)
{
	disconnect(patternBox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectPattern(QListWidgetItem*)));
	QList<QListWidgetItem*> itl = patternBox->findItems(pattern, Qt::MatchExactly);
	if (itl.count() != 0)
	{
		QListWidgetItem *it = itl[0];
		patternBox->setCurrentItem(it);
	}
	else
		patternBox->clearSelection();
	m_Pattern_scaleX = scaleX;
	m_Pattern_scaleY = scaleX;
	m_Pattern_offsetX = offsetX;
	m_Pattern_offsetY = offsetY;
	m_Pattern_rotation = rotation;
	m_Pattern_skewX = skewX;
	m_Pattern_skewY = skewY;
	m_Pattern_mirrorX = mirrorX;
	m_Pattern_mirrorY = mirrorY;
	connect(patternBox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectPattern(QListWidgetItem*)));
}

void Tpalette::changePatternProps()
{
	PatternPropsDialog *dia = new PatternPropsDialog(this, currentUnit, false);
	dia->spinXscaling->setValue(m_Pattern_scaleX);
	dia->spinYscaling->setValue(m_Pattern_scaleY);
	if (m_Pattern_scaleX == m_Pattern_scaleY)
		dia->keepScaleRatio->setChecked(true);
	dia->spinXoffset->setValue(m_Pattern_offsetX);
	dia->spinYoffset->setValue(m_Pattern_offsetY);
	dia->spinAngle->setValue(m_Pattern_rotation);
	double asina = atan(m_Pattern_skewX);
	dia->spinXSkew->setValue(asina / (M_PI / 180.0));
	double asinb = atan(m_Pattern_skewY);
	dia->spinYSkew->setValue(asinb / (M_PI / 180.0));
	dia->FlipH->setChecked(m_Pattern_mirrorX);
	dia->FlipV->setChecked(m_Pattern_mirrorY);
	connect(dia, SIGNAL(NewPatternProps(double, double, double, double, double, double, double, bool, bool)), this, SIGNAL(NewPatternProps(double, double, double, double, double, double, double, bool, bool)));
	dia->exec();
	m_Pattern_scaleX = dia->spinXscaling->value();
	m_Pattern_scaleY = dia->spinYscaling->value();
	m_Pattern_offsetX = dia->spinXoffset->value();
	m_Pattern_offsetY = dia->spinYoffset->value();
	m_Pattern_rotation = dia->spinAngle->value();
	double a    = M_PI / 180.0 * dia->spinXSkew->value();
	double b    = M_PI / 180.0 * dia->spinYSkew->value();
	double sina = tan(a);
	double sinb = tan(b);
	m_Pattern_skewX = sina;
	m_Pattern_skewY = sinb;
	m_Pattern_mirrorX = dia->FlipH->isChecked();
	m_Pattern_mirrorY = dia->FlipV->isChecked();
	delete dia;
}

void Tpalette::setActTrans(double val, double val2)
{
	disconnect(strokeOpacity, SIGNAL(valueChanged(double)), this, SLOT(slotTransS(double)));
	disconnect(fillOpacity, SIGNAL(valueChanged(double)), this, SLOT(slotTransF(double)));
	strokeOpacity->setValue(qRound(100 - (val2 * 100)));
	fillOpacity->setValue(qRound(100 - (val * 100)));
	connect(strokeOpacity, SIGNAL(valueChanged(double)), this, SLOT(slotTransS(double)));
	connect(fillOpacity, SIGNAL(valueChanged(double)), this, SLOT(slotTransF(double)));
}

void Tpalette::setActBlend(int val, int val2)
{
	disconnect(blendModeFill, SIGNAL(activated(int)), this, SIGNAL(NewBlend(int)));
	disconnect(blendModeStroke, SIGNAL(activated(int)), this, SIGNAL(NewBlendS(int)));
	blendModeFill->setCurrentIndex(val);
	blendModeStroke->setCurrentIndex(val2);
	connect(blendModeFill, SIGNAL(activated(int)), this, SIGNAL(NewBlend(int)));
	connect(blendModeStroke, SIGNAL(activated(int)), this, SIGNAL(NewBlendS(int)));
}

void Tpalette::slotTransS(double val)
{
	emit NewTransS((100 - val) / 100.0);
}

void Tpalette::slotTransF(double val)
{
	emit NewTrans((100 - val) / 100.0);
}

void Tpalette::editLineSelectorButton()
{
	if (editLineSelector->isChecked())
	{
		stackedWidget->setCurrentIndex(0);
		editFillSelector->setChecked(false);
	}
	updateFromItem();
}

void Tpalette::editFillSelectorButton()
{
	if (editFillSelector->isChecked())
	{
		stackedWidget->setCurrentIndex(1);
		editLineSelector->setChecked(false);
	}
	updateFromItem();
}

void Tpalette::unitChange(double, double, int unitIndex)
{
	if (TGradDia)
		TGradDia->unitChange(unitIndex);
	currentUnit = unitIndex;
}

