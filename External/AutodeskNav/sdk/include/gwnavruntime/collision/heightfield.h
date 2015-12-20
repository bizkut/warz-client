/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_HeightField_H
#define Navigation_HeightField_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/math/triangle3f.h"
#include "gwnavruntime/collision/heightfieldblob.h"
#include "gwnavruntime/blob/blobaggregate.h"
#include "gwnavruntime/math/box2f.h"
#include "gwnavruntime/collision/collisiontypes.h"
#include "gwnavruntime/collision/heightfieldtile.h"

namespace Kaim
{

class World;
class FileOpenerBase;
class IndexedMesh;
class HeightFieldBlob;

typedef Vec2i HeightfieldTilePos;     ///< A type that represents the position of a Tile within a 2D grid.
typedef Box2i HeightfieldTileBox;     ///< A type that represents a bounding box around the Tiles of the Heigtfield.

typedef Vec2i HeightfieldVertexPos;   ///< A type that represents one of the vertices of a Tile within a 2D grid.
typedef Box2i HeightfieldVertexBox;   ///< A type that represents a bounding box around the Vertices of the Heighfield.

struct HeightFieldConfig
{
	HeightFieldConfig() : m_xAltitudeCount(0), m_yAltitudeCount(0), m_tileSize(0.f) {}

	KyUInt32  m_xAltitudeCount;
	KyUInt32  m_yAltitudeCount;
	KyFloat32 m_tileSize;
	Vec2f     m_origin;
};

/// Heightfield with a uniform grid of sampled altitudes.  Each tile can be triangulated into two triangles.
/// Memory & CPU efficient way of representing a terrain.
/// Supports RayCasting.  However, do not call the RayCast function directly.  Instead you should use a CollisionRayCastQuery.
/// In this way, all the collision objects will be properly queried and you can combine HeightFields and IndexedMeshes for an 
/// optimal representation of the world in terms of Collision.

class HeightField : public RefCountBase<HeightField, MemStat_CollisionData>
{
	KY_CLASS_WITHOUT_COPY(HeightField)
public:
	HeightField() : m_blob(KY_NULL), m_altitudes(KY_NULL), m_tileSizeInv(1.f) {}
	~HeightField() { Clear(); }

	KyResult Initialize(const HeightFieldConfig& config);
	KyResult InitFromBlob(HeightFieldBlob* blob);
	KyResult InitFromBlobHandler(Ptr<BlobHandler<HeightFieldBlob> > handler);
	void Clear();

	// RayCast on this heightfield.  Returns RayHit if any triangles are intersected.
	CollisionRayCastResult RayCast(const Vec3f& startPoint, const Vec3f& endPoint) const;

	KY_INLINE KyUInt32 GetXTileCount() const { return m_tileBox.CountX(); }
	KY_INLINE KyUInt32 GetYTileCount() const { return m_tileBox.CountY(); }
	KY_INLINE KyUInt32 GetTileCount()  const { return GetTileCount() * GetYTileCount(); }

	KY_INLINE KyUInt32 GetXAltitudeCount() const { return m_vertexBox.CountX(); }
	KY_INLINE KyUInt32 GetYAltitudeCount() const { return m_vertexBox.CountY(); }
	KY_INLINE KyUInt32 GetAltitudeCount()  const { return GetTileCount() * GetYTileCount(); }

	KY_INLINE KyFloat32 GetXExtents() const { return GetXTileCount() * m_blob->m_tileSize; }
	KY_INLINE KyFloat32 GetYExtents() const { return GetYTileCount() * m_blob->m_tileSize; }

	KY_INLINE void SetAltitude(const HeightfieldVertexPos& pixelPos, KyFloat32 altitude)
	{
		m_altitudes[m_vertexBox.GetRowMajorIndex(pixelPos)] = altitude;
	}

	KY_INLINE Vec3f GetVertex(const HeightfieldVertexPos& vertexPos) const
	{
		const KyFloat32 altitude = GetAltitude(vertexPos);
		return m_blob->m_origin + Vec3f((KyFloat32)vertexPos.x * m_blob->m_tileSize, (KyFloat32)vertexPos.y * m_blob->m_tileSize, altitude);
	}

