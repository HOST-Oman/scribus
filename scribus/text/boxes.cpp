//
//  boxes.cpp
//  Scribus
//
//  Created by Andreas Vox on 23.05.15.
//  Modified by Dawood Albadi on 12/ 1/ 2016
//
//

#include "pageitem.h"
#include "boxes.h"
#include "appmodes.h"
#include "qapplication.h"
#include "pageitem_textframe.h"
#include "scribusdoc.h"
#include "prefsmanager.h"
#include "sccolorengine.h"
#include "colorblind.h"

int GroupBox::pointToPosition(FPoint coord) const
{
	FPoint rel = coord - FPoint(m_x, m_y);
	foreach (const Box *box, boxes())
	{
		if (box->containsPoint(rel))
		{
			int result = box->pointToPosition(rel);
			if (result >= 0)
				return result;
		}
	}
	return -1;
}

void GroupBox::render(TextLayoutPainter *p, const StoryText &text) const
{
	p->save();
	p->translate(x(), y());
	foreach (const Box *box, boxes())
	{
		box->render(p, text);
	}
	p->restore();
}

FRect GroupBox::boundingBox(int pos, uint len) const
{
	FRect result;
	foreach (const Box *box, boxes())
	{
		if (box->containsPos(pos))
		{
			result = result.unite(box->boundingBox(pos, len));
		}
	}
	if (result.isValid())
		result.moveBy(x(), -y());
	return result;
}

void GroupBox::addBox(const Box* box)
{
	m_boxes.append(const_cast<Box*>(box));

	if (box->firstChar() < m_firstChar)
		m_firstChar = box->firstChar();
	if (box->lastChar() > m_lastChar)
		m_lastChar = box->lastChar();

	if (0 == m_ascent)
		m_ascent = box->ascent();

	FRect newRect = box->bbox();
	newRect.moveBy(m_x, m_y);
	newRect = bbox().unite(newRect);
	if (0 == m_y)
		m_y = newRect.y();
	if (0 == m_x)
		m_x = newRect.x();
	if (0 == m_width)
		m_width = newRect.width();
	if (0 == m_descent)
		m_descent = newRect.height() - m_ascent;
}

Box* GroupBox::addBox(uint i)
{	m_boxes.removeAt(i);
	Box* result = m_boxes.at(i);
	//TODO: recalc bounds;
	int lastsLastChar = m_last->lastChar();
	delete m_last;
	if (m_lines->boxes().isEmpty()) {
		//clear();
		return m_lines->boxes().at(i);
	}
	// fix lastInFrame
	if (m_lines->lastChar() != lastsLastChar) return m_lines->boxes().at(i);//fix me
	m_lastChar = m_lines->boxes().last()->lastChar();

	return result;
}

Box* GroupBox::removeBox(uint i)
{
	delete m_lines->boxes().at(i);
	return m_lines;
}

#if 0
void GroupBox::justify(const ParagraphStyle& style)
{
	if ((style.alignment() != ParagraphStyle::Justified) && (style.alignment() != ParagraphStyle::Extended))
		return;
	for(int i = 0; i < m_boxes.count(); i++)
	{
		m_boxes[i]->justify(style);
	}
}
#endif

void LineBox::render(TextLayoutPainter *p, const StoryText &text) const
{
	p->save();
	p->translate(x(), y() + ascent());
	foreach (const Box *box, boxes())
	{
		box->render(p, text);
	}
	p->restore();
}

