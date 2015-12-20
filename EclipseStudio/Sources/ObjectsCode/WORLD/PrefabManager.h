//=========================================================================
//	Module: PrefabManager.h
//	Copyright (C) 2013.
//=========================================================================

#pragma once
#include "r3dAllocatorInterface.h"
#include "r3dString.h"
#include "obj_Prefab.h"

//////////////////////////////////////////////////////////////////////////

class PrefabManager
{
	typedef r3dgameMap(r3dSTLString, pugi::xml_document*) Prefabs;
	Prefabs prefabs;

	void RefreshPrefabObjects(const r3dSTLString &name, const pugi::xml_node &n);
	bool LoadPrefab(const r3dSTLString &name);
	void DeleteAllPrefabs();

public:
	PrefabManager();
	~PrefabManager();

	void CreatePrefab(const GameObjects &gs, const r3dSTLString &name, const obj_Prefab *rootObj);
	void DeletePrefab(const r3dSTLString &name);
	void FillNameList(stringlist_t &outNames);
	pugi::xml_node GetPrefabDescription(const r3dSTLString &name);
	obj_Prefab * InstantiatePrefab(const r3dSTLString &name, const r3dVector &pos);

	bool LoadAllPrefabs();
	void SavePrefabs();
	void ValidatePrefabs();

	void BreakAllPrefabs();
};

//////////////////////////////////////////////////////////////////////////

void CreateXMLDescription(pugi::xml_node &rootNode, const GameObjects &gameObjects, const obj_Prefab *rootObj);

//////////////////////////////////////////////////////////////////////////

extern PrefabManager g_PrefabManager;
