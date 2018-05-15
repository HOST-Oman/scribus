/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          hruler.cpp  -  description
                             -------------------
    begin                : Tue Apr 10 2001
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

#include <cmath>

#include <QCursor>
#include <QMouseEvent>
// #include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPixmap>
#include <QPolygon>
#include <QRect>

#include "canvasgesture_rulermove.h"
#include "hruler.h"
#include "prefsmanager.h"
#include "scpage.h"

#include "scribusdoc.h"
#include "scribusview.h"
#include "selection.h"
#include "units.h"

#include "iconmanager.h"


#ifdef Q_OS_MAC
    #define topline 1
#else
    #define topline 3
#endif
#define bottomline 15
#define rulerheight (bottomline - topline)
#define midline (topline + rulerheight/2)
#define tabline 7

enum ruler_code 
{ 
	rc_none = 0, 
	rc_leftFrameDist = 1, 
	rc_rightFrameDist = 2,
	rc_indentFirst = 3,
	rc_leftMargin = 4,
	rc_tab = 5,
	rc_rightMargin = 6
};


Hruler::Hruler(ScribusView *pa, ScribusDoc *doc) : QWidget(pa),
	textEditMode(false),
	ColGap(0.0),
	lineCorr(0.0),
	Cols(0),
	RExtra(0.0),
	Extra(0.0),
	Indent(0.0),
	First(0.0),
	RMargin(0.0),
	Revers(false),
	currItem(nullptr),
	ItemPos(0.0),
	ItemEndPos(0.0),
	offs(0.0),
	itemScale(1.0),
	Markp(0),
	oldMark(0),
	Mpressed(false),
	ActCol(1),
	ActTab(0),
	Scaling(0.0),
	RulerCode(rc_none),
	MouseX(0),
	m_doc(doc),
	m_view(pa),
	whereToDraw(0),
	drawMark(false)
{
	prefsManager=PrefsManager::instance();
	setBackgroundRole(QPalette::Window);
	setAutoFillBackground(true);
	QPalette palette;
	palette.setBrush(QPalette::Window, QColor(240, 240, 240));
	setPalette(palette);
	setMouseTracking(true);
	rulerGesture = new RulerGesture(m_view, RulerGesture::HORIZONTAL);
	unitChange();
}


double Hruler::textBase() const
{
	return ItemPos + lineCorr + Extra;
}

double Hruler::textWidth() const
{
	return ItemEndPos - ItemPos - 2*lineCorr - Extra - RExtra;
}


double Hruler::textPosToCanvas(double x) const
{
	return x + textBase();
}

int Hruler::textPosToLocal(double x) const
{
	return qRound(textPosToCanvas(x) * Scaling)- m_view->contentsX();
}

double Hruler::localToTextPos(int x) const
{
	return ((x + m_view->contentsX()) / Scaling - textBase());
}

void Hruler::shift(double pos)
{
	offs = pos;
}

void Hruler::shiftRel(double dist)
{
	offs += dist;
}


int Hruler::findRulerHandle(QPoint mp, double grabRadius)
{
	int mx = mp.x();
	QRect fpo;
	
	int result = rc_none;
	
	int Pos = textPosToLocal(0);
	if (Pos-1 < (mx + grabRadius) && Pos-1 > (mx - grabRadius))
		result = rc_leftFrameDist;
	
	Pos = textPosToLocal(textWidth());
	if (Pos+1 < (mx + grabRadius) && Pos+1 > (mx - grabRadius))
		result = rc_rightFrameDist;
	
	double ColWidth = (textWidth() - ColGap * (Cols - 1)) / Cols;
	
	ActCol = 0;
	for (int CurrCol = 0; CurrCol < Cols; ++CurrCol)
	{
		Pos = textPosToLocal((ColWidth+ColGap)*CurrCol);
		fpo = QRect(Pos, topline, static_cast<int>(ColWidth*Scaling), rulerheight);
		if (fpo.contains(mp))
		{
			ActCol = CurrCol+1;
			break;
		}
	}
	if (ActCol == 0)
	{
		ActCol = 1;
		return result;
	}
	
	Pos = textPosToLocal(First+Indent+(ColWidth+ColGap)*(ActCol-1));
	fpo = QRect(Pos-1, topline, grabRadius+1, rulerheight/2);
	if (fpo.contains(mp))
	{
		return rc_indentFirst;
	}
	Pos = textPosToLocal(Indent+(ColWidth+ColGap)*(ActCol-1));
	fpo = QRect(Pos-1, midline, grabRadius+1, rulerheight/2);
	if (fpo.contains(mp))
	{
		return rc_leftMargin;
	}
	Pos = textPosToLocal(RMargin+(ColWidth+ColGap)*(ActCol-1));
	fpo = QRect(Pos-grabRadius, midline, grabRadius, rulerheight/2);
	if (fpo.contains(mp))
	{
		return rc_rightMargin;
	}
	if (TabValues.count() != 0)
	{
		for (int yg = 0; yg < signed(TabValues.count()); yg++)
		{
			Pos = textPosToLocal(TabValues[yg].tabPosition+(ColWidth+ColGap)*(ActCol-1));
			fpo = QRect(Pos-grabRadius, tabline, 2*grabRadius, rulerheight/2 + 2);
			if (fpo.contains(mp))
			{
				result = rc_tab;
				ActTab = yg;
				break;
			}
		}
	}
	return result;
}


