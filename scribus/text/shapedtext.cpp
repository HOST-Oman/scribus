#include "shapedtext.h"

#include <QSharedData>


class ShapedTextImplementation : QSharedData
{
	
public:
	ShapedTextImplementation(ITextSource* src, int firstChar, int lastChar, ITextContext* ctx) : m_needsContext(false), m_source(src), m_context(ctx)
	{
		m_firstChar = firstChar;
		m_lastChar = lastChar < 0? src->length() - 1 : lastChar;
	}
	
	ShapedTextImplementation(const ShapedTextImplementation& o) : m_needsContext(o.m_needsContext), m_source(o.m_source), m_context(o.m_context), m_firstChar(o.m_firstChar), m_lastChar(o.m_lastChar),m_glyphs(o.m_glyphs)
	{}
	
	
	bool m_needsContext;
	const ITextSource* m_source;
	const ITextContext* m_context;
	int m_firstChar;
	int m_lastChar;
	QList<GlyphCluster> m_glyphs;
	
	/** only possible if it also cleanly splits the textsource */
	bool canSplit(int pos) const
	{
		return false; // TODO: implement
	}
	
	ShapedText split(int pos)
	{
		// TODO: implement
	}
	
	/** only possible if they are adjacent pieces of the same text source */
	bool canCombine(const QSharedPointer<ShapedTextImplementation>  other) const
	{
		return false; // TODO: implement
	}
	
	void combine(QSharedPointer<ShapedTextImplementation>  other)
	{
		// TODO: implement
	}
};





ShapedText::ShapedText(ITextSource* src, int firstChar, int lastChar, ITextContext* ctx) : p_impl(new ShapedTextImplementation(src,firstChar,lastChar,ctx)) {}
ShapedText::ShapedText(const ShapedText& other) : p_impl(other.p_impl) {}
	
bool ShapedText::needsContext() const { return p_impl->m_needsContext; }
void ShapedText::needsContext(bool b) { p_impl->m_needsContext = b; }
const ITextSource* ShapedText::source() const { return p_impl->m_source; }
int ShapedText::firstChar() const { return p_impl->m_firstChar; }
int ShapedText::lastChar() const { return p_impl->m_lastChar; }
const QList<GlyphCluster>& ShapedText::glyphs() const { return p_impl->m_glyphs; }
QList<GlyphCluster>& ShapedText::glyphs() { return p_impl->m_glyphs; }

bool ShapedText::canSplit(int pos) const { return p_impl->canSplit(pos); }
ShapedText ShapedText::split(int pos) { return p_impl->split(pos); }
bool ShapedText::canCombine(const ShapedText& other) const { return p_impl->canCombine(other.p_impl); }
void ShapedText::combine(ShapedText& other) { p_impl->combine(other.p_impl); }

