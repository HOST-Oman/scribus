/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          texttoolb.cpp  -  description
                             -------------------
    begin                : Sun Mar 10 2002
    copyright            : (C) 2002 by Franz Schmid
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

#include <QToolTip>
#include <QEvent>
#include <QMenu>
#include <QPixmap>
#include <QToolButton>

#include "modetoolbar.h"

#include "autoformbuttongroup.h"
#include "polyprops.h"
#include "scraction.h"
#include "scribus.h"
#include "scribusdoc.h"
#include "scrspinbox.h"

ModeToolBar::ModeToolBar(ScribusMainWindow* parent) : ScToolBar( tr("Tools"), "Tools", parent, Qt::Vertical)
{
	SubMode = 0;
	ValCount = 32;
	static double AutoShapes0[] = {0.0, 0.0, 0.0, 0.0, 100.0, 0.0, 100.0, 0.0, 100.0, 0.0, 100.0, 0.0,
									100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 0.0, 100.0, 0.0, 100.0,
									0.0, 100.0, 0.0, 100.0, 0.0, 0.0, 0.0, 0.0};
	ShapeVals = AutoShapes0;
	m_ScMW=parent;
	this->addAction(m_ScMW->scrActions["toolsSelect"]);
	this->addAction(m_ScMW->scrActions["toolsInsertTextFrame"]);
	this->addAction(m_ScMW->scrActions["toolsInsertImageFrame"]);
	this->addAction(m_ScMW->scrActions["toolsInsertRenderFrame"]);
	this->addAction(m_ScMW->scrActions["toolsInsertTable"]);
	
	this->addAction(m_ScMW->scrActions["toolsInsertShape"]);
	autoFormButtonGroup = new AutoformButtonGroup( nullptr );
	m_ScMW->scrActions["toolsInsertShape"]->setMenu(autoFormButtonGroup);
	QToolButton* tb = dynamic_cast<QToolButton*>(this->widgetForAction(m_ScMW->scrActions["toolsInsertShape"]));
	tb->setPopupMode(QToolButton::DelayedPopup);
	m_ScMW->scrActions["toolsInsertShape"]->setIcon(QIcon(autoFormButtonGroup->getIconPixmap(0,16)));

	this->addAction(m_ScMW->scrActions["toolsInsertPolygon"]);
	insertPolygonButtonMenu = new QMenu();
	idInsertPolygonButtonMenu = insertPolygonButtonMenu->addAction( "Properties...", this, SLOT(GetPolyProps()));
	m_ScMW->scrActions["toolsInsertPolygon"]->setMenu(insertPolygonButtonMenu);
	QToolButton* tb2 = dynamic_cast<QToolButton*>(this->widgetForAction(m_ScMW->scrActions["toolsInsertPolygon"]));
	tb2->setPopupMode(QToolButton::DelayedPopup);

	this->addAction(m_ScMW->scrActions["toolsInsertArc"]);
	this->addAction(m_ScMW->scrActions["toolsInsertSpiral"]);
	this->addAction(m_ScMW->scrActions["toolsInsertLine"]);
	this->addAction(m_ScMW->scrActions["toolsInsertBezier"]);
	this->addAction(m_ScMW->scrActions["toolsInsertFreehandLine"]);

	propWidget = new QWidget();
	group1Layout = new QGridLayout( propWidget );
	group1Layout->setSpacing( 3 );
	group1Layout->setMargin( 2 );
	group1Layout->setAlignment( Qt::AlignTop );
	Angle = new ScrSpinBox( -180, 180, propWidget, 6 );
	Angle->setValue( 0 );
	AngleTxt = new QLabel( tr("Angle:"), propWidget );
	group1Layout->addWidget( Angle, 0, 1 );
	group1Layout->addWidget( AngleTxt, 0 , 0 );
	PWidth = new ScrSpinBox( 0, 100, propWidget, 0 );
	PWidth->setValue( 10 );
	PWidthTxt = new QLabel( tr("Width:"), propWidget );
	group1Layout->addWidget( PWidth, 1, 1 );
	group1Layout->addWidget( PWidthTxt, 1 , 0 );
	calPop = new QMenu();
	calValAct = new QWidgetAction(this);
	calValAct->setDefaultWidget(propWidget);
	calPop->addAction(calValAct);
	this->addAction(m_ScMW->scrActions["toolsInsertCalligraphicLine"]);
	m_ScMW->scrActions["toolsInsertCalligraphicLine"]->setMenu(calPop);
	QToolButton* tb3 = dynamic_cast<QToolButton*>(this->widgetForAction(m_ScMW->scrActions["toolsInsertCalligraphicLine"]));
	tb3->setPopupMode(QToolButton::DelayedPopup);

	this->addAction(m_ScMW->scrActions["toolsRotate"]);
	this->addAction(m_ScMW->scrActions["toolsZoom"]);
	this->addAction(m_ScMW->scrActions["toolsEditContents"]);
	this->addAction(m_ScMW->scrActions["toolsEditWithStoryEditor"]);
	this->addAction(m_ScMW->scrActions["toolsLinkTextFrame"]);
	this->addAction(m_ScMW->scrActions["toolsUnlinkTextFrame"]);
	this->addAction(m_ScMW->scrActions["toolsMeasurements"]);
	this->addAction(m_ScMW->scrActions["toolsCopyProperties"]);
	this->addAction(m_ScMW->scrActions["toolsEyeDropper"]);

	languageChange();
	connect(autoFormButtonGroup, SIGNAL(FormSel(int, int, qreal *)), this, SLOT(SelShape(int, int, qreal *)));
	connect(Angle, SIGNAL(valueChanged(double)), this, SLOT(newCalValues()));
	connect(PWidth, SIGNAL(valueChanged(double)), this, SLOT(newCalValues()));
}