void Hruler::mousePressEvent(QMouseEvent *m)
{
	if (m_doc->isLoading())
		return;
	Mpressed = true;
	MouseX = m->x();
	if (textEditMode)
	{
		RulerCode = findRulerHandle(m->pos(), m_doc->guidesPrefs().grabRadius);
		
		if ((RulerCode == rc_none) && (ActCol != 0) && (m->button() == Qt::LeftButton))
		{
			ParagraphStyle::TabRecord tb;
			tb.tabPosition = localToTextPos(m->x());
			tb.tabType = 0;
			tb.tabFillChar = m_doc->itemToolPrefs().textTabFillChar[0];
			TabValues.prepend(tb);
			ActTab = 0;
			RulerCode = rc_tab;
			UpdateTabList();
			qApp->setOverrideCursor(QCursor(Qt::SizeHorCursor));
			emit DocChanged(false);
		}
	}
	else
	{
		if (m_doc->guidesPrefs().guidesShown)
		{
			qApp->setOverrideCursor(QCursor(Qt::SplitVCursor));
			m_view->startGesture(rulerGesture);
			m_view->registerMousePress(m->globalPos());
		}
	}
}

void Hruler::mouseReleaseEvent(QMouseEvent *m)
{
	if (m_doc->isLoading())
	{
		Mpressed = false;
		return;
	}
	if (textEditMode && currItem)
	{
		if ((m->y() < height()) && (m->y() > 0))
		{
			bool mustApplyStyle = false;
			ParagraphStyle paraStyle;
			double ColWidth = (textWidth() - ColGap * (Cols - 1)) / Cols;
			switch (RulerCode)
			{
				case rc_leftFrameDist:
					m_doc->m_Selection->itemAt(0)->setTextToFrameDistLeft(Extra);
					emit DocChanged(false);
					break;
				case rc_rightFrameDist:
					m_doc->m_Selection->itemAt(0)->setTextToFrameDistRight(RExtra);
					emit DocChanged(false);
					break;
				case rc_indentFirst:
					paraStyle.setFirstIndent(First);
					mustApplyStyle = true;
					emit DocChanged(false);
					break;
				case rc_leftMargin:
					paraStyle.setLeftMargin(Indent);
					paraStyle.setFirstIndent(First);
					mustApplyStyle = true;
					emit DocChanged(false);
					break;
				case rc_rightMargin:
					paraStyle.setRightMargin(ColWidth - RMargin);
					mustApplyStyle = true;
					emit DocChanged(false);
					break;
				case rc_tab:
					if (m->button() == Qt::RightButton)
					{
						TabValues[ActTab].tabType += 1;
						if (TabValues[ActTab].tabType > 4)
							TabValues[ActTab].tabType = 0;
					}
					paraStyle.setTabValues(TabValues);
					mustApplyStyle = true;
					emit DocChanged(false);
					break;
				default:
					break;
			}
			if (mustApplyStyle)
			{
				Selection tempSelection(this, false);
				tempSelection.addItem(currItem);
				m_doc->itemSelection_ApplyParagraphStyle(paraStyle, &tempSelection);
			}
			else
			{
				currItem->update();
			}
		}
		else
		{
			if (RulerCode == rc_tab)
			{
				TabValues.removeAt(ActTab);
				ActTab = 0;
				ParagraphStyle paraStyle;
				paraStyle.setTabValues(TabValues);
				Selection tempSelection(this, false);
				tempSelection.addItem(currItem);
				m_doc->itemSelection_ApplyParagraphStyle(paraStyle, &tempSelection);
				emit DocChanged(false);
			}
		}
		RulerCode = rc_none;
		m_view->DrawNew();
		m_doc->m_Selection->itemAt(0)->emitAllToGUI();
	}
	else
	{
		if (Mpressed)
		{
			rulerGesture->mouseReleaseEvent(m);
			Mpressed = false;
		}
	}
	Mpressed = false;
	qApp->restoreOverrideCursor();
}

