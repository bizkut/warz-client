//=========================================================================
//	Module: CollectionType.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "CollectionsUtils.h"

//////////////////////////////////////////////////////////////////////////

struct CollectionType
{
	r3dVector scale; 
	float renderDist; // max render dist

	float lod1Dist; // dist to swap to lod1
	float lod2Dist; // dist to swap to lod2
	bool physicsEnable;
	bool hasAnimation;
	bool terrainAligned;

#ifndef WO_SERVER
	r3dMesh * meshLOD[COLLECTION_ELEMENT_MAX_LOD_COUNT];
#else
	char serverMeshName[256]; // on server we just need mesh name to load physics info
#endif

	int UIVisible;

	enum { MAX_ANIM_LAYERS = 3 };

	struct AnimLayer
	{
		float Scale ;
		float Freq ;
		float Speed ;
	} AnimLayers [MAX_ANIM_LAYERS];

	float Stiffness;
	float Mass;
	float BendPow;
	float LeafMotionRandomness;
	float Dissipation;
	float BorderDissipation;
	float BorderDissipationStrength;

	/**	Editor related info. Not used in game. */
	struct GroupInfo
	{
		int		Active;
		float	Scale;		
		float	Rotate;	
		float	Spacing;	
		int		ExpandedProperties;

		GroupInfo()
		: Active(0)
		, Scale(0.2f)
		, Rotate(90.0f)
		, Spacing(10.0f)
		, ExpandedProperties(0)
		{}
	};
	GroupInfo groupInfo;

	CollectionType();
	bool InitMeshes(const char *meshName);
	void FreeMeshes();
};