ModeToolBar::~ModeToolBar()
{
	delete calValAct;
	delete calPop;
}

void ModeToolBar::newCalValues()
{
	m_ScMW->doc->itemToolPrefs().calligraphicPenAngle = Angle->value();
	m_ScMW->doc->itemToolPrefs().calligraphicPenWidth = PWidth->value();
}

void ModeToolBar::GetPolyProps()
{
	PolygonProps* dia = new PolygonProps(m_ScMW, m_ScMW->doc->itemToolPrefs().polyCorners, m_ScMW->doc->itemToolPrefs().polyFactor, m_ScMW->doc->itemToolPrefs().polyUseFactor, m_ScMW->doc->itemToolPrefs().polyRotation, m_ScMW->doc->itemToolPrefs().polyCurvature, m_ScMW->doc->itemToolPrefs().polyInnerRot, m_ScMW->doc->itemToolPrefs().polyOuterCurvature);
	if (dia->exec())
	{
		dia->getValues(&m_ScMW->doc->itemToolPrefs().polyCorners, &m_ScMW->doc->itemToolPrefs().polyFactor, &m_ScMW->doc->itemToolPrefs().polyUseFactor, &m_ScMW->doc->itemToolPrefs().polyRotation, &m_ScMW->doc->itemToolPrefs().polyCurvature, &m_ScMW->doc->itemToolPrefs().polyInnerRot, &m_ScMW->doc->itemToolPrefs().polyOuterCurvature);
		m_ScMW->scrActions["toolsInsertPolygon"]->trigger();
	}
	delete dia;
}

void ModeToolBar::SelShape(int s, int c, qreal *vals)
{
	m_ScMW->scrActions["toolsInsertShape"]->setIcon(QIcon(autoFormButtonGroup->getIconPixmap(s,16)));
	SubMode = s;
	ValCount = c;
	ShapeVals = vals;
	m_ScMW->scrActions["toolsInsertShape"]->setChecked(false);
	m_ScMW->scrActions["toolsInsertShape"]->setChecked(true);
}

void ModeToolBar::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange)
	{
		languageChange();
	}
	else
		QWidget::changeEvent(e);
}

void ModeToolBar::setDoc(ScribusDoc* doc)
{
	Angle->setValue(doc->itemToolPrefs().calligraphicPenAngle);
	PWidth->setValue(doc->itemToolPrefs().calligraphicPenWidth);
}

void ModeToolBar::languageChange()
{
	AngleTxt->setText(tr("Angle:"));
	PWidthTxt->setText(tr("Width:"));
	idInsertPolygonButtonMenu->setText( tr("Properties..."));
	ScToolBar::languageChange();
}