void Hruler::enterEvent(QEvent *e)
{
	if (textEditMode)
		qApp->changeOverrideCursor(IconManager::instance()->loadCursor("tab.png", 3));
}

void Hruler::leaveEvent(QEvent *m)
{
	emit MarkerMoved(0, -1);
	qApp->restoreOverrideCursor();
	m_view->m_canvasMode->setModeCursor();
}

void Hruler::mouseMoveEvent(QMouseEvent *m)
{
	if (m_doc->isLoading())
		return;
	if (textEditMode)
	{
		double ColWidth = (textWidth() - ColGap * (Cols - 1)) / Cols;
		int ColEnd, ColStart;
		double oldInd;
		if (RulerCode == rc_leftFrameDist || RulerCode == rc_rightFrameDist)
		{
			ColStart = 0; //textPosToLocal(0);
			ColEnd   = width(); //textPosToLocal(textWidth());
		}
		else
		{
			ColStart = textPosToLocal((ColWidth+ColGap)*(ActCol-1));
			ColEnd   = textPosToLocal((ColWidth+ColGap)*(ActCol-1) + ColWidth);
		}
		if ((Mpressed) && (m->y() < height()) && (m->y() > 0) && (m->x() > ColStart - m_doc->guidesPrefs().grabRadius) && (m->x() < ColEnd + m_doc->guidesPrefs().grabRadius))
		{
			qApp->changeOverrideCursor(QCursor(Qt::SizeHorCursor));
			double toplimit = textWidth() + RExtra - (ColGap * (Cols - 1))-1;
			double toplimit2 = textWidth() + Extra - (ColGap * (Cols - 1))-1;
			switch (RulerCode)
			{
				case rc_leftFrameDist:
					Extra -= (MouseX - m->x()) / Scaling;
					if (Extra < 0)
						Extra = 0;
					if (Extra > toplimit2)
						Extra = toplimit2;
					emit MarkerMoved(currItem->xPos(), textBase()-currItem->xPos());
					repaint();
					break;
				case rc_rightFrameDist:
					RExtra += (MouseX - m->x()) / Scaling;
					if (RExtra < 0)
						RExtra = 0;
					if (RExtra > toplimit)
						RExtra = toplimit;
					emit MarkerMoved(textBase(), toplimit -RExtra);
					repaint();
					break;
				case rc_indentFirst:
					First -= (MouseX - m->x()) / Scaling;
					if (First+Indent < 0)
						First = -Indent;
					if (First+Indent > ColWidth)
						First  = ColWidth-Indent;
					emit MarkerMoved(textBase(), First+Indent);
					repaint();
					break;
				case rc_leftMargin:
					oldInd = Indent+First;
					Indent -= (MouseX - m->x()) / Scaling;
					if (Indent < 0)
						Indent = 0;
					if (Indent > ColWidth-1)
						Indent  = ColWidth-1;
					First = oldInd - Indent;
					emit MarkerMoved(textBase(), Indent);
					repaint();
					break;
				case rc_rightMargin:
					RMargin -= (MouseX - m->x()) / Scaling;
					if (RMargin < 0)
						RMargin = 0;
					if (RMargin > ColWidth-1)
						RMargin  = ColWidth-1;
					emit MarkerMoved(textBase(), RMargin);
					repaint();
					break;
				case rc_tab:
					TabValues[ActTab].tabPosition -= (MouseX - m->x()) / Scaling;
					if (TabValues[ActTab].tabPosition < 0)
						TabValues[ActTab].tabPosition = 0;
					if (TabValues[ActTab].tabPosition > ColWidth-1)
						TabValues[ActTab].tabPosition  = ColWidth-1;
					emit MarkerMoved(textBase(), TabValues[ActTab].tabPosition);
					UpdateTabList();
					repaint();
					break;
				default:
					break;
			}
			MouseX = m->x();
/*			if (RulerCode != rc_none)
			{
				QPoint py = m_view->viewport()->mapFromGlobal(m->globalPos());
				QPainter p;
				p.begin(m_view->viewport());
				p.setCompositionMode(QPainter::CompositionMode_Xor);
				p.setPen(QPen(Qt::white, 1, Qt::DotLine, Qt::FlatCap, Qt::MiterJoin));
				QPoint out = m_view->contentsToViewport(QPoint(0, qRound(m_doc->currentPage()->yOffset() * Scaling)));
				p.drawLine(Markp, out.y(), Markp, out.y()+qRound(m_doc->currentPage()->height() * Scaling));
				p.drawLine(py.x(), out.y(), py.x(), out.y()+qRound(m_doc->currentPage()->height() * Scaling));
				p.end(); 
				Markp = py.x();
			}*/
			return;
		}
		if ((!Mpressed) && (m->y() < height()) && (m->y() > 0) && (m->x() > ColStart - 2*m_doc->guidesPrefs().grabRadius) && (m->x() < ColEnd + 2*m_doc->guidesPrefs().grabRadius))
		{
			setCursor(IconManager::instance()->loadCursor("tab.png", 3));
			switch(findRulerHandle(m->pos(), m_doc->guidesPrefs().grabRadius))
			{
				case rc_leftFrameDist: 
					setCursor(QCursor(Qt::SplitHCursor));
					break;
				case rc_rightFrameDist:
					setCursor(QCursor(Qt::SplitHCursor));
					break;
				case rc_indentFirst:
					setCursor(QCursor(Qt::SizeHorCursor));
					break;
				case rc_leftMargin:
					setCursor(QCursor(Qt::SizeHorCursor));
					break;
				case rc_rightMargin:
					setCursor(QCursor(Qt::SizeHorCursor));
					break;
				case rc_tab:
					setCursor(QCursor(Qt::SizeHorCursor));
					break;
			}
			Draw(m->x());
			double marker = localToTextPos(m->x());
			emit MarkerMoved(textBase(), marker);
			return;
		}
		if ((Mpressed) && (RulerCode == rc_tab) && ((m->y() > height()) || (m->y() < 0)))
		{
			setCursor(IconManager::instance()->loadCursor("DelPoint.png", 1, 1));
			return;
		}
		setCursor(QCursor(Qt::ArrowCursor));
	}
	else
	{
		if (Mpressed)
		{
			rulerGesture->mouseMoveEvent(m);
		}
		else
			setCursor(QCursor(Qt::ArrowCursor));
	}
}

