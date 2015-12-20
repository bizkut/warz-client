/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_IPositionOnPathValidator_H
#define Navigation_IPositionOnPathValidator_H

#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{
class Bot;
class PositionOnPath;


/// Interface class for PositionOnPath validation classes.
class IPositionOnPathValidator : public RefCountBase<IPositionOnPathValidator, MemStat_PathFollowing>
{
	KY_CLASS_WITHOUT_COPY(IPositionOnPathValidator)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	IPositionOnPathValidator() {}
	virtual ~IPositionOnPathValidator() {}

	virtual bool Validate(Bot* bot, const PositionOnPath& positionOnPath) = 0;
};

} // namespace Kaim

#endif // Navigation_IPositionOnPathValidator_H
