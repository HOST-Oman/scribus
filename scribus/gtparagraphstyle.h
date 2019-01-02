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

#ifndef GTPARAGRAPHSTYLE_H
#define GTPARAGRAPHSTYLE_H

#include "scribusapi.h"
#include "gtstyle.h"
#include "sctextstruct.h"

enum Alignment {
	LEFT,
	CENTER,
	RIGHT,
	BLOCK,
	FORCED,
	AlignmentMAX
};

enum TabType {
	LEFT_T,
	RIGHT_T,
	FULL_STOP_T,
	COMMA_T,
	CENTER_T
};

class SCRIBUS_API gtParagraphStyle : public gtStyle
{
private:
	void init();
protected:
	int    flags;
	bool   defaultStyle;
	double lineSpacing;
	int    alignment;
	double indent;
	double firstLineIndent;
	double spaceAbove;
	double spaceBelow;
	QList<ParagraphStyle::TabRecord> tabValues;
	bool dropCap;
	int  dropCapHeight;
	bool m_bullet; ///< Is style using bullet?
	QString m_bulletStr; ///< String used as bullet
	bool m_numeration; ///< Is style using numeration?
	int m_numLevel; ///< Level in hierarchical numbering
	int m_numFormat; ///< 1_2_3, i_ii_ii, a_b_c
	int m_numStart; ///< Numeration starts at number
	QString m_numPrefix; ///< Numeration prefix of style
	QString m_numSuffix; ///< Numeration sufffix of style
	bool adjToBaseline;
	bool autoLineSpacing;
	bool isVisible;
public:

	typedef enum
	{
		lineSpacingWasSet = 1,
		alignmentWasSet = 2,
		indentWasSet = 4,
		firstIndentWasSet = 8,
		spaceAboveWasSet  = 16,
		spaceBelowWasSet = 32,
		tabValueWasSet = 64,
		fillShadeWasSet = 128,
		dropCapWasSet = 256,
		dropCapHeightWasSet = 512,
		adjToBaselineWasSet = 1024,
		autoLineSpacingWasSet  = 2048,
		bulletWasSet = 4096,
		numWasSet = 8192
	} wasSetFlags;

	int    getFlags();
	bool   isDefaultStyle();
	void   setDefaultStyle(bool defStyle);
	double getLineSpacing();
	void   setLineSpacing(double newLineSpacing);
	bool   getAutoLineSpacing();
	void   setAutoLineSpacing(bool newALS);
	int    getAlignment();
	void   setAlignment(Alignment newAlignment);
	void   setAlignment(int newAlignment);
	double getIndent();
	void   setIndent(double newIndent);
	double getFirstLineIndent();
	void   setFirstLineIndent(double newFirstLineIndent);
	double getSpaceAbove();
	void   setSpaceAbove(double newSpaceAbove);
	double getSpaceBelow();
	void   setSpaceBelow(double newSpaceBelow);
	QList<ParagraphStyle::TabRecord>* getTabValues();
	void   setTabValue(double newTabValue, TabType ttype = LEFT_T);
	bool   hasDropCap();
	void   setDropCap(bool newDropCap);
	void   setDropCap(int newHeight);
	int    getDropCapHeight();
	void   setDropCapHeight(int newHeight);
	bool   isAdjToBaseline();
	void   setAdjToBaseline(bool newAdjToBaseline);
	void   getStyle(gtStyle* style);
	gtParagraphStyle(const QString& name);
	gtParagraphStyle(const gtParagraphStyle& p);
	gtParagraphStyle(const gtStyle& s);
	~gtParagraphStyle();
	QString target();
	bool hasBullet();
	void setBullet(bool newBullet, const QString& str);
	QString getBullet();
	bool hasNum();
	void setNum(bool newNum, int format=0, int level=0, int start = 1, const QString& prefix = "", const QString& suffix = "");
	int getNumLevel();
	int getNumFormat();
	int getNumStart();
	QString getNumPrefix();
	QString getNumSuffix();
};

#endif // GTPARAGRAPHSTYLE_H
