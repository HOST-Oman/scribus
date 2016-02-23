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

#undef NDEBUG

#include <cassert>
#include "../styles/charstyle.h"
#include "pageitem.h"
#include "prefsstructs.h"
#include "../styles/paragraphstyle.h"
#include "specialchars.h"
#include "storytext.h"
#include "textlayout.h"
#include "boxes.h"



TextLayout::TextLayout(StoryText* text, PageItem* frame)
{
	m_story = text;
	m_frame = frame;

	m_validLayout = false;
	m_magicX = 0.0;
	m_lastMagicPos = -1;
	
	m_lines = new GroupBox();
}

TextLayout::~TextLayout()
{
	delete m_lines;
}


uint TextLayout::lines() const
{
	return m_lines->boxes().count();
}

const LineBox* TextLayout::line(uint i) const
{
	return dynamic_cast<const LineBox*>(m_lines->boxes()[i]);
}

const PathData& TextLayout::point(int pos) const
{
	return m_path[pos];
}

PathData& TextLayout::point(int pos)
{
	if (pos >= story()->length())
		m_path.resize(story()->length());
	return m_path[pos];
}


void TextLayout::appendLine(const LineBox* ls)
	{ 
		assert( ls->firstChar() >= 0 );
		assert( ls->firstChar() < story()->length() );
//		assert( ls->lastChar() >= 0 && ls->firstChar() - ls->lastChar() < 1 );
		assert( ls->lastChar() < story()->length() );
		m_lines->addBox(ls);
	}

// Remove the last line from the list. Used when we need to backtrack on the layouting.
void TextLayout::removeLastLine ()
{
	if (m_lines->boxes().isEmpty()) return;
	Box* last = m_lines->removeBox(lines() - 1);
	delete last;
}

void TextLayout::render(ScPainter *p, const StoryText &text)
{

     p->save();
	 m_lines->moveBy(-m_lines->x(), -m_lines->y() + m_lines->descent());
	 m_lines->render(p, text);
     p->restore();
}

void TextLayout::clear() 
{
	delete m_lines;
	m_lines = new GroupBox();
	m_path.clear();
	if (m_frame->asPathText() != NULL)
		m_path.resize(story()->length());
}

void TextLayout::setStory(StoryText *story)
{
	m_story = story;
	clear();
}

int TextLayout::startOfLine(int pos) const
{
	for (uint i=0; i < lines(); ++i) {
		const LineBox* ls = line(i);
		if (ls->firstChar() <= pos && pos <= ls->lastChar())
			return ls->firstChar();
	}
	return 0;
}

int TextLayout::endOfLine(int pos) const
{
	for (uint i=0; i < lines(); ++i) {
		const LineBox* ls = line(i);
		if (ls->containsPos(pos))
			return story()->text(ls->lastChar()) == SpecialChars::PARSEP ? ls->lastChar() :
				story()->text(ls->lastChar()) == ' ' ? ls->lastChar() : ls->lastChar() + 1;
	}
	return story()->length();
}

int TextLayout::prevLine(int pos) const
{
	for (uint i=0; i < lines(); ++i)
	{
		// find line for pos
		const LineBox* ls = line(i);
		if (ls->containsPos(pos))
		{
			if (i == 0)
				return startOfLine(pos);
			// find current xpos
			qreal xpos = ls->boundingBox(pos, 1).x();
			if (pos != m_lastMagicPos || xpos > m_magicX)
				m_magicX = xpos;
			
			const LineBox* ls2 = line(i-1);
			// find new cpos
			for (int j = ls2->firstChar(); j <= ls2->lastChar(); ++j)
			{
				xpos = ls2->boundingBox(j,1).x();
				if (xpos > m_magicX) {
					m_lastMagicPos = j;
					return j;
				}
			}
			m_lastMagicPos = ls2->lastChar();
			return ls2->lastChar();
		}
	}
	return m_lines->firstChar();
}

int TextLayout::nextLine(int pos) const
{
	for (uint i=0; i < lines(); ++i)
	{
		// find line for pos
		const LineBox* ls = line(i);
		if (ls->containsPos(pos))
		{
			if (i+1 == lines())
				return endOfLine(pos);
			// find current xpos
			qreal xpos = ls->boundingBox(pos, 1).x();

			if (pos != m_lastMagicPos || xpos > m_magicX)
				m_magicX = xpos;
			
			const LineBox* ls2 = line(i+1);
			// find new cpos
			for (int j = ls2->firstChar(); j <= ls2->lastChar(); ++j)
			{
				xpos = ls2->boundingBox(j, 1).x();
				if (xpos > m_magicX) {
					m_lastMagicPos = j;
					return j;
				}
			}
			m_lastMagicPos = ls2->lastChar() + 1;
			return ls2->lastChar() + 1;
		}
	}
	return m_lines->lastChar();
}

