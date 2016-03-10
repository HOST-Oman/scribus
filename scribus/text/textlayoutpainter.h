/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */

#ifndef TEXTLAYOUTPAINTER_H
#define TEXTLAYOUTPAINTER_H

#include <QStack>

#include "scribusapi.h"
#include "sctextstruct.h"


struct TextLayoutColor
{
        QString color;
        double shade;

        TextLayoutColor()
                : color("Black")
                , shade(100)
        { }

        TextLayoutColor(QString c, double s=100)
                : color(c)
                , shade(s)
        { }
};

class SCRIBUS_API TextLayoutPainter
{
public:
        TextLayoutPainter() { }
        virtual ~TextLayoutPainter();

        virtual void setFont(const ScFace font);
        virtual ScFace font();

        virtual void setFontSize(double size);
        virtual double fontSize();

        virtual void setStrokeColor(TextLayoutColor c);
        virtual TextLayoutColor strokeColor();

        virtual void setFillColor(TextLayoutColor c);
        virtual TextLayoutColor fillColor();

        virtual void setStrokeWidth(double w);
        virtual double strokeWidth();

        virtual void translate(double x, double y);
        virtual double x();
        virtual double y();

        virtual void drawGlyph(const GlyphLayout gl, bool selected) = 0;
        virtual void drawGlyphOutline(const GlyphLayout gl, bool fill, bool selected) = 0;
        virtual void drawLine(QPointF start, QPointF end) = 0;
        virtual void drawRect(QRectF rect) = 0;
        virtual void drawObject(PageItem* item) = 0;

        virtual void save();
        virtual void restore();
        virtual void scale(double h, double v);
        virtual double getScaleV();
        virtual double getScaleH();

private:
        struct State
        {
                ScFace font;
                double fontSize;
                TextLayoutColor strokeColor;
                TextLayoutColor fillColor;
                double strokeWidth;
                double x;
                double y;
                double scaleH;
                double scaleV;

                State()
                        : fontSize(0)
                        , strokeWidth(0)
                        , x(0)
                        , y(0)
                        , scaleH(1)
                        , scaleV(1)
                {}
        };

        State m_state;
        QStack<State> m_stack;
};

#endif // TEXTLAYOUTPAINTER_H
