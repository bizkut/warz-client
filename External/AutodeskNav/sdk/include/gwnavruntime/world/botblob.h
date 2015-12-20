/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_BotBlob_H
#define Navigation_BotBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/world/botconfig.h"
#include "gwnavruntime/pathfollower/avoidancecomputerblob.h"
#include "gwnavruntime/pathfollower/channeltrajectoryconfigblob.h"
#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{

class Bot;


//////////////////////////////////////////////////////////////////////////
//
// Path finder config
//
//////////////////////////////////////////////////////////////////////////

class PathFinderConfigBlob
{
public:
	KyFloat32 m_fromOutsideNavMeshDistance;
	KyFloat32 m_toOutsideNavMeshDistance;
	KyFloat32 m_propagationBoxExtent;
	KyUInt32 m_abstractGraphTraversalMode; // cast this as PathFinderAbstractGraphTraversalMode
};

class PathFinderConfigBlobBuilder : public BaseBlobBuilder<PathFinderConfigBlob>
{
	KY_CLASS_WITHOUT_COPY(PathFinderConfigBlobBuilder)

public:
	PathFinderConfigBlobBuilder(const PathFinderConfig& config) : m_config(config) {}

	virtual void DoBuild();

private:
	const PathFinderConfig& m_config;
};

inline void SwapEndianness(Endianness::Target e, PathFinderConfigBlob& self)
{
	SwapEndianness(e, self.m_fromOutsideNavMeshDistance);
	SwapEndianness(e, self.m_toOutsideNavMeshDistance);
	SwapEndianness(e, self.m_propagationBoxExtent);
	SwapEndianness(e, self.m_abstractGraphTraversalMode);
}



//////////////////////////////////////////////////////////////////////////
//
// Bot config
//
//////////////////////////////////////////////////////////////////////////

class BotConfigBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, BotConfigBlob, 2)

public:
	KyUInt32 m_visualDebugId;
	KyUInt32 m_databaseIndex;
	KyFloat32 m_radius;
	KyFloat32 m_height;

	// ------------------- PathFind Config -------------------
	PathFinderConfigBlob m_pathFinderConfig;
	ChannelComputerConfig m_channelComputerConfig;

	// ------------------- PathProgress Config -------------------
	PathProgressConfig m_pathProgressConfig;

	// ------------------- Trajectory Config -------------------

	// Trajectory
	KyUInt32 m_trajectoryMode; // Cast this to TrajectoryMode
	ShortcutTrajectoryConfig m_shortcutTrajectoryConfig;
	ChannelTrajectoryConfigBlob m_channelTrajectoryConfig;
	KyFloat32 m_trajectoryMinimalMoveDistance; ///< 

	// Avoidance Config
	KyUInt32 m_enableAvoidance;
	ColliderCollectorConfig m_colliderCollectorConfig;
	AvoidanceComputerBlob m_avoidanceConfig;

	KyFloat32 m_maxDesiredLinearSpeed;
};

inline void SwapEndianness(Endianness::Target e, BotConfigBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_databaseIndex);
	SwapEndianness(e, self.m_radius);
	SwapEndianness(e, self.m_height);
	SwapEndianness(e, self.m_pathFinderConfig);
	SwapEndianness(e, self.m_channelComputerConfig);
	SwapEndianness(e, self.m_pathProgressConfig);
	SwapEndianness(e, self.m_trajectoryMode);
	SwapEndianness(e, self.m_shortcutTrajectoryConfig);
	SwapEndianness(e, self.m_channelTrajectoryConfig);
	SwapEndianness(e, self.m_trajectoryMinimalMoveDistance);
	SwapEndianness(e, self.m_enableAvoidance);
	SwapEndianness(e, self.m_colliderCollectorConfig);
	SwapEndianness(e, self.m_avoidanceConfig);
	SwapEndianness(e, self.m_maxDesiredLinearSpeed);
}

class BotConfigBlobBuilder : public BaseBlobBuilder<BotConfigBlob>
{
	KY_CLASS_WITHOUT_COPY(BotConfigBlobBuilder)

public:
	BotConfigBlobBuilder(const BotConfig& botConfig, KyUInt32 visualDebugId, KyUInt32 databaseIndex)
		: m_botConfig(botConfig), m_visualDebugId(visualDebugId), m_databaseIndex(databaseIndex) {}

	virtual void DoBuild();

private:
	const BotConfig& m_botConfig;
	KyUInt32 m_visualDebugId;
	KyUInt32 m_databaseIndex;
};



//////////////////////////////////////////////////////////////////////////
//
// Bot direction
//
//////////////////////////////////////////////////////////////////////////

class FrontDirectionBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, FrontDirectionBlob, 0)

public:
	KyUInt32 m_visualDebugId;
	Vec3f m_frontDirection;
};

inline void SwapEndianness(Endianness::Target e, FrontDirectionBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_frontDirection);
}

class FrontDirectionBlobBuilder : public BaseBlobBuilder<FrontDirectionBlob>
{
	KY_CLASS_WITHOUT_COPY(FrontDirectionBlobBuilder)

public:
	FrontDirectionBlobBuilder(KyUInt32 visualDebugId, const Vec3f* frontDirection)
		: m_visualDebugId(visualDebugId)
		, m_frontDirection(frontDirection)
	{}

	virtual void DoBuild();

private:
	KyUInt32 m_visualDebugId;
	const Vec3f* m_frontDirection;
};



} // namespace Kaim

#endif // Navigation_BotBlob_H
