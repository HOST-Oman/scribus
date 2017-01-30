/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          fpointarray.cpp  -  description
                             -------------------
    begin                : Mit Jul 24 2002
    copyright            : (C) 2002 by Franz Schmid
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

#include "fpointarray.h"
#include <cstdarg>

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include <QRegExp>
#include <QVector>

#include "util.h"
#include "util_math.h"
#include "sclimits.h"

using namespace std;


FPointArray FPointArray::copy() const
{ 
	FPointArray tmp;
	tmp << *this;
	tmp.QVector<FPoint>::squeeze();
	return tmp; 
}


FPointArray & FPointArray::operator=( const FPointArray &a )
{ 
	QVector<FPoint>::operator=(a);
	m_svgState = NULL;
	QVector<FPoint>::squeeze();
	return *this; 
}


/* optimized for speed:
 *   never shrink
 *   when growing, try to double size
 *   if capacity permits, just increase count
 */
bool FPointArray::resize(int newCount)
{
	if (newCount <= 0)
	{
		QVector<FPoint>::resize(0);
		QVector<FPoint>::squeeze();
	}
	else
	{
		QVector<FPoint>::resize(newCount);
	}
	return true;
}

void FPointArray::reverse()
{
	FPointArray tmp;
	tmp << *this;
	tmp.QVector<FPoint>::squeeze();
	QVector<FPoint>::resize(0);
	QVector<FPoint>::squeeze();
	for (int a = 0; a < tmp.count()-1; a += 2)
	{
		QVector<FPoint>::prepend(tmp.point(a+1));
		QVector<FPoint>::prepend(tmp.point(a));
	}
}

bool FPointArray::setPoints( int nPoints, double firstx, double firsty, ... )
{
	va_list ap;
	if ( nPoints < 0 || !FPointArray::resize(nPoints) )
		return false;
	setPoint( 0, firstx, firsty );
	int i = 1;
	double x, y;
	nPoints--;
	va_start( ap, firsty );
	while ( nPoints-- )
	{
		x = static_cast<double>(va_arg( ap, double ));
		y = static_cast<double>(va_arg( ap, double ));
		setPoint( i++, x, y );
    }
	va_end( ap );
	return true;
}

bool FPointArray::putPoints( int index, int nPoints, double firstx, double firsty,  ... )
{
	va_list ap;
	if ( index + nPoints > QVector<FPoint>::count())
	{
		if ( !FPointArray::resize(index + nPoints) )
			return false;
	}
	if ( nPoints <= 0 )
		return true;
	setPoint( index, firstx, firsty );		// set first point
	int i = index + 1;
	double x, y;
	nPoints--;
	va_start( ap, firsty );
	while ( nPoints-- )
	{
		x = static_cast<double>(va_arg(ap, double));
		y = static_cast<double>(va_arg(ap, double));
		setPoint( i++, x, y );
	}
	va_end( ap );
	return true;
}

bool FPointArray::putPoints( int index, int nPoints, const FPointArray & from, int fromIndex )
{
	if ( index + nPoints > QVector<FPoint>::count() )
	{	// extend array
		if ( !FPointArray::resize(index + nPoints) )
			return false;
	}
	if ( nPoints <= 0 )
		return true;
	Iterator p = begin();
	p += index;
	ConstIterator q = from.begin();
	q += fromIndex;
	while( --nPoints >= 0 )
	{
		*p++ = *q++;
    }
	return true;
}

void FPointArray::point(int i, double *x, double *y) const
{
	const FPoint& p = QVector<FPoint>::at(i);
	if (x)
		*x = p.xp;
	if (y)
		*y = p.yp;
}


QPoint FPointArray::pointQ(int i) const
{
	const FPoint& p = QVector<FPoint>::at(i);
	return QPoint(qRound(p.xp), qRound(p.yp));
}

QPointF FPointArray::pointQF(int i) const
{
	const FPoint& p = QVector<FPoint>::at(i);
	QPointF r(p.xp, p.yp);
	return r;
}

void FPointArray::translate( double dx, double dy )
{
	FPoint pt( dx, dy );
	Iterator pend = begin();
	pend += QVector<FPoint>::count();
	for (Iterator p = begin(); p != pend; p++)
	{
		if (!isMarkerI(p))
			*p += pt;
	}
}