void Hruler::paintEvent(QPaintEvent *e)
{
	if (m_doc->isLoading())
		return;
	double sc = m_view->scale();
	Scaling = sc;
	QFont ff = font();
	ff.setPointSize(6);
	setFont(ff);
	QPainter p;
	p.begin(this);
	p.setClipRect(e->rect());
	p.setFont(font());
	
	drawMarks(p);

	if (textEditMode)
	{
		int rectX1 = textPosToLocal(Extra);
		int rectX2 = textPosToLocal(ItemEndPos-ItemPos-RExtra);
		p.eraseRect(QRect(QPoint(rectX1, 1), QPoint(rectX2, 15)));
		p.drawLine(rectX1, 16, rectX2, 16);
		p.save();
		p.setRenderHints(QPainter::Antialiasing, true);
		if (Revers)
		{
			p.translate( textPosToLocal(0), 0);
			p.scale(-1, 1);
			p.translate( textPosToLocal(Extra) - textPosToLocal(ItemEndPos-ItemPos-RExtra), 0);
			p.translate(-textPosToLocal(0), 0);
		}
		for (int CurrCol = 0; CurrCol < Cols; ++CurrCol)
		{
			double ColWidth = (textWidth() - ColGap * (Cols - 1)) / Cols;
			double Pos = (ColWidth + ColGap) * CurrCol;
			double EndPos = Pos + ColWidth;
			drawTextMarks(Pos, EndPos, p);
			
			p.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
			int xPos = textPosToLocal(Pos);
			p.drawLine(xPos, topline, xPos, bottomline);
			if (CurrCol == 0)
			{
				p.drawLine(xPos, 15, (xPos+4), 15);
				p.drawLine(xPos, topline, (xPos+4), topline);
			}
			
			p.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
			
			xPos = textPosToLocal(Pos+First+Indent);
			QPolygon cr;
			cr.setPoints(3, xPos, midline, xPos+3, topline, xPos-3, topline);
			p.drawPolygon(cr);

			xPos = textPosToLocal(Pos+Indent);
			QPolygon cr2;
			cr2.setPoints(3, xPos, midline, xPos+3, bottomline, xPos-3, bottomline);
			p.drawPolygon(cr2);
			
			xPos = textPosToLocal(Pos+RMargin);
			QPolygon cr3;
			cr3.setPoints(3, xPos, topline, xPos, bottomline, xPos-3, midline);
			p.drawPolygon(cr3);

			if (TabValues.count() != 0)
			{
				p.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
				for (int yg = 0; yg < signed(TabValues.count()); yg++)
				{
					xPos = textPosToLocal(Pos+TabValues[yg].tabPosition);
					if (Pos+TabValues[yg].tabPosition < EndPos)
					{
						switch (static_cast<int>(TabValues[yg].tabType))
						{
							case 0:
								if (Revers)
								{
									p.save();
									p.translate(Pos + TabValues[yg].tabPosition,0);
									p.scale(-1,1);
									p.drawLine(0, tabline, 0, bottomline);
									p.drawLine(0, bottomline, 8, bottomline);
									p.restore();
								}
								else
								{
									p.drawLine(xPos, tabline, xPos, bottomline);
									p.drawLine(xPos, bottomline, xPos+8, bottomline);
								}
								break;
							case 1:
								if (Revers)
								{
									p.save();
									p.translate(Pos+TabValues[yg].tabPosition,0);
									p.scale(-1,1);
									p.drawLine(0, tabline, 0, bottomline);
									p.drawLine(0, bottomline, -8, bottomline);
									p.restore();
								}
								else
								{
									p.drawLine(xPos, tabline, xPos, bottomline);
									p.drawLine(xPos, bottomline, xPos-8, bottomline);
								}
								break;
							case 2:
							case 3:
								p.drawLine(xPos, tabline, xPos, bottomline);
								p.drawLine(xPos-4, bottomline, xPos+4, bottomline);
								p.drawLine(xPos+3, bottomline-3, xPos+2, bottomline-3);
								break;
							case 4:
								p.drawLine(xPos, tabline, xPos, bottomline);
								p.drawLine(xPos-4, bottomline, xPos+4, bottomline);
								break;
							default:
								break;
						}
					}
				}
			}
			
			p.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
			xPos = textPosToLocal(EndPos);
			p.drawLine(xPos, topline, xPos, bottomline);
			if (CurrCol == Cols-1)
			{
				p.drawLine(xPos, bottomline, xPos-4 , bottomline);
				p.drawLine(xPos, topline, xPos-4, topline);
			}
		}
		p.restore();
	}
	if (drawMark && !Mpressed)
	{
		drawMarker(p);
	}
	p.end();
}


