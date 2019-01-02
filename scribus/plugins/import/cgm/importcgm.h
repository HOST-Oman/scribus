/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
                          importcgm.h  -  description
                             -------------------
    begin                : Wed Dez 23 2009
    copyright            : (C) 2009 by Franz Schmid
    email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/

#ifndef IMPORTCGM_H
#define IMPORTCGM_H

#include "pluginapi.h"
#include "pageitem.h"
#include "sccolor.h"
#include "fpointarray.h"
#include <QList>
#include <QTransform>
#include <QMultiMap>
#include <QtGlobal>
#include <QObject>
#include <QString>

class MultiProgressDialog;
class ScribusDoc;
class Selection;
class TransactionSettings;

class ScBitReader
{
	public:
		ScBitReader(QByteArray &data);
		~ScBitReader();
		quint32 getUInt(uint size);
		void alignToWord();
	private:
		int actByte;
		int actBit;
		QByteArray buffer;
};

//! \brief Cgm importer plugin
class CgmPlug : public QObject
{
	Q_OBJECT

public:
	/*!
	\author Franz Schmid
	\date
	\brief Create the Cgm importer window.
	\param fName QString
	\param flags combination of loadFlags
	\param showProgress if progress must be displayed
	\retval EPSPlug plugin
	*/
	CgmPlug( ScribusDoc* doc, int flags );
	~CgmPlug();

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

private:
	void parseHeader(const QString& fName, double &b, double &h);
	bool convert(const QString& fn);
	void decodeText(QFile &f);

/* binary Decoder */
	void    decodeBinary(QDataStream &ts, quint16 elemClass, quint16 elemID, quint16 paramLen);
	void    decodeClass0(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass1(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass2(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass3(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass4(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass5(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass6(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass7(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass8(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    decodeClass9(QDataStream &ts, quint16 elemID, quint16 paramLen);
	void    getBinaryBezierPath(QDataStream &ts, quint16 paramLen);
	void    getBinaryPath(QDataStream &ts, quint16 paramLen, bool disjoint = false);
	void    getBinaryColorTable(QDataStream &ts, quint16 paramLen);
	ScColor getBinaryDirectColor(ScBitReader *breader);
	ScColor getBinaryDirectColor(QDataStream &ts);
	QString getBinaryIndexedColor(ScBitReader *breader);
	QString getBinaryIndexedColor(QDataStream &ts);
	QString getBinaryColor(QDataStream &ts);
	double  getBinaryDistance(QDataStream &ts);
	QPointF getBinaryCoords(QDataStream &ts, bool raw = false);
	uint    getBinaryUInt(QDataStream &ts, int intP);
	int     getBinaryInt(QDataStream &ts, int intP);
	double  getBinaryReal(QDataStream &ts, int realP, int realM);
	QString getBinaryText(QDataStream &ts);
	void    alignStreamToWord(QDataStream &ts, uint len);

/* core functions */
	void    handleStartMetaFile(const QString& value);
	void    handleStartPicture(const QString& value);
	void    handleStartPictureBody(double width, double height);
	void    handleMetaFileDescription(const QString& value);
	QString handleColor(ScColor &color, const QString& proposedName);
	double  convertCoords(double input);
	QPointF convertCoords(QPointF input);
	void appendPath(QPainterPath &path1, QPainterPath &path2);
	PageItem* itemAdd(PageItem::ItemType itemType, PageItem::ItemFrameType frameType, double x, double y, double b, double h, double w, const QString& fill, const QString& stroke);
	void    finishItem(PageItem* ite, bool line = true);

/* common variables */
	int metaFileVersion;
	int vdcType;
	int vdcInt;
	int vdcReal;
	int vdcMantissa;
	bool vcdFlippedH;
	bool vcdFlippedV;
	double vdcWidth;
	double vdcHeight;
	double metaFileScale;
	double metaScale;
	int metaFileScaleMode;
	int intPrecision;
	int realPrecision;
	int realMantissa;
	int realFraction;
	bool realPrecisionSet;
	int indexPrecision;
	int colorPrecision;
	int colorIndexPrecision;
	uint maxColorIndex;
	int m_colorModel;
	int colorMode;
	int namePrecision;
	int lineWidthMode;
	int edgeWidthMode;
	int markerSizeMode;
	double viewPortScale;
	int viewPortScaleMode;

	int lineBundleIndex;
	Qt::PenStyle lineType;
	Qt::PenCapStyle lineCap;
	Qt::PenJoinStyle lineJoin;
	double lineWidth;
	Qt::PenStyle edgeType;
	Qt::PenCapStyle edgeCap;
	Qt::PenJoinStyle edgeJoin;
	double edgeWidth;
	uint minColor, maxColor;
	QString lineColor;
	bool lineVisible;
	QString edgeColor;
	QString fillColor;
	int fillType;
	int patternIndex;
	QMap<int, QString> patternTable;
	double patternScaleX;
	double patternScaleY;
	QString backgroundColor;
	bool backgroundSet;
	QMap<uint, QString> ColorTableMap;
	QRectF clipRect;
	bool useClipRect;
	bool clipSet;
	QPainterPath regionPath;
	bool recordRegion;
	int currentRegion;
	QMap<int, QPainterPath> regionMap;
	QPointF fillRefPoint;

	QList<PageItem*> Elements;
	int currentItemNr;
	QStack<QList<PageItem*> > groupStack;
	ColorList CustColors;
	double baseX, baseY;
	double docWidth;
	double docHeight;
	QStringList importedColors;

	FPointArray Coords;
	bool interactive;
	MultiProgressDialog * progressDialog;
	bool cancel;
	ScribusDoc* m_Doc;
	Selection* tmpSel;
	int importerFlags;
	int oldDocItemCount;
	QString baseFile;
	bool importRunning;
	bool firstPage;
	bool vcdSet;
	bool wasEndPic;
	bool recordFigure;
	QPainterPath figurePath;
	bool figClose;
	int figDocIndex;
	int figElemIndex;
	int figGstIndex;
	QString figFillColor;
	QMap<int, QString> fontID_Map;
	int m_fontIndex;
	QString textColor;
	int textSize;
	int textAlignH;
	int textScaleMode;
	QString pictName;

public slots:
	void cancelRequested() { cancel = true; }
};

#endif
