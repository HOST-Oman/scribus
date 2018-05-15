/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "sclcmscolortransformimpl.h"
#include "sclcmscolormgmtengineimpl.h"

ScLcmsColorTransformImpl::ScLcmsColorTransformImpl(ScColorMgmtEngine& engine, cmsHTRANSFORM lcmsTransform)
                        : ScColorTransformImplBase(engine), m_transformHandle(lcmsTransform)
{

}

ScLcmsColorTransformImpl::~ScLcmsColorTransformImpl()
{
	deleteTransform();
}

bool ScLcmsColorTransformImpl::isNull() const
{
	return (m_transformHandle == nullptr);
}

bool ScLcmsColorTransformImpl::apply(void* input, void* output, uint numElem)
{
	if (m_transformHandle)
	{
		cmsDoTransform(m_transformHandle, input, output, numElem);
		return true;
	}
	return false;
}

bool ScLcmsColorTransformImpl::apply(QByteArray& input, QByteArray& output, uint numElem)
{
	if (m_transformHandle)
	{
		cmsDoTransform(m_transformHandle, input.data(), output.data(), numElem);
		return true;
	}
	return false;
}

void ScLcmsColorTransformImpl::deleteTransform(void)
{
	if (m_transformHandle)
	{
		cmsDeleteTransform(m_transformHandle);
		m_transformHandle = nullptr;
	}
}

