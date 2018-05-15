/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "propertiespalette_shadow.h"

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include "commonstrings.h"
#include "sccolorengine.h"
#include "pageitem.h"
#include "propertiespalette_utils.h"

#include "scribuscore.h"
#include "scraction.h"

#include "selection.h"
#include "units.h"
#include "undomanager.h"
#include "util.h"
#include "util_math.h"

PropertiesPalette_Shadow::PropertiesPalette_Shadow( QWidget* parent) : PropTreeWidget(parent)
{
	m_ScMW = 0;
	m_doc = 0;
	m_item = 0;
	m_haveDoc  = false;
	m_haveItem = false;
	m_unitIndex = 0;
	m_unitRatio = 1.0;
	hasSoftShadow = new PropTreeItem(this, PropTreeItem::CheckBox, tr( "Has Drop Shadow"));
	hasSoftShadow->setBoolValue(false);

	softShadowXOffset = new PropTreeItem(this, PropTreeItem::DoubleSpinBox, tr( "X-Offset:"));
	softShadowXOffset->setUnitValue(0);
	softShadowXOffset->setDecimalsValue(2);
	softShadowXOffset->setMinMaxValues(-200.0, 200.0);
	softShadowXOffset->setDoubleValue(5.0);

	softShadowYOffset = new PropTreeItem(this, PropTreeItem::DoubleSpinBox, tr( "Y-Offset:"));
	softShadowYOffset->setUnitValue(0);
	softShadowYOffset->setDecimalsValue(2);
	softShadowYOffset->setMinMaxValues(-200.0, 200.0);
	softShadowYOffset->setDoubleValue(5.0);

	softShadowBlurRadius = new PropTreeItem(this, PropTreeItem::DoubleSpinBox, tr( "Blur:"));
	softShadowBlurRadius->setUnitValue(0);
	softShadowBlurRadius->setDecimalsValue(1);
	softShadowBlurRadius->setMinMaxValues(-20.0, 20.0);
	softShadowBlurRadius->setDoubleValue(2.0);

	softShadowColor = new PropTreeItem(this, PropTreeItem::ColorComboBox, tr( "Color:"));
	softShadowColor->setStringValue( tr( "Black"));

	softShadowShade = new PropTreeItem(this, PropTreeItem::IntSpinBox, tr( "Shade:"));
	softShadowShade->setUnitValue(7);
	softShadowShade->setDecimalsValue(0);
	softShadowShade->setMinMaxValues(0, 100);
	softShadowShade->setIntValue(100);

	softShadowOpacity = new PropTreeItem(this, PropTreeItem::DoubleSpinBox, tr( "Opacity:"));
	softShadowOpacity->setUnitValue(7);
	softShadowOpacity->setDecimalsValue(1);
	softShadowOpacity->setMinMaxValues(0.0, 100.0);
	softShadowOpacity->setDoubleValue(100.0);

	softShadowBlendMode = new PropTreeItem(this, PropTreeItem::ComboBox, tr( "Blendmode:"));
	QStringList modes;
	softShadowBlendMode->setComboStrings(modes);
	softShadowBlendMode->setStringValue( tr( "Normal"));

	softShadowErase = new PropTreeItem(this, PropTreeItem::CheckBox, tr( "Content covers\nDrop Shadow"));
	softShadowErase->setBoolValue(false);

	softShadowObjTrans = new PropTreeItem(this, PropTreeItem::CheckBox, tr( "Inherit Object\nTransparency"));
	softShadowObjTrans->setBoolValue(false);

	languageChange();
	m_haveItem = false;

	setSizePolicy( QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
	connect(this->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(handleNewValues()));

	m_haveItem = false;
}

void PropertiesPalette_Shadow::setMainWindow(ScribusMainWindow* mw)
{
	m_ScMW = mw;
	connect(m_ScMW, SIGNAL(UpdateRequest(int)), this, SLOT(handleUpdateRequest(int)));
}

void PropertiesPalette_Shadow::setDoc(ScribusDoc *d)
{
	if((d == (ScribusDoc*) m_doc) || (m_ScMW && m_ScMW->scriptIsRunning()))
		return;

	if (m_doc)
	{
		disconnect(m_doc->m_Selection, SIGNAL(selectionChanged()), this, SLOT(handleSelectionChanged()));
		disconnect(m_doc             , SIGNAL(docChanged())      , this, SLOT(handleSelectionChanged()));
	}

	m_doc  = d;
	m_item = nullptr;
	m_unitRatio   = m_doc->unitRatio();
	m_unitIndex   = m_doc->unitIndex();
	int precision = unitGetPrecisionFromIndex(m_unitIndex);
	double maxXYWHVal =  200 * m_unitRatio;
	double minXYVal   = -200 * m_unitRatio;

	m_haveDoc = true;
	m_haveItem = false;
	softShadowXOffset->setUnitValue(m_unitIndex);
	softShadowXOffset->setDecimalsValue(precision);
	softShadowXOffset->setMinMaxValues(minXYVal, maxXYWHVal);
	softShadowXOffset->setDoubleValue(minXYVal);

	softShadowYOffset->setUnitValue(m_unitIndex);
	softShadowYOffset->setDecimalsValue(precision);
	softShadowYOffset->setMinMaxValues(minXYVal, maxXYWHVal);
	softShadowYOffset->setDoubleValue(minXYVal);

	softShadowBlurRadius->setUnitValue(m_unitIndex);
	softShadowBlurRadius->setDecimalsValue(1);
	softShadowBlurRadius->setMinMaxValues(0.0, 20.0);
	softShadowBlurRadius->setDoubleValue(5);

	softShadowShade->setDecimalsValue(0);
	softShadowShade->setMinMaxValues(0, 100);
	softShadowShade->setIntValue(100);

	softShadowOpacity->setDecimalsValue(0);
	softShadowOpacity->setMinMaxValues(0, 100);
	softShadowOpacity->setIntValue(100);
	updateColorList();

	connect(m_doc->m_Selection, SIGNAL(selectionChanged()), this, SLOT(handleSelectionChanged()));
	connect(m_doc             , SIGNAL(docChanged())      , this, SLOT(handleSelectionChanged()));
}

void PropertiesPalette_Shadow::unsetDoc()
{
	if (m_doc)
	{
		disconnect(m_doc->m_Selection, SIGNAL(selectionChanged()), this, SLOT(handleSelectionChanged()));
		disconnect(m_doc             , SIGNAL(docChanged())      , this, SLOT(handleSelectionChanged()));
	}
	m_haveDoc  = false;
	m_haveItem = false;
	m_doc   = nullptr;
	m_item  = nullptr;
	setEnabled(false);
}

void PropertiesPalette_Shadow::unsetItem()
{
	m_haveItem = false;
	m_item     = nullptr;
	handleSelectionChanged();
}

void PropertiesPalette_Shadow::handleUpdateRequest(int updateFlags)
{
	if (updateFlags & reqColorsUpdate)
		updateColorList();
}

PageItem* PropertiesPalette_Shadow::currentItemFromSelection()
{
	PageItem *currentItem = nullptr;
	if (m_doc)
	{
		if (m_doc->m_Selection->count() > 1)
			currentItem = m_doc->m_Selection->itemAt(0);
		else if (m_doc->m_Selection->count() == 1)
			currentItem = m_doc->m_Selection->itemAt(0);
	}
	return currentItem;
}

void PropertiesPalette_Shadow::setCurrentItem(PageItem *i)
{
	if (!m_ScMW || m_ScMW->scriptIsRunning())
		return;
	if (!m_doc)
		setDoc(i->doc());
	m_haveItem = false;
	m_item = i;
	hasSoftShadow->setBoolValue(i->hasSoftShadow());
	softShadowXOffset->setDoubleValue(i->softShadowXOffset() * m_unitRatio);
	softShadowYOffset->setDoubleValue(i->softShadowYOffset() * m_unitRatio);
	softShadowBlurRadius->setDoubleValue(i->softShadowBlurRadius() * m_unitRatio);
	softShadowColor->setStringValue(i->softShadowColor());
	softShadowShade->setIntValue(i->softShadowShade());
	softShadowOpacity->setDoubleValue(qRound(100 - (i->softShadowOpacity() * 100)));
	softShadowBlendMode->setIntValue(i->softShadowBlendMode());
	softShadowErase->setBoolValue(i->softShadowErasedByObject());
	softShadowObjTrans->setBoolValue(i->softShadowHasObjectTransparency());
	m_haveItem = true;
	updateSpinBoxConstants();
}

void PropertiesPalette_Shadow::handleSelectionChanged()
{
	if (!m_haveDoc || !m_ScMW || m_ScMW->scriptIsRunning())
		return;

	PageItem* currItem = currentItemFromSelection();
	if (currItem)
		setCurrentItem(currItem);
	updateGeometry();
}

void PropertiesPalette_Shadow::unitChange()
{
	if (!m_haveDoc)
		return;
	m_unitRatio = m_doc->unitRatio();
	m_unitIndex = m_doc->unitIndex();

	bool sigBlocked1 = softShadowXOffset->blockSignals(true);
	bool sigBlocked2 = softShadowYOffset->blockSignals(true);
	bool sigBlocked3 = softShadowBlurRadius->blockSignals(true);
	bool sigBlocked4 = this->model()->blockSignals(true);

	softShadowXOffset->setUnitValue(m_unitIndex);
	softShadowYOffset->setUnitValue(m_unitIndex);
	softShadowBlurRadius->setUnitValue(m_unitIndex);

	softShadowXOffset->blockSignals(sigBlocked1);
	softShadowYOffset->blockSignals(sigBlocked2);
	softShadowBlurRadius->blockSignals(sigBlocked3);
	this->model()->blockSignals(sigBlocked4);
}

void PropertiesPalette_Shadow::updateColorList()
{
	if (!m_haveDoc || !m_ScMW || m_ScMW->scriptIsRunning())
		return;
	softShadowColor->setColorList(m_doc->PageColors);
}

void PropertiesPalette_Shadow::handleNewValues()
{
	if (m_haveItem)
	{
		double x = softShadowXOffset->valueAsDouble() / m_unitRatio;
		double y = softShadowYOffset->valueAsDouble() / m_unitRatio;
		double r = softShadowBlurRadius->valueAsDouble() / m_unitRatio;
		QString color = softShadowColor->valueAsString();
		if (color == CommonStrings::tr_NoneColor)
			color = CommonStrings::None;
		int b = softShadowBlendMode->valueAsInt();
		double o = (100 - softShadowOpacity->valueAsDouble()) / 100.0;
		int s = softShadowShade->valueAsInt();
		if (m_haveDoc)
		{
			m_doc->itemSelection_SetSoftShadow(hasSoftShadow->valueAsBool(), color, x, y, r, s, o, b, softShadowErase->valueAsBool(), softShadowObjTrans->valueAsBool());
		}
	}
}

void PropertiesPalette_Shadow::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange)
	{
		languageChange();
	}
	else
		QWidget::changeEvent(e);
}

