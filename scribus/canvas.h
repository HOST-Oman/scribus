/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */
/***************************************************************************
scribusview.h  -  description
-------------------
    begin                : Fre Apr  6 21:47:55 CEST 2001
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

#ifndef CANVAS_H
#define CANVAS_H

#include <QApplication>
//#include <QDebug>
#include <QPolygon>
#include <QRect>
#include <QRectF>
#include <QWidget>

#include "scribusapi.h"

#include "commonstrings.h"
#include "fpoint.h"
#include "fpointarray.h"
#include "pageitempointer.h"


class ScPage;
class PageItem;
class ScLayer;
class ScPainter;
class ScribusDoc;
class ScribusView;

struct CanvasViewMode
{
	QList<PageItemPointer> linkedFramesToShow;
	QPolygon redrawPolygon;
	bool m_MouseButtonPressed {false};
	bool operItemMoving {false};
	bool operItemResizing {false};
	bool operItemSelecting {false};
	bool operTextSelecting {false};
	bool previewMode {false};
	bool viewAsPreview {false};
	/** if true, selected objects will not be drawn by drawContents() */
	bool drawSelectedItemsWithControls {false};
	/** if true, drawContents() will draw framelinks even if View->Show Framelinks is false */
	bool drawFramelinksWithContents {false};
	// used for buffering:
	bool forceRedraw {false};
	double scale {1};
	int previewVisual {-1};
};

QDataStream &operator<<(QDataStream & ds, const CanvasViewMode & vm);
QDataStream &operator>>(QDataStream & ds, CanvasViewMode & vm);


class SCRIBUS_API Canvas : public QWidget
{
	Q_OBJECT
	
public:	
	static const int moveWithFullOutlinesThreshold = 21;
	static const int moveWithBoxesOnlyThreshold = 41;

	Canvas(ScribusDoc* doc, ScribusView* parent);
	
	friend class ScribusView; // for now...
	friend class CanvasMode;
	friend class CanvasMode_CopyProperties;
	friend class CanvasMode_Edit;
	friend class CanvasMode_EditArc;
	friend class CanvasMode_EditGradient;
	friend class CanvasMode_EditMeshGradient;
	friend class CanvasMode_EditMeshPatch;
	friend class CanvasMode_EditWeldPoint;
	friend class CanvasMode_EditPolygon;
	friend class CanvasMode_EditSpiral;
	friend class CanvasMode_EditTable;
	friend class CanvasMode_EyeDropper;
	friend class CanvasMode_FrameLinks;
	friend class CanvasMode_ImageImport;
	friend class CanvasMode_Magnifier;
	friend class CanvasMode_NodeEdit;
	friend class CanvasMode_ObjImport;
	friend class CanvasMode_Panning;
	friend class CanvasMode_Normal;
	friend class CanvasMode_Rotate;
	friend class FreehandMode;
	friend class CalligraphicMode;
	
	/* Don't rely on these codes!
	 * 2 8 3
	 * 7   6
	 * 4 5 1
	 * But always OUTSIDE < 0, INSIDE >= 0 and any specific handle > 0.
	 */
	enum FrameHandle { 
		OUTSIDE = -1,
		INSIDE = 0,
		NORTHWEST = 2,
		NORTH = 8,
		NORTHEAST = 3,
		EAST = 6,
		SOUTHEAST = 1,
		SOUTH = 5,
		SOUTHWEST = 4,
		WEST = 7
	};
	
	enum RenderMode {
		RENDER_NORMAL,                // update buffer, paint buffer: expensive for large regions
		RENDER_BUFFERED,              // just paint buffer: fast, but only controls may change (eg. resize mode)
		// in the following two modes, only the selected objects are updated. Might not be exact.
		RENDER_SELECTION_SEPARATE,    // paint buffer w/o selection, then paint selection (eg. img edit, nodeedit, rotate, beziercurve)
		RENDER_SELECTION_BUFFERED,    // paint buffer w/o selection, update selection buffer, then paint selection buffer (eg. move, text edit)
		RENDER_LEGACY
	};
	
	void setRenderMode(RenderMode m);
	
