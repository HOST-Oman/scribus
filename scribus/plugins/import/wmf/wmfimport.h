/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

/* Code inspired by KOffice libwmf and adapted for Scribus by Jean Ghali */

#ifndef WMFIMPORT_H
#define WMFIMPORT_H

#include <QtXml/QtXml>

#include <QColor>
#include <QList>
#include <QSize>
#include <QTextCodec>

#include "pluginapi.h"
#include "fpointarray.h"
#include "wmfcontext.h"

class QBuffer;
class FPointArray;
class PageItem;
class ScribusDoc;
class Selection;
class PrefsManager;
class TransactionSettings;

class  WmfCmd;
class  WmfObjHandle;
class  WmfObjPenHandle;
class  WmfObjBrushHandle;
struct WmfPlaceableHeader;

class WMFImport : public QObject
{
	Q_OBJECT

public:

	WMFImport(ScribusDoc* doc, int flags);
	~WMFImport();

	bool import(const QString& fname, const TransactionSettings& trSettings, int flags);
	QImage readThumbnail(const QString& fn);

	void wmfClosePath(FPointArray *i);
	void wmfMoveTo(double x1, double y1);
	void wmfLineTo(FPointArray *i, double x1, double y1);
	void wmfCurveToCubic(FPointArray *i, double x1, double y1, double x2, double y2, double x3, double y3);

	bool interactive { false };
	//! \brief Indicator if there is any unsupported feature in imported wmf.
    bool unsupported { false };
	bool importFailed { false };
    bool importCanceled { true };

    ScribusDoc* m_Doc { nullptr };
	Selection*  m_tmpSel { nullptr };
	QStringList importedColors;

protected:

	QString m_docDesc;
	QString m_docTitle;

	WMFContext m_context;

    bool m_IsPlaceable { false };
    bool m_IsEnhanced { false };
    bool m_Valid { false };

    // coordinate system
    QRect  m_HeaderBoundingBox;
    QRect  m_BBox;

	QList<WmfCmd*> m_commands;
    WmfObjHandle** m_ObjHandleTab { nullptr };
    FPointArray    m_Points;
    int            m_Dpi { 1440 };

	/** Protected import functions */
	bool importWMF(const TransactionSettings& trSettings, int flags);
	bool loadWMF( const QString &fileName );
    bool loadWMF( QBuffer &buffer );

	QList<PageItem*> parseWmfCommands(void);

	void finishCmdParsing( PageItem* item );

	/** Import a QColor */
	QString importColor(const QColor& color);

	/** Get color from parameters array */
	QColor  colorFromParam(const short* params );

	/** Get text codec from charset code */
	QTextCodec* codecFromCharset( int charset );

	/** Translate characters in symbol charset to unicode */
	QString symbolToUnicode ( const QByteArray& chars ) const;

	/** Get polygon array from parameters array */
	FPointArray pointsFromParam( short num, const short* params ) const;

	/** Transform a point array to an item path used as polyline */
	FPointArray pointsToPolyline( const FPointArray& points, bool closePath ) const;

	void pointsToAngle( double xStart, double yStart, double xEnd, double yEnd, double& angleStart, double& angleLength ) const;

	/** Handle win-object-handles */
    void addHandle( WmfObjHandle*  );
    void deleteHandle( int );

	/** Calculate header checksum */
    unsigned short calcCheckSum( WmfPlaceableHeader* );

	/** Find function in metafunc table by metafile-function.
        Returns index or -1 if not found. */
    virtual int findFunc( unsigned short aFunc ) const;

	/** Converts two parameters to long */
	unsigned int toDWord( const short* params );

public:

	/** set window origin */
    void setWindowOrg( QList<PageItem*>& items, long num, const short* params );
    /** set window extents */
    void setWindowExt( QList<PageItem*>& items, long num, const short* params );

    /****************** Drawing *******************/
    /** draw line to coord */
    void lineTo( QList<PageItem*>& items, long num, const short* params );
    /** move pen to coord */
    void moveTo( QList<PageItem*>& items, long num, const short* params );
    /** draw ellipse */
	void ellipse(QList<PageItem*>& items, long num, const short* params );
    /** draw polygon */
    void polygon( QList<PageItem*>& items, long num, const short* params );
    /** draw a list of polygons */
    void polyPolygon( QList<PageItem*>& items, long num, const short* params );
    /** draw series of lines */
    void polyline( QList<PageItem*>& items, long num, const short* params );
    /** draw a rectangle */
    void rectangle( QList<PageItem*>& items, long num, const short* params );
    /** draw round rectangle */
    void roundRect( QList<PageItem*>& items, long num, const short* params );
    /** draw arc */
    void arc( QList<PageItem*>& items, long num, const short* params );
    /** draw chord */
    void chord( QList<PageItem*>& items, long num, const short* params );
    /** draw pie */
    void pie( QList<PageItem*>& items, long num, const short* params );
    /** set polygon fill mode */
    void setPolyFillMode( QList<PageItem*>& items, long num, const short* params );
    /** set background pen color */
    void setBkColor( QList<PageItem*>& items, long num, const short* params );
    /** set background pen mode */
	void setBkMode( QList<PageItem*>& items, long num, const short* params );
    /** save device context */
    void saveDC( QList<PageItem*>& items, long num, const short* params );
    /** restore device context */
	void restoreDC( QList<PageItem*>& items, long num, const short* params );
    /**  clipping region is the intersection of this region and the original region */
    void intersectClipRect( QList<PageItem*>& items, long num, const short* params );
    /** delete a clipping rectangle of the original region */
    void excludeClipRect( QList<PageItem*>& items, long num, const short* params );

    /****************** Text *******************/
    /** set text color */
    void setTextColor( QList<PageItem*>& items, long num, const short* params );
    /** set text alignment */
    void setTextAlign( QList<PageItem*>& items, long num, const short* params );
    /** draw text */
    void textOut( QList<PageItem*>& items, long num, const short* params );
    void extTextOut( QList<PageItem*>& items, long num, const short* params );

    /****************** Object handle *******************/
    /** Activate object handle */
	void selectObject( QList<PageItem*>& items, long num, const short* params );
    /** Free object handle */
    void deleteObject( QList<PageItem*>& items, long num, const short* params );
    /** create an empty object in the object list */
    void createEmptyObject( QList<PageItem*>& items, long num, const short* params );
    /** create a logical brush */
    void createBrushIndirect( QList<PageItem*>& items, long num, const short* params );
    /** create a logical pen */
    void createPenIndirect( QList<PageItem*>& items, long num, const short* params );
    /** create a logical font */
    void createFontIndirect( QList<PageItem*>& items, long num, const short* params );

    /****************** misc *******************/
    /** nothing to do */
    void noop( QList<PageItem*>& items, long , const short* );
    /** end of meta file */
    void end( QList<PageItem*>& items, long /*num*/, const short* /*params*/ );

};

#endif
