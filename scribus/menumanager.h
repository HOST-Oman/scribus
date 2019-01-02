/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
    begin                : Jan 2005
    copyright            : (C) 2005 by Craig Bradney
    email                : cbradney@zip.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QAction>
#include <QObject>
#include <QPoint>
#include <QMenu>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QString>

class QMenuBar;

#include "scribusapi.h"
#include "actionmanager.h"
class ScrAction;
class ScribusMainWindow;

/**
@author Craig Bradney
*/
class SCRIBUS_API MenuManager : public QObject
{
	Q_OBJECT
	public:
		MenuManager(QMenuBar* mb, QObject *parent = 0);
		~MenuManager();

		enum MenuType {Normal, DLL};

		bool addMenuToWidgetOfAction(const QString &menuName, ScrAction *action);
		bool createMenu(const QString &menuName, const QString &menuText = QString::null, const QString& parent = QString::null, bool checkable = false, bool rememberMenu = false);
		void removeMenuItem(const QString& s, ScrAction *menuAction, const QString &parent);
		bool removeMenuItem(ScrAction *menuAction, const QString &parent);
		void generateKeyManList(QStringList *actionNames);
		void runMenuAtPos(const QString &, const QPoint);
		void setMenuEnabled(const QString &menuName, const bool enabled);
		void setText(const QString &menuName, const QString &menuText);



		QMenu *getLocalPopupMenu(const QString &menuName);
		bool addMenuStringToMenuBar(const QString &menuName, bool rememberMenu=false);
		bool addMenuStringToMenuBarBefore(const QString &, const QString &beforeMenuName);
		void clear();
		bool clearMenu(const QString &menuName);
		bool empty();
		bool menuExists(const QString &menuName);
		void addMenuItemString(const QString& s, const QString &parent);
		void addMenuItemStringAfter(const QString &s, const QString &after, const QString &parent);
		void addMenuItemStringstoMenu(const QString &menuName, QMenu *menuToAddTo, const QMap<QString, QPointer<ScrAction> > &menuActions);
		void addMenuItemStringstoRememberedMenu(const QString &menuName, const QMap<QString, QPointer<ScrAction> > &menuActions);
		void addMenuItemStringstoMenuBar(const QString &menuName, const QMap<QString, QPointer<ScrAction> > &menuActions);
		void clearMenuStrings(const QString &menuName);
		void dumpMenuStrings();
		QMenu *undoMenu() {return m_undoMenu;}
		QMenu *redoMenu() {return m_redoMenu;}

	public slots:
		void languageChange();

protected:
	QMenuBar *scribusMenuBar;

	QMap<QString, QList<QString> > menuStrings;
	QMap<QString, QString> menuStringTexts;
	QMap<QString, QMenu*> menuBarMenus;
	QMap<QString, QMenu*> rememberedMenus;
	//some hacks to keep undo menu functioning for now
	QMenu *m_undoMenu;
	QMenu *m_redoMenu;
};

#endif

