//=========================================================================
//	Module: CamouflageDataManager.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "Tsg_stl/HashTable.h"
#include "../../GameEngine/gameobjects/GameObj.h"

//////////////////////////////////////////////////////////////////////////

/**	Class that manages camouflage textures for meshes with camouflage material. */
class CamouflageDataManager
{
public:
	CamouflageDataManager();
	
	/**	Update camouflage data for object through ray casting. */
	void UpdateCamouflageData(const GameObject &o);

	/**	Get camouflage data for object. */
	r3dMaterial::CamoData GetCamouflageData(const GameObject &o);

	/**	Set current camouflage data. This means that next call of GetCurrentData will retrieve data for object passed as o parameter. */
	void SetCurrentData(const GameObject &o);

	/**	Get current camouflage data. Current data is set by SetCurrentData function. */
	const r3dMaterial::CamoData & GetCurrentData() const { return currData; }

private:
	struct PassThroughHasher
	{
		uint32_t operator ()(uint32_t h) const { return h; }
	};

	/**	Storage of camouflage data structure for objects. */
	typedef HashTableDynamic<r3dMaterial::CamoData, uint32_t, PassThroughHasher, 256> GameObjectCamoData;
	GameObjectCamoData dataStorage;

	/**	Current data holder. */
	r3dMaterial::CamoData currData;
};

extern CamouflageDataManager gCamouflageDataManager;

//////////////////////////////////////////////////////////////////////////

//	Transparent camo fader time line class
class CamoTimeLine
{
	float transparentCamoAlphaTarget;

public:
	float currentTimeSpot;
	float transparentCamoAlpha;
	float camoMasterTransparency;
	float lastCamoTime;
	const float fadeTime;
	const float transTime;
	float transparencyOnShock;

	CamoTimeLine();

	float GetTarget() { return transparentCamoAlphaTarget ; }

	void Update();
	void SetNewCamoTarget(float t);
};
