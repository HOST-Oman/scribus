/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          gradienteditor  -  description
                             -------------------
    begin                : Mit Mai 26 2004
    copyright            : (C) 2004 by Franz Schmid
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

#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPolygon>
#include <QToolTip>

#include "fpoint.h"
#include "gradientpreview.h"
#include "iconmanager.h"
#include "scpainter.h"


GradientPreview::GradientPreview(QWidget *pa) : QFrame(pa)
{
	setFrameShape( QFrame::Panel );
	setFrameShadow( QFrame::Sunken );
	setLineWidth( 2 );
	setMinimumSize(QSize(200, 70));
	setMaximumSize(QSize(3000, 70));
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	Mpressed = false;
	outside = true;
	onlyselect = true;
	isEditable = true;
	fill_gradient = VGradient(VGradient::linear);
	fill_gradient.clearStops();

	QColor color;
	color = QColor(255,255,255);
	fill_gradient.addStop( color, 0.0, 0.5, 1.0 );
	color = QColor(0,0,0);
	fill_gradient.addStop( color, 1.0, 0.5, 1.0 );

	QList<VColorStop*> cstops = fill_gradient.colorStops();
	StopM.clear();
	contextStop = 0;
	ActStop = 0;
	for (int i = 0; i < fill_gradient.stops(); ++i)
	{
		int center = qRound(cstops.at(i)->rampPoint * (width()-20))+10;
		StopM.append(center);
	}
} 

void GradientPreview::paintEvent(QPaintEvent *e)
{
	QList<VColorStop*> cstops = fill_gradient.colorStops();
	StopM.clear();
	for (int i = 0; i < fill_gradient.stops(); ++i)
	{
		int center = qRound(cstops.at(i)->rampPoint * (width()-20))+10;
		StopM.append(center);
	}
	QImage pixm(width()-20, 37, QImage::Format_ARGB32_Premultiplied);
	QPainter pb;
	QBrush b(QColor(205,205,205), IconManager::instance()->loadPixmap("testfill.png"));
	pb.begin(&pixm);
	pb.fillRect(0, 0, pixm.width(), pixm.height(), b);
	pb.end();
	ScPainter *p = new ScPainter(&pixm, width()-20, 37);
//	p->clear(Qt::white);
	p->setPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
	p->setFillMode(2);
	p->fill_gradient = fill_gradient;
	p->setGradient(VGradient::linear, FPoint(0,20), FPoint(width()-20,20), FPoint(0, 0), 1.0, 0.0);
	p->drawRect(0, 0, width()-20, 37);
	p->end();
	delete p;
	QPainter pw;
	pw.begin(this);
	pw.drawImage(10, 5, pixm);
	if (isEditable)
	{
		for (int i = 0; i < fill_gradient.stops(); ++i)
		{
			int center = qRound(cstops.at(i)->rampPoint * (width()-20))+10;
			pw.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
			if (StopM[qMax(ActStop,0)] == center)
				pw.setBrush(Qt::red);
			else
				pw.setBrush(Qt::blue);
			QPolygon cr;
			cr.setPoints(3, center, 43, center-4, 56, center+4, 56);
			pw.drawPolygon(cr);
		}
	}
	pw.end();
	QFrame::paintEvent(e);
}

void GradientPreview::keyPressEvent(QKeyEvent *e)
{
	if (isEditable)
	{
		if(e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace)
		{
			if ((ActStop > 0) && (ActStop != static_cast<int>(StopM.count()-1)))
			{
				onlyselect = false;
				fill_gradient.removeStop(ActStop);
				ActStop = 0;
				repaint();
				QList<VColorStop*> cstops = fill_gradient.colorStops();
				emit selectedStop(cstops.at(ActStop));
			}
		}
	}
}

void GradientPreview::mousePressEvent(QMouseEvent *m)
{
	QRect fpo;
	Mpressed = true;
	qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
	ActStop = -1;
	if (isEditable)
	{
		QList<VColorStop*> cstops = fill_gradient.colorStops();
		for (int yg = 0; yg < static_cast<int>(StopM.count()); ++yg)
		{
			fpo = QRect(static_cast<int>(StopM[yg])-4, 43, 8, 13);
			if (fpo.contains(m->pos()))
			{
				ActStop = yg;
				emit selectedStop(cstops.at(ActStop));
				repaint();
				onlyselect = true;
				return;
			}
		}
	}
}

