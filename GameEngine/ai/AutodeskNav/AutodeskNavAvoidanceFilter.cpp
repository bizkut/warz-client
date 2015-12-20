//=========================================================================
//	Module: AutodeskNavAvoidanceFilter.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if ENABLE_AUTODESK_NAVIGATION

//////////////////////////////////////////////////////////////////////////

#include "AutodeskNavAvoidanceFilter.h"
#include "../../gameobjects/GameObj.h"

#include "gwnavruntime/spatialization/spatializedcylinder.h"
#include "gwnavruntime/world/boxobstacle.h"

//////////////////////////////////////////////////////////////////////////

AutodeskNavAvoidanceFilterSuperZombie::AutodeskNavAvoidanceFilterSuperZombie()
{
}

AutodeskNavAvoidanceFilterSuperZombie::~AutodeskNavAvoidanceFilterSuperZombie()
{
}

Kaim::AvoidanceFilterResult AutodeskNavAvoidanceFilterSuperZombie::Filter(const Kaim::Bot& /*referenceBot*/, const Kaim::SpatializedCylinder& obstacle)
{
	if(obstacle.GetSpatializedPoint().GetObjectType() == Kaim::SpatializedPointObjectType_BoxObstacle)
	{
		Kaim::BoxObstacle* box = (Kaim::BoxObstacle*)(obstacle.GetSpatializedPoint().GetObject());
		GameObject* obj = (GameObject*)box->GetUserData();
		if( obj && obj->isObjType( OBJTYPE_Barricade ) )
		{
			return Kaim::AvoidanceFilter_Ignore;
		}
	}

	return Kaim::AvoidanceFilter_Avoid;
}

#endif // ENABLE_AUTODESK_NAVIGATION