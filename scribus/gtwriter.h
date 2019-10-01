/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
 *   Copyright (C) 2004 by Riku Leino                                      *
 *   tsoots@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#ifndef GTWRITER_H
#define GTWRITER_H

#include "scribusapi.h"
#include "gtaction.h"

class PageItem;

/*
	gtWriter handles the writing to the scribus text frame.
*/
class SCRIBUS_API gtWriter 
{
public:
	gtWriter(bool append, PageItem *pageitem);
	~gtWriter();

	gtFrameStyle* getDefaultStyle();
	void setFrameStyle(gtFrameStyle *fStyle);
	void setParagraphStyle(gtParagraphStyle *pStyle);
	void setCharacterStyle(gtStyle *cStyle);
	void unsetFrameStyle();
	void unsetParagraphStyle();
	void unsetCharacterStyle();
	double getPreferredLineSpacing(int fontSize);
	double getPreferredLineSpacing(double fontSize);
	void append(const QString& text); // Use the styles set beforehand
	void append(const QString& text, gtStyle *style); // Use the style provided as a parameter
	void append(const QString& text, gtStyle *style, bool updatePStyle);
	void appendUnstyled(const QString& text); // Do not apply any formatting
	double getFrameWidth();
	QString getFrameName();
	bool getUpdateParagraphStyles();
	void setUpdateParagraphStyles(bool newUPS);
	bool getOverridePStyleFont();
	void setOverridePStyleFont(bool newOPSF);
	bool inNote {false};
	bool inNoteBody {false};

private:
	gtAction *m_action {nullptr};
	gtFrameStyle* m_defaultStyle {nullptr};
	gtStyle* m_currentStyle {nullptr};

/* 
   Frame style is the default style for text. Styles will be used in order so
   that if no character style is found then it will try to use paragraph
   style if no paragraph style is found frame style will be used. Last set 
   frame style will be left to the default style for the text frame.
*/
	gtFrameStyle* m_frameStyle {nullptr};
	gtStyle* m_paragraphStyle {nullptr};
	gtStyle* m_characterStyle {nullptr};
	bool m_errorSet {false};
	void setDefaultStyle();
};

#endif // WRITER_H
