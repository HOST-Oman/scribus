/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef _UTIL_FILE_H
#define _UTIL_FILE_H

#include <QCryptographicHash>
#include "scribusapi.h"

class QDataStream;
class QString;
class QByteArray;
class ScStreamFilter;
class ScribusDoc;
class PageItem;

/**
* @brief Copy a source file to a target
   * 
   * This function copy a file to a destination. If destination exists, 
   * the target file is overwritten.
   *
   * @param  source the source file
   * @param  target the target file
   * @return true on success, false on failure.
**/
bool SCRIBUS_API copyFile(const QString& source, const QString& target);
/**
* @brief Copy a source file to a target using atomic operations
   * 
   * This function copy a file to a destination using atomic operations. 
   * If destination exists, the target file is overwritten.
   *
   * @param  source the source file
   * @param  target the target file
   * @return true on success, false on failure.
**/
bool SCRIBUS_API copyFileAtomic(const QString& source, const QString& target);
/**
* @brief Copy a source file to a stream filter
   * 
   * This function copy a file to a stream filter. The target filter has
   * to be opened before the function call.
   *
   * @param  source the source file
   * @param  target the target filter
   * @return true on success, false on failre.
**/
bool SCRIBUS_API copyFileToFilter(const QString& source, ScStreamFilter& target);
/**
* @brief Copy a source file to a data stream
   * @param  source the source file
   * @param  target the target stream
   * @return true on success, false on failre.
**/
bool SCRIBUS_API copyFileToStream(const QString& source, QDataStream& target);
/**
* @brief Move a source file to a destination
   * 
   * This function move a file to a destination. The source file is deleted
   * when done.
   *
   * @param  source the source file
   * @param  target the target file
   * @return true on success, false on failre.
**/
bool SCRIBUS_API moveFile(const QString& source, const QString& target);
/**
* @brief Update the access and modification time of a file
   * 
   * This function updates the access and modification time of a file.
   *
   * @param  file name of the file to touch
   * @return true on success, false on failure.
**/
bool SCRIBUS_API touchFile(const QString& file);
/**
* @brief Check if an executable exists in the path given
   *
   * This function checks if an executable exists in the path given
   *
   * @param  file name of the file to check, possibly including parameters that will be ignored
   * @return true on success, false on failure.
**/
bool SCRIBUS_API fileInPath(const QString& filename);
/**
* @brief Check if a file's checksum matches the has in the given file.
   *
   * This function checks if an executable exists in the path given
   *
   * @param  the directory where the files live in
   * @param  the file to check the has for
   * @param  the file that contains the hash data
   * @param  the method/checksum
   * @return true on success, false on failure.
**/
bool SCRIBUS_API checkFileHash(const QString& directory, const QString& filename, const QString& hashFilename, QCryptographicHash::Algorithm method);

PageItem SCRIBUS_API * getVectorFileFromData(ScribusDoc *doc, QByteArray &data, const QString& ext, double x, double y, double w = -1.0, double h = -1.0);
#endif
