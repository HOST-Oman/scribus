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
#include "textlayoutpainter.h"

int GroupBox::pointToPosition(QPointF coord) const
{
	QPointF rel = coord - QPointF(m_x, m_y);
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

QLineF GroupBox::positionToPoint(int pos) const
{
	QLineF result;
	foreach (const Box *box, boxes())
	{
		if (box->containsPos(pos))
		{
			result = box->positionToPoint(pos);
		}
	}
	if (!result.isNull())
		result.translate(x(), y());
	return result;
}

void GroupBox::render(TextLayoutPainter *p) const
{
	p->save();
	p->translate(x(), y());
	foreach (const Box *box, boxes())
	{
		box->render(p);
	}
	p->restore();
}

void GroupBox::render(TextLayoutPainter *p, PageItem *item) const
{
	p->save();
	p->translate(x(), y());
	foreach (const Box *box, boxes())
	{
		box->render(p, item);
	}
	p->restore();
}

void GroupBox::addBox(const Box* box)
{
	boxes().append(const_cast<Box*>(box));
	QObject::connect(box, SIGNAL(boxChanged()), this, SLOT(childChanged()));
	update();
}

void GroupBox::removeBox(int i)
{
	if (i < 0 || i > boxes().size())
		return;

	boxes().removeAt(i);
	update();
}

void GroupBox::update()
{
	m_naturalHeight = m_naturalWidth = 0;
	foreach (Box* box, boxes()) {
		m_firstChar = qMin(m_firstChar, box->firstChar());
		m_lastChar = qMax(m_lastChar, box->lastChar());
		if (m_direction == D_Horizontal)
		{
			m_naturalWidth += box->width();
			m_naturalHeight = qMax(m_naturalHeight, box->naturalHeight());
		}
		else
		{
			box->moveTo(box->x(), naturalHeight());
			m_naturalWidth = qMax(m_naturalWidth, box->naturalWidth());
			m_naturalHeight += box->height();
		}
	}

	emit boxChanged();
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

int LineBox::pointToPosition(QPointF coord) const
{
	int position = GroupBox::pointToPosition(coord);
	if (position < 0)
	{
		if (containsPoint(coord))
		{
			if (coord.x() < x())
				position = firstChar();
			else
				position = lastChar();
		}
	}

	return position;
}

QLineF LineBox::positionToPoint(int pos) const
{
	QLineF result;
	foreach (const Box *box, boxes())
	{
		if (box->containsPos(pos))
		{
			double xPos = x() + box->x();
			result = QLineF(xPos, y(), xPos, y() + height());
			break;
		}
	}
	return result;
}

void LineBox::render(TextLayoutPainter *p) const
{
	p->translate(x(), y());

	drawBackGround(p);

	p->translate(0, ascent());
	foreach (const Box *box, boxes())
	{
		box->render(p);
	}

	p->translate(-x(), -y() - ascent());
}

void LineBox::render(TextLayoutPainter *p, PageItem *item) const
{
	p->translate(x(), y());

	drawBackGround(p);

	QRectF selection;
	foreach (const Box *box, boxes())
	{
		if (item->itemText.selected(box->firstChar()) || item->itemText.selected(box->lastChar()))
			selection |= QRectF(box->x(), 0, box->width(), height());
	}

	if (!selection.isEmpty())
	{
		bool s = p->selected();
		bool sw = p->strokeWidth();

		p->setSelected(true);
		p->setStrokeWidth(0);
		p->drawRect(selection);

		p->setSelected(s);
		p->setStrokeWidth(sw);
	}

	p->translate(0, ascent());
	foreach (const Box *box, boxes())
	{
		box->render(p, item);
	}

	p->translate(-x(), -y() - ascent());
}

void LineBox::drawBackGround(TextLayoutPainter *p) const
{
	QString lastColor;
	double lastShade = 100;
	QRectF lastRect;
	for (int i = 0; i < boxes().count(); i++)
	{
		const GlyphBox* box = dynamic_cast<const GlyphBox*>(boxes()[i]);
		if (!box)
			continue;
		const CharStyle& style = box->style();
		if (style.backColor() != CommonStrings::None)
		{
			QRectF rect(box->x(), 0, box->width(), height());
			if (lastColor.isEmpty() || (lastColor == style.backColor() && lastShade == style.backShade()))
			{
				lastRect |= rect;
			}
			else
			{
				if (!lastRect.isEmpty())
				{
					TextLayoutColor backColor(lastColor, lastShade);
					p->save();
					p->setFillColor(backColor);
					p->setStrokeColor(backColor);
					p->drawRect(lastRect);
					p->restore();
				}
				lastRect = rect;
			}
			lastColor = style.backColor();
			lastShade = style.backShade();
		}
		else
		{
			if (!lastRect.isEmpty())
			{
				TextLayoutColor backColor(lastColor, lastShade);
				p->save();
				p->setFillColor(backColor);
				p->setStrokeColor(backColor);
				p->drawRect(lastRect);
				p->restore();
			}
			lastColor.clear();
			lastShade = 100;
			lastRect = QRectF();
		}
	}
	if (!lastRect.isEmpty())
	{
		TextLayoutColor backColor(lastColor, lastShade);
		p->save();
		p->setFillColor(backColor);
		p->setStrokeColor(backColor);
		p->drawRect(lastRect);
		p->restore();
	}
}

void LineBox::addBox(const Box* box)
{
	m_boxes.append(const_cast<Box*>(box));
	update();
}

void LineBox::removeBox(int i)
{
	if (i < 0 || i > boxes().size())
		return;

	boxes().removeAt(i);
	update();
}

void LineBox::update()
{
	m_naturalWidth = 0;
	foreach (Box* box, boxes()) {
		m_firstChar = qMin(m_firstChar, box->firstChar());
		m_lastChar = qMax(m_lastChar, box->lastChar());

		box->moveTo(m_naturalWidth, box->y());
		m_naturalWidth += box->width();
	}

	emit boxChanged();
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


void PathLineBox::update()
{
	foreach (Box* box, boxes()) {
		m_firstChar = qMin(m_firstChar, box->firstChar());
		m_lastChar = qMax(m_lastChar, box->lastChar());
	}

	emit boxChanged();
}

void PathLineBox::drawBackGround(TextLayoutPainter *p) const
{
	for (int i = 0; i < boxes().count(); i++)
	{
		const GlyphBox* box = dynamic_cast<const GlyphBox*>(boxes()[i]);
		if (!box)
			continue;

		const CharStyle& style = box->style();
		if (style.backColor() != CommonStrings::None)
		{
			QRectF rect(box->x(), -box->ascent(), box->width(), box->height());
			TextLayoutColor backColor(style.backColor(), style.backShade());
			p->save();
			p->setMatrix(box->matrix());
			p->setFillColor(backColor);
			p->setStrokeColor(backColor);
			p->drawRect(rect);
			p->restore();
		}
	}
}


void GlyphBox::render(TextLayoutPainter *p, PageItem *item) const
{

	bool s = p->selected();

	bool selected = item->itemText.selected(firstChar()) || item->itemText.selected(lastChar());

	if (((selected && item->isSelected()) || ((item->nextInChain() != 0 || item->prevInChain() != 0) && selected)) &&
		(item->doc()->appMode == modeEdit || item->doc()->appMode == modeEditTable))
	{
		p->setSelected(true);
	}

	render(p);

	p->setSelected(s);
}

void GlyphBox::render(TextLayoutPainter *p) const
{
	// This is a very hot method and can be easily called tens of thousands of times per second,
	// so it is always a good idea to profile changes to this code.
	const CharStyle& style = m_glyphRun.style();
	const ScFace& font = style.font();
	double fontSize = style.fontSize() / 10.0;
	bool hasFillColor = style.fillColor() != CommonStrings::None;
	bool hasStrokeColor = style.strokeColor() != CommonStrings::None;

	p->save();

	p->setFont(font);
	p->setFontSize(fontSize);

	p->translate(x(), y());

	p->setMatrix(m_matrix);

	if (hasFillColor)
		p->setFillColor(TextLayoutColor(style.fillColor(), style.fillShade()));
	if (hasStrokeColor)
		p->setStrokeColor(TextLayoutColor(style.strokeColor(), style.strokeShade()));

	foreach (const GlyphLayout gl, m_glyphRun.glyphs())
	{
		p->save();

		// Do underlining first so you can get typographically correct
		// underlines when drawing a white outline
		if (m_effects & ScStyle_Underline && hasStrokeColor)
		{
			double st, lw;
			if ((style.underlineOffset() != -1) || (style.underlineWidth() != -1))
			{
				if (style.underlineOffset() != -1)
					st = (style.underlineOffset() / 1000.0) * font.descent(fontSize);
				else
					st = font.underlinePos(fontSize);
				if (style.underlineWidth() != -1)
					lw = (style.underlineWidth() / 1000.0) * fontSize;
				else
					lw = qMax(font.strokeWidth(fontSize), 1.0);
			}
			else
			{
				st = font.underlinePos(fontSize);
				lw = qMax(font.strokeWidth(fontSize), 1.0);
			}
			if (style.baselineOffset() != 0)
				st += fontSize * gl.scaleV * (style.baselineOffset() / 1000.0);

			double sw = p->strokeWidth();
			const TextLayoutColor& sc = p->strokeColor();

			p->setStrokeColor(p->fillColor());
			p->setStrokeWidth(lw);
			if (style.effects() & ScStyle_Subscript)
				p->drawLine(QPointF(gl.xoffset, gl.yoffset - st), QPointF(gl.xoffset + gl.xadvance, gl.yoffset - st));
			else
				p->drawLine(QPointF(gl.xoffset, -st), QPointF(gl.xoffset + gl.xadvance, -st));

			p->setStrokeWidth(sw);
			p->setStrokeColor(sc);
		}

		p->translate(gl.xoffset, gl.yoffset);

		if (style.baselineOffset() != 0)
			p->translate(0, -fontSize * (style.baselineOffset() / 1000.0));
		double glxSc = gl.scaleH * style.fontSize() / 100.0;
		double glySc = gl.scaleV * style.fontSize() / 100.0;

		if (gl.glyph == 0)
		{
			p->setStrokeColor(TextLayoutColor(PrefsManager::instance()->appPrefs.displayPrefs.controlCharColor.name()));
			p->setStrokeWidth(style.fontSize() * gl.scaleV * style.outlineWidth() * 2 / 10000.0);
			p->drawGlyphOutline(gl, false);
		}
		else if ((font.isStroked()) && hasStrokeColor && ((style.fontSize() * gl.scaleV * style.outlineWidth() / 10000.0) != 0))
		{
			p->setStrokeColor(p->fillColor());
			p->setStrokeWidth(style.fontSize() * gl.scaleV * style.outlineWidth() / 10000.0);
			p->drawGlyphOutline(gl, false);
		}
		else
		{
			if (m_effects & ScStyle_Shadowed && hasStrokeColor)
			{
				double xoff = (style.fontSize() * gl.scaleH * style.shadowXOffset() / 10000.0) / glxSc;
				double yoff = (style.fontSize() * gl.scaleV * style.shadowYOffset() / 10000.0) / glySc;

				bool s = p->selected();
				const TextLayoutColor& fc = p->fillColor();

				p->translate(xoff, -yoff);

				p->setFillColor(p->strokeColor());
				p->setSelected(false);
				p->drawGlyph(gl);

				p->translate(-xoff, yoff);
				p->setSelected(s);
				p->setFillColor(fc);
			}

			if ((style.effects() & ScStyle_Outline) && hasStrokeColor && ((style.fontSize() * gl.scaleV * style.outlineWidth() / 10000.0) != 0))
			{
				p->setStrokeWidth((style.fontSize() * gl.scaleV * style.outlineWidth() / 10000.0) / glySc);
				p->drawGlyphOutline(gl, hasFillColor);
			}
			else if (hasFillColor)
				p->drawGlyph(gl);
		}

		if (m_effects & ScStyle_Strikethrough && hasStrokeColor)
		{
			double st, lw;
			if ((style.strikethruOffset() != -1) || (style.strikethruWidth() != -1))
			{
				if (style.strikethruOffset() != -1)
					st = (style.strikethruOffset() / 1000.0) * font.ascent(fontSize);
				else
					st = font.strikeoutPos(fontSize);
				if (style.strikethruWidth() != -1)
					lw = (style.strikethruWidth() / 1000.0) * fontSize;
				else
					lw = qMax(font.strokeWidth(fontSize), 1.0);
			}
			else
			{
				st = font.strikeoutPos(fontSize);
				lw = qMax(font.strokeWidth(fontSize), 1.0);
			}
			if (style.baselineOffset() != 0)
				st += fontSize * gl.scaleV * (style.baselineOffset() / 1000.0);

			double sw = p->strokeWidth();
			const TextLayoutColor& sc = p->strokeColor();

			p->setStrokeColor(p->fillColor());
			p->setStrokeWidth(lw);
			p->drawLine(QPointF(gl.xoffset, gl.yoffset - st), QPointF(gl.xoffset + gl.xadvance, gl.yoffset - st));

			p->setStrokeWidth(sw);
			p->setStrokeColor(sc);
		}

		p->restore();
		p->translate(gl.xadvance, 0);
	}

	p->restore();
}

int GlyphBox::pointToPosition(QPointF coord) const
{
	double relX = coord.x() - m_x;
	double xPos = 0.0;

	for (int i = 0; i < m_glyphRun.glyphs().length(); ++i)
	{
		double width = m_glyphRun.glyphs().at(i).xadvance;
		if (xPos <= relX && relX <= xPos + width)
		{
			return m_firstChar + i; // FIXME: use clusters
		}
		xPos += width;
	}
	return -1;
}

void ObjectBox::render(TextLayoutPainter *p) const
{
	p->save();
	double oldX = m_item->xPos();
	double oldY = m_item->yPos();

	p->translate(x(), y() - ascent());
	p->setScale(m_style.scaleH() / 1000.0, m_style.scaleV() / 1000.0);
	p->setMatrix(m_matrix);

	m_item->setXPos(m_item->gXpos);
	m_item->setYPos((m_item->gHeight * (m_style.scaleV() / 1000.0)) + m_item->gYpos);

	if (m_style.baselineOffset() != 0)
	{
		p->translate(0, -m_item->gHeight * (m_style.baselineOffset() / 1000.0));
		m_item->setYPos(m_item->yPos() - m_item->gHeight * (m_style.baselineOffset() / 1000.0));
	}

	p->drawObject(m_item);

	m_item->setXPos(oldX);
	m_item->setYPos(oldY);
	p->restore();
}

void ObjectBox::render(TextLayoutPainter *p, PageItem *item) const
{
	render(p);
}


int ObjectBox::pointToPosition(QPointF coord) const
{
	if (x() <= coord.x() && coord.x() <= x() + width())
	{
		return firstChar();
	}

	return -1;
}
