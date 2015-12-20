/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: JAPA - secondary contact: GUAL
#ifndef Navigation_CollisionWorld_H
#define Navigation_CollisionWorld_H

#include "gwnavruntime/collision/heightfield.h"
#include "gwnavruntime/collision/collisiondata.h"
#include "gwnavruntime/containers/collection.h"
#include "gwnavruntime/collision/icollisioninterface.h"
#include "gwnavruntime/math/box2f.h"

namespace Kaim
{

class ColCellPosInfo
{
public:
	Collection<Ptr<CollisionData> > m_colDatas;
};

/// This class is a runtime container for all CollisionData that represents the world.
/// Theere is one and only one CollisionWorld per Kaim::World.
/// Through some sub-classes (IndexedMesh, HeightField, NavCellGrid, ...), the CollisionWorld performs the addition and removal
/// of CollisionData, maintaining a coherent state of "active" CollisionData upon which Queries may be performed.
/// Call AddCollisionData() and RemoveCollisionData() in order to activate/deactivate CollisionData in the world.

class CollisionWorld : public RefCountBaseV<CollisionWorld, MemStat_WorldFwk>
{
	KY_CLASS_WITHOUT_COPY(CollisionWorld)
public:
	CollisionWorld() : m_cellSize(100.f), m_invCellSize(1.f / m_cellSize), m_bufferOfColCellPosInfo(KY_NULL), m_sizeOfCellBuffer(0) {}
	~CollisionWorld() { Clear(); }

	void RemoveAllCollisionData();
	void Clear();

	void SetCollisionInterface(Ptr<ICollisionInterface> visInterface) { m_collisionInterface = visInterface; }

	void AddCollisionData(Ptr<CollisionData> colData);
	void RemoveCollisionData(Ptr<CollisionData> colData);

	CollisionRayCastResult RayCast(const Vec3f& a, const Vec3f& b) const;

	bool IsWorldEmpty() const { return m_colData.GetCount() == 0; } 

private:
	void GetIntersectingHeightFields(const Vec3f& a, const Vec3f& b, KyArray<CollisionData*>& results) const;
	void EnlargeGrid(const CellBox& oldCellBox);

	KyResult GetStartCellFromRay(const Vec3f& a, const Vec3f& b, CellPos& startCellPos) const;

	KY_INLINE CellPos GetCellPosFromVec2f_Floor(const Vec2f& pos) const
	{
		return CellPos((KyInt32)floorf(pos.x * m_invCellSize), (KyInt32)floorf(pos.y * m_invCellSize));
	}

	KY_INLINE CellPos GetCellPosFromVec2f_Ceil(const Vec2f& pos) const
	{
		return CellPos((KyInt32)ceilf(pos.x * m_invCellSize), (KyInt32)ceilf(pos.y * m_invCellSize));
	}

	KY_INLINE CellPos GetCellPosFromVec2f(const Vec2f& pos) const
	{
		return CellPos((KyInt32)(pos.x * m_invCellSize), (KyInt32)(pos.y * m_invCellSize));
	}

	KY_INLINE CellBox GetCellBoxFromBox2f(const Box2f& box2F) const
	{
		const CellPos cellPosMin = GetCellPosFromVec2f_Floor(box2F.m_min);
		const CellPos cellPosMax = GetCellPosFromVec2f_Ceil(box2F.m_max);
		return CellBox(cellPosMin, cellPosMax);
	}

	KY_INLINE Vec2f GetPosFromCellPos(const CellPos& cellPos) const
	{
		return Vec2f(cellPos.x * m_cellSize, cellPos.y * m_cellSize);
	}

	KY_INLINE Box2f GetAABB2D() const
	{
		return Box2f(GetPosFromCellPos(m_cellBox.m_min), GetPosFromCellPos(m_cellBox.m_max));
	}

	TrackedCollection<Ptr<CollisionData>, MemStat_WorldFwk> m_colData;
	Ptr<ICollisionInterface>                          m_collisionInterface;

	CellBox                                           m_cellBox;
	KyFloat32                                         m_cellSize;
	KyFloat32                                         m_invCellSize;
	ColCellPosInfo*                                   m_bufferOfColCellPosInfo;
	KyUInt32                                          m_sizeOfCellBuffer;
};

}

#endif // Navigation_CollisionWorld_H