void FPointArray::scale( double sx, double sy )
{
	Iterator pend = begin();
	pend += QVector<FPoint>::count();
	for (Iterator p = begin(); p != pend; p++)
	{
		if (!isMarkerI(p))
			p->setXY(p->x() * sx, p->y() * sy);
	}
}

QRectF FPointArray::boundingRect()
{
	FPoint min = getMinClipF(this);
	FPoint max = getMaxClipF(this);
	return QRectF(QPointF(min.x(), min.y()), QPointF(max.x(), max.y()));
}

FPoint FPointArray::WidthHeight() const
{
	if ( QVector<FPoint>::count() == 0 )
		return FPoint( 0.0, 0.0 );		// null rectangle
	ConstIterator pd = begin();
	ConstIterator pend = begin();
	pend += QVector<FPoint>::count();
	double minx, maxx, miny, maxy;
	minx = maxx = pd->xp;
	miny = maxy = pd->yp;
	for ( ++pd; pd != pend; ++pd )
	{	// find min+max x and y
		if (isMarkerI(pd))
		{
			continue;
		}
		if ( pd->xp < minx )
			minx = pd->xp;
		else
			if ( pd->xp > maxx )
		    	maxx = pd->xp;
		if ( pd->y() < miny )
			miny = pd->yp;
		else
			if ( pd->yp > maxy )
	    		maxy = pd->yp;
    }
	return FPoint(maxx - minx,maxy - miny);
}

void FPointArray::map( QTransform m )
{
	const double m11 = m.m11();
	const double m12 = m.m12();
	const double m21 = m.m21();
	const double m22 = m.m22();
	const double dx  = m.dx();
	const double dy  = m.dy();
	double mx, my;
	Iterator pend = begin();
	pend += QVector<FPoint>::count();
	for (Iterator p = begin(); p != pend; p++)
	{
		if (isMarkerD(p->xp, p->yp))
		{
			mx = p->xp;
			my = p->yp;
		}
		else
		{
			mx = m11 * p->xp + m21 * p->yp + dx;
			my = m22 * p->yp + m12 * p->xp + dy;
		}
		p->xp = mx;
		p->yp = my;
	}
}

void FPointArray::setMarker()
{
	double maxVal = std::numeric_limits<double>::max() / 2.0;
	addQuadPoint(maxVal, maxVal, maxVal, maxVal, maxVal, maxVal, maxVal, maxVal);
}

bool FPointArray::isMarker(int pos) const
{
	double maxVal = std::numeric_limits<double>::max() / 3.0;
	const FPoint& p = QVector<FPoint>::at(pos);
	return ((p.x() >= maxVal) && (p.y() >= maxVal));
}

bool FPointArray::isMarkerI(ConstIterator p) const
{
	double maxVal = std::numeric_limits<double>::max() / 3.0;
	return ((p->xp >= maxVal) && (p->yp >= maxVal));
}

bool FPointArray::isMarkerD(double x, double y) const
{
	double maxVal = std::numeric_limits<double>::max() / 3.0;
	return ((x >= maxVal) && (y >= maxVal));
}

void FPointArray::addPoint(double x, double y)
{
	QVector<FPoint>::append(FPoint(x, y));
}

void FPointArray::addPoint(FPoint p)
{
	QVector<FPoint>::append(p);
}


bool FPointArray::hasLastQuadPoint(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) const
{
	int i = QVector<FPoint>::count()-4;
	if (i < 0)
		return false;
	ConstIterator p = begin();
	p += i;
	if (p->xp != x1 || p->yp != y1)
		return false;
	++p; 
	if (p->xp != x2 || p->yp != y2)
		return false;
	++p; 
	if (p->xp != x3 || p->yp != y3)
		return false;
	++p; 
	if (p->xp != x4 || p->yp != y4)
		return false;
	
	return true;
}

void FPointArray::addQuadPoint(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
	QVector<FPoint>::append(FPoint(x1, y1));
	QVector<FPoint>::append(FPoint(x2, y2));
	QVector<FPoint>::append(FPoint(x3, y3));
	QVector<FPoint>::append(FPoint(x4, y4));
}

