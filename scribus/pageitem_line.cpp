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

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <cassert>

#include "scconfig.h"

#include "commonstrings.h"
#include "pageitem.h"
#include "pageitem_line.h"
#include "prefsmanager.h"
#include "scpage.h"
#include "scpainter.h"
#include "scpaths.h"

#include "scribusstructs.h"
#include "scribusdoc.h"
#include "scribusview.h"
#include "undomanager.h"
#include "undostate.h"
#include "util.h"
#include "util_math.h"

using namespace std;

PageItem_Line::PageItem_Line(ScribusDoc *pa, double x, double y, double w, double h, double w2, const QString& fill, const QString& outline)
	: PageItem(pa, PageItem::Line, x, y, w, h, w2, fill, outline)
{
}

void PageItem_Line::DrawObj_Item(ScPainter *p, QRectF /*e*/)
{
	if (m_Doc->RePos)
		return;

	if (m_Doc->layerOutline(LayerID))
		p->drawLine(FPoint(0, 0), FPoint(m_width, 0));
	else
	{
		if (NamedLStyle.isEmpty())
		{
			ScPattern *strokePattern = m_Doc->checkedPattern(patternStrokeVal);
			if (strokePattern)
			{
				if (patternStrokePath)
				{
					QPainterPath guidePath;
					guidePath.moveTo(0, 0);
					guidePath.lineTo(m_width, 0);
					DrawStrokePattern(p, guidePath);
				}
				else
				{
					p->setPattern(strokePattern, patternStrokeScaleX, patternStrokeScaleY, patternStrokeOffsetX, patternStrokeOffsetY, patternStrokeRotation, patternStrokeSkewX, patternStrokeSkewY, patternStrokeMirrorX, patternStrokeMirrorY);
					p->setStrokeMode(ScPainter::Pattern);
					p->drawLine(FPoint(0, 0), FPoint(m_width, 0));
				}
			}
			else if (GrTypeStroke > 0)
			{
				if ((!gradientStrokeVal.isEmpty()) && (!m_Doc->docGradients.contains(gradientStrokeVal)))
					gradientStrokeVal = "";
				if (!(gradientStrokeVal.isEmpty()) && (m_Doc->docGradients.contains(gradientStrokeVal)))
					stroke_gradient = m_Doc->docGradients[gradientStrokeVal];
				if (stroke_gradient.stops() < 2) // fall back to solid stroking if there are not enough colorstops in the gradient.
				{
					if (lineColor() != CommonStrings::None)
					{
						p->setBrush(strokeQColor);
						p->setStrokeMode(ScPainter::Solid);
					}
					else
					{
						no_stroke = true;
						p->setStrokeMode(ScPainter::None);
					}
				}
				else
				{
					p->setStrokeMode(ScPainter::Gradient);
					p->stroke_gradient = stroke_gradient;
					if (GrTypeStroke == 6)
						p->setGradient(VGradient::linear, FPoint(GrStrokeStartX, GrStrokeStartY), FPoint(GrStrokeEndX, GrStrokeEndY), FPoint(GrStrokeStartX, GrStrokeStartY), GrStrokeScale, GrStrokeSkew);
					else
						p->setGradient(VGradient::radial, FPoint(GrStrokeStartX, GrStrokeStartY), FPoint(GrStrokeEndX, GrStrokeEndY), FPoint(GrStrokeFocalX, GrStrokeFocalY), GrStrokeScale, GrStrokeSkew);
				}
				p->drawLine(FPoint(0, 0), FPoint(m_width, 0));
			}
			else if (lineColor() != CommonStrings::None)
			{
				p->setStrokeMode(ScPainter::Solid);
				p->drawLine(FPoint(0, 0), FPoint(m_width, 0));
			}
			else
				no_stroke = true;
		}
		else
		{
			p->setStrokeMode(ScPainter::Solid);
			multiLine ml = m_Doc->MLineStyles[NamedLStyle];
			QColor tmp;
			for (int it = ml.size()-1; it > -1; it--)
			{
				if (ml[it].Color != CommonStrings::None) // && (ml[it].Width != 0))
				{
					SetQColor(&tmp, ml[it].Color, ml[it].Shade);
					p->setPen(tmp, ml[it].Width, static_cast<Qt::PenStyle>(ml[it].Dash), static_cast<Qt::PenCapStyle>(ml[it].LineEnd), static_cast<Qt::PenJoinStyle>(ml[it].LineJoin));
					p->drawLine(FPoint(0, 0), FPoint(m_width, 0));
				}
			}
		}
	}
	if (m_startArrowIndex != 0)
	{
		QTransform arrowTrans;
		arrowTrans.translate(0, 0);
		arrowTrans.scale(-1,1);
		arrowTrans.scale(m_startArrowScale / 100.0, m_startArrowScale / 100.0);
		drawArrow(p, arrowTrans, m_startArrowIndex);
	}
	if (m_endArrowIndex != 0)
	{
		QTransform arrowTrans;
		arrowTrans.translate(m_width, 0);
		arrowTrans.scale(m_endArrowScale / 100.0, m_endArrowScale / 100.0);
		drawArrow(p, arrowTrans, m_endArrowIndex);
	}
}

