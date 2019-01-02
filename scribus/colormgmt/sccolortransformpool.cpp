/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QSharedPointer>
#include "sccolormgmtengine.h"
#include "sccolormgmtstructs.h"
#include "sccolortransformpool.h"

ScColorTransformPool::ScColorTransformPool(int engineID) : m_engineID(engineID)
{

}

void ScColorTransformPool::clear()
{
	m_pool.clear();
}

void ScColorTransformPool::addTransform(const ScColorTransform& transform, bool force)
{
	// Check engine ID. If different, transform was created by another engine
	//  and we MUST NOT add it to the transform pool
	if (m_engineID != transform.engine().engineID())
		return;
	ScColorTransform trans;
	if (!force)
		trans = findTransform(transform.transformInfo());
	if (trans.isNull())
		m_pool.append(transform.weakRef());
}

void ScColorTransformPool::removeTransform(const ScColorTransform& transform)
{
	if (m_engineID != transform.engine().engineID())
		return;
	m_pool.removeOne(transform.strongRef());
}

void ScColorTransformPool::removeTransform(const ScColorTransformInfo& info)
{
	QList< QWeakPointer<ScColorTransformData> >::Iterator it = m_pool.begin();
	while (it != m_pool.end())
	{
		QSharedPointer<ScColorTransformData> ref = it->toStrongRef();
		if ((info == ref->transformInfo()) || ref.isNull())
		{
			it = m_pool.erase(it);
			continue;
		}
		++it;
	}
}

ScColorTransform ScColorTransformPool::findTransform(const ScColorTransformInfo& info) const
{
	ScColorTransform transform(nullptr);
	QList< QWeakPointer<ScColorTransformData> >::ConstIterator it = m_pool.begin();
	for ( ; it != m_pool.end(); ++it)
	{
		QSharedPointer<ScColorTransformData> ref = it->toStrongRef();
		if (!ref.isNull())
		{
			if (info == ref->transformInfo())
			{
				transform = ScColorTransform(ref);
				break;
			}
		}
	}
	return transform;
}
