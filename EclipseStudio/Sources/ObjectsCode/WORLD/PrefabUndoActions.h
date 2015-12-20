//=========================================================================
//	Module: PrefabUndoActions.h
//	Copyright (C) 2013.
//=========================================================================

#pragma once
#include "obj_Prefab.h"
#include "../../../../Eternity/UndoHistory/UndoHistory.h"

//////////////////////////////////////////////////////////////////////////
#ifndef FINAL_BUILD
class CreatePrefabUndo: public IUndoItem
{
	static const UndoAction_e ms_eActionID = UA_PREFAB_CREATE;
	obj_Prefab *prefab;
	r3dVector savedPos;
	r3dVector savedScale;
	r3dVector savedRot;
	r3dString prefabName;
	GameObjects prefabObjects;

public:
	CreatePrefabUndo();

	virtual void			Release			() { delete this; }
	virtual	UndoAction_e	GetActionID		() { return ms_eActionID; }

	virtual void			Undo			();
	virtual void			Redo			();

	void					SetPrefab		(obj_Prefab *p);

	static IUndoItem * CreateUndoItem	()
	{
		return game_new CreatePrefabUndo;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};
#endif
//////////////////////////////////////////////////////////////////////////
