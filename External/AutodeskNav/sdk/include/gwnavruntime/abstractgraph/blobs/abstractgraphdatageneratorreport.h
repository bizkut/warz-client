/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphDataGeneratorReport_H
#define Navigation_AbstractGraphDataGeneratorReport_H


#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_Stats.h"
#include "gwnavruntime/basesystem/floatformatter.h"
#include "gwnavruntime/basesystem/logstream.h"


namespace Kaim
{

class AbstractGraphDataGenerator;

class AbstractGraphGenerationReport
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	AbstractGraphGenerationReport()	{ Clear(); }

	void Clear()
	{
		m_createNodesTimeInSeconds = 0.f;
		m_computeCostsTimeInSeconds = 0.f;
		m_generationTimeInSeconds = 0.f;
		m_abstractGraphCount = 0;
		m_averageNodeCount = 0;
		m_averageNeighborNodeCount = 0;
	}

	void ComputeStats(AbstractGraphDataGenerator* generator);

	AbstractGraphGenerationReport& operator+=(const AbstractGraphGenerationReport& other)
	{
		m_createNodesTimeInSeconds += other.m_createNodesTimeInSeconds;
		m_computeCostsTimeInSeconds += other.m_computeCostsTimeInSeconds;
		m_generationTimeInSeconds += other.m_generationTimeInSeconds;
		m_abstractGraphCount += other.m_abstractGraphCount;
		m_averageNodeCount += other.m_averageNodeCount;
		m_averageNeighborNodeCount += other.m_averageNeighborNodeCount;
		return *this;
	}

	AbstractGraphGenerationReport& operator/=(KyUInt32 div)
	{
		m_createNodesTimeInSeconds /= div;
		m_computeCostsTimeInSeconds /= div;
		m_generationTimeInSeconds /= div;
		m_abstractGraphCount /= div;
		m_averageNodeCount /= div;
		m_averageNeighborNodeCount /= div;
		return *this;
	}

public:
	KyUInt32 m_abstractGraphCount;
	KyUInt32 m_averageNodeCount;
	KyUInt32 m_averageNeighborNodeCount;

	KyFloat32 m_createNodesTimeInSeconds;
	KyFloat32 m_computeCostsTimeInSeconds;
	KyFloat32 m_generationTimeInSeconds;
};

template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const AbstractGraphGenerationReport& report)
{
	FloatFormatter fmt2f("%.2f");
	const char* prefix = "  ";

	os << prefix << "Create Nodes time:                  " << fmt2f(report.m_createNodesTimeInSeconds)        << " seconds" << Endl;
	os << prefix << "Compute Costs time:                 " << fmt2f(report.m_computeCostsTimeInSeconds)       << " seconds" << Endl;
	os << prefix << "Total Generation time:              " << fmt2f(report.m_generationTimeInSeconds)         << " seconds" << Endl;

	os << prefix << Endl;

	os << prefix << "AbstractGraph Count :               " << report.m_abstractGraphCount << Endl;
	os << prefix << "Average Node Count :                " << report.m_averageNodeCount << Endl;
	os << prefix << "Average Neighbor Node Count :       " << report.m_averageNeighborNodeCount << Endl;

	return os;
}

class AbstractDataGenerationReport
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	AbstractDataGenerationReport()	{ Init(); }

	void Init()
	{
		m_outputDetails = false;
		m_averageReport.Clear();
		m_abstractDataProcessingTime = 0.f;
		m_reportCount = 0;
	}

	void AddReport(AbstractGraphDataGenerator* generator);

	void Finalize()
	{
		if (m_reportCount != 0)
			m_averageReport /= m_reportCount;
	}

public:
	bool m_outputDetails;
	AbstractGraphGenerationReport m_averageReport;
	KyUInt32 m_reportCount; 
	KyFloat32 m_abstractDataProcessingTime;
};


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const AbstractDataGenerationReport& report)
{
	if (report.m_reportCount == 0)
		return os;

	FloatFormatter fmt2f("%.2f");
	const char* prefix = "  ";
	const char* title = "ABSTRACT GRAPH POST PROCESS REPORT";
	const char* averageTitle = "Average Abstract Graph Generation";

	os << KY_LOG_BIG_TITLE_BEGIN(prefix, title);
	os << prefix << "Total PostProcess time: " << report.m_abstractDataProcessingTime << Endl;
	os << KY_LOG_SMALL_TITLE_BEGIN(prefix, averageTitle);
	os << report.m_averageReport << Endl;
	os << KY_LOG_SMALL_TITLE_END(prefix, averageTitle);
	os << KY_LOG_BIG_TITLE_END(prefix, title);

	return os;
}

}

#endif
