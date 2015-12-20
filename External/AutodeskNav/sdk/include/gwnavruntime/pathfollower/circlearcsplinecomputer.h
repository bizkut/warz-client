/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_CircleArcSplineComputer_H
#define Navigation_CircleArcSplineComputer_H


#include "gwnavruntime/pathfollower/circlearcspline.h"
#include "gwnavruntime/pathfollower/circlearcsplinedisplay.h"
#include "gwnavruntime/pathfollower/circlearcsplinebubbledata.h"
#include "gwnavruntime/pathfollower/channeltrajectoryconfig.h"
#include "gwnavruntime/pathfollower/radiusprofile.h"

#include "gwnavruntime/channel/channelsectionptr.h"


namespace Kaim
{

/// Enumerates the CircleArcSpline computation results.
enum CircleArcSplineComputationResult
{
	CircleArcSplineComputation_NotComputed = 0, ///< The spline has not yet been computed.

	CircleArcSplineComputation_Success     = 1, ///< The spline has been successfully computed, without any warning.

	CircleArcSplineComputation_Warning_RestrictedRadius             = 1 <<  4, ///< A spline has been found, but it contains at least one turn with a radius that is not in the corresponding RadiusProfile.

	CircleArcSplineComputation_Failed_InvalidInputs                 = 1 <<  8, ///< Inputs are invalid: startSection or endSection are invalid or not in the same Channel.
	CircleArcSplineComputation_Failed_BubbleArrayBuildError         = 1 <<  9, ///< BubbleArray could not be build from Channel and start constraints.
	CircleArcSplineComputation_Failed_StringPullerError             = 1 << 10, ///< The string puller failed.
	CircleArcSplineComputation_Failed_InvalidStringPulledBubbleList = 1 << 11, ///< The string puller result is invalid (it returned an empty list).
	CircleArcSplineComputation_Failed_TurnListConstructionError     = 1 << 12, ///< The string puller result could not be converted into turn list (an edge or circle arc quits the Channel).
	CircleArcSplineComputation_Failed_TurnListRadiusAdjustmentError = 1 << 13, ///< An error occurred while computing optimized turns.
	CircleArcSplineComputation_Failed_CircleArcSplineBuildError     = 1 << 14, ///< Turn list could not be converted into CircleArcSpline (a turn is not correctly defined).
};

KY_INLINE bool IsSuccessCase(CircleArcSplineComputationResult result) { return (result == CircleArcSplineComputation_Success); }
KY_INLINE bool IsWarningCase(CircleArcSplineComputationResult result) { return (result == CircleArcSplineComputation_Warning_RestrictedRadius); }
KY_INLINE bool IsSuccessOrWarningCase(CircleArcSplineComputationResult result) { return (IsSuccessCase(result) || IsWarningCase(result)); }
KY_INLINE bool IsErrorCase(CircleArcSplineComputationResult result) { return (!IsSuccessCase(result) && !IsWarningCase(result)); }


// Internal: Class that configure a case tracker that save CircleArcSplineComputer
// inputs into a blob under specified conditions. This class is useful to track
// down mishandled edge cases.
class SplineInputBlobDumpConfig
{
public:
	SplineInputBlobDumpConfig()
		: m_trackedResultFlags(0)
		, m_maxSavedCase(25)
	{}

	bool DoTrackAnyCase()                                          const { return (m_trackedResultFlags != 0); }
	bool IsATrackedResult(CircleArcSplineComputationResult result) const { return ((m_trackedResultFlags & result) != 0); }

	static const KyUInt32 DumpAllFailureCases = 
		CircleArcSplineComputation_Failed_InvalidInputs                 |
		CircleArcSplineComputation_Failed_BubbleArrayBuildError         |
		CircleArcSplineComputation_Failed_StringPullerError             |
		CircleArcSplineComputation_Failed_InvalidStringPulledBubbleList |
		CircleArcSplineComputation_Failed_TurnListConstructionError     |
		CircleArcSplineComputation_Failed_TurnListRadiusAdjustmentError |
		CircleArcSplineComputation_Failed_CircleArcSplineBuildError     ;

public:
	KyUInt32 m_trackedResultFlags;
	KyUInt32 m_maxSavedCase;
};

/// Class aggregating a CircleArcSpline and the corresponding computation result. Each
/// RadiusProfile provided to the CircleArcSplineComputer will be associated to an
/// instance of this class.
class RadiusProfileCircleArcSplineComputerOutput
{
public:
	~RadiusProfileCircleArcSplineComputerOutput() { Clear(); }
	void Clear()
	{
		m_spline.Clear();
		m_result = CircleArcSplineComputation_NotComputed;
	}

