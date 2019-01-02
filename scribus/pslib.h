/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          pslib.h  -  description
                             -------------------
    begin                : Sat May 26 2001
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

#ifndef PSLIB_H
#define PSLIB_H

#include <vector>
#include <utility>

#include <QDataStream>
#include <QFile>
#include <QList>
#include <QPen>
#include <QString>

#include "scribusapi.h"
#include "scribusstructs.h"
#include "colormgmt/sccolormgmtengine.h"
#include "tableborder.h"


class ScPage;
class ScribusDoc;
class PageItem;
class MultiProgressDialog;
class ScImage;
class ScLayer;
class PSPainter;

/**
  *@author Franz Schmid
  * Diese Klasse erzeugt Postscript-Dateien
  */

class SCRIBUS_API PSLib : public QObject
{
	Q_OBJECT

	friend class PSPainter;

	public:

		typedef enum
		{
			OptimizeCompat = 0,
			OptimizeSize = 1
		} Optimization;

		PSLib(PrintOptions &options, bool psart, SCFonts &AllFonts, QMap<QString, QMap<uint, FPointArray> > DocFonts, ColorList DocColors, bool pdf = false, bool spot = true);
		virtual ~PSLib();

		void setOptimization (Optimization opt) { optimization = opt; }

		virtual int   CreatePS(ScribusDoc* Doc, PrintOptions &options);
		virtual const QString& errorMessage();

		virtual void PS_Error(const QString& message);
		virtual void PS_Error_ImageDataWriteFailure();
		virtual void PS_Error_ImageLoadFailure(const QString& fileName);
		virtual void PS_Error_MaskLoadFailure(const QString& fileName);
		virtual void PS_Error_InsufficientMemory();

		virtual bool PS_set_file(const QString& fn);
		virtual void PS_set_Info(const QString& art, const QString& was);
		virtual bool PS_begin_doc(ScribusDoc *doc, double x, double y, double width, double height, int numpage, bool sep, bool farb);
		virtual void PS_begin_page(ScPage* pg, MarginStruct* Ma, bool Clipping);
		virtual void PS_end_page();
		virtual void PS_curve(double x1, double y1, double x2, double y2, double x3, double y3);
		virtual void PS_moveto(double x, double y);
		virtual void PS_lineto(double x, double y);
		virtual void PS_closepath();
		virtual void PS_translate(double x, double y);
		virtual void PS_scale(double x, double y);
		virtual void PS_rotate(double x);
		virtual void PS_clip(bool mu);
		virtual void PS_save();
		virtual void PS_restore();
		virtual void PS_setcmykcolor_fill(double c, double m, double y, double k);
		virtual void PS_setcmykcolor_dummy();
		virtual void PS_setcmykcolor_stroke(double c, double m, double y, double k);
		virtual void PS_setlinewidth(double w);
		virtual void PS_setcapjoin(Qt::PenCapStyle ca, Qt::PenJoinStyle jo);
		virtual void PS_setdash(Qt::PenStyle st, double offset, QVector<double> dash);
		virtual void PS_selectfont(const QString& f, double s);
		virtual void PS_fill();
		virtual void PS_fillspot(const QString& color, double shade);
		virtual void PS_stroke();
		virtual void PS_strokespot(const QString& color, double shade);
		virtual void PS_fill_stroke();
		virtual void PS_newpath();
		virtual void PS_show(double x, double y);
		virtual void PS_showSub(uint chr, const QString& font, double size, bool stroke);
		virtual bool PS_image(PageItem *item, double x, double y, const QString& fn, double scalex, double scaley, const QString& Prof, bool UseEmbedded, const QString& Name = "");
		virtual bool PS_ImageData(PageItem *item, const QString& fn, const QString& Name, const QString& Prof, bool UseEmbedded);
		virtual void PS_plate(int nr, const QString& name = "");
		virtual void PS_setGray();
		virtual void PDF_Bookmark(const QString& text, uint Seite);
		virtual void PDF_Annotation(PageItem *item, const QString& text, double x, double y, double b, double h);
		virtual void PS_close();
		virtual void PS_insert(const QString& i);
		virtual void PS_TemplateStart(const QString& Name);
		virtual void PS_TemplateEnd();
		virtual void PS_UseTemplate(const QString& Name);
		virtual bool ProcessItem(ScribusDoc* Doc, ScPage* page, PageItem* item, uint PNr, bool sep, bool farb, bool master, bool embedded = false, bool useTemplate = false);
		virtual void ProcessPage(ScribusDoc* Doc, ScPage* page, uint PNr, bool sep = false, bool farb = true);
		virtual bool ProcessMasterPageLayer(ScribusDoc* Doc, ScPage* page, ScLayer& ll, uint PNr, bool sep = false, bool farb = true);
		virtual bool ProcessPageLayer(ScribusDoc* Doc, ScPage* a, ScLayer& ll, uint PNr, bool sep = false, bool farb = true);
		virtual void PS_HatchFill(PageItem *currItem);
		virtual void drawArrow(PageItem *ite, QTransform &arrowTrans, int arrowIndex);
		virtual void putColor(const QString& color, double shade, bool fill);
		virtual void putColorNoDraw(const QString& color, double shade);
		virtual void GetBleeds(ScPage* page, double& left, double& right);
		virtual void GetBleeds(ScPage* page, double& left, double& right, double& bottom, double& top);
		virtual void SetClipPath(const FPointArray &points, bool poly = true);
		virtual void SetPathAndClip(const FPointArray &points, bool clipRule);
		virtual void HandleBrushPattern(PageItem *item, QPainterPath &path, ScPage* a, uint PNr, bool sep, bool farb, bool master);
		virtual void HandleStrokePattern(PageItem *item);
		virtual void HandleMeshGradient(PageItem* item);
		virtual void HandlePatchMeshGradient(PageItem* item);
		virtual void HandleDiamondGradient(PageItem* item);
		virtual void HandleTensorGradient(PageItem* item);
		virtual void HandleGradientFillStroke(PageItem *item, bool stroke = true, bool forArrow = false);
		virtual void SetColor(const QString& color, double shade, double *c, double *m, double *y, double *k);
		virtual void SetColor(const ScColor& color, double shade, double *c, double *m, double *y, double *k);
		virtual void setTextSt(ScribusDoc* Doc, PageItem* ite, uint a, ScPage* pg, bool sep, bool farb, bool master);
		bool psExport;

