/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef IMPORTPDF_H
#define IMPORTPDF_H

#include <QList>
#include <QTransform>
#include <QMultiMap>
#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QSizeF>
#include <QBuffer>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QImage>

#include "fpointarray.h"
#include "importpdfconfig.h"
#include "pluginapi.h"
#include "pageitem.h"
#include "sccolor.h"

class QColor;
class QMatrix;

class MultiProgressDialog;
class ScribusDoc;
class Selection;
class TransactionSettings;

class GooString;
class PDFDoc;

//! \brief PDF importer plugin
class PdfPlug : public QObject
{
	Q_OBJECT

public:
	/*!
	\author Franz Schmid
	\date
	\brief Create the PDF importer window.
	\param fName QString
	\param flags combination of loadFlags
	\param showProgress if progress must be displayed
	\retval EPSPlug plugin
	*/
	PdfPlug( ScribusDoc* doc, int flags );
	~PdfPlug();

	/*!
	\author Franz Schmid
	\date
	\brief Perform import.
	\param fn QString
	\param trSettings undo transaction settings
	\param flags combination of loadFlags
	\param showProgress if progress must be displayed
	\retval bool true if import was ok
	 */
	bool import(const QString& fn, const TransactionSettings& trSettings, int flags, bool showProgress = true);
	QImage readThumbnail(const QString& fn);
	QImage readPreview(int pgNum, int width, int height, int box);
	enum PDF_Box_Type
	{
		Media_Box	= 0,
		Bleed_Box	= 1,
		Trim_Box	= 2,
		Crop_Box	= 3,
		Art_Box		= 4
	};

private:
	bool convert(const QString& fn);
	QRectF getCBox(int box, int pgNum);
	QString UnicodeParsedString(POPPLER_CONST GooString *s1);
	
	QList<PageItem*> Elements;
	double baseX, baseY;
	double docWidth;
	double docHeight;
	qreal scPg;

	QStringList importedColors;

	bool interactive;
	MultiProgressDialog * progressDialog;
	bool cancel;
	ScribusDoc* m_Doc;
	Selection* tmpSele;
	int importerFlags;
	int oldDocItemCount;
	QString baseFile;
	PDFDoc *m_pdfDoc;

public slots:
	void cancelRequested() { cancel = true; }
};

#endif
