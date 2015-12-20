//=========================================================================
//	Module: AutodeskNavProfile.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#if ENABLE_AUTODESK_NAVIGATION

//////////////////////////////////////////////////////////////////////////

#include "gwnavruntime/world/navigationprofile.h"
#include "gwnavruntime/pathfollower/iavoidancefilter.h"
#include "gwnavruntime/navmesh/traverselogic.h"

class AutodeskNavProfileSuperZombie : public Kaim::NavigationProfile<Kaim::DefaultTraverseLogic>
{
public:
	virtual Kaim::Ptr<Kaim::IAvoidanceFilter> GetSharedAvoidanceFilter();

	Kaim::Ptr<Kaim::IAvoidanceFilter> m_sharedAvoidanceFilter;
};

#endif // ENABLE_AUTODESK_NAVIGATION