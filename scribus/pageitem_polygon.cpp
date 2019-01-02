/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          pageitem.cpp  -  description
                             -------------------
    begin                : Sat Apr 7 2001
    copyright            : (C) 2001 by Franz Schmid
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

#include "pageitem_polygon.h"

#include <cmath>
#include <cassert>

#include "scconfig.h"

#include "pageitem.h"
#include "prefsmanager.h"
#include "scpage.h"
#include "scpainter.h"
#include "scpaths.h"
#include "scraction.h"

#include "scribusstructs.h"
#include "scribusdoc.h"
#include "undomanager.h"
#include "undostate.h"
#include "util.h"

using namespace std;

PageItem_Polygon::PageItem_Polygon(ScribusDoc *pa, double x, double y, double w, double h, double w2, const QString& fill, const QString& outline)
	: PageItem(pa, PageItem::Polygon, x, y, w, h, w2, fill, outline)
{
}

void PageItem_Polygon::DrawObj_Item(ScPainter *p, QRectF /*e*/)
{
	if (!m_Doc->RePos)
	{
		p->setupPolygon(&PoLine);
		p->fillPath();
	}
}

void PageItem_Polygon::applicableActions(QStringList & actionList)
{
	actionList << "itemConvertToBezierCurve";
	actionList << "itemConvertToImageFrame";
	actionList << "itemConvertToTextFrame";
}

QString PageItem_Polygon::infoDescription() const
{
	return QString();
}
