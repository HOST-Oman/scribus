/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef __SCPAINTER_H__
#define __SCPAINTER_H__

#include <QPainterPath>
#include <QPainter>
#include <QVector>
#include <QStack>
#include <QColor>
#include <QTransform>
#include <QFont>
#include <QImage>
#include <QPointF>
#include "scribusapi.h"
#include "scconfig.h"
#include "fpoint.h"
#include "fpointarray.h"
#include "vgradient.h"
#include "mesh.h"
#include "sctextstruct.h"
#include "util.h"

class ScPattern;

typedef struct _cairo cairo_t;
typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo_pattern cairo_pattern_t;

class SCRIBUS_API ScPainter
{
protected:
	double m_lineWidth;


public:
	VGradient fill_gradient;
	VGradient stroke_gradient;
	VGradient mask_gradient;
	ScPattern *m_maskPattern;
	ScPattern *m_pattern;

	ScPainter() {}
	virtual ~ScPainter() = 0;

	enum FillMode { None, Solid, Gradient, Pattern, Hatch };
	virtual void drawGlyph(const GlyphLayout glyphLayout, const ScFace font, double fontSize) = 0;
	virtual void drawGlyphOutline(const GlyphLayout glyphLayout, const ScFace font, double fontSize, double outlineWidth) = 0;
	virtual void beginLayer(double transparency, int blendmode, FPointArray *clipArray = 0 ) = 0;
	virtual void endLayer( ) = 0;
	virtual void setAntialiasing(bool enable) = 0;
	virtual void begin( ) = 0;
	virtual void end( ) = 0;
	virtual void clear() = 0;
	virtual void clear( const QColor & ) = 0;

	// matrix manipulation
	virtual void setWorldMatrix( const QTransform &  ) = 0;
	virtual const QTransform worldMatrix( ) = 0;
	virtual void setZoomFactor( double  ) = 0;
	virtual double zoomFactor( ) = 0;
	virtual void translate( double, double  ) = 0;
	virtual void translate( const QPointF& offset  ) = 0;
	virtual void rotate( double  ) = 0;
	virtual void scale( double, double  ) = 0;

	// drawing
	virtual void moveTo( const double &, const double &  ) = 0;
	virtual void lineTo( const double &, const double &  ) = 0;
	virtual void curveTo( FPoint p1, FPoint p2, FPoint p3  ) = 0;
	virtual void newPath( ) = 0;
	virtual void closePath( ) = 0;
	virtual void fillPath( ) = 0;
	virtual void strokePath( ) = 0;
	virtual void setFillRule( bool fillRule  ) = 0;
	virtual bool fillRule( ) = 0;
	virtual void setFillMode( int fill  ) = 0;
	virtual int  fillMode() = 0;
	virtual void setStrokeMode( int stroke  ) = 0;
	virtual int  strokeMode() = 0;
	virtual void setGradient( VGradient::VGradientType mode, FPoint orig, FPoint vec, FPoint foc, double scale, double skew ) = 0;
	virtual void setPattern(ScPattern *pattern, double scaleX, double scaleY, double offsetX, double offsetY, double rotation, double skewX, double skewY, bool mirrorX, bool mirrorY ) = 0;

	virtual void setMaskMode( int mask  ) = 0;
	virtual void setGradientMask( VGradient::VGradientType mode, FPoint orig, FPoint vec, FPoint foc, double scale, double skew ) = 0;
	virtual void setPatternMask(ScPattern *pattern, double scaleX, double scaleY, double offsetX, double offsetY, double rotation, double skewX, double skewY, bool mirrorX, bool mirrorY ) = 0;

	virtual void set4ColorGeometry(FPoint p1, FPoint p2, FPoint p3, FPoint p4, FPoint c1, FPoint c2, FPoint c3, FPoint c4 ) = 0;
	virtual void set4ColorColors(QColor col1, QColor col2, QColor col3, QColor col4 ) = 0;
	virtual void setDiamondGeometry(FPoint p1, FPoint p2, FPoint p3, FPoint p4, FPoint c1, FPoint c2, FPoint c3, FPoint c4, FPoint c5 ) = 0;
	virtual void setMeshGradient(FPoint p1, FPoint p2, FPoint p3, FPoint p4, QList<QList<meshPoint> > meshArray ) = 0;
	virtual void setMeshGradient(FPoint p1, FPoint p2, FPoint p3, FPoint p4, QList<meshGradientPatch> meshPatches ) = 0;

