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

#include "sxwim.h"
#include <QStringList>
#include <QTemporaryDir>

#ifndef HAVE_XML
#error The sxwim plugin requires libxml to build
#endif

#include "prefsmanager.h"
#include <prefsfile.h>
#include <prefscontext.h>
#include <prefstable.h>
#include "stylereader.h"
#include "contentreader.h"
#include "sxwdia.h"
#include "third_party/zip/scribus_zip.h"

QString FileFormatName()
{
    return QObject::tr("OpenOffice.org Writer Documents");
}

QStringList FileExtensions()
{
	return QStringList("sxw");
}

void GetText(const QString& filename, const QString& encoding, bool textOnly, gtWriter *writer)
{
	SxwIm* sim = new SxwIm(filename, encoding, writer, textOnly);
	delete sim;
}

/********** Class SxwIm ************************************************************/

SxwIm::SxwIm(const QString& fileName, const QString& enc, gtWriter* w, bool textOnly)
{
	PrefsContext* prefs = PrefsManager::instance().prefsFile->getPluginContext("SxwIm");
	bool update = prefs->getBool("update", true);
	bool prefix = prefs->getBool("prefix", true);
	bool ask = prefs->getBool("askAgain", true);
	bool pack = prefs->getBool("pack", true);
	encoding = enc;
	writer = w;
	if (!textOnly)
	{
		if (ask)
		{
			SxwDialog* sxwdia = new SxwDialog(update, prefix, pack);
			if (sxwdia->exec()) {
				update = sxwdia->shouldUpdate();
				prefix = sxwdia->usePrefix();
				pack = sxwdia->packStyles();
				prefs->set("update", update);
				prefs->set("prefix", sxwdia->usePrefix());
				prefs->set("askAgain", sxwdia->askAgain());
				prefs->set("pack", sxwdia->packStyles());
				delete sxwdia;
			} else {
				delete sxwdia;
				return;
			}
		}
	}
	filename = fileName;
	writer->setUpdateParagraphStyles(update);
	ScZipHandler* fun = new ScZipHandler();
	if (fun->open(fileName))
	{
		QTemporaryDir *dir = new QTemporaryDir();
		QString baseDir = dir->path();
		fun->extract(STYLE, baseDir, ScZipHandler::SkipPaths);
		fun->extract(CONTENT, baseDir, ScZipHandler::SkipPaths);
		stylePath   = baseDir + "/" + STYLE;
		contentPath = baseDir + "/" + CONTENT;
		if ((!stylePath.isNull()) && (!contentPath.isNull()))
		{
			QString docname = filename.right(filename.length() - filename.lastIndexOf("/") - 1);
			docname = docname.left(docname.lastIndexOf("."));
			StyleReader *sreader = new StyleReader(docname, writer, textOnly, prefix, pack);
			sreader->parse(stylePath);
			ContentReader *creader = new ContentReader(docname, sreader, writer, textOnly);
			creader->parse(contentPath);
			delete sreader;
			delete creader;
		}
		delete dir;
	}
	delete fun;
}

SxwIm::~SxwIm()
{

}