	void clearBuffers();              // very expensive
	
	// deprecated:
	void resetRenderMode() { m_renderMode = RENDER_NORMAL; clearBuffers(); }
	void setRenderModeFillBuffer() { m_renderMode = RENDER_BUFFERED; }
	void setRenderModeUseBuffer(bool use) { m_renderMode = (use ? RENDER_BUFFERED : RENDER_NORMAL) ; }

	double scale() const { return m_viewMode.scale; }
	void setScale(double scale);
	QPoint canvasToLocal(const FPoint& p) const;
	QPoint canvasToGlobal(const FPoint& p) const;
	QPoint canvasToLocal(QPointF p) const;
	QPoint canvasToGlobal(QPointF p) const;
	QRect canvasToLocal(const QRectF& p) const;
	QRectF canvasToLocalF(const QRectF& p) const;
	QRect canvasToGlobal(const QRectF& p) const;
	FPoint localToCanvas(QPoint p) const;
//	FPoint localToCanvas(QPointF p) const;
	FPoint globalToCanvas(QPoint p) const;
//	FPoint globalToCanvas(QPointF p) const;
	QRectF globalToCanvas(QRect p) const;
//	QRectF globalToCanvas(QRectF p) const;
	bool hitsCanvasPoint(QPoint globalPoint, const FPoint& canvasPoint) const;
	bool hitsCanvasPoint(QPoint globalPoint, QPointF canvasPoint) const;
	bool hitsCanvasPoint(const FPoint& globalPoint, const QPointF& canvasPoint) const;
	QRect exposedRect() const;
	bool cursorOverTextFrameControl(QPoint globalPos, PageItem* frame);
	bool cursorOverFrameControl(QPoint globalPos, const QRectF& targetRect, PageItem* frame);
	/** Returns the framehandle or INSIDE if the position falls into the frame. */
	FrameHandle frameHitTest(QPointF canvasPoint, PageItem* item) const;
	FrameHandle frameHitTest(QPointF canvasPoint, const QRectF& frame) const;
	/**
		Returns the item under the cursor or nullptr if none found.
	 Does *not* change the selection.
	 If itemAbove is given, it will look for an item under itemAbove, allowing select under.
	 The flag 'allowInGroup' controls if single items within a group or only whole groups are considered.
	 The flag 'allowMasterItems' controls if items from a masterpage are considered.
	 (this flag is ignored in masterpage mode, since all items are masterpage items then).
	 */
	PageItem* itemUnderCursor(QPoint globalPos, PageItem* itemAbove=nullptr, bool allowInGroup=false, bool allowMasterItems=false) const;
	PageItem* itemInGroup(PageItem* group, const QRectF& mouseArea) const;
	PageItem* itemUnderItem(PageItem* item, int& index) const;
	
	const QPolygon& redrawPolygon() const { return m_viewMode.redrawPolygon; }
	QPolygon& newRedrawPolygon() 
	{
		m_viewMode.redrawPolygon.clear();
		return m_viewMode.redrawPolygon;
	}
	void setForcedRedraw(bool on) { m_viewMode.forceRedraw = on; }
	bool isForcedRedraw() const { return m_viewMode.forceRedraw; }
	void setPreviewMode(bool on) { m_viewMode.previewMode = on; }
	bool isPreviewMode() const { return m_viewMode.previewMode || m_viewMode.viewAsPreview; }
	bool usePreviewVisual() const { return m_viewMode.viewAsPreview && m_viewMode.previewVisual != 0; }
	int previewVisual() const { return m_viewMode.previewVisual; }
	void setPreviewVisual(int mode);
	
