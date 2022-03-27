//
// C++ Implementation: sccursor
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "scresizecursor.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPen>
#include <QBrush>

#define SCCURSORMAXANGLE 360

QMap<unsigned int, QCursor> ScResizeCursor::m_cursors;

ScResizeCursor::ScResizeCursor()
{
	initCursorDb(m_currIndex);
}

ScResizeCursor::ScResizeCursor(double rotation)
{
	int iRot(qRound(rotation));
	if (iRot < 0)
		m_currIndex = SCCURSORMAXANGLE + (iRot % SCCURSORMAXANGLE);
	else
		m_currIndex = iRot % SCCURSORMAXANGLE ;
	
	initCursorDb(m_currIndex);
}

ScResizeCursor::operator const QCursor &() const
{
	return m_cursors[m_currIndex];
}

void ScResizeCursor::initCursorDb(int idx)
{
	if (m_cursors.contains(idx))
		return;
	
	QRect rect(0, 0, 32, 32);
	int ww(rect.width());
	int hh(rect.height());
	int uu(ww / 32 );
	QPen pen(QColor(232, 232, 232), 1);
	QBrush brush(QColor(32, 32, 32));
	
	// the double-arrow
	// we build it around (0,0) to ease rotations
	// TODO design a _nice_ cursor
	
	QPainterPath path;
	double sext(12);
	double pext(6);
	double pint(6);
	double sint(1);
/*
		  /\			^
		 /  \			.
		/    \			.
	       /      \			.
	      /        \		.
	     /          \		.
	    /            \		s
	   /              \		e
	  /_______   ______\		x
		 |   |			t	^
		 |   |			.	.
		 |   |			.	.
		 |   |			.	p
		 |   |			.	e
		 |   |			.	x
		 |   |			.	t
		 |   |			.	.
		 | | |			.	.
	--------------------------	v	v
		 <a>
	 <⋅⋅⋅⋅b⋅⋅⋅⋅>

	 a/ sint
	 b/ pint
*/
	
	path.moveTo(0, -sext * uu);
	path.lineTo(pint * uu, -pext * uu);
	path.lineTo(sint * uu, -pext * uu);
	path.lineTo(sint * uu, pext * uu);
	path.lineTo(pint * uu, pext * uu);
	path.lineTo(0, sext * uu);
	path.lineTo(-pint * uu, pext * uu);
	path.lineTo(-sint * uu, pext * uu);
	path.lineTo(-sint * uu,-pext * uu);
	path.lineTo(-pint * uu,-pext * uu);
	path.lineTo(0, -sext * uu);
	
	int rot(idx);
	QPixmap pxm(rect.size());
	pxm.fill(QColor (255, 255, 255, 0 ));
		
	QPainter painter(&pxm);
	if((idx != 0)
//		&& (idx != 45)
		&& (idx != 90)
//		&& (idx != 135)
		&& (idx != 180)
//		&& (idx != 225)
		&& (idx != 270)
//		&& (idx != 315)
		&& (idx != 360)
		)
		painter.setRenderHint(QPainter::Antialiasing, true);
	painter.translate(ww / 2, ww / 2);
	painter.rotate(rot);
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawPath(path);
		
	m_cursors[rot] = QCursor(pxm, ww / 2, hh / 2);
}
