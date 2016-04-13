#include "glyphcluster.h"

GlyphCluster::GlyphCluster(const CharStyle* style, LayoutFlags flags, int first, int last, PageItem* o, int i)
	: m_style(style)
	, m_flags(flags)
	, m_object(o)
	, m_firstChar(first)
	, m_lastChar(last)
	, m_visualIndex(i)
{}

double GlyphCluster::width() const
{
	double width = 0;
	foreach (const GlyphLayout gl, m_glyphs)
	{
		width += gl.xadvance * gl.scaleH;
	}
	return width;
}

double GlyphCluster::ascent() const
{
	const ScFace &font = m_style->font();
	double asc = 0;
	foreach (const GlyphLayout gl, m_glyphs) {
		GlyphMetrics gm = font.glyphBBox(gl.glyph, m_style->fontSize() / 10.0);
		asc = qMax(asc, gm.ascent * gl.scaleV);
	}
	return asc;
}

double GlyphCluster::desent() const
{
	const ScFace &font = m_style->font();
	double des = 0;
	foreach (const GlyphLayout gl, m_glyphs) {
		GlyphMetrics gm = font.glyphBBox(gl.glyph, m_style->fontSize() / 10.0);
		des = qMax(des, gm.descent * gl.scaleV);
	}
	return -des;
}

const CharStyle& GlyphCluster::style() const
{
	return *m_style;
}

bool GlyphCluster::hasFlag(LayoutFlags f) const
{
	return (m_flags & f) == f;
}

void GlyphCluster::setFlag(LayoutFlags f)
{
	m_flags = static_cast<LayoutFlags>(m_flags | f);
}

void GlyphCluster::clearFlag(LayoutFlags f)
{
	m_flags = static_cast<LayoutFlags>(m_flags & ~f);
}

QList<GlyphLayout>& GlyphCluster::glyphs()
{
	return m_glyphs;
}

const QList<GlyphLayout>& GlyphCluster::glyphs() const {
	return m_glyphs;
}

PageItem* GlyphCluster::object() const
{
	return m_object;
}

int GlyphCluster::firstChar() const
{
	return m_firstChar;
}

int GlyphCluster::lastChar() const
{
	return m_lastChar;
}

int GlyphCluster::visualIndex() const
{
	return m_visualIndex;
}