	virtual void setHatchParameters(int mode, double distance, double angle, bool useBackground, QColor background, QColor foreground, double width, double height ) = 0;

	virtual void setClipPath( ) = 0;

	virtual void drawImage( QImage *image ) = 0;
	virtual void setupPolygon(FPointArray *points, bool closed = true ) = 0;
	virtual void setupSharpPolygon(FPointArray *points, bool closed = true ) = 0;
	virtual void sharpLineHelper(FPoint &pp ) = 0;
	virtual void sharpLineHelper(QPointF &pp ) = 0;
	virtual void drawPolygon( ) = 0;
	virtual void drawPolyLine( ) = 0;
	virtual void drawLine(FPoint start, FPoint end ) = 0;
	virtual void drawLine(const QPointF& start, const QPointF& end ) = 0;
	virtual void drawSharpLine(FPoint start, FPoint end ) = 0;
	virtual void drawSharpLine(QPointF start, QPointF end ) = 0;
	virtual void drawRect(double, double, double, double ) = 0;
	virtual void drawSharpRect(double x, double y, double w, double h ) = 0;
	virtual void drawText(QRectF area, QString text, bool filled = true, int align = 0 ) = 0;
	virtual void drawShadeCircle(const QRectF &re, const QColor color, bool sunken, int lineWidth ) = 0;
	virtual void drawShadePanel(const QRectF &r, const QColor color, bool sunken, int lineWidth ) = 0;
	virtual void colorizeAlpha(QColor color ) = 0;
	virtual void colorize(QColor color ) = 0;
	virtual void blurAlpha(int radius ) = 0;
	virtual void blur(int radius ) = 0;
	virtual void drawShadow(const GlyphLayout glyphLayout, const ScFace font, double fontSize, double xoff, double yoff) = 0;
	// pen + brush
	virtual QColor pen( ) = 0;
	virtual QColor brush( ) = 0;
	virtual void setPen( const QColor &  ) = 0;
	virtual void setPen( const QColor &c, double w, Qt::PenStyle st, Qt::PenCapStyle ca, Qt::PenJoinStyle jo  ) = 0;
	virtual void setPenOpacity( double op  ) = 0;
	virtual void setLineWidth( double w) { m_lineWidth = w; }
	virtual void setDash(const QVector<double>& array, double ofs ) = 0;
	virtual void setBrush( const QColor &  ) = 0;
	virtual void setBrushOpacity( double op  ) = 0;
	virtual void setOpacity( double op  ) = 0;
	virtual void setFont( const QFont &f  ) = 0;
	virtual QFont font( ) = 0;

	// stack management
	virtual void save( ) = 0;
	virtual void restore( ) = 0;

	virtual void setRasterOp( int blendMode  ) = 0;
	virtual void setBlendModeFill( int blendMode  ) = 0;
	virtual void setBlendModeStroke( int blendMode  ) = 0;
};


class SCRIBUS_API ScScreenPainter : public ScPainter
{
public:
	ScScreenPainter( QImage *target, unsigned int w, unsigned int h, double transparency = 1.0, int blendmode = 0 );
	~ScScreenPainter();
	void beginLayer(double transparency, int blendmode, FPointArray *clipArray = 0);
	void endLayer();
	void setAntialiasing(bool enable);
	void begin();
	void end();
	void clear();
	void clear( const QColor & );


	// matrix manipulation
	void setWorldMatrix( const QTransform & );
	const QTransform worldMatrix();
	void setZoomFactor( double );
	double zoomFactor() { return m_zoomFactor; }
	void translate( double, double );
	void translate( const QPointF& offset );
	void rotate( double );
	void scale( double, double );

