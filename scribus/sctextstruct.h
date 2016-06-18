/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef SCTEXTSTRUCT_H
#define SCTEXTSTRUCT_H

#ifdef HAVE_CONFIG_H
#include "scconfig.h"
#endif

#include "scribusapi.h"

#include <QString>

#include "scfonts.h"
#include "style.h"
#include "styles/charstyle.h"
#include "styles/paragraphstyle.h"

class PageItem;
class Mark;
class ScribusDoc;

/* Strucure for Pageitem Text */


/*
 *  sctext.h
 *  Scribus
 *
 *  Created by Andreas Vox on 29.07.05.
 *  Copyright 2005 under GPL2. All rights reserved.
 *
 */

// from charstlye.h ScStyleFlags
enum LayoutFlags {
	ScLayout_None			= 0,
	ScLayout_BulletNum		= 1<<0, 	// marks list layout glyphs
	ScLayout_FixedSpace		= 1<<1, 	// marks a fixed space
	ScLayout_ExpandingSpace		= 1<<2, 	// marks an expanding space
	ScLayout_ImplicitSpace		= 1<<3, 	// marks an implicit space
	ScLayout_TabLeaders		= 1<<4, 	// marks a tab with fillchar
	ScLayout_HyphenationPossible	= 1<<7, 	// marks possible hyphenation point
	ScLayout_DropCap		= 1<<11,
	ScLayout_SuppressSpace		= 1<<12,	// internal use in PageItem (Suppresses spaces when in Block alignment)
	ScLayout_SoftHyphenVisible	= 1<<13,	// marks when a possible hyphenation point is used (st end of line)
	ScLayout_StartOfLine		= 1<<14,	// marks the start of line
	ScLayout_Underlined		= 1<<15,	// marks underlined glyphs
	ScLayout_LineBoundry		= 1<<16,	// marks possible line breaking point
	ScLayout_RightToLeft		= 1<<17,	// marks right-to-left glyph
	ScLayout_SmallCaps		= 1<<18,	// marks small caps glyph
	ScLayout_CJKFence		= 1<<19,	// marks CJK fence glyph that needs spacing adjustment at start of line
	ScLayout_JustificationTracking	= 1<<20		// marks place of tracking in justification (e.g. for Thai)
};


/**
 * This struct stores a positioned glyph. This is the result of the layout process.
 */
struct SCRIBUS_API GlyphLayout {
	float xadvance;
	float yadvance;
	float xoffset;
	float yoffset;
	double scaleV;
	double scaleH;
	uint glyph;
	
	GlyphLayout() : xadvance(0.0f), yadvance(0.0f), xoffset(0.0f), yoffset(0.0f),
		scaleV(1.0), scaleH(1.0), glyph(0)
	{ }
};

class SCRIBUS_API ScText : public CharStyle
{
public:
	ParagraphStyle* parstyle; // only for parseps
	int embedded;
	Mark* mark;
	QChar ch;
	ScText() :
		CharStyle(),
		parstyle(NULL),
		embedded(0), mark(NULL), ch() {}
	ScText(const ScText& other) :
		CharStyle(other),
		parstyle(NULL),
		embedded(other.embedded), mark(NULL), ch(other.ch)
	{
		if (other.parstyle)
			parstyle = new ParagraphStyle(*other.parstyle);
		if (other.mark)
			setNewMark(other.mark);
	}
	~ScText();

	bool hasObject(ScribusDoc *doc) const;
	//returns true if given MRK is found, if MRK is NULL then any mark returns true
	bool hasMark(Mark * MRK = NULL) const;
	QList<PageItem*> getGroupedItems(ScribusDoc *doc);
	PageItem* getItem(ScribusDoc *doc);
private:
	void setNewMark(Mark* mrk);
};


/** @brief First Line Offset Policy
 * Set wether the first line offset is based on max glyph height
 * or some of predefined height.
 * I put a prefix because it could easily conflict 
 */
enum FirstLineOffsetPolicy
{
    FLOPRealGlyphHeight = 0, // Historical
    FLOPFontAscent	 = 1,
    FLOPLineSpacing  = 2,
	FLOPBaselineGrid = 3
};


#endif // SCTEXTSTRUCT_H

