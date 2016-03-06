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

#ifndef TEXTLAYOUT_H
#define TEXTLAYOUT_H

#include <QList>

#include "scribusapi.h"

#include "fpoint.h"
#include "frect.h"
#include "scpainter.h"
#include "sctextstruct.h"

class StoryText;
class Box;
class GroupBox;
class LineBox;

struct PathData
{
	float PtransX;
	float PtransY;
	float PRot;
	float PDx;
};

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
};

class SCRIBUS_API TextLayoutPainter
{
public:
	TextLayoutPainter() { }
	virtual ~TextLayoutPainter();

	virtual void setFont(const ScFace font);
	virtual ScFace font();

	virtual void setFontSize(double size);
	virtual double fontSize();

	virtual void setStrokeColor(TextLayoutColor c);
	virtual TextLayoutColor strokeColor();

	virtual void setFillColor(TextLayoutColor c);
	virtual TextLayoutColor fillColor();

	virtual void setStrokeWidth(double w);
	virtual double strokeWidth();

	virtual void translate(double x, double y);
	virtual double x();
	virtual double y();

	virtual void drawGlyph(const GlyphLayout gl, bool selected) = 0;
	virtual void drawGlyphOutline(const GlyphLayout gl, bool fill, bool selected) = 0;
	virtual void drawLine(QPointF start, QPointF end) = 0;
	virtual void drawRect(QRectF rect) = 0;
	virtual void drawObject(PageItem* item) = 0;

	virtual void save();
	virtual void restore();
	virtual void scale(double h, double v);
	virtual double getScaleV();
	virtual double getScaleH();

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
		double scaleV;
		double scaleH;

		State()
			: fontSize(0)
			, strokeWidth(0)
			, x(0)
			, y(0)
		{}
	};

	State m_state;
	QStack<State> m_stack;
};

/**
	This class manages the physical layout of a textframe, ie. its line 
	structure and the lines' glyph layouts. It will use some of the layouters above to create a Box.
    It listens to change events from the StoryText and the PageItem and will update the layout accordingly.
 */
class SCRIBUS_API TextLayout
{
public:
	TextLayout(StoryText* text, PageItem* frame);
	~TextLayout();
	
	bool overflows() const;
	
	StoryText* story() { return m_story; }
	const StoryText* story() const { return m_story; }
	void setStory(StoryText* story);
	void render(TextLayoutPainter *p, const StoryText& text);
	int startOfLine(int pos) const;
	int endOfLine(int pos) const;
	int prevLine(int pos) const;
	int nextLine(int pos) const;
	int startOfFrame() const;
	int endOfFrame() const;

	int screenToPosition(FPoint coord) const;
 	FRect boundingBox(int pos, uint len = 1) const;

	uint lines() const;
	
	const LineBox*  line(uint i) const;
	const Box* box() const;
	Box* box();
	const PathData& point(int pos) const;
	PathData& point(int pos);

	void appendLine(LineBox* ls);
	void removeLastLine ();

	void clear();

protected:
	friend class FrameControl;
	
	StoryText* m_story;
    PageItem* m_frame;
	GroupBox* m_box;
	
	QVector<PathData> m_path;
	bool m_validLayout;
	mutable qreal m_magicX;
	mutable int m_lastMagicPos;

};

#endif
