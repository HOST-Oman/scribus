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



#ifndef CANVAS_GESTURE_RULERMOVE_H
#define CANVAS_GESTURE_RULERMOVE_H

#include <QPoint>

class QCursor;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QEvent;
class QInputMethodEvent;
class QMouseEvent;
class QKeyEvent;
class QPainter;

#include "scribusapi.h"
#include "canvas.h"
#include "canvasgesture.h"
#include "canvasmode.h"
#include "fpoint.h"

class ScribusMainWindow;
class ScribusView;

/**
  This class realizes the moving of guides and the moving of the ruler origin
 */
class SCRIBUS_API RulerGesture : public CanvasGesture
{
	Q_OBJECT
public:
	enum Mode { HORIZONTAL, VERTICAL, ORIGIN };
	RulerGesture (ScribusView* view, Mode mode);
	virtual ~RulerGesture() {}

	/**
		Prepares the gesture for 'mode' without using an existing guide. If 'mode' is HORIZONTAL
		or VERTICAL, a new guide will be created when the mouse is moved over a page.
	 */
	void prepare(Mode mode);
	void clear();
	
	virtual void drawControls(QPainter* p);
	virtual void activate(bool);
	virtual void deactivate(bool);
	virtual void mouseReleaseEvent(QMouseEvent *m);
	virtual void mouseMoveEvent(QMouseEvent *m);
	/**
	  This method should be called when the mousebutton is pressed.
	  If there's a moveable guide near this position, it prepares the gesture for moving this guide.
	 */
	virtual void mousePressEvent(QMouseEvent *m);
	
	Mode getMode() { return m_mode; }
	/**
		Use this to test if there's a moveable guide near this position.
		It prepares the gesture for moving this guide.
	 */
	bool mouseHitsGuide(const FPoint& mousePointDoc);
	/**
	  It tests for a guide near position, that guide being moveable or not.
	  If the test results in success, emits guideInfo;
	*/
	void mouseSelectGuide(QMouseEvent *m);
private:
	ScribusMainWindow* m_ScMW;
	Mode m_mode;
	bool m_haveGuide;
	int m_page;
	double m_guide;
	double m_currentGuide;
	bool m_haveCursor;
	QCursor m_cursor;
	QPoint m_xy;
	FPoint m_mousePoint;
	void movePoint(QMouseEvent *m, bool mouseRelease);
	
	signals:
		void guideInfo(int /*direction*/, qreal /*position*/);
};


#endif
