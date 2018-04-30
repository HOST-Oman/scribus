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

#ifndef SCRIBUSVIEW_H
#define SCRIBUSVIEW_H

#include <vector>
// include files for QT
#include <QScrollArea>
#include <QLineEdit>
#include <QScrollBar>
#include <QMap>
#include <QMenu>
#include <QLabel>
#include <QComboBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QSpinBox>
#include <QCursor>
#include <QDragLeaveEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QRect>
#include <QRectF>
#include <QTime>
#include <QTimer>
#include <QWheelEvent>
#include <QRubberBand>
#include <QList>

class QEvent;
class QMimeData;

// application specific includes
#include "observable.h"
#include "scribusapi.h"
#include "scribusdoc.h"
#include "ui/clockwidget.h"
#include "undotransaction.h"

class Canvas;
class CanvasMode;
class CanvasGesture;
class Hruler;
class Vruler;
class ScPage;
class RulerMover;
class PageItem;
class PageSelector;
class ScribusDoc;
class ScribusWin;
class ScribusMainWindow;
class ScrSpinBox;
class Selection;
class UndoManager;
class TransactionSettings;
#include "selectionrubberband.h"

/**
 * This class provides an incomplete base for your application view.
 */

class SCRIBUS_API ScribusView : public QScrollArea, public Observer<QRectF>
{
	Q_OBJECT

public:
    ScribusView(QWidget* win=0, ScribusMainWindow* mw=0, ScribusDoc* doc=0);
    ~ScribusView();
	
	friend class CanvasMode_CopyProperties;
	friend class CanvasMode_Edit;
	friend class CanvasMode_EditGradient;
	friend class CanvasMode_EditMeshGradient;
	friend class CanvasMode_EditMeshPatch;
	friend class CanvasMode_EditTable;
	friend class CanvasMode_EditWeldPoint;
	friend class CanvasMode_EditPolygon;
	friend class CanvasMode_EditArc;
	friend class CanvasMode_EditSpiral;
	friend class CanvasMode_FrameLinks;
	friend class CanvasMode_ImageImport;
	friend class CanvasMode_Magnifier;
	friend class CanvasMode_NodeEdit;
	friend class CanvasMode_Normal;
	friend class CanvasMode_ObjImport;
	friend class CanvasMode_Rotate;
	
	void requestMode(int appMode);
//	void setCursorBasedOnAppMode(int appMode);
	void startGesture(CanvasGesture*);
	void stopGesture();
	
  /** Vergroesserungseingabefeld */
	RulerMover *rulerMover; //Widget between the two rulers for dragging the ruler origin
	Hruler *horizRuler;
	Vruler *vertRuler;
	ClockWidget *clockLabel;
	QPushButton *endEditButton;
  /** Dokument zu dem die Seite gehoert */
	ScribusDoc * const Doc;
	Canvas * const m_canvas;
	CanvasMode* m_canvasMode; // might be a CanvasGesture FIXME make private
	CanvasMode* canvasMode();
	QMap<int,CanvasMode*> modeInstances;
	ApplicationPrefs * const Prefs;
	UndoManager * const undoManager;
	ScribusMainWindow* m_ScMW;
	double OldScale;
	double dragX,dragY,dragW,dragH;
	double oldW;
	int RotMode;
	bool HaveSelRect;
	bool DraggedGroup;
	bool MidButt;
	bool updateOn;
	bool Magnify;
	bool storedFramesShown;
	bool storedShowControls;
	int editStrokeGradient;
	bool m_AnnotChanged;
	bool m_EditModeWasOn;
	bool m_ChangedState;
	SelectionRubberBand *redrawMarker;
	FPoint RCenter;
	FPoint m_mousePointDoc;
	void updatesOn(bool on);
	//CB This MUST now be called AFTER a call to doc->addPage or doc->addMasterPage as it
	//does NOT create a page anymore.
	ScPage* addPage(int nr, bool mov = true);

	void reformPages(bool moveObjects = true);
	void reformPagesView();
	void showMasterPage(int nr);
	void hideMasterPage();
	void showSymbolPage(QString symbolName);
	void hideSymbolPage();
	void showInlinePage(int id);
	void hideInlinePage();
	QImage PageToPixmap(int Nr, int maxGr, PageToPixmapFlags flags = Pixmap_DrawFrame | Pixmap_DrawBackground);
	QImage MPageToPixmap(QString name, int maxGr, bool drawFrame = true);
	void RecalcPicturesRes();
	/**
	 * Called when the ruler origin is dragged
	 * @param m mouse event
	 */
	void setNewRulerOrigin(QMouseEvent *m);
	void getDragRectScreen(double *x, double *y, double *w, double *h);
	void getGroupRectScreen(double *x, double *y, double *w, double *h);
	bool PointOnLine(QPoint Start, QPoint Ende, QRect MArea);
	void TransformPoly(int mode, int rot = 1, double scaling = 1.0);
	bool slotSetCurs(int x, int y);
	void HandleCurs(PageItem *currItem, QRect mpo);
	void Deselect(bool prop = true);
	void SelectItemNr(uint nr, bool draw = true, bool single = false);
	void SelectItem(PageItem *pi, bool draw = true, bool single = false);
	void rememberOldZoomLocation(int mx=0, int my=0);
	bool groupTransactionStarted() { return m_groupTransactions > 0; }
	void startGroupTransaction(const QString &actionName = "",
							   const QString &description = "",
							   QPixmap *actionPixmap = 0,
							   Selection* customSelection = 0);
	void endGroupTransaction();
	void cancelGroupTransaction();
	void setScale(const double newScale);
	double scale() const;

	virtual void changed(QRectF re, bool);

	void updateCanvas(QRectF box = QRectF());
	void updateCanvas(double x, double y, double width, double height) { updateCanvas(QRectF(x,y,width,height)); }
	void setCanvasOrigin(double x, double y);
	void setCanvasCenter(double x, double y);
	void scrollCanvasBy(double deltaX, double deltaY);
	FPoint canvasOrigin() const;
	QRectF visibleCanvas() const;
	void setRedrawMarkerShown(bool shown);
	
private:
	// legacy:
	void updateContents(QRect box = QRect());
	void updateContents(int x, int y, int w, int h);
	void repaintContents(QRect box);
	void resizeContents(int w, int h);
	QPoint contentsToViewport(QPoint p);
	QPoint viewportToContents(QPoint p);
public: // for now
	int contentsX();
	int contentsY();
	int contentsWidth();
	int contentsHeight();
	void setContentsPos(int x, int y);
	int visibleWidth() { return viewport()->size().width(); } ;
	int visibleHeight() { return viewport()->size().height(); } ;
	void stopAllDrags();
	void scrollBy(int x, int y);
	void zoom(double scale = 0.0);
	void zoom(int canvasX, int canvasY, double scale, bool preservePoint);

public slots: // Public slots
	void languageChange();
	void toggleCMS(bool cmsOn);
	void switchPreviewVisual(int vis);
	void togglePreviewEdit(bool edit);
	void togglePreview(bool inPreview);
	void unitChange();
	void setRulersShown(bool isShown);
  /** Zooms in or out */
	void slotZoom100();
  /** Zooms in */
	void slotZoomIn(int mx=0,int my=0);
	void slotZoomOut(int mx=0,int my=0);
  /** Redraws everything */
	void DrawNew();
	void GotoPa(int Seite);
	void GotoLa(int l);
	void GotoPage(int Seite);
	void ChgUnit(int art);

	void SetCPo(double x, double y);
	void SetCCPo(double x, double y);
	void editExtendedImageProperties();
	void RefreshGradient(PageItem *currItem, double dx = 8, double dy = 8);
	void ToPicFrame();
	void ToPolyFrame();
	void ToTextFrame();
	void ToBezierFrame();
	void ToPathText();
	void FromPathText();
	void Bezier2Poly();
	void PasteToPage();
	void TextToPath();

//for linking frame after draw new frame
private:
	PageItem * firstFrame;

private: // Private attributes
	int m_previousMode;
	QMenu *pmen3;
	QMenu *pmenResolution;
	QPoint m_pressLocation;
	QTime m_moveTimer;
	QTimer *m_dragTimer;
	bool m_dragTimerFired;
	bool Ready;
	int  oldX;
	int  oldY;
	int  m_groupTransactions;
	int m_oldCanvasHeight;
	int m_oldCanvasWidth;
	UndoTransaction m_groupTransaction;
	bool _isGlobalMode;
	bool linkAfterDraw;
	bool ImageAfterDraw;

private slots:
	void setZoom();
	/**
	 * Called to update the GUI when the canvas(view) scrollbars are moved
	 * @param x 
	 * @param y 
	 */
	void setRulerPos(int x, int y);
	void setObjectUndoMode();
	void setGlobalUndoMode();
	void dragTimerTimeOut();

public:
	virtual void wheelEvent ( QWheelEvent *ev );
	virtual void changeEvent(QEvent *e);

