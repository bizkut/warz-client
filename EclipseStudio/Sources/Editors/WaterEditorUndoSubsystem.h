//=========================================================================
//	Module: WaterEditorUndoSubsystem.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

#include "../../../Eternity/UndoHistory/UndoHistory.h"
#include "../ObjectsCode/WORLD/WaterPlane.H"

//////////////////////////////////////////////////////////////////////////

void InitWaterValueTrackers(obj_WaterPlane *o);
void WaterCellsTrackUndoStart(obj_WaterPlane *o);
void WaterCellsTrackUndoEnd(obj_WaterPlane *o);

//////////////////////////////////////////////////////////////////////////

class WaterGridChangeUndo: public IUndoItem
{
	static const UndoAction_e ms_eActionID = UA_WATER_GRID_CHANGE;
	obj_WaterPlane::GridVecType savedVal;
	obj_WaterPlane *obj;

public:
	WaterGridChangeUndo()
	: obj(0)
	{
		m_sTitle = "Water cells change";
	}
	~WaterGridChangeUndo(){};

	void SetWaterObject(obj_WaterPlane *o, obj_WaterPlane::GridVecType &gridState)
	{
		if (o)
		{
			obj = o;
			savedVal = gridState;
		}
		SetTag(reinterpret_cast<uint32_t>(o));
	}

	virtual void			Release			() { delete this; }
	virtual	UndoAction_e	GetActionID		() { return ms_eActionID; }

	virtual void			Undo			();
	virtual void			Redo			();

	static IUndoItem * CreateUndoItem	()
	{
		return game_new WaterGridChangeUndo;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

struct WaterPlaneSettingsTracker;

class WatterPlaneSettingsUndo: public IUndoItem
{
	obj_WaterPlane::Settings prevValue;
	obj_WaterPlane::Settings curValue;
	obj_WaterPlane *waterPlane;

	static const UndoAction_e ms_eActionID = UA_WATERPLANE_SETTINGS_CHANGE;

public:
	WatterPlaneSettingsUndo();

	void			Release			();
	UndoAction_e	GetActionID		();

	void			Undo			();
	void			Redo			();

	void			SetValues( const obj_WaterPlane::Settings& oldV, const obj_WaterPlane::Settings& newV );
	void			SetWaterPlane( obj_WaterPlane *plane );

	static IUndoItem * CreateUndoItem	();
	static void Register();
};
