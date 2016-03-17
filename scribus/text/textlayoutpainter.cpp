/*
 For general Scribus (>=1.3.2) copyright and licensing information please refer
 to the COPYING file provided with the program. Following this notice may exist
 a copyright and/or license notice that predates the release of Scribus 1.3.2
 for which a new license (GPL+exception) is in place.
 */

#include "textlayoutpainter.h"

TextLayoutPainter::TextLayoutPainter()
{ }

TextLayoutPainter::~TextLayoutPainter()
{ }

void TextLayoutPainter::setFont(const ScFace font)
{
	if (m_state.font != font)
		m_state.font = font;
}

const ScFace TextLayoutPainter::font() const
{
	return m_state.font;
}

void TextLayoutPainter::setFontSize(double size)
{
	m_state.fontSize = size;
}

double TextLayoutPainter::fontSize() const
{
	return m_state.fontSize;
}

void TextLayoutPainter::setStrokeColor(TextLayoutColor color)
{
	m_state.strokeColor = color;
}

TextLayoutColor TextLayoutPainter::strokeColor() const
{
	return m_state.strokeColor;
}

void TextLayoutPainter::setFillColor(TextLayoutColor color)
{
	m_state.fillColor = color;
}

TextLayoutColor TextLayoutPainter::fillColor() const
{
	return m_state.fillColor;
}

void TextLayoutPainter::setStrokeWidth(double w)
{
	m_state.strokeWidth = w;
}

double TextLayoutPainter::strokeWidth() const
{
	return m_state.strokeWidth;
}

void TextLayoutPainter::translate(double x, double y)
{
	m_state.x += x;
	m_state.y += y;
}

double TextLayoutPainter::x() const
{
	return m_state.x;
}

double TextLayoutPainter::y() const
{
	return m_state.y;
}

void TextLayoutPainter::scale(double h, double v)
{
	m_state.scaleH = h;
	m_state.scaleV = v;
}

double TextLayoutPainter::getScaleV() const
{
	return m_state.scaleV;
}

double TextLayoutPainter::getScaleH() const
{
	return m_state.scaleH;
}

void TextLayoutPainter::setSelected(bool s)
{
	m_state.selected = s;
}

bool TextLayoutPainter::selected() const
{
	return m_state.selected;
}

void TextLayoutPainter::save()
{
	m_stack.push(m_state);
}

void TextLayoutPainter::restore()
{
	m_state = m_stack.top();
	m_stack.pop();
}
