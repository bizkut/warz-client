/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_InChannelUtils_H
#define Navigation_InChannelUtils_H


#include "gwnavruntime/base/types.h"

namespace Kaim
{

class Vec3f;
class ChannelSectionPtr;

class InChannelUtils
{
public:

	/// Check the provided position is inside the provided section, and if not
	/// try to find the actual channel section in the neighbor ones.
	/// \pre channelSectionPtr must be valid and is intended to be coherent with
	/// the provided position.
	/// \returns KY_SUCCESS if a channel section has been found for the position.
	static KyResult AdjustChannelSection(const Vec3f& position, ChannelSectionPtr& channelSectionPtr);

};

}

#endif
