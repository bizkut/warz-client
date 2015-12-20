/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_FindFirstVisiblePositionOnPathQuery_H
#define Navigation_FindFirstVisiblePositionOnPathQuery_H

#include "gwnavruntime/queries/utils/basefindfirstvisiblepositiononpathquery.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "gwnavruntime/visualsystem/displaylist.h"


namespace Kaim
{

/// The FindFirstVisiblePositionOnPathQuery class searches for a PositionOnPath that
/// can be reached from a provided point (#m_visibilityStartPos3f).
/// It samples the path starting from #m_startPositionOnPath up to #m_endPositionOnPath,
/// and stops at the first visible PositionOnPath it reaches.
template<class TraverseLogic>
class FindFirstVisiblePositionOnPathQuery : public BaseFindFirstVisiblePositionOnPathQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	FindFirstVisiblePositionOnPathQuery();
	virtual ~FindFirstVisiblePositionOnPathQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPositionOnPath     Sets the value of #m_startPositionOnPath.
	/// \param endPositionOnPath       Sets the value of #m_endPositionOnPath.
	/// \param visibilityStartPoint    Sets the value of #m_visibilityStartPos3f.
	void Initialize(const PositionOnPath& startPositionOnPath, const PositionOnPath& endPositionOnPath, const Vec3f& visibilityStartPoint);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// Set The NavMesh triangle that corresponds to #m_visibilityStartPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query.
	/// Call this method after Initialize(), during which it is cleared. 
	void SetVisibilityStartTrianglePtr(const NavTrianglePtr& visibilityStartTrianglePtr);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	void SetSamplingDistance(KyFloat32 samplingDistance);

	/// Sets the RayCanGoMarginMode value to be used in the ray can go queries.
	/// Since this query is typically used to find a valid target on the path, the best
	/// mode is IdealTrajectoryMargin.
	void SetRayCanGoMarginMode(RayCanGoMarginMode rayCanGoMarginMode);


	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	FindFirstVisiblePositionOnPathQueryResult GetResult()                      const;
	const PositionOnPath&                     GetStartPositionOnPath()         const;
	const PositionOnPath&                     GetEndPositionOnPath()           const;
	const Vec3f&                              GetVisibilityStartPoint()        const;
	const NavTrianglePtr&                     GetVisibilityStartTrianglePtr()  const;
	KyFloat32                                 GetSamplingDistance()            const;
	const PositionOnPath&                     GetFirstVisiblePositionOnPath()  const;

private:
	CandidateValidationResult ValidateCandidate(
		RayCanGoQuery<TraverseLogic>& rayCanGoQuery, const PositionOnPath& candidate, WorkingMemory* workingMemory);

public:
	ScopedDisplayList* m_displayList;
};

// Small utility class that simply wrap a WorkingMemArray<PositionOnPath> to clean it correctly in destructor.
class PositionOnPathWorkingMemArray
{
public:
	PositionOnPathWorkingMemArray() {}

	KyResult BindToDatabase(WorkingMemory* workingMemory, KyUInt32 capacity)
	{
		m_array.Init(workingMemory);

		if (m_array.IsInitialized() == false)
			return KY_ERROR;

		return m_array.SetMinimumCapacity(capacity);
	}

	KyResult PushBack(const PositionOnPath& positionOnPath)
	{
		return m_array.PushBack(InternalPositionOnPath(positionOnPath));
	}

	void GetPositionOnPath(KyUInt32 index, PositionOnPath& positionOnPath) const
	{
		m_array[index].CopyToPositionOnPath(positionOnPath);
	}

	KyUInt32 GetCount() const { return m_array.GetCount(); }

private:
	class InternalPositionOnPath
	{
	public:
		InternalPositionOnPath(const PositionOnPath& positionOnPath)
		{
			positionOnPath.GetRawValues(m_path, m_position, m_onPathStatus, m_currentIndexOnPath, m_distanceToEndOfPath);
		}

		void CopyToPositionOnPath(PositionOnPath& positionOnPath) const
		{
			positionOnPath.SetRawValues(m_path, m_position, m_onPathStatus, m_currentIndexOnPath, m_distanceToEndOfPath);
		}

		Path* m_path;
		Vec3f m_position;
		PositionOnPathStatus m_onPathStatus;
		KyUInt32 m_currentIndexOnPath;
		KyFloat32 m_distanceToEndOfPath;
	};

	WorkingMemArray<InternalPositionOnPath> m_array;
};

//class PositionOnPathWorkingMemArray
//{
//public:
//	PositionOnPathWorkingMemArray(WorkingMemory* /*workingMemory*/, KyUInt32 /*capacity*/) {}
//
//	KyArray<PositionOnPath> m_array;
//};

}

#include "gwnavruntime/queries/findfirstvisiblepositiononpathquery.inl"


#endif // Navigation_FindFirstVisiblePositionOnPathQuery_H