void GradientPreview::mouseReleaseEvent(QMouseEvent *m)
{
	qApp->restoreOverrideCursor();
	QRect insideRect = QRect(10, 43, width()-20, 13);
	if (isEditable)
	{
		QRect fpo;
		if (m->button() == Qt::LeftButton)
		{
			if ((Mpressed) && (ActStop > 0) && (ActStop != static_cast<int>(StopM.count()-1)) && (outside || !insideRect.contains(m->pos())))
			{
				onlyselect = false;
				fill_gradient.removeStop(ActStop);
				ActStop = 0;
				repaint();
				QList<VColorStop*> cstops = fill_gradient.colorStops();
				emit selectedStop(cstops.at(ActStop));
			}
			if ((m->y() < height()) && (m->y() > 43) && (m->x() > 0) && (m->x() < width()) && (ActStop == -1))
			{
				QList<VColorStop*> cstops = fill_gradient.colorStops();
				double  newStop = static_cast<double>((m->x() - 10)) / (static_cast<double>(width())-20);
				QColor  stopColor = (cstops.count() > 0) ? cstops.at(0)->color : QColor(255, 255, 255);
				QString stopName  = (cstops.count() > 0) ? cstops.at(0)->name  : QString("White");
				int     stopShade = (cstops.count() > 0) ? cstops.at(0)->shade : 100;
				fill_gradient.addStop(stopColor, newStop, 0.5, 1.0, stopName, stopShade);
				repaint();
				onlyselect = false;
				cstops = fill_gradient.colorStops();
				for (int yg = 0; yg < static_cast<int>(StopM.count()); ++yg)
				{
					fpo = QRect(static_cast<int>(StopM[yg])-4, 43, 8, 13);
					if (fpo.contains(m->pos()))
					{
						ActStop = yg;
						emit selectedStop(cstops.at(ActStop));
						repaint();
						break;
					}
				}
			}
		}
		else if (m->button() == Qt::RightButton)
		{
			Mpressed = false;
//			QList<VColorStop*> cstops = fill_gradient.colorStops();
			int stop = -1;
			for (int yg = 0; yg < static_cast<int>(StopM.count()); ++yg)
			{
				fpo = QRect(static_cast<int>(StopM[yg])-4, 43, 8, 13);
				if (fpo.contains(m->pos()))
				{
					stop = yg;
					break;
				}
			}
			contextStop = stop;
			mPos = m->pos();
			QMenu *pmen = new QMenu();
			setCursor(QCursor(Qt::ArrowCursor));
			pmen->addAction( tr("Add Stop"), this, SLOT(addStop()));
			if (stop != -1)
				pmen->addAction( tr("Remove Stop"), this, SLOT(removeStop()));
			pmen->exec(QCursor::pos());
			delete pmen;
		}
	}
	Mpressed = false;
	if ((!onlyselect) && (ActStop >= 0)){
		emit gradientChanged();
		QList<VColorStop*> cstops = fill_gradient.colorStops();
		emit currStep(cstops.at(ActStop)->rampPoint);
	}
}

void GradientPreview::mouseMoveEvent(QMouseEvent *m)
{
	QRect insideRect = QRect(10, 43, width()-20, 13);
	if (isEditable)
	{
		QRect fpo;
		qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
		if ((!Mpressed) && (m->y() < height()) && (m->y() > 43) && (m->x() > 9) && (m->x() < width()-9))
		{
			setCursor(IconManager::instance()->loadCursor("AddPoint.png", 1, 1));
			for (int yg = 0; yg < static_cast<int>(StopM.count()); ++yg)
			{
				fpo = QRect(static_cast<int>(StopM[yg])-4, 43, 8, 13);
				if (fpo.contains(m->pos()))
				{
					setCursor(QCursor(Qt::SizeHorCursor));
					return;
				}
			}
		}
		if (m->buttons() & Qt::LeftButton)
		{
			if ((Mpressed) && (m->y() < height()) && (m->y() > 43) && (m->x() > 9) && (m->x() < width()-9) && (ActStop != -1))
			{
				qApp->changeOverrideCursor(QCursor(Qt::SizeHorCursor));
				double newStop = static_cast<double>((m->x() - 10)) / (static_cast<double>(width())-20);
				if (ActStop > 1)
				{
					if (StopM[ActStop-1]+2 >= m->x())
						return;
				}
				if (ActStop < static_cast<int>(StopM.count()-2))
				{
					if (StopM[ActStop+1]-2 < m->x())
						return;
				}
				StopM[ActStop] = m->x();
				QList<VColorStop*> cstops = fill_gradient.colorStops();
				cstops.at(ActStop)->rampPoint = newStop;
				qSort(cstops.begin(), cstops.end());
				onlyselect = false;
				repaint();
			}
			if ((Mpressed) && (outside || !insideRect.contains(m->pos())) && (ActStop > 0) && (ActStop != static_cast<int>(StopM.count()-1)))
				qApp->changeOverrideCursor(IconManager::instance()->loadCursor("DelPoint.png", 1, 1));
		}
	}
}

