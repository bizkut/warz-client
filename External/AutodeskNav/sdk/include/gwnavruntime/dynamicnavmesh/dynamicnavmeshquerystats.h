/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_DynamicNavMeshQueryStats_H
#define Navigation_DynamicNavMeshQueryStats_H

// primary contact: LASI - secondary contact: NONE
#include "gwnavruntime/basesystem/logger.h"

namespace Kaim
{

class DynamicNavMeshQueryStats
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynamicNavMeshQueryStats():
		m_extractInputData(0.0f), 
		m_edgeIntersectTime(0.0f), 
		m_sweeplineTime(0.0f),
		m_polygonBuildingTime(0.0f),
		m_triangulatorInputConversionTime(0.0f),
		m_triangulationTime(0.0f),
		m_delaunayTime(0.0f),
		m_triangulatoroutPutConversionTime(0.0f),
		m_navFloorBuilding(0.0f),
		m_edgeTypeFixing(0.0f),
		m_altitudeComputationTime(0.0f),
		m_ACT_PixelBoxAndFloatComputation(0.0f),
		m_ACT_StaticVertexBrowse(0.0f),
		m_ACT_IsInsideTriangletest(0.0f),
		m_ACT_edgeCaseResolution(0.0f),
		m_blobBuildingTime(0.0f),
		m_totalQueryTime(0.0f), 
		m_inputEdgeCount(0){}

public: 
	DynamicNavMeshQueryStats& operator+=(const DynamicNavMeshQueryStats& rhs) 
	{
		m_extractInputData                 += rhs.m_extractInputData;
		m_edgeIntersectTime                += rhs.m_edgeIntersectTime;
		m_sweeplineTime                    += rhs.m_sweeplineTime;
		m_polygonBuildingTime              += rhs.m_polygonBuildingTime;
		m_triangulatorInputConversionTime  += rhs.m_triangulatorInputConversionTime;
		m_triangulationTime                += rhs.m_triangulationTime;
		m_delaunayTime                     += rhs.m_delaunayTime;
		m_triangulatoroutPutConversionTime += rhs.m_triangulatoroutPutConversionTime;
		m_navFloorBuilding                 += rhs.m_navFloorBuilding;
		m_edgeTypeFixing                   += rhs.m_edgeTypeFixing;
		m_altitudeComputationTime          += rhs.m_altitudeComputationTime;
		m_ACT_PixelBoxAndFloatComputation  += rhs.m_ACT_PixelBoxAndFloatComputation;
		m_ACT_StaticVertexBrowse           += rhs.m_ACT_StaticVertexBrowse;
		m_ACT_IsInsideTriangletest         += rhs.m_ACT_IsInsideTriangletest;
		m_ACT_edgeCaseResolution           += rhs.m_ACT_edgeCaseResolution;
		m_blobBuildingTime                 += rhs.m_blobBuildingTime;
		m_totalQueryTime                   += rhs.m_totalQueryTime;
		m_inputEdgeCount                   += rhs.m_inputEdgeCount;
		return *this;
	} 

	bool operator<(const DynamicNavMeshQueryStats& other)
	{
		return m_totalQueryTime < other.m_totalQueryTime;
	}

