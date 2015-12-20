//=========================================================================
//	Module: CollectionsManager.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

#ifndef WO_SERVER
#include "gameobjects/GenericQuadTree.h"
#endif
#include "CollectionsUtils.h"
#include "CollectionElement.h"
#include "CollectionType.h"
#ifndef WO_SERVER
#include "LevelEditor_Collections.h"
#endif

//////////////////////////////////////////////////////////////////////////

enum CollectionsDrawModeEnum
{
	R3D_IDME_NORMAL,
	R3D_IDME_SHADOW,
	R3D_IDME_DEPTH,
	R3D_IDME_HIGHLIGHT0,
	R3D_IDME_HIGHLIGHT1
};

//////////////////////////////////////////////////////////////////////////

class CollectionsManager
{
public:
	typedef Collections::UtilArray<CollectionElement> Elements;
	typedef Collections::UtilArray<CollectionType> Types;

	/**	Space split parameters for saving. */
	int numSaveXCells;
	int numSaveZCells;

private:

	/**	Collection type array. */
	Types types;

	/**	Collection elements (instances) array. */
	Elements elements;

#ifndef WO_SERVER
	/**	Quad tree for efficient instances culling. */
	QuadTree *quadTree;
	/**	Array with visible objects indices. Filled by ComputeVisiblity function. */
	r3dTL::TArray<int> visibleObjects;
	r3dTL::TArray<int> saved_visibleObjects;
#endif

	r3dPoint3D instanceViewRefPos;
	float prevUpdateTime;

	float animT;

	bool LoadTypesFromXML();
	bool LoadElementsFile(const char *fname, float loadProgressStart, float loadProgressEnd);
	bool LoadElements(float loadProgressStart, float loadProgressEnd);
#ifndef WO_SERVER
	void SaveTypesToXML();
	void RebuildElementTypeIndices();
	bool SaveElements() const;
	void UpdateWind();
#endif


public:
	CollectionsManager();
	~CollectionsManager();
	void Init( float loadProgressStart, float loadProgressEnd );
	void Destroy();
#ifndef WO_SERVER
	void Save();

	/**	Compute visibility here. */
	void ComputeVisibility(bool shadows, bool directionalSM);
	void SaveVisibility();
	void RestoreVisibility();
	void Update();
#endif

	/**	warning: do not memorize this pointer. */
	CollectionType * GetType(int idx);
	uint32_t GetTypesCount() const;
	int CreateNewType();

	/**
	* Not actual delete, but set type entry as free. Immediate delete is not possible, because type indices invalidation.
	* To remove "free" types, call RebuildElementTypeIndices()
	*/
	void DeleteType(int idx);

	/**	warning: do not memorize this pointer. */
	CollectionElement * GetElement(int idx);
	uint32_t GetElementsCount() const;
	int CreateNewElement();
	void DeleteElement(int idx);

	/**
	* Update object placement in quadtree. This function should be called after CollectionElement position change.
	* @return True if update was successful, and object reside in appropriate place.
	*/
	bool UpdateElementQuadTreePlacement(int idx);

#ifndef WO_SERVER
	/**	Get all collection elements in given radius. */
	void GetElementsInRadius(const r3dPoint2D &center, float r, r3dTL::TArray<int> &objIndices) const;

	/**	Get collection elements along given ray. */
	void GetElementsAlongRay(const r3dPoint3D &org, const r3dPoint3D &dir, r3dTL::TArray<int> &objIndices) const;
#endif

	void SetInstanceViewRefPos(const r3dPoint3D& pos);

#ifndef WO_SERVER
	/** Collect all used materials. */
	void GetUsedMaterials(r3dgfxVector(r3dMaterial*)& materials);

	void Render(CollectionsDrawModeEnum drawMode);
	void DEBUG_Draw();
#endif
};

//////////////////////////////////////////////////////////////////////////

extern CollectionsManager gCollectionsManager;
#ifndef WO_SERVER
void DoneDrawCollections();
#endif

//////////////////////////////////////////////////////////////////////////

