/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
	copyright            : (C) 2006 by Craig Bradney
	email                : cbradney@zip.com.au
***************************************************************************/

#ifndef COLORSETMANAGER_H
#define COLORSETMANAGER_H

#include "scribusapi.h"
#include "sccolor.h"
#include "scribusdoc.h"
#include "vgradient.h"
#include "scpattern.h"
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTreeWidget>

struct ApplicationPrefs;

class SCRIBUS_API ColorSetManager
{
	public:
		ColorSetManager();
		~ColorSetManager();
		
		void initialiseDefaultPrefs(struct ApplicationPrefs& appPrefs);
		void findPaletteLocations();
		void searchDir(const QString& path, QMap<QString, QString> &pList, QTreeWidgetItem* parent = nullptr);
		void findPalettes(QTreeWidgetItem* parent = nullptr);
		void findUserPalettes(QTreeWidgetItem* parent = nullptr);
		QStringList paletteNames();
		QStringList userPaletteNames();
		
		QString paletteFileFromName(const QString& paletteName);
		QString userPaletteFileFromName(const QString& paletteName);
		bool paletteLocationLocked(const QString& palettePath);
		bool checkPaletteFormat(const QString& paletteFileName);
		bool loadPalette(const QString& paletteFileName, ScribusDoc *doc, ColorList &colors, QHash<QString,VGradient> &gradients, QHash<QString, ScPattern> &patterns, bool merge);
		
	protected:
		QStringList paletteLocations;
		QMap<QString, QString> palettes;
		QMap<QString, QString> userPalettes;
		QMap<QString, bool> paletteLocationLocks;
};
#endif
