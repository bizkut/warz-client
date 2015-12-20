//=========================================================================
//	Module: AutodeskNavProfile.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if ENABLE_AUTODESK_NAVIGATION

//////////////////////////////////////////////////////////////////////////

#include "AutodeskNavProfile.h"
#include "AutodeskNavAvoidanceFilter.h"

//////////////////////////////////////////////////////////////////////////

Kaim::Ptr<Kaim::IAvoidanceFilter> AutodeskNavProfileSuperZombie::GetSharedAvoidanceFilter()
{
	if (m_sharedAvoidanceFilter == KY_NULL)
		m_sharedAvoidanceFilter = *KY_NEW AutodeskNavAvoidanceFilterSuperZombie;
	return m_sharedAvoidanceFilter;
}


#endif // ENABLE_AUTODESK_NAVIGATION