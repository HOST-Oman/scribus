/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QDebug>
#include <QImageReader>
#include <QMapIterator>
#include <QObject>

#include "commonstrings.h"
#include "util_formats.h"

#ifdef GMAGICK_FOUND
#include <magick/api.h>
#include <magick/magick.h>
//#include <magick/memory.h>
#include <magick/symbols.h>
#endif

FormatsManager* FormatsManager::m_instance = nullptr;

FormatsManager::FormatsManager()
{
	m_fmts.insert(FormatsManager::AI,   QStringList() << "ai");
	m_fmts.insert(FormatsManager::BMP,  QStringList() << "bmp");
	m_fmts.insert(FormatsManager::CVG,  QStringList() << "cvg");
	m_fmts.insert(FormatsManager::EPS,  QStringList() << "eps" << "epsf" << "epsi" << "eps2" << "eps3" << "epi" << "ept");
	m_fmts.insert(FormatsManager::GIF,  QStringList() << "gif");
	m_fmts.insert(FormatsManager::JPEG, QStringList() << "jpg" << "jpeg");
	m_fmts.insert(FormatsManager::KRA,  QStringList() << "kra");
	m_fmts.insert(FormatsManager::ORA,  QStringList() << "ora");
	m_fmts.insert(FormatsManager::PAT,  QStringList() << "pat");
	m_fmts.insert(FormatsManager::PCT,  QStringList() << "pct" << "pic" << "pict");
	m_fmts.insert(FormatsManager::PDF,  QStringList() << "pdf");
	m_fmts.insert(FormatsManager::PGF,  QStringList() << "pgf");
	m_fmts.insert(FormatsManager::PNG,  QStringList() << "png");
	m_fmts.insert(FormatsManager::PS,   QStringList() << "ps");
	m_fmts.insert(FormatsManager::PSD,  QStringList() << "psd");
	m_fmts.insert(FormatsManager::SVG,  QStringList() << "svg" << "svgz");
	m_fmts.insert(FormatsManager::TIFF, QStringList() << "tif" << "tiff");
	m_fmts.insert(FormatsManager::UNICONV, QStringList() << "cdt" << "ccx" << "cmx" << "aff" << "sk" << "sk1" << "plt" << "dxf" << "dst" << "pes" << "exp" << "pcs");
	m_fmts.insert(FormatsManager::WMF,  QStringList() << "wmf");
	m_fmts.insert(FormatsManager::WPG,  QStringList() << "wpg");
	m_fmts.insert(FormatsManager::XFIG, QStringList() << "fig");
	m_fmts.insert(FormatsManager::XPM,  QStringList() << "xpm");

#ifdef GMAGICK_FOUND
	QStringList gmagickformats;
	InitializeMagick(0);
	ExceptionInfo exception;
	MagickInfo **magick_array;
	magick_array=GetMagickInfoArray(&exception);
	if (!magick_array)
		return;
	for (int i=0; magick_array[i] != 0; i++)
	{
		if (magick_array[i]->stealth)
			continue;
		gmagickformats<<QString(magick_array[i]->name).toLower();
	}
	MagickFree(magick_array);
	//qDebug()<<gmagickformats;
	gmagickformats.removeAll("eps");
	gmagickformats.removeAll("html");
	gmagickformats.removeAll("pdf");
	gmagickformats.removeAll("pict");
	gmagickformats.removeAll("ps");
	gmagickformats.removeAll("psd");
	gmagickformats.removeAll("svg");
	gmagickformats.removeAll("svgz");
	gmagickformats.removeAll("tiff");
	gmagickformats.removeAll("txt");
	gmagickformats.removeAll("wmf");
	gmagickformats.removeAll("wpg");
	m_fmts.insert(FormatsManager::GMAGICK, gmagickformats);
#endif

	m_fmtNames[FormatsManager::AI]   = QObject::tr("Adobe Illustrator");
	m_fmtNames[FormatsManager::BMP]  = QObject::tr("BMP");
	m_fmtNames[FormatsManager::CVG]  = QObject::tr("Calamus Vector Graphics");
	m_fmtNames[FormatsManager::EPS]  = QObject::tr("Encapsulated PostScript");
	m_fmtNames[FormatsManager::GIF]  = QObject::tr("GIF");
	m_fmtNames[FormatsManager::JPEG] = QObject::tr("JPEG");
	m_fmtNames[FormatsManager::KRA]  = QObject::tr("Krita");
	m_fmtNames[FormatsManager::ORA]  = QObject::tr("Open Raster");
	m_fmtNames[FormatsManager::PAT]  = QObject::tr("Pattern Files");
	m_fmtNames[FormatsManager::PDF]  = QObject::tr("PDF Document");
	m_fmtNames[FormatsManager::PGF]  = QObject::tr("PGF");
	m_fmtNames[FormatsManager::PNG]  = QObject::tr("PNG");
	m_fmtNames[FormatsManager::PSD]  = QObject::tr("Adobe Photoshop");
	m_fmtNames[FormatsManager::PS]   = QObject::tr("PostScript");
	m_fmtNames[FormatsManager::SVG]  = QObject::tr("Scalable Vector Graphics");
	m_fmtNames[FormatsManager::TIFF] = QObject::tr("TIFF");
	m_fmtNames[FormatsManager::WMF]  = QObject::tr("Windows Meta File");
	m_fmtNames[FormatsManager::WPG]  = QObject::tr("WordPerfect Graphics");
	m_fmtNames[FormatsManager::XFIG] = QObject::tr("Xfig File");
	m_fmtNames[FormatsManager::XPM]  = QObject::tr("XPM");

#ifdef GMAGICK_FOUND
	m_fmtNames[FormatsManager::GMAGICK] = QObject::tr("GraphicsMagick");
#endif
	m_fmtNames[FormatsManager::PCT]  = QObject::tr("Macintosh Pict");
	m_fmtNames[FormatsManager::QT]  = QObject::tr("Qt Supported File");
	m_fmtNames[FormatsManager::UNICONV] = QObject::tr("UniConvertor");


	m_fmtMimeTypes.insert(FormatsManager::AI,   QStringList() << "application/illustrator");
	m_fmtMimeTypes.insert(FormatsManager::CVG,  QStringList() << "");
	m_fmtMimeTypes.insert(FormatsManager::EPS,  QStringList() << "application/postscript");
	m_fmtMimeTypes.insert(FormatsManager::GIF,  QStringList() << "image/gif");
	m_fmtMimeTypes.insert(FormatsManager::JPEG, QStringList() << "image/jpeg");
	m_fmtMimeTypes.insert(FormatsManager::KRA,  QStringList() << "application/x-krita");
	m_fmtMimeTypes.insert(FormatsManager::ORA,  QStringList() << "");
	m_fmtMimeTypes.insert(FormatsManager::PAT,  QStringList() << "");
	m_fmtMimeTypes.insert(FormatsManager::PCT,  QStringList() << "");
	m_fmtMimeTypes.insert(FormatsManager::PDF,  QStringList() << "application/pdf");
	m_fmtMimeTypes.insert(FormatsManager::PGF,  QStringList() << "image/pgf");
	m_fmtMimeTypes.insert(FormatsManager::PNG,  QStringList() << "image/png");
	m_fmtMimeTypes.insert(FormatsManager::PS,   QStringList() << "application/postscript");
	m_fmtMimeTypes.insert(FormatsManager::PSD,  QStringList() << "application/photoshop");
	m_fmtMimeTypes.insert(FormatsManager::QT,   QStringList() << "");
	m_fmtMimeTypes.insert(FormatsManager::SVG,  QStringList() << "image/svg+xml");
	m_fmtMimeTypes.insert(FormatsManager::TIFF, QStringList() << "image/tiff");
	m_fmtMimeTypes.insert(FormatsManager::WMF,  QStringList() << "image/wmf");
	m_fmtMimeTypes.insert(FormatsManager::WPG,  QStringList() << "");
	m_fmtMimeTypes.insert(FormatsManager::XFIG, QStringList() << "image/x-xfig");
	m_fmtMimeTypes.insert(FormatsManager::XPM,  QStringList() << "image/xpm ");

	QMapIterator<int, QStringList> i(m_fmts);
	while (i.hasNext())
	{
		i.next();
		m_fmtList << i.value().first().toUpper();
	}

	m_qtSupportedImageFormats = QImageReader::supportedImageFormats();
	QStringList qtFmts;
	for (int qf = 0; qf < m_qtSupportedImageFormats.count(); qf++)
	{
		QString fmt = m_qtSupportedImageFormats[qf];
		bool found = false;
		QMapIterator<int, QStringList> i(m_fmts);
		while (i.hasNext())
		{
			i.next();
			if (i.value().contains(fmt))
				found = true;
		}
		if (!found)
			qtFmts.append(fmt);
	}
	m_fmts.insert(FormatsManager::QT, qtFmts);
	m_supportedImageFormats = m_qtSupportedImageFormats;
	updateSupportedImageFormats(m_supportedImageFormats);
}

