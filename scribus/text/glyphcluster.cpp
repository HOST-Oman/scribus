#include "glyphcluster.h"

GlyphCluster::GlyphCluster(const CharStyle* style, LayoutFlags flags, int first, int last, const InlineFrame& o, int i)
	: m_style(style)
	, m_flags(flags)
	, m_object(o)
	, m_firstChar(first)
	, m_lastChar(last)
	, m_visualIndex(i)
	, m_scaleH(1.0)
	, m_scaleV(1.0)
	, extraWidth(0.0)
	, xoffset(0.0)
	, yoffset(0.0)
{}

void GlyphCluster::append(GlyphLayout& gl)
{
	gl.scaleH = m_scaleH;
	gl.scaleV = m_scaleV;
	m_glyphs.append(gl);
}

double GlyphCluster::width() const
{
	double width = 0;
	foreach (const GlyphLayout gl, m_glyphs)
	{
		width += gl.xadvance * m_scaleH;
	}
	return width + extraWidth;
}

double GlyphCluster::ascent() const
{
	const ScFace &font = m_style->font();
	double asc = 0;
	foreach (const GlyphLayout gl, m_glyphs) {
		GlyphMetrics gm = font.glyphBBox(gl.glyph, m_style->fontSize() / 10.0);
		asc = qMax(asc, gm.ascent * m_scaleV);
	}
	return asc;
}

double GlyphCluster::desent() const
{
	const ScFace &font = m_style->font();
	double des = 0;
	foreach (const GlyphLayout gl, m_glyphs) {
		GlyphMetrics gm = font.glyphBBox(gl.glyph, m_style->fontSize() / 10.0);
		des = qMax(des, gm.descent * m_scaleV);
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
	if (f == ScLayout_SuppressSpace)
	{
		for (int i = 0; i < m_glyphs.count(); i++)
		{
			GlyphLayout& gl = m_glyphs[i];
			gl.xadvance = 0;
		}
		extraWidth = 0;
	}
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

const InlineFrame& GlyphCluster::object() const
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

double GlyphCluster::scaleH() const
{
	return m_scaleH;
}

double GlyphCluster::scaleV() const
{
	return m_scaleV;
}

void GlyphCluster::setScaleH(double s)
{
	m_scaleH = s;
	for (int i = 0; i < m_glyphs.count(); i++)
	{
		GlyphLayout& gl = m_glyphs[i];
		gl.scaleH = m_scaleH;
	}
}

void GlyphCluster::setScaleV(double s)
{
	m_scaleV = s;
	for (int i = 0; i < m_glyphs.count(); i++)
	{
		GlyphLayout& gl = m_glyphs[i];
		gl.scaleV = m_scaleV;
	}
}