	CircleArcSpline                  m_spline;
	CircleArcSplineComputationResult m_result;
};


// Forward declare CircleArcSplineComputer to make it friend of CircleArcSplineTurn
// and thus allow direct member manipulation.
class CircleArcSplineComputer;

// Internal: Class aggregating all useful information about a turn when building
// a CircleArcSpline.
class CircleArcSplineTurn
{
public:
	CircleArcSplineTurn()
	{
		m_bubbleData.SetBubble(&m_bubble, CircleArcSplineBubbleData::UndefinedBubbleType);
		m_bubble.SetUserData(&m_bubbleData);
	}

	CircleArcSplineTurn(const Bubble& bubble, const CircleArcSplineBubbleData& bubbleData)
		: m_bubble(bubble)
		, m_bubbleData(bubbleData)
	{
		m_bubbleData.SetBubble(&m_bubble, bubbleData.GetBubbleType());
		m_bubble.SetUserData(&m_bubbleData);
	}

	CircleArcSplineTurn(const CircleArcSplineTurn& other) { *this = other; }

	void operator=(const CircleArcSplineTurn& other)
	{
		m_bubble = other.m_bubble;
		m_bubbleData = other.m_bubbleData;

		m_bubbleData.SetBubble(&m_bubble, m_bubbleData.GetBubbleType());
		m_bubble.SetUserData(&m_bubbleData);
	}

	const Bubble&                    GetBubble()     const { return m_bubble;     }
	const CircleArcSplineBubbleData& GetBubbleData() const { return m_bubbleData; }

private:
	friend class CircleArcSplineComputer;
	Bubble m_bubble;
	CircleArcSplineBubbleData m_bubbleData;
};


typedef SharedPoolList<CircleArcSplineTurn> TurnList;


/// Class used to compute a CircleArcSpline in a Channel.
class CircleArcSplineComputer
{
public:
	CircleArcSplineComputer();
	~CircleArcSplineComputer() { Clear(); }

	void Initialize(const SplineConfig& splineConfig,
		const Vec3f& startPosition, const ChannelSectionPtr& startSection,
		const Vec3f& endPosition, const ChannelSectionPtr& endSection,
		const RadiusProfileArray& radiusProfileArray = RadiusProfileArray());

	void Clear();
	void ClearInternals();

	void SetStartConstraint(const BubbleFunnelExtremityConstraint& constraint) { m_startConstraint = constraint; }

	CircleArcSplineComputationResult Compute();

	KyUInt32 GetOutputCount() const { return m_profileOutputs.GetCount(); }
	const RadiusProfileCircleArcSplineComputerOutput& GetOutput(KyUInt32 index) const { return m_profileOutputs[index]; }


public: // internal: these are public only to be accessible from unit tests
	enum NextChannelTurnRangeSearchResult
	{
		NextChannelTurnFound,
		NoChannelTurnUpToEnd
	};


	// ---- First steps: computed once for all RadiusProfiles

	KyResult CheckInputValidity();
	KyResult ComputeBubbleArray(BubbleArray& bubbleArray);
	KyResult ComputeStringPulledBubbleList(const BubbleArray& bubbleArray, StringPulledBubbleList& stringPulledBubbleList);


	// ---- Last steps: computed for each RadiusProfile

	// Compute all the last steps: Conversion to turn list, radius adjustment,
	// simplification & conversion into CircleArcSpline.
	void ComputeProfile(const StringPulledBubbleList& stringPulledBubbleList, RadiusProfileCircleArcSplineComputerOutput& output, const RadiusProfile& radiusProfile);

	// Convert the string pulled Bubble list into a TurnList.
	CircleArcSplineComputationResult ConvertStringPulledBubbleListIntoTurnList(const StringPulledBubbleList& stringPulledBubbleList, TurnList& turnList);

	// Enlarge turns to fit preferred turn radii and respecting neighbor turns constraints.
	CircleArcSplineComputationResult AdjustTurnListToRadiusProfile(const RadiusProfile& radiusProfile, TurnList& turnList);

	// Merge close turns together.
	CircleArcSplineComputationResult SimplifyTurnList(const RadiusProfile& radiusProfile, TurnList& turnList);

	// Convert the TurnList into a CircleArcSpline.
	CircleArcSplineComputationResult ConvertTurnListIntoCircleArcSpline(const TurnList& turnList, CircleArcSpline& m_circleArcSpline);


	//////////////////////////////////////////////////////////////////////////
	// BubbleArray computation