void Hruler::drawMarker(QPainter& p)
{
	QPolygon cr;
#ifdef OPTION_SMOOTH_MARKERS
	// draw new marker to pixmap
	static const int SCALE = 16;
	static const QColor BACKGROUND(255, 255, 255);
	static QPixmap pix( 4*SCALE, 16*SCALE );
	static bool initpix = true;
	if (initpix) {
		initpix = false;
		QPainter pp( &pix );
		pp.setBrush( BACKGROUND );
		pp.drawRect( 0, 0, 4*SCALE, 16*SCALE );
		
		pp.setPen(Qt::red);
		pp.setBrush(Qt::red);
		cr.setPoints(3, 2*SCALE, 16*SCALE, 4*SCALE, 0, 0, 0);
		pp.drawPolygon(cr);
	}
	// draw pixmap
	p.save();
	p.translate(-m_view->contentsX(), 0);
	p.scale(1.0/SCALE, 1.0/(SCALE+1));
	p.drawPixmap((where-2)*SCALE, 1, pix);
	p.restore();
	// restore marks
	p.setBrush(Qt::black);
	p.setPen(Qt::black);
	p.setFont(font());
	double sc = m_view->getScale();
	double cc = width() / sc;
	double firstMark = ceil(offs / iter) * iter - offs;
	while (firstMark < cc)
	{
		p.drawLine(qRound(firstMark * sc), 10, qRound(firstMark * sc), 16);
		firstMark += iter;
	}
#else
	// draw slim marker
	p.resetTransform();
	p.translate(-m_view->contentsX(), 0);
	p.setPen(Qt::red);
	p.setBrush(Qt::red);
	cr.setPoints(5,  whereToDraw, 5, whereToDraw, 16, whereToDraw, 5, whereToDraw+2, 0, whereToDraw-2, 0);
	p.drawPolygon(cr);
#endif
}


