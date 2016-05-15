/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
						main.cpp  -  description
							-------------------
	begin                : Fre Apr  6 21:47:55 CEST 2001
	copyright            : (C) 2001 by Franz Schmid
	email                : Franz.Schmid@altmuehlnet.de
	copyright            : (C) 2004 by Alessandro Rimoldi
	email                : http://ideale.ch/contact
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

#include <iostream>
#include <signal.h>

#include <QApplication>
#include <QMessageBox>

#define BASE_QM "scribus"

#include "scribusapp.h"
#include "scribuscore.h"
#include "scribus.h"
#include "scimagecachemanager.h"

#include "scconfig.h"

int mainApp(int argc, char **argv);
void initCrashHandler();
static void defaultCrashHandler(int sig);

ScribusCore SCRIBUS_API *ScCore;
ScribusMainWindow SCRIBUS_API *ScMW;
ScribusQApp SCRIBUS_API *ScQApp;
bool emergencyActivated;

int main(int argc, char *argv[])
{
	return mainApp(argc, argv);
}

/*!
\author Franz Schmid
\author Alessandro Rimoldi
\author Craig Bradney
\date Mon Feb  9 14:07:46 CET 2004
\brief Launches the Gui
\param argc Number of arguments passed to Scribus
\param argv *argv list of the arguments passed to Scribus
\retval int Error code from the execution of Scribus
*/
int mainApp(int argc, char **argv)
{
	emergencyActivated=false;
#if QT_VERSION >= 0x050600
	ScribusQApp::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	ScribusQApp app(argc, argv);
	initCrashHandler();
	app.parseCommandLine();
	int appRetVal=app.init();
	if (appRetVal==EXIT_FAILURE)
		return(EXIT_FAILURE);
	if (app.useGUI)
		return app.exec();
	return EXIT_SUCCESS;	
}

void initCrashHandler()
{
	typedef void (*HandlerType)(int);
	HandlerType handler	= 0;
	handler = defaultCrashHandler;
	if (!handler)
		handler = SIG_DFL;
	sigset_t mask;
	sigemptyset(&mask);
#ifdef SIGSEGV
	signal (SIGSEGV, handler);
	sigaddset(&mask, SIGSEGV);
#endif
#ifdef SIGFPE
	signal (SIGFPE, handler);
	sigaddset(&mask, SIGFPE);
#endif
#ifdef SIGILL
	signal (SIGILL, handler);
	sigaddset(&mask, SIGILL);
#endif
#ifdef SIGABRT
	signal (SIGABRT, handler);
	sigaddset(&mask, SIGABRT);
#endif
	sigprocmask(SIG_UNBLOCK, &mask, 0);
}

void defaultCrashHandler(int sig)
{
	static int crashRecursionCounter = 0;
	crashRecursionCounter++;
	signal(SIGALRM, SIG_DFL);
	if (crashRecursionCounter < 2)
	{
		emergencyActivated=true;
		crashRecursionCounter++;
		QString sigHdr=QObject::tr("Scribus Crash");
		QString sigLine="-------------";
		QString sigMsg=QObject::tr("Scribus crashes due to Signal #%1").arg(sig);
		std::cout << sigHdr.toStdString() << std::endl;
		std::cout << sigLine.toStdString() << std::endl;
		std::cout << sigMsg.toStdString() << std::endl;
		ScImageCacheManager::instance().removeMasterLock();
		if (ScribusQApp::useGUI)
		{
			ScCore->closeSplash();
			ScMessageBox::critical(ScMW, sigHdr, sigMsg);
			ScMW->emergencySave();
			ScMW->close();
		}
		alarm(300);
	}
	exit(255);
}