	NextChannelTurnRangeSearchResult GetNextChannelTurnRange(const Channel* channel, KyUInt32 currentIdx, KyUInt32 lastIdx, KyUInt32& nextTurnStartIdx, KyUInt32& nextTurnEndIdx);
	void UpdateBubbleMaxRadius(const Vec3f& center, const Vec3f& constrainingPosition, const Gate& constrainingGate, KyFloat32& maxBubbleRadius) const;
	void CapBubblesRadiiAccordinglyToStartAndEndPositions(const Channel* channel, KyUInt32 gateIdx, KyFloat32& leftCornerMaxRadius, KyFloat32& rightCornerMaxRadius) const;
	void AddCornerBubble(const Channel* channel, const Vec3f& channelCorner, KyFloat32 bubbleRadius, RotationDirection rotationDirection, KyUInt32 firstGateIndex, KyUInt32 lastGateIndex, BubbleArray& bubbleArray);
	KyResult AddIntermediaryGateBubbles(const Channel* channel, KyUInt32 gateIdx, KyFloat32 minGateWidth, BubbleArray& bubbleArray);
	KyResult AddNextChannelTurnBubbles(const Channel* channel, KyUInt32 nextTurnStartIdx, KyUInt32 nextTurnEndIdx, BubbleArray& bubbleArray);
	void AdjustBubbleRadiusRelativelyToNearbyBubble(const Bubble* nearbyBubble, Bubble& bubble);
	void EnsureBubbleArrayIsStringPullable(BubbleArray& bubbleArray);


	//////////////////////////////////////////////////////////////////////////
	// TurnList atomic modifications

	// Check the candidate Bubble is valid if used for the specified turn. If so, it then:
	// 1. replace the turn Bubble by the candidate one
	// 2. update turn entry and exit positions
	// 3. update the neighbor turns:
	//      - remove the ones that are no more in touch with the pulled string
	//      - update the previous turn exit position
	//      - update the next turn entry position
	KyResult ChangeTurnBubble(TurnList& turnList, TurnList::Iterator turnIt, const Bubble& candidate, Vec3f& curPosition, KyUInt32& curSectionIdx);

	// Remove turns that are just brushed past by the pulled string.
	KyResult RemoveExtraneousTurnsFromList(TurnList& turnList);


	//////////////////////////////////////////////////////////////////////////
	// Utility internal methods

	void InitializeDisplayList(ScopedDisplayList& displayList, const char* groupName, const char* listName) const;
	void RenderBubbleData(const CircleArcSplineBubbleData* bubbleData, const char* groupName) const;
	void RenderBubbleArray(const BubbleArray& bubbleArray) const;
	void RenderTurnList(const TurnList& turnList, const char* listBaseName, const StringPulledBubbleListDisplayConfig& displayConfig) const;
	void RenderResult(ScopedDisplayList& displayList, CircleArcSplineComputationResult result) const;


public: // Internal - intermediary computation result accessors for unit test
	const TurnList& GetTurnList() const { return m_turnList; }


public: // Internal: We let parameters accessible for validation unit tests
	// Inputs
	SplineConfig m_splineConfig;
	RadiusProfileArray m_radiusProfileArray;
	ChannelSectionPtr m_startSection;
	ChannelSectionPtr m_endSection;
	Vec3f m_startPosition;
	Vec3f m_endPosition;
	BubbleFunnelExtremityConstraint m_startConstraint;

public:
	// This is the minimal distance let between adjacent bubbles to ensure the
	// string can pass through the gap in-between. Default is 0.01m.
	KyFloat32 m_margin;

private:
	// Intermediary data structures
	KyArray<CircleArcSplineBubbleData> m_bubbleUserDataPool;
	TurnList m_turnList;
	TurnList::NodePool m_turnPool;

	// These positions defines the diagonal in the strip going from the last start
	// Bubble center to the previous one: the next corner Bubbles must be on the left
	// of this diagonal to ensure the strip is string-pullable.
	Vec2f m_startConstraintStripDiagonalLeftPosition;
	Vec2f m_startConstraintStripDiagonalRightPosition;
	bool m_checkStripDiagonal;

	// Output
	KyArray<RadiusProfileCircleArcSplineComputerOutput> m_profileOutputs;


public:
	// You can directly set these advanced parameters to toggle and configure VisualDebug.
	World* m_advancedVisualDebugWorld;
	KyUInt32 m_advancedVisualDebugId;
	String m_advancedVisualDebugPrefix;
	String m_advancedVisualDebugProfileLabel;
	StringPulledBubbleListDisplayConfig m_rawBubbleListDisplayConfig;
	StringPulledBubbleListDisplayConfig m_adjustedBubbleListDisplayConfig;
	StringPulledBubbleListDisplayConfig m_simplifiedBubbleListDisplayConfig;
	CircleArcDisplayConfig m_circleArcDisplayConfig;

	// Activate this to track down error cases
	SplineInputBlobDumpConfig m_advancedDebug_InputBlobDumpConfig;
};

} // namespace Kaim

#endif // Navigation_CircleArcSplineComputer_H
