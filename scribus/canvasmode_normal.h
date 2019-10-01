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



#ifndef CANVASMODE_NORMAL_H
#define CANVASMODE_NORMAL_H

#include <QObject>
#include <QTime>

#include "canvasmode.h"
#include "fpointarray.h"

class LineMove;
class PageItem;
class ResizeGesture;
class RulerGesture;
class ScribusMainWindow;
class ScribusView;


// This class encapsulate the old code for mouse interaction from scribusview.cpp

class CanvasMode_Normal :  public CanvasMode
{
	Q_OBJECT
	
public:
	explicit CanvasMode_Normal(ScribusView* view);
	~CanvasMode_Normal() override = default;

	void enterEvent(QEvent *) override;
	void leaveEvent(QEvent *) override;
	
	void activate(bool) override;
	void deactivate(bool) override;
	void mouseDoubleClickEvent(QMouseEvent *m) override;
	void mouseReleaseEvent(QMouseEvent *m) override;
	void mouseMoveEvent(QMouseEvent *m) override;
	void mousePressEvent(QMouseEvent *m) override;
	void drawControls(QPainter* p) override;

	void keyPressEvent(QKeyEvent *e) override;
	void keyReleaseEvent(QKeyEvent *e) override;

	bool handleKeyEvents() override { return true; }

private:
	inline bool GetItem(PageItem** pi);
	void handleCheckBoxPress(PageItem* currItem);
	void handlePushButtonPress(PageItem* currItem);
	void handleRadioButtonPress(PageItem* currItem);
	void handleCheckBoxRelease(PageItem* currItem);
	void handlePushButtonRelease(PageItem* currItem);
	void handleRadioButtonRelease(PageItem* currItem);
	void handleJavaAction(PageItem* currItem, int event);
	void handleNamedAction(PageItem* currItem);
	void handleLinkAnnotation(PageItem* currItem);
	void handleFocusOut(PageItem* currItem);
	void handleFocusIn(PageItem* currItem);
	void handleMouseLeave(PageItem* currItem);
	void handleMouseEnter(PageItem* currItem);
	bool SeleItem(QMouseEvent *m);
	void createContextMenu(PageItem *currItem, double mx, double my);

	FPoint m_mousePressPoint;
	FPoint m_mouseCurrentPoint;
	FPoint m_mouseSavedPoint;
	FPoint m_objectDeltaPos;

	bool m_shiftSelItems;
	int  m_frameResizeHandle;
	int  m_dragConstrainInitPtX, m_dragConstrainInitPtY;
	ScribusMainWindow* m_ScMW;
	ResizeGesture* m_resizeGesture;
	LineMove* m_lineMoveGesture;
	RulerGesture* m_guideMoveGesture;
	bool m_lastPosWasOverGuide;
	PageItem* m_hoveredItem;

public slots:
	void importToPage();
};


#endif
