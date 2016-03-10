/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */

#include "textlayoutpainter.h"

TextLayoutPainter::~TextLayoutPainter()
{ }

void TextLayoutPainter::setFont(const ScFace font)
{
        m_state.font = font;
}

ScFace TextLayoutPainter::font()
{
        return m_state.font;
}

void TextLayoutPainter::setFontSize(double size)
{
        m_state.fontSize = size;
}

double TextLayoutPainter::fontSize()
{
        return m_state.fontSize;
}

void TextLayoutPainter::setStrokeColor(TextLayoutColor color)
{
        m_state.strokeColor = color;
}

TextLayoutColor TextLayoutPainter::strokeColor()
{
        return m_state.strokeColor;
}

void TextLayoutPainter::setFillColor(TextLayoutColor color)
{
        m_state.fillColor = color;
}

TextLayoutColor TextLayoutPainter::fillColor()
{
        return m_state.fillColor;
}

void TextLayoutPainter::setStrokeWidth(double w)
{
        m_state.strokeWidth = w;
}

double TextLayoutPainter::strokeWidth()
{
        return m_state.strokeWidth;
}

void TextLayoutPainter::translate(double x, double y)
{
        m_state.x += x;
        m_state.y += y;
}

double TextLayoutPainter::x()
{
        return m_state.x;
}

double TextLayoutPainter::y()
{
        return m_state.y;
}

void TextLayoutPainter::save()
{
        m_stack.push(m_state);
}

void TextLayoutPainter::restore()
{
        m_state = m_stack.pop();
}

void TextLayoutPainter::scale(double h, double v)
{
        m_state.scaleH = h;
        m_state.scaleV = v;
}

double TextLayoutPainter::getScaleV()
{
        return m_state.scaleV;
}

double TextLayoutPainter::getScaleH()
{
        return m_state.scaleH;
}
