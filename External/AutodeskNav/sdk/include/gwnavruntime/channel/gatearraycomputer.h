/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_GateArrayComputer_H
#define Navigation_GateArrayComputer_H

#include "gwnavruntime/channel/channelcomputerconfig.h"
#include "gwnavruntime/channel/stringpuller.h"
#include "gwnavruntime/channel/channel.h"
#include "gwnavruntime/channel/stringpulledbubblelist.h"
#include "gwnavruntime/channel/channelsectionwidener.h"
#include "gwnavruntime/queries/utils/bestfirstsearch2dbordercollector.h"
#include "gwnavruntime/queries/utils/bestfirstsearchtraversal.h"
#include "gwnavruntime/queries/trianglefromposquery.h"
#include "gwnavruntime/queries/raycastquery.h"
#include "gwnavruntime/queries/utils/clearancepolygonintersector.h"
#include "gwnavruntime/database/activedata.h"
#include "gwnavruntime/base/timeprofiler.h"



namespace Kaim
{

enum GateArrayComputerResult
{
	GateArrayComputerResult_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	GateArrayComputerResult_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	GateArrayComputerResult_ComputingFirstGate,
	GateArrayComputerResult_ComputingIntermediateGates,
	GateArrayComputerResult_ComputingLastGate,

	GateArrayComputerResult_Done_InputError,
	GateArrayComputerResult_Done_FirstGateError,
	GateArrayComputerResult_Done_IntermediateGatesError,
	GateArrayComputerResult_Done_LastGateError,

	GateArrayComputerResult_Done
};

class GateArrayComputer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	GateArrayComputer();

	void Initialize(Database* database, const StringPulledBubbleList& stringPulledBubbleList, Channel* channel);

	bool IsFinished() const { return m_result >= GateArrayComputerResult_Done_InputError; }
	GateArrayComputerResult GetResult() const { return m_result; }

	void Clear();

	template<class TraverseLogic>
	void Advance();

	template<class TraverseLogic>
	KyResult Compute();

	ChannelComputerConfig& GetComputerConfig();

	KyFloat32 GetComputationDurationMks()  const;
	void*     GetTraverseLogicUserData() const;

	Channel* GetChannel() { return m_channel; }

	void SetComputerConfig(const ChannelComputerConfig& computerConfig);
	void SetTraverseLogicUserData(void* traverseLogicUserData);

protected:
	void UpdateInternalParametersFromConfig();

	template<class TLogic>
	KyResult TreatNextStringPulledBubble_FirstGate(KyUInt32 bubbleIdx);

	template<class TLogic>
	KyResult TreatNextStringPulledBubble_LastGate();

	template<class TraverseLogic>
	KyResult ComputeLeftTurnGate(const Gate& previousGate, Gate& gate, const Vec3f& leftCorner, const Vec2f& direction, KyFloat32 gateWidth);

	template<class TraverseLogic>
	KyResult ComputeRightTurnGate(const Gate& previousGate, Gate& gate, const Vec3f& rightCorner, const Vec2f& direction, KyFloat32 gateWidth);

	template<class TraverseLogic>
	KyResult ComputeIntermediaryGates();

	void InitClearancePolygonIntersectorForCurrentSection(ClearancePolygonIntersector& clearancePolygonIntersector);


protected:
	Ptr<Database> m_database;
	ChannelComputerConfig m_computerConfig;
	void* m_traverseLogicUserData;

	GateArrayComputerResult m_result;
	// stats
	KyFloat32 m_computationDurationMks;

	// internals
	KyFloat32 m_minTurnAngleCos; // Optim: compute once for all at Compute() begin from the current computer config degree value.

	// Internal parameter for intermediary Gates computation. These values are computed
	// from the ChannelComputerConfig client values.
	KyFloat32 m_intermediaryGateMinDistance;

	const StringPulledBubbleList* m_stringPulledBubbleList;
	Ptr<Channel> m_channel;

public: // internal
	KyUInt32 m_currentBubbleIdx;
	Vec2f m_prevEdgeDir2d;
	Vec2f m_nextEdgeDir2d;
	Gate m_currentNewGate;
	KyFloat32 m_channelCornerRadius;
	Vec3f m_channelCornerPos;

	// temp for intermediate gates computing and adjusting
	ChannelSectionWidener m_channelSectionWidener;
	NavTrianglePtr m_previousGateNavTriangle;
	KyUInt32 m_prevGateIdx;

public:
	bool m_advancedVisualDebuggingEnabled;
};

} // namespace Kaim


#include "gwnavruntime/channel/gatearraycomputer.inl"


#endif // Navigation_GateArrayComputer_H
