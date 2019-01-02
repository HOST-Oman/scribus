/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include <cassert>

#include "paletteloader.h"
#include "sccolor.h"
#include "scribusdoc.h"

PaletteLoader::PaletteLoader()
{
	m_colors = nullptr;
	m_gradients = nullptr;
}

void PaletteLoader::setupTargets(ColorList* colors, QHash<QString, VGradient> *gradients)
{
	assert (colors != nullptr);
	
	m_colors = colors;
	gradients = m_gradients;
}
