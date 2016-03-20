//
//  boxes.h
//  Scribus
//
//  Created by Andreas Vox on 23.05.15.
//
//

#ifndef __Scribus__boxes__
#define __Scribus__boxes__

#include <QObject>

#include "fpoint.h"
#include "frect.h"
#include "sctextstruct.h"
#include "scpainter.h"

class StoryText;
class TextLayoutPainter;

/**
 class Box has a similar role as TeX's boxes. Scribus packs glyph runs into GlyphBoxes, GlyphBoxes and InlineBoxes into LineBoxes and LineBoxes into GroupBox(T_Block). (and in the future: math atoms, tables & table cells, ...)
 */
class Box : public QObject {
	Q_OBJECT

public:
	enum BoxType {
		T_Invalid,
		T_Line,
		T_Block,
		T_Glyphs,
		T_Path,
		T_Object
	};

	enum BoxDirection {
		D_Horizontal,
		D_Vertical
	};

protected:
	BoxType m_type;
	BoxDirection m_direction;
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
		m_direction = D_Horizontal;
		m_x = m_y = m_width = m_ascent = m_descent = 0;
		m_firstChar = 0;
		m_lastChar = 0;
	}

	virtual ~Box()
	{
		while (!m_boxes.isEmpty())
			delete m_boxes.takeFirst();
	}

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
	BoxDirection getDirection() { return m_direction; }

	QRectF bbox() const { return QRectF(m_x, m_y, m_width, height()); }

	virtual bool containsPoint(QPointF coord) const { return bbox().contains(coord); }
	bool containsPos(int pos) const { return firstChar() <= pos && pos <= lastChar(); }
	/// returns a char position for the point coord + (m_x, m_y)
	virtual int pointToPosition(QPointF coord) const = 0;
	virtual QLineF positionToPoint(int pos) const { return QLineF(); }

	int firstChar() const { return m_firstChar == INT_MAX ? 0 : m_firstChar; }
	int lastChar() const { return m_lastChar == INT_MIN ? 0 : m_lastChar; }

//	virtual QList<const Box*> pathForPos(int pos) const = 0;
//	virtual void justify(const ParagraphStyle& style) {}

	QList<Box*>& boxes() { return m_boxes; }
	const QList<const Box*>& boxes() const {
		return reinterpret_cast<const QList<const Box*> & > (m_boxes);
	}

	/// Render the box and any boxes it contains, recursively.
	virtual void render(TextLayoutPainter *p) const = 0;

	/// Same as render() but handles text selection, for rendering on screen.
	virtual void render(TextLayoutPainter *p, PageItem *item) const = 0;

//	virtual qreal naturalWidth() const { return width(); }
//	virtual qreal naturalHeight() const { return height(); }
//	virtual qreal minWidth() const { return width(); }
//	virtual qreal minHeight() const { return height(); }
//	virtual qreal maxWidth() const { return width(); }
//	virtual qreal maxHeight() const { return height(); }
//	virtual void  justifyLine(qreal width) {}
//	virtual void  justifyBlock(qreal width) {}

//	virtual QString toString() const = 0;

public slots:
	virtual void childChanged() { }
signals:
	void boxChanged();
};


class GroupBox : public Box
{
public:
	GroupBox(BoxDirection direction)
	{
		m_type = T_Block;
		m_direction = direction;
		m_firstChar = INT_MAX;
		m_lastChar = INT_MIN;
	}

	int pointToPosition(QPointF coord) const;
	QLineF positionToPoint(int pos) const;
//	QList<const Box*> pathForPos(int pos) const;

	virtual void addBox(const Box* box);
	virtual void removeBox(int i);
	void render(TextLayoutPainter *p, PageItem *item) const;
	void render(TextLayoutPainter *p) const;
//	void justify(const ParagraphStyle& style);

	void childChanged()
	{
		update();
	}

private:
	void update();
};


class LineBox : public GroupBox
{
	void drawBackGround(TextLayoutPainter *p) const;

public:
	LineBox()
		: GroupBox(D_Horizontal)
	{
		m_type = T_Line;
	}

	int pointToPosition(QPointF coord) const;
	QLineF positionToPoint(int pos) const;
	void addBox(const Box* box);
	void removeBox(int i);
	void render(TextLayoutPainter *p) const;
//	void justify(const ParagraphStyle& style);
	void render(TextLayoutPainter *p, PageItem *item) const;
	qreal colLeft;
};


class GlyphBox : public Box
{
public:
	GlyphBox(const GlyphRun& run)
		: m_glyphRun(run)
		, m_effects(run.style().effects())
	{
		m_type = T_Glyphs;
		m_firstChar = run.firstChar();
		m_lastChar = run.lastChar();
		m_width = run.width();
	}

//	QList<const Box*> pathForPos(int pos) const;
	void render(TextLayoutPainter *p) const;
	void render(TextLayoutPainter *p, PageItem *item) const;
	int pointToPosition(QPointF coord) const;
	const CharStyle& style() const { return m_glyphRun.style(); }

private:
	GlyphRun m_glyphRun;
	const StyleFlag m_effects;
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
		m_width = run.width();
	}

	void render(TextLayoutPainter *p) const;
	void render(TextLayoutPainter*, PageItem *item) const;
	int pointToPosition(QPointF coord) const;

};
#endif /* defined(__Scribus__boxes__) */
