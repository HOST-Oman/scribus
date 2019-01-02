/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef SCLCMS2COLORTRANSFORMIMPL_H
#define SCLCMS2COLORTRANSFORMIMPL_H

#include "lcms2.h"
#include "sccolormgmtimplelem.h"
#include "sccolortransformdata.h"

class ScLcms2ColorTransformImpl : public ScColorTransformImplBase
{
	friend class ScLcms2ColorMgmtEngineImpl;

public:
	ScLcms2ColorTransformImpl(ScColorMgmtEngine& engine, cmsHTRANSFORM lcmsTransform);
	virtual ~ScLcms2ColorTransformImpl();

	virtual bool isNull() const;

	virtual bool apply(void* input, void* output, uint numElem);
	virtual bool apply(QByteArray& input, QByteArray& output, uint numElem);

protected:
	cmsHTRANSFORM m_transformHandle;

	void deleteTransform();
};

#endif