	// drawing
	void moveTo( const double &, const double & );
	void lineTo( const double &, const double & );
	void curveTo( FPoint p1, FPoint p2, FPoint p3 );
	void newPath();
	void closePath();
	void fillPath();
	void strokePath();
	void setFillRule( bool fillRule ) { m_fillRule = fillRule; }
	bool fillRule() { return m_fillRule; }
	void setFillMode( int fill ) { m_fillMode = fill; }
	int  fillMode() { return m_fillMode; }
	void setStrokeMode( int stroke );
	int  strokeMode() { return m_strokeMode; }
	void setGradient( VGradient::VGradientType mode, FPoint orig, FPoint vec, FPoint foc, double scale, double skew);
	void setPattern(ScPattern *pattern, double scaleX, double scaleY, double offsetX, double offsetY, double rotation, double skewX, double skewY, bool mirrorX, bool mirrorY);

	void setMaskMode( int mask );
	void setGradientMask( VGradient::VGradientType mode, FPoint orig, FPoint vec, FPoint foc, double scale, double skew);
	void setPatternMask(ScPattern *pattern, double scaleX, double scaleY, double offsetX, double offsetY, double rotation, double skewX, double skewY, bool mirrorX, bool mirrorY);

	void set4ColorGeometry(FPoint p1, FPoint p2, FPoint p3, FPoint p4, FPoint c1, FPoint c2, FPoint c3, FPoint c4);
	void set4ColorColors(QColor col1, QColor col2, QColor col3, QColor col4);
	void setDiamondGeometry(FPoint p1, FPoint p2, FPoint p3, FPoint p4, FPoint c1, FPoint c2, FPoint c3, FPoint c4, FPoint c5);
	void setMeshGradient(FPoint p1, FPoint p2, FPoint p3, FPoint p4, QList<QList<meshPoint> > meshArray);
	void setMeshGradient(FPoint p1, FPoint p2, FPoint p3, FPoint p4, QList<meshGradientPatch> meshPatches);

	void setHatchParameters(int mode, double distance, double angle, bool useBackground, QColor background, QColor foreground, double width, double height);

	void setClipPath();

	void drawImage( QImage *image);
	void setupPolygon(FPointArray *points, bool closed = true);
	void setupSharpPolygon(FPointArray *points, bool closed = true);
	void sharpLineHelper(FPoint &pp);
	void sharpLineHelper(QPointF &pp);
	void drawPolygon();
	void drawPolyLine();
	void drawLine(FPoint start, FPoint end);
	void drawLine(const QPointF& start, const QPointF& end);
	void drawSharpLine(FPoint start, FPoint end);
	void drawSharpLine(QPointF start, QPointF end);
	void drawRect(double, double, double, double);
	void drawSharpRect(double x, double y, double w, double h);
	void drawText(QRectF area, QString text, bool filled = true, int align = 0);
	void drawShadeCircle(const QRectF &re, const QColor color, bool sunken, int lineWidth);
	void drawShadePanel(const QRectF &r, const QColor color, bool sunken, int lineWidth);
	void colorizeAlpha(QColor color);
	void colorize(QColor color);
	void blurAlpha(int radius);
	void blur(int radius);
	void drawGlyph(const GlyphLayout gl, const ScFace font, double fontSize);
	void drawShadow(const GlyphLayout gl, const ScFace font, double fontSize, double xoff, double yoff);
	void drawGlyphOutline(const GlyphLayout gl, const ScFace font, double fontSize, double outlineWidth);
	// pen + brush
	QColor pen();
	QColor brush();
	void setPen( const QColor &c ) { m_stroke = c; }
	void setPen( const QColor &c, double w, Qt::PenStyle st, Qt::PenCapStyle ca, Qt::PenJoinStyle jo );
	void setPenOpacity( double op );
	void setDash(const QVector<double>& array, double ofs);
	void setBrush( const QColor &c ) { m_fill = c; }
	void setBrushOpacity( double op );
	void setOpacity( double op );
	void setFont( const QFont &f );
	QFont font();

	// stack management
	void save();
	void restore();


	void setRasterOp( int blendMode );
	void setBlendModeFill( int blendMode );
	void setBlendModeStroke( int blendMode );

private:
	virtual void fillPathHelper();
	virtual void strokePathHelper();

	cairo_t *m_cr;
	struct layerProp
	{
		cairo_surface_t *data;
		int blendmode;
		double tranparency;
		int maskMode;				// 0 = none, 1 = gradient 2 = pattern
		double mask_patternScaleX;
		double mask_patternScaleY;
		double mask_patternOffsetX;
		double mask_patternOffsetY;
		double mask_patternRotation;
		double mask_patternSkewX;
		double mask_patternSkewY;
		bool mask_patternMirrorX;
		bool mask_patternMirrorY;
		double mask_gradientScale;
		double mask_gradientSkew;
		VGradient mask_gradient;
		ScPattern *maskPattern;
		FPointArray groupClip;
		bool pushed;
		bool fillRule;
	};
protected:
	cairo_pattern_t *getMaskPattern();
	cairo_surface_t *m_imageMask;
	QImage m_imageQ;

	QStack<layerProp> m_Layers;
	QStack<double> m_zoomStack;
	QImage *m_image;
	double  m_layerTransparency;
	int  m_blendMode;
	int  m_blendModeFill;
	int  m_blendModeStroke;
	unsigned int m_width;
	unsigned int m_height;
	QTransform m_matrix;
	QFont m_font;
	bool mf_underline;
	bool mf_strikeout;
	bool mf_shadow;
	bool mf_outlined;
	/*! \brief Filling */
	QColor m_fill;
	double m_fill_trans;
	bool m_fillRule;
	int m_fillMode;				// 0 = none, 1 = solid, 2 = gradient 3 = pattern 4 = hatch
	double m_patternScaleX;
	double m_patternScaleY;
	double m_patternOffsetX;
	double m_patternOffsetY;
	double m_patternRotation;
	double m_patternSkewX;
	double m_patternSkewY;
	bool m_patternMirrorX;
	bool m_patternMirrorY;
	double m_gradientScale;
	double m_gradientSkew;
	FPoint gradPatchP1;
	FPoint gradPatchP2;
	FPoint gradPatchP3;
	FPoint gradPatchP4;
	FPoint gradControlP1;
	FPoint gradControlP2;
	FPoint gradControlP3;
	FPoint gradControlP4;
	FPoint gradControlP5;
	QColor gradPatchColor1;
	QColor gradPatchColor2;
	QColor gradPatchColor3;
	QColor gradPatchColor4;
	QList<QList<meshPoint> > meshGradientArray;
	QList<meshGradientPatch> meshGradientPatches;
	double m_hatchAngle;
	double m_hatchDistance;
	int m_hatchType;				// 0 = single 1 = double 2 = triple
	bool m_hatchUseBackground;
	QColor m_hatchBackground;
	QColor m_hatchForeground;
	double m_hatchWidth;
	double m_hatchHeight;
	/*! \brief Stroking */
	QColor m_stroke;
	double m_stroke_trans;
	double m_LineWidth;
	int m_strokeMode;				// 0 = none, 1 = solid, 2 = gradient 3 = pattern
	int m_maskMode;				// 0 = none, 1 = gradient 2 = pattern
	double m_mask_patternScaleX;
	double m_mask_patternScaleY;
	double m_mask_patternOffsetX;
	double m_mask_patternOffsetY;
	double m_mask_patternRotation;
	double m_mask_patternSkewX;
	double m_mask_patternSkewY;
	bool m_mask_patternMirrorX;
	bool m_mask_patternMirrorY;
	double m_mask_gradientScale;
	double m_mask_gradientSkew;

	/*! \brief Zoom Factor of the Painter */
	double m_zoomFactor;
	bool imageMode;
	bool layeredMode;
	bool svgMode;
	/*! \brief Line End Style */
	Qt::PenCapStyle PLineEnd;
  /*! \brief Line Join Style */
	Qt::PenJoinStyle PLineJoin;
  /*! \brief The Dash Array */
	QVector<double> m_array;
	double m_offset;
	/*! \brief Zoom Factor of the Painter */
	bool m_imageMode;
	bool m_layeredMode;
	bool m_svgMode;
};

#endif