void FPointArray::addQuadPoint(FPoint p1, FPoint p2, FPoint p3, FPoint p4)
{
	QVector<FPoint>::append(p1);
	QVector<FPoint>::append(p2);
	QVector<FPoint>::append(p3);
	QVector<FPoint>::append(p4);
}

double FPointArray::lenPathSeg(int seg) const
{
	FPoint p1 = point(seg);
	FPoint k1 = point(seg+1);
	FPoint p2 = point(seg+2);
	FPoint k2 = point(seg+3);
	FPoint newP, oldP;
	double newLen = 1;
	double oldLen = 0;
	double ts = 0.5;
	double t = 0.5;
	int iter = 2;
	while (true)
	{
		oldP = p1;
		newLen = 0;
		for (int dx = 0; dx < iter; ++dx)
		{
			double tm = 1.0 - t;
			newP = ((tm * tm * tm) * p1) + (3 * t * (tm * tm) * k1) + (3 * t * t * tm * k2 + t * t * t * p2);
			newLen += sqrt(pow(newP.x()-oldP.x(),2.0)+pow(newP.y()-oldP.y(),2.0));
			oldP = newP;
			t += ts;
		}
		if (fabs(newLen - oldLen) < 0.01)
			break;
		oldLen = newLen;
		ts /= 2.0;
		iter *= 2;
		t = ts;
	}
	return newLen;
}

double FPointArray::lenPathDist(int seg, double t1, double t2) const
{
	FPoint p1 = point(seg);
	FPoint k1 = point(seg+1);
	FPoint p2 = point(seg+2);
	FPoint k2 = point(seg+3);
	FPoint newP, oldP;
	double newLen = 0;
	double ts, t, tm;
	tm = 1.0 - t1;
	oldP = ((tm * tm * tm) * p1) + (3 * t1 * (tm * tm) * k1) + (3 * t1 * t1 * tm * k2 + t1 * t1 * t1 * p2);
	ts = (t2 - t1) / 100;
	t = t1 + ts;
	for (int dx = 0; dx < 99; ++dx)
	{
		tm = 1.0 - t;
		newP = ((tm * tm * tm) * p1) + (3 * t * (tm * tm) * k1) + (3 * t * t * tm * k2 + t * t * t * p2);
		newLen += sqrt(pow(newP.x()-oldP.x(),2.0)+pow(newP.y()-oldP.y(),2.0));
		oldP = newP;
		t += ts;
	}
	return newLen;
}

void FPointArray::pointTangentNormalAt( int seg, double t, FPoint* p, FPoint* tn, FPoint* n ) const
{
	// Calculate derivative if necessary.
	FPoint d;
	if( tn || n )
		pointDerivativesAt( seg, t, p, &d, 0L );
	else
		pointDerivativesAt( seg, t, p, 0L, 0L );
	// Normalize derivative.
	if( tn || n )
	{
		const double norm = sqrt( d.x() * d.x() + d.y() * d.y() );
		d = norm ? d * ( 1.0 / norm ) : FPoint( 0.0, 0.0 );
	}
	// Assign tangent vector.
	if( tn )
		*tn = d;
	// Calculate normal vector.
	if( n )
	{
		// Calculate vector product of "binormal" x tangent
		// (0,0,1) x (dx,dy,0), which is simply (dy,-dx,0).
		n->setX( d.y() );
		n->setY( -d.x() );
	}
	FPoint p1 = point(seg);
	FPoint k1 = point(seg+1);
	FPoint p2 = point(seg+2);
	FPoint k2 = point(seg+3);
	double tm = 1.0 - t;
	*p = ((tm * tm * tm) * p1) + (3 * t * (tm * tm) * k1) + (3 * (t * t) * tm * k2 + (t * t * t) * p2);
}

