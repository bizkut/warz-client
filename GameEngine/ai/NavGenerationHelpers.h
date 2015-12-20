//=========================================================================
//	Module: NavGenerationHelpers.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

#include "NavRegionManager.h"

//////////////////////////////////////////////////////////////////////////

namespace Nav
{
	/**	This structure describe entire level geometry. */
	struct LevelGeometry
	{
		struct Mesh
		{
			typedef r3dTL::TArray<r3dPoint3D> VertArr;
			typedef r3dTL::TArray<int> IndexArr;
			typedef r3dTL::TArray<byte> WalkableInfo;

			VertArr vertices;
			IndexArr indices;
			WalkableInfo walkableTris;

		};
		typedef r3dTL::TArray<Mesh> MeshesArr;
		MeshesArr meshes;
		uint32_t meshStartObjectsIdx;

		LevelGeometry() : meshStartObjectsIdx(0) {}
	};

	//	For caching purposes
	extern LevelGeometry *gLevelMesh;
	void CreateTerrainGeomtry(LevelGeometry &lg);
	void CreateAllGameObjectsAndCollections(LevelGeometry &lg);
	void ResetCachedLevelGeometry();
	void RemoveAllGameObjectsAndCollections(LevelGeometry &lg);
	void ExportGeometryToObjFormat(LevelGeometry &lg, const r3dString &outFilePath);
	void PopulateLevelMesh(LevelGeometry *&lg);
	r3dString GetLevelObjFilePath();
	r3dString GetNavDataFolderPath();

//////////////////////////////////////////////////////////////////////////

	struct DrawBuildInfo
	{
		CRITICAL_SECTION buildInfoStringGuard;
		char buildInfoString[256];

		DrawBuildInfo();
		~DrawBuildInfo();

		void Draw();
	};
	extern DrawBuildInfo gDrawBuildInfo;

	extern LONG gNavMeshBuildComplete;
}

