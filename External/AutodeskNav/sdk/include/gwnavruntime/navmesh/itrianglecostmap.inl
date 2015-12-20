/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

// Internal class, no need to expose it in ".h" file.
class TriangleCostInCell
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CostMap)

public:
	TriangleCostInCell();
	~TriangleCostInCell();

	void Clear();

	static KyUInt32 GetNeededBufferSize(const ActiveNavFloorCollection& activeNavFloors);

	void Init(const ActiveNavFloorCollection& activeNavFloors, char*& memoryStart);

	KyFloat32 GetCostMultiplier(const NavTriangleRawPtr& triangle) const;
	void SetCostMultiplier(const NavTriangleRawPtr& triangle, KyFloat32 costMultiplier);
	KyFloat32* GetTriangleCostBuffer(NavFloor* navFloor) const;
public:
	char* m_memoryStart;
};

KY_INLINE TriangleCostInCell::TriangleCostInCell()  { Clear(); }
KY_INLINE TriangleCostInCell::~TriangleCostInCell() { Clear(); }
KY_INLINE void TriangleCostInCell::Clear() { m_memoryStart = KY_NULL; }

KY_INLINE KyFloat32* TriangleCostInCell::GetTriangleCostBuffer(NavFloor* navFloor) const
{
	KyFloat32** pointersTotriangleCostBuffer = (KyFloat32**)m_memoryStart;;
	return pointersTotriangleCostBuffer[navFloor->m_idxInActiveNavFloorCollection];
}

KY_INLINE void TriangleCostInCell::SetCostMultiplier(const NavTriangleRawPtr& triangle, KyFloat32 costMultiplier)
{
	KyFloat32* triangleCostBuffer = GetTriangleCostBuffer(triangle.GetNavFloor());
	triangleCostBuffer[triangle.GetTriangleIdx()] = costMultiplier;
}

KY_INLINE KyFloat32 TriangleCostInCell::GetCostMultiplier(const NavTriangleRawPtr& triangle) const
{
	KyFloat32* triangleCostBuffer = GetTriangleCostBuffer(triangle.GetNavFloor());
	if (triangleCostBuffer != KY_NULL)
		return triangleCostBuffer[triangle.GetTriangleIdx()];

	return 1.f;
}


template<class CostMultiplierComputer>
inline void ITriangleCostMap::InitAndBrowseAllTrianglesInBox3f(CostMultiplierComputer& compute, const Box3f& aabb)
{
	if (InitCostMapForBox3f(aabb) == false)
		return;

	Kaim::ActiveData* activeData = m_database->GetActiveData();
	Kaim::Box2iIteratorInBiggerRowMajorBox boxIterator(m_cellBox, activeData->m_cellBox);

	while (boxIterator.IsFinished() == false)
	{
		TriangleCostInCell& triangleCostInCell = ((TriangleCostInCell*)m_bufferOfCostsInCell)[boxIterator.GetInnerBoxRowMajorIndex()];
		ActiveCell& activeCell = activeData->m_bufferOfActiveCell[boxIterator.GetOuterBoxRowMajorIndex()];

		NavFloor** activeNavFloorBuffer = activeCell.GetActiveNavFloorBuffer();
		KyUInt32 activeNavFloorCount    = activeCell.GetActiveNavFloorsCount();

		for(KyUInt32 activeFloorIdx = 0; activeFloorIdx < activeNavFloorCount; activeFloorIdx++)
		{
			NavFloor* navFloor = activeNavFloorBuffer[activeFloorIdx];

			if (DoesNavFloorIntersectBox3f(navFloor, aabb) == false)
				continue;

			KyUInt32 triangleCount = navFloor->GetNavFloorBlob()->GetNavTriangleCount();
			KyFloat32* costMultiplierBuffer = triangleCostInCell.GetTriangleCostBuffer(navFloor);
			for(NavTriangleIdx triangleIdx = 0; triangleIdx < triangleCount; ++triangleIdx)
			{
				NavTriangleRawPtr triangle(navFloor, triangleIdx);
				KyFloat32 costMultiplier = compute.ComputeCostMultiplier(triangle);
				costMultiplierBuffer[triangleIdx] = costMultiplier;
			}
		}

		++boxIterator;
	}
}

namespace
{
	template<class CostMultiplierComputer>
	class VolumePropagator
	{
		KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

	public:
		VolumePropagator(CostMultiplierComputer& costMultiplierComputer, ITriangleCostMap* costMap)
			: m_costMultiplierComputer(&costMultiplierComputer)
			, m_triangleCostMap(costMap)
		{}

