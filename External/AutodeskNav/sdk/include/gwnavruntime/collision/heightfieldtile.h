/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_HeightFieldTile_H
#define Navigation_HeightFieldTile_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/math/cardinaldir.h"
#include "gwnavruntime/math/triangle3f.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"

namespace Kaim
{

struct HeightFieldTile
{
	enum TwoDSide
	{
		LeftOfPoint  = 0,
		RightOfPoint = 1,
	};

	HeightFieldTile() : m_entrantDirection(CardinalDir_INVALID) {}

	KY_INLINE KyUInt32 GetSide(const Vec2f& a, const Vec2f& b, const Vec3f& c) const
	{
		return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x) <= 0.f ? RightOfPoint : LeftOfPoint;
	}

	KY_INLINE Triangle3f GetFirstTriangle() const
	{
		Triangle3f tri;
		tri.Set(m_vertices[0], m_vertices[2], m_vertices[3]);
		return tri;
	}
	
	KY_INLINE Triangle3f GetSecondTriangle() const
	{
		Triangle3f tri;
		tri.Set(m_vertices[0], m_vertices[1], m_vertices[2]);
		return tri;
	}

	// This function assumes that m_entrantDirection == CardinalDir_INVALID
	// In other words, we began from within this tile and need to determine the direction in which we exit.
	CardinalDir GetFirstExitDirection(const Vec2f& a, const Vec2f& b) const;

	// This function assumes that m_entrantDirection != CardinalDir_INVALID
	// In other words, we entered the tile from a given direction and know that we cannot "go backwards".
	
	/*
		B       A
		v2--e1--v1  // CCW order in Navigation.
		|       |   // This means that each vertex is numbered such that vN will be the "right" vertex of eN
		e2      e0
		|  /|\  |
		v3--|--v0
			|
			r
	*/
	KY_INLINE CardinalDir GetExitDirectionFromEntryDirection(const Vec2f& a, const Vec2f& b) const
	{
		const CardinalDir facingDir = GetOppositeCardinalDir(m_entrantDirection);
		const KyUInt32 rightVertexSide = GetSide(a, b, m_vertices[facingDir]);  // rightVertexSide == 0 if A is to the left of the segment
		const KyUInt32 leftVertexSide = GetSide(a, b, m_vertices[ClampVertexIndex(facingDir + 1)]); // leftVertexSide  == 1 if A to to the right of the segment
		const KyUInt32 exitDirOffset = (rightVertexSide + leftVertexSide * 2 - (rightVertexSide & leftVertexSide)) + 1;

		return ClampVertexIndex(m_entrantDirection + exitDirOffset);
	}

	// Some functions require vertex indices to "wrap" (e.g. 4 => 0 as there is no index '4').
	KY_INLINE KyUInt32 ClampVertexIndex(const KyUInt32 index) const { return index & 3 /* equivalent to % 4 */; }

	CellPos      m_cellPos;
	Vec3f        m_vertices[4];
	CardinalDir  m_entrantDirection;
};

}

#endif //Navigation_HeightField_H