FormatsManager::~FormatsManager()
{
}

FormatsManager* FormatsManager::instance()
{
	if (m_instance == nullptr)
		m_instance = new FormatsManager();

	return m_instance;
}

void FormatsManager::deleteInstance()
{
	delete m_instance;
	m_instance = nullptr;
}

void FormatsManager::imageFormatSupported(const QString& ext)
{
// 	return m_supportedImageFormats.contains(QByteArray(ext).toLatin1());
}

void FormatsManager::updateSupportedImageFormats(QList<QByteArray>& supportedImageFormats)
{
	QMapIterator<int, QStringList> it(m_fmts);
	while (it.hasNext())
	{
		it.next();
		QStringListIterator itSL(it.value());
		while (itSL.hasNext())
		{
			QString t(itSL.next());
			supportedImageFormats.append(t.toLocal8Bit());
		}
	}
}

QString FormatsManager::nameOfFormat(int type)
{
	QMapIterator<int, QString> it(m_fmtNames);
	while (it.hasNext())
	{
		it.next();
		if (type & it.key())
			return it.value();
	}
	return QString::null;
}

QStringList FormatsManager::mimetypeOfFormat(int type)
{
	QMapIterator<int, QStringList> it(m_fmtMimeTypes);
	while (it.hasNext())
	{
		it.next();
		if (type & it.key())
			return it.value();
	}
	return QStringList();
}

