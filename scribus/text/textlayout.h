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

#ifndef TEXTLAYOUT_H
#define TEXTLAYOUT_H

#include <QList>

#include "scribusapi.h"

#include "fpoint.h"
#include "frect.h"
#include "scpainter.h"
#include "sctextstruct.h"

class StoryText;
class GroupBox;
class LineBox;

struct PathData
{
	float PtransX;
	float PtransY;
	float PRot;
	float PDx;
};


/**
	This class manages the physical layout of a textframe, ie. its line 
	structure and the lines' glyph layouts. It will use some of the layouters above to create a Box.
    It listens to change events from the StoryText and the PageItem and will update the layout accordingly.
 */
class SCRIBUS_API TextLayout
{
public:
	TextLayout(StoryText* text, PageItem* frame);
	~TextLayout();
	
	bool overflows() const;
	
	StoryText* story() { return m_story; }
	const StoryText* story() const { return m_story; }
	void setStory(StoryText* story);

	int startOfLine(int pos) const;
	int endOfLine(int pos) const;
	int prevLine(int pos) const;
	int nextLine(int pos) const;
	int startOfFrame() const;
	int endOfFrame() const;

	int screenToPosition(FPoint coord) const;
 	FRect boundingBox(int pos, uint len = 1) const;

	uint lines() const;
	
	const LineBox*  line(uint i) const;
	const PathData& point(int pos) const;
	PathData& point(int pos);

	void appendLine(const LineBox* ls);
	void removeLastLine ();

	void clear();

protected:
	friend class FrameControl;
	
	StoryText* m_story;
    PageItem* m_frame;
	GroupBox* m_lines;
	
	QVector<PathData> m_path;
	bool m_validLayout;
	mutable qreal m_magicX;
	mutable int m_lastMagicPos;

};

#endif
