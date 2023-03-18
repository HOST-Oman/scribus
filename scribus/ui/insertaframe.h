/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
//
//
// Author: Craig Bradney <cbradney@scribus.info>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef INSERTAFRAME_H
#define INSERTAFRAME_H

#include <QButtonGroup>
#include <QDialog>
#include <QMap>
#include <QString>

#include "gtgettext.h"
#include "ui_insertaframe.h"
#include "scribusapi.h"
#include "usertaskstructs.h"

class PageItem;
class ScribusDoc;

class SCRIBUS_API InsertAFrame : public QDialog, Ui::InsertAFrame
{
    Q_OBJECT

public:
	InsertAFrame(QWidget* parent, ScribusDoc *doc);
	~InsertAFrame(){};
	
	void getNewFrameProperties(InsertAFrameData &iafData);
protected:
	ScribusDoc* m_Doc;
	ImportSetup m_ImportSetup;
	QButtonGroup *typeButtonGroup;
	QButtonGroup *pagePlacementButtonGroup;
	QButtonGroup *framePositionButtonGroup;
	QButtonGroup *sizeButtonGroup;
	QHash<PageItem*, QString> pageItemMap;
	
protected slots:
	void slotSelectType(int id);
	void slotSelectPagePlacement(int id);
	void slotCreatePageNumberRange();
	void slotSelectPosition(int id);
	void slotSelectSize(int id);
	void slotLinkToExistingFrame(int state);
	void locateImageFile();
	void locateDocFile();
	void iconSetChange();
};

#endif