	void DrawMasterItems(ScPainter *painter, ScPage *page, ScLayer& layer, QRect clip);
	//notesFramesPass determine if notes frames are drawed or not
	void DrawPageItems(ScPainter *painter, ScLayer& layer, QRect clip, bool notesFramesPass);
	void paintEvent ( QPaintEvent * p ) override;
	void displayXYHUD(QPoint m);
	void displayCorrectedXYHUD(QPoint m, double x, double y);
	void displayCorrectedSingleHUD(QPoint m, double val, bool isX);
	void displayXYHUD(QPoint m, double x, double y);
	void displaySizeHUD(QPoint m, double x, double y, bool isLine = false);
	void displayRotHUD(QPoint m, double rot);
	void displayRealRotHUD(QPoint m, double rot);
	/**
	 * Displays a tooltip of the format <code>{label}: {N} {unit}</code> where <code>N</code> is
	 * @a value converted to the current document unit and <code>unit</code> the current document
	 * unit. The tooltip will be displayed nearby the global point @a point.
	 */
		void displayDoubleHUD(QPoint point, const QString& label, double value);

	void setupEditHRuler(PageItem * item, bool forceAndReset = false);
	
private:
	void DrawPageBorderSub(ScPainter *p, ScPage *page);
	void DrawPageBorder(ScPainter *p, const QRectF& clip, bool master = false);
	void DrawPageMarginsGridSub(ScPainter *p, ScPage *page);
	void DrawPageMargins(ScPainter *p, const QRectF& clip, bool master = false);
	void DrawPageBaselineGridSub(ScPainter *p, ScPage *page);
	void DrawPageBaselineGrid(ScPainter *p, const QRectF& clip, bool master = false);
	void DrawPageGridSub(ScPainter *p, ScPage *page, const QRectF& clip);
	void DrawPageGrid(ScPainter *p, const QRectF& clip, bool master = false);
	void DrawPageGuidesSub(ScPainter *p, ScPage *page);
	void DrawPageGuides(ScPainter *p, const QRectF& clip, bool master = false);
	void DrawPageIndicatorSub(ScPainter *p, ScPage *page);
	void DrawPageIndicator(ScPainter *p, const QRectF& clip, bool master = false);
	void drawLinkFrameLine(ScPainter* painter, FPoint &start, FPoint &end);
	void PaintSizeRect(QRect newRect);
	void PaintSizeRect(QPolygon newRect);
	void Transform(PageItem *currItem, QPainter *p);
	void Transform(PageItem *currItem, QTransform& m);
	void TransformM(PageItem *currItem, QPainter *p);
	void getGroupRectScreen(double *x, double *y, double *w, double *h);

	/**
		Enlarges the buffer such that it contains the viewport.
	 */
	bool adjustBuffer();
	/**
		Fills the given buffer with contents.
	    bufferOrigin and clipRect are in local coordinates
	 */
	void fillBuffer(QPaintDevice* buffer, QPoint bufferOrigin, QRect clipRect);
	void drawContents(QPainter *p, int clipx, int clipy, int clipw, int cliph);
	void drawBackgroundMasterpage(ScPainter* painter, int clipx, int clipy, int clipw, int cliph);
	void drawBackgroundPageOutlines(ScPainter* painter, int clipx, int clipy, int clipw, int cliph);
	void drawFrameLinks(ScPainter* painter);
	void drawControls(QPainter* p);
	void drawControlsMovingItemsRect(QPainter* pp);
	void drawControlsBezierCurve(QPainter* pp, PageItem* currItem);
	void drawControlsMeasurementLine(QPainter* pp);
	void drawControlsDrawLine(QPainter* pp);
	void drawControlsFreehandLine(QPainter* pp);
	void getLinkedFrames(PageItem* currItem);
	void getClipPathForPages(FPointArray* PoLine);
	void calculateFrameLinkPoints(PageItem* pi1, PageItem* pi2, FPoint& start, FPoint& end);

	// create a potentially hidpi pixmap
	QPixmap createPixmap(double w, double h);
	// draw a potentially hidpi pixmap
	void drawPixmap(QPainter &painter, double x, double y, const QPixmap &pixmap, double sx, double sy, double sw, double sh);
		
private:
	ScribusDoc* m_doc;
	ScribusView* m_view;
	CanvasViewMode m_viewMode;
	
	RenderMode m_renderMode;
	QPixmap m_buffer;
	QRect   m_bufferRect;
	QPixmap m_selectionBuffer;
	QRect   m_selectionRect;
	QPoint  m_oldMinCanvasCoordinate;
};


#endif