void PageItem_Line::applicableActions(QStringList & actionList)
{
	actionList << "itemConvertToBezierCurve";
}

QString PageItem_Line::infoDescription() const
{
	return QString();
}

QPointF PageItem_Line::startPoint()
{
	return QPointF(m_xPos, m_yPos);
}

QPointF PageItem_Line::endPoint()
{
	double rot = this->rotation();
	double x = m_xPos + m_width * cos(rot * M_PI / 180.0);
	double y = m_yPos + m_width * sin(rot * M_PI / 180.0);
	return QPointF(x, y);
}

void PageItem_Line::getBoundingRect(double *x1, double *y1, double *x2, double *y2) const
{
	double minx =  std::numeric_limits<double>::max();
	double miny =  std::numeric_limits<double>::max();
	double maxx = -std::numeric_limits<double>::max();
	double maxy = -std::numeric_limits<double>::max();
	if (m_rotation != 0)
	{
		FPointArray pb;
		pb.resize(0);
		pb.addPoint(FPoint(0,       -m_lineWidth / 2.0, m_xPos, m_yPos, m_rotation, 1.0, 1.0));
		pb.addPoint(FPoint(m_width, -m_lineWidth / 2.0, m_xPos, m_yPos, m_rotation, 1.0, 1.0));
		pb.addPoint(FPoint(m_width, +m_lineWidth / 2.0, m_xPos, m_yPos, m_rotation, 1.0, 1.0));
		pb.addPoint(FPoint(0,       +m_lineWidth / 2.0, m_xPos, m_yPos, m_rotation, 1.0, 1.0));
		for (uint pc = 0; pc < 4; ++pc)
		{
			minx = qMin(minx, pb.point(pc).x());
			miny = qMin(miny, pb.point(pc).y());
			maxx = qMax(maxx, pb.point(pc).x());
			maxy = qMax(maxy, pb.point(pc).y());
		}
		*x1 = minx;
		*y1 = miny;
		*x2 = maxx;
		*y2 = maxy;
	}
	else
	{
		*x1 = m_xPos;
		*y1 = m_yPos - qMax(1.0, m_lineWidth) / 2.0;
		*x2 = m_xPos + m_width;
		*y2 = m_yPos + qMax(1.0, m_lineWidth) / 2.0;
	}

	QRectF totalRect = QRectF(QPointF(*x1, *y1), QPointF(*x2, *y2));
	if (m_startArrowIndex != 0)
	{
		QTransform arrowTrans;
		FPointArray arrow = m_Doc->arrowStyles().at(m_startArrowIndex-1).points.copy();
		arrowTrans.translate(m_xPos, m_yPos);
		arrowTrans.rotate(m_rotation);
		arrowTrans.translate(0, 0);
		arrowTrans.scale(m_startArrowScale / 100.0, m_startArrowScale / 100.0);
		if (NamedLStyle.isEmpty())
		{
			if (m_lineWidth != 0.0)
				arrowTrans.scale(m_lineWidth, m_lineWidth);
		}
		else
		{
			multiLine ml = m_Doc->MLineStyles[NamedLStyle];
			if (ml[ml.size()-1].Width != 0.0)
				arrowTrans.scale(ml[ml.size()-1].Width, ml[ml.size()-1].Width);
		}
		arrowTrans.scale(-1,1);
		arrow.map(arrowTrans);
		FPoint minAr = getMinClipF(&arrow);
		FPoint maxAr = getMaxClipF(&arrow);
		totalRect = totalRect.united(QRectF(QPointF(minAr.x(), minAr.y()), QPointF(maxAr.x(), maxAr.y())));
	}
	if (m_endArrowIndex != 0)
	{
		QTransform arrowTrans;
		FPointArray arrow = m_Doc->arrowStyles().at(m_endArrowIndex-1).points.copy();
		arrowTrans.translate(m_xPos, m_yPos);
		arrowTrans.rotate(m_rotation);
		arrowTrans.translate(m_width, 0);
		arrowTrans.scale(m_endArrowScale / 100.0, m_endArrowScale / 100.0);
		if (NamedLStyle.isEmpty())
		{
			if (m_lineWidth != 0.0)
				arrowTrans.scale(m_lineWidth, m_lineWidth);
		}
		else
		{
			multiLine ml = m_Doc->MLineStyles[NamedLStyle];
			if (ml[ml.size()-1].Width != 0.0)
				arrowTrans.scale(ml[ml.size()-1].Width, ml[ml.size()-1].Width);
		}
		arrow.map(arrowTrans);
		FPoint minAr = getMinClipF(&arrow);
		FPoint maxAr = getMaxClipF(&arrow);
		totalRect = totalRect.united(QRectF(QPointF(minAr.x(), minAr.y()), QPointF(maxAr.x(), maxAr.y())));
	}
	totalRect.getCoords(x1, y1, x2, y2);
}

