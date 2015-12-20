//=========================================================================
//	Module: AI_PlayerSenses.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "AI_PlayerSenses.h"
#include "../Gameplay/obj_Zombie.h"
#include "AI_Player.H"
#include "../../Editors/LevelEditor.h"
#include "../../GameLevel.h"
#include "../../XMLHelpers.h"

//////////////////////////////////////////////////////////////////////////

extern r3dCamera gCam;

//////////////////////////////////////////////////////////////////////////

namespace
{
	char LevelXMLSettingsFileName[512] = {0} ;
	bool LoadLevelXMLSettings(pugi::xml_document &xmlLevelFile, pugi::xml_node &xmlRoot, Bytes &xmlFileBuffer)
	{
		_snprintf(LevelXMLSettingsFileName, sizeof LevelXMLSettingsFileName - 1, LEVEL_SETTINGS_FILE, r3dGameLevel::GetHomeDir());

		r3dFile* f = r3d_open(LevelXMLSettingsFileName, "rb");
		if (f)
		{
			if (!ParseXMLInMemory(f, &xmlFileBuffer, &xmlLevelFile))
			{
				fclose(f);
				return false ;
			}

			xmlRoot = xmlLevelFile.root().child("root") ;
			fclose(f);
		}
		return true;
	}
} // unnamed namespace

//////////////////////////////////////////////////////////////////////////

PlayerLifeProps::PlayerLifeProps(obj_Player *o)
: standStillVisiblity(20.0f)
, walkVisibility(50.0f)
, runVisibility(70.0f)
, sprintVisibility(100.0f)
, swimIdleVisibility(20.0f)
, swimSlowVisibility(40.0f)
, swimVisibility(60.0f)
, swimFastVisibility(90.0f)
, walkNoise(2.0f)
, runNoise(5.0f)
, sprintNoise(10.0f)
, fireNoise(40.0f)
, swimSlowNoise(2.0f)
, swimNoise(5.0f)
, swimFastNoise(10.0f)
, weaponFired(false)
, smell(10.0f)
, owner(o)
{
	r3d_assert(owner);
#ifndef FINAL_BUILD
	LoadAISettingsXML();
#endif
}

//////////////////////////////////////////////////////////////////////////

bool PlayerLifeProps::DetectByZombie(const obj_Zombie &z, bool &hardLock)
{
	bool detected = false;

	if (!owner)
		return detected;

	r3dVector zombiePos = z.GetPosition();
	r3dVector dir = zombiePos - owner->GetPosition();
	float dist = dir.Length();
	dir /= dist;

	float plrSmell = GetPlayerRawSmell();
	float plrNoise = GetPlayerRawNoise();
	float plrVis = GetPlayerRawVisibility();

/* //@
	const obj_Zombie::Config &cfg = z.GetAIConfig();

	hardLock = false;

	if (plrVis + cfg.detectionRadius > dist)
	{
		//	Check if zombie can see (facing to the) potential target
		D3DXMATRIX rotM = z.GetRotationMatrix();
		D3DXVECTOR3 fwd(0, 0, 1);
		D3DXVec3TransformCoord(&fwd, &fwd, &rotM);
		D3DXVec3TransformCoord(&fwd, &fwd, &rotM);
		r3dVector fwdVec(fwd.x, fwd.y, fwd.z);
		float angle = fwdVec.Dot(dir);
		if (angle > 0.2f)
		{
			//	Issue raycast query to check visibility occluders
			PxVec3 origin(zombiePos.x, zombiePos.y + 1.0f, zombiePos.z);
			PxVec3 dir(-dir.x, -dir.y, -dir.z);
			PxSceneQueryFlags flags = PxSceneQueryFlag::eDISTANCE;
			PxRaycastHit h;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eDYNAMIC | PxSceneQueryFilterFlag::eSTATIC));
			if (!g_pPhysicsWorld->PhysXScene->raycastSingle(origin, dir, dist, flags, h, filter))
			{
				detected = true;
			}
		}
	}

	detected |= plrSmell + cfg.detectionRadius > dist;

	if (detected)
		hardLock = true;

	detected |= plrNoise + cfg.detectionRadius > dist;
*/

	return detected;
}

//////////////////////////////////////////////////////////////////////////

float PlayerLifeProps::GetPlayerRawSmell() const
{
	return smell;
}

//////////////////////////////////////////////////////////////////////////

float PlayerLifeProps::GetPlayerRawNoise() const
{
	float noise = 0;

	if (weaponFired)
	{
		noise = fireNoise;
	}
	else if (CUberData::ANIMDIR_Stand != owner->PlayerMoveDir)
	{
		switch (owner->PlayerState)
		{
		case PLAYER_MOVE_CROUCH:
		case PLAYER_MOVE_CROUCH_AIM:
		case PLAYER_MOVE_WALK_AIM:
		case PLAYER_MOVE_PRONE:
			noise = walkNoise;
			break;
		case PLAYER_MOVE_RUN:
			noise = runNoise;
			break;
		case PLAYER_MOVE_SPRINT:
			noise = sprintNoise;
			break;
		case PLAYER_SWIM_SLOW:
			noise = swimSlowNoise;
			break;
		case PLAYER_SWIM:
			noise = swimNoise;
			break;
		case PLAYER_SWIM_FAST:
			noise = swimFastNoise;
			break;
		}
	}

	return noise;
}

