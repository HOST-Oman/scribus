/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef WMFCONTEXT_H
#define WMFCONTEXT_H

#include <QBrush>
#include <QFont>
#include <QTransform>
#include <QPen>
#include <QPoint>
#include <QSize>
#include <QStack>
#include "fpointarray.h"

class WMFGraphicsState
{
protected:

	QPointF windowOrg;
	QSizeF  windowExt;
	QPointF viewportOrg;
	QSizeF  viewportExt;

	void updateWorldMatrix();

public:
	QFont		font;
	QPoint		position;
	QPen        pen;
	QBrush      brush;
	QColor      backgroundColor;
	Qt::BGMode  backgroundMode;
	QColor      textColor;
	int         textAlign;
	int         textCharset;
	double      textRotation;
	bool        windingFill;
	FPointArray path;
	QTransform     worldMatrix;
	
public:
	WMFGraphicsState();

	void setWindowOrg(double x, double y);
	void setWindowExt(double x, double y);
	void setViewportOrg(double x, double y);
	void setViewportExt(double x, double y);
};

class WMFContext : public QStack<WMFGraphicsState>
{
public:

	WMFContext();

	void save();
	void restore();
	void reset();

	WMFGraphicsState& current();

	QFont        font() { return current().font; }
	QPoint       position() { return current().position; }
	QPen         pen() { return current().pen; }
	QBrush       brush() { return current().brush; }
	QColor       backgroundColor() { return current().backgroundColor; }
	Qt::BGMode   backgroundMode() { return current().backgroundMode; }
	QColor       textColor() { return current().textColor; }
	int          textAlign() { return current().textAlign; }
	int          textCharSet()  { return current().textCharset; }
	double       textRotation() { return current().textRotation; }
	bool         windingFill() { return current().windingFill; }
	FPointArray& path () { return current().path; }
	QTransform&     worldMatrix () { return current().worldMatrix; }
	void setFont (const QFont& font) { current().font = font; }
	void setPosition (const QPoint& pos) { current().position = pos; }
	void setPen   (const QPen& pen) { current().pen = pen; }
	void setBrush (const QBrush& brush) { current().brush = brush; }
	void setBackgroundColor (const QColor& color) { current().backgroundColor = color; }
	void setBackgroundMode  (const Qt::BGMode& mode) { current().backgroundMode = mode; }
	void setTextColor  (const QColor& color) { current().textColor = color; }
	void setTextAlign  (int align) { current().textAlign = align; }
	void setTextCharset(int align) { current().textCharset = align; }
	void setTextRotation (double rot) { current().textRotation = rot; }
	void setWindingFill (const bool winding) { current().windingFill = winding; }
	void setPath (const FPointArray& path) { current().path = path; }
	void setWorldMatrix (const QTransform& matrix) { current().worldMatrix = matrix; }

	// window operations
	void setWindowOrg(double x, double y) { current().setWindowOrg(x, y); }
	void setWindowExt(double x, double y) { current().setWindowExt(x, y); }
	void setViewportOrg(double x, double y) { current().setViewportOrg(x, y); }
	void setViewportExt(double x, double y) { current().setViewportExt(x, y); }

	// matrix operations
	void translate (double x, double y) { current().worldMatrix.translate(x, y); }
	void scale     (double x, double y) { current().worldMatrix.scale(x, y); }
	void rotate    (double deg) { current().worldMatrix.rotate(deg); }
};


#endif
