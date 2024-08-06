#ifndef SCCOLORSPACEDATA_GRAY_H
#define SCCOLORSPACEDATA_GRAY_H

#include <cassert>
#include <climits>
#include <limits>
#include "sccolorprofile.h"
#include "sccolorspacedata.h"

template<typename T, eColorFormat COLORFORMAT>
class ScColorSpaceDataTempl_Gray : public ScColorSpaceData
{
public:
	ScColorSpaceDataTempl_Gray(ScColorProfile& profile);

	uint alphaIndex(void) const override { return 0; }
	void flattenAlpha(void* dataIn, uint numElems) const override {};
};

template<typename T, eColorFormat COLORFORMAT>
ScColorSpaceDataTempl_Gray<T, COLORFORMAT>::ScColorSpaceDataTempl_Gray(ScColorProfile& profile)
{
	m_colorFormat = COLORFORMAT;
	m_profile     = profile;
	assert(m_colorFormat == Format_GRAY_8 || m_colorFormat == Format_GRAY_16);
	if (m_profile)
	{
		assert(m_profile.colorSpace() == ColorSpace_Gray);
	}
};

using ScColorSpaceData_GRAY8	= ScColorSpaceDataTempl_Gray<unsigned char, Format_GRAY_8>;
using ScColorSpaceData_GRAY16	= ScColorSpaceDataTempl_Gray<unsigned short, Format_GRAY_16>;

#endif
