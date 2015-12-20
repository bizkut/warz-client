/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_DiskCanGoInChannel_H
#define Navigation_DiskCanGoInChannel_H

#include "gwnavruntime/channel/channelsectionptr.h"

namespace Kaim
{

enum DiskCanGoInChannelResult
{
	DiskCanGoInChannel_NotInitialised,
	DiskCanGoInChannel_NotProcessed,
	DiskCanGoInChannel_Success,                ///< The start and end positions are both inside the Channel and all Gates in-between are correctly crossed.

	DiskCanGoInChannel_InvalidChannel,         ///< The Channel is not valid (it has less than 2 Gates and thus no section).
	DiskCanGoInChannel_PassedChannelEndGate,   ///< The start position is inside the Channel and all Gates up to the Channel end one are correctly crossed.
	DiskCanGoInChannel_PassedChannelStartGate, ///< The start position is inside the Channel and all Gates up to the Channel start one are correctly crossed.
	DiskCanGoInChannel_StartIsOutside,         ///< The start position is outside the start section.
	DiskCanGoInChannel_CollisionDetected       ///< A collision is detected with Channel borders.
};


class DiskCanGoInChannel
{
public:
	DiskCanGoInChannel();

	void Initialize(const Vec2f& startPos2f, const ChannelSectionPtr& startSectionPtr, const Vec2f& destPos2f);
	
	void InitSymmetricRadius(KyFloat32 radius);
	void InitAsymmetricRadius(KyFloat32 radiusLeft, KyFloat32 radiusRight);
	
	// if not set it will be computed in perform
	void SetDestinationSectionPtr(const ChannelSectionPtr& destSectionPtr);
	
	void Perform();

private:
	bool IsProperlyInitializedFromRayCast();
	bool IsProperlyInitializedDestSectionProvided();

public:
	ChannelSectionPtr m_startSectionPtr;
	Vec2f m_startPos2f;
	Vec2f m_destPos2f;
	KyFloat32 m_radiusLeft;
	KyFloat32 m_radiusRight;

	ChannelSectionPtr m_destSectionPtr;
	DiskCanGoInChannelResult m_result;
};


} // namespace Kaim

#endif
