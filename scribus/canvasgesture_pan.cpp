/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */
/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <QEvent>
#include <QMouseEvent>

#include "canvasgesture_pan.h"

#include "canvas.h"
#include "scribusapp.h" // for qApp
#include "scribusview.h"
#include "iconmanager.h"

PanGesture::PanGesture (CanvasMode* parent) : CanvasGesture(parent)
{
}

void PanGesture::activate(bool fromGesture)
{
	m_cursor = m_view->cursor();
	m_view->setCursor(IconManager::instance()->loadCursor("handc.png"));
}

void PanGesture::deactivate(bool forgesture)
{
	m_view->setCursor(m_cursor);
}

void PanGesture::drawControls(QPainter* p)
{
	m_delegate->drawControls(p);
}

void PanGesture::mousePressEvent(QMouseEvent* m)
{
	FPoint m_canvasPoint = m_canvas->globalToCanvas(m->globalPos());
	m_canvasRefPoint = QPointF(m_canvasPoint.x(), m_canvasPoint.y());
}

void PanGesture::mouseMoveEvent(QMouseEvent* m)
{
	FPoint m_canvasPoint = m_canvas->globalToCanvas(m->globalPos());
	double scale = m_canvas->scale();
	double dx = (m_canvasRefPoint.x() - m_canvasPoint.x()) * scale;
	double dy = (m_canvasRefPoint.y() - m_canvasPoint.y()) * scale;
	m_view->scrollBy(dx, dy);
}

void PanGesture::mouseReleaseEvent(QMouseEvent* m)
{
	m_view->stopGesture();
}
