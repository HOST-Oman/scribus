/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef SCIMGDATALOADER_TIFF_H
#define SCIMGDATALOADER_TIFF_H

#include <tiffio.h>
#include "scimgdataloader.h"
//Added by qt3to4:
#include <QList>

class ScImgDataLoader_TIFF : public ScImgDataLoader
{
protected:

	enum PSDColorMode
	{
		CM_BITMAP = 0,
		CM_GRAYSCALE = 1,
		CM_INDEXED = 2,
		CM_RGB = 3,
		CM_CMYK = 4,
		CM_MULTICHANNEL = 7,
		CM_DUOTONE = 8,
		CM_LABCOLOR = 9
	};
	void initSupportedFormatList();
	int  getLayers(const QString& fn, int page);
	bool getImageData(TIFF* tif, RawImage *image, uint widtht, uint heightt, uint size, uint16 m_photometric, uint16 bitspersample, uint16 m_samplesperpixel, bool &bilevel, bool &isCMYK);
	bool getImageData_RGBA(TIFF* tif, RawImage *image, uint widtht, uint heightt, uint size, uint16 bitspersample, uint16 m_samplesperpixel);
	void blendOntoTarget(RawImage *tmp, int layOpa, const QString& layBlend, bool cmyk, bool useMask);
	QString getLayerString(QDataStream & s);
	bool loadChannel( QDataStream & s, const PSDHeader & header, QList<PSDLayer> &layerInfo, uint layer, int channel, int component, RawImage &tmpImg);
	bool loadLayerInfo(QDataStream & s, QList<PSDLayer> &layerInfo);
	bool loadLayerChannels( QDataStream & s, const PSDHeader & header, QList<PSDLayer> &layerInfo, uint layer, bool* firstLayer);

	bool testAlphaChannelAvailability(const QString& fn, int page, bool& hasAlpha);
	void unmultiplyRGBA(RawImage *image);

	int    m_random_table[4096];
	uint16 m_photometric, m_samplesperpixel;

public:
	ScImgDataLoader_TIFF();

//	virtual void preloadAlphaChannel(const QString& fn, int res);
	virtual bool preloadAlphaChannel(const QString& fn, int page, int res, bool& hasAlpha);
	virtual void loadEmbeddedProfile(const QString& fn, int page = 0);
	virtual bool loadPicture(const QString& fn, int page, int res, bool thumbnail);

	virtual bool useRawImage() { return true; }
};

#endif
