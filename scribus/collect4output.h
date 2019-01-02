/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef COLLECT4OUTPUT_H
#define COLLECT4OUTPUT_H

#include <QObject>
#include <QMap>

#include "scribusstructs.h"

class QString;
class ScribusDoc;
class PrefsContext;
class PageItem;


/*! \brief Performs "Collect for Output" tasks.
collect() method copies the document, fonts and images
into user defined directory. QObject inheritance mainly due
moc speedup and tr() methods.
\author Petr Vanek, Franz Schmid
*/
class CollectForOutput : public QObject
{
	Q_OBJECT

	public:
		/*! \brief Setup the attributes
		\param doc a Scribus doecument reference
		\param withFonts collect/move fonts into output directory too
		\param withProfiles collect/move CMS profiles into output directory too
		\param compressDoc use gzipped document
		*/
		CollectForOutput(ScribusDoc* doc, const QString& outputDirectory=QString::null, bool withFonts=false, bool withProfiles=false, bool compressDoc=false);
		~CollectForOutput(){};

		/*! \brief Main method doing everything.
		It calls all related methods
		*/
		virtual QString collect(QString &newFileName);

	protected:
		/*! Doc to collect */
		ScribusDoc* m_Doc;
		/*! Use compressed document. See the constructor */
		bool m_compressDoc;
		/*! Collect fonts too. See the constructor */
		bool m_withFonts;
		/*! Collect icc profiles too. See the constructor */
		bool m_withProfiles;
		/*! User defined directory via GUI */
		QString m_outputDirectory;
		/*! Name of the moved file with the new directory path */
		QString newName;
		/*! \brief Remember already collected files to collect the same files only once.
		It's QMap - newFile, oldFile.*/
		QMap<QString, QString> collectedFiles;
		/*! Reference to the preferences */
		PrefsContext* dirs;

		/*! Ask user for output directory via GUI.
		\retval true on success */
		bool newDirDialog();
		/*! Check permissions and export document itself.
		\retval true on success */
		bool collectDocument();
		/*! Collect all related items, esp. images.
		\retval true on success */
		bool collectItems();
		/*! Processes the item, helper function for collectItems() */
		void processItem(PageItem *ite);
		/*! Collect used fonts if requested.
		\retval true on success */
		bool collectFonts();
		/*! Helper function for collectFonts() */
		QStringList findFontMetrics(const QString& baseDir, const QString& baseName) const;
		/*! Collect used profiles if requested.
		\retval true on success */
		bool collectProfiles();
		/*! \brief Copy used file into new location with magic checks.
		It looks into collectedFiles map. If there is newFile (key) already
		found - it will construct new filename to prevent overwriting.
		E.g. newFile.png can be newFile_0.png.
		It checks already collected files not to collect one item 2 times.
		\param oldFile full path of the original file
		\param newFile suggested fullpath of the collected file
		\retval QString really used fullpath of the new file
		*/
		QString collectFile(const QString& oldFile, QString newFile);

		ProfilesL docProfiles;
		QStringList patterns;
		int profileCount;
		int itemCount;
		int fontCount;
		int patternCount;
		bool uiCollect;

	signals:
		void fontsCollected(int);
		void patternsCollected(int);
		void profilesCollected(int);
		void itemsCollected(int);
};

#endif
