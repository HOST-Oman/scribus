/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          pageitem.h  -  description
                             -------------------
    copyright            : Scribus Team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PAGEITEM_REGULARPOLYGON_H
#define PAGEITEM_REGULARPOLYGON_H

#include <QString>
#include <QRectF>

#include "scribusapi.h"
#include "pageitem.h"
class ScPainter;
class ScribusDoc;


class SCRIBUS_API PageItem_RegularPolygon : public PageItem
{
	Q_OBJECT

public:
	PageItem_RegularPolygon(ScribusDoc *pa, double x, double y, double w, double h, double w2, const QString& fill, const QString& outline);
	PageItem_RegularPolygon(const PageItem & p) : PageItem(p) {}
	~PageItem_RegularPolygon() {};

	PageItem_RegularPolygon * asRegularPolygon() override { return this; }
	bool isRegularPolygon() const override { return true; }
	ItemType realItemType() const override { return PageItem::RegularPolygon; }
	void applicableActions(QStringList& actionList) override;
	QString infoDescription() const override;

	void recalcPath();
	/* Regular Polygon Tool */
	int polyCorners;  //! Number of corners for a polygon
	double polyFactor; //! Convex/Concave factor as a double
	bool polyUseFactor; //! Whether to use a factor for shaping
	double polyRotation; //! Rotation of a polygon
	double polyCurvature; //! Curvature of polygon
	double polyInnerRot;
	double polyOuterCurvature;
	
protected:
	void DrawObj_Item(ScPainter *p, QRectF e) override;

};

#endif