void GradientPreview::leaveEvent(QEvent*)
{
	if (isEditable)
	{
		if (Mpressed)
		{
			if ((ActStop > 0) && (ActStop != static_cast<int>(StopM.count()-1)))
				qApp->changeOverrideCursor(IconManager::instance()->loadCursor("DelPoint.png", 1, 1));
			else
				qApp->changeOverrideCursor(QCursor(Qt::ArrowCursor));
		}
		outside = true;
	}
}

void GradientPreview::enterEvent(QEvent*)
{
	outside = false;
}

void GradientPreview::addStop()
{
	QList<VColorStop*> cstops = fill_gradient.colorStops();
	double  newStop = static_cast<double>((mPos.x() - 10)) / (static_cast<double>(width())-20);
	QColor  stopColor = (cstops.count() > 0) ? cstops.at(0)->color : QColor(255, 255, 255);
	QString stopName  = (cstops.count() > 0) ? cstops.at(0)->name  : QString("White");
	int     stopShade = (cstops.count() > 0) ? cstops.at(0)->shade : 100;
	fill_gradient.addStop(stopColor, newStop, 0.5, 1.0, stopName, stopShade);
	repaint();
	onlyselect = false;
	cstops = fill_gradient.colorStops();
	for (int yg = 0; yg < static_cast<int>(StopM.count()); ++yg)
	{
		QRect fpo = QRect(static_cast<int>(StopM[yg])-4, 43, 8, 13);
		if (fpo.contains(mPos))
		{
			ActStop = yg;
			emit selectedStop(cstops.at(ActStop));
			repaint();
			break;
		}
	}
}

void GradientPreview::removeStop()
{
	if ((contextStop > 0) && (contextStop != static_cast<int>(StopM.count()-1)))
	{
		onlyselect = false;
		fill_gradient.removeStop(contextStop);
		ActStop = 0;
		repaint();
		QList<VColorStop*> cstops = fill_gradient.colorStops();
		emit selectedStop(cstops.at(ActStop));
	}
}

void GradientPreview::updateDisplay()
{
	repaint();
	if (!fill_gradient.colorStops().isEmpty())
	{
		QList<VColorStop*> cstops = fill_gradient.colorStops();
		emit selectedStop(cstops.at(ActStop));
	}
}

void GradientPreview::setActColor(const QColor& c, const QString& n, int s)
{
	if (ActStop == -1)
		return;
	QList<VColorStop*> cstops = fill_gradient.colorStops();
	cstops.at(ActStop)->color = c;
	cstops.at(ActStop)->name = n;
	cstops.at(ActStop)->shade = s;
	repaint();
}

void GradientPreview::setActTrans(double t)
{
	if (ActStop == -1)
		return;
	QList<VColorStop*> cstops = fill_gradient.colorStops();
	cstops.at(ActStop)->opacity = t;
	repaint();
}

void GradientPreview::setActStep(double t)
{
	if (ActStop == -1)
		return;
	QList<VColorStop*> cstops = fill_gradient.colorStops();
	cstops.at(ActStop)->rampPoint = t;
	repaint();
}

void GradientPreview::setGradient(const VGradient& gradient)
{
	if ((gradient.colorStops().count() == fill_gradient.colorStops().count()) && (ActStop >= 0))
	{
		int diffStops = 0;
		for (int i = 0; i < fill_gradient.colorStops().count(); ++i)
		{
			VColorStop* stop1 = gradient.colorStops().at(i);
			VColorStop* stop2 = fill_gradient.colorStops().at(i);
			if ((stop1->color != stop2->color) || (stop1->midPoint != stop2->midPoint) ||
				(stop1->name  != stop2->name)  || (stop1->opacity != stop2->opacity)   ||
				(stop1->rampPoint != stop2->rampPoint) || (stop1->shade != stop2->shade))
			{
				++diffStops;
			}
		}
		if (diffStops > 1)
			ActStop = 0;
	}
	if ((ActStop < 0) && (gradient.colorStops().count() > 0))
		ActStop = 0;
	if (ActStop >= gradient.colorStops().count())
		ActStop = 0;
	fill_gradient = gradient;
}

void GradientPreview::setGradientEditable(bool val)
{
	isEditable = val;
	repaint();
}