//////////////////////////////////////////////////////////////////////////

float PlayerLifeProps::GetPlayerRawVisibility() const
{
	float vis = standStillVisiblity;
	if (CUberData::ANIMDIR_Stand != owner->PlayerMoveDir)
	{
		switch (owner->PlayerState)
		{
		case PLAYER_MOVE_CROUCH:
		case PLAYER_MOVE_CROUCH_AIM:
		case PLAYER_MOVE_WALK_AIM:
			vis = walkVisibility;
			break;
		case PLAYER_MOVE_RUN:
			vis = runVisibility;
			break;
		case PLAYER_MOVE_SPRINT:
			vis = sprintVisibility;
			break;
		case PLAYER_SWIM_IDLE:
			vis = swimIdleVisibility;
			break;
		case PLAYER_SWIM_SLOW:
			vis = swimSlowVisibility;
			break;
		case PLAYER_SWIM:
			vis = swimVisibility;
			break;
		case PLAYER_SWIM_FAST:
			vis = swimFastVisibility;
			break;
		}
	}

	return vis;
}

float PlayerLifeProps::getPlayerVisibility()
{
	return GetPlayerRawVisibility()/100.0f;
}

float PlayerLifeProps::getPlayerHearRadius()
{
	return GetPlayerRawNoise()/100.0f;
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
void PlayerLifeProps::DebugVisualizeRanges()
{
	if (!owner) return;

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH);
	int psId = r3dRenderer->GetCurrentPixelShaderIdx();
	int vsId = r3dRenderer->GetCurrentVertexShaderIdx();
	r3dRenderer->SetPixelShader();
	r3dRenderer->SetVertexShader();
	r3dRenderer->SetTex(0, 0);


	//	Visibility dist
	r3dDrawCircle3D(owner->GetPosition()+r3dPoint3D(0,0.2f,0), GetPlayerRawVisibility(), gCam, 0.1f, r3dColor::blue);
	//	Smell dist
	//r3dDrawCircle3D(owner->GetPosition()+r3dPoint3D(0,0.2f,0), GetPlayerRawSmell(), gCam, 0.1f, r3dColor::yellow);
	//	Noise dist
	r3dDrawCircle3D(owner->GetPosition()+r3dPoint3D(0,0.2f,0), GetPlayerRawNoise(), gCam, 0.1f, r3dColor::green);

	r3dRenderer->Flush();
	r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
	r3dRenderer->SetPixelShader(psId);
	r3dRenderer->SetVertexShader(vsId);
}

//////////////////////////////////////////////////////////////////////////

/*
void PlayerLifeProps::DebugDrawZombieSenses()
{
	if (!owner) return;

	r3dPoint3D plrPos = owner->GetPosition();

	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
	{
		if( obj->ObjTypeFlags & OBJTYPE_Zombie )
		{
			obj_Zombie* z = static_cast< obj_Zombie* > ( obj );

			//	Draw distance info above head
			r3dPoint3D scrPos;
			r3dPoint3D worldPos = z->GetPosition();
			if (!r3dProjectToScreen(worldPos, &scrPos))
				continue;

			char buf[128] = {0};
			const obj_Zombie::Config &cfg = z->GetAIConfig();
			float dist = (z->GetPosition() - plrPos).Length();
			float adjVis = GetPlayerAdjustedVisibility(dist);
			float adjNoise = GetPlayerAdjustedNoise(dist);
			float adjSmell = GetPlayerAdjustedSmell(dist);

			sprintf(buf, "V: %.1f, %.1f\nN: %.1f, %.1f\nS: %.1f, %.1f", cfg.vision, adjVis, cfg.hearing, adjNoise, cfg.smelling, adjSmell);
			imgui_Static(scrPos.x, scrPos.y, buf, 80, true, 40);

		}
	}
}
*/

//////////////////////////////////////////////////////////////////////////

bool PlayerLifeProps::SaveAISettingsXML()
{
	pugi::xml_document xmlLevelFile;
	pugi::xml_node xmlRoot;
	Bytes xmlFileBuffer;

	if (!LoadLevelXMLSettings(xmlLevelFile, xmlRoot, xmlFileBuffer))
		return false;

	SerializeAISettingsXML<true>(xmlRoot);
	xmlLevelFile.save_file(LevelXMLSettingsFileName);
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool PlayerLifeProps::LoadAISettingsXML()
{
	pugi::xml_document xmlLevelFile;
	pugi::xml_node xmlRoot;
	Bytes xmlFileBuffer;

	if (!LoadLevelXMLSettings(xmlLevelFile, xmlRoot, xmlFileBuffer))
		return false;

	SerializeAISettingsXML<false>(xmlRoot);
	return true;
}

#endif

