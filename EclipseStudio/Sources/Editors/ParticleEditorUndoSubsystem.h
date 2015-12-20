//=========================================================================
//	Module: ParticleEditorUndoSubsystem.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "Particle.h"
#include "Particle_Int.h"
#include "../../../Eternity/UndoHistory/UndoHistory.h"

//////////////////////////////////////////////////////////////////////////

void InitParticleValueTrackers(r3dParticleData *pd);
void InitParticleEmitterValueTrackers(r3dParticleEmitter &pe);

//////////////////////////////////////////////////////////////////////////

class ParticleEmitterAddDelUndo: public IUndoItem
{
	static const UndoAction_e ms_eActionID = UA_PARTICLE_EMITTER_ADDDEL;
	r3dParticleEmitter **pel;
	r3dParticleEmitter *internalCopy;
	int slotIndex;

	typedef void (ParticleEmitterAddDelUndo::*ActionFunc)();
	ActionFunc undoFunc;
	ActionFunc redoFunc;

	void CreateAction();
	void DeleteAction();

public:
	ParticleEmitterAddDelUndo()
	: pel(0)
	, slotIndex(-1)
	, undoFunc(0)
	, redoFunc(0)
	, internalCopy(0)
	{
	}

	~ParticleEmitterAddDelUndo();

	virtual void			Release			() { delete this; }
	virtual	UndoAction_e	GetActionID		() { return ms_eActionID; }

	virtual void			Undo			();
	virtual void			Redo			();

	void					SetEmitter(r3dParticleEmitter **e, int slotID)
	{
		pel = e;
		slotIndex = slotID;
	}

	void SetAsDeleteUndo();
	void SetAsCreateUndo();

	static IUndoItem * CreateUndoItem	()
	{
		return gfx_new ParticleEmitterAddDelUndo;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

