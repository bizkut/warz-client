/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_PathProgressComputer_H
#define Navigation_PathProgressComputer_H

#include "gwnavruntime/pathfollower/basepathprogresscomputer.h"
#include "gwnavruntime/world/bot.h"
#include "gwnavruntime/basesystem/iperfmarkerinterface.h"

namespace Kaim
{


/// PathProgressComputer maintains the progress of a Bot along a Path.
/// PathProgressComputer does NOT compute the output trajectory (spline or velocity...), the ITrajectory class has this responsibility.
/// PathProgressComputer is stateless and shared among bots that use the same NavigationProfile.
/// There is a BasePathProgressComputer abstract base class and a PathProgressComputer<TraverseLogic> concrete class so that the Bot can call
/// pathProgressComputer->Update() without being templated by TraverseLogic.
/// Users are not supposed to implement their own implementation of BasePathProgressComputer.
template <class TraverseLogic>
class PathProgressComputer: public BasePathProgressComputer
{
	KY_CLASS_WITHOUT_COPY(PathProgressComputer)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	PathProgressComputer() {}

	virtual ~PathProgressComputer() {}

	virtual void Update(Bot* bot, KyFloat32 simulationTimeInSeconds);

private:
	void UpdatePathEventList(Bot* bot);
};

} // namespace Kaim

#include "gwnavruntime/pathfollower/pathprogresscomputer.inl"

#endif // Navigation_PathFollower_H