	KY_INLINE KyResult GetTileAtPos(const Vec3f& pos, HeightFieldTile& tile) const
	{
		// Transform pos, snap it to grid.
		const Vec3f localPos = pos - m_blob->m_origin;
		const CellPos cellPos(SnapFloat(localPos.x), SnapFloat(localPos.y));
		return GetTileAtPos(cellPos, tile);
	}

	KY_INLINE KyResult GetTileAtPos(const HeightfieldTilePos& pixelPos, HeightFieldTile& tile) const
	{
		if (m_tileBox.IsInside(pixelPos) == false)
			return KY_ERROR;

		const KyFloat32 southEastX = ((KyFloat32)pixelPos.x * m_blob->m_tileSize) + m_blob->m_origin.x + m_blob->m_tileSize;
		const KyFloat32 southEastY = ((KyFloat32)pixelPos.y * m_blob->m_tileSize) + m_blob->m_origin.y;

		tile.m_vertices[0].x = southEastX;
		tile.m_vertices[0].y = southEastY;
		tile.m_vertices[0].z = GetAltitude(pixelPos.NeighborEast());

		tile.m_vertices[1].x = southEastX;
		tile.m_vertices[1].y = southEastY + m_blob->m_tileSize;
		tile.m_vertices[1].z = GetAltitude(pixelPos.NeighborNorthEast());

		tile.m_vertices[2].x = southEastX - m_blob->m_tileSize;
		tile.m_vertices[2].y = southEastY + m_blob->m_tileSize;
		tile.m_vertices[2].z = GetAltitude(pixelPos.NeighborNorth());

		tile.m_vertices[3].x = southEastX - m_blob->m_tileSize;
		tile.m_vertices[3].y = southEastY;
		tile.m_vertices[3].z = GetAltitude(pixelPos);
		
		tile.m_cellPos = pixelPos;

		return KY_SUCCESS;
	}

	Box2f GetAABB2D() const
	{
		return Box2f(m_blob->m_origin.Get2d(), m_blob->m_origin.Get2d() + 
			Vec2f(m_blob->m_xAltitudeCount * m_blob->m_tileSize, m_blob->m_yAltitudeCount * m_blob->m_tileSize));
	}

	void VisualDebug(World* world);

	KyResult ConvertToIndexedMesh(IndexedMesh& mesh) const;

	KyResult WriteToObj(File* file) const;

	BlobHandler<HeightFieldBlob>* GetBlobHandler() { return m_blobHandler; }

private:

	KY_INLINE KyFloat32 GetAltitude(const HeightfieldVertexPos& vertexPos) const
	{
		return m_altitudes[m_vertexBox.GetRowMajorIndex(vertexPos)];
	}

	KyResult GetStartTile(const Vec3f& startPoint, const Vec3f& endPoint, HeightFieldTile& tile) const;
	KyResult GetNextTile(const HeightFieldTile& tile, const Vec2f& rayA, const Vec2f& rayB, HeightFieldTile& outTile, const HeightFieldTile* destTile = KY_NULL) const;
	CollisionRayCastResult RayVsTriangle(const Triangle3f& tri, const Vec3f& from, const Vec3f& to) const;

	KY_INLINE KyUInt32 SnapFloat(KyFloat32 inputValue) const
	{
		return KyUInt32(inputValue * m_tileSizeInv);
	}

	KY_INLINE Vec3f Interpolate(const Vec3f& v0, const Vec3f& v1, KyFloat32 rt) const
	{
		const KyFloat32 s = 1.0f - rt;
		return Vec3f(s * v0[0] + rt * v1[0],
			         s * v0[1] + rt * v1[1],
			         s * v0[2] + rt * v1[2]);
	}

	void OnSetBlob();

	Ptr<BlobHandler<HeightFieldBlob> > m_blobHandler;
	HeightFieldBlob*                   m_blob;
	KyFloat32*                         m_altitudes; // Shortcut
	KyFloat32                          m_tileSizeInv;
	HeightfieldTileBox                 m_tileBox;
	HeightfieldVertexBox               m_vertexBox;
	Vec3f                              m_verts[4];
};

}

#endif // Navigation_HeightField_H