void Hruler::drawMarks(QPainter& p)
{
	p.setBrush(Qt::black);
	p.setPen(Qt::black);
	//p.drawLine(0, 16, width(), 16);
	double sc = Scaling;
	double cc = width() / sc;
	double firstMark = ceil(offs / iter) * iter - offs;
	while (firstMark < cc)
	{
		p.drawLine(qRound(firstMark * sc), 13, qRound(firstMark * sc), 16);
		firstMark += iter;
	}
	firstMark = ceil(offs / iter2) * iter2 - offs;
	int markC = static_cast<int>(ceil(offs / iter2));

	QString tx;
	double xl, frac;
	while (firstMark < cc)
	{
		p.drawLine(qRound(firstMark * sc), topline + 5, qRound(firstMark * sc), 16);
		switch (m_doc->unitIndex())
		{
			case SC_MM:
				tx = QString::number(markC * iter2 / (iter2 / 100) / cor);
				break;
			case SC_IN:
				xl = (markC * iter2 / iter2) / cor;
				tx = QString::number(static_cast<int>(xl));
				frac = fabs(xl - static_cast<int>(xl));
				if ((static_cast<int>(xl) == 0) && (frac > 0.1))
					tx = "";
				if ((frac > 0.24) && (frac < 0.26))
					tx += QChar(0xBC);
				if ((frac > 0.49) && (frac < 0.51))
					tx += QChar(0xBD);
				if ((frac > 0.74) && (frac < 0.76))
					tx += QChar(0xBE);
				tx = tx;
				break;
			case SC_P:
			case SC_C:
				tx = QString::number(markC * iter2 / (iter2 / 5) / cor);
				break;
			case SC_CM:
				tx = QString::number(markC * iter2 / iter2 / cor);
				break;
			default:
				tx = QString::number(markC * iter2);
				break;
		}
		drawNumber(tx, qRound(firstMark * sc) + 2, 9, p);
		//p.drawText(qRound(firstMark * sc)+2, 9, tx);
		firstMark += iter2;
		markC++;
	}
}	

void Hruler::drawTextMarks(double Pos, double EndPos, QPainter& p)
{
	double xl;
	
	p.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
	p.setBrush(Qt::blue);
	for (xl = Pos; xl < EndPos; xl += iter)
	{
		int xli = textPosToLocal(xl);
		p.drawLine(xli, 10, xli, 16);
	}
	for (xl = Pos; xl < EndPos; xl += iter2)
	{
		int xli = textPosToLocal(xl);
		p.drawLine(xli, topline, xli, 16);
		switch (m_doc->unitIndex())
		{
			case SC_IN:
			{
				QString tx = "";
				int num1 = static_cast<int>((xl-Pos) / iter2 / cor);
				if (num1 != 0)
					tx = QString::number(num1);
				double frac = (xl / iter2 / cor) - num1;
				if ((frac > 0.24) && (frac < 0.26))
					tx += QChar(0xBC);
				if ((frac > 0.49) && (frac < 0.51))
					tx += QChar(0xBD);
				if ((frac > 0.74) && (frac < 0.76))
					tx += QChar(0xBE);
				if (Revers)
				{
					p.save();
					p.translate(xli-2, 0);
					p.scale(-1,1);
					drawNumber(tx, 0, 17, p);
					//p.drawText(0, 17, tx);
					p.restore();
				}
				else
					drawNumber(tx, xli+2, 9, p);
				//p.drawText(qRound((xl+2/sc) * sc), 9, tx);
				break;
			}
			case SC_P:
				if (Revers)
				{
					p.save();
					p.translate(xli-2,0);
					p.scale(-1,1);
					drawNumber(QString::number((xl-Pos) / iter / cor), 0, 17, p);
					//p.drawText(0, 17, QString::number((xl-Pos) / iter / cor));
					p.restore();
				}
				else
					drawNumber(QString::number((xl-Pos) / iter / cor), xli+2, 9, p);
				//p.drawText(qRound((xl+2/sc) * sc), 9, QString::number((xl-Pos) / iter / cor));
				break;
			case SC_CM:
				if (Revers)
				{
					p.save();
					p.translate(xli-2,0);
					p.scale(-1,1);
					drawNumber(QString::number((xl-Pos) / iter / 10 / cor), 0, 9, p);
					//p.drawText(0, 9, QString::number((xl-Pos) / iter / 10 / cor));
					p.restore();
				}
				else
					drawNumber(QString::number((xl-Pos) / iter / 10 / cor), xli+2, 9, p);
				//p.drawText(qRound((xl+2/sc) * sc), 9, QString::number((xl-Pos) / iter / 10 / cor));
				break;
			case SC_C:
				if (Revers)
				{
					p.save();
					p.translate(xli-2,0);
					p.scale(-1,1);
					drawNumber(QString::number((xl-Pos) / iter  / cor), 0, 9, p);
					//p.drawText(0, 9, QString::number((xl-Pos) / iter  / cor));
					p.restore();
				}
				else
					drawNumber(QString::number((xl-Pos) / iter  / cor), xli+2, 9, p);
				//p.drawText(qRound((xl+2/sc) * sc), 9, QString::number((xl-Pos) / iter  / cor));
				break;
			default:
				if (Revers)
				{
					p.save();
					p.translate(xli-2,0);
					p.scale(-1,1);
					drawNumber(QString::number((xl-Pos) / iter * 10 / cor), 0, 9, p);
					//p.drawText(0, 9, QString::number((xl-Pos) / iter * 10 / cor));
					p.restore();
				}
				else
					drawNumber(QString::number((xl-Pos) / iter * 10 / cor), xli+2, 9, p);
				//p.drawText(qRound((xl+2/sc) * sc), 9, QString::number((xl-Pos) / iter * 10 / cor));
				break;
		}
	}
}