	KyFloat32 GetPercentage(KyFloat32 x) const { return (x/m_totalQueryTime * 100.0f); }
	void PrintStats() const
	{
		KY_LOG_MESSAGE((" --- DynamicNavMeshQuery Stats ---"));
		KY_LOG_MESSAGE((" inputDataExtraction                     : %.2f ms (%.1f %%)" , m_extractInputData                , GetPercentage(m_extractInputData)));
		KY_LOG_MESSAGE((" edgeIntersectTime                       : %.2f ms (%.1f %%)" , m_edgeIntersectTime               , GetPercentage(m_edgeIntersectTime)));
		KY_LOG_MESSAGE((" sweeplineTime                           : %.2f ms (%.1f %%)" , m_sweeplineTime                   , GetPercentage(m_sweeplineTime)));
		KY_LOG_MESSAGE((" polygonBuildingTime                     : %.2f ms (%.1f %%)" , m_polygonBuildingTime             , GetPercentage(m_polygonBuildingTime)));
		KY_LOG_MESSAGE((" triangulatorInputConversion             : %.2f ms (%.1f %%)" , m_triangulatorInputConversionTime , GetPercentage(m_triangulatorInputConversionTime)));
		KY_LOG_MESSAGE((" triangulationTime                       : %.2f ms (%.1f %%)" , m_triangulationTime               , GetPercentage(m_triangulationTime)));
		KY_LOG_MESSAGE((" delaunay-isation                        : %.2f ms (%.1f %%)" , m_delaunayTime                    , GetPercentage(m_delaunayTime)));
		KY_LOG_MESSAGE((" triangulatoroutPutConversionTime        : %.2f ms (%.1f %%)" , m_triangulatoroutPutConversionTime, GetPercentage(m_triangulatoroutPutConversionTime)));
		KY_LOG_MESSAGE((" navFloorBuilding                        : %.2f ms (%.1f %%)" , m_navFloorBuilding                , GetPercentage(m_navFloorBuilding)));
		KY_LOG_MESSAGE((" edgeTypeFixing                          : %.2f ms (%.1f %%)" , m_edgeTypeFixing                  , GetPercentage(m_edgeTypeFixing)));
		KY_LOG_MESSAGE((" altitudeComputationTime                 : %.2f ms (%.1f %%)" , m_altitudeComputationTime         , GetPercentage(m_altitudeComputationTime)));
		KY_LOG_MESSAGE((" \t - PixelBoxAndFloatComputation          : %.2f ms (%.1f %%)" , m_ACT_PixelBoxAndFloatComputation , GetPercentage(m_ACT_PixelBoxAndFloatComputation)));
		KY_LOG_MESSAGE((" \t - StaticVertexBrowse                   : %.2f ms (%.1f %%)" , m_ACT_StaticVertexBrowse          , GetPercentage(m_ACT_StaticVertexBrowse)));
		KY_LOG_MESSAGE((" \t - IsInsideTriangletest                 : %.2f ms (%.1f %%)" , m_ACT_IsInsideTriangletest        , GetPercentage(m_ACT_IsInsideTriangletest)));
		KY_LOG_MESSAGE((" \t - edgeCaseResolution                   : %.2f ms (%.1f %%)" , m_ACT_edgeCaseResolution          , GetPercentage(m_ACT_edgeCaseResolution)));
		KY_LOG_MESSAGE((" blobBuilding                            : %.2f ms (%.1f %%)" , m_blobBuildingTime                , GetPercentage(m_blobBuildingTime)));
		KY_LOG_MESSAGE((" number of input edges: %i", m_inputEdgeCount));
		PrintTotalTime();
	}

	void PrintTotalTime() const
	{
		KY_LOG_MESSAGE((" totalQueryTime : %.2f ms", m_totalQueryTime));
	}

public: 
	KyFloat32 m_extractInputData;
	KyFloat32 m_edgeIntersectTime;
	KyFloat32 m_sweeplineTime;
	KyFloat32 m_polygonBuildingTime;
	KyFloat32 m_triangulatorInputConversionTime;
	KyFloat32 m_triangulationTime;
	KyFloat32 m_delaunayTime;
	KyFloat32 m_triangulatoroutPutConversionTime;
	KyFloat32 m_navFloorBuilding;
	KyFloat32 m_edgeTypeFixing;
	KyFloat32 m_altitudeComputationTime;
	KyFloat32 m_ACT_PixelBoxAndFloatComputation;
	KyFloat32 m_ACT_StaticVertexBrowse;
	KyFloat32 m_ACT_IsInsideTriangletest;
	KyFloat32 m_ACT_edgeCaseResolution;

	KyFloat32 m_blobBuildingTime;
	KyFloat32 m_totalQueryTime;
	KyUInt32 m_inputEdgeCount;
};

} // namespace Kaim



#endif // Navigation_DynamicNavMeshQueryStats_H