void PropertiesPalette_Shadow::languageChange()
{
	disconnect(this->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(handleNewValues()));

	hasSoftShadow->setText(0, tr( "Has Drop Shadow"));
	softShadowXOffset->setText(0, tr( "X-Offset:"));
	softShadowYOffset->setText(0, tr( "Y-Offset:"));
	softShadowBlurRadius->setText(0, tr( "Blur:"));
	softShadowColor->setText(0, tr( "Color:"));
	softShadowShade->setText(0, tr( "Shade:"));
	softShadowOpacity->setText(0, tr( "Opacity:"));
	softShadowErase->setText(0, tr( "Content covers\nDrop Shadow"));
	softShadowObjTrans->setText(0, tr( "Inherit Object\nTransparency"));
	QStringList modes;
	modes.append( tr("Normal"));
	modes.append( tr("Darken"));
	modes.append( tr("Lighten"));
	modes.append( tr("Multiply"));
	modes.append( tr("Screen"));
	modes.append( tr("Overlay"));
	modes.append( tr("Hard Light"));
	modes.append( tr("Soft Light"));
	modes.append( tr("Difference"));
	modes.append( tr("Exclusion"));
	modes.append( tr("Color Dodge"));
	modes.append( tr("Color Burn"));
	modes.append( tr("Hue"));
	modes.append( tr("Saturation"));
	modes.append( tr("Color"));
	modes.append( tr("Luminosity"));
	softShadowBlendMode->setComboStrings(modes);
	softShadowBlendMode->setStringValue( tr("Normal"));
	softShadowBlendMode->setText(0, tr( "Blendmode:"));

	connect(this->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(handleNewValues()));
}

void PropertiesPalette_Shadow::updateSpinBoxConstants()
{
	if (!m_haveDoc)
		return;
	if(m_doc->m_Selection->count()==0)
		return;
}
