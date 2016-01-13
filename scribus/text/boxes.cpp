//
//  boxes.cpp
//  Scribus
//
//  Created by Andreas Vox on 23.05.15.
//
//

#include "pageitem.h"
#include "boxes.h"
#include "appmodes.h"
#include "qapplication.h"
#include "pageitem_textframe.h"
#include "scribusdoc.h"
#include "prefsmanager.h"

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
		qDebug()<<"bbox.x1 "<<bbox().left()<<"<< bbox.x2 "<<bbox().right()<<" x coor "<<rel.x();
		qDebug()<<"bbox.y1 "<<bbox().top()<<"<< bbox.y2 "<<bbox().bottom()<<" y coor "<<rel.y();
		if (b->containsPoint(rel))
		{
			int result = b->pointToPosition(rel);
			if (result >= 0)
				return result;
		}
	}
	return -1;
}
void GroupBox::render(ScPainter *p)
{
    p->translate(x(),y());
 for (int i = 0; i < boxes().count(); i++)
 {
   LineBox* linebox = dynamic_cast<LineBox*> (boxes()[i]);

     linebox->render(p);

 }
   p->translate(-x(),-y());
}
void LineBox::render(ScPainter *p)
{
    p->translate(x(),y());
 for (int i = 0; i < boxes().count(); i++)
 {
   GlyphBox* glyphbox = dynamic_cast<GlyphBox*> (boxes()[i]);

     glyphbox->render(p);

 }
    p->translate(-x(),-y());
}
void GlyphBox::render(ScPainter *p)
{
    p->save();
    p->translate(x(),y());
    const CharStyle& style(glyphs.style());
    const ScFace font = style.font();
    for (int i = 0; i < m_glyphs.count(); ++i)
    {
        const GlyphLayout& glyphLayout(m_glyphs.at(i));
        uint glyphId = glyphLayout.glyph;
        FPointArray gly = font.glyphOutline(glyphId);
        if (gly.size() > 3)
        {
			p->translate(glyphLayout.xoffset, glyphLayout.yoffset);
            if (style.baselineOffset() != 0)
                p->translate(0, -(style.fontSize() / 10.0) * (style.baselineOffset() / 1000.0));
            double glxSc = glyphLayout.scaleH * style.fontSize() / 100.00;
            double glySc = glyphLayout.scaleV * style.fontSize() / 100.0;
            p->scale(glxSc, glySc);
            bool fr = p->fillRule();
            p->setFillRule(false);
            p->setupPolygon(&gly, true);
            if (glyphId == 0)
            {
                p->setPen(PrefsManager::instance()->appPrefs.displayPrefs.controlCharColor, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
                p->setLineWidth(style.fontSize() * glyphLayout.scaleV * style.outlineWidth() * 2 / 10000.0);
                p->strokePath();
            }
            else if ((font.isStroked()) && (style.strokeColor() != CommonStrings::None) && ((style.fontSize() * glyphLayout.scaleV * style.outlineWidth() / 10000.0) != 0))
            {
                QColor tmp = p->brush();
                p->setPen(tmp, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
                p->setLineWidth(style.fontSize() * glyphLayout.scaleV * style.outlineWidth() / 10000.0);
                p->strokePath();
            }
            else
            {
                if (style.fillColor() != CommonStrings::None)
                    p->fillPath();
                if ((style.effects() & ScStyle_Outline) && (style.strokeColor() != CommonStrings::None) && ((style.fontSize() * glyphLayout.scaleV * style.outlineWidth() / 10000.0) != 0))
                {
                    p->setLineWidth((style.fontSize() * glyphLayout.scaleV * style.outlineWidth() / 10000.0) / glySc);
                    p->strokePath();
                }
            }
            p->setFillRule(fr);
        }
    }
    p->restore();

}

FRect GroupBox::boundingBox(int pos, uint len) const
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
	if (result.isValid())
		result.moveBy(0, -y());
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
	if (0 == m_y) m_y = newRect.y();
	if (0 == m_x) m_x = newRect.x();
	if (0 == m_width) m_width = newRect.width();
	if (0 == m_descent) m_descent = newRect.height() - m_ascent;
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

/*
GlyphBox::GlyphBox(const GlyphRun& glyphrun)
{
	m_type = T_Glyphs;
}
*/
Box* GroupBox::removeBox(uint i)
{
	delete m_lines->boxes().at(i);
	return m_lines;
}

int GlyphBox::pointToPosition(FPoint coord) const
{
	qreal relX = coord.x() - m_x;
	qreal xPos = 0.0;
	for (int i = 0; i < m_glyphs.length(); ++i)
	{
		qreal width = m_glyphs.at(i).xadvance;
		if (xPos <= relX && relX <= xPos + width)
		{
			return m_firstChar + i; // FIXME: use clusters
		}
		xPos += width;
	}
	return -1;
}

LineBox::LineBox()
{
	m_type = T_Line;
}