QString FormatsManager::extensionsForFormat(int type)
{
	QString a,b,c;
	fileTypeStrings(type, a, b, c);
	return b;
}

QString FormatsManager::fileDialogFormatList(int type)
{
	QString a,b,c;
	fileTypeStrings(type, a, b, c);
	return a + b + ";;" +c;
}

QString FormatsManager::extensionListForFormat(int type, int listType)
{
	QString nameMatch;
	QString separator(listType==0 ? " *." : "|");
	QMapIterator<int, QStringList> it(m_fmts);
	bool first=true;
	int n=0;
	while (it.hasNext())
	{
		it.next();
		if (type & it.key())
		{
			//Just in case the Qt used doesn't support jpeg or gif
			if ((JPEG & it.key()) && !m_supportedImageFormats.contains(QByteArray("jpg")))
				continue;
			if ((GIF & it.key()) && !m_supportedImageFormats.contains(QByteArray("gif")))
				continue;
			if (first)
				first=false;
			QStringListIterator itSL(it.value());
			while (itSL.hasNext())
			{
				if (listType==0)
					nameMatch += separator;
				nameMatch += itSL.next();
				if (listType==1 && itSL.hasNext())
					nameMatch += separator;
			}
		}
		++n;
		if (listType==1 && it.hasNext() && nameMatch.length()>0 && !nameMatch.endsWith(separator))
			nameMatch += separator;
	}
	if (listType==0 && nameMatch.startsWith(" "))
		nameMatch.remove(0,1);
	if (listType==1 && nameMatch.endsWith("|"))
		nameMatch.chop(1);
	return nameMatch;
}

void FormatsManager::fileTypeStrings(int type, QString& formatList, QString& formatText, QString& formatAll, bool lowerCaseOnly)
{
	QString allFormats = QObject::tr("All Supported Formats")+" (";
	QStringList formats;
	QMapIterator<int, QStringList> it(m_fmts);
	bool first=true;
	int n=0;
	while (it.hasNext())
	{
		it.next();
		if (type & it.key())
		{
			//Just in case the Qt used doesn't support jpeg or gif
			if ((JPEG & it.key()) && !m_supportedImageFormats.contains(QByteArray("jpg")))
				continue;
			if ((GIF & it.key()) && !m_supportedImageFormats.contains(QByteArray("gif")))
				continue;
			if (first)
				first=false;
			else
			{
				allFormats += " ";
			}
			QString text=m_fmtNames[it.key()] + " (";
			QStringListIterator itSL(it.value());
			while (itSL.hasNext())
			{
				QString t("*." + itSL.next());
				allFormats += t;
				text += t;
				if(!lowerCaseOnly)
				{
					allFormats += " " + t.toUpper();
					text += " " + t.toUpper();
				}
				if (itSL.hasNext())
				{
					allFormats += " ";
					text += " ";
				}
			}
			text += ")";
			formats.append(text);
		}
		++n;
	}
	formatList+=allFormats + ");;";
	formats.sort(Qt::CaseInsensitive);
	formatText+=formats.join(";;");
	formatAll=QObject::tr("All Files (*)");
}

