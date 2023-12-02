/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
	begin                : May 2005
	copyright            : (C) 2005 by Craig Bradney
	email                : cbradney@scribus.info
***************************************************************************/

/***************************************************************************
*                                                                         *
*   Scribus program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef SCRIBUSAPP_H
#define SCRIBUSAPP_H

#include <QApplication>
#include <QString>
#include <QStringList>

#include "scribusapi.h"

class ScribusCore;
class ScribusMainWindow;
class ScDLManager;

class SCRIBUS_API ScribusQApp : public QApplication
{
	Q_OBJECT

	public:
		ScribusQApp( int & argc, char ** argv );
		~ScribusQApp();

		int init();
		void initLang();
		void initDLMgr();
		void parseCommandLine();

		void changeGUILanguage(const QString & newGUILang);
		void changeIconSet(const QString& newIconSet);
		void setLocale();
		void changeLabelVisibility(bool visibility);

		/*!
		\author Franz Schmid
		\author Alessandro Rimoldi
		\date Mon Feb  9 14:07:46 CET 2004
		\brief If the lang argument is empty, returns the value in the locales
		The lang is always a two character code, except for "en_GB" where
		the whole string is returned. For all the other locales starting
		with "en", no locale is returned.
		(Inspired from Klocale.cpp)
		\param lang QString a two letter string describing the lang environment
		\retval QStringList A string describing the language environment
		*/
		QStringList getLang(QString m_lang);
		/*!
		\author Franz Schmid
		\author Alessandro Rimoldi
		\date Mon Feb  9 14:07:46 CET 2004
		\brief Loads the translations for Scribus and for the Plugins
		\param langs QString a two letter string describing the lang environment
		*/
		void installTranslators(const QStringList & langs);

		const ScribusCore* core() {return m_ScCore;}
		static bool useGUI;
		void neverSplash(bool splashOff);
		bool neverSplashExists();
		const QString& currGUILanguage() { return m_GUILang; }
		const QString& userPrefsDir() { return m_prefsUserDir; }
		ScDLManager* dlManager() { return m_scDLMgr; }
		QString pythonScript; // script to be run in python from CLI
		QStringList pythonScriptArgs; // command line arguments and flags for script from CLI

	private:
		void showHeader();
		void showVersion();
		/*!
		\author Franz Schmid
		\author Alessandro Rimoldi
		\date Mon Feb  9 14:07:46 CET 2004
		\brief If no argument specified the lang, returns the one in the locales
		*/
		void showUsage();
		/*!
		\author Craig Bradney
		\date Wed Nov 18 2004
		\brief Instantiates the Language Manager and prints installed languages with brief instructions around
		*/
		void showAvailLangs();

		ScribusCore* m_ScCore {nullptr};
		QString m_lang;
		QString m_GUILang {"en_GB"};
		QLocale m_locale;
		bool m_showSplash {true};
		bool m_showFontInfo {false};
		bool m_showProfileInfo {false};
		bool m_use_indigo_ui {false};
		//! \brief If is there user given prefs file...
		QString m_prefsUserDir;
		QList<QString> m_filesToLoad;
		QString m_fileName;
		ScDLManager *m_scDLMgr {nullptr};

	protected:
		virtual bool event(QEvent *event);

	protected slots:
		void downloadComplete(const QString& t);

	signals:
		void appStarted();
		void iconSetChanged();
		void localeChanged();
		void labelVisibilityChanged(bool);
};

extern SCRIBUS_API ScribusQApp * ScQApp;

#endif
