//=========================================================================
//	Module: CollectionType.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "CollectionType.h"

//////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
//	CollectionType class
//-------------------------------------------------------------------------

CollectionType::CollectionType()
: scale(1, 1, 1)
, renderDist(0)
, lod1Dist(0)
, lod2Dist(0)
, physicsEnable(false)
, hasAnimation(false)
, UIVisible(0)
, Stiffness(32.0f)
, Mass(2.0f)
, BendPow(1.0f)
, LeafMotionRandomness(1.0f)
, Dissipation(0.5f)
, BorderDissipation(0.5f)
, BorderDissipationStrength(24.0f)
, terrainAligned(false)
{
	for (int i = 0, e = MAX_ANIM_LAYERS; i < e; i ++)
	{
		AnimLayers[ i ].Scale	= 0 ;
		AnimLayers[ i ].Freq	= 0 ;
		AnimLayers[ i ].Speed	= 0 ;
	}

#ifndef WO_SERVER
	for (uint32_t i = 0; i < _countof(meshLOD); ++i)
	{
		meshLOD[i] = 0;
	}
#else
	serverMeshName[0] = 0;
#endif
}

//////////////////////////////////////////////////////////////////////////

bool CollectionType::InitMeshes(const char *meshName)
{
	if (!meshName)
		return false;
#ifdef WO_SERVER
	r3dscpy(serverMeshName, meshName);
	return true;
#else
	uint32_t lodIdx = 0;
	const uint32_t totalLodsToCheck = 4;
	const char *const lodSuffixes[totalLodsToCheck] = { "%s.sco", "%s_LOD1.sco", "%s_LOD2.sco", "%s_LOD3.sco" };

	char filename[MAX_PATH];
	strcpy_s(filename, _countof(filename), meshName);
	filename[strlen(filename) - 4] = 0;
	char TempLodName[MAX_PATH];

	for (uint32_t i = 0; i < totalLodsToCheck && lodIdx < COLLECTION_ELEMENT_MAX_LOD_COUNT; ++i)
	{
		sprintf(TempLodName, lodSuffixes[i], filename);

		char tempLodNameScb[MAX_PATH] = {0};
		r3dscpy(tempLodNameScb, TempLodName);
		if(strlen(tempLodNameScb)>5)
			tempLodNameScb[strlen(tempLodNameScb)-1] = 'b';

		if (r3d_access(TempLodName, 0) == 0 || r3d_access(tempLodNameScb, 0) == 0)
		{
			r3dMesh * m = gfx_new r3dMesh(0);
			m->Load(TempLodName);
			m->RecalcBoundBox();
			m->FillBuffers();
			meshLOD[lodIdx++] = m;
		}
		else 
		{
			if (i == 0) ++lodIdx;
		}

	}

	if (!meshLOD[0])
		return false;

	groupInfo.Spacing = meshLOD[0]->localBBox.Size.X * 2;
	if (groupInfo.Spacing < 5)
		groupInfo.Spacing = 5;

	return true;
#endif
}

//////////////////////////////////////////////////////////////////////////

void CollectionType::FreeMeshes()
{
#ifndef WO_SERVER
	for (uint32_t i = 0; i < _countof(meshLOD); ++i)
	{
		if (meshLOD[i])
		{
			meshLOD[i]->Unload();
			meshLOD[i] = 0;
		}
	}
#endif
}