		bool IsSearchFinished() { return false; }
		bool ShouldVisitNeighborTriangle(const NavTriangleRawPtr& triangleRawPtr, KyUInt32 indexOfNeighborTriangle)
		{
			NavTriangleRawPtr neighbor = GetNeighborTriangle(triangleRawPtr, indexOfNeighborTriangle);
			if (neighbor.IsValid() == false)
				return false;

			return m_costMultiplierComputer->DoesTriangleIntersectVolume(neighbor);
		}

		// only called on the start triangles
		// we suppose the start triangle intersects the propagation volume
		bool ShouldVisitTriangle(const NavTriangleRawPtr& /*triangleRawPtr*/) { return true; }

		NavTriangleRawPtr GetNeighborTriangle(const NavTriangleRawPtr& /*triangleRawPtr*/, KyUInt32 indexOfNeighborTriangle)
		{
			return m_neighborTrianglePtr[indexOfNeighborTriangle];
		}

		void Visit(const NavTriangleRawPtr& triangleRawPtr, const TriangleStatusInGrid& triangleStatus)
		{
			m_triangleCostMap->SetCostMultiplier(triangleRawPtr, m_costMultiplierComputer->ComputeCostMultiplier(triangleRawPtr));

			for(KyUInt32 i = 0; i < 3; ++i)
				m_neighborTrianglePtr[i].Invalidate();

			NavFloor* navFloor = triangleRawPtr.GetNavFloor();
			const NavHalfEdgeIdx firstNavHalfEdgeIdx = NavFloorBlob::NavTriangleIdxToFirstNavHalfEdgeIdx(triangleRawPtr.GetTriangleIdx());

			NavHalfEdgeRawPtr halfEdgeRawPtr[3];
			for(KyUInt32 i = 0; i < 3; ++i)
				halfEdgeRawPtr[i].Set(navFloor, firstNavHalfEdgeIdx + i);


			for(KyUInt32 i = 0; i < 3; ++i)
			{
				NavHalfEdgeRawPtr pairHalfEdgeRawPtr;
				bool isACrossableEdges = halfEdgeRawPtr[i].IsHalfEdgeCrossable(pairHalfEdgeRawPtr);

				if (isACrossableEdges == false)
					continue;

				m_neighborTrianglePtr[i].Set(pairHalfEdgeRawPtr.GetNavFloor(), NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(pairHalfEdgeRawPtr.GetHalfEdgeIdx()));

				bool neighborTriangleAlreadyVisited = triangleStatus.IsTriangleOpen(m_neighborTrianglePtr[i]);
				if (neighborTriangleAlreadyVisited)
					m_neighborTrianglePtr[i].Invalidate();
			}
		}

	public:
		CostMultiplierComputer* m_costMultiplierComputer;
		ITriangleCostMap* m_triangleCostMap;
	private:
		bool m_neighborTriangleAlreadyVisited[3];
		NavTriangleRawPtr m_neighborTrianglePtr[3];
	};
}

template<class CostMultiplierComputer>
inline void ITriangleCostMap::InitAndPropagateInTrianglesFromPosInVolume(CostMultiplierComputer& computer, const Box3f& aabb, const Vec3f& inputPosition)
{
	if (InitCostMapForBox3f(aabb) == false)
		return;

	QueryUtils queryUtils(m_database, m_database->GetWorkingMemory(), KY_NULL);

	NavTrianglePtr startTrianglePtr;
	if (queryUtils.FindTriangleFromPositionIfNotValid(startTrianglePtr, inputPosition, m_database->GetPositionSpatializationRange()) == false)
	{
		KY_LOG_WARNING(("The input position you provide to propagate from is not in the navMesh, the propafation cannot be done."));
		// no triangle to start the propagation
		return;
	}

	VolumePropagator<CostMultiplierComputer> volumePropagator(computer, this);
	BreadthFirstSearchTraversal<VolumePropagator<CostMultiplierComputer> > traversal(queryUtils, m_cellBox, volumePropagator);
	TraversalResult traversalRC;

	traversalRC = traversal.SetStartTriangle(startTrianglePtr.GetRawPtr());
	if (traversalRC != TraversalResult_DONE)
		return;

	traversalRC = traversal.Search();
	switch(traversalRC)
	{
	case TraversalResult_DONE :
		break;
	case TraversalResult_LACK_OF_MEMORY_FOR_OPEN_NODES:
	case TraversalResult_LACK_OF_MEMORY_FOR_CLOSED_NODES:
	case TraversalResult_LACK_OF_MEMORY_FOR_VISITED_NODES:
		{
			KY_LOG_WARNING(("This traversal reached the maximum size of result memory in the queryUtils (in TriangleCostMap"));
			break;
		}
	default:
		{
			KY_LOG_ERROR( ("Unknown error in the traversal in TriangleCostMap"));
			break;
		}
	}
}

}

