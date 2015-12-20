/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: MUBI
#ifndef Navigation_BotMoveOnNavMesh_H
#define Navigation_BotMoveOnNavMesh_H

#include "gwnavruntime/queries/moveonnavmeshquery.h"

namespace Kaim
{

/// This class is used to get move bots on the NavMesh without physics engine.
class BotMoveOnNavMesh
{
public:
	template<class TraverseLogic>
	static Vec3f ComputeMoveOnNavMesh(const BotMoveOnNavMesh& input)
	{
		if(input.m_simulationTimeInSeconds == 0.0f)
			return input.m_currentPosition;

		// compute new position
		Kaim::Vec3f move = input.m_velocity * input.m_simulationTimeInSeconds; 

		// query the navMesh to ensure that we are moving to a valid location
		Kaim::Vec2f move2d;
		KyFloat32 dist = move.GetNormalized2d(move2d);

		if(dist == 0.0f)
			return input.m_currentPosition;

		Kaim::MoveOnNavMeshQuery<TraverseLogic> query;
		query.BindToDatabase          (input.m_database);
		query.SetTraverseLogicUserData(input.m_traverseLogicUserData);
		query.Initialize              (input.m_currentPosition, move2d, dist);
		query.SetStartTrianglePtr     (input.m_navTrianglePtr); // initialize the StartTrianglePtr with the triangle the bot is standing upon
		query.SetPerformQueryStat     (Kaim::QueryStat_LabEngine);

		query.PerformQuery();

		switch (query.GetResult())
		{
		case Kaim::MOVEONNAVMESH_NOT_INITIALIZED:
		case Kaim::MOVEONNAVMESH_NOT_PROCESSED:
			return input.m_currentPosition;

		case Kaim::MOVEONNAVMESH_DONE_START_OUTSIDE:
		case Kaim::MOVEONNAVMESH_DONE_START_NAVTAG_FORBIDDEN:
		case Kaim::MOVEONNAVMESH_DONE_START_LINKONEDGE_ERROR:
		case Kaim::MOVEONNAVMESH_DONE_LACK_OF_WORKING_MEMORY:
		case Kaim::MOVEONNAVMESH_DONE_UNKNOWN_ERROR:
			return input.m_currentPosition + move;

		case Kaim::MOVEONNAVMESH_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION:
		case Kaim::MOVEONNAVMESH_DONE_INTERMEDIARYPOS_FOUND:
		case Kaim::MOVEONNAVMESH_DONE_ARRIVALPOS_FOUND_NO_COLLISION:
			return query.GetArrivalPos();

		default:
			break;
		}

		return input.m_currentPosition;
	}

public:
	Vec3f          m_currentPosition;
	Vec3f          m_velocity; 
	KyFloat32      m_simulationTimeInSeconds;
	Database*      m_database;
	void*          m_traverseLogicUserData;
	NavTrianglePtr m_navTrianglePtr;
};

} // namespace Kaim

#endif //Navigation_BotMoveOnNavMesh_H

