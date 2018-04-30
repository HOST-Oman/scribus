/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          scribuscore.h  -  description
                             -------------------
    copyright            : Scribus Team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef SCRIBUSCORE_H
#define SCRIBUSCORE_H

#include <QObject>
#include <QList>
#include "scribus.h"
#include "scribusapi.h"

#include "colormgmt/sccolormgmtengine.h"

class QWidget;
class FileWatcher;
class IconManager;
class PluginManager;
class ScSplashScreen;
class ScribusMainWindow;
class PrefsManager;
class UndoManager;

extern SCRIBUS_API ScribusCore* ScCore;

/**
	@author Craig Bradney <cbradney@zip.com.au>
	@brief The core functions moved from the old ScribusMainWindow class
	NO WINDOWS
	NO VIEWS
	NO DOCS
	NO PAGEITEMS
*/

class SCRIBUS_API ScribusCore : public QObject
{
Q_OBJECT

public:
	ScribusCore();
	~ScribusCore();
	
	ScSplashScreen* splash() {return m_SplashScreen;}
	/*
	int exportToPDF() {return 0;}
	int exportToEPS() {return 0;}
	int exportToSVG() {return 0;}
	int runScript() {return 0;}
	*/	
	int init(bool useGUI, const QList<QString>& filesToUse);
	int initScribusCore(bool showSplash, bool showFontInfo, bool showProfileInfo, const QString newGuiLanguage);
	bool initialized() const {return m_ScribusInitialized;}
	const QString& getGuiLanguage() const;

	void initSplash(bool showSplash);
	bool initFonts(bool showFontInfo);
	void showSplash(bool);
	bool splashShowing() const;
	void closeSplash();
	void setSplashStatus(const QString&);
	bool usingGUI() const;
	int startGUI(bool showSplash, bool showFontInfo, bool showProfileInfo, const QString newGuiLanguage);
	/**
	* @brief Are we trying to adhere to Apple Mac HIG ?
	* @retval bool true if we are on Qt/Mac
	*/
	bool isMacGUI() const;
	/**
	* @brief Are we executing Scribus on Windows ?
	* @retval bool true if we are on Qt/Win
	*/
	bool isWinGUI() const;
	bool haveCMS() const {return m_HaveCMS;}
	bool haveGS() const {return m_HaveGS;}
	bool havePNGAlpha() const {return m_HavePngAlpha;}
	bool haveTIFFSep() const {return m_HaveTiffSep;}
	void getCMSProfiles(bool showInfo);
	void getCMSProfilesDir(QString pfad, bool showInfo, bool recursive);
	void InitDefaultColorTransforms(void);
	void ResetDefaultColorTransforms(void);
	bool fileWatcherActive() const;
	void recheckGS();
	
	//Main Window members
	ScribusMainWindow* primaryMainWindow();
	
	PluginManager* pluginManager;	
	FileWatcher* fileWatcher;
	
	ProfilesL InputProfiles;
	ProfilesL InputProfilesCMYK;
	ProfilesL MonitorProfiles;
	ProfilesL PrinterProfiles;
	ProfilesL PDFXProfiles;
	ProfilesL LabProfiles;

	ScColorMgmtEngine defaultEngine;
	ScColorProfile   monitorProfile;
	ScColorProfile   defaultRGBProfile;
	ScColorProfile   defaultCMYKProfile;
	ScColorProfile   defaultLabProfile;
	ScColorTransform defaultRGBToScreenSolidTrans;
	ScColorTransform defaultRGBToScreenImageTrans;
	ScColorTransform defaultCMYKToScreenImageTrans;
	ScColorTransform defaultRGBToCMYKTrans;
	ScColorTransform defaultCMYKToRGBTrans;
	ScColorTransform defaultLabToRGBTrans;
	ScColorTransform defaultLabToCMYKTrans;
	ScColorTransform defaultLabToScreenTrans;
	
protected:
	void initCMS();
	
	QList<ScribusMainWindow*> m_ScMWList;
	int m_currScMW;
	
	ScSplashScreen *m_SplashScreen;
	IconManager *m_iconManager;
	UndoManager *m_undoManager;
	PrefsManager *m_prefsManager;
	bool m_ScribusInitialized;
	bool m_UseGUI;
	QList<QString> m_Files;
	bool m_HaveCMS;
	bool m_HaveGS;
	bool m_HavePngAlpha;
	bool m_HaveTiffSep;
};

/*


*/

#endif