void FPointArray::pointDerivativesAt( int seg, double t, FPoint* p, FPoint* d1, FPoint* d2 ) const
{
	// Copy points.
	FPoint* q = new FPoint[ 4 ];
	q[ 0 ] = point(seg);
	q[ 1 ] = point(seg+1);
	q[ 3 ] = point(seg+2);
	q[ 2 ] = point(seg+3);
	// The De Casteljau algorithm.
	for( unsigned short j = 1; j <= 3; j++ )
	{
		for( unsigned short i = 0; i <= 3 - j; i++ )
		{
			q[ i ] = ( 1.0 - t ) * q[ i ] + t * q[ i + 1 ];
		}
		// Save second derivative now that we have it.
		if( j == 1 )
		{
			if( d2 )
				*d2 = 6 * ( q[ 2 ] - 2 * q[ 1 ] + q[ 0 ] );
		}
		// Save first derivative now that we have it.
		else if( j == 2 )
		{
			if( d1 )
				*d1 = 3 * ( q[ 1 ] - q[ 0 ] );
		}
	}
	// Save point.
	if( p )
		*p = q[ 0 ];
	delete[]( q );
	return;
}


struct SVGState
{
	double CurrX, CurrY, StartX, StartY;
	bool FirstM, WasM, PathClosed;
	int PathLen;
	
	void reset(double x, double y)
	{
		CurrX = x;
		CurrY = y;
		StartX = x;
		StartY = y;
		PathLen = 0;
	}
	
	void move(double x, double y, int newPoints)
	{
		CurrX = x;
		CurrY = y;
		PathLen += newPoints;
	}
	
	bool needsMarker()
	{
		bool result = (!FirstM) && (WasM);
		if (result)
			PathLen += 4;
		return result;
	}
};


QString FPointArray::svgPath(bool closed) const
{
	QString tmp = "";
	FPoint np, np1, np2, np3, np4, firstP;
	bool nPath = true;
	bool first = true;
	if (size() > 3)
	{
		for (int poi=0; poi < size()-3; poi += 4)
		{
			if (isMarker(poi))
			{
				nPath = true;
				continue;
			}
			if (nPath)
			{
				np = point(poi);
				if ((!first) && (closed) && (np4 == firstP))
					tmp += "Z ";
				tmp += "M"+QString::number(np.x())+" "+QString::number(np.y())+" ";
				nPath = false;
				first = false;
				firstP = np;
				np4 = np;
			}
			np = point(poi);
			np1 = point(poi+1);
			np2 = point(poi+3);
			np3 = point(poi+2);
			if ((np == np1) && (np2 == np3))
				tmp += QString("L%1 %2 ").arg(np3.x()).arg(np3.y());
			else
				tmp += QString("C%1 %2 %3 %4 %5 %6 ").arg(np1.x()).arg(np1.y()).arg(np2.x()).arg(np2.y()).arg(np3.x()).arg(np3.y());
			np4 = np3;
		}
		if (closed)
			tmp += "Z";
	}
	return tmp;
}

QPainterPath FPointArray::toQPainterPath(bool closed)
{
	QPainterPath m_path = QPainterPath();
	bool nPath = true;
	bool first = true;
	FPoint np, np1, np2, np3, np4, firstP;
	if (size() > 3)
	{
		for (int poi = 0; poi < size()-3; poi += 4)
		{
			if (isMarker(poi))
			{
				nPath = true;
				continue;
			}
			if (nPath)
			{
				np = point(poi);
				if ((!first) && (closed) && (np4 == firstP))
					m_path.closeSubpath();
				m_path.moveTo(np.x(), np.y());
				nPath = false;
				first = false;
				firstP = np;
				np4 = np;
			}
			np = point(poi);
			np1 = point(poi+1);
			np2 = point(poi+3);
			np3 = point(poi+2);
			if ((np == np1) && (np2 == np3))
				m_path.lineTo(np3.x(), np3.y());
			else
				m_path.cubicTo(np1.x(), np1.y(), np2.x(), np2.y(), np3.x(), np3.y());
			np4 = np3;
		}
		if (closed)
			m_path.closeSubpath();
	}
	return m_path;
}

void FPointArray::fromQPainterPath(QPainterPath &path, bool close)
{
	resize(0);
	svgInit();
	for (int i = 0; i < path.elementCount(); ++i)
	{
		const QPainterPath::Element &elm = path.elementAt(i);
		switch (elm.type)
		{
			case QPainterPath::MoveToElement:
				if (m_svgState->WasM)
					svgClosePath();
				m_svgState->WasM = true;
				svgMoveTo(elm.x, elm.y);
				break;
			case QPainterPath::LineToElement:
				svgLineTo(elm.x, elm.y);
				break;
			case QPainterPath::CurveToElement:
				svgCurveToCubic(elm.x, elm.y, path.elementAt(i+1).x, path.elementAt(i+1).y, path.elementAt(i+2).x, path.elementAt(i+2).y );
				break;
			default:
				break;
		}
	}
	if (close)
		svgClosePath();
}

