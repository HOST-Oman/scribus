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
#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QKeySequence>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>
#include <QVector>
#include <QPointer>
#include <QMultiHash>
#include <QPixmap>
#include <QActionGroup>

class QEvent;

#include "scribusapi.h"
#include "scraction.h"

class IconManager;
class ScribusDoc;
class ScribusMainWindow;
class ScribusQApp;
class ScribusView;
class UndoManager;
/**
@author Craig Bradney
*/
class SCRIBUS_API ActionManager : public QObject
{
	Q_OBJECT

	friend class StoryEditor;
	public:
		ActionManager ( QObject * parent );
		~ActionManager() override;
		
		virtual void changeEvent(QEvent *e);
		
		void init(ScribusMainWindow *);
		bool compareKeySeqToShortcut(const QKeySequence& ks, const QString& actionName);
		bool compareKeySeqToShortcut(int k, Qt::KeyboardModifiers km, const QString& actionName);
		static void createDefaultShortcuts();
		static QMap<QString, QKeySequence>* defaultShortcuts() {return &defKeys;}
		static void createDefaultMenus();
		static void createDefaultMenuNames();
		static void createDefaultNonMenuActions();
		static void createDefaultNonMenuNames();
		static QVector< QPair<QString, QStringList> >* defaultMenuNames() {return &defMenuNames;}
		static QVector< QPair<QString, QStringList> >* defaultNonMenuNames() {return &defNonMenuNames;}
		static QString defaultMenuNameEntryTranslated(const QString& index);
		static QVector< QPair<QString, QStringList> >* defaultMenus() {return &defMenus;}
		static QVector< QPair<QString, QStringList> >* defaultNonMenuActions() {return &defNonMenuActions;}
		void createActions();
		void disconnectModeActions();
		void connectModeActions();
		void disconnectNewViewActions();
		void connectNewViewActions(ScribusView *);
		void disconnectNewDocActions();
		void connectNewDocActions(ScribusDoc *);
		void disconnectNewSelectionActions();
		void connectNewSelectionActions(ScribusView *,ScribusDoc *);
		void saveActionShortcutsPreEditMode();
		void restoreActionShortcutsPostEditMode();
		void enableActionStringList(QMap<QString, QPointer<ScrAction> > *actionMap, QStringList *list, bool enabled, bool checkingUnicode=false, const QString& fontName=QString());
		void enableUnicodeActions(QMap<QString, QPointer<ScrAction> > *actionMap, bool enabled, const QString& fontName=QString());
		void setPDFActions(ScribusView *);
		
	public slots:
		void languageChange();
		void handleMultipleSelections(bool isMultiple);
		
	protected:
		void initFileMenuActions();
		void initEditMenuActions();
		void initStyleMenuActions();
		void initItemMenuActions();
		void initInsertMenuActions();
		void initPageMenuActions();
		void initTableMenuActions();
		void initViewMenuActions();
		void initToolsMenuActions();
		void initExtrasMenuActions();
		void initWindowsMenuActions();
		void initScriptMenuActions();
		void initHelpMenuActions();
		static void initUnicodeActions(QMap<QString, QPointer<ScrAction> > *actionMap, QWidget *actionParent, QStringList *actionNamesList);
		void initSpecialActions();
		static void setActionTooltips(QMap<QString, QPointer<ScrAction> > *actionMap);
		static void languageChangeUnicodeActions(QMap<QString, QPointer<ScrAction> > *actionMap);
		void languageChangeActions();
		static QKeySequence defaultKey(const QString &actionName);
	
		QPixmap noIcon;
		ScribusMainWindow *mainWindow {nullptr};
		UndoManager *undoManager {nullptr};
		IconManager& im;
		QMap<QString, QPointer<ScrAction> > *scrActions {nullptr};
		QMultiHash<QString, QActionGroup*> *scrActionGroups {nullptr};
		QStringList *modeActionNames {nullptr};
		QStringList *nonEditActionNames {nullptr};
		QStringList *unicodeCharActionNames {nullptr};
		static QMap<QString, QKeySequence> defKeys;
		static QVector< QPair<QString, QStringList> > defMenuNames;
		static QVector< QPair<QString, QStringList> > defMenus;
		static QVector< QPair<QString, QStringList> > defNonMenuNames;
		static QVector< QPair<QString, QStringList> > defNonMenuActions;
};

#endif
