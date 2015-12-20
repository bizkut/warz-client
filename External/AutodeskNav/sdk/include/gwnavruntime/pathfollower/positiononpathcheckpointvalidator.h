/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_PositionOnPathCheckPointValidator_H
#define Navigation_PositionOnPathCheckPointValidator_H

#include "gwnavruntime/pathfollower/ipositiononpathvalidator.h"


namespace Kaim
{

/// Default implementation of IPositionOnPathValidator interface that use Bot::HasReachedPosition()
/// function and BotConfig::m_checkPointRadius.
class PositionOnPathCheckPointValidator : public IPositionOnPathValidator
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	PositionOnPathCheckPointValidator() {}
	virtual ~PositionOnPathCheckPointValidator() {}

	virtual bool Validate(Bot* bot, const PositionOnPath& positionOnPath);
};

} // namespace Kaim

#endif // Navigation_PositionOnPathCheckPointValidator_H
