/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
 
#ifndef SCCOLORMGMTSTRUCTS_H
#define SCCOLORMGMTSTRUCTS_H

#include <QString>

// If you change that enum do not forget to update functions
// colorFormatType() and colorFormatHasAlpha()
enum eColorFormat
{
	Format_Undefined,
	Format_RGB_8,
	Format_RGB_16,
	Format_RGBA_8,
	Format_RGBA_16,
	Format_ARGB_8,
	Format_ARGB_16,
	Format_BGRA_8,
	Format_BGRA_16,
	Format_CMYK_8,
	Format_CMYK_16,
	Format_CMYKA_8,
	Format_CMYKA_16,
	Format_YMCK_8,
	Format_YMCK_16,
	Format_GRAY_8,
	Format_GRAY_16,
	Format_LabA_8,
	Format_Lab_Dbl
};

enum eColorType
{
	Color_Unknown,
	Color_RGB,
	Color_CMYK,
	Color_Gray,
	Color_Lab
};

enum eColorTransformFlags
{
	Ctf_BlackPointCompensation = 1,
	Ctf_BlackPreservation      = 2,
	Ctf_Softproofing           = 4,
	Ctf_GamutCheck             = 8
};

enum eRenderIntent
{
	Intent_Perceptual = 0,
	Intent_Relative_Colorimetric = 1,
	Intent_Saturation = 2,
	Intent_Absolute_Colorimetric = 3,
	Intent_Max = 4
};

enum eColorSpaceType
{
	ColorSpace_Unknown,
	ColorSpace_XYZ,
	ColorSpace_Lab,
	ColorSpace_Luv,
	ColorSpace_YCbCr,
	ColorSpace_Yxy,
	ColorSpace_Rgb,
	ColorSpace_Gray,
	ColorSpace_Hsv,
	ColorSpace_Hls,
	ColorSpace_Cmyk,
	ColorSpace_Cmy
};

enum eProfileClass
{
	Class_Unknown,
	Class_Input,
	Class_Display,
	Class_Output,
	Class_Link,
	Class_Abstract,
	Class_ColorSpace,
	Class_NamedColor
};

enum eIlluminant
{
	Illuminant_D50,
	Illuminant_D65
};

enum eObserver
{
	Observer_2deg,
	Observer_10deg
};

class ScColorMgmtStrategy
{
	public:
		bool useBlackPointCompensation() const;
		void setUseBlackPointCompensation(bool useBlackPointCompensation);
		bool useBlackPreservation() const;
		void setUseBlackPreservation(bool useBlackPreservation);

	private:
		bool m_useBlackPointCompensation {true};
		bool m_useBlackPreservation {false};
};

struct ScColorProfileInfo
{
	QString file;
	QString description;
	eColorSpaceType colorSpace;
	eProfileClass   deviceClass;
	QString debug;
};

struct ScColorTransformInfo
{
	QString inputProfile;
	QString outputProfile;
	QString proofingProfile;
	eColorFormat inputFormat;
	eColorFormat outputFormat;
	eRenderIntent renderIntent;
	eRenderIntent proofingIntent;
	long flags;
};

bool operator==(const ScColorTransformInfo& v1, const ScColorTransformInfo& v2);

struct ScXYZ
{
	double X;
	double Y;
	double Z;
};

struct ScLab
{
	double L;
	double a;
	double b;
};

eColorType colorFormatType(eColorFormat format);
uint       colorFormatNumChannels(eColorFormat format);
uint       colorFormatBytesPerChannel(eColorFormat format);
bool       colorFormatHasAlpha(eColorFormat format);

#endif