bool extensionIndicatesPDF(const QString &ext)
{
	QStringList strl;
	strl << "ai" << "pdf";
	return strl.contains(ext, Qt::CaseInsensitive);
}

bool extensionIndicatesEPS(const QString &ext)
{
	QStringList strl;
	strl << "eps" << "epsf" << "epsi" << "eps2" << "eps3" << "epi" << "ept";
	return strl.contains(ext, Qt::CaseInsensitive);
}

bool extensionIndicatesEPSorPS(const QString &ext)
{
	QStringList strl;
	strl << "eps" << "epsf" << "epsi" << "ps" << "eps2" << "eps3" << "epi" << "ept";
	return strl.contains(ext, Qt::CaseInsensitive);
}

bool extensionIndicatesTIFF(const QString &ext)
{
	QStringList strl;
	strl << "tif" << "tiff";
	return strl.contains(ext, Qt::CaseInsensitive);
}

bool extensionIndicatesPSD(const QString &ext)
{
	QStringList strl;
	strl << "psd";
	return strl.contains(ext, Qt::CaseInsensitive);
//	return (QString::compare("psd", ext, Qt::CaseInsensitive) == 0);
}

bool extensionIndicatesJPEG(const QString &ext)
{
	QStringList strl;
	strl << "jpg" << "jpeg";
	return strl.contains(ext, Qt::CaseInsensitive);
}

bool extensionIndicatesPattern(const QString &ext)
{
	QStringList strl;
	strl << "pat";
	return strl.contains(ext, Qt::CaseInsensitive);
}

QString getImageType(const QString& filename)
{
	QString ret;
	QFile f(filename);
	QFileInfo fi(f);
	if (fi.exists())
	{
		QByteArray buf(24, ' ');
		if (f.open(QIODevice::ReadOnly))
		{
			f.read(buf.data(), 24);
			if ((buf[0] == '%') && (buf[1] == '!') && (buf[2] == 'P') && (buf[3] == 'S') && (buf[4] == '-') && (buf[5] == 'A'))
				ret = "eps";
			else if ((buf[0] == '\xC5') && (buf[1] == '\xD0') && (buf[2] == '\xD3') && (buf[3] == '\xC6'))
				ret = "eps";
			else if ((buf[0] == 'G') && (buf[1] == 'I') && (buf[2] == 'F') && (buf[3] == '8'))
				ret = "gif";
			else if ((buf[0] == '\xFF') && (buf[1] == '\xD8') && (buf[2] == '\xFF'))
				ret = "jpg";
			else if ((buf[20] == 'G') && (buf[21] == 'P') && (buf[22] == 'A') && (buf[23] == 'T'))
				ret = "pat";
			else if ((buf[0] == '%') && (buf[1] == 'P') && (buf[2] == 'D') && (buf[3] == 'F'))
				ret = "pdf";
			else if ((buf[0] == 'P') && (buf[1] == 'G') && (buf[2] == 'F'))
				ret = "pgf";
			else if ((buf[0] == '\x89') && (buf[1] == 'P') && (buf[2] == 'N') && (buf[3] == 'G'))
				ret = "png";
			else if ((buf[0] == '8') && (buf[1] == 'B') && (buf[2] == 'P') && (buf[3] == 'S'))
				ret = "psd";
			else if (((buf[0] == 'I') && (buf[1] == 'I') && (buf[2] == '\x2A')) || ((buf[0] == 'M') && (buf[1] == 'M') && (buf[3] == '\x2A')))
				ret = "tif";
			else if ((buf[0] == '/') && (buf[1] == '*') && (buf[2] == ' ') && (buf[3] == 'X') && (buf[4] == 'P') && (buf[5] == 'M'))
				ret = "xpm";
			else if ((buf[0] == 'V') && (buf[1] == 'C') && (buf[2] == 'L') && (buf[3] == 'M') && (buf[4] == 'T') && (buf[5] == 'F'))
				ret = "svm";
			f.close();
		}
	}
	return ret;
}
