/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "util_ghostscript.h"
#include "scpaths.h"
#include "scribuscore.h"
#include "scimgdataloader_pdf.h"

#ifdef HAVE_PODOFO
#include <podofo/podofo.h>
#endif


ScImgDataLoader_PDF::ScImgDataLoader_PDF()
{
	initSupportedFormatList();
}

void ScImgDataLoader_PDF::initSupportedFormatList()
{
	m_supportedFormats.clear();
	m_supportedFormats.append( "pdf" );
}

void ScImgDataLoader_PDF::loadEmbeddedProfile(const QString& fn, int /* page */)
{
	m_embeddedProfile.resize(0);
	m_profileComponents = 0;
}

bool ScImgDataLoader_PDF::loadPicture(const QString& fn, int page, int gsRes, bool /*thumbnail*/)
{
	QStringList args;
	if (!QFile::exists(fn))
		return false;
	QString tmpFile = QDir::toNativeSeparators(ScPaths::tempFileDir() + "sc.png");
	QString picFile = QDir::toNativeSeparators(fn);
	float xres = gsRes;
	float yres = gsRes;

	initialize();
	m_imageInfoRecord.actualPageNumber = page;

	m_imageInfoRecord.type = ImageTypePDF;
	m_imageInfoRecord.exifDataValid = false;
	m_imageInfoRecord.numberOfPages = 99; // FIXME
#ifdef HAVE_PODOFO
	try
	{
		PoDoFo::PdfError::EnableDebug( false );
		PoDoFo::PdfError::EnableLogging( false );
		PoDoFo::PdfMemDocument doc( fn.toLocal8Bit().data() );
		m_imageInfoRecord.numberOfPages = doc.GetPageCount();
		if (page > m_imageInfoRecord.numberOfPages)
		{
			qDebug() << "Incorrect page number specified!";
			m_imageInfoRecord.actualPageNumber = page = 0;
		}
	}
	catch(PoDoFo::PdfError& e)
	{
		qDebug() << "PoDoFo error while reading page count!";
		e.PrintErrorMsg();
	}		
#endif
	args.append("-r"+QString::number(gsRes));
	args.append("-sOutputFile="+tmpFile);
	args.append("-dFirstPage=" + QString::number(qMax(1, page)));
	args.append("-dLastPage=" + QString::number(qMax(1, page)));
	args.append("-dUseArtBox");
	args.append(picFile);
//	qDebug() << "scimgdataloader_pdf:" << args;
	int retg = callGS(args);
	if (retg == 0)
	{
		m_image.load(tmpFile);
		QFile::remove(tmpFile);
		if (!ScCore->havePNGAlpha())
		{
			for (int yi = 0; yi < m_image.height(); ++yi)
			{
				QRgb *s = (QRgb*)(m_image.scanLine( yi ));
				for (int xi = 0; xi < m_image.width(); ++xi )
				{
					if ((*s) == 0xffffffff)
						(*s) &= 0x00ffffff;
					s++;
				}
			}
		}
		m_imageInfoRecord.BBoxX = 0;
		m_imageInfoRecord.BBoxH = m_image.height();
		m_imageInfoRecord.xres = gsRes;
		m_imageInfoRecord.yres = gsRes;
		m_imageInfoRecord.colorspace = ColorSpaceRGB;
		m_image.setDotsPerMeterX ((int) (xres / 0.0254));
		m_image.setDotsPerMeterY ((int) (yres / 0.0254));
		m_pixelFormat = Format_BGRA_8;
		return true;
	}
	return false;
}

bool ScImgDataLoader_PDF::preloadAlphaChannel(const QString& fn, int page, int gsRes, bool& hasAlpha)
{
//	short resolutionunit = 0;

	initialize();
	m_imageInfoRecord.actualPageNumber = page;

	hasAlpha = false;
	QFileInfo fi = QFileInfo(fn);
	if (!fi.exists())
		return false;
	QString tmpFile = QDir::toNativeSeparators(ScPaths::tempFileDir() + "sc.png");
	QString picFile = QDir::toNativeSeparators(fn);
	QStringList args;
	args.append("-r"+QString::number(gsRes));
//	args.append("-sOutputFile=\""+tmpFile + "\"");
	args.append("-sOutputFile="+tmpFile);
	args.append("-dFirstPage=" + QString::number(qMax(1, page)));
	args.append("-dLastPage=" + QString::number(qMax(1, page)));
	args.append("-dUseArtBox");
//	args.append("\""+picFile+"\"");
	args.append(picFile);
//	qDebug() << "scimgdataloader_pdf(alpha):" << args;
	int retg = callGS(args);
	if (retg == 0)
	{
		m_image.load(tmpFile);
		QFile::remove(tmpFile);
		if (!ScCore->havePNGAlpha())
		{
			QRgb *s;
			for (int yi=0; yi < m_image.height(); ++yi)
			{
				s = (QRgb*)(m_image.scanLine( yi ));
				for (int xi=0; xi < m_image.width(); ++xi)
				{
					if ((*s) == 0xffffffff)
						(*s) &= 0x00ffffff;
					s++;
				}
			}
		}
		hasAlpha = true;
		return true;
	}
	return false;
}
