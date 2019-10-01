/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          rulermover.cpp  -  description
                             -------------------
    begin                : Sun Jul 24 2005
    copyright            : (C) 2005 by Franz Schmid
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

#include "rulermover.h"

#include <QApplication>
#include <QCursor>
#include <QMenu>
#include <QPixmap>
#include <QPalette>
#include <QWidget>


#include "canvas.h"
#include "canvasgesture_rulermove.h"
#include "iconmanager.h"
#include "scraction.h"
#include "scribus.h"
#include "scribusdoc.h"
#include "scribusview.h"
#include "ui/guidemanager.h"


RulerMover::RulerMover(ScribusView *pa) : QWidget(pa)
{
	setBackgroundRole(QPalette::Window);
	setAutoFillBackground(true);
	QPalette palette;
	palette.setBrush(QPalette::Window, QColor(240, 240, 240));
	palette.setBrush(backgroundRole(), QBrush(IconManager::instance().loadPixmap("mover.png")));
	setPalette(palette);
	currView = pa;
	rulerGesture = new RulerGesture(currView, RulerGesture::ORIGIN);
	Mpressed = false;
}

void RulerMover::mouseDoubleClickEvent(QMouseEvent *)
{
	currView->Doc->rulerXoffset = 0;
	currView->Doc->rulerYoffset = 0;
	currView->DrawNew();
}

void RulerMover::mousePressEvent(QMouseEvent *m)
{
	if (m->button() == Qt::LeftButton)
	{
		Mpressed = true;
		qApp->setOverrideCursor(QCursor(Qt::CrossCursor));
		currView->startGesture(rulerGesture);
		currView->registerMousePress(m->globalPos());
	}
	QWidget::mousePressEvent(m);
}

void RulerMover::mouseReleaseEvent(QMouseEvent *m)
{
	qApp->restoreOverrideCursor();
	if (Mpressed)
	{
		rulerGesture->mouseReleaseEvent(m);
		Mpressed = false;
	}
	else if (m->button() == Qt::RightButton)
	{
		QMenu *pmen = new QMenu();
		QMenu *pmen2 = nullptr;
		pmen->addAction( tr("Reset Rulers"), this, SLOT(resetRulers()));
		pmen->addAction(currView->Doc->scMW()->scrActions["viewRulerMode"]);
		if (currView->Doc->guidesPrefs().rulerMode)
		{
			pmen2 = new QMenu();
			if (pmen2)
			{
				pmen2->setTitle( tr("Move on current Page"));
				pmen2->addAction( tr("Origin at Top Left"), this, SLOT(resetRulers()));
				pmen2->addAction( tr("Origin at Top Right"), this, SLOT(moveRulerTopRight()));
				pmen2->addAction( tr("Origin at Bottom Left"), this, SLOT(moveRulerBottomLeft()));
				pmen2->addAction( tr("Origin at Bottom Right"), this, SLOT(moveRulerBottomRight()));
				pmen2->addAction( tr("Origin at Center"), this, SLOT(moveRulerCenter()));
				pmen2->addAction( tr("Origin at Top Center"), this, SLOT(moveRulerTopCenter()));
				pmen2->addAction( tr("Origin at Bottom Center"), this, SLOT(moveRulerBottomCenter()));
				pmen->addMenu(pmen2);
			}
		}
		pmen->exec(QCursor::pos());
		delete pmen;
		delete pmen2;
	}
	if (currView->Doc->guidesPrefs().guidesShown)
		currView->Doc->scMW()->guidePalette->setupPage();
	QWidget::mouseReleaseEvent(m);
}

void RulerMover::mouseMoveEvent(QMouseEvent *m)
{
	if (Mpressed)
		rulerGesture->mouseMoveEvent(m);
	QWidget::mouseMoveEvent(m);
}

void RulerMover::resetRulers()
{
	currView->Doc->rulerXoffset = 0;
	currView->Doc->rulerYoffset = 0;
	currView->DrawNew();
}

void RulerMover::moveRulerTopRight()
{
	currView->Doc->rulerXoffset = currView->Doc->currentPage()->width();
	currView->Doc->rulerYoffset = 0;
	currView->DrawNew();
}

void RulerMover::moveRulerBottomLeft()
{
	currView->Doc->rulerXoffset = 0;
	currView->Doc->rulerYoffset = currView->Doc->currentPage()->height();
	currView->DrawNew();
}

void RulerMover::moveRulerBottomRight()
{
	currView->Doc->rulerXoffset = currView->Doc->currentPage()->width();
	currView->Doc->rulerYoffset = currView->Doc->currentPage()->height();
	currView->DrawNew();
}

void RulerMover::moveRulerCenter()
{
	currView->Doc->rulerXoffset = currView->Doc->currentPage()->width() / 2.0;
	currView->Doc->rulerYoffset = currView->Doc->currentPage()->height() / 2.0;
	currView->DrawNew();
}

void RulerMover::moveRulerTopCenter()
{
	currView->Doc->rulerXoffset = currView->Doc->currentPage()->width() / 2.0;
	currView->Doc->rulerYoffset = 0;
	currView->DrawNew();
}

void RulerMover::moveRulerBottomCenter()
{
	currView->Doc->rulerXoffset = currView->Doc->currentPage()->width() / 2.0;
	currView->Doc->rulerYoffset = currView->Doc->currentPage()->height();
	currView->DrawNew();
}

