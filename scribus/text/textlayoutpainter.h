/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */

#ifndef TEXTLAYOUTPAINTER_H
#define TEXTLAYOUTPAINTER_H

#include <stack>

#include "scribusapi.h"
#include "sctextstruct.h"


struct TextLayoutColor
{
	QString color;
	double shade;

	TextLayoutColor()
		: color("Black")
		, shade(100)
	{ }

	TextLayoutColor(QString c, double s=100)
		: color(c)
		, shade(s)
	{ }

	bool operator ==(TextLayoutColor other)
	{
		return other.color == color && other.shade == shade;
	}

	bool operator !=(TextLayoutColor other)
	{
		return !(*this == other);
	}
};

class SCRIBUS_API TextLayoutPainter
{
public:
	TextLayoutPainter();
	virtual ~TextLayoutPainter();

	virtual void setFont(const ScFace font);
	virtual const ScFace font() const;

	virtual void setFontSize(double size);
	virtual double fontSize() const;

	virtual void setStrokeColor(TextLayoutColor c);
	virtual TextLayoutColor strokeColor() const;

	virtual void setFillColor(TextLayoutColor c);
	virtual TextLayoutColor fillColor() const;

	virtual void setStrokeWidth(double w);
	virtual double strokeWidth() const;

	virtual void translate(double x, double y);
	virtual double x() const;
	virtual double y() const;

	virtual void setScale(double h, double v);
	virtual double scaleV() const;
	virtual double scaleH() const;

	virtual void setSelected(bool s);
	virtual bool selected() const;

	virtual void drawGlyph(const GlyphLayout gl) = 0;
	virtual void drawGlyphOutline(const GlyphLayout gl, bool fill) = 0;
	virtual void drawLine(QPointF start, QPointF end) = 0;
	virtual void drawRect(QRectF rect) = 0;
	virtual void drawObject(PageItem* item) = 0;

	virtual void save();
	virtual void restore();

private:
	struct State
	{
		ScFace font;
		double fontSize;
		TextLayoutColor strokeColor;
		TextLayoutColor fillColor;
		double strokeWidth;
		double x;
		double y;
		double scaleH;
		double scaleV;
		bool selected;

		State()
			: fontSize(0)
			, strokeWidth(0)
			, x(0)
			, y(0)
			, scaleH(1)
			, scaleV(1)
			, selected(false)
		{}
	};

	std::stack<State> m_stack;
};

#endif // TEXTLAYOUTPAINTER_H