FPointArray::~FPointArray()
{
	if (m_svgState)
		delete m_svgState;
}


void FPointArray::svgInit()
{
	if (!m_svgState)
		m_svgState = new SVGState;
	m_svgState->reset(0,0);
	m_svgState->FirstM = true;
	m_svgState->WasM = false;
}


void FPointArray::svgMoveTo(double x, double y)
{
	if (!m_svgState)
		return;
	m_svgState->reset(x, y);
	m_svgState->WasM = true;
}


void FPointArray::svgLineTo(double x1, double y1)
{
	if (!m_svgState)
		return;
	if (m_svgState->needsMarker())
		setMarker();
	m_svgState->FirstM = false;
	m_svgState->WasM = false;
	if (size() > 3)
	{
		FPoint b1 = point(size()-4);
		FPoint b2 = point(size()-3);
		FPoint b3 = point(size()-2);
		FPoint b4 = point(size()-1);
		FPoint n1 = FPoint(m_svgState->CurrX, m_svgState->CurrY);
		FPoint n2 = FPoint(x1, y1);
		if ((b1 == n1) && (b2 == n1) && (b3 == n2) && (b4 == n2))
			return;
	}
	addPoint(FPoint(m_svgState->CurrX, m_svgState->CurrY));
	addPoint(FPoint(m_svgState->CurrX, m_svgState->CurrY));
	addPoint(FPoint(x1, y1));
	addPoint(FPoint(x1, y1));
	m_svgState->move(x1, y1, 4);
}


void FPointArray::svgCurveToCubic(double x1, double y1, double x2, double y2, double x3, double y3)
{
	if (!m_svgState)
		return;
	if (m_svgState->needsMarker())
		setMarker();
	m_svgState->FirstM = false;
	m_svgState->WasM = false;
	if (m_svgState->PathLen > 3)
	{
		FPoint b1 = point(size()-4);
		FPoint b2 = point(size()-3);
		FPoint b3 = point(size()-2);
		FPoint b4 = point(size()-1);
		FPoint n1 = FPoint(m_svgState->CurrX, m_svgState->CurrY);
		FPoint n2 = FPoint(x1, y1);
		FPoint n3 = FPoint(x3, y3);
		FPoint n4 = FPoint(x2, y2);
		if ((b1 == n1) && (b2 == n2) && (b3 == n3) && (b4 == n4))
			return;
	}
	addPoint(FPoint(m_svgState->CurrX, m_svgState->CurrY));
	addPoint(FPoint(x1, y1));
	addPoint(FPoint(x3, y3));
	addPoint(FPoint(x2, y2));
	m_svgState->move(x3, y3, 4);
}


void FPointArray::svgClosePath()
{
	if (!m_svgState)
		return;
	if (m_svgState->PathLen > 2)
	{
		if ((m_svgState->PathLen == 4) || (point(size()-2).x() != m_svgState->StartX) || (point(size()-2).y() != m_svgState->StartY))
		{
			addPoint(point(size()-2));
			addPoint(point(size()-3));
			addPoint(FPoint(m_svgState->StartX, m_svgState->StartY));
			addPoint(FPoint(m_svgState->StartX, m_svgState->StartY));
		}
	}
}

void FPointArray::svgArcTo(double r1, double r2, double angle, bool largeArcFlag, bool sweepFlag, double x1, double y1)
{
	if (!m_svgState)
		return;
	calculateArc(false, m_svgState->CurrX, m_svgState->CurrY, angle, x1, y1, r1, r2, largeArcFlag, sweepFlag);
}

