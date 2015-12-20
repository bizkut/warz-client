/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: GUAL
#ifndef Navigation_ColliderCollectorConfig_H
#define Navigation_ColliderCollectorConfig_H


#include "gwnavruntime/base/memory.h"


namespace Kaim
{

/// Class that configures how the Bot collects colliders (other bots, obstacles) around
class ColliderCollectorConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	ColliderCollectorConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_colliderCollectorRadius               = 15.0f;
		m_colliderCollectorHalfHeight           = 5.0f;
		m_colliderCollectorFramesBetweenUpdates = 30;
	}

	bool operator==(const ColliderCollectorConfig& other) const
	{
		return m_colliderCollectorRadius                == other.m_colliderCollectorRadius
			&& m_colliderCollectorHalfHeight            == other.m_colliderCollectorHalfHeight
			&& m_colliderCollectorFramesBetweenUpdates  == other.m_colliderCollectorFramesBetweenUpdates;
	}

	bool operator!=(const ColliderCollectorConfig& other) const
	{
		return !operator==(other);
	}

public:
	// ---------------------------------- Potential Collider Collection Parameters ----------------------------------

	/// Defines the radius around the Bot in which potential colliders will be collected. In meters.
	/// \defaultvalue 15.0f
	KyFloat32 m_colliderCollectorRadius;

	/// Defines the height above and below the Bot altitude in which potential colliders will be collected. In meters.
	/// \defaultvalue 5.0f
	KyFloat32 m_colliderCollectorHalfHeight;

	/// Defines the number of frames between two consecutive collection of potential colliders.
	/// \defaultvalue 30
	KyUInt32 m_colliderCollectorFramesBetweenUpdates;
};

inline void SwapEndianness(Endianness::Target e, ColliderCollectorConfig& self)
{
	SwapEndianness(e, self.m_colliderCollectorRadius);
	SwapEndianness(e, self.m_colliderCollectorHalfHeight);
	SwapEndianness(e, self.m_colliderCollectorFramesBetweenUpdates);
}


} // namespace Kaim

#endif // Navigation_ColliderCollectorConfig_H