void PageItem_Line::getVisualBoundingRect(double * x1, double * y1, double * x2, double * y2) const
{
	double minx =  std::numeric_limits<double>::max();
	double miny =  std::numeric_limits<double>::max();
	double maxx = -std::numeric_limits<double>::max();
	double maxy = -std::numeric_limits<double>::max();
	double extraSpace = 0.0;
	if (NamedLStyle.isEmpty())
	{
		if ((lineColor() != CommonStrings::None) || (!patternStrokeVal.isEmpty()) || (GrTypeStroke > 0))
		{
			extraSpace = m_lineWidth / 2.0;
			if ((extraSpace == 0) && m_Doc->view()) // Hairline case
				extraSpace = 0.5 / m_Doc->view()->scale();
		}
		if ((!patternStrokeVal.isEmpty()) && (m_Doc->docPatterns.contains(patternStrokeVal)) && (patternStrokePath))
		{
			ScPattern *pat = &m_Doc->docPatterns[patternStrokeVal];
			QTransform mat;
			mat.rotate(patternStrokeRotation);
			mat.scale(patternStrokeScaleX / 100.0, patternStrokeScaleY / 100.0);
			QRectF p1R = QRectF(0, 0, pat->width / 2.0, pat->height / 2.0);
			QRectF p2R = mat.map(p1R).boundingRect();
			extraSpace = p2R.height();
		}
	}
	else
	{
		multiLine ml = m_Doc->MLineStyles[NamedLStyle];
		const SingleLine& sl = ml.last();
		if (sl.Color != CommonStrings::None)
		{
			extraSpace = sl.Width / 2.0;
			if ((extraSpace == 0) && m_Doc->view()) // Hairline case
				extraSpace = 0.5 / m_Doc->view()->scale();
		}
	}
	if (m_rotation != 0)
	{
		FPointArray pb;
		pb.resize(0);
		pb.addPoint(FPoint(0.0,           -extraSpace, xPos(), yPos(), m_rotation, 1.0, 1.0));
		pb.addPoint(FPoint(visualWidth(), -extraSpace, xPos(), yPos(), m_rotation, 1.0, 1.0));
		pb.addPoint(FPoint(visualWidth(), +extraSpace, xPos(), yPos(), m_rotation, 1.0, 1.0));
		pb.addPoint(FPoint(0.0,           +extraSpace, xPos(), yPos(), m_rotation, 1.0, 1.0));
		for (uint pc = 0; pc < 4; ++pc)
		{
			minx = qMin(minx, pb.point(pc).x());
			miny = qMin(miny, pb.point(pc).y());
			maxx = qMax(maxx, pb.point(pc).x());
			maxy = qMax(maxy, pb.point(pc).y());
		}
		*x1 = minx;
		*y1 = miny;
		*x2 = maxx;
		*y2 = maxy;
	}
	else
	{
		*x1 = m_xPos;
		*y1 = m_yPos - extraSpace;
		*x2 = m_xPos + visualWidth();
		*y2 = m_yPos + extraSpace;
	}

	QRectF totalRect(QPointF(*x1, *y1), QPointF(*x2, *y2));
	if (m_startArrowIndex != 0)
	{
		QTransform arrowTrans;
		FPointArray arrow = m_Doc->arrowStyles().at(m_startArrowIndex-1).points.copy();
		arrowTrans.translate(m_xPos, m_yPos);
		arrowTrans.rotate(m_rotation);
		arrowTrans.translate(0, 0);
		arrowTrans.scale(m_startArrowScale / 100.0, m_startArrowScale / 100.0);
		if (NamedLStyle.isEmpty())
		{
			if (m_lineWidth != 0.0)
				arrowTrans.scale(m_lineWidth, m_lineWidth);
		}
		else
		{
			multiLine ml = m_Doc->MLineStyles[NamedLStyle];
			if (ml[ml.size()-1].Width != 0.0)
				arrowTrans.scale(ml[ml.size()-1].Width, ml[ml.size()-1].Width);
		}
		arrowTrans.scale(-1,1);
		arrow.map(arrowTrans);
		FPoint minAr = getMinClipF(&arrow);
		FPoint maxAr = getMaxClipF(&arrow);
		totalRect = totalRect.united(QRectF(QPointF(minAr.x(), minAr.y()), QPointF(maxAr.x(), maxAr.y())));
	}
	if (m_endArrowIndex != 0)
	{
		QTransform arrowTrans;
		FPointArray arrow = m_Doc->arrowStyles().at(m_endArrowIndex-1).points.copy();
		arrowTrans.translate(m_xPos, m_yPos);
		arrowTrans.rotate(m_rotation);
		arrowTrans.translate(m_width, 0);
		arrowTrans.scale(m_endArrowScale / 100.0, m_endArrowScale / 100.0);
		if (NamedLStyle.isEmpty())
		{
			if (m_lineWidth != 0.0)
				arrowTrans.scale(m_lineWidth, m_lineWidth);
		}
		else
		{
			multiLine ml = m_Doc->MLineStyles[NamedLStyle];
			if (ml[ml.size()-1].Width != 0.0)
				arrowTrans.scale(ml[ml.size()-1].Width, ml[ml.size()-1].Width);
		}
		arrow.map(arrowTrans);
		FPoint minAr = getMinClipF(&arrow);
		FPoint maxAr = getMaxClipF(&arrow);
		totalRect = totalRect.united(QRectF(QPointF(minAr.x(), minAr.y()), QPointF(maxAr.x(), maxAr.y())));
	}
	totalRect.getCoords(x1, y1, x2, y2);
}