void FPointArray::calculateArc(bool relative, double &curx, double &cury, double angle, 
							   double x, double y, double r1, double r2, bool largeArcFlag, bool sweepFlag)
{
	double sin_th, cos_th;
	double a00, a01, a10, a11;
	double x0, y0, x1, y1, xc, yc;
	double d, sfactor, sfactor_sq;
	double th0, th1, th_arc;
	int i, n_segs;
	sin_th = sin(angle * (M_PI / 180.0));
	cos_th = cos(angle * (M_PI / 180.0));
	double dx;
	if(!relative)
		dx = (curx - x) / 2.0;
	else
		dx = -x / 2.0;
	double dy;
	if(!relative)
		dy = (cury - y) / 2.0;
	else
		dy = -y / 2.0;
	double _x1 =  cos_th * dx + sin_th * dy;
	double _y1 = -sin_th * dx + cos_th * dy;
	double Pr1 = r1 * r1;
	double Pr2 = r2 * r2;
	double Px = _x1 * _x1;
	double Py = _y1 * _y1;
	// Spec : check if radii are large enough
	double check = Px / Pr1 + Py / Pr2;
	if(check > 1)
	{
		r1 = r1 * sqrt(check);
		r2 = r2 * sqrt(check);
	}
	a00 = cos_th / r1;
	a01 = sin_th / r1;
	a10 = -sin_th / r2;
	a11 = cos_th / r2;
	x0 = a00 * curx + a01 * cury;
	y0 = a10 * curx + a11 * cury;
	if(!relative)
		x1 = a00 * x + a01 * y;
	else
		x1 = a00 * (curx + x) + a01 * (cury + y);
	if(!relative)
		y1 = a10 * x + a11 * y;
	else
		y1 = a10 * (curx + x) + a11 * (cury + y);
	/* (x0, y0) is current point in transformed coordinate space.
		(x1, y1) is new point in transformed coordinate space.
		
		The arc fits a unit-radius circle in this space.
	    */
	d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
	sfactor_sq = 1.0 / d - 0.25;
	if(sfactor_sq < 0)
		sfactor_sq = 0;
	sfactor = sqrt(sfactor_sq);
	if(sweepFlag == largeArcFlag)
		sfactor = -sfactor;
	xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
	yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
	
	/* (xc, yc) is center of the circle. */
	th0 = atan2(y0 - yc, x0 - xc);
	th1 = atan2(y1 - yc, x1 - xc);
	th_arc = th1 - th0;
	if(th_arc < 0 && sweepFlag)
		th_arc += 2 * M_PI;
	else if(th_arc > 0 && !sweepFlag)
		th_arc -= 2 * M_PI;
	n_segs = static_cast<int>(ceil(fabs(th_arc / (M_PI * 0.5 + 0.001))));
	for(i = 0; i < n_segs; i++)
	{
	{
		double sin_th, cos_th;
		double a00, a01, a10, a11;
		double x1, y1, x2, y2, x3, y3;
		double t;
		double th_half;
		double _th0 = th0 + i * th_arc / n_segs;
		double _th1 = th0 + (i + 1) * th_arc / n_segs;
		sin_th = sin(angle * (M_PI / 180.0));
		cos_th = cos(angle * (M_PI / 180.0));
		/* inverse transform compared with rsvg_path_arc */
		a00 = cos_th * r1;
		a01 = -sin_th * r2;
		a10 = sin_th * r1;
		a11 = cos_th * r2;
		th_half = 0.5 * (_th1 - _th0);
		t = (8.0 / 3.0) * sin(th_half * 0.5) * sin(th_half * 0.5) / sin(th_half);
		x1 = xc + cos(_th0) - t * sin(_th0);
		y1 = yc + sin(_th0) + t * cos(_th0);
		x3 = xc + cos(_th1);
		y3 = yc + sin(_th1);
		x2 = x3 + t * sin(_th1);
		y2 = y3 - t * cos(_th1);
		svgCurveToCubic(a00 * x1 + a01 * y1, a10 * x1 + a11 * y1, a00 * x2 + a01 * y2, a10 * x2 + a11 * y2, a00 * x3 + a01 * y3, a10 * x3 + a11 * y3 );
	}
	}
	if(!relative)
		curx = x;
	else
		curx += x;
	if(!relative)
		cury = y;
	else
		cury += y;
}


