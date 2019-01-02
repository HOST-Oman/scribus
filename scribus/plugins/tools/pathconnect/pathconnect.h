/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
*   Copyright (C) 2008 by Franz Schmid                                     *
*   franz.schmid@altmuehlnet.de                                            *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
*   This program is distributed in the hope that it will be useful,        *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
*                                                                          *
*   You should have received a copy of the GNU General Public License      *
*   along with this program; if not, write to the                          *
*   Free Software Foundation, Inc.,                                        *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.              *
****************************************************************************/

#ifndef PATHCONNECT_H
#define PATHCONNECT_H

#include "fpointarray.h"
#include "pluginapi.h"
#include "scplugin.h"
#include "scribuscore.h"

class ScribusDoc;

/** \brief This is a simple "Path Connect" plugin for Scribus 1.3 and later.
\author Franz Schmid
\date November 2008
*/
class PLUGIN_API PathConnectPlugin : public ScActionPlugin
{
	Q_OBJECT

	public:
		// Standard plugin implementation
		PathConnectPlugin();
		virtual ~PathConnectPlugin();
		virtual bool run(ScribusDoc* doc, const QString& target = QString::null);
		virtual const QString fullTrName() const;
		virtual const AboutData* getAboutData() const;
		virtual void deleteAboutData(const AboutData* about) const;
		virtual void languageChange();
		virtual void addToMainWindowMenu(ScribusMainWindow *) {};

		// Special features (none)
	private:
		FPointArray computePath(int pointOne, int pointTwo, int mode, FPointArray &p1, FPointArray &p2);
		FPointArray reversePath(FPointArray &path);

		PageItem* m_item1;
		PageItem* m_item2;
		ScribusDoc* m_doc;

		FPointArray originalPath1;
		FPointArray originalPath2;
		double originalXPos;
		double originalYPos;
		
		bool firstUpdate;
	private slots:
		void updateEffect(int effectType, int pointOne, int pointTwo, int mode);
};

extern "C" PLUGIN_API int pathconnect_getPluginAPIVersion();
extern "C" PLUGIN_API ScPlugin* pathconnect_getPlugin();
extern "C" PLUGIN_API void pathconnect_freePlugin(ScPlugin* plugin);

#endif
