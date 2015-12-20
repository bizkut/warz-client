/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: GUAL
#ifndef Navigation_BasePathProgressComputer_H
#define Navigation_BasePathProgressComputer_H

#include "gwnavruntime/world/progressonpathstatus.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"


namespace Kaim
{

class Bot;
class IPathEventListObserver;
class IPositionOnPathValidator;

/// The abstract base class for PathProgressComputer classes.
/// PathProgressComputer maintains the progress of a Bot along a Path.
/// PathProgressComputer does NOT compute the output trajectory (spline or velocity...), the ITrajectory class has this responsibility.
/// PathProgressComputer is stateless and shared among bots that use the same NavigationProfile.
/// There is a BasePathProgressComputer abstract base class and a PathProgressComputer<TraverseLogic> concrete class so that the Bot can call
/// pathProgressComputer->Update() without being templated by TraverseLogic.
/// Users are not supposed to implement their own implementation of BasePathProgressComputer.
class BasePathProgressComputer: public RefCountBase<BasePathProgressComputer, MemStat_PathFollowing>
{
	KY_CLASS_WITHOUT_COPY(BasePathProgressComputer)

public:
	// ---------------------------------- Public Member Functions ----------------------------------
	BasePathProgressComputer();
	virtual ~BasePathProgressComputer();

	/// Sets the SetPathEventListObserver
	void SetPathEventListObserver(const Ptr<IPathEventListObserver>& m_pathEventListObserver);
	Ptr<IPathEventListObserver> GetPathEventListObserver() { return m_pathEventListObserver; }

	/// Sets the IPositionOnPathValidator to be used to validate target on path check points.
	void SetPositionOnPathCheckPointValidator(const Ptr<IPositionOnPathValidator>& positionOnPathCheckPointValidator);
	Ptr<IPositionOnPathValidator> GetPositionOnPathCheckPointValidator() { return m_positionOnPathCheckPointValidator; }

	void CleanPathEventListOnNewPath(Bot* bot);


	// ---------------------------------- Pure Virtual Functions ----------------------------------
	virtual void Update(Bot* bot, KyFloat32 simulationTimeInSeconds) = 0;

protected:
	void ComputeProgressOnPath(Bot* bot);
	bool CanProgressAfterEvent(Bot* bot); // called by ComputeProgressOnPath
	void UpdateProgressOnPathStatus(Bot* bot); // called by ComputeProgressOnPath
	void MoveForwardAfterCrossedBisectorPlanes(Bot* bot); // called by ComputeProgressOnPath, should not be called if at upperBound or lastPathNode
	void SnapProgressToNodeOrEvent(Bot* bot);

	void ComputePositionInChannel(Bot* bot);

protected:
	Ptr<IPathEventListObserver>   m_pathEventListObserver;
	Ptr<IPositionOnPathValidator> m_positionOnPathCheckPointValidator;
};


} // namespace Kaim

#endif // Navigation_IPathFollower_H
