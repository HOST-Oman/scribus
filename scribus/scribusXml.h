/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                 	scribusXml.h the document xml library for scribus
                             -------------------
    begin                : Sam Jul 14 10:00:00 CEST 2001
    copyright            : (C) 2001 by Christian T�p
    email                : christian.toepp@mr-ct@gmx.de
 ***************************************************************************/

#ifndef _SCRIBUSXML_H
#define _SCRIBUSXML_H

#include <QDomDocument>
#include <QDomElement>
#include <QList>
#include <QProgressBar>

#include "scfonts.h"
#include "scribusapi.h"
#include "scribusstructs.h"
#include "styles/styleset.h"

class PageItem;
class PrefsManager;
class SCFonts;
class ScribusDoc;
class ScElemMimeData;
class ScPattern;
class ScXmlStreamWriter;
class Selection;

class SCRIBUS_API ScriXmlDoc
{

public:
	ScriXmlDoc();
	~ScriXmlDoc() {};
	/*!
	\author Frederic Dubuy <effediwhy@gmail.com>, Petr Vanek
	\date august 17th 2004, 10/03/2004
	\brief Preliminary Scribus file validator. totally rewritten when fixing crash bug #1092. It's much simpler now.
	\param file filename of file to test
	\retval bool true = Scribus format file, false : not Scribus
	*/
	bool readElemHeader(const QString& file, bool isFile, double *x, double *y, double *w, double *h);
	bool readElem(const QString& fileNameOrData, SCFonts &avail, ScribusDoc *doc, double xPos, double yPos, bool isDataFromFile, bool loc, QMap<QString,QString> &FontSub);
	bool readElemToLayer(const QString& fileNameOrData, SCFonts &avail, ScribusDoc *doc, double xPos, double yPos, bool isDataFromFile, bool loc, QMap<QString,QString> &FontSub, int toLayer);
	
	static QString writeElem(ScribusDoc *doc, Selection *selection);
	static ScElemMimeData* writeToMimeData(ScribusDoc *doc, Selection *selection);

private:
	static QList<PageItem*> getItemsFromSelection(ScribusDoc *doc, Selection* selection);
};

#endif





