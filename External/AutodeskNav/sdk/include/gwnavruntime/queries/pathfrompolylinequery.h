/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_PathFromPolylineQuery_H
#define Navigation_PathFromPolylineQuery_H

#include "gwnavruntime/queries/utils/basepathfrompolylinequery.h"
#include "gwnavruntime/queries/raycastquery.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/path/path.h"
#include "gwnavruntime/channel/channelarraycomputer.h"


namespace Kaim
{

template<class TraverseLogic>
class PathFromPolylineQuery : public BasePathFromPolylineQuery
{
public:

	// ---------------------------------- Public Member Functions ----------------------------------

	PathFromPolylineQuery();
	virtual ~PathFromPolylineQuery();

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the input and output, lets all configuration parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	void Initialize(const Vec3f* vertexBuffer, KyUInt32 vertexCount);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Write accessor for TraversalParams::m_positionSpatializationRange.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_positionSpatializationRange is set to its default value.
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	// ---------------------------------- Query framework functions ----------------------------------

	/// Performs a single iteration of the PathFinder.
	/// \copydoc IQuery::Advance()
	virtual void Advance(WorkingMemory* workingMemory);

	virtual void ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------
	PathFromPolylineQueryResult GetResult()   const;
	Path*                       GetPath()     const;
	const Vec3f&                GetStartPos() const;
	const KyArrayPOD<Vec3f>&    GetPolyline() const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public: //Internal
	bool HasNavDataChanged();
	KyResult FirstStepOfClamperContextInit(WorkingMemory* workingMemory);
	KyResult PathClamperContextInitSetp(WorkingMemory* workingMemory);
protected:
	void SetFinish(WorkingMemory* workingMemory);

	KyResult CheckNavDataChange(WorkingMemory* workingMemory);
	bool HasExplorationReachedDestNode();
};

}

#include "gwnavruntime/queries/pathfrompolylinequery.inl"

#endif //Navigation_PathFromPolylineQuery_H

