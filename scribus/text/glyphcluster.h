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
	double m_scaleH;
	double m_scaleV;
	double m_extraWidth;
	double m_xoffset;
	double m_yoffset;

public:
	GlyphCluster(const CharStyle* style, LayoutFlags flags, int first, int last, PageItem* o, int i);

	void append(GlyphLayout&);

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
	void addToExtraWidth(double);

	double xoffset() const;
	double yoffset() const;
	void setXOffset(double);
	void setYOffset(double);
	void addToXOffset(double);
	void addToYOffset(double);

	double ascent() const;
	double desent() const;

	double scaleH() const;
	double scaleV() const;
	void setScaleH(double);
	void setScaleV(double);
};

#endif // GLYPHRUN_H