void Hruler::drawNumber(QString txt, int x, int y0, QPainter & p)
{
	const int y = y0 - 3 + topline;
	p.drawText(x,y,txt);
}

double Hruler::ruleSpacing() {
	return iter;
}

void Hruler::Draw(int where)
{
	// erase old marker
	int currentCoor = where - m_view->contentsX();
	whereToDraw = where;
	drawMark = true;
	repaint(oldMark-3, 0, 7, 17);
//	drawMark = false;
	oldMark = currentCoor;
}

void Hruler::setItem(PageItem * item)
{
	currItem = item;
	QTransform mm = currItem->getTransform();
	QPointF itPos = mm.map(QPointF(0, currItem->yPos()));
	itemScale = mm.m11();
	ItemPos = itPos.x();
	ItemEndPos = ItemPos + item->width() * itemScale;
	/*if (m_doc->guidesPrefs().rulerMode)
	{
		ItemPos -= m_doc->currentPage()->xOffset();
		ItemEndPos -= m_doc->currentPage()->xOffset();
	}*/
	
	if ((item->lineColor() != CommonStrings::None) || (!item->strokePattern().isEmpty()))
		lineCorr = item->lineWidth() / 2.0;
	else
		lineCorr = 0;
	ColGap = item->ColGap;
	Cols = item->Cols;
	Extra = item->textToFrameDistLeft();
	RExtra = item->textToFrameDistRight();

	const ParagraphStyle& currentStyle = item->currentStyle();
	First = currentStyle.firstIndent();
	Indent = currentStyle.leftMargin();
	double columnWidth = (item->width() - (item->columnGap() * (item->columns() - 1))
				- item->textToFrameDistLeft() - item->textToFrameDistLeft()
				- 2*lineCorr) / item->columns();
	RMargin = columnWidth - currentStyle.rightMargin();
	if (item->imageFlippedH())
		Revers = true;
	else
		Revers = false;
	textEditMode = true;
	TabValues = currentStyle.tabValues();	
}

void Hruler::UpdateTabList()
{
	ParagraphStyle::TabRecord tb;
	tb.tabPosition = TabValues[ActTab].tabPosition;
	tb.tabType = TabValues[ActTab].tabType;
	tb.tabFillChar =  TabValues[ActTab].tabFillChar;
	int tab_n = static_cast<int>(TabValues.count());
	int tab_i = tab_n;
	TabValues.removeAt(ActTab);
	for (int i = static_cast<int>(TabValues.count()-1); i > -1; --i)
	{
		if (tb.tabPosition < TabValues[i].tabPosition)
			tab_i = i;
	}
	ActTab = tab_i;
	if (tab_n == tab_i)
	{
		TabValues.append(tb);
		ActTab = static_cast<int>(TabValues.count()-1);
	}
	else
	{
		TabValues.insert(ActTab, tb);
	}
}

