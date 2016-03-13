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
    qreal width=0;
    for (int i=0; i<m_glyphs.count();i++)
    {
        GlyphLayout gl = m_glyphs[i];
		width += gl.xadvance;
    }
    return width;
}

void GlyphRun::insertSoftHyphen()
{
	
}

void GlyphRun::removeSoftHyphen()
{
	
}



ScText::~ScText() 
{
	// delete the linked list if present
	/*GlyphLayout  more = glyph.more;
	while (more) {
		glyph.more = glyph.more->more;
		delete more;
		more = glyph.more;
	}*/
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
