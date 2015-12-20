/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_RayCastInChannel_H
#define Navigation_RayCastInChannel_H

#include "gwnavruntime/channel/channelsectionptr.h"

namespace Kaim
{


enum RayCastInChannelResult
{
	RayCastInChannel_NotInitialised,
	RayCastInChannel_NotProcessed,

	RayCastInChannel_Success,                     ///< The start and end positions are both inside the Channel and all Gates in-between are correctly crossed.									      

	RayCastInChannel_PassedChannelFirstSection,   ///< The start position is inside the Channel and all Gates up to the Channel start one are correctly crossed.
	RayCastInChannel_PassedChannelLastSection,    ///< The start position is inside the Channel and all Gates up to the Channel end one are correctly crossed.
	RayCastInChannel_CollisionDetected_OnLeft,    ///< A collision is detected with Channel borders.
	RayCastInChannel_CollisionDetected_OnRight,   ///< A collision is detected with Channel borders.

	RayCastInChannel_StartIsOutside,              ///< The start position is outside the start section.
	RayCastInChannel_InvalidChannel,              ///< The Channel is not valid (it has less than 2 Gates and thus no section).	
};

class RayCastInChannel
{
public:
	RayCastInChannel();
	
	void Initialize(const Vec2f& startPos2f, const ChannelSectionPtr& startSectionPtr, const Vec2f& maxMove2D);
	
	void Perform();
	
private:

	void PerformThroughNext();
	
	void PerformThroughPrevious();
	
	void PerformAgainstSectionBorders();
	
	void UpdateArrivalPos(const Vec2f& collisionPos);
	
	bool IsProperlyInitialized();
	

public:
	ChannelSectionPtr m_startSectionPtr;
	Vec2f m_startPos2f;
	/// A vector that represents the direction and the maximum distance the ray will travel.
	Vec2f m_maxMove2D;

	ChannelSectionPtr m_arrivalSectionPtr;
	Vec2f m_arrivalPos2f;
	RayCastInChannelResult m_result;
};


} // namespace Kaim

#endif