static const char * getCoord( const char *ptr, double &number )
{
	int integer, exponent;
	double decimal, frac;
	int sign, expsign;
	
	exponent = 0;
	integer = 0;
	frac = 1.0;
	decimal = 0;
	sign = 1;
	expsign = 1;
	
	// read the sign
	if(*ptr == '+')
		ptr++;
	else if(*ptr == '-')
	{
		ptr++;
		sign = -1;
	}
	
	// read the integer part
	while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
		integer = (integer * 10) + *(ptr++) - '0';
	if(*ptr == '.') // read the decimals
	{
		ptr++;
		while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
			decimal += (*(ptr++) - '0') * (frac *= 0.1);
	}
	
	if(*ptr == 'e' || *ptr == 'E') // read the exponent part
	{
		ptr++;
		
		// read the sign of the exponent
		if(*ptr == '+')
			ptr++;
		else if(*ptr == '-')
		{
			ptr++;
			expsign = -1;
		}
		
		exponent = 0;
		while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
		{
			exponent *= 10;
			exponent += *ptr - '0';
			ptr++;
		}
	}
	number = integer + decimal;
	number *= sign * pow( static_cast<double>(10), static_cast<double>( expsign * exponent ) );
	// skip the following space
	if(*ptr == ' ')
		ptr++;
	
	return ptr;
}


