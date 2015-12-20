//=========================================================================
//	Module: DecalChiefUndoRedoActions.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "../../../../Eternity/UndoHistory/UndoHistory.h"
#include "DecalChief.h"

//-------------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------------

class DecalUndoAdd: public IUndoItem
{
	static const UndoAction_e ms_eActionID = UA_DECAL_ADD;
	DecalParams decalParms;

public:
	DecalUndoAdd();

	virtual void			Release			() { delete this; }
	virtual	UndoAction_e	GetActionID		() { return ms_eActionID; }

	virtual void			Undo			();
	virtual void			Redo			();

	void					SetDecalParams	(const DecalParams &p)
	{
		decalParms = p;
	}

	static IUndoItem * CreateUndoItem	()
	{
		return game_new DecalUndoAdd;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

//-------------------------------------------------------------------------
//	Del
//-------------------------------------------------------------------------

class DecalUndoDel: public IUndoItem
{
	static const UndoAction_e ms_eActionID = UA_DECAL_DEL;
	DecalParams decalParms;

public:
	DecalUndoDel();

	virtual void			Release			() { delete this; }
	virtual	UndoAction_e	GetActionID		() { return ms_eActionID; }

	virtual void			Undo			();
	virtual void			Redo			();

	void					SetDecalParams	(const DecalParams &p)
	{
		decalParms = p;
	}

	static IUndoItem * CreateUndoItem	()
	{
		return game_new DecalUndoDel;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};
