/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/
/***************************************************************************
							 -------------------
	begin                : Sat Nov 23 2013
	copyright            : (C) 2013 by Franz Schmid
	email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/
#ifndef SCZIPHANDLER_H
#define SCZIPHANDLER_H

#include <QtCore>
class Zip;
class UnZip;

class ScZipHandler
{
	public:
		enum ExtractionOption
		{
			ExtractPaths = 0x0001,
			SkipPaths = 0x0002,
			VerifyOnly = 0x0004,
			NoSilentDirectoryCreation = 0x0008
		};

		ScZipHandler(bool forWrite = false);
		virtual ~ScZipHandler();
		bool open(const QString& fileName);
		bool close();
		bool contains(const QString& fileName);
		bool read(const QString& fileName, QByteArray &buf);
		bool write(const QString& dirName);
		bool extract(const QString& name, const QString& path, ExtractionOption eo);
		QStringList files();
	private:
		UnZip* m_uz;
		Zip* m_zi;
};

#endif
