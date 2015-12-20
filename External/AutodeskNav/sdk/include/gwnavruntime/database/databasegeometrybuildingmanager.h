/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: LAPA

#ifndef Navigation_DatabaseVisRepGrid_H
#define Navigation_DatabaseVisRepGrid_H

#include "gwnavruntime/visualsystem/ivisualgeometry.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/database/databasevisualgeometrybuilder.h"

namespace Kaim
{

class DatabaseGeometryBuildingManager
{
	class DatabaseVisualTile
	{
	public:
		DatabaseVisualTile() : m_needToBeRebuild(false) {}
		~DatabaseVisualTile() {}

		Ptr<IVisualGeometry> m_visualGeometry;
		CellBox m_cellBox;
		bool m_needToBeRebuild;
	};

public:
	DatabaseGeometryBuildingManager();
	~DatabaseGeometryBuildingManager();

	void Clear();


	void BuildGeometry(bool forceRebuildAll = false);


	// Modify the VisualGeometryBuildConfig
	void SetAltOffSet(KyFloat32 altOffset);
	void SetDetailLevel(VisualGeometryDetailLevel detailLevel);

	VisualGeometryDetailLevel GetDetailLevel() const;

	// no tiling
	void SetVisualGeometry(Ptr<IVisualGeometry> geometry);
	IVisualGeometry* GetVisualGeometry() const;

	// tilling is used
	void SetVisualGeometryFactory(Ptr<IVisualGeometryFactory> factory);
	IVisualGeometryFactory* GetVisualGeometryFactory()  const;

	// m_tileSize can be changed through this accessor only when the database is clear
	// once at least one navdata has been added, it cannot be changed anymore until the Database is cleared again.
	void SetTileSize(KyInt32 tileSize);

public: // Internal
	void OnEnlarge(const CellBox& cellBox);
	void OnChangeAtPos(const CellPos& cellPos);
	void OnChangeInGraph();
	
	KyInt32 ComputeTileCoordFromCellCoord(KyInt32 coord) const;
	Vec2i ComputeTilePosFromCellPos(const CellPos& cellPos) const;
	Box2i ComputeTileBoxFromCellBox(const CellBox& cellBox) const;
	CellBox ComputeCellBoxForTilePos(const Vec2i& tilePos) const;

private:
	friend class Database;
	void OnEnlarge_FactoryProvided(const CellBox& newCellBox);
	void OnChangeAtPos_FactoryProvided(const CellPos& cellPos);
	void OnChangeInGraph_FactoryProvided();

	void AskNewGeometryForTileIfNeeded(DatabaseVisualTile& tile);
private:
	Database* m_database;
	KyUInt32 m_databaseIdx;

	VisualGeometryBuildConfig m_buildConfig; // to be used in all tiles

	KyInt32 m_tileSize; // number of cell on x/y axis per tile

	// not a CellBox, a tileBox !
	// tile at pos (0, 0) has cellBox [(0         ;         0) (m_tileSize-1  ;m_tileSize-1)]
	// tile at pos (1, 0) has cellBox [(m_tileSize;         0) (2*m_tileSize-1;m_tileSize-1)]
	// tile at pos (0,-1) has cellBox [(0         ;m_tileSize) (m_tileSize-1  ;          -1)]
	Box2i m_tileBox;
	// array of visRep associated to m_boxOfTile. Access it through RawMajorIdx
	DatabaseVisualTile* m_bufferOfTile; 
	KyUInt32 m_bufferSize;

	DatabaseVisualTile m_tileForGraphs;

	Ptr<IVisualGeometryFactory> m_geometryFactory;

	// Used if provided AND if no geometryFactory provided
	// no tile mode
	Ptr<IVisualGeometry> m_visualGeometry;
};

KY_INLINE IVisualGeometryFactory*   DatabaseGeometryBuildingManager::GetVisualGeometryFactory() const { return m_geometryFactory;           }
KY_INLINE IVisualGeometry*          DatabaseGeometryBuildingManager::GetVisualGeometry()        const { return m_visualGeometry;            }
KY_INLINE VisualGeometryDetailLevel DatabaseGeometryBuildingManager::GetDetailLevel()           const { return m_buildConfig.m_detailLevel; }

KY_INLINE void DatabaseGeometryBuildingManager::SetAltOffSet(KyFloat32 altOffset) { m_buildConfig.m_altitudeOffset = altOffset; }
KY_INLINE void DatabaseGeometryBuildingManager::SetDetailLevel(VisualGeometryDetailLevel detailLevel) { m_buildConfig.m_detailLevel = detailLevel; }

KY_INLINE KyInt32 DatabaseGeometryBuildingManager::ComputeTileCoordFromCellCoord(KyInt32 coord) const
{
	return coord >= 0 ? coord / m_tileSize : ((coord + 1) / m_tileSize) - 1;
}

KY_INLINE Vec2i DatabaseGeometryBuildingManager::ComputeTilePosFromCellPos(const CellPos& cellPos) const
{
	return Vec2i(ComputeTileCoordFromCellCoord(cellPos.x), ComputeTileCoordFromCellCoord(cellPos.y));
}

KY_INLINE Box2i DatabaseGeometryBuildingManager::ComputeTileBoxFromCellBox(const CellBox& cellBox) const
{
	return Box2i(ComputeTilePosFromCellPos(cellBox.Min()), ComputeTilePosFromCellPos(cellBox.Max()));
}

KY_INLINE CellBox DatabaseGeometryBuildingManager::ComputeCellBoxForTilePos(const Vec2i& tilePos) const
{
	const CellPos minCellPos(tilePos.x * m_tileSize, tilePos.y * m_tileSize);
	return CellBox(minCellPos, minCellPos + CellPos(m_tileSize - 1, m_tileSize - 1));
}

KY_INLINE void DatabaseGeometryBuildingManager::OnEnlarge(const CellBox& cellBox)
{
	if (m_geometryFactory != KY_NULL)
		OnEnlarge_FactoryProvided(cellBox);
}
KY_INLINE void DatabaseGeometryBuildingManager::OnChangeAtPos(const CellPos& cellPos)
{
	if (m_geometryFactory != KY_NULL)
		OnChangeAtPos_FactoryProvided(cellPos);
}
KY_INLINE void DatabaseGeometryBuildingManager::OnChangeInGraph()
{
	if (m_geometryFactory != KY_NULL)
		AskNewGeometryForTileIfNeeded(m_tileForGraphs);
}

KY_INLINE void DatabaseGeometryBuildingManager::AskNewGeometryForTileIfNeeded(DatabaseVisualTile& tile)
{
	tile.m_needToBeRebuild = true;
	if (tile.m_visualGeometry == KY_NULL)
	{
		tile.m_visualGeometry = m_geometryFactory->CreateIVisualGeometryForDatabase(m_databaseIdx);
		KY_LOG_ERROR_IF(tile.m_visualGeometry == KY_NULL,
			("Failed to create an IVisualGeometry from the IVisualGeometryFactory. A portion of the Database won't be displayed."));
	}
}

} // namespace Kaim

#endif // Navigation_DatabaseVisRepGrid_H
