//=========================================================================
//	Module: AutodeskNavAvoidanceFilter.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#if ENABLE_AUTODESK_NAVIGATION

//////////////////////////////////////////////////////////////////////////

#include "gwnavruntime/pathfollower/iavoidancefilter.h"

class AutodeskNavAvoidanceFilterSuperZombie : public Kaim::IAvoidanceFilter
{
public:
	AutodeskNavAvoidanceFilterSuperZombie();
	virtual ~AutodeskNavAvoidanceFilterSuperZombie();

	virtual Kaim::AvoidanceFilterResult Filter(const Kaim::Bot& /*referenceBot*/, const Kaim::SpatializedCylinder& obstacle);
};

#endif // ENABLE_AUTODESK_NAVIGATION