bool FPointArray::parseSVG(const QString& svgPath)
{
	QString d = svgPath;
	d = d.replace( QRegExp( "," ), " ");
	bool ret = false;
	if (!d.isEmpty())
	{
		d = d.simplified();
		QByteArray pathData = d.toLatin1();
		const char *ptr = pathData.constData();
		const char *end = pathData.constData() + pathData.length() + 1;
		double contrlx, contrly, curx, cury, subpathx, subpathy, tox, toy, x1, y1, x2, y2, xc, yc;
		double px1, py1, px2, py2, px3, py3;
		bool relative;
		int moveCount = 0;
		svgInit();
		char command = *(ptr++), lastCommand = ' ';
		subpathx = subpathy = curx = cury = contrlx = contrly = 0.0;
		while (ptr < end)
		{
			if (*ptr == ' ')
				ptr++;
			relative = false;
			switch (command)
			{
			case 'f':
			case 'F':
				{
					ptr = getCoord( ptr, tox );
					break;
				}
			case 'm':
				relative = true;
			case 'M':
				{
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );
					m_svgState->WasM = true;
					subpathx = curx = relative ? curx + tox : tox;
					subpathy = cury = relative ? cury + toy : toy;
					svgMoveTo(curx, cury );
					moveCount++;
					break;
				}
			case 'l':
				relative = true;
			case 'L':
				{
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					svgLineTo( curx, cury );
					break;
				}
			case 'h':
				{
					ptr = getCoord( ptr, tox );
					curx = curx + tox;
					svgLineTo( curx, cury );
					break;
				}
			case 'H':
				{
					ptr = getCoord( ptr, tox );
					curx = tox;
					svgLineTo( curx, cury );
					break;
				}
			case 'v':
				{
					ptr = getCoord( ptr, toy );
					cury = cury + toy;
					svgLineTo( curx, cury );
					break;
				}
			case 'V':
				{
					ptr = getCoord( ptr, toy );
					cury = toy;
					svgLineTo(  curx, cury );
					break;
				}
			case 'z':
			case 'Z':
				{
					curx = subpathx;
					cury = subpathy;
					svgClosePath();
					break;
				}
			case 'c':
				relative = true;
			case 'C':
				{
					ptr = getCoord( ptr, x1 );
					ptr = getCoord( ptr, y1 );
					ptr = getCoord( ptr, x2 );
					ptr = getCoord( ptr, y2 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );
					px1 = relative ? curx + x1 : x1;
					py1 = relative ? cury + y1 : y1;
					px2 = relative ? curx + x2 : x2;
					py2 = relative ? cury + y2 : y2;
					px3 = relative ? curx + tox : tox;
					py3 = relative ? cury + toy : toy;
					svgCurveToCubic( px1, py1, px2, py2, px3, py3 );
					contrlx = relative ? curx + x2 : x2;
					contrly = relative ? cury + y2 : y2;
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
			case 's':
				relative = true;
			case 'S':
				{
					ptr = getCoord( ptr, x2 );
					ptr = getCoord( ptr, y2 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );
					px1 = 2 * curx - contrlx;
					py1 = 2 * cury - contrly;
					px2 = relative ? curx + x2 : x2;
					py2 = relative ? cury + y2 : y2;
					px3 = relative ? curx + tox : tox;
					py3 = relative ? cury + toy : toy;
					svgCurveToCubic( px1, py1, px2, py2, px3, py3 );
					contrlx = relative ? curx + x2 : x2;
					contrly = relative ? cury + y2 : y2;
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
			case 'q':
				relative = true;
			case 'Q':
				{
					ptr = getCoord( ptr, x1 );
					ptr = getCoord( ptr, y1 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );
					px1 = relative ? (curx + 2 * (x1 + curx)) * (1.0 / 3.0) : (curx + 2 * x1) * (1.0 / 3.0);
					py1 = relative ? (cury + 2 * (y1 + cury)) * (1.0 / 3.0) : (cury + 2 * y1) * (1.0 / 3.0);
					px2 = relative ? ((curx + tox) + 2 * (x1 + curx)) * (1.0 / 3.0) : (tox + 2 * x1) * (1.0 / 3.0);
					py2 = relative ? ((cury + toy) + 2 * (y1 + cury)) * (1.0 / 3.0) : (toy + 2 * y1) * (1.0 / 3.0);
					px3 = relative ? curx + tox : tox;
					py3 = relative ? cury + toy : toy;
					svgCurveToCubic( px1, py1, px2, py2, px3, py3 );
					contrlx = relative ? curx + x1 : (tox + 2 * x1) * (1.0 / 3.0);
					contrly = relative ? cury + y1 : (toy + 2 * y1) * (1.0 / 3.0);
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
			case 't':
				relative = true;
			case 'T':
				{
					ptr = getCoord(ptr, tox);
					ptr = getCoord(ptr, toy);
					xc = 2 * curx - contrlx;
					yc = 2 * cury - contrly;
					px1 = relative ? (curx + 2 * xc) * (1.0 / 3.0) : (curx + 2 * xc) * (1.0 / 3.0);
					py1 = relative ? (cury + 2 * yc) * (1.0 / 3.0) : (cury + 2 * yc) * (1.0 / 3.0);
					px2 = relative ? ((curx + tox) + 2 * xc) * (1.0 / 3.0) : (tox + 2 * xc) * (1.0 / 3.0);
					py2 = relative ? ((cury + toy) + 2 * yc) * (1.0 / 3.0) : (toy + 2 * yc) * (1.0 / 3.0);
					px3 = relative ? curx + tox : tox;
					py3 = relative ? cury + toy : toy;
					svgCurveToCubic( px1, py1, px2, py2, px3, py3 );
					contrlx = xc;
					contrly = yc;
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
			case 'a':
				relative = true;
			case 'A':
				{
					bool largeArc, sweep;
					double angle, rx, ry;
					ptr = getCoord( ptr, rx );
					ptr = getCoord( ptr, ry );
					ptr = getCoord( ptr, angle );
					ptr = getCoord( ptr, tox );
					largeArc = tox == 1;
					ptr = getCoord( ptr, tox );
					sweep = tox == 1;
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );
					calculateArc( relative, curx, cury, angle, tox, toy, rx, ry, largeArc, sweep );
				}
			}
			lastCommand = command;
			if (*ptr == '+' || *ptr == '-' || *ptr == '.' || (*ptr >= '0' && *ptr <= '9'))
			{
				// there are still coords in this command
				if (command == 'M')
					command = 'L';
				else if (command == 'm')
					command = 'l';
			}
			else
				command = *(ptr++);

			if (lastCommand != 'C' && lastCommand != 'c' &&
			        lastCommand != 'S' && lastCommand != 's' &&
			        lastCommand != 'Q' && lastCommand != 'q' &&
			        lastCommand != 'T' && lastCommand != 't')
			{
				contrlx = curx;
				contrly = cury;
			}
		}
		if (((lastCommand != 'z') && (lastCommand != 'Z')) || (moveCount > 1))
			ret = true;
		if (size() > 2)
		{
			if ((point(0).x() == point(size()-2).x()) && (point(0).y() == point(size()-2).y()) && (moveCount == 1))
				ret = false;
		}
	}
	return ret;
}

