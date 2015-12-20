/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LAPA

#ifndef Navigation_LivePath_H
#define Navigation_LivePath_H

#include "gwnavruntime/path/path.h"
#include "gwnavruntime/path/pathvalidityinterval.h"

namespace Kaim
{

class Path;
class ChannelArray;
class Bot;
class IPathFinderQuery;

class LivePathOnPathCalculated : public IOnDone
{
public:
	LivePathOnPathCalculated(LivePath* livePath) : m_livePath(livePath) {}
	virtual ~LivePathOnPathCalculated() {}
	virtual void OnDone();
public:
	LivePath* m_livePath;
	Ptr<IOnDone> m_preSetOnDone;
};

enum PathFinderQueryStatus
{
	PathFinderQueryStatus_Idle,
	PathFinderQueryStatus_Computing,
};

/// Enumerates the possible source for a new Path.
enum PathSource
{
	PathSource_NoPath,                ///< The Path has not been set yet or has been Cleared.
	PathSource_PathFinderQueryResult, ///< The Path has been computed by a IPathFinderQuery.
	PathSource_UserInjected,          ///< The Path has been injected by the user.
};

enum UpperBoundType
{
	PathLastNode = 0,
	ValidityUpperBound,
	ValidityTemporaryUpperBound
};


/// This class aggregates a Path and all runtime related stuff namely the
/// PathValidityInterval. It is in charge of:
/// - computing a new Path (using its IPathFinderQuery) and switching the Path
///   when a Path computation is done (you can alternately directly inject a
///   Path computed outside),
/// - maintaining runtime information up to date with regards to the NavData
///   streaming (either due to explicit streaming or to dynamic NavMesh).
class LivePath
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Path)
	KY_CLASS_WITHOUT_COPY(LivePath)
public:
	// ---------------------------------- Main API Functions ----------------------------------

	LivePath();
	~LivePath();

	void Clear();

	/// Returns true if a path computation has been launched with the PathFinderQuery and
	/// the followed path has not been replaced by the result path yet.
	bool IsComputingNewPath() const;

	/// Returns whether or not a new Path has been set and is ready to be followed.
	bool NeedToReplaceCurrentPathWithNewPath() const;

	// ---------------------------------- Getters ----------------------------------

	Path*                   GetFollowedPath()       const;
	PathSource              GetFollowedPathSource() const;
	PathValidityStatus      GetPathValidityStatus() const;
	const PathEventList&    GetPathEventList()      const;
	const IPathFinderQuery* GetPathFinderQuery()    const;
	ChannelArray*           GetChannelArray()       const;

	PathEventList&    GetPathEventList();
	UpperBoundType    GetUpperBoundType() const;

	// ---------------------------------- Path Following Control ----------------------------------

	/// Call this method to force an update of the PathValidityInterval.
	/// This is already called through Bot when NavData did change nearby the Path.
	void ForceValidityIntervalRecompute();

public://Internal
	/// Calls SetPath() to inject newPath and set #m_newPathSource to
	/// #PathSource_UserInjected.
	/// Called internally by the Bot, you should not need to call this function.
	void InjectUserPath(Path* newPath);

	/// Calls SetPath() to inject newPath and set #m_newPathSource to
	/// Called internally by the Bot or by the LivePath, you should not need to call this function.
	void InjectPathFromPathFinderQueryOnDone(Path* newPath);

	template<class TraverseLogic>
	KyResult ReStartIntervalComputation(void* traverseLogicUserData, const PositionOnPath& targetOnPath,
		KyFloat32 minDistanceFromTargetOnPathBackward, KyFloat32 minDistanceFromTargetOnPathFrowaard);

	template<class TraverseLogic>
	KyResult ValidateForward(void* traverseLogicUserData, KyFloat32 minimumDistanceAdvancement,
		PathValidityInterval::FirstNewIntervalExtendedStatus* firstIntervalStatus);

	void* GetBotTraverseLogicUserData() const;
	void SetBotTraverseLogicUserData(void* traverseLogicUserData);

	/// This method is called to properly switch between the currently followed Path and the new one freshly set.
	/// This is called in Bot::ReplaceFollowedPathWithNewPath() when a new Path has been detected 
	/// and the path follower is ready for Path switch (done in BasePathProgressComputer::Update)
	void ReplaceCurrentPathWithNewPath();

private:
	void SetNewPath(Path* newPath, PathSource pathType);
	friend class Bot;
	void ClearFollowedPath();
	void ComputeNewPathAsync(IPathFinderQuery* pathFinderQuery);
	void CancelAsynPathComputation();

public: // internal
	Bot* m_bot;

	Ptr<LivePathOnPathCalculated> m_onPathCalculated;

	Ptr<Path> m_followedPath;
	Ptr<Path> m_newPath; ///< The new Path that has just been set.

	PathSource m_pathType; ///< The source of the current Path.
	PathSource m_newPathSource; ///< The source of the new Path that has just been set.
	PathFinderQueryStatus m_pathFinderQueryStatus;

	PathValidityInterval m_validityInterval;

	bool m_needToSendLivePathBlob; ///< (VisualDebug) Avoids sending all the active KyGuid each frame. Accessed by the Bot.
};

}

#include "gwnavruntime/path/livepath.inl"


#endif
