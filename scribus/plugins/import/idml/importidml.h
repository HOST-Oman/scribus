/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
							 -------------------
	begin                : Sat Jan 14 2012
	copyright            : (C) 2012 by Franz Schmid
	email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/
#ifndef IMPORTIDML_H
#define IMPORTIDML_H

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
#include <QDomDocument>
#include <QDomElement>

#include "third_party/zip/scribus_zip.h"

class MultiProgressDialog;
class ScribusDoc;
class Selection;
class TransactionSettings;

//! \brief Idml importer plugin
class IdmlPlug : public QObject
{
	Q_OBJECT

public:
	/*!
	\author Franz Schmid
	\date
	\brief Create the Idml importer window.
	\param fName QString
	\param flags combination of loadFlags
	\param showProgress if progress must be displayed
	\retval EPSPlug plugin
	*/
	IdmlPlug( ScribusDoc* doc, int flags );
	~IdmlPlug();

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
	bool readColors(const QString& fileName, ColorList & colors);

private:
	struct ObjectStyle
	{
		QString parentStyle;
		QString fillColor;
		QString fillGradient;
		QString strokeColor;
		QString strokeGradient;
		int fillTint;
		int strokeTint;
		double lineWidth;
		double Opacity;
		int blendMode;
		QPointF gradientFillStart;
		double gradientFillLength;
		double gradientFillAngle;
		QPointF gradientStrokeStart;
		double gradientStrokeLength;
		double gradientStrokeAngle;
		double Extra;
		double TExtra;
		double BExtra;
		double RExtra;
		double TextColumnCount;
		double TextColumnGutter;
		double TextColumnFixedWidth;
		PageItem::TextFlowMode TextFlow;
		QString LeftLineEnd;
		QString RightLineEnd;
	};
	QString getNodeValue(QDomNode &baseNode, const QString& path);
	bool convert(const QString& fn);
	bool parseFontsXML(const QDomElement& grElem);
	void parseFontsXMLNode(const QDomElement& grNode);
	bool parseGraphicsXML(const QDomElement& grElem);
	void parseGraphicsXMLNode(const QDomElement& grNode);
	bool parseStylesXML(const QDomElement& sElem);
	void parseStylesXMLNode(const QDomElement& sNode);
	void parseObjectStyle(const QDomElement& styleElem);
	void parseCharacterStyle(const QDomElement& styleElem);
	void parseParagraphStyle(const QDomElement& styleElem);
	bool parsePreferencesXML(const QDomElement& prElem);
	void parsePreferencesXMLNode(const QDomElement& prNode);
	bool parseSpreadXML(const QDomElement& spElem);
	void parseSpreadXMLNode(const QDomElement& spNode);
	QList<PageItem*> parseItemXML(const QDomElement& itElem, const QTransform& pTrans = QTransform());
	bool parseStoryXML(const QDomElement& stElem);
	void parseStoryXMLNode(const QDomElement& stNode);
	void parseParagraphStyleRange(QDomElement &ste, PageItem* item);
	void parseCharacterStyleRange(QDomElement &stt, PageItem* item, QString fontBase, QString fontStyle, ParagraphStyle &newStyle, int posC);
	void readCharStyleAttributes(CharStyle &newStyle, const QDomElement &styleElem);
	void readParagraphStyleAttributes(ParagraphStyle &newStyle, const QDomElement &styleElem);
	void resolveObjectStyle(ObjectStyle &nstyle, const QString& baseStyleName);
	int convertBlendMode(const QString& blendName);
	QString constructFontName(const QString& fontBaseName, const QString& fontStyle);
	
	QList<PageItem*> Elements;
	double baseX, baseY;
	double docWidth;
	double docHeight;

	FPointArray Coords;
	bool interactive;
	MultiProgressDialog * progressDialog;
	bool cancel;
	ScribusDoc* m_Doc;
	Selection* tmpSel;
	int importerFlags;
	QString baseFile;
	bool firstLayer;
	bool firstPage;
	int pagecount;
	int mpagecount;
	bool facingPages;
	QDomDocument designMapDom;
	QStringList importedColors;
	QMap<QString, QString> colorTranslate;
	QStringList importedGradients;
	QMap<QString, QString> gradientTranslate;
	QMap<QString, int> gradientTypeMap;
	QMap<QString, QString> layerTranslate;
	QMap<QString, PageItem*> storyMap;
	QMap<QString, QString> styleTranslate;
	QMap<QString, QStringList> styleParents;
	QMap<QString, QString> charStyleTranslate;
	QMap<QString, QMap<QString, QString> > fontTranslateMap;
	QMap<QString, QStringList> masterSpreads;
	QString def_fillColor;
	QString def_fillGradient;
	QString def_strokeColor;
	QString def_strokeGradient;
	QString def_LeftLineEnd;
	QString def_RightLineEnd;
	int def_Blendmode;
	int def_fillBlendmode;
	int def_strokeBlendmode;
	int def_fillTint;
	int def_strokeTint;
	double def_lineWidth;
	double def_Opacity;
	double def_fillOpacity;
	double def_strokeOpacity;
	double def_gradientAngle;
	double def_gradientLen;
	double def_gradientX;
	double def_gradientY;
	double def_gradientStrokeStartX;
	double def_gradientStrokeStartY;
	double def_gradientStrokeLength;
	double def_gradientStrokeAngle;
	double def_Extra;
	double def_TExtra;
	double def_BExtra;
	double def_RExtra;
	double def_TextColumnCount;
	double def_TextColumnGutter;
	double def_TextColumnFixedWidth;
	PageItem::TextFlowMode def_TextFlow;
	QMap<PageItem*, QString> frameLinks;
	QMap<QString, PageItem*> frameTargets;
	QMap<QString, ObjectStyle> ObjectStyles;

	ScZipHandler *fun;

public slots:
	void cancelRequested() { cancel = true; }
};

#endif