#if 0
void LineBox::justify(const ParagraphStyle& style)
{
	double glyphNatural = 0;
	double spaceNatural = 0;
	double glyphExtension;
	double spaceExtension;
	int spaceInsertion = 0;
	double imSpace = -1;

	// measure natural widths for glyphs and spaces
	for (int i = 0; i < m_boxes.count(); ++i)
	{
		GlyphBox *box = dynamic_cast<GlyphBox*>(m_boxes.at(i));
		GlyphRun run(box->glyphs);
		if (!run.hasFlag(ScLayout_ExpandingSpace))
		{
			glyphNatural += run.width();
		}
		else if (!run.hasFlag(ScLayout_SuppressSpace) )
		{
			spaceNatural += run.width();
			if (imSpace < 0.0 || imSpace > run.width())
				imSpace = run.width();
		}
		if (i != 0 && run.hasFlag(ScLayout_ImplicitSpace))
			//implicitSpace(itemText.text(sof - 1), ch))
		{
			spaceInsertion += 1;
		}
	}

	imSpace /= 2;

	// decision: prio 1: stretch glyph;  prio 2: insert spaces;  prio 3: stretch spaces
	if (width() < spaceNatural + glyphNatural *
			style.minGlyphExtension() && spaceNatural > 0)
	{
		glyphExtension = style.minGlyphExtension() - 1;
		spaceExtension = (width() - glyphNatural * (1+glyphExtension) ) / spaceNatural  - 1;
		imSpace = 0;
	}
	else if (width() < spaceNatural + glyphNatural * style.maxGlyphExtension() && glyphNatural > 0)
	{
		spaceExtension = 0;
		glyphExtension = (width() - spaceNatural) / glyphNatural  - 1;
		imSpace = 0;
	}
	else
	{
		glyphExtension = style.maxGlyphExtension() - 1;
		if (spaceInsertion) {
			double remaining = width() - glyphNatural * (1 + glyphExtension) - spaceNatural;
			if (imSpace > 0) {
				if (remaining / spaceInsertion < imSpace) {
					imSpace = remaining / spaceInsertion;
					spaceExtension = 0;
				} else {
					spaceExtension = (remaining + spaceNatural) / (spaceNatural + spaceInsertion * imSpace) - 1;
					imSpace *= spaceExtension + 1;
				}
			} else {
				imSpace = remaining / spaceInsertion;
				spaceExtension = 0;
			}
		} else {
			if (spaceNatural > 0)
				spaceExtension = (width() - glyphNatural * (1+glyphExtension) ) / spaceNatural  - 1;
			else
				spaceExtension = 0;
		}
	}

	double glyphScale = 1 + glyphExtension;

	/*
		qDebug() << QString("justify: line = %7 natural = %1 + %2 = %3
	(%4); spaces + %5%%; min=%8; glyphs + %6%%; min=%9")
			   .arg(spaceNatural).arg(glyphNatural).arg(spaceNatural+glyphNatural).arg(line.naturalWidth)
			   .arg(spaceExtension).arg(glyphExtension).arg(line.width)
			   .arg(style.minWordTracking()).arg(style.minGlyphExtension());
		*/

	int startItem = 0;
	if (dynamic_cast<GlyphBox*>(m_boxes[startItem])->glyphs.hasFlag(ScLayout_DropCap))
		startItem++;

	// distribute whitespace on spaces and glyphs
	for (int i = startItem; i < m_boxes.count(); ++i)
	{
		GlyphBox *box = dynamic_cast<GlyphBox*>(m_boxes.at(i));
		GlyphRun& run(box->glyphs);
		if (i != 0 && run.hasFlag(ScLayout_ImplicitSpace))
		{
			GlyphBox *lastBox = dynamic_cast<GlyphBox*>(m_boxes.at(i));
			GlyphRun& lastRun(lastBox->glyphs);
			lastRun.glyphs().last().xadvance += imSpace;
		}
		double wide = run.width();
		if (!run.hasFlag(ScLayout_ExpandingSpace))
		{
			for (int j = 0; j < run.glyphs().count(); ++j)
			{
				GlyphLayout& glyph = run.glyphs()[j];
				glyph.xadvance += wide * glyphExtension;
				glyph.xoffset *= glyphScale;
				glyph.scaleH *= glyphScale;
			}
		}
		else if (!run.hasFlag(ScLayout_SuppressSpace))
		{
			GlyphLayout& glyph = run.glyphs().last();
			glyph.xadvance += wide * spaceExtension;
		}
	}
}
#endif

