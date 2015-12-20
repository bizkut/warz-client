/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_TrajectoryBlob_H
#define Navigation_TrajectoryBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{

class ITrajectory;
class Bot;


//////////////////////////////////////////////////////////////////////////
// ITrajectory blob
//////////////////////////////////////////////////////////////////////////
class TrajectoryBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, TrajectoryBlob, 0)

public:
	// Actual ITrajectory members
	Vec3f m_trajectoryVelocity;
	KyFloat32 m_turningRadius;
	KyUInt32 m_visualDebugId;
};

inline void SwapEndianness(Endianness::Target e, TrajectoryBlob& self)
{
	SwapEndianness(e, self.m_trajectoryVelocity);
	SwapEndianness(e, self.m_turningRadius);
	SwapEndianness(e, self.m_visualDebugId);
}


class TrajectoryBlobBuilder: public BaseBlobBuilder<TrajectoryBlob>
{
	KY_CLASS_WITHOUT_COPY(TrajectoryBlobBuilder)

public:
	TrajectoryBlobBuilder(const ITrajectory* trajectory, const Bot* bot): m_trajectory(trajectory), m_bot(bot) {}
	~TrajectoryBlobBuilder() {}

private:
	virtual void DoBuild();

	const ITrajectory* m_trajectory;
	const Bot*         m_bot;
};

} // namespace Kaim

#endif // Navigation_TrajectoryBlob_H
