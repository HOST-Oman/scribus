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
	QString m_str;

public:
	GlyphCluster(const CharStyle* style, LayoutFlags flags, int first, int last, PageItem* o, int i, QString str);

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

	double ascent() const;
	double desent() const;

	double scaleH() const;
	double scaleV() const;
	void setScaleH(double);
	void setScaleV(double);

	bool isEmpty() const;
	bool isControlGlyphs() const;
	bool isSpace() const;
	QVector<FPointArray> glyphClusterOutline() const;
	// get text out
	QString getText() const;

	double extraWidth;
	double xoffset;
	double yoffset;
};

#endif // GLYPHRUN_H
