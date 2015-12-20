/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_IPathEventObserver_H
#define Navigation_IPathEventObserver_H

#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/path/patheventlist.h"


namespace Kaim
{
class Bot;
class NavTag;

/// IPathEventListObservers are called when related PathEventList has just been created
/// or is about to be destroyed.
/// It is useful to namely maintain a decoration on a PathEventList, such as parsing the
/// PathEvent array to add check points.
class IPathEventListObserver : public RefCountBase<IPathEventListObserver, MemStat_PathFollowing>
{
public:
	IPathEventListObserver() {}
	virtual ~IPathEventListObserver() {}

	enum FirstIntervalStatus
	{
		FirstIntervalIsNew,                    ///< The first new interval is not an extension of a previous one, it starts from the previous last PathEvent
		FirstIntervalIsExtensionOfPreviousLast ///< The first new interval is an extension of a previous one whose end PathEvent has been moved
	};

	/// This function is called each time a stage of Path validation/PathEventList build is done.
	/// In this function you can:
	/// - set some PathEvent as CheckPointStatus_EventIsACheckPoint,
	/// - modify the PathEvent::m_userSpecific member,
	/// - retrieve the NavTag on intervals between events to do your own stuff (such
	///   as registering the bot in the corresponding game object).
	/// The newly created PathEvents have been pushed back in pathEventList. The index of PathEvent
	/// that starts the first new PathEventInterval is the startIdxOfFirstNewEventInterval function parameter.
	/// If this is the first stage of the validation (say after a new Path has been computed or
	///  NavData changed...), startIdxOfFirstNewEventInterval is 0 (the first PathEvent of the
	///  PathEventList, aka lowerBound, starting the first PathEventInterval). Note that in this
	///  case, parameter firstIntervalStatus value is 'FirstIntervalIsNew'. Note that the first PathEvent
	///  is very likely to be before bot target on path.
	/// If it is not the first validation stage, we may have stopped to a non-definitive PathEvent for
	///  time-slicing purpose and, int his case, the first new PathEventInterval is an extension
	///  the last PathEventInterval computed during previous building sage, which means that the end
	///  of the interval that was of type PathEventType_OnNavMeshPathNode has been moved forward along
	///  the Path. In this case, firstIntervalStatus value is 'FirstIntervalIsExtensionOfPreviousLast. 
	///  If the previous last PathEventInterval has not been extended, firstIntervalStatus value
	///  is FirstIntervalIsNew and startIdxOfFirstNewEventInterval is the index of the previous
	///  last PathEvent
	virtual void OnPathEventListBuildingStageDone(Bot* bot, PathEventList& pathEventList, KyUInt32 startIdxOfFirstNewEventInterval, FirstIntervalStatus firstIntervalStatus) = 0;

	/// Explains the reason the PathEventList is about to be destroyed.
	enum DestructionPurpose
	{
		PathIsBeeingCleared,              ///< The Path is being destroyed itself.
		ReplacePathWithNewPath,           ///< A new Path has been computed, the former Path and the related PathEventList are being destroyed.
		RecomputePathEventListForSamePath ///< A new PathEventList is about to be computed for the same Path.
	};

	/// This function is called each time a non-empty PathEventList is going to be destroyed.
	/// This happens when we need to recompute the PathValidityInterval from scratch: A new
	/// Path has been computed, NavData has changed...
	virtual void OnPathEventListDestroy(Bot* bot,  PathEventList& pathEventList, DestructionPurpose destructionPurpose) = 0;
};


/// This default IPathEventListObserver does not set any PathEvent as CheckPoint.
/// Calling SetCheckPointStatus on a PathEvent of pathEventList allows the user to indicate
/// that the PathEvent must be validated before the Bot can't progress further on its Path.
/// For instance, PositionOnPathCheckPointValidator checks the Bot reached the position of the event using Bot::HasReachedPosition.
class DefaultPathEventListObserver : public IPathEventListObserver
{
public:
	DefaultPathEventListObserver() {}

	virtual ~DefaultPathEventListObserver() {}

	virtual void OnPathEventListDestroy(Kaim::Bot* /*bot*/, Kaim::PathEventList& /*pathEventList*/, DestructionPurpose /*destructionPurpose*/) {}
	
	virtual void OnPathEventListBuildingStageDone(Kaim::Bot* /*bot*/, Kaim::PathEventList& /*pathEventList*/, KyUInt32 /*firstIndexOfNewEvent*/, FirstIntervalStatus /*firstIntervalStatus*/) {}
};

} // namespace Kaim

#endif // Navigation_IPathEventObserver_H
