/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          scpattern.cpp  -  description
                             -------------------
    begin                : Sat Sep 9 2006
    copyright            : (C) 2006 by Franz Schmid
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

#include <QString>

#include "scpattern.h"
#include "scpainter.h"
#include "pageitem.h"
#include "pageitem_imageframe.h"
#include "scribusdoc.h"
#include "commonstrings.h"

ScPattern::ScPattern()
{
	items.clear();
	doc = 0;
	pattern = QImage();
	scaleX = 1.0;
	scaleY = 1.0;
	width = 0.0;
	height = 0.0;
	xoffset = 0.0;
	yoffset = 0.0;
};

ScPattern::~ScPattern()
{
//	while (!items.isEmpty())
//	{
//		delete items.takeFirst();
//	}
}

void ScPattern::setDoc(ScribusDoc *theDoc)
{
	doc = theDoc;
}

QImage* ScPattern::getPattern()
{
	return &pattern;
}

void ScPattern::setPattern(QString name)
{
	items.clear();
	doc->setLoading(true);
	PageItem* newItem = new PageItem_ImageFrame(doc, 0, 0, 1, 1, 0, CommonStrings::None, CommonStrings::None);
	if (newItem->loadImage(name, false, 72, false))
	{
		pattern = newItem->pixm.qImage().copy();
		scaleX = (72.0 / newItem->pixm.imgInfo.xres) * newItem->pixm.imgInfo.lowResScale;
		scaleY = (72.0 / newItem->pixm.imgInfo.xres) * newItem->pixm.imgInfo.lowResScale;
		newItem->setWidth(pattern.width());
		newItem->setHeight(pattern.height());
		newItem->SetRectFrame();
		newItem->gXpos = 0.0;
		newItem->gYpos = 0.0;
		newItem->gWidth = pattern.width();
		newItem->gHeight = pattern.height();
		width = pattern.width();
		height = pattern.height();
		items.append(newItem);
	}
	else
		pattern = QImage();
	doc->setLoading(false);
}

void ScPattern::createPreview()
{
	double sc = 500.0 / qMax(width, height);
	bool savedFlag = doc->guidesPrefs().framesShown;
	doc->guidesPrefs().framesShown = false;
	pattern = QImage(qRound(width * sc), qRound(height * sc), QImage::Format_ARGB32_Premultiplied);
	pattern.fill( qRgba(0, 0, 0, 0) );
	ScPainter *painter = new ScPainter(&pattern, pattern.width(), pattern.height(), 1, 0);
	painter->setZoomFactor(sc);
	for (int em = 0; em < items.count(); ++em)
	{
		PageItem* embedded = items.at(em);
		painter->save();
		painter->translate(embedded->gXpos, embedded->gYpos);
		embedded->isEmbedded = true;
		embedded->invalid = true;
		embedded->DrawObj(painter, QRectF());
		embedded->isEmbedded = false;
		painter->restore();
	}
	painter->end();
	delete painter;
	doc->guidesPrefs().framesShown = savedFlag;
}
