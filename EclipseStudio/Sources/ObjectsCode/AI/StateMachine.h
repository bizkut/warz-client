//=========================================================================
//	Module: StateMachine.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
//	State class
//-------------------------------------------------------------------------

template <typename EntityType>
class r3dState
{
public:
	virtual void Enter(EntityType *o) {}
	virtual void Execute(EntityType *o) {}
	virtual void Exit(EntityType *o) {}
};

//-------------------------------------------------------------------------
//	General purpose state machine class
//-------------------------------------------------------------------------

template <typename EntityType>
class r3dFiniteStateMachine
{
public:
	typedef r3dState<EntityType> EntityState;

	explicit r3dFiniteStateMachine(EntityType *o)
	: owner(o)
	, currentState(0)
	, prevState(0)
	, globalState(0)
	{}

	void SetCurrentState(EntityState *s)			{ currentState = s; }
	void SetPreviousState(EntityState *s)			{ prevState = s; }
	void SetGlobalState(EntityState *s)				{ globalState = s; }
	EntityState * GetCurrentState()					{ return currentState; }
	EntityState * GetPreviousState()				{ return prevState; }

	/**	Update FSM. */
	void Update()
	{
		if (currentState)
			currentState->Execute(owner);
		if (globalState)
			globalState->Execute(owner);
	}

	/**	Change FSM to new state. */
	void ChangeState(EntityState *newState)
	{
		r3d_assert(newState);
		if (currentState == newState)
			return;

		prevState = currentState;
		if (currentState)
			currentState->Exit(owner);
		currentState = newState;
		currentState->Enter(owner);
	}

	void RevertToPreviousState()
	{
		ChangeState(prevState);
	}

	bool IsInState(const EntityState &st) const
	{
		return &st == currentState;
	}

	bool WasInState(const EntityState &st) const
	{
		return &st == prevState;
	}

private:
	EntityType *owner;
	EntityState *currentState;
	EntityState *prevState;
	EntityState *globalState;
};