void GlyphBox::render(TextLayoutPainter *p, const StoryText &text) const
{
	const CharStyle style(m_glyphRun.style());
	double fontSize = style.fontSize() / 10.0;
	bool selected = text.selected(firstChar()) || text.selected(lastChar()); // FIXME-HOST: this should move to TextFramePainter
	bool hasFillColor = style.fillColor() != CommonStrings::None;
	bool hasStrokeColor = style.strokeColor() != CommonStrings::None;
	bool hasBackColor = style.backColor() != CommonStrings::None;

	p->save();

	p->setFont(font());
	p->setFontSize(fontSize);

	p->translate(x(), y());
	p->translate(m_glyphRun.xoffset(), m_glyphRun.yoffset());

	if (hasFillColor)
		p->setFillColor(TextLayoutColor(style.fillColor(), style.fillShade()));
	if (hasStrokeColor)
		p->setStrokeColor(TextLayoutColor(style.strokeColor(), style.strokeShade()));

	if (hasBackColor)
	{
		TextLayoutColor backColor(style.backColor(), style.backShade());
		p->save();
		p->setFillColor(backColor);
		p->setStrokeColor(backColor);
		p->drawRect(QRectF(0, -ascent(), width(), height()));
		p->restore();
	}

	foreach (const GlyphLayout gl, m_glyphRun.glyphs())
	{
		p->save();

		// Do underlining first so you can get typographically correct
		// underlines when drawing a white outline
		if ((style.effects() & ScStyle_Underline) && hasStrokeColor)
		{
			double st, lw;
			if ((style.underlineOffset() != -1) || (style.underlineWidth() != -1))
			{
				if (style.underlineOffset() != -1)
					st = (style.underlineOffset() / 1000.0) * font().descent(fontSize);
				else
					st = font().underlinePos(fontSize);
				if (style.underlineWidth() != -1)
					lw = (style.underlineWidth() / 1000.0) * fontSize;
				else
					lw = qMax(font().strokeWidth(fontSize), 1.0);
			}
			else
			{
				st = font().underlinePos(fontSize);
				lw = qMax(font().strokeWidth(fontSize), 1.0);
			}
			if (style.baselineOffset() != 0)
				st += fontSize * gl.scaleV * (style.baselineOffset() / 1000.0);
			p->save();
			p->setStrokeColor(p->fillColor());
			p->setStrokeWidth(lw);
			if (style.effects() & ScStyle_Subscript)
				p->drawLine(QPointF(gl.xoffset, gl.yoffset - st), QPointF(gl.xoffset + gl.xadvance, gl.yoffset - st));
			else
				p->drawLine(QPointF(gl.xoffset, -st), QPointF(gl.xoffset + gl.xadvance, -st));
			p->restore();
		}

		if (gl.glyph <= ScFace::CONTROL_GLYPHS)
		{
			p->save();
			p->translate(gl.xoffset, gl.yoffset);

			if (style.baselineOffset() != 0)
				p->translate(0, -fontSize * (style.baselineOffset() / 1000.0));
			double glxSc = gl.scaleH * style.fontSize() / 100.0;
			double glySc = gl.scaleV * style.fontSize() / 100.0;

			if (gl.glyph == 0)
			{
				p->setStrokeColor(TextLayoutColor(PrefsManager::instance()->appPrefs.displayPrefs.controlCharColor.name()));
				p->setStrokeWidth(style.fontSize() * gl.scaleV * style.outlineWidth() * 2 / 10000.0);
				p->drawGlyphOutline(gl, false, selected);
			}
			else if ((font().isStroked()) && hasStrokeColor && ((style.fontSize() * gl.scaleV * style.outlineWidth() / 10000.0) != 0))
			{
				p->setStrokeColor(p->fillColor());
				p->setStrokeWidth(style.fontSize() * gl.scaleV * style.outlineWidth() / 10000.0);
				p->drawGlyphOutline(gl, false, selected);
			}
			else
			{
				if ((style.effects() & ScStyle_Shadowed) && hasStrokeColor)
				{
					double xoff = (style.fontSize() * gl.scaleH * style.shadowXOffset() / 10000.0) / glxSc;
					double yoff = (style.fontSize() * gl.scaleV * style.shadowYOffset() / 10000.0) / glySc;
					p->save();
					p->translate(xoff, -yoff);
					p->setFillColor(p->strokeColor());
					p->drawGlyph(gl, false);
					p->restore();
				}

				if ((style.effects() & ScStyle_Outline) && hasStrokeColor && ((style.fontSize() * gl.scaleV * style.outlineWidth() / 10000.0) != 0))
				{
					p->setStrokeWidth((style.fontSize() * gl.scaleV * style.outlineWidth() / 10000.0) / glySc);
					p->drawGlyphOutline(gl, hasFillColor, selected);
				}
				else if (hasFillColor)
					p->drawGlyph(gl, selected);
			}
			p->restore();
		}

		if ((style.effects() & ScStyle_Strikethrough) && hasStrokeColor)
		{
			double st, lw;
			if ((style.strikethruOffset() != -1) || (style.strikethruWidth() != -1))
			{
				if (style.strikethruOffset() != -1)
					st = (style.strikethruOffset() / 1000.0) * font().ascent(fontSize);
				else
					st = font().strikeoutPos(fontSize);
				if (style.strikethruWidth() != -1)
					lw = (style.strikethruWidth() / 1000.0) * fontSize;
				else
					lw = qMax(font().strokeWidth(fontSize), 1.0);
			}
			else
			{
				st = font().strikeoutPos(fontSize);
				lw = qMax(font().strokeWidth(fontSize), 1.0);
			}
			if (style.baselineOffset() != 0)
				st += fontSize * gl.scaleV * (style.baselineOffset() / 1000.0);
			p->save();
			p->setStrokeColor(p->fillColor());
			p->setStrokeWidth(lw);
			p->drawLine(QPointF(gl.xoffset, gl.yoffset - st), QPointF(gl.xoffset + gl.xadvance, gl.yoffset - st));
			p->restore();
		}

		p->restore();
		p->translate(gl.xadvance, 0);
	}

	p->restore();
}

int GlyphBox::pointToPosition(FPoint coord) const
{
	qreal relX = coord.x() - m_x;
	qreal xPos = 0.0;

	for (int i = 0; i < m_glyphRun.glyphs().length(); ++i)
	{
		qreal width = m_glyphRun.glyphs().at(i).xadvance;
		if (xPos <= relX && relX <= xPos + width)
		{
			return m_firstChar + i; // FIXME: use clusters
		}
		xPos += width;
	}
	return -1;
}

void ObjectBox::render(TextLayoutPainter *p, const StoryText& text) const
{
	p->save();
	p->translate(x(), y());
	p->drawObject(m_item, m_style);
	p->restore();
}

int ObjectBox::pointToPosition(FPoint coord) const
{
	if (x() <= coord.x() && coord.x() <= x() + width())
	{
		return firstChar();
	}

	return -1;
}
