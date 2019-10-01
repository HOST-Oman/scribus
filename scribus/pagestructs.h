/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PAGESTRUCTS_H
#define PAGESTRUCTS_H

#include <QMap>
#include <QList>
#include <QString>
#include "numeration.h"

struct ObjectAttribute 
{
	QString name;
	QString type;
	QString value;
	QString parameter;
	QString relationship;
	QString relationshipto;
	QString autoaddto;
};

typedef QList<ObjectAttribute> ObjAttrVector;

typedef enum {Beginning, End, NotShown} TOCPageLocation;

struct ToCSetup
{
	QString name; //Name of ToC
	QString itemAttrName; //Attribute to Scan for
	QString frameName; //Destination frame
	TOCPageLocation pageLocation; //Place the page number for the TOC at the beginning, end or not at all
	bool listNonPrintingFrames; //List non printing frames in the TOC
	QString textStyle; //Paragraph style for text
	//QString leaderParaStyle; //Paragraph style for leaders
	//QString pageNumberParaStyle; //Paragraph style for page numbers
};

typedef QList<ToCSetup> ToCSetupVector;

struct DocumentSection
{
	uint number; //Just an index in the section list
	QString name; //User defined name for the section
	uint fromindex; //First page _index_ of the section in the document (old page number)
	uint toindex; //Last page _index_ of the section in the document (old page number)
	NumFormat type; //Type of section numbering, ie i,ii,iii or a,b,c or 1,2,3, etc
	uint sectionstartindex; // Start of section, an index in the range of type, eg for type i,ii,iii, this would be 2 for "ii".
	bool reversed; // Counting 10-1 ?
	bool active; // Is the section active, ie, if the fromindex is 10, and theres 5 pages, this should be inactive.
	QChar pageNumberFillChar; //Prefix to be placed before page number
	int pageNumberWidth; //Minimum width of page number string

	bool operator==(const DocumentSection &other) const
	{
		if (number != other.number)
			return false;
		if (name != other.name)
			return false;
		if (fromindex != other.fromindex)
			return false;
		if (toindex != other.toindex)
			return false;
		if (type != other.type)
			return false;
		if (sectionstartindex != other.sectionstartindex)
			return false;
		if (reversed != other.reversed)
			return false;
		if (active != other.active)
			return false;
		if (pageNumberFillChar != other.pageNumberFillChar)
			return false;
		if (pageNumberWidth != other.pageNumberWidth)
			return false;
		return true;
	}

	inline bool operator!=(const DocumentSection &other) const
	{
		return (this->operator==(other) == false);
	}
};

typedef QMap<uint, DocumentSection> DocumentSectionMap;

typedef enum
{
	singlePage,
	doublePage,
	triplePage,
	quadroPage
} PageLayout;

typedef enum
{
	LeftPage,
	MiddlePage,
	RightPage
} PageLocation;


#endif
