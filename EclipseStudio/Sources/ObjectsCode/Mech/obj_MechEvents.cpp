
//	Module: obj_MechEvents.cpp
//	Copyright (C) 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "obj_MechEvents.h"
#include "obj_Mech.h"
#include "GameCommon.h"
#include "..\EFFECTS\obj_ParticleSystem.H"
#include "Particle.h"
#include "XMLHelpers.h"
#include <Shlwapi.h>

//////////////////////////////////////////////////////////////////////////

namespace
{
	const char * const ACTIONS_LIB_PATH = "Data/ObjectsDepot/Mechs/Effects/mech_effects_lib.xml";
	r3dTexture * EffectIcon = 0;
	int gShowEffectLocations = false;
}
void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);

//////////////////////////////////////////////////////////////////////////

namespace MechFXEvents
{

enum EditorMode
{
	EDITORMODE_EVENTS,
	EDITORMODE_BONE_PICK,
	EDITORMODE_PARTICLE_PICK,
	EDITORMODE_ACTIONS_PICK,
	EDITORMODE_ACTION_FROM_LIB_PICK,
};

//////////////////////////////////////////////////////////////////////////

Action::~Action()
{
	if (particleFX)
		GameWorld().DeleteObject(particleFX);
}

//////////////////////////////////////////////////////////////////////////

EventSystem::EventSystem()
: owner(0)
, boneIndexToSet(0)
, particleToSet(0)
, editorMode(EDITORMODE_EVENTS)
{

}

//////////////////////////////////////////////////////////////////////////

EventSystem::~EventSystem()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::Init(obj_Mech *owner_)
{
	Clear();
	owner = owner_;
	for (uint32_t i = 0; i < _countof(isEventsExpanded); ++i)
	{
		isEventsExpanded[i] = false;
	}

	eventsAndActions.Resize(EVENT_Total);
	editorEventsExpanded.Resize(EVENT_Total);
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::Clear()
{
	owner = 0;
	actionsLibrary.Clear();

	for (uint32_t i = 0; i < eventsAndActions.Count(); ++i)
	{
		EventActions &ea = eventsAndActions[i];
		for (uint32_t j = 0; j < ea.Count(); ++j)
		{
			Action &a = ea[j];
			if (a.particleFX)
				GameWorld().DeleteObject(a.particleFX);
		}
	}
	eventsAndActions.Clear();
}

//////////////////////////////////////////////////////////////////////////

float EventSystem::DrawEditor(float scrx, float scry, float scrw, float scrh)
{
	float starty = scry;

	typedef int (EventSystem::*PickFunc)(float scrx, float &scry);
	PickFunc pickFuncs[EVENT_Total] =
	{
		0,
		&EventSystem::PickBoneEditor,
		&EventSystem::PickParticleEditor,
		&EventSystem::PickActionsEditor,
		&EventSystem::PickActionFromLibraryEditor,
	};

	PickFunc editorPickDialog = pickFuncs[editorMode];

	if (editorPickDialog)
	{
		int rv = (this->*editorPickDialog)(scrx, starty);
		if (rv >= 0)
		{
			editorMode = EDITORMODE_EVENTS;
		}
		return starty - scry;
	}

	static int idx = 0;
	static const char * toolsList[] =
	{
		"Effects Library",
		"Events Setup"
	};
	starty += imgui_Toolbar(scrx, starty, 360.0f, 22.0f, &idx, 0, toolsList, _countof(toolsList));

	switch (idx)
	{
	case 0:
		starty += DrawActionsEditor(scrx, starty, scrw, scrh);
		break;
	case 1:
		starty += DrawEventsEditor(scrx, starty, scrw, scrh);
		break;
	}

	return starty - scry;
}

//////////////////////////////////////////////////////////////////////////

float EventSystem::DrawActionsEditor(float scrx, float scry, float scrw, float scrh)
{
	//	Show actions
	float starty = scry;
	uint32_t toDel = std::numeric_limits<uint32_t>::max();

	editorActionExpanded.Resize(actionsLibrary.Count());

	for (uint32_t i = 0; i < actionsLibrary.Count(); ++i)
	{
		ActionDesc & a = actionsLibrary[i];
		bool &isExpanded = editorActionExpanded[i];

		const char *expSym = isExpanded ? "V" : ">";
		if (imgui_Button(scrx, starty + 1, 22.0f, 22.0f, expSym))
		{
			isExpanded = !isExpanded;
		}
		imgui_Static(scrx + 23.0f, starty, a.name.c_str(), 314);
		if (imgui_Button(scrx + 337.0f, starty, 22.0f, 22.0f, "X"))
		{
			toDel = i;
		}
		starty += 25.0f;

		if (isExpanded)
		{
			const char * name = a.particleFXName.empty() ? "<Empty>" : a.particleFXName.c_str();
			imgui_Static(scrx + 15, starty, name, 289);
			if (imgui_Button(scrx + 305, starty, 50.0f, 22.0f, "Particle"))
			{
				particleToSet = &a.particleFXName;
				editorMode = EDITORMODE_PARTICLE_PICK;
			}
			starty += 23.0f;

			starty += imgui_Value_Slider(scrx + 15, starty, "Start delay", &a.playDelay, 0.0f, 10.0f, "%-02.3f", true, true, 345);
			starty += imgui_Value_Slider(scrx + 15, starty, "Play time", &a.playLength, 0.0f, 100.0f, "%-02.3f", true, true, 345);
		}
	}

	if (toDel < actionsLibrary.Count())
	{
		actionsLibrary.Erase(toDel);
	}

	starty += 22.0f;
	static char tmpBuf[64] = {0};

	imgui2_StringValueEx(scrx, starty, 360.0f, "Effect Name: ", tmpBuf);
	starty += 23.0f;
	if (imgui_Button(scrx, starty, 180, 22.0f, "New Effect"))
	{
		if (strlen(tmpBuf) == 0)
		{
			MessageBox(r3dRenderer->HLibWin, "Effect name should not be empty", "Error", MB_ICONERROR | MB_OK);
		}
		else
		{
			Action newAction;
			newAction.name = tmpBuf;
			
			bool unique = true;
			for (uint32_t i = 0; i < actionsLibrary.Count(); ++i)
			{
				//	We have name collision
				if (actionsLibrary[i].name == newAction.name)
				{
					char tmpBuf2[256] = {0};
					sprintf_s(tmpBuf2, _countof(tmpBuf2), "Effect with name '%s' already exist. Pick another name", tmpBuf);
					MessageBox(r3dRenderer->HLibWin, tmpBuf2, "Error", MB_ICONERROR | MB_OK);
					unique = false;
					break;
				}
			}
			if (unique)
			{
				actionsLibrary.PushBack(newAction);
			}
		}
	}
	starty += 20.0f;

	return starty - scry;
}

//////////////////////////////////////////////////////////////////////////

float EventSystem::DrawEventsEditor(float scrx, float scry, float scrw, float scrh)
{
	float starty = scry;

	const static char* list[] =
	{
		"Fire Start",
		"Fire End",
		"Walk Start",
		"Walk End",
		"Death",
		"Landing",
		"Bumping",
		"Step",
		"StartUp",
		"Hit"
	};

	COMPILE_ASSERT(_countof(list) == EVENT_Total);

	for (uint32_t j = 0; j < EVENT_Total; ++j)
	{
		const char *expSym = isEventsExpanded[j] ? "V" : ">";
		if (imgui_Button(scrx, starty + 1, 22.0f, 22.0f, expSym))
		{
			isEventsExpanded[j] = !isEventsExpanded[j];
		}

		starty += imgui_Static(scrx + 23.0f, starty, list[j], 336);

		if (!isEventsExpanded[j])
			continue;

		EventActions &ea = eventsAndActions[j];
		uint32_t toDel = ea.Count();
		int numEmpty = 0;

		editorEventsExpanded[j].Resize(ea.Count());

		for (uint32_t i = 0; i < ea.Count(); ++i)
		{
			Action &aIt = ea[i];

			bool isEmpty = aIt.name.empty();
			if (isEmpty) ++numEmpty;

			const char * actionName = isEmpty ? "<Empty>" : aIt.name.c_str();
			imgui_Static(scrx + 37, starty, actionName, 223);
			if (imgui_Button(scrx + 261, starty, 50.0f, 22.0f, "Effect"))
			{
				actionToPick = &aIt;
				editorMode = EDITORMODE_ACTIONS_PICK;
			}
			if (!isEmpty)
			{
				if (imgui_Button(scrx + 311, starty, 22.0f, 22.0f, "P"))
				{
					OnEvent(static_cast<Events>(j), true);
				}
				if (imgui_Button(scrx + 333, starty, 22.0f, 22.0f, "X"))
				{
					toDel = i;
				}
				bool &isExpanded = editorEventsExpanded[j][i];
				if (imgui_Button(scrx + 15, starty, 22.0f, 22.0f, isExpanded ? "V" : ">"))
				{
					isExpanded = !isExpanded;
				}

				if (isExpanded)
				{
					starty += 23.0f;
					const char * boneName = aIt.boneIdx < 0 ? "<Empty>" : owner->m_BindSkeleton->GetBoneName(aIt.boneIdx);
					imgui_Static(scrx + 15, starty, boneName, 289);
					if (imgui_Button(scrx + 305, starty, 50.0f, 22.0f, "Bone"))
					{
						boneIndexToSet = &aIt.boneIdx;
						editorMode = EDITORMODE_BONE_PICK;
					}
					starty += 28.0f;
					starty += imgui_Value_Slider(scrx + 15, starty, "Offset X", &aIt.positionOffset.x, -10.0f, 10.0f, "%-02.2f", true, true, 345);
					starty += imgui_Value_Slider(scrx + 15, starty, "Offset Y", &aIt.positionOffset.y, -10.0f, 10.0f, "%-02.2f", true, true, 345);
					starty += imgui_Value_Slider(scrx + 15, starty, "Offset Z", &aIt.positionOffset.z, -10.0f, 10.0f, "%-02.2f", true, true, 345);
					starty += 5.0f;

					r3dVector radAngle = R3D_RAD2DEG(aIt.localRotationAngles);
					starty += imgui_Value_Slider(scrx + 15, starty, "Rot X", &radAngle.x, 0, 360.0f, "%3.2f", true, true, 345);
					starty += imgui_Value_Slider(scrx + 15, starty, "Rot Y", &radAngle.y, 0, 360.0f, "%3.2f", true, true, 345);
					starty += imgui_Value_Slider(scrx + 15, starty, "Rot Z", &radAngle.z, 0, 360.0f, "%3.2f", true, true, 345);
					aIt.localRotationAngles = R3D_DEG2RAD(radAngle);

					starty += 5.0f;
					starty += imgui_Value_Slider(scrx + 15, starty, "Scale", &aIt.scale, 0, 3.0f, "%-02.2f", true, true, 345);
				}
			}

			starty += 23.0f;

		}

		//	Remove action which is scheduled for deletion
		if (toDel < ea.Count())
		{
			ea.Erase(toDel);
		}

		//	Add one free slot
		if (numEmpty == 0)
		{
			ea.PushBack(Action());
		}
	}
	starty += imgui_Checkbox(scrx, starty, "Show Effect Locations", &gShowEffectLocations, 1);

	return starty - scry;
}

//////////////////////////////////////////////////////////////////////////

int EventSystem::PickBoneEditor(float scrx, float &scry)
{
	float &starty = scry;
	if (boneNames.empty())
	{
		for (int i = 0; i < owner->m_BindSkeleton->GetNumBones(); ++i)
		{
			boneNames.push_back(owner->m_BindSkeleton->GetBoneName(i));
		}
	}

	starty += imgui_Static(scrx, starty, "Select bone from list");

	static float offset = 0;
	static int pickedBoneIndex = -1;
	imgui_DrawList(scrx, starty, 360.0f, 300.0f, boneNames, &offset, &pickedBoneIndex);

	starty += 305.0f;

	if (imgui_Button(scrx, starty, 160.0f, 22.0f, "Pick Bone") && pickedBoneIndex >= 0)
	{
		*boneIndexToSet = pickedBoneIndex;
		boneIndexToSet = 0;
		return 1;
	}

	if (imgui_Button(scrx + 160.0f, starty, 160.0f, 22.0f, "Cancel"))
	{
		boneIndexToSet = 0;
		return 0;
	}
	starty += 23.0f;

	return -1;
}

//////////////////////////////////////////////////////////////////////////

int EventSystem::PickParticleEditor(float scrx, float &scry)
{
	float &starty = scry;

	//	Fill particles array
	if (particleNames.empty())	
	{
		WIN32_FIND_DATA ffblk;
		HANDLE h;
		char buf[MAX_PATH];

		particleNames.clear();
		sprintf(buf, "%s\\*.prt", "data\\Particles");

		h = FindFirstFile(buf, &ffblk);
		if (h != INVALID_HANDLE_VALUE)
		{
			do
			{
				PathRemoveExtensionA(ffblk.cFileName);
				particleNames.push_back(ffblk.cFileName);
			}
			while(FindNextFile(h, &ffblk) != 0);
			FindClose(h);
		}
	}

	starty += imgui_Static(scrx, starty, "Select particle system from list");

	static float offset = 0;
	static int pickedParticleIdx = -1;
	imgui_DrawList(scrx, starty, 360.0f, 300.0f, particleNames, &offset, &pickedParticleIdx);

	starty += 305.0f;

	if (imgui_Button(scrx, starty, 160.0f, 22.0f, "Pick Particle") && pickedParticleIdx >= 0)
	{
		*particleToSet = particleNames[pickedParticleIdx];
		particleToSet = 0;
		return 1;
	}

	if (imgui_Button(scrx + 160.0f, starty, 160.0f, 22.0f, "Cancel"))
	{
		particleToSet = 0;
		return 0;
	}
	starty += 23.0f;

	return -1;
}

//////////////////////////////////////////////////////////////////////////

int EventSystem::PickActionsEditor(float scrx, float &scry)
{
	stringlist_t actionNames;
	float &starty = scry;

	for (uint32_t i = 0; i < actionsLibrary.Count(); ++i)
	{
		actionNames.push_back(actionsLibrary[i].name);
	}

	starty += imgui_Static(scrx, starty, "Select effect from list");

	static float offset = 0;
	static int idx = -1;
	imgui_DrawList(scrx, starty, 360.0f, 300.0f, actionNames, &offset, &idx);

	starty += 305.0f;

	if (imgui_Button(scrx, starty, 160.0f, 22.0f, "Pick Effect") && idx >= 0)
	{
		Action newAction;
		CreateActionByName(actionNames[idx], newAction);
		*actionToPick = newAction;
		actionToPick = 0;
		return 1;
	}

	if (imgui_Button(scrx + 160.0f, starty, 160.0f, 22.0f, "Cancel"))
	{
		actionToPick = 0;
		return 0;
	}
	starty += 23.0f;

	return -1;
}

//////////////////////////////////////////////////////////////////////////

int EventSystem::PickActionFromLibraryEditor(float scrx, float &scry)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::GetParticleWorldPose(const Action &a, D3DXMATRIX &outPose)
{
	D3DXMATRIX worldMat = owner->DrawMatrix;

	D3DXMATRIX offsetMat, rotMat;
	D3DXMatrixTranslation(&offsetMat, a.positionOffset.x, a.positionOffset.y, a.positionOffset.z);
	D3DXMatrixRotationYawPitchRoll(&rotMat, a.localRotationAngles.y, a.localRotationAngles.x, a.localRotationAngles.z);

	D3DXMatrixMultiply(&worldMat, &offsetMat, &worldMat);
	owner->m_Animation->GetCurrentSkeleton()->GetBoneWorldTM(a.boneIdx, &outPose, worldMat);
	D3DXMatrixMultiply(&outPose, &rotMat, &outPose);
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::Update()
{
	for (uint32_t i = 0; i < eventsAndActions.Count(); ++i)
	{
		EventActions & ea = eventsAndActions[i];
		for (uint32_t j = 0; j < ea.Count(); ++j)
		{
			Action & a = ea[j];
			if (a.boneIdx < 0 || !a.particleFX)
				continue;

			D3DXMATRIX poseMat;
			GetParticleWorldPose(a, poseMat);

			r3dPoint3D pt(poseMat._41, poseMat._42, poseMat._43);
			a.particleFX->SetPosition(pt);
			a.particleFX->RotateParticleSystem(poseMat);
			a.particleFX->RenderScale = a.scale;

			float time = r3dGetTime();
			if (a.timeAtStart + a.playDelay < time)
			{
				a.particleFX->Torch->bEmit = 1;
			}
			if (a.playLength > 0 && a.timeAtStart + a.playLength + a.playDelay < time)
			{
				a.particleFX->Torch->bEmit = 0;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::OnEvent(Events ev, bool forceRestart)
{
	EventActions & events = eventsAndActions[static_cast<int>(ev)];

	for (uint32_t i = 0; i < events.Count(); ++i)
	{
		Action & a = events[i];
		//	Check for action validity
		if (a.particleFXName.empty() || a.name.empty())
			return;

		if (forceRestart && a.particleFX)
		{
			GameWorld().DeleteObject(a.particleFX);
			a.particleFX = 0;
		}

		//	Create particle FX
		if (!a.particleFX || a.particleFX->Name != a.particleFXName.c_str())
		{
			if (a.particleFX)
			{
				GameWorld().DeleteObject(a.particleFX);
			}

			a.particleFX = (obj_ParticleSystem*)srv_CreateGameObject
			(
				"obj_ParticleSystem",
				a.particleFXName.c_str(),
				owner->GetPosition()
			);
			if (!a.particleFX)
			{
				return;
			}
			a.particleFX->bKeepAlive = true;
			a.particleFX->Torch->bEmit = 0;
			a.particleFX->SetObjFlags(OBJFLAG_SkipCastRay);
			a.particleFX->m_isSerializable = false;
		}
		a.timeAtStart = r3dGetTime();
		if (a.particleFX)
			a.particleFX->Torch->bEmit = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::LoadFromXML(pugi::xml_node &node)
{
	LoadActionsLibrary();

	//	Events
	pugi::xml_node eventData = node.child("event");
	while (!eventData.empty())
	{
		int idx = eventData.attribute("type").as_int();
		r3dSTLString actionName;

		pugi::xml_node actionNameNode = eventData.child("action_name");
		while (!actionNameNode.empty())
		{
			actionName = actionNameNode.attribute("val").value();
			actionNameNode = actionNameNode.next_sibling("action_name");
			Action newAction;
			if (CreateActionByName(actionName, newAction))
			{
				GetXMLVal("bone_index", eventData, &newAction.boneIdx);
				GetXMLVal("offset", eventData, &newAction.positionOffset);
				GetXMLVal("rotation", eventData, &newAction.localRotationAngles);
				eventsAndActions[idx].PushBack(newAction);
			}
		}

		eventData = eventData.next_sibling("event");
	}
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::SaveToXML(pugi::xml_node &node)
{
	SaveActionsLibrary();

	//	Save events
	for (uint32_t i = 0, i_end = eventsAndActions.Count(); i != i_end; ++i)
	{
		pugi::xml_node eventData = node.append_child();
		eventData.set_name("event");
		eventData.append_attribute("type").set_value(i);

		const EventActions &ea = eventsAndActions[i];
		for (uint32_t j = 0, j_end = ea.Count(); j < j_end; ++j)
		{
			const Action &it = ea[j];
			if (it.name.empty())
				continue;

			SetXMLVal("action_name", eventData, &it.name);
			SetXMLVal("bone_index", eventData, &it.boneIdx);
			SetXMLVal("offset", eventData, &it.positionOffset);
			SetXMLVal("rotation", eventData, &it.localRotationAngles);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::LoadActionsLibrary()
{
	r3dFile* f = r3d_open(ACTIONS_LIB_PATH, "rb");
	if (f)
	{
		char* fileBuffer = game_new char[f->size + 1];
		fread(fileBuffer, f->size, 1, f);
		fileBuffer[f->size] = 0;

		pugi::xml_document xmlFile;
		pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
		if (!parseResult)
			r3dError("Failed to parse XML %s, error: %s", ACTIONS_LIB_PATH, parseResult.description());

		pugi::xml_node rootNode = xmlFile.child("root");

		LoadActionsLibraryFromXML(rootNode);

		delete fileBuffer;
		fclose(f);
	}
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::LoadActionsLibraryFromXML(pugi::xml_node &rootNode)
{
	pugi::xml_node actionData = rootNode.child("action");
	while (!actionData.empty())
	{
		r3dSTLString actionName = actionData.attribute("name").value();
		Action a;
		a.name = actionName;
		GetXMLVal("particle_system_name", actionData, &a.particleFXName);
		GetXMLVal("play_delay", actionData, &a.playDelay);
		GetXMLVal("play_length", actionData, &a.playLength);
		actionsLibrary.PushBack(a);

		actionData = actionData.next_sibling("action");
	}
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::SaveActionsLibrary()
{
	pugi::xml_document xmlFile;
	pugi::xml_node n = xmlFile.append_child();
	n.set_name("root");
	SaveActionsLibraryToXML(n);
	xmlFile.save_file(ACTIONS_LIB_PATH);
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::SaveActionsLibraryToXML(pugi::xml_node &rootNode)
{
	//	Save actions
	for (uint32_t i = 0; i < actionsLibrary.Count(); ++i)
	{
		const ActionDesc &actionEntry = actionsLibrary[i];

		pugi::xml_node actionData = rootNode.append_child();
		actionData.set_name("action");
		actionData.append_attribute("name").set_value(actionEntry.name.c_str());
		SetXMLVal("particle_system_name", actionData, &actionEntry.particleFXName);
		SetXMLVal("play_delay", actionData, &actionEntry.playDelay);
		SetXMLVal("play_length", actionData, &actionEntry.playLength);
	}
}

//////////////////////////////////////////////////////////////////////////

bool EventSystem::CreateActionByName(const r3dSTLString &name, Action &outAction) const
{
	for (uint32_t i = 0; i < actionsLibrary.Count(); ++i)
	{
		const ActionDesc &a = actionsLibrary[i];
		if (a.name == name)
		{
			Action newAction(a);
			outAction = newAction;
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

void EventSystem::DrawDebug()
{
	if (!gShowEffectLocations)
		return;

	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

	for (uint32_t i = 0; i < eventsAndActions.Count(); ++i)
	{
		EventActions &ea = eventsAndActions[i];
		for (uint32_t j = 0; j < ea.Count(); ++j)
		{
			Action & a = ea[j];

			if (a.boneIdx < 0 || a.particleFXName.empty())
				return;

			D3DXMATRIX poseMat;
			GetParticleWorldPose(a, poseMat);

			r3dVector pos(poseMat._41, poseMat._42, poseMat._43);

			if (!EffectIcon)
				EffectIcon = r3dRenderer->LoadTexture("Data\\Images\\Particle.dds");

			r3dDrawIcon3D(pos, EffectIcon, r3dColor(255,255,255), 32);
		}
	}


	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
}

}

