//
//  boxes.h
//  Scribus
//
//  Created by Andreas Vox on 23.05.15.
//
//

#ifndef __Scribus__boxes__
#define __Scribus__boxes__

#include "fpoint.h"
#include "frect.h"
#include "sctextstruct.h"
#include "scpainter.h"

class StoryText;
class TextLayoutPainter;

/**
 class Box has a similar role as TeX's boxes. Scribus packs glyph runs into GlyphBoxes, GlyphBoxes and InlineBoxes into LineBoxes and LineBoxes into GroupBox(T_Block). (and in the future: math atoms, tables & table cells, ...)
 */
class Box {
public:
	enum BoxType {
		T_Invalid,
		T_Line,
		T_Block,
		T_Glyphs,
		T_Path,
		T_Object
	};
	
protected:
	BoxType m_type;
	qreal m_x;
	qreal m_y;
	qreal m_width;
	qreal m_descent;
	qreal m_ascent;
	QList<Box*> m_boxes;
	int m_firstChar;
	int m_lastChar;
	
public:
	Box()
	{
		m_type = T_Invalid;
		m_x = m_y = m_width = m_ascent = m_descent = 0;
		m_firstChar = 0;
		m_lastChar = 0;
	}
	virtual ~Box() {}
	
//	virtual GlyphBox* asGlyphBox() { return NULL; }
//	virtual const BoxGroup* asBoxGroup()  const { return NULL; }
//	virtual InlineBox* asInlineBox() { return NULL; }
//	virtual PathBox* asPathBox() { return NULL; }
	
	qreal x() const { return m_x; }
	qreal y() const { return m_y; }
	void moveTo (double x, double y) { m_x = x, m_y = y; }
	void moveBy (double x, double y) { m_x += x, m_y += y; }

	qreal width() const { return m_width; }
	void addWidth(double w) { m_width += w; }
	void setWidth(double w) { m_width = w; }

	qreal height() const { return m_ascent - m_descent; }
	void setHeight(double h, double vBase) { m_ascent = h * (1-vBase); m_descent = h * vBase; }

	qreal ascent() const { return m_ascent; }
	qreal descent() const { return m_descent; }
	void setAscent(double a) { m_ascent = a; }
	void setDescent(double d) { m_descent = d; }

	QRectF bbox() const { return QRectF(m_x, m_y, m_width, height()); }
	/// returns the bounding box relative to (m_x, m_y)
	virtual FRect boundingBox(int pos, uint len = 1) const = 0;

	virtual bool containsPoint(FPoint coord) const { return bbox().contains(coord.toQPointF()); }
	bool containsPos(int pos) const { return firstChar() <= pos && pos <= lastChar(); }
	/// returns a char position for the point coord + (m_x, m_y)
	virtual int pointToPosition(FPoint coord) const = 0;

	int firstChar() const { return m_firstChar; }
	int lastChar() const { return m_lastChar; }

//	virtual QList<const Box*> pathForPos(int pos) const = 0;
//	virtual void justify(const ParagraphStyle& style) {}

	QList<Box*>& boxes() { return m_boxes; }
	const QList<const Box*>& boxes() const {
		return reinterpret_cast<const QList<const Box*> & > (m_boxes);
	}
	
	virtual void render(TextLayoutPainter *p, const StoryText& text) const = 0;
//	virtual void render(ScPainter* p, const RenderOptions& renderOptions) const = 0;
//	virtual qreal naturalWidth() const { return width(); }
//	virtual qreal naturalHeight() const { return height(); }
//	virtual qreal minWidth() const { return width(); }
//	virtual qreal minHeight() const { return height(); }
//	virtual qreal maxWidth() const { return width(); }
//	virtual qreal maxHeight() const { return height(); }
//	virtual void  justifyLine(qreal width) {}
//	virtual void  justifyBlock(qreal width) {}

//	virtual QString toString() const = 0;
//	virtual void renderPDF(QTextStream &cStr, const QMap<QString, QString>& fontMap) const = 0;
};


class GroupBox : public Box
{
public:
	GroupBox()
	{
		m_type = T_Block;
		m_firstChar = 0;
		m_lastChar = -1;
	}
	
	int pointToPosition(FPoint coord) const;
	FRect boundingBox(int pos, uint len = 1) const;
//	QList<const Box*> pathForPos(int pos) const;

	void addBox(const Box* box);
	Box* addBox(uint i);
	Box* removeBox(uint i);
	void render(TextLayoutPainter *p, const StoryText& text) const;
//	void justify(const ParagraphStyle& style);

private:
	Box* m_last;
	GroupBox* m_lines;
};


class LineBox : public GroupBox
{
public:
	LineBox()
	{
		m_type = T_Line;
	}

	int pointToPosition(FPoint coord) const;
	bool containsPoint(FPoint coord) const;
	void render(TextLayoutPainter *p, const StoryText& text) const;
//	void justify(const ParagraphStyle& style);
	qreal colLeft;
};


class GlyphBox : public Box
{
	
public:
	GlyphBox(const CharStyle* style, LayoutFlags flags) : m_glyphRun(style, flags) {m_type = T_Glyphs;}
	GlyphBox(const GlyphRun& run)
		: m_glyphRun(run)
	{
		m_type = T_Glyphs;
		m_firstChar = run.firstChar();
		m_lastChar = run.lastChar();
	}

	FRect boundingBox(int pos, uint len = 1) const
	{
		FPoint topLeft(bbox().x(), bbox().y());
		FSize size(bbox().width(), bbox().height());
		FRect newbox(topLeft, size);

		return newbox;
	}

//	QList<const Box*> pathForPos(int pos) const;
	void render(TextLayoutPainter *p, const StoryText& text) const;
	int pointToPosition(FPoint coord) const;

	GlyphRun glyphRun() const { return m_glyphRun; }
	ScFace font() const { return m_glyphRun.style().font(); }

private:
	GlyphRun m_glyphRun;
};

class ObjectBox : public Box
{
	PageItem* m_item;
	CharStyle m_style;

public:
	ObjectBox(const GlyphRun& run)
		: m_item(run.object())
		, m_style(run.style())
	{
		m_type = T_Object;
	}

	void render(TextLayoutPainter *p, const StoryText& text) const;
	FRect boundingBox(int pos, uint len = 1) const
	{
		FPoint topLeft(bbox().x(), bbox().y());
		FSize size(bbox().width(), bbox().height());
		FRect newbox(topLeft, size);

		return newbox;
	}
	int pointToPosition(FPoint coord) const;
};
#endif /* defined(__Scribus__boxes__) */