	private:

		void PutStream (const QString& c);
		void PutStream (const QByteArray& array, bool hexEnc);
		void PutStream (const char* in, int length, bool hexEnc);

		bool PutImageToStream(ScImage& image, int plate);
		bool PutImageToStream(ScImage& image, const QByteArray& mask, int plate);

		bool PutImageDataToStream(const QByteArray& image);
		bool PutInterleavedImageMaskToStream(const QByteArray& image, const QByteArray& mask, bool gray);

		void WriteASCII85Bytes(const QByteArray& array);
		void WriteASCII85Bytes(const unsigned char* array, int length);

		void paintBorder(const TableBorder& border, const QPointF& start, const QPointF& end, const QPointF& startOffsetFactors, const QPointF& endOffsetFactors);

		Optimization optimization;

		QString ToStr(double c);
		QString IToStr(int c);
		QString MatrixToStr(double m11, double m12, double m21, double m22, double x, double y);
		QString PSEncode(const QString& in);
		QString ErrorMessage;
		QString Prolog;
		QString Header;
		QString Creator;
		QString User;
		QString Titel;
		QString BBox;
		QString BBoxH;
		QString Farben;
		QString FNamen;
		QString PDev;
		QString GrayCalc;
		bool GraySc;
		int  PageIndex;
		QString FillColor;
		QString StrokeColor;
		double LineW;
		QString Fonts;
		QString FontDesc;
		QMap<QString, QString> UsedFonts;
		QMap<QString, QString> FontSubsetMap;
		bool isPDF;
		QFile Spool;
		QDataStream spoolStream;
		int  Plate;
		bool DoSep;
		bool useSpotColors;
		bool fillRule;
		ScColorTransform solidTransform;
		QString currentSpot;
		ColorList colorsToUse;
		QString colorDesc;
		ScribusDoc *m_Doc;
		QMap<QString, QString> spotMap;
		MultiProgressDialog* progressDialog;
		bool abortExport;
		PrintOptions Options;
		ScPage* ActPage;

	protected slots:
		void cancelRequested();
};

#endif
