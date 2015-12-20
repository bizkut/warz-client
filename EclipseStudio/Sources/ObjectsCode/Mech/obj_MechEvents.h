//=========================================================================
//	Module: obj_MechEvents.h
//	Copyright (C) 2013.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

class obj_ParticleSystem;
class obj_Mech;

//////////////////////////////////////////////////////////////////////////

namespace MechFXEvents
{

struct ActionDesc
{
	r3dSTLString name;
	r3dSTLString particleFXName;
	float playDelay;
	float playLength;

	ActionDesc()
	: playDelay(0)
	, playLength(0)
	{}

	ActionDesc(const ActionDesc &o)
	: playDelay(o.playDelay)
	, playLength(o.playLength)
	, name(o.name)
	, particleFXName(o.particleFXName)
	{}
};

//////////////////////////////////////////////////////////////////////////

struct Action: public ActionDesc
{
	obj_ParticleSystem *particleFX;
	int boneIdx;
	bool isExpanded;
	r3dVector positionOffset;
	r3dVector localRotationAngles;
	float scale;

	float timeAtStart;

	Action()
	: isExpanded(false)
	, particleFX(0)
	, boneIdx(-1)
	, timeAtStart(0)
	, positionOffset(0, 0, 0)
	, localRotationAngles(0, 0, 0)
	, scale(1.0f)
	{}

	explicit Action(const ActionDesc &ad)
	: ActionDesc(ad)
	, isExpanded(false)
	, particleFX(0)
	, boneIdx(-1)
	, timeAtStart(0)
	, positionOffset(0, 0, 0)
	, localRotationAngles(0, 0, 0)
	, scale(1.0f)
	{}

	~Action();
};

//////////////////////////////////////////////////////////////////////////

enum Events
{
	EVENT_FireStart,
	EVENT_FireEnd,
	EVENT_WalkStart,
	EVENT_WalkEnd,
	EVENT_Death,
	EVENT_Landing,
	EVENT_Bump,
	EVENT_Step,
	EVENT_StartUp,
	EVENT_Hit,
	EVENT_Total
};

//////////////////////////////////////////////////////////////////////////

class EventSystem
{
	obj_Mech *owner;

	bool isEventsExpanded[EVENT_Total];

	typedef r3dTL::TArray<ActionDesc> ActionsLib;
	ActionsLib actionsLibrary;
	r3dTL::TArray<bool> editorActionExpanded;

	typedef r3dTL::TArray<Action> EventActions;
	typedef r3dTL::TArray<EventActions> EventArr;
	EventArr eventsAndActions;
	r3dTL::TArray<r3dTL::TArray<bool> > editorEventsExpanded;

	/** Editor related. */
	enum EditorMode editorMode;
	stringlist_t boneNames;
	int *boneIndexToSet;
	int PickBoneEditor(float scrx, float &scry);

	stringlist_t particleNames;
	r3dSTLString *particleToSet;
	int PickParticleEditor(float scrx, float &scry);

	Action *actionToPick;
	int PickActionsEditor(float scrx, float &scry);
	int PickActionFromLibraryEditor(float scrx, float &scry);

	void LoadActionsLibrary();
	void LoadActionsLibraryFromXML(pugi::xml_node &rootNode);
	void SaveActionsLibrary();
	void SaveActionsLibraryToXML(pugi::xml_node &rootNode);

	bool CreateActionByName(const r3dSTLString &name, Action &outAction) const;

	float DrawActionsEditor(float scrx, float scry, float scrw, float scrh);
	float DrawEventsEditor(float scrx, float scry, float scrw, float scrh);

	void GetParticleWorldPose(const Action &a, D3DXMATRIX &outPose);

public:
	EventSystem();
	~EventSystem();
	void Init(obj_Mech *owner_);
	void Clear();
	void LoadFromXML(pugi::xml_node &node);
	void SaveToXML(pugi::xml_node &node);
	void OnEvent(Events ev, bool forceRestart = false);
	float DrawEditor(float scrx, float scry, float scrw, float scrh);
	void Update();
	void DrawDebug();
};
}

