//
//  boxes.cpp
//  Scribus
//
//  Created by Andreas Vox on 23.05.15.
//
//

#include "boxes.h"


GroupBox::GroupBox()
{
	m_firstChar = 0;
	m_lastChar = -1;
}


int GroupBox::pointToPosition(FPoint coord) const
{
	FPoint rel = coord - FPoint(m_x, m_y);
	for (int i=0; i < m_boxes.count(); ++i)
	{
		Box* b = m_boxes[i];
		if (b->containsPoint(rel))
		{
			int result = b->screenToPosition(rel);
			if (result >= 0)
				return result;
		}
	}
	return -1;
}


FRect GroupBox::boundingBox(int pos, uint len) const;
{
	FRect result;
	for (int i=0; i < m_boxes.count(); ++i)
	{
		Box* b = m_boxes[i];
		if (b->containsPos(pos))
		{
			result = result.unite(b->boundingBox(pos, len));
		}
	}
	return result.valid()? result + FPoint(m_x, m_y) : result;
}


void GroupBox::addBox(const Box* box)
{
	m_boxes.append(box);
	
	if (box->firstChar() < m_firstChar)
		m_firstChar = box->firstChar();
	if (box->lastChar() > m_lastChar)
		m_lastChar = box->lastChar();
	
	if (0 == m_ascent)
		m_ascent = b->ascent();

	FRect newRect = b->bbox().moveBy(m_x, m_y);
	newRect = bbox().unite(newRect);
	m_y = newRect.y() + m_ascent;
	m_x = newRect.x();
	m_width = newRect.width();
	m_descent = newRect.height() - m_ascent;
}

Box* GroupBox::addBox(uint i)
{
	Box* result = m_boxes.removeAt(i);
TODO: recalc bounds
	int lastsLastChar = last->lastChar();
	delete last;
	if (m_lines->boxes().isEmpty()) {
		clear();
		return;
	}
	// fix lastInFrame
	if (m_lines->lastChar() != lastsLastChar) return;
	m_lastChar = m_lines->boxes().last()->lastChar();

	return result;
}


GlypHBox::GlyphBox()
{
	m_type = T_Glyphs;
}


int GlyphBox::pointToPosition(FPoint coord) const
{
	qreal relX = coord.x() - m_x;
	qreal xPos = 0.0;
	for (int i = 0; i < m_gylphs.length(); ++i)
	{
		qreal width = m_glyphs[i].xadvance;
		if (xPos <= relX && relX <= xPos + width)
		{
			return m_firstChar + i; // FIXME: use clusters
		}
		xPos += width;
	}
	return -1;
}


FRect GlyphBox::boundingBox(int pos, uint len) const
{
	int relPos = firstChar - pos;
	qreal xPos1 = m_x;
	for (int i = 0; i < relPos - 1; ++i)
	{
		xPos1 += m_glyphs[i].xadvance;
	}
	qreal width = m_glyphs[relPos].xadvance;
	qreal xPos2 = xPos1 + width;
	for (int i = relPos + 1; i < pos + len; ++i)
	{
		xPos2 += m_glyphs[i].xadvance;
	}
	return FRect(xPos1, -descent, xPos2 - xPos1, ascent);
}


LineBox::LineBox()
{
	m_type = T_Line;
}



