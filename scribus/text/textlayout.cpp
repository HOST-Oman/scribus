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
#include "textlayoutpainter.h"
#include "boxes.h"


TextLayout::TextLayout(StoryText* text, PageItem* frame)
{
	m_story = text;
	m_frame = frame;

	m_validLayout = false;
	m_magicX = 0.0;
	m_lastMagicPos = -1;
	
	m_box = new GroupBox();
}

TextLayout::~TextLayout()
{
	delete m_box;
}


uint TextLayout::lines() const
{
	return m_box->boxes().count();
}

const LineBox* TextLayout::line(uint i) const
{
	return dynamic_cast<const LineBox*>(m_box->boxes()[i]);
}

const Box* TextLayout::box() const
{
	return m_box;
}

Box* TextLayout::box()
{
	return m_box;
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


void TextLayout::appendLine(LineBox* ls)
{
	assert( ls->firstChar() >= 0 );
	assert( ls->firstChar() < story()->length() );
	assert( ls->lastChar() < story()->length() );
	// HACK: the ascent set by PageItem_TextFrame::layout()
	// is useless, we reset it again based on the y position
	ls->setAscent(ls->y() - m_box->height());
	m_box->addBox(ls);
}

// Remove the last line from the list. Used when we need to backtrack on the layouting.
void TextLayout::removeLastLine ()
{
	m_box->removeBox(m_box->boxes().count() - 1);
}

void TextLayout::render(TextLayoutPainter *p, const StoryText &text)
{
	p->save();
	m_box->render(p, text);
	p->restore();
}

void TextLayout::clear() 
{
	delete m_box;
	m_box = new GroupBox();
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
			qreal xpos = ls->positionToPoint(pos).x1();
			if (pos != m_lastMagicPos || xpos > m_magicX)
				m_magicX = xpos;
			
			const LineBox* ls2 = line(i-1);
			// find new cpos
			for (int j = ls2->firstChar(); j <= ls2->lastChar(); ++j)
			{
				xpos = ls2->positionToPoint(j).x1();
				if (xpos > m_magicX) {
					m_lastMagicPos = j;
					return j;
				}
			}
			m_lastMagicPos = ls2->lastChar();
			return ls2->lastChar();
		}
	}
	return m_box->firstChar();
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
			qreal xpos = ls->positionToPoint(pos).x1();

			if (pos != m_lastMagicPos || xpos > m_magicX)
				m_magicX = xpos;
			
			const LineBox* ls2 = line(i+1);
			// find new cpos
			for (int j = ls2->firstChar(); j <= ls2->lastChar(); ++j)
			{
				xpos = ls2->positionToPoint(j).x1();
				if (xpos > m_magicX) {
					m_lastMagicPos = j;
					return j;
				}
			}
			m_lastMagicPos = ls2->lastChar() + 1;
			return ls2->lastChar() + 1;
		}
	}
	return m_box->lastChar();
}

int TextLayout::startOfFrame() const
{
	return m_box->firstChar();
}

int TextLayout::endOfFrame() const
{
	return m_box->lastChar() + 1;
}


int TextLayout::pointToPosition(QPointF coord) const
{
	return m_box->pointToPosition(coord);
}


QLineF TextLayout::positionToPoint(int pos) const
{
	QLineF result;

	result = m_box->positionToPoint(pos);
	if (result.isNull())
	{
		qreal x, y1, y2;
		if (lines() > 0)
		{
			// TODO: move this branch to GroupBox::positionToPoint()
			// last glyph box in last line
			Box* line = m_box->boxes().last();
			Box* glyph = line->boxes().last();
			QChar ch = story()->text(glyph->lastChar());
			if (ch == SpecialChars::PARSEP || ch == SpecialChars::LINEBREAK)
			{
				// last character is a newline, draw the cursor on the next line.
				x = dynamic_cast<LineBox*>(line)->colLeft + 1;
				y1 = line->y() + line->height();
				y2 = y1 + line->height();
			}
			else
			{
				// draw the cursor at the end of last line.
				x = line->x() + glyph->x() + glyph->width();
				y1 = line->y();
				y2 = y1 + line->height();
			}
		}
		else
		{
			// rather the trailing style than a segfault.
			const ParagraphStyle& pstyle(story()->paragraphStyle(qMin(pos, story()->length())));
			x = 1;
			y1 = 0;
			y2 = pstyle.lineSpacing();
		}
		result.setLine(x, y1, x, y2);
		result.translate(m_box->x(), m_box->y());
	}
	
	return result;
}
