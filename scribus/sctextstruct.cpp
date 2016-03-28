/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QObject>
#include <QList>
#include <QPointer>
#include "sctextstruct.h"
#include "scfonts.h"
#include "pageitem.h"
#include "scribusdoc.h"


qreal GlyphRun::width() const
{
	qreal width = 0;
	foreach (const GlyphLayout gl, m_glyphs)
	{
		width += gl.xadvance * gl.scaleH;
	}
	return width;
}

double GlyphRun::ascent() const
{
	ScFace font = m_style->font();
	double asc = 0;
	foreach (const GlyphLayout gl, m_glyphs) {
		GlyphMetrics gm = font.glyphBBox(gl.glyph, m_style->fontSize() / 10.0);
		asc = qMax(asc, gm.ascent * gl.scaleV);
	}
	return asc;
}

double GlyphRun::desent() const
{
	ScFace font = m_style->font();
	double des = 0;
	foreach (const GlyphLayout gl, m_glyphs) {
		GlyphMetrics gm = font.glyphBBox(gl.glyph, m_style->fontSize() / 10.0);
		des = qMax(des, gm.descent * gl.scaleV);
	}
	return -des;
}

ScText::~ScText() 
{
	if (parstyle)
		delete parstyle;
	parstyle = NULL;
	mark = NULL;
}

bool ScText::hasObject(ScribusDoc *doc) const
{
	if (this->ch == SpecialChars::OBJECT)
		return ((embedded > 0) && (doc->FrameItems.contains(embedded)));
	return false;
}

bool ScText::hasMark(Mark* MRK) const
{
	if (this->ch == SpecialChars::OBJECT)
	{
		if (MRK == NULL)
			return mark != NULL;
		else
			return mark == MRK;
	}
	return false;
}

QList<PageItem*> ScText::getGroupedItems(ScribusDoc *doc)
{
	QList<PageItem*> result;
	if ((embedded > 0) && (doc->FrameItems.contains(embedded)))
	{
		PageItem* dItem = doc->FrameItems[embedded];
		result.append(dItem);
		if (dItem->isGroup())
			result = dItem->getItemList();
	}
	return result;
}

PageItem* ScText::getItem(ScribusDoc *doc)
{
	if ((embedded > 0) && (doc->FrameItems.contains(embedded)))
		return doc->FrameItems[embedded];
	return NULL;
}

void ScText::setNewMark(Mark *mrk)
{
	if (!mrk->isUnique())
		mark = mrk;
}