	void keyPressEvent(QKeyEvent *k);
	void keyReleaseEvent(QKeyEvent *k);
	void inputMethodEvent ( QInputMethodEvent * event );
	QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const ;
	
	inline void registerMousePress(QPoint p);
	bool mousePressed();
	void resetMousePressed();
	inline QPoint mousePressLocation();
	inline bool moveTimerElapsed();
	inline void resetMoveTimer();
	
	inline void startDragTimer();
	inline void stopDragTimer();
	inline void resetDragTimer();
	inline bool dragTimerElapsed();

	bool handleObjectImport(QMimeData* mimeData, TransactionSettings* trSettings = NULL);

protected: // Protected methods
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);
	virtual void resizeEvent ( QResizeEvent * event );
	bool eventFilter(QObject *obj, QEvent *event);

	// those appear to be gone from QScrollArea:
	virtual void contentsDragEnterEvent(QDragEnterEvent *e);
	virtual void contentsDragMoveEvent(QDragMoveEvent *e);
	virtual void contentsDragLeaveEvent(QDragLeaveEvent *e);
	virtual void contentsDropEvent(QDropEvent *e);
	virtual void setHBarGeometry(QScrollBar &bar, int x, int y, int w, int h);
	virtual void setVBarGeometry(QScrollBar &bar, int x, int y, int w, int h);
	void scrollContentsBy(int dx, int dy);
	
	//The width of vertical ruler/height of horizontal ruler, set to 17 in scribusview.cpp
	int m_vhRulerHW;

signals:
	void changeUN(int);
	void changeLA(int);
	void HaveSel();
	void DocChanged();
	void ItemGeom();
	void PolyOpen();
	void PStatus(int, uint);
	void SetAngle(double);
	void SetSizeValue(double);
	void SetLineArt(Qt::PenStyle, Qt::PenCapStyle, Qt::PenJoinStyle);
	void SetLocalValues(double, double, double, double);
	void ItemTextAttr(double);
	void ItemTextCols(int, double);
	void SetDistValues(double, double, double, double);
	void ItemCharStyle(const CharStyle&);
	void ItemTextAlign(int);
	void ItemTextEffects(int);
	void HasTextSel();
	void HasNoTextSel();
	void MVals(double, double, double, double, double, double, int);
	void PaintingDone();
	void LoadPic();
	void StatusPic();
	void AppendText();
	void DoGroup();
	void CutItem();
	void CopyItem();
	void Amode(int);
	void AddBM(PageItem *);
	void DelBM(PageItem *);
	void ChBMText(PageItem *);
	void ToScrap(QString);
	void LoadElem(QString, double, double, bool, bool, ScribusDoc *, ScribusView *);
	void LevelChanged(uint);
	void HavePoint(bool, bool);
	void ClipPo(double, double);
	void PolyStatus(int, uint);
	void AnnotProps();
	void EndNodeEdit();
	void Hrule(int);
	void Vrule(int);
	void MousePos(double, double);
	void callGimp();
	void signalGuideInformation(int, qreal);
};




inline void ScribusView::registerMousePress(QPoint p)
{
	m_pressLocation = p;
	m_moveTimer.start();
	m_dragTimerFired = false;
}


inline QPoint ScribusView::mousePressLocation()
{
	return m_pressLocation;
}


inline bool ScribusView::moveTimerElapsed()
{
	return (m_moveTimer.elapsed() > Prefs->uiPrefs.mouseMoveTimeout);
}


inline void ScribusView::resetMoveTimer()
{
	m_moveTimer.start();
}


inline void ScribusView::startDragTimer()
{
	m_dragTimerFired = false;
	m_dragTimer->setSingleShot(true);
	m_dragTimer->start(1000);			// set Timeout for starting a Drag operation to 1 sec.
}

inline void ScribusView::stopDragTimer()
{
	m_dragTimer->stop();
}


inline void ScribusView::resetDragTimer()
{
	m_dragTimerFired = false;
}


inline bool ScribusView::dragTimerElapsed()
{
	return m_dragTimerFired;
}

inline CanvasMode* ScribusView::canvasMode()
{
	return m_canvasMode;
}

#endif
