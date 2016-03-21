/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */

#ifndef BOXES_H
#define BOXES_H

#include <QObject>

#include "sctextstruct.h"

class StoryText;
class TextLayoutPainter;


/**
 Class Box has a similar role as TeX's boxes.
 Scribus packs glyph runs into GlyphBoxes, GlyphBoxes and ObjectBoxes into LineBoxes
 and LineBoxes into GroupBox(T_Block).
 (and in the future: math atoms, tables & table cells, ...)
 */
class Box: public QObject {
	Q_OBJECT

public:
	enum BoxType {
		T_Invalid,
		T_Block,
		T_Line,
		T_PathLine,
		T_Glyph,
		T_Object
	};

	enum BoxDirection {
		D_Horizontal,
		D_Vertical
	};

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

	double x() const { return m_x; }
	double y() const { return m_y; }
	void moveTo (double x, double y) { m_x = x, m_y = y; }
	void moveBy (double x, double y) { m_x += x, m_y += y; }

	double width() const { return m_width; }
	void setWidth(double w) { m_width = w; }

	double ascent() const { return m_ascent; }
	double descent() const { return m_descent; }
	double height() const { return m_ascent - m_descent; }
	void setAscent(double a) { m_ascent = a; }
	void setDescent(double d) { m_descent = d; }

	QRectF bbox() const { return QRectF(m_x, m_y, m_width, height()); }

	virtual bool containsPoint(QPointF coord) const { return bbox().contains(coord); }
	bool containsPos(int pos) const { return firstChar() <= pos && pos <= lastChar(); }

	/// returns a char position for the point coord + (m_x, m_y)
	virtual int pointToPosition(QPointF coord) const = 0;
	virtual QLineF positionToPoint(int pos) const { return QLineF(); }

	int firstChar() const { return m_firstChar == INT_MAX ? 0 : m_firstChar; }
	int lastChar() const { return m_lastChar == INT_MIN ? 0 : m_lastChar; }

	void setMatrix(QTransform x) { m_matrix = x; }
	const QTransform& matrix() const { return m_matrix; }

//	virtual void justify(const ParagraphStyle& style) {}

	QList<Box*>& boxes() { return m_boxes; }
	const QList<const Box*>& boxes() const {
		return reinterpret_cast<const QList<const Box*> & > (m_boxes);
	}

	/// Render the box and any boxes it contains, recursively.
	virtual void render(TextLayoutPainter *p) const = 0;

	/// Same as render() but handles text selection, for rendering on screen.
	virtual void render(TextLayoutPainter *p, PageItem *item) const = 0;

	virtual double naturalWidth() const { return width(); }
	virtual double naturalHeight() const { return height(); }

public slots:
	virtual void childChanged() { }
signals:
	void boxChanged();

protected:
	BoxType m_type;
	BoxDirection m_direction;
	double m_x;
	double m_y;
	double m_width;
	double m_descent;
	double m_ascent;
	QList<Box*> m_boxes;
	int m_firstChar;
	int m_lastChar;
	QTransform m_matrix;
};


class GroupBox: public Box
{
public:
	GroupBox(BoxDirection direction)
	{
		m_type = T_Block;
		m_direction = direction;
		m_firstChar = INT_MAX;
		m_lastChar = INT_MIN;
		m_naturalWidth = m_naturalHeight = 0;
	}

	int pointToPosition(QPointF coord) const;
	QLineF positionToPoint(int pos) const;

	void render(TextLayoutPainter *p) const;
	void render(TextLayoutPainter *p, PageItem *item) const;

	double naturalWidth() const { return m_naturalWidth; }
	double naturalHeight() const { return m_naturalHeight; }

//	void justify(const ParagraphStyle& style);

	virtual void addBox(const Box* box);
	virtual void removeBox(int i);

	void childChanged()
	{
		update();
	}

protected:
	double m_naturalWidth;
	double m_naturalHeight;

private:
	void update();
};


class LineBox: public GroupBox
{
public:
	LineBox()
		: GroupBox(D_Horizontal)
	{
		m_type = T_Line;
	}

	int pointToPosition(QPointF coord) const;
	QLineF positionToPoint(int pos) const;

	void render(TextLayoutPainter *p) const;
	void render(TextLayoutPainter *p, PageItem *item) const;

	double naturalWidth() const { return m_naturalWidth; }
	double naturalHeight() const { return height(); }

//	void justify(const ParagraphStyle& style);

	void addBox(const Box* box);
	void removeBox(int i);

protected:
	virtual void drawBackGround(TextLayoutPainter *p) const;
	virtual void update();
};

class PathLineBox: public LineBox
{
public:
	PathLineBox()
	{
		m_type = T_PathLine;
	}

protected:
	void update();
	void drawBackGround(TextLayoutPainter *p) const;
};

class GlyphBox: public Box
{
public:
	GlyphBox(const GlyphRun& run)
		: m_glyphRun(run)
		, m_effects(run.style().effects())
	{
		m_type = T_Glyph;
		m_firstChar = run.firstChar();
		m_lastChar = run.lastChar();
		m_width = run.width();
	}

	int pointToPosition(QPointF coord) const;

	void render(TextLayoutPainter *p) const;
	void render(TextLayoutPainter *p, PageItem *item) const;

	const CharStyle& style() const { return m_glyphRun.style(); }

private:
	GlyphRun m_glyphRun;
	const StyleFlag m_effects;
};

class ObjectBox: public GlyphBox
{
public:
	ObjectBox(const GlyphRun& run)
		: GlyphBox(run)
		, m_item(run.object())
	{
		m_type = T_Object;
	}

	void render(TextLayoutPainter *p) const;
	void render(TextLayoutPainter*, PageItem *item) const;

private:
	PageItem* m_item;
};

#endif // BOXES_H
