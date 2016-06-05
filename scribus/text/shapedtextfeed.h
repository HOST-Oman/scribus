#ifndef SHAPEDTEXTFEED_H
#define SHAPEDTEXTFEED_H


#include <QList>

#include "textshaper.h"

class GlyphCluster;
class ITextContext;
class ITextSource;

/**
 * This is a small helper class that automatically calls the shaper when more
 * input is needed.
 */
class ShapedTextFeed
{
	ITextContext* m_context;
	ITextSource* m_textSource;
	TextShaper m_shaper;
	int m_endChar;
	
public:
	ShapedTextFeed(ITextSource* source, int startChar, ITextContext* context);
	
	bool haveMoreText(int glyphPos, QList<GlyphCluster>& glyphs);
	
	static QList<GlyphCluster> putInVisualOrder(const QList<GlyphCluster>& glyphs, int start, int end);
	
};

#endif


