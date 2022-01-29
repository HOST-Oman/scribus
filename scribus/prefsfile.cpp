/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
 *   Copyright (C) 2004 by Riku Leino                                      *
 *   tsoots@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QXmlInputSource>
#include "prefsfile.h"
#include "prefsreader.h"
#include "prefstable.h"

PrefsFile::PrefsFile(const QString& pFilePath, bool write)
         : m_prefsFilePath(pFilePath),
           m_ioEnabled(write)
{
	if (m_ioEnabled)
		canWrite();
	load();
}

bool PrefsFile::hasContext(const QString& contextName) const
{
	return m_contexts.contains(contextName);
}

PrefsContext* PrefsFile::getContext(const QString& contextName, bool persistent)
{
	if (!m_contexts.contains(contextName))
		m_contexts[contextName] = new PrefsContext(contextName, persistent);
	return m_contexts[contextName];
}

PrefsContext* PrefsFile::getPluginContext(const QString& contextName, bool persistent)
{
	if (!m_pluginContexts.contains(contextName))
		m_pluginContexts[contextName] = new PrefsContext(contextName, persistent);
	return m_pluginContexts[contextName];
}

PrefsContext* PrefsFile::getUserPrefsContext(const QString& contextName, bool persistent)
{
	if (!m_userprefsContexts.contains(contextName))
		m_userprefsContexts[contextName] = new PrefsContext(contextName, persistent);
	return m_userprefsContexts[contextName];
}

void PrefsFile::load()
{
	PrefsReader handler(&m_contexts, &m_pluginContexts);
	QFile rc(m_prefsFilePath);
	QXmlInputSource  source(&rc);
	QXmlSimpleReader reader;
	reader.setContentHandler(&handler);
	reader.parse(source);
}

void PrefsFile::write()
{
	if ((!m_ioEnabled) || ((m_contexts.empty()) && (m_pluginContexts.empty())))
		return; // No prefs file path set -> can't write or no prefs to write

	QFile prefsXML(m_prefsFilePath);
	if (!prefsXML.open(QIODevice::WriteOnly))
		return;

	QTextStream stream(&prefsXML);
	stream.setEncoding(QStringConverter::Utf8);
	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<preferences>\n";
	if (!m_contexts.empty())
	{
		stream << "\t<level name=\"application\">\n";
		writeContexts(&m_contexts, stream);
		stream << "\t</level>\n";
	}
	if (!m_userprefsContexts.empty())
	{
		stream << "\t<level name=\"plugin\">\n";
		writeContexts(&m_userprefsContexts, stream);
		stream << "\t</level>\n";
	}
	if (!m_pluginContexts.empty())
	{
		stream << "\t<level name=\"plugin\">\n";
		writeContexts(&m_pluginContexts, stream);
		stream << "\t</level>\n";
	}
	stream << "</preferences>\n";

	prefsXML.close();
}

void PrefsFile::writeContexts(const ContextMap* contextMap, QTextStream& stream)
{
	for (auto it = contextMap->begin(); it != contextMap->end(); ++it)
	{
		const PrefsContext* prefsContext = it.value();
		if (!prefsContext->isPersistent() || prefsContext->isEmpty())
			continue;

		stream << "\t\t<context name=\"" + replaceIllegalChars(it.key()) + "\">\n";

		for (auto it2 = prefsContext->values.begin(); it2 != prefsContext->values.end(); ++it2)
		{
			stream << "\t\t\t<attribute key=\"" + replaceIllegalChars(it2.key()) + "\" ";
			stream << "value=\""  + replaceIllegalChars(it2.value()) + "\"/>\n";
		}

		for (auto it3 = prefsContext->tables.begin(); it3 != prefsContext->tables.end(); ++it3)
		{
			stream << "\t\t\t<table name=\"" + replaceIllegalChars(it3.key()) + "\">\n";
			PrefsTable* t = it3.value();
			for (int i = 0; i < t->height(); ++i)
			{
				stream << QString("\t\t\t\t<row index=\"%1\">\n").arg(i);
				for (int j = 0; j < t->width(); ++j)
				{
					stream << QString("\t\t\t\t\t<col index=\"%1\">").arg(j);
					stream << replaceIllegalChars(t->get(i, j, "__NOT__SET__")) << "</col>\n";
				}
				stream << "\t\t\t\t</row>\n";
			}
			stream << "\t\t\t</table>\n";
		}
		stream << "\t\t</context>\n";
	}
}

QString PrefsFile::replaceIllegalChars(const QString& text)
{
	QString s = text;
	s.replace("&", "&amp;");
	s.replace("<", "&lt;");
	s.replace(">", "&gt;");
	s.replace("\"", "&quot;");
	s.replace("\'", "&apos;");
	return s;
}

void PrefsFile::canWrite()
{
	if (!m_ioEnabled)
		return;
	QFile f(m_prefsFilePath);
	QFileInfo fi(f);
	if (fi.exists())
		m_ioEnabled = fi.isWritable();
	else
	{
		QFile f2(m_prefsFilePath.left(m_prefsFilePath.lastIndexOf("/")));
		QFileInfo fi2(f2);
		m_ioEnabled = fi2.isWritable();
	}
}

PrefsFile::~PrefsFile()
{
	ContextMap::Iterator it;
	for (it = m_contexts.begin(); it != m_contexts.end(); ++it)
		delete it.value();
	for (it = m_pluginContexts.begin(); it != m_pluginContexts.end(); ++it)
		delete it.value();
}