int TextLayout::startOfFrame() const
{
	return m_lines->firstChar();
}

int TextLayout::endOfFrame() const
{
	return m_lines->lastChar() + 1;
}


int TextLayout::screenToPosition(FPoint coord) const
{
	int result = m_lines->pointToPosition(coord /*- FPoint(m_frame->xPos(), m_frame->yPos())*/);
	if (result >= 0)
		return result;
#if 0
	qreal maxx = coord.x() - 1.0;
	for (unsigned int i=0; i < lines(); ++i)
	{
		LineSpec ls = line(i);
//		qDebug() << QString("screenToPosition: (%1,%2) -> y %3 - %4 + %5").arg(coord.x()).arg(coord.y()).arg(ls.y).arg(ls.ascent).arg(ls.descent);
		if (ls.y + ls.descent < coord.y())
			continue;
		qreal xpos = ls.x;
		for (int j = ls.firstChar; j <= ls.lastChar; ++j)
		{
//				qDebug() << QString("screenToPosition: (%1,%2) -> x %3 + %4").arg(coord.x()).arg(coord.y()).arg(xpos).arg(item(j)->glyph.wide());
			qreal width = story()->getGlyphs(j)->wide();
			xpos += width;
			if (xpos >= coord.x())
			{
				if (story()->hasObject(j))
					return j;
				else
					return xpos - width/2 > coord.x() ? j : j+1;
			}
		}
		if (xpos > maxx)
			maxx = xpos;
		if (xpos + 1.0 > coord.x()) // allow 1pt after end of line
			return ls.lastChar + 1;
		else if (coord.x() <= ls.x + ls.width) // last line of paragraph?
			return ((ls.lastChar == m_lastInFrame) ? (ls.lastChar + 1) : ls.lastChar);
		else if (xpos < ls.x + 0.01 && maxx >= coord.x()) // check for empty line
			return ls.firstChar;
	}
#endif
	return qMax(endOfFrame(), startOfFrame());
}


FRect TextLayout::boundingBox(int pos, uint len) const
{
	FRect result;
	if (m_lines->containsPos(pos))
	{
		result = m_lines->boundingBox(pos, len);
		if (result.isValid())
		{
//			result.moveBy(m_frame->xPos(), m_frame->yPos());
			return result;
		}
	}
	
#if 0
	LineBox* ls;
	for (uint i=0; i < lines(); ++i)
	{
		ls = line(i);
		if (ls->lastChar() < pos)
			continue;
		if (ls->firstChar() <= pos) {
			/*
			//if (ls.lastChar == pos && (item(pos)->effects() & ScLayout_SuppressSpace)  )
			{
				if (i+1 < lines())
				{
					ls = line(i+1);
					result.setRect(ls.x, ls.y - ls.ascent, 1, ls.ascent + ls.descent);
				}
				else
				{
					ls = line(lines()-1);
					const ParagraphStyle& pstyle(paragraphStyle(pos));
					result.setRect(ls.x, ls.y + pstyle.lineSpacing() - ls.ascent, 1, ls.ascent + ls.descent);
				}
			}
			else */
			{
				qreal xpos = ls.x;
				for (int j = ls.firstChar; j < pos; ++j)
				{
					if (story()->hasObject(j))
						xpos += (story()->object(j)->width() + story()->object(j)->lineWidth()) * story()->getGlyphs(j)->scaleH;
					else
						xpos += story()->getGlyphs(j)->wide();
				}
				qreal finalw = 1;
				if (story()->hasObject(pos))
					finalw = (story()->object(pos)->width() + story()->object(pos)->lineWidth()) * story()->getGlyphs(pos)->scaleH;
				else
					finalw = story()->getGlyphs(pos)->wide();
				const CharStyle& cs(story()->charStyle(pos));
				qreal desc = -cs.font().descent(cs.fontSize() / 10.0);
				qreal asce = cs.font().ascent(cs.fontSize() / 10.0);
				result.setRect(xpos, ls.y - asce, pos < story()->length()? finalw : 1, desc+asce);
			}
			return result;
		}
	}
	
#endif
	const ParagraphStyle& pstyle(story()->paragraphStyle(qMin(pos, story()->length()))); // rather the trailing style than a segfault.
	if (lines() > 0)
	{
		const LineBox* ls = line(lines()-1);
		result.setRect(ls->x(), ls->y() + pstyle.lineSpacing() - ls->ascent(), 1, ls->height());
	}
	else
	{
		result.setRect(1, 1, 1, pstyle.lineSpacing());
	}

	return result;
}

