/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: MUBI - secondary contact: GUAL
#ifndef Navigation_ChannelTrajectoryConfigBlob_H
#define Navigation_ChannelTrajectoryConfigBlob_H

#include "gwnavruntime/pathfollower/radiusprofileblob.h"
#include "gwnavruntime/pathfollower/channeltrajectoryconfig.h"


namespace Kaim
{

class ChannelTrajectoryConfig;


/// Class that configures how the trajectory is computed from the Channel.
class ChannelTrajectoryConfigBlob
{
public:
	ChannelTrajectoryConfigBlob() {}

public:
	SplineConfig m_splineConfig;
	RadiusProfileArrayBlob m_radiusProfileArray;

	KyFloat32 m_recomputationDistance;
	KyFloat32 m_stabilityDistance;
	KyFloat32 m_targetOnSplineDistance;
	KyFloat32 m_maxDistanceToSplinePosition;
	KyUInt32  m_velocityComputationMode;
};

inline void SwapEndianness(Endianness::Target e, ChannelTrajectoryConfigBlob& self)
{
	SwapEndianness(e, self.m_splineConfig);
	SwapEndianness(e, self.m_radiusProfileArray);
	SwapEndianness(e, self.m_recomputationDistance);
	SwapEndianness(e, self.m_stabilityDistance);
	SwapEndianness(e, self.m_targetOnSplineDistance);
	SwapEndianness(e, self.m_maxDistanceToSplinePosition);
	SwapEndianness(e, self.m_velocityComputationMode);
}

class ChannelTrajectoryConfigBlobBuilder : public BaseBlobBuilder<ChannelTrajectoryConfigBlob>
{
	KY_CLASS_WITHOUT_COPY(ChannelTrajectoryConfigBlobBuilder)

public:
	ChannelTrajectoryConfigBlobBuilder(const ChannelTrajectoryConfig* channelTrajectoryConfig)
		: m_channelTrajectoryConfig(channelTrajectoryConfig)
	{}

	virtual void DoBuild();

private:
	const ChannelTrajectoryConfig* m_channelTrajectoryConfig;
};

} // namespace Kaim

#endif // Navigation_ChannelTrajectoryConfig_H
