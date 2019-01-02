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
 

#include <QDebug>
#include <QDir>
#include <QtGlobal>
#include <QFileInfo>
#include <QMap>
#include <QObject>
#include <QStringList> 

#include "scconfig.h"
#include "localemgr.h"
#include "scpaths.h"

LocaleManager * LocaleManager::m_instance = nullptr;

LocaleManager * LocaleManager::instance()
{
	if(!m_instance)
	{
		m_instance = new LocaleManager;
		Q_ASSERT(m_instance);
		m_instance->init();
	}
	return m_instance;
}

void LocaleManager::deleteInstance()
{
	delete m_instance;
	m_instance = nullptr;
}

void LocaleManager::init()
{
	m_sysLocale=QLocale::system();
//	qDebug()<<"language:"<<QLocale::languageToString(sysLocale.language());
//	qDebug()<<"measurement:"<<sysLocale.measurementSystem();
//	qDebug()<<"bcp47name:"<<sysLocale.bcp47Name();

	generateLocaleList();
}

void LocaleManager::generateLocaleList()
{
	//Build table;
	//No, we don't translate these, they are internal use that don't get to the GUI
	m_localeTable.clear();
	m_localeTable.append(LocaleDef("default","mm",   "A4"   ));
	m_localeTable.append(LocaleDef("en",     "in",   "Letter"));
	m_localeTable.append(LocaleDef("en_AU",  "mm",   "A4"   ));
	m_localeTable.append(LocaleDef("en_GB",  "mm",   "A4"   ));
	m_localeTable.append(LocaleDef("en_US",  "in",   "Letter"));
	m_localeTable.append(LocaleDef("fr",     "mm",   "A4"   ));
	m_localeTable.append(LocaleDef("fr_QC",  "pica", "Letter"));
}

void LocaleManager::printSelectedForLocale(const QString& locale)
{
	QString selectedLocale(locale);
	for (int i = 0; i < m_localeTable.size(); ++i)
	{
		if (m_localeTable[i].m_locale==selectedLocale)
		{
			qDebug()<<m_localeTable[i].m_locale.leftJustified(6) << ": " << m_localeTable[i].m_unit << ": " << m_localeTable[i].m_pageSize;
			return;
		}
	}

	qDebug()<<"No definition for locale: "<<selectedLocale;
	selectedLocale="default";
	for (int i = 0; i < m_localeTable.size(); ++i)
	{
		if (m_localeTable[i].m_locale==selectedLocale)
		{
			qDebug()<<m_localeTable[i].m_locale.leftJustified(6) << ": " << m_localeTable[i].m_unit << ": " << m_localeTable[i].m_pageSize;
			return;
		}
	}
}

QString LocaleManager::pageSizeForLocale(const QString& locale)
{
	for (int i = 0; i < m_localeTable.size(); ++i)
	{
		if (m_localeTable[i].m_locale==locale)
			return m_localeTable[i].m_pageSize;
	}

	//qDebug()<<"No definition for locale: "<<selectedLocale;
	//No, we don't translate these, they are internal use that don't get to the GUI
	if (m_sysLocale.measurementSystem()==0)
		return "A4";
	return "Letter";
//	qFatal("Page Size not found in LocaleManager");
//	return "";
}

QString LocaleManager::unitForLocale(const QString &locale)
{
	for (int i = 0; i < m_localeTable.size(); ++i)
	{
		if (m_localeTable[i].m_locale==locale)
			return m_localeTable[i].m_unit;
	}
	//qDebug()<<"No definition for locale: "<<selectedLocale;
	//No, we don't translate these, they are internal use that don't get to the GUI
	if (m_sysLocale.measurementSystem()==0)
		return "mm";
	return "in";
//	qFatal("Unit not found in LocaleManager");
//	return "";
}

LocaleManager::~LocaleManager()
{
	m_localeTable.clear();
}


