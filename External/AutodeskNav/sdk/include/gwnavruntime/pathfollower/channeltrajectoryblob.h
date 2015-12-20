/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY

#ifndef Navigation_ChannelTrajectory_Blob_H
#define Navigation_ChannelTrajectory_Blob_H

#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/pathfollower/circlearcsplineblob.h"
#include "gwnavruntime/pathfollower/positiononcirclearcsplineblob.h"
#include "gwnavruntime/channel/channelarrayblob.h"

namespace Kaim
{

class ChannelTrajectory;
class Bot;


class ChannelTrajectoryBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, ChannelTrajectoryBlob, 0)

public:
	ChannelTrajectoryBlob() {}

public:
	KyUInt32 m_visualDebugId;
	CircleArcSplineBlob m_spline;

	PositionOnCircleArcSplineBlob m_positionOnSpline;
	PositionOnCircleArcSplineBlob m_targetOnSpline;

	KyUInt32 m_currentChannelIsValid; // true if has a current channelIndex (m_currentChannelIdx, m_currentSectionIdx are valid)
	KyUInt32 m_currentChannelIdx;
	KyUInt32 m_currentSectionIdx;

	BlobArray<char> m_followRadiusProfileName;
};

inline void SwapEndianness(Endianness::Target e, ChannelTrajectoryBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_spline);
	SwapEndianness(e, self.m_positionOnSpline);
	SwapEndianness(e, self.m_targetOnSpline);
	SwapEndianness(e, self.m_currentChannelIsValid);
	SwapEndianness(e, self.m_currentChannelIdx);
	SwapEndianness(e, self.m_currentSectionIdx);
	SwapEndianness(e, self.m_followRadiusProfileName);
}

class ChannelTrajectoryBlobBuilder : public BaseBlobBuilder<ChannelTrajectoryBlob>
{
	KY_CLASS_WITHOUT_COPY(ChannelTrajectoryBlobBuilder)

public:
	ChannelTrajectoryBlobBuilder(const ChannelTrajectory* channelTrajectory, const Bot* bot, KyUInt32 visualDebugId) : m_channelTrajectory(channelTrajectory), m_bot(bot), m_visualDebugId(visualDebugId) {}

	virtual void DoBuild();

private:
	const ChannelTrajectory* m_channelTrajectory;
	const Bot* m_bot;
	KyUInt32 m_visualDebugId;
};

} // namespace Kaim

#endif
