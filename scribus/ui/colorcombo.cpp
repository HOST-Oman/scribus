/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/*
 *  sccombobox.h
 *  ScribusCVS
 *
 *  Created by Andreas Vox on 01.10.05.
 *  Copyright 2005 under GPL2. All rights reserved.
 *
 */

#include "colorcombo.h"
#include "commonstrings.h"
// #include "qdebug.h"

ColorCombo::ColorCombo(QWidget* parent) : QComboBox(parent)
{
#ifdef Q_OS_MAC
//	setStyle( new ColorCombo::ScMacStyle() );
#endif
	setEditable(false);
	ColorListBox* lb = new ColorListBox(this);
	setModel( lb->model() );
	setItemDelegate( lb->itemDelegate() );
	setView( lb );
}

ColorCombo::ColorCombo(ColorCombo::PixmapType type, QWidget* parent)
{
#ifdef Q_OS_MAC
//	setStyle( new ColorCombo::ScMacStyle() );
#endif
	setEditable(false);
	ColorListBox::PixmapType clbType = ColorListBox::smallPixmap;
	if (type == ColorCombo::widePixmaps)
		clbType = ColorListBox::widePixmap;
	else if (type == ColorCombo::fancyPixmaps)
		clbType = ColorListBox::fancyPixmap;
	ColorListBox* lb = new ColorListBox(clbType, this);
	setModel( lb->model() );
	setItemDelegate( lb->itemDelegate() );
	setView( lb );
}

ColorCombo::ColorCombo(bool rw, QWidget* parent) : QComboBox(parent)
{
#ifdef Q_OS_MAC
//	setStyle( new ColorCombo::ScMacStyle() );
#endif
	setEditable(rw);
	ColorListBox* lb = new ColorListBox(this);
	setModel( lb->model() );
	setItemDelegate( lb->itemDelegate() );
	setView( lb );
}


QString ColorCombo::currentColor() const
{
	QString  colorName = currentText();
	if (colorName == CommonStrings::tr_NoneColor)
		colorName = CommonStrings::None;
	return colorName;
}

void ColorCombo::setPixmapType(ColorCombo::PixmapType type)
{
	ColorListBox* clb = dynamic_cast<ColorListBox*>(view());
	if (!clb)
		return;

	if (type == ColorCombo::smallPixmaps)
		clb->setPixmapType(ColorListBox::smallPixmap);
	else if (type == ColorCombo::widePixmaps)
		clb->setPixmapType(ColorListBox::widePixmap);
	else if (type == ColorCombo::fancyPixmaps)
		clb->setPixmapType(ColorListBox::fancyPixmap);
}

void ColorCombo::updateBox(ColorList& list, bool insertNone)
{
	ColorListBox* clb = dynamic_cast<ColorListBox*>(view());
	if (!clb)
		return;

	clb->clear();
	if (insertNone)
	{
		clb->addItem(CommonStrings::tr_NoneColor);
		clb->item(0)->setData(Qt::UserRole, CommonStrings::None);
	}
	clb->insertItems(list);
}

void ColorCombo::insertItems(ColorList& list)
{
	ColorListBox* clb = dynamic_cast<ColorListBox*>(view());
	if (!clb)
		return;

	clb->insertItems(list);
}

void ColorCombo::insertItem(const ScColor& col, ScribusDoc* doc, const QString& colName)
{
	ColorListBox* clb = dynamic_cast<ColorListBox*>(view());
	if (clb)
		clb->addItem( new ColorPixmapItem(ColorPixmapValue(col, doc, colName)));
}

void ColorCombo::initColorList(ColorList *colorList, ScribusDoc *doc, QString colorValue)
{
	clear();
	setPixmapType(ColorCombo::fancyPixmaps);
	addItem(CommonStrings::tr_NoneColor, CommonStrings::None);
	if (colorValue == CommonStrings::None)
		setCurrentIndex(count()-1);
	ColorList::Iterator endOfColorList(colorList->end());
	for (ColorList::Iterator itc = colorList->begin(); itc != endOfColorList; ++itc)
	{
		insertItem( itc.value(), doc, itc.key() );
		if (itc.key() == colorValue)
			setCurrentIndex(count()-1);
	}
}
