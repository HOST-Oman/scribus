/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          page.h  -  description
                             -------------------
    begin                : Sat Apr 7 2001
    copyright            : (C) 2001 by Franz Schmid
    email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCPAGE_H
#define SCPAGE_H

#include <utility>

#include <QList>

#include "scribusapi.h"
#include "undostate.h"
#include "scribusstructs.h"
#include "guidemanagercore.h"

class PageItem;
class QString;
class UndoManager;
class UndoState;
class ScribusDoc;


/**
  *@author Franz Schmid
  */
class SCRIBUS_API ScPage : public UndoObject, public SingleObservable<ScPage>
{
public:
	ScPage(const double x, const double y, const double b, const double h);
	~ScPage();
	double xOffset() const { return m_xOffset; }
	double yOffset() const { return m_yOffset; }
	double width() const { return m_width; }
	double height() const { return m_height; }
	double initialWidth() const { return m_initialWidth; }
	double initialHeight() const { return m_initialHeight; }
	int orientation() const { return m_orientation; }
	void setXOffset(const double);
	void setYOffset(const double);
	void setWidth(const double);
	void setHeight(const double);
	void setInitialWidth(const double);
	void setInitialHeight(const double);
	void setOrientation(int);
	void copySizingProperties(ScPage *sourcePage, const MarginStruct& pageMargins);
	MarginStruct margins() const { return Margins; }
	double leftMargin() const { return Margins.left(); }
	double topMargin() const { return Margins.top(); }
	double bottomMargin() const { return Margins.bottom(); }
	double rightMargin() const { return Margins.right(); }

	MarginStruct Margins;
	MarginStruct initialMargins;
  /** Nummer der Seite */
	int LeftPg;
	//! Name of the master page that this page uses
	QString MPageNam;

	QString m_pageSize;
	int marginPreset;
	ScribusDoc* doc() const { return m_Doc; }
	void setDocument(ScribusDoc* doc);
	int pageNr() const { return m_pageNr; }
	void setPageNr(int pageNr);
	const QString& pageSectionNumber() const { return m_pageSectionNumber; }
	void setPageSectionNumber(const QString&);
	//! Return the page's name
	const QString& pageName() const {return m_PageName;}
	void setPageName(const QString& newName);
	void restore(UndoState* state, bool isUndo);

	/*! \brief As a bit of a dirty hack, we declare this mutable so it can be altered
	even while the object is `const'. That's normally only for internal
	implementation, but in this case it at least lets us guarantee the rest
	of the object is unchanged in (eg) pdflib. This should be replaced with
	proper access methods later. */
	mutable QList<PageItem*> FromMaster;
	//! \brief Guides lists and basic operations
	GuideManagerCore guides;
	PDFPresentationData PresentVals;

protected:
	UndoManager * const undoManager;
	void restorePageItemCreation(ScItemState<PageItem*> *state, bool isUndo);
	void restorePageItemDeletion(ScItemState< QList<PageItem*> > *state, bool isUndo);
	void restorePageAttributes(SimpleState *state, bool isUndo);
	void restorePageItemConversion(ScItemState<std::pair<PageItem*, PageItem*> >*state, bool isUndo);
	void restorePageItemConversionToSymbol(ScItemState<std::pair<PageItem*, PageItem*> >*state, bool isUndo);

	double m_xOffset;
	double m_yOffset;
	double m_width;
	double m_height;
	double m_initialWidth;
	double m_initialHeight;
	int m_pageNr;
	int m_orientation;
	//! Name of this page, currently only allowed to be used by a master page
	QString m_PageName;
	ScribusDoc* m_Doc;	
	QString m_pageSectionNumber;
};

Q_DECLARE_METATYPE(ScPage*);

#endif
