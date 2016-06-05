
#include <QDebug>

#include "shapedtextfeed.h"



static bool logicalGlyphRunComp(const GlyphCluster &r1, const GlyphCluster &r2)
{
	return r1.firstChar() < r2.firstChar();
}

static bool visualGlyphRunComp(const GlyphCluster &r1, const GlyphCluster &r2)
{
	return r1.visualIndex() < r2.visualIndex();
}





ShapedTextFeed::ShapedTextFeed(ITextSource* source, int firstChar, ITextContext* context) :
    m_textSource(source),
    m_context(context), 
    m_shaper(context, *source, firstChar),
	m_endChar(firstChar)
{}


bool ShapedTextFeed::haveMoreText(int glyphPos, QList<GlyphCluster>& glyphs)
{
    while (glyphPos >= glyphs.count())
    {
        ShapedText more = m_shaper.shape(m_textSource->nextBlockStart(m_endChar));
		if (more.glyphs().count() == 0)
			break;
//		qDebug() << "feed" << m_endChar << "-->" << more.lastChar() + 1;
		m_endChar = more.lastChar() + 1;
        std::sort(more.glyphs().begin(), more.glyphs().end(), logicalGlyphRunComp);
        glyphs.append(more.glyphs());
    }
    return glyphPos < glyphs.count();
}


QList<GlyphCluster> ShapedTextFeed::putInVisualOrder(const QList<GlyphCluster>& glyphs, int start, int end)
{
    int glyphsCount = end - start;
    QList<GlyphCluster> runs;
    for (int i = 0; i < glyphsCount; ++i)
        runs.append(glyphs.at(start + i));
    std::sort(runs.begin(), runs.end(), visualGlyphRunComp);
    return runs;
}

