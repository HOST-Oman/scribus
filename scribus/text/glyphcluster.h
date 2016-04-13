#ifndef GLYPHRUN_H
#define GLYPHRUN_H

#include <QList>

#include "sctextstruct.h"

class GlyphCluster
{
	const CharStyle* m_style;
	LayoutFlags m_flags;
	QList<GlyphLayout> m_glyphs;
	PageItem* m_object;
	int m_firstChar;
	int m_lastChar;
	int m_visualIndex;

public:
	GlyphCluster(const CharStyle* style, LayoutFlags flags, int first, int last, PageItem* o, int i);

	const CharStyle& style()  const;
	bool hasFlag(LayoutFlags f) const ;
	void setFlag(LayoutFlags f);
	void clearFlag(LayoutFlags f);

	QList<GlyphLayout>& glyphs();
	const QList<GlyphLayout>& glyphs() const;
	PageItem* object() const;

	int firstChar() const;
	int lastChar() const;
	int visualIndex() const;

	double width() const;
	double ascent() const;
	double desent() const;
};

#endif // GLYPHRUN_H
