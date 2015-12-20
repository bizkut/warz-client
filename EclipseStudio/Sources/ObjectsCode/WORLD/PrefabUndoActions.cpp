//=========================================================================
//	Module: PrefabUndoActions.cpp
//	Copyright (C) 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "obj_Prefab.h"
#include "PrefabUndoActions.h"
#include "gameobjects/ObjManag.h"

//////////////////////////////////////////////////////////////////////////
#ifndef FINAL_BUILD
CreatePrefabUndo::CreatePrefabUndo()
: prefab(0)
{
	m_sTitle = "Create Prefab";
}

//////////////////////////////////////////////////////////////////////////

void CreatePrefabUndo::Undo()
{
	if (!prefab)
		return;

	savedRot = prefab->GetRotationVector();
	savedScale = prefab->GetScale();
	savedPos = prefab->GetPosition();
	prefabName = prefab->Name;

	prefabObjects = prefab->gameObjects;

	prefab->BreakSelf();

	prefab = 0;
}

//////////////////////////////////////////////////////////////////////////

void CreatePrefabUndo::Redo()
{
	for (uint32_t i = 0; i < prefabObjects.Count(); ++i)
	{
		GameObject *go = prefabObjects[i];
		GameWorld().DeleteObject(go);
	}
	prefabObjects.Clear();

	obj_Prefab *p = g_PrefabManager.InstantiatePrefab(prefabName.c_str(), savedPos);
	p->SetScale(savedScale);
	p->SetRotationVector(savedRot);
	prefab = p;
}

//////////////////////////////////////////////////////////////////////////

void CreatePrefabUndo::SetPrefab(obj_Prefab *p)
{
	prefab = p;
}
#endif