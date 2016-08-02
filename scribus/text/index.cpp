/*
 *  index.cpp
 *  ScribusProject
 *
 *  Created by Andreas Vox on 30.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <QDebug>

#include "index.h"



// find run with runStart <= pos < runEnd
uint RunIndex::search(int pos) const
{
	std::vector<uint>::const_iterator it = std::upper_bound(runEnds.cbegin(), runEnds.cend(), pos);
	return it - runEnds.begin();
}


uint RunIndex::insert(int pos)
{
	uint i = search(pos);
	
	if (i >= runEnds.size())
	{
		runEnds.push_back(pos);
		return runEnds.size() - 1;
	}
	else
	{
		runEnds.insert(runEnds.begin() + i, pos);
		return i;
	}
}


void RunIndex::remove (uint idx)
{
	assert ( idx < runEnds.size() );

	runEnds.erase(runEnds.begin() + idx);
}


void RunIndex::adjust(int pos, int delta)
{
	uint idx = search(pos);
	for (uint i = idx; i < runEnds.size(); ++i)
	{
		runEnds[i] += delta;
	}
}


