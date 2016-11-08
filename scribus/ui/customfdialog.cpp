/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

/***************************************************************************
                          customfdialog.cpp  -  description
                             -------------------
    begin                : Fri Nov 30 2001
    copyright            : (C) 2001 by Franz Schmid
    email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QImageReader>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QTextCodec>
#include <QVBoxLayout>

#include "customfdialog.h"

#include "../plugins/formatidlist.h"
#include "cmsettings.h"
#include "commonstrings.h"
#include "fileloader.h"
#include "iconmanager.h"
#include "loadsaveplugin.h"
#include "prefsmanager.h"
#include "sccombobox.h"
#include "scfilewidget.h"
#include "scimage.h"
#include "scpreview.h"
#include "scribusstructs.h"
#include "scslainforeader.h"
#include "units.h"
#include "util.h"
#include "util_color.h"
#include "util_formats.h"




extern QString DocDir;

ImIconProvider::ImIconProvider() : QFileIconProvider()
{
	fmts.clear();
	QString tmp[] = {"eps", "epsi", "gif", "png", "jpg", "jpeg", "xpm", "tif", "tiff", "bmp", "pbm", "pgm", "ppm", "xbm", "xpm", "psd", "pat"};
	size_t array = sizeof(tmp) / sizeof(*tmp);
	for (uint a = 0; a < array; ++a)
		fmts.append(tmp[a]);
	IconManager* im=IconManager::instance();
	imagepm = im->loadIcon("16/image-x-generic.png");
	pspm = im->loadIcon("postscript.png");
	txtpm = im->loadIcon("txt.png");
	docpm = im->loadIcon("doc.png");
	pdfpm = im->loadIcon("pdf.png");
	oosxdpm = im->loadIcon("ooo_draw.png");
	oosxwpm = im->loadIcon("ooo_writer.png");
	vectorpm = im->loadIcon("vectorgfx.png");
}

QIcon ImIconProvider::icon(const QFileInfo &fi) const
{
	QStringList allFormatsV = LoadSavePlugin::getExtensionsForImport(FORMATID_FIRSTUSER);
	QString ext = fi.suffix().toLower();
	if (ext.isEmpty())
		return QFileIconProvider::icon(fi);
	if (fmts.contains(ext, Qt::CaseInsensitive))
		return imagepm;
	else
	{
		ext = fi.completeSuffix().toLower();
		if (ext.endsWith("ps", Qt::CaseInsensitive))
			return pspm;
		else if (ext.endsWith("txt", Qt::CaseInsensitive))
			return txtpm;
		else if (ext.endsWith("scd", Qt::CaseInsensitive) || ext.endsWith("scd.gz", Qt::CaseInsensitive))
			return docpm;
		else if (ext.endsWith("sla", Qt::CaseInsensitive) || ext.endsWith("sla.gz", Qt::CaseInsensitive))
			return docpm;
		else if (ext.endsWith("pdf", Qt::CaseInsensitive))
			return pdfpm;
		else if (ext.endsWith("sxd", Qt::CaseInsensitive))
			return oosxdpm;
		else if (ext.endsWith("sxw", Qt::CaseInsensitive))
			return oosxwpm;
		else if (allFormatsV.contains(ext) || ext.endsWith("sce", Qt::CaseInsensitive) || ext.endsWith("shape", Qt::CaseInsensitive))
			return vectorpm;
		else
			return QFileIconProvider::icon(fi);
	}
	return QIcon();
}

FDialogPreview::FDialogPreview(QWidget *pa) : QLabel(pa)
{
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	setFixedSize( QSize( 200, 200 ) );
	setScaledContents( false );
	setFrameShape( QLabel::WinPanel );
	setFrameShadow( QLabel::Sunken );
	updtPix();
}

void FDialogPreview::updtPix()
{
	QPixmap pm;
	QRect inside = contentsRect();
	pm = QPixmap(inside.width(), inside.height());
	pm.fill(Qt::white);
	setPixmap(pm);
}

void FDialogPreview::GenPreview(QString name)
{
	QPixmap pm;
	QString Buffer = "";
	updtPix();
	if (name.isEmpty())
		return;
	QFileInfo fi = QFileInfo(name);
	if (fi.isDir())
		return;
	int w = pixmap()->width();
	int h = pixmap()->height();
	bool mode = false;
	QString ext = fi.suffix().toLower();
	QString formatD(FormatsManager::instance()->extensionListForFormat(FormatsManager::IMAGESIMGFRAME, 1));
 	QStringList formats = formatD.split("|");
	formats.append("pat");
	formats.removeAll("pdf");
	
	QStringList allFormatsV = LoadSavePlugin::getExtensionsForPreview(FORMATID_FIRSTUSER);
	if (ext.isEmpty())
		ext = getImageType(name);
	if (formats.contains(ext.toUtf8()))
	{
		ScImage im;
		//No doc to send data anyway, so no doc to get into scimage.
		CMSettings cms(0, "", Intent_Perceptual);
		cms.allowColorManagement(false);
		if (im.loadPicture(name, 1, cms, ScImage::Thumbnail, 72, &mode))
		{
			int ix,iy;
			if ((im.imgInfo.exifDataValid) && (!im.imgInfo.exifInfo.thumbnail.isNull()))
			{
				ix = im.imgInfo.exifInfo.width;
				iy = im.imgInfo.exifInfo.height;
			}
			else
			{
				ix = im.width();
				iy = im.height();
			}
			int xres = im.imgInfo.xres;
			int yres = im.imgInfo.yres;
			QString tmp = "";
			QString tmp2 = "";
			QImage im2 = im.scaled(w - 5, h - 44, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			QPainter p;
			QBrush b(QColor(205,205,205), IconManager::instance()->loadPixmap("testfill.png"));
			// Qt4 FIXME imho should be better
			pm = *pixmap();
			p.begin(&pm);
			p.fillRect(0, 0, w, h-44, b);
			p.fillRect(0, h-44, w, 44, QColor(255, 255, 255));
			p.drawImage((w - im2.width()) / 2, (h - 44 - im2.height()) / 2, im2);
			p.drawText(2, h-29, tr("Size:")+" "+tmp.setNum(ix)+" x "+tmp2.setNum(iy));
			if (!(extensionIndicatesPDF(ext) || extensionIndicatesEPSorPS(ext)))
				p.drawText(2, h-17, tr("Resolution:")+" "+tmp.setNum(xres)+" x "+tmp2.setNum(yres)+" "+ tr("DPI"));
			QString cSpace;
			if ((extensionIndicatesPDF(ext) || extensionIndicatesEPSorPS(ext)) && (im.imgInfo.type != ImageType7))
				cSpace = tr("Unknown");
			else
				cSpace=colorSpaceText(im.imgInfo.colorspace);
			p.drawText(2, h-5, tr("Colorspace:")+" "+cSpace);
			p.end();
			setPixmap(pm);
			repaint();
		}
	}
	else if (allFormatsV.contains(ext.toUtf8()))
	{
		FileLoader *fileLoader = new FileLoader(name);
		int testResult = fileLoader->testFile();
		delete fileLoader;
		if ((testResult != -1) && (testResult >= FORMATID_FIRSTUSER))
		{
			const FileFormat * fmt = LoadSavePlugin::getFormatById(testResult);
			if( fmt )
			{
				QImage im = fmt->readThumbnail(name);
				if (!im.isNull())
				{
					QString desc = tr("Size:")+" ";
					desc += value2String(im.text("XSize").toDouble(), PrefsManager::instance()->appPrefs.docSetupPrefs.docUnitIndex, true, true);
					desc += " x ";
					desc += value2String(im.text("YSize").toDouble(), PrefsManager::instance()->appPrefs.docSetupPrefs.docUnitIndex, true, true);
					im = im.scaled(w - 5, h - 21, Qt::KeepAspectRatio, Qt::SmoothTransformation);
					QPainter p;
					QBrush b(QColor(205,205,205), IconManager::instance()->loadPixmap("testfill.png"));
					pm = *pixmap();
					p.begin(&pm);
					p.fillRect(0, 0, w, h-21, b);
					p.fillRect(0, h-21, w, 21, QColor(255, 255, 255));
					p.drawImage((w - im.width()) / 2, (h - 21 - im.height()) / 2, im);
					p.drawText(2, h-5, desc);
					p.end();
					setPixmap(pm);
					repaint();
				}
			}
		}
	}
	else if (ext.toUtf8() == "sce")
	{
		QByteArray cf;
		if (loadRawText(name, cf))
		{
			QString f;
			if (cf.left(16) == "<SCRIBUSELEMUTF8")
				f = QString::fromUtf8(cf.data());
			else
				f = cf.data();
			ScPreview *pre = new ScPreview();
			QImage im = pre->createPreview(f);
			im = im.scaled(w - 5, h - 21, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			QPainter p;
			QBrush b(QColor(205,205,205), IconManager::instance()->loadPixmap("testfill.png"));
			pm = *pixmap();
			p.begin(&pm);
			p.fillRect(0, 0, w, h-21, b);
			p.fillRect(0, h-21, w, 21, QColor(255, 255, 255));
			p.drawImage((w - im.width()) / 2, (h - 21 - im.height()) / 2, im);
			QString desc = tr("Size:")+QString(" %1 x %2").arg(im.width()).arg(im.height());
			p.drawText(2, h-5, desc);
			p.end();
			setPixmap(pm);
			repaint();
			delete pre;
		}
	}
	else
	{
		ScSlaInfoReader slaInfos;
		if (slaInfos.readInfos(name))
		{
			QString Title = tr("Title:")+" ";
			QString ti2 = slaInfos.title();
			if (ti2.isEmpty())
				ti2= tr("No Title");
			Title += ti2+"\n";
			QString Author = tr("Author:")+" ";
			QString au2 = slaInfos.author();
			if (au2.isEmpty())
				au2 = tr("Unknown");
			Author += au2+"\n";
			QString Format =  tr("File Format:")+" ";
			QString fm2 = slaInfos.format();
			if (fm2.isEmpty())
				fm2 = tr("Unknown");
			Format += fm2;
			setText( tr("Scribus Document")+"\n\n"+Title+Author+Format);
		}
		else  if ((ext == "txt") || (ext == "html") || (ext == "xml"))
		{
			if (loadText(name, &Buffer))
				setText(Buffer.left(200));
		}
	}
}

CustomFDialog::CustomFDialog(QWidget *parent, QString wDir, QString caption, QString filter, int flags)
			: QDialog(parent), optionFlags(flags)
{
	setModal(true);
	setWindowTitle(caption);
	setWindowIcon(IconManager::instance()->loadIcon("AppIcon.png"));
	vboxLayout = new QVBoxLayout(this);
	vboxLayout->setSpacing(5);
	vboxLayout->setMargin(10);
    hboxLayout = new QHBoxLayout;
	hboxLayout->setSpacing(5);
	hboxLayout->setMargin(0);
	fileDialog = new ScFileWidget(this);
	hboxLayout->addWidget(fileDialog);
	fileDialog->setIconProvider(new ImIconProvider());
	fileDialog->setNameFilter(filter);
	fileDialog->selectNameFilter(filter);
	fileDialog->setDirectory(wDir);
	vboxLayout1 = new QVBoxLayout;
	vboxLayout1->setSpacing(0);
	vboxLayout1->setMargin(0);
	vboxLayout1->setContentsMargins(0, 37, 0, 0);
	vboxLayout1->setAlignment( Qt::AlignTop );
	pw = new FDialogPreview( this );
	pw->setMinimumSize(QSize(200, 200));
	pw->setMaximumSize(QSize(200, 200));
	vboxLayout1->addWidget(pw);
	hboxLayout->addLayout(vboxLayout1);
	vboxLayout->addLayout(hboxLayout);
    QHBoxLayout *hboxLayout1 = new QHBoxLayout;
	hboxLayout1->setSpacing(5);
	hboxLayout1->setContentsMargins(9, 0, 0, 0);
	showPreview = new QCheckBox(this);
	showPreview->setText( tr("Show Preview"));
	showPreview->setToolTip( tr("Show a preview and information for the selected file"));
	showPreview->setChecked(true);
	previewIsShown = true;
	hboxLayout1->addWidget(showPreview);
	QSpacerItem *spacerItem = new QSpacerItem(2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum);
	hboxLayout1->addItem(spacerItem);
	OKButton = new QPushButton( CommonStrings::tr_OK, this);
	OKButton->setDefault( true );
	hboxLayout1->addWidget( OKButton );
	if (flags & fdDisableOk)
		OKButton->setEnabled(false);
	CancelB = new QPushButton( CommonStrings::tr_Cancel, this);
	CancelB->setAutoDefault( false );
	hboxLayout1->addWidget( CancelB );
	vboxLayout->addLayout(hboxLayout1);

	SaveZip=NULL;
	WithFonts=NULL;
	WithProfiles=NULL;
	TxCodeM = NULL;
	TxCodeT = NULL;
	Layout = LayoutC = NULL;
	Layout1 = Layout1C = NULL;
	if (flags & fdDirectoriesOnly)
	{
		Layout = new QFrame(this);
		Layout1 = new QHBoxLayout(Layout);
		Layout1->setSpacing( 0 );
		Layout1->setContentsMargins(9, 0, 0, 0);
		SaveZip = new QCheckBox( tr( "&Compress File" ), Layout);
		Layout1->addWidget(SaveZip, Qt::AlignLeft);
		QSpacerItem* spacer = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
		Layout1->addItem( spacer );
		vboxLayout->addWidget(Layout);
		LayoutC = new QFrame(this);
		Layout1C = new QHBoxLayout(LayoutC);
		Layout1C->setSpacing( 0 );
		Layout1C->setContentsMargins(9, 0, 0, 0);
		WithFonts = new QCheckBox( tr( "&Include Fonts" ), LayoutC);
		Layout1C->addWidget(WithFonts, Qt::AlignLeft);
		WithProfiles = new QCheckBox( tr( "&Include Color Profiles" ), LayoutC);
		Layout1C->addWidget(WithProfiles, Qt::AlignLeft);
		QSpacerItem* spacer2 = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
		Layout1C->addItem( spacer2 );
		vboxLayout->addWidget(LayoutC);
		fileDialog->setFileMode(QFileDialog::DirectoryOnly);
		pw->hide();
		showPreview->setVisible(false);
		showPreview->setChecked(false);
		previewIsShown = false;
	}
	else
	{
		if (flags & fdCompressFile)
		{
			Layout = new QFrame(this);
			Layout1 = new QHBoxLayout(Layout);
			Layout1->setSpacing( 5 );
			Layout1->setContentsMargins(9, 0, 0, 0);
			SaveZip = new QCheckBox( tr( "&Compress File" ), Layout);
			Layout1->addWidget(SaveZip);
			QSpacerItem* spacer = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
			Layout1->addItem( spacer );
		}
		if (flags & fdExistingFiles)
			fileDialog->setFileMode(QFileDialog::ExistingFile);
		else if (flags & fdExistingFilesI)
			fileDialog->setFileMode(QFileDialog::ExistingFiles);
		else
		{
			fileDialog->setFileMode(QFileDialog::AnyFile);
			if (flags & fdCompressFile)
				vboxLayout->addWidget(Layout);
		}
		
		if (SaveZip!=NULL)
			SaveZip->setToolTip( "<qt>" + tr( "Compress the Scribus document on save" ) + "</qt>");
		if (WithFonts!=NULL)
			WithFonts->setToolTip( "<qt>" + tr( "Include fonts when collecting files for the document. Be sure to know and understand licensing information for any fonts you collect and possibly redistribute." ) + "</qt>");
		if (WithProfiles!=NULL)
			WithProfiles->setToolTip( "<qt>" + tr( "Include color profiles when collecting files for the document" ) + "</qt>");
		
		if (flags & fdShowCodecs)
		{
			LayoutC = new QFrame(this);
			Layout1C = new QHBoxLayout(LayoutC);
			Layout1C->setSpacing( 0 );
			Layout1C->setContentsMargins(9, 0, 0, 0);
			TxCodeT = new QLabel(this);
			TxCodeT->setText( tr("Encoding:"));
			Layout1C->addWidget(TxCodeT);
			TxCodeM = new ScComboBox(LayoutC);
			TxCodeM->setEditable(false);
			QString tmp_txc[] = {"ISO 8859-1", "ISO 8859-2", "ISO 8859-3",
								"ISO 8859-4", "ISO 8859-5", "ISO 8859-6",
								"ISO 8859-7", "ISO 8859-8", "ISO 8859-9",
								"ISO 8859-10", "ISO 8859-13", "ISO 8859-14",
								"ISO 8859-15", "UTF-8", "UTF-16", "KOI8-R", "KOI8-U",
								"CP1250", "CP1251", "CP1252", "CP1253",
								"CP1254", "CP1255", "CP1256", "CP1257",
								"Apple Roman"};
			size_t array = sizeof(tmp_txc) / sizeof(*tmp_txc);
			for (uint a = 0; a < array; ++a)
				TxCodeM->addItem(tmp_txc[a]);
			QString localEn = QTextCodec::codecForLocale()->name();
			if (localEn == "ISO-10646-UCS-2")
				localEn = "UTF-16";
			bool hasIt = false;
			for (int cc = 0; cc < TxCodeM->count(); ++cc)
			{
				if (TxCodeM->itemText(cc) == localEn)
				{
					TxCodeM->setCurrentIndex(cc);
					hasIt = true;
					break;
				}
			}
			if (!hasIt)
			{
				TxCodeM->addItem(localEn);
				TxCodeM->setCurrentIndex(TxCodeM->count()-1);
			}
			TxCodeM->setMinimumSize(QSize(200, 0));
			Layout1C->addWidget(TxCodeM);
			QSpacerItem* spacer2 = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
			Layout1C->addItem( spacer2 );
			vboxLayout->addWidget(LayoutC);
		}
		if (flags & fdShowImportOptions)
		{
			LayoutC = new QFrame(this);
			Layout1C = new QHBoxLayout(LayoutC);
			Layout1C->setSpacing( 0 );
			Layout1C->setContentsMargins(9, 0, 0, 0);
			TxCodeT = new QLabel(this);
			TxCodeT->setText( tr("Import Option:"));
			Layout1C->addWidget(TxCodeT);
			TxCodeM = new ScComboBox(LayoutC);
			TxCodeM->setEditable(false);
			TxCodeM->addItem( tr("Keep original size"));
			TxCodeM->addItem( tr("Downscale to page size"));
			TxCodeM->addItem( tr("Upscale to page size"));
			TxCodeM->setCurrentIndex(0);
			TxCodeM->setMinimumSize(QSize(200, 0));
			Layout1C->addWidget(TxCodeM);
			QSpacerItem* spacer2 = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
			Layout1C->addItem( spacer2 );
			vboxLayout->addWidget(LayoutC);
		}
		bool setter2 = flags & fdHidePreviewCheckBox;
		if (!setter2)
		{
			bool setter = flags & fdShowPreview;
			showPreview->setChecked(setter);
			previewIsShown = setter;
			pw->setVisible(setter);
		}
		else
		{
			showPreview->hide();
			previewIsShown = false;
			pw->setVisible(false);
		}
		if (flags & fdCompressFile)
			connect(SaveZip, SIGNAL(clicked()), this, SLOT(handleCompress()));
	}
	fileDialog->setNameFilterDetailsVisible(false);
	extZip = "gz";
	connect(OKButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(CancelB, SIGNAL(clicked()), this, SLOT(reject()));
	connect(showPreview, SIGNAL(clicked()), this, SLOT(togglePreview()));
	connect(fileDialog, SIGNAL(currentChanged(const QString &)), this, SLOT(fileClicked(const QString &)));
	connect(fileDialog, SIGNAL(filesSelected(const QStringList &)), this, SLOT(accept()));
	connect(fileDialog, SIGNAL(accepted()), this, SLOT(accept()));
	connect(fileDialog, SIGNAL(rejected()), this, SLOT(reject()));
	resize(minimumSizeHint());
}


void CustomFDialog::fileClicked(const QString &path)
{
	if (optionFlags & fdDisableOk)
		OKButton->setEnabled(!path.isEmpty());
	if (previewIsShown)
		pw->GenPreview(path);
}

void CustomFDialog::okClicked()
{
	QString selFile=selectedFile();
	if (selFile.isEmpty())
		return;
	QFileInfo fi(selFile);
	if (fi.isDir() && (fileDialog->fileMode() != QFileDialog::DirectoryOnly))
		fileDialog->gotoSelectedDirectory();
	else
		accept();
}

void CustomFDialog::togglePreview()
{
	previewIsShown = !previewIsShown;
	pw->setVisible(previewIsShown);
	if (previewIsShown)
	{
		QStringList sel = fileDialog->selectedFiles();
		if (!sel.isEmpty())
			pw->GenPreview(QDir::fromNativeSeparators(sel[0]));
	}
	// #11856: Hack to avoid file dialog widget turning black with Qt5
	qApp->processEvents();
	pw->setVisible(!previewIsShown);
	qApp->processEvents();
	pw->setVisible(previewIsShown);
	fileDialog->repaint();
	qApp->processEvents();
	repaint();
}

void CustomFDialog::setSelection(QString sel)
{
	fileDialog->selectFile( QFileInfo(sel).fileName() );
	if (previewIsShown)
		pw->GenPreview(sel);
}

QString CustomFDialog::selectedFile()
{
	QStringList sel = fileDialog->selectedFiles();
	if (!sel.isEmpty())
		return QDir::fromNativeSeparators(sel[0]);
	return QString();
}

void CustomFDialog::addWidgets(QWidget *widgets)
{
	vboxLayout->addWidget(widgets);
}

CustomFDialog::~CustomFDialog()
{
}

void CustomFDialog::handleCompress()
{
	QString   fileName;
	QFileInfo tmp(selectedFile());
	QString   fn(tmp.fileName());
	QStringList fc = fn.split(".", QString::KeepEmptyParts);
	if (fc.count() > 0)
		fileName = fc.at(0);
	for (int a = 1; a < fc.count(); a++)
	{
		if (fc.at(a).compare("sla", Qt::CaseInsensitive) == 0)
			continue;
		if (fc.at(a).compare("gz", Qt::CaseInsensitive) == 0)
			continue;
		if (fc.at(a).compare(ext, Qt::CaseInsensitive) == 0)
			continue;
		if (fc.at(a).compare(extZip, Qt::CaseInsensitive) == 0)
			continue;
		fileName += "." + fc[a];
	}
	if (SaveZip->isChecked())
		tmp.setFile(fileName + "." + extZip);
	else
		tmp.setFile(fileName + "." + ext);
	setSelection(tmp.fileName());
}

void CustomFDialog::setExtension(QString e)
{
	ext = e;
}

QString CustomFDialog::extension()
{
	return ext;
}

void CustomFDialog::setZipExtension(QString e)
{
	extZip = e;
}

QString CustomFDialog::zipExtension()
{
	return extZip;
}
