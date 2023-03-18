/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
    begin                : Feb 2005
    copyright            : (C) 2005 by Craig Bradney
    email                : cbradney@scribus.info
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef PAGESIZE_H
#define PAGESIZE_H

#include <QMap>
#include <QString>
#include "scribusapi.h"
#include "units.h"

struct PageSizeInfo
{
	double width;
	double height;
	QString trSizeName;
	int pageUnitIndex;
};

typedef QMap<QString, PageSizeInfo> PageSizeInfoMap;

class SCRIBUS_API PageSize
{
public:
	PageSize(const QString&);
	PageSize(const double, const double);
	PageSize& operator=(const PageSize& other);
	void init(const QString&);
	QString name() const { return m_pageSizeName; }
	QString nameTR() const { return m_trPageSizeName; }
	double width() const { return m_width; }
	double height() const { return m_height; }
	double originalWidth() const { return m_width * unitGetRatioFromIndex(m_pageUnitIndex); }
	double originalHeight() const { return m_height * unitGetRatioFromIndex(m_pageUnitIndex); }
	QString originalUnit() const { return unitGetSuffixFromIndex(m_pageUnitIndex); }
	QStringList sizeList() const;
	QStringList sizeTRList() const;
	QStringList activeSizeList() const;
	QStringList activeSizeTRList() const;
	void generateSizeList();
	void printSizeList() const;
	QStringList untransPageSizeList(const QStringList &transList);

private:
	PageSizeInfoMap m_pageSizeList;
	double m_width { 0.0 };
	double m_height { 0.0 };
	int m_pageUnitIndex { -1 };
	QString m_pageSizeName;
	QString m_trPageSizeName;
};

#endif

