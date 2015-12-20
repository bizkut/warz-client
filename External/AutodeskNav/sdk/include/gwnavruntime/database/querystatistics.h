/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY

#ifndef Navigation_QueryStatistics_H
#define Navigation_QueryStatistics_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/base/memory.h"

namespace Kaim
{

class Database;


class PerformQueryStatCounter
{
public:
	void Clear();
	void Increment(QueryType queryType);
	KyUInt32 GetCount(QueryType queryType) const;

public:
	KyUInt32 m_perQueryCounter[QueryType_FirstCustom];
};

enum QueryStatReportType
{
	QueryStatReport_RelevantOnly, /// The report will prune PerformQueryStat with no count
	QueryStatReport_All, /// The report will show all stats for all queries
};

/// This class is not instantiated in databases when KY_BUILD_SHIPPING is defined
/// It contains statistics on calls to PerformQuery() or PerformQueryWithInputCoordPos().
/// Note that some queries can call other queries internally.
/// Expected usage is:
/// \code {
///   PerformQueryStatistics* queryStats = database->GetPerformQueryStatistics()
///   queryStats.ResetPerformQueryCounter();
///	  // ..
///   // Several frames running
///   // ..
///   StringBuffer report;
///   queryStats.Report(&report); // Give the total number of calls per PerformQueryStat per QueryType (not per frame)
/// } \endcode
class PerformQueryStatistics
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
public:
	
	/// Call This to rest the counters
	void ResetPerformQueryCounter();
	/// The report will give the total number of calls per PerformQueryStat per QueryType (it is not reported per frame)
	void Report(StringBuffer* report, QueryStatReportType reportType = QueryStatReport_RelevantOnly) const;
	

public: // internal
	void Initialize(); /// Called by the Database

	static const char* GetPerformQueryStatName(PerformQueryStat queryStat);

	void IncrementPerformQueryStat(PerformQueryStat stat, QueryType queryType);
	KyUInt32 GetPerformQueryStatCount(PerformQueryStat stat, QueryType queryType);
	KyUInt32& GetPerformQueryCount(QueryType queryType);

public:
	KyArray<KyUInt32> m_perQueryCounter; ///< counter used to determine how many queries of a given QueryType were performed between two calls to ResetPerformQueryCounter()
	KyArray<PerformQueryStatCounter> m_queryStatCounter; ///< counter used to determine how many queries of a given QueryType associated to a specific Stat were performed between two calls to ResetPerformQueryCounter()
};

/// This class is used in PerformQuery() or PerformQueryWithInputCoordPos() functions to count the number of query performed
class ScopedPerformedQueryCounter
{
	KY_CLASS_WITHOUT_COPY(ScopedPerformedQueryCounter)
public:
	ScopedPerformedQueryCounter(QueryType queryType, Database* database, PerformQueryStat queryStat);
	~ScopedPerformedQueryCounter();

private:
#ifndef KY_BUILD_SHIPPING
	KyUInt32& m_counter;
#endif
};


KY_INLINE void PerformQueryStatCounter::Increment(QueryType queryType) { ++m_perQueryCounter[queryType]; }
KY_INLINE KyUInt32 PerformQueryStatCounter::GetCount(QueryType queryType) const { return m_perQueryCounter[queryType]; }
KY_INLINE void PerformQueryStatCounter::Clear()
{
	for (KyUInt32 i = 0; i < QueryType_FirstCustom; ++i)
		m_perQueryCounter[i] = 0;
}


KY_INLINE KyUInt32& PerformQueryStatistics::GetPerformQueryCount(QueryType queryType)  { return m_perQueryCounter[queryType]; }
KY_INLINE void PerformQueryStatistics::IncrementPerformQueryStat(PerformQueryStat stat, QueryType queryType) { if (stat < PerformQueryStat_Count) m_queryStatCounter[stat].Increment(queryType); }
KY_INLINE KyUInt32 PerformQueryStatistics::GetPerformQueryStatCount(PerformQueryStat stat, QueryType queryType) { return m_queryStatCounter[stat].GetCount(queryType); }


#ifndef KY_BUILD_SHIPPING
KY_INLINE ScopedPerformedQueryCounter::~ScopedPerformedQueryCounter() { ++m_counter;  }
#else
KY_INLINE ScopedPerformedQueryCounter::ScopedPerformedQueryCounter(QueryType, Database*, PerformQueryStat) {}
KY_INLINE ScopedPerformedQueryCounter::~ScopedPerformedQueryCounter() {}
#endif

}

#endif
