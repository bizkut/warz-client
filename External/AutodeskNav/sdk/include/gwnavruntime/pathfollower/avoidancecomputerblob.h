/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_AvoidanceComputerBlob_H
#define Navigation_AvoidanceComputerBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"


namespace Kaim
{

class AvoidanceConfig;

class AvoidanceComputerBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, AvoidanceComputerBlob, 1)

public:
	KyUInt32  m_enableSlowingDown;
	KyUInt32  m_enableStop;
	KyUInt32  m_enableForcePassage;
	KyFloat32 m_waitPassageTimeLimit;
	KyFloat32 m_forcePassageTimeLimit;
	KyFloat32 m_minimalTimeToCollision;
	KyFloat32 m_avoidanceAngleSpan;
	KyUInt32  m_avoidanceSampleCount;
	KyFloat32 m_timeToCollisionInfluence;
	KyFloat32 m_desiredVelocityInfluence;
	KyFloat32 m_previousVelocityInfluence;
	KyFloat32 m_safetyDistance;
	KyFloat32 m_stopCollisionTime;
	KyFloat32 m_stopWaitTime;
};

inline void SwapEndianness(Endianness::Target e, AvoidanceComputerBlob& self)
{
	SwapEndianness(e, self.m_enableSlowingDown);
	SwapEndianness(e, self.m_enableStop);
	SwapEndianness(e, self.m_enableForcePassage);
	SwapEndianness(e, self.m_waitPassageTimeLimit);
	SwapEndianness(e, self.m_forcePassageTimeLimit);
	SwapEndianness(e, self.m_minimalTimeToCollision);

	SwapEndianness(e, self.m_avoidanceAngleSpan);
	SwapEndianness(e, self.m_avoidanceSampleCount);
	SwapEndianness(e, self.m_timeToCollisionInfluence);
	SwapEndianness(e, self.m_desiredVelocityInfluence);
	SwapEndianness(e, self.m_previousVelocityInfluence);
	SwapEndianness(e, self.m_safetyDistance);

	SwapEndianness(e, self.m_stopCollisionTime);
	SwapEndianness(e, self.m_stopWaitTime);
}

class AvoidanceComputerBlobBuilder: public BaseBlobBuilder<AvoidanceComputerBlob>
{
	KY_CLASS_WITHOUT_COPY(AvoidanceComputerBlobBuilder)

public:
	AvoidanceComputerBlobBuilder(const AvoidanceConfig& avoidanceComputer) : m_avoidanceComputerConfig(avoidanceComputer) {}
	~AvoidanceComputerBlobBuilder() {}

private:
	virtual void DoBuild();

	const AvoidanceConfig& m_avoidanceComputerConfig;
};

} // namespace Kaim

#endif // Navigation_AvoidanceComputerBlob_H
