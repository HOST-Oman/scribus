/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef IMPORTAI_H
#define IMPORTAI_H

#include <QList>
#include <QTransform>
#include <QObject>
#include <QString>

#include "pluginapi.h"

#include "fpointarray.h"
#include "mesh.h"
#include "sccolor.h"
#include "text/storytext.h"
#include "vgradient.h"

class MultiProgressDialog;
class PageItem;
class ScribusDoc;
class Selection;
class TransactionSettings;

//! \brief Adobe Illustrator importer plugin
class AIPlug : public QObject
{
	Q_OBJECT

public:
	/*!
	\author Franz Schmid
	\date
	\brief Create the AI importer window.
	\param doc a Scribus document reference
	\param flags combination of loadFlags - see loadFlags in LoadSavePlugin
	*/
	AIPlug( ScribusDoc* doc, int flags );
	~AIPlug();

	/*!
	\author Franz Schmid
	\date
	\brief Perform import.
	\param fNameIn QString a filename to import
	\param trSettings undo transaction settings
	\param flags combination of loadFlags in LoadSavePlugin
	\param showProgress if progress must be displayed
	\retval bool true if import was ok
	 */
	bool import(const QString& fNameIn, const TransactionSettings& trSettings, int flags, bool showProgress = true);
	QImage readThumbnail(const QString& fn);
	bool readColors(const QString& fileName, ColorList & colors);

private:
	
	/*!
	\author Franz Schmid
	\date
	\brief Does the conversion.
	\param infile a filename
	\param outfile a filename for output
	\retval bool true if conversion was ok
	 */
	bool extractFromPDF(const QString& infile, const QString& outfile);

	bool decompressAIData(QString &fName);
	bool parseHeader(const QString& fName, double &x, double &y, double &b, double &h);
	QString removeAIPrefix(QString comment);
	QString parseColor(QString data);
	QString parseColorGray(QString data);
	QString parseColorRGB(QString data);
	QString parseCustomColor(QString data, double &shade);
	QString parseCustomColorX(QString data, double &shade, const QString& type);
	QStringList getStrings(const QString& data);
	void getCommands(const QString& data, QStringList &commands);
	void decodeA85(QByteArray &psdata, const QString& tmp);
	void processData(const QString& data);
	void processGradientData(const QString& data);
	void processSymbol(QDataStream &ts, bool sym);
	void processPattern(QDataStream &ts);
	void processRaster(QDataStream &ts);
	void processComment(QDataStream &ts, const QString& comment);
	bool convert(const QString& fn);
	
	QList<PageItem*> Elements;
	QList<PageItem*> PatternElements;
	QStack<QList<PageItem*> > groupStack;
	QStack<FPointArray> clipStack;
	ColorList CustColors;
	QStringList importedColors;
	QStringList importedGradients;
	QStringList importedPatterns;
	double baseX, baseY;
	double docX;
	double docY;
	double docWidth;
	double docHeight;

	double LineW;
	Qt::PenCapStyle CapStyle;
	Qt::PenJoinStyle JoinStyle;
	double DashOffset;
	QList<double> DashPattern;
	double Opacity;
	int blendMode;
	QString CurrColorFill;
	QString CurrColorStroke;
	double CurrStrokeShade;
	double CurrFillShade;
	bool fillRule;
	bool itemLocked;

	FPointArray Coords;
	FPointArray clipCoords;
	FPointArray currentSpecialPath;
	FPoint currentPoint;
	int currentLayer;
	bool firstLayer;
	bool FirstU, WasU, ClosedPath;
	bool interactive;
	MultiProgressDialog * progressDialog;
	bool cancel;
	ScribusDoc* m_Doc;
	Selection* tmpSel;
	int importerFlags;
	QStringList commandList;
	bool convertedPDF;
	QMap<QString, VGradient> m_gradients;
	VGradient currentGradient;
	QString currentGradientName;
	QTransform currentGradientMatrix;
	QPointF currentGradientOrigin;
	double currentGradientAngle;
	double currentGradientLenght;
	bool gradientMode;
	bool wasBC;
	bool itemRendered;
	QTransform startMatrix;
	QTransform endMatrix;
	bool patternMode;
	QString currentPatternDefName;
	QString currentPatternName;
	double patternX1;
	double patternY1;
	double patternX2;
	double patternY2;
	double currentPatternX;
	double currentPatternY;
	double currentPatternXScale;
	double currentPatternYScale;
	double currentPatternRotation;
	QString currentStrokePatternName;
	double currentStrokePatternX;
	double currentStrokePatternY;
	double currentStrokePatternXScale;
	double currentStrokePatternYScale;
	double currentStrokePatternRotation;
	bool meshMode;
	int meshXSize, meshYSize;
	int currentMeshXPos, currentMeshYPos;
	int meshNodeCounter;
	int meshColorMode;
	double meshNode1PointX, meshNode1PointY;
	double meshNode1Control1X, meshNode1Control1Y;
	double meshNode1Control2X, meshNode1Control2Y;
	double meshNode2PointX, meshNode2PointY;
	double meshNode2Control1X, meshNode2Control1Y;
	double meshNode2Control2X, meshNode2Control2Y;
	double meshNode3PointX, meshNode3PointY;
	double meshNode3Control1X, meshNode3Control1Y;
	double meshNode3Control2X, meshNode3Control2Y;
	double meshNode4PointX, meshNode4PointY;
	double meshNode4Control1X, meshNode4Control1Y;
	double meshNode4Control2X, meshNode4Control2Y;
	QString meshColor1, meshColor2, meshColor3, meshColor4;
	QList<QList<MeshPoint> > meshGradientArray;
	QString docCreator;
	QString docDate;
	QString docTime;
	QString docOrganisation;
	QString docTitle;
	int textMode;
	QTransform textMatrix;
	StoryText textData;
	QString textFont;
	double textSize;
	double maxWidth;
	double tempW;
	double maxHeight;
	double textKern;
	double textScaleH;
	double textScaleV;
	int startCurrentTextRange;
	int endCurrentTextRange;
	QString currentSymbolName;
	QMap<QString, QPointF> importedSymbols;
	bool symbolMode;
	bool dataMode;
    bool fObjectMode;
	QString dataString;

public slots:
	void cancelRequested() { cancel = true; }
};

#endif