void Hruler::unitChange()
{
	double sc = m_view->scale();
	cor=1;
	int docUnitIndex=m_doc->unitIndex();
	switch (docUnitIndex)
	{
		case SC_PT:
			if (sc > 1 && sc <= 4)
				cor = 2;
			if (sc > 4)
				cor = 10;
			if (sc < 0.3)
			{
				iter = unitRulerGetIter1FromIndex(docUnitIndex) * 3;
	  			iter2 = unitRulerGetIter2FromIndex(docUnitIndex) * 3;
			}
			else if (sc < 0.5)
			{
				iter = unitRulerGetIter1FromIndex(docUnitIndex) * 2;
	  			iter2 = unitRulerGetIter2FromIndex(docUnitIndex) * 2;
			}
			else
			{
				iter = unitRulerGetIter1FromIndex(docUnitIndex) / cor;
				iter2 = unitRulerGetIter2FromIndex(docUnitIndex) / cor;
			}
			break;
		case SC_MM:
			if (sc > 1)
				cor = 10;
			iter = unitRulerGetIter1FromIndex(docUnitIndex) / cor;
			iter2 = unitRulerGetIter2FromIndex(docUnitIndex) / cor;
			break;
		case SC_IN:
			iter = unitRulerGetIter1FromIndex(docUnitIndex);
			iter2 = unitRulerGetIter2FromIndex(docUnitIndex);
			if (sc > 1 && sc <= 4)
			{
				cor = 2;
				iter /= cor;
				iter2 /= cor;
			}
			if (sc > 4)
			{
				cor = 4;
				iter /= cor;
				iter2 /= cor;
			}
			if (sc < 0.25)
			{
				cor = 0.5;
				iter = 72.0*16.0;
				iter2 = 72.0*2.0;
			}
			break;
		case SC_P:
			iter = unitRulerGetIter1FromIndex(docUnitIndex);
			iter2 = unitRulerGetIter2FromIndex(docUnitIndex);
			if (sc >= 1 && sc <= 4)
			{
				cor = 1;
				iter = 12.0;
				iter2 = 60.0;
			}
			if (sc > 4)
			{
				cor = 2;
				iter = 6.0;
				iter2 = 12.0;
			}
			if (sc < 0.3)
			{
				cor = 0.25;
				iter = 12.0*4;
				iter2 = 60.0*4;
			}
			else
			if (sc < 1)
			{
				cor = 1;
				iter = 12.0;
				iter2 = 60.0;
			}
			break;
		case SC_CM:
			if (sc > 1 && sc <= 4)
				cor = 1;
			if (sc > 4)
				cor = 10;
			if (sc < 0.6)
			{
				cor=0.1;
				iter = 720.0/25.4;
				iter2 = 7200.0/25.4;
			}
			else
			{
				iter = unitRulerGetIter1FromIndex(docUnitIndex) / cor;
	  			iter2 = unitRulerGetIter2FromIndex(docUnitIndex) / cor;
	  		}
			break;
		case SC_C:
			iter = unitRulerGetIter1FromIndex(docUnitIndex);
			iter2 = unitRulerGetIter2FromIndex(docUnitIndex);
			if (sc >= 1 && sc <= 4)
			{
				cor = 1;
				iter = 72.0/25.4*4.512;
				iter2 = 72.0/25.4*4.512*5.0;
			}
			if (sc > 4)
			{
				cor = 2;
				iter = 72.0/25.4*4.512/2.0;
				iter2 = 72.0/25.4*4.512;
			}
			if (sc < 0.3)
			{
				cor = 0.1;
				iter = 72.0/25.4*4.512*10;
				iter2 = 72.0/25.4*4.512*5.0*10;
			}
			else
			if (sc < 1)
			{
				cor = 1;
				iter = 72.0/25.4*4.512;
				iter2 = 72.0/25.4*4.512*5.0;
			}
			break;
		default:
			if (sc > 1 && sc <= 4)
				cor = 2;
			if (sc > 4)
				cor = 10;
			iter = unitRulerGetIter1FromIndex(0) / cor;
	 		iter2 = unitRulerGetIter2FromIndex(0) / cor;
			break;
	}
}
