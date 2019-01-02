/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef SVGPLUG_H
#define SVGPLUG_H

#include <QObject>
#include <QDomElement>
#include "pluginapi.h"
#include "loadsaveplugin.h"
#include "tableborder.h"

class QString;
class ScLayer;
class ScribusDoc;
class ScribusMainWindow;
class PageItem;
class ScPage;
class ScText;

struct SVGOptions
{
	bool inlineImages;
	bool exportPageBackground;
	bool compressFile;
};

class PLUGIN_API SVGExportPlugin : public ScActionPlugin
{
	Q_OBJECT

public:
	// Standard plugin implementation
	SVGExportPlugin();
	virtual ~SVGExportPlugin();
	/*!
	\author Franz Schmid
	\brief Run the SVG export
	\param filename a file to export to
	\retval bool true
	*/
	virtual bool run(ScribusDoc* doc=0, const QString& filename = QString::null);
	virtual const QString fullTrName() const;
	virtual const AboutData* getAboutData() const;
	virtual void deleteAboutData(const AboutData* about) const;
	virtual void languageChange();
	virtual void addToMainWindowMenu(ScribusMainWindow *) {};

	// Special features (none)
};

extern "C" PLUGIN_API int svgexplugin_getPluginAPIVersion();
extern "C" PLUGIN_API ScPlugin* svgexplugin_getPlugin();
extern "C" PLUGIN_API void svgexplugin_freePlugin(ScPlugin* plugin);

class SVGExPlug : public QObject
{
	Q_OBJECT
	friend class SvgPainter;

public:
	/*!
	\author Franz Schmid
	\brief Create the SVG exporter window
	\param fName QString file name
	 */
	SVGExPlug( ScribusDoc* doc );
	~SVGExPlug();

	bool doExport( const QString& fName, SVGOptions &Opts );
	SVGOptions Options;

private:
	ScribusDoc* m_Doc;
	/*!
	\author Franz Schmid
	\brief Process a page to export to SVG format
	\param Seite Page *
	*/
	void ProcessPageLayer(ScPage *page, ScLayer& layer);
	void ProcessItemOnPage(double xOffset, double yOffset, PageItem *Item, QDomElement *parentElem);
	void paintBorder(const TableBorder& border, const QPointF& start, const QPointF& end, const QPointF& startOffsetFactors, const QPointF& endOffsetFactors, QDomElement &ob);
	QString processDropShadow(PageItem *Item);
	QDomElement processHatchFill(PageItem *Item, const QString& transl = "");
	QDomElement processSymbolStroke(PageItem *Item, const QString& trans);
	QDomElement processSymbolItem(PageItem *Item, const QString& trans);
	QDomElement processPolyItem(PageItem *Item, const QString& trans, const QString& fill, const QString& stroke);
	QDomElement processLineItem(PageItem *Item, const QString& trans, const QString& stroke);
	QDomElement processImageItem(PageItem *Item, const QString& trans, const QString& fill, const QString& stroke);
	QDomElement processTextItem(PageItem *Item, const QString& trans, const QString& fill, const QString& stroke);
	QDomElement processInlineItem(PageItem* embItem, const QString& trans, double scaleH, double scaleV);
	QString handleGlyph(uint gid, const ScFace& font);
	QDomElement processArrows(PageItem *Item, const QDomElement& line, const QString& trans);
	QString handleMask(PageItem *Item, double xOffset, double yOffset);
	QString getFillStyle(PageItem *Item);
	QString getStrokeStyle(PageItem *Item);
	void writeBasePatterns();
	void writeBaseSymbols();
	/*!
	\author Franz Schmid
	\param ite PageItem *
	\retval QString Clipping Path
	*/
	QString SetClipPath(FPointArray *ite, bool closed);
	QDomElement createClipPathElement(FPointArray *ite, QDomElement* pathElem = 0);
	/*!
	\author Franz Schmid
	\brief Converts double to string
	\param c double
	\retval QString
		*/
	QString FToStr(double c);
	/*!
	\author Franz Schmid
	\brief Converts integer to QString
	\param c int
	\retval QString representation of value
	*/
	QString IToStr(int c);
	/*!
	\author Franz Schmid
	\param farbe QString color
	\param shad int
	\param plug ScribusMainWindow *
	\retval QString Colour settings
	*/
	QString MatrixToStr(QTransform &mat);
	QString SetColor(const QString& farbe, int shad);
	/*!
	\author Franz Schmid
	\param sl struct SingleLine *
	\param Item PageItem *
	\retval QString Stroke settings
	*/
	QString GetMultiStroke(struct SingleLine *sl, PageItem *Item);
	int GradCount;
	int ClipCount;
	int PattCount;
	int MaskCount;
	int FilterCount;
	QString baseDir;
	QDomDocument docu;
	QDomElement docElement;
	QDomElement globalDefs;
	QList<QString> glyphNames;
};

#endif
