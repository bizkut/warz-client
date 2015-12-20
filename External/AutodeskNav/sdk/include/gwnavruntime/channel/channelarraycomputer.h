/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_ChannelArrayComputer_H
#define Navigation_ChannelArrayComputer_H

#include "gwnavruntime/channel/channelarraycomputerstats.h"
#include "gwnavruntime/channel/diagonalstripcomputer.h"
#include "gwnavruntime/channel/stringpuller.h"
#include "gwnavruntime/channel/gatearraycomputer.h"
#include "gwnavruntime/channel/bubblearraybuilder.h"
#include "gwnavruntime/channel/channelsectionptr.h"
#include "gwnavruntime/channel/channelarray.h"

#include "gwnavruntime/queries/utils/pathfinderqueryutils.h"


namespace Kaim
{

enum ChannelArrayComputerResult
{
	ChannelArrayResult_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	ChannelArrayResult_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	ChannelArrayResult_StartNewPathSection,
	ChannelArrayResult_ComputingChannel_DiagonalStrip_Init,
	ChannelArrayResult_ComputingChannel_DiagonalStrip,
	ChannelArrayResult_ComputingChannel_StringPull_Init,
	ChannelArrayResult_ComputingChannel_StringPull,
	ChannelArrayResult_ComputingChannel_GateArray_Init,
	ChannelArrayResult_ComputingChannel_GateArray,
	ChannelArrayResult_ComputingChannel_PreChannelPolyline,
	ChannelArrayResult_ComputingChannel_PostChannelPolyline,
	ChannelArrayResult_ComputingAggregatedPath,

	ChannelArrayResult_Error_InvalidRawPath,
	ChannelArrayResult_Error_ChannelComputation_DiagonalStrip,
	ChannelArrayResult_Error_ChannelComputation_StringPull,
	ChannelArrayResult_Error_ChannelComputation_GateArray,
	ChannelArrayResult_Error_ChannelComputation_PreChannelPolyline,
	ChannelArrayResult_Error_ChannelComputation_PostChannelPolyline,
	ChannelArrayResult_Error_PathAggregation,

	ChannelArrayResult_Done
};

class ChannelArrayComputer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryWorkingMem)

public:
	ChannelArrayComputer();
	~ChannelArrayComputer();

	void SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig) { m_channelComputerConfig = channelComputerConfig; }

	void* GetTraverseLogicUserData() const { return m_traverseLogicUserData; }
	void SetTraverseLogicUserData(void* traverseLogicUserData) { m_traverseLogicUserData = traverseLogicUserData; }


	void Init(Path* rawPath);
	void Clear();

	template<class TraverseLogic>
	void Advance(WorkingMemory* workingMemory);
	bool IsFinished() const { return m_result >= ChannelArrayResult_Error_InvalidRawPath; }
	ChannelArrayComputerResult GetResult() const { return m_result; }

	template<class TraverseLogic>
	KyResult FullCompute(Path* rawPath, Ptr<Path>& pathWithChannels, WorkingMemory* workingMemory);


private:
	template<class TLogic>
	KyResult ComputePreChannelPolyline(Channel* channel);

	template<class TLogic>
	KyResult ComputePostChannelPolyline(Channel* channel);

	template<class TLogic>
	KyResult CreateAggregatedPath();

	template<class TLogic>
	KyResult CreateChannelPath(Database* database, const Channel& channel, Ptr<Path>& channelPath);

	void UpdateChannelSectionFromPath(const Path* channelPath, KyUInt32 channelIdx, Ptr<Channel>& channel);

	KyResult FillAggregatedPathWithChannelPath(const Path* ChannelPath, KyUInt32& currentAggregatedPathEdgeIndex,
		KyInt32 clearanceDistanceInt, KyInt32 cellSizeInCoord);

	KyResult FillAggregatedPathWithRawPathSection(KyUInt32 firstEdgeIndex, KyUInt32 lastEdgeIndex, KyUInt32& currentAggregatedPathEdgeIndex);

	bool GetNextEdgeOnNavMesh(const KyUInt32 currentEdgeIdx, KyUInt32& nextOnNavMeshEdgeIdx);
	bool GetNextEdgeNotOnNavMesh(const KyUInt32 currentEdgeIdx, KyUInt32& nextOnNavMeshEdgeIdx);

	void FindEndNodeIdxOfNavMeshSection();
	void FindEndNodeIdxOfNonNavMeshSection();

	void SetPathNodeToPathWithChannels(const Path* path, KyUInt32 pathNodeIndex, KyUInt32 aggregatedPathNodeIndex);
	void SetPathEdgeToPathWithChannels(const Path* path, KyUInt32 pathEdgeIndex, KyUInt32 aggregatedPathEdgeIndex);


public:
	ChannelComputerConfig m_channelComputerConfig;
	void* m_traverseLogicUserData;

	KyArray<KyUInt32> m_navGraphSectionBoundaryIndices;
	KyUInt32 m_currentSectionStartNodeIdx;
	KyUInt32 m_currentSectionEndNodeIdx;

	Ptr<Path> m_rawPath;
	Ptr<ChannelArray> m_channelArray;
	Ptr<Path> m_pathWithChannels;

	ChannelArrayComputerResult m_result;

	Ptr<Channel> m_channelUnderConstruction;

	DiagonalStrip m_diagonalStrip;
	BubbleArray m_bubbleArray;
	StringPulledBubbleList m_stringPulledBubbleList;

	DiagonalStripComputer m_diagonalStripComputer;
	GateArrayComputer m_gateArrayComputer;

	bool m_advancedVisualDebuggingEnabled;
	ChannelArrayComputerStats m_stats;

};

} // namespace Kaim


#include "gwnavruntime/channel/channelarraycomputer.inl"


#endif // Navigation_ChannelArrayComputer_H
