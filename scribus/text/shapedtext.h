/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */
/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#ifndef SHAPEDTEXT_H
#define SHAPEDTEXT_H

#include <QList>
#include <QSharedPointer>

#include "itextcontext.h"
#include "itextsource.h"
#include "glyphcluster.h"




class ShapedTextImplementation;
class ShapedTextCacheImplementation;
class ShapedTextFeedImplementation;


/**
 * This class holds the shaped glyphs for a range of characters. The glyphs are always in logical order.
 */
class ShapedText 
{
    QSharedPointer<ShapedTextImplementation> p_impl;

public:
	ShapedText(ITextSource* src, int firstChar, int lastChar, ITextContext* ctx = 0);
	ShapedText(const ShapedText& other);
	
	bool needsContext() const;
	void needsContext(bool);
	
	// original text and link to styles
	const ITextSource* source() const;
	int firstChar() const;
	int lastChar() const;
	
	// shaped clusters
	const QList<GlyphCluster>& glyphs() const;
	QList<GlyphCluster>& glyphs();
	
	/** only possible if it also cleanly splits the textsource */
	bool canSplit(int pos) const;
	ShapedText split(int pos);
	/** only possible if they are adjacent pieces of the same text source */
	bool canCombine(const ShapedText& other) const;
	void combine(ShapedText& other);
};


class ShapedTextCache
{
	QSharedPointer<ShapedTextCacheImplementation> p_impl;
	
public:
	bool contains(int charPos, uint len = 1) const;
	const ShapedText* get(int charPos, uint minLen=1) const;
	void put(const ShapedText& txt);
};


#endif
