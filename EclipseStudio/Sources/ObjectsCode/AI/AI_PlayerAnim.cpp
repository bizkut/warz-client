#include "r3dpch.h"
#include "r3d.h"

#include "GameCommon.h"
#include "ObjectsCode/weapons/WeaponArmory.h"
#include "ObjectsCode/weapons/Gear.h"
#include "ObjectsCode/weapons/Weapon.h"

#include "AI_Player.h"
#include "AI_PlayerAnim.h"

//////////////////////////////////////////////////////////////////////////
	r3dAnimPool*	g_CharactersAnimationsPool = NULL;
	
CUberData::CUberData()
{
	r3d_assert(g_CharactersAnimationsPool == NULL);
	g_CharactersAnimationsPool = &animPool_;

	LoadSkeleton();
	LoadAnimations();
	LoadWeaponTable();
}

CUberData::~CUberData()
{
	g_CharactersAnimationsPool = NULL;
	SAFE_DELETE(bindSkeleton_);
}

int CUberData::GetMoveDirFromAcceleration(const r3dPoint3D& accel)
{
	// detect direction from acceleration
	int MoveDir = CUberData::ANIMDIR_Stand;

	     if(accel.x > 0 && accel.z > 0) MoveDir = CUberData::ANIMDIR_StrRight;
	else if(accel.x < 0 && accel.z > 0) MoveDir = CUberData::ANIMDIR_StrLeft;
	else if(               accel.z > 0) MoveDir = CUberData::ANIMDIR_Str;
	else if(accel.x > 0 && accel.z < 0) MoveDir = CUberData::ANIMDIR_BackRight;
	else if(accel.x < 0 && accel.z < 0) MoveDir = CUberData::ANIMDIR_BackLeft;
	else if(               accel.z < 0) MoveDir = CUberData::ANIMDIR_Back;
	else if(accel.x > 0               ) MoveDir = CUberData::ANIMDIR_Right;
	else if(accel.x < 0               ) MoveDir = CUberData::ANIMDIR_Left;
	
	return MoveDir;
}

int CUberData::AddAnimation(const char* name, const char* anim_fname)
{
	static const char* animDir = "data\\Animations5"; //GLOBAL_ANIM_FOLDER;

	char buf[MAX_PATH];
	sprintf(buf, "%s\\%s.anm", animDir, anim_fname ? anim_fname : name);
	int aid = animPool_.Add(name, buf);
	if(aid == -1) 
		r3dError("can't add %s anim", name);
  
	return aid;
}

void CUberData::AddAnimationWithFPS(const char* name, int& aid, int& fps_aid)
{
	static const char* animDir = "data\\Animations5"; //GLOBAL_ANIM_FOLDER;

	char buf[MAX_PATH];
	sprintf(buf, "%s\\%s.anm", animDir, name);
	aid = animPool_.Add(name, buf);
	if(aid == -1) 
		r3dError("can't add %s anim", name);

	sprintf(buf, "%s\\FPS_%s.anm", animDir, name);
	if(r3dFileExists(buf))
	{
		char fps_name[128];
		sprintf(fps_name, "FPS_%s", name);
		fps_aid = animPool_.Add(fps_name, buf);
	}
	else
	{
		fps_aid = aid;
	}
}

int CUberData::TryToAddAnimation(const char* name, const char* anim_fname)
{
	static const char* animDir = "data\\Animations5"; //GLOBAL_ANIM_FOLDER;

	char buf[MAX_PATH];
	sprintf(buf, "%s\\%s.anm", animDir, anim_fname ? anim_fname : name);
	if(!r3dFileExists(buf))
		return -1;
	int aid = animPool_.Add(name, buf);
	return aid;
}

// recursively enable all bones in animation
static void enableAnimBone(const r3dSkeleton* skel, int bone, r3dAnimData* ad, int enable)
{
	const char* name = skel->GetBoneName(bone);
	ad->EnableTrack(name, enable);
  
	for(int i=0, e = skel->GetNumBones(); i < e; i++) 
	{
		if(skel->GetParentBoneId(i) == bone)
			enableAnimBone(skel, i, ad, enable);
	}
}

void enableAnimBones(const char* boneName, const r3dSkeleton* skel, r3dAnimData* ad, int enable)
{
	ad->BipedSetEnabled(!enable);
  
	int bone = skel->GetBoneID(boneName);
	r3d_assert(bone != -1);
  
	enableAnimBone(skel, bone, ad, enable);
}

void CUberData::LoadLowerAnimations()
{
	// no loops and prefixes, copy-paste FTW

	int* i;
	i = aid_.crouch;
	i[0] = AddAnimation("Crouch_Aim_Stand");
	i[1] = AddAnimation("Crouch_Str");
	i[2] = AddAnimation("Crouch_StrLeft");
	i[3] = AddAnimation("Crouch_StrRight");
	i[4] = AddAnimation("Crouch_Left");
	i[5] = AddAnimation("Crouch_Right");
	i[6] = AddAnimation("Crouch_Back");
	i[7] = AddAnimation("Crouch_BackLeft");
	i[8] = AddAnimation("Crouch_BackRight");

	i = aid_.walk;
	i[0] = AddAnimation("Walk_Aim_Stand_01");
	i[1] = AddAnimation("Walk_Str");
	i[2] = AddAnimation("Walk_StrLeft");
	i[3] = AddAnimation("Walk_StrRight");
	i[4] = AddAnimation("Walk_Left");
	i[5] = AddAnimation("Walk_Right");
	i[6] = AddAnimation("Walk_Back");
	i[7] = AddAnimation("Walk_BackLeft");
	i[8] = AddAnimation("Walk_BackRight");

	i = aid_.prone;
	i[0] = AddAnimation("Prone_Idle");
	i[1] = AddAnimation("Prone_Str");
	i[2] = AddAnimation("Prone_StrLeft");
	i[3] = AddAnimation("Prone_StrRight");
	i[4] = AddAnimation("Prone_Left");
	i[5] = AddAnimation("Prone_Right");
	i[6] = AddAnimation("Prone_Back");
	i[7] = AddAnimation("Prone_BackLeft");
	i[8] = AddAnimation("Prone_BackRight");

	aid_.prone_up_weapon = AddAnimation("Prone_Up_Weapon");
	aid_.prone_down_weapon = AddAnimation("Prone_Down_Weapon");
	aid_.prone_up_noweapon = AddAnimation("Prone_Up_NoWeapon");
	aid_.prone_down_noweapon = AddAnimation("Prone_Down_NoWeapon");

	i = aid_.run;
	i[1] = AddAnimation("Run_Str");
	i[2] = AddAnimation("Run_StrLeft");
	i[3] = AddAnimation("Run_StrRight");
	i[4] = AddAnimation("Run_Left");
	i[5] = AddAnimation("Run_Right");
	i[6] = AddAnimation("Run_Back");
	i[7] = AddAnimation("Run_BackLeft");
	i[8] = AddAnimation("Run_BackRight");

	i = aid_.sprint;
	i[1] = AddAnimation("Sprint_Str");
	i[2] = AddAnimation("Sprint_StrLeft");
	i[3] = AddAnimation("Sprint_StrRight");
	
	i = aid_.turnins;
	i[0] = AddAnimation("Walk_Str_TurnIn", "Walk_Str");
	i[1] = AddAnimation("Crouch_Str_TurnIn", "Crouch_Str");
	i[2] = AddAnimation("Walk_Str_TurnIn", "Walk_Str");//AddAnimation("Swim_Str_TurnIn", "Swim_Str");

	i = aid_.swimTransitions;
	i[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwim] = AddAnimation("Swim_Idle_to_Swim_Medium_Forward");
	i[CUberData::animIndices_s::SWIMTRANS_SwimToSwimIdle] = AddAnimation("Swim_Medium_Forward_to_Swim_Idle");
	i[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwimLeft] = AddAnimation("Swim_Idle_to_Swim_Medium_Left");
	i[CUberData::animIndices_s::SWIMTRANS_SwimLeftToSwimIdle] = AddAnimation("Swim_Medium_Left_to_Swim_Idle");
	i[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwimRight] = AddAnimation("Swim_Idle_to_Swim_Medium_Right");
	i[CUberData::animIndices_s::SWIMTRANS_SwimRightToSwimIdle] = AddAnimation("Swim_Medium_Right_to_Swim_Idle");
	i[CUberData::animIndices_s::SWIMTRANS_SwimLeftToSwimRight] = AddAnimation("Swim_Medium_Left_to_Swim_Medium_Right");
	i[CUberData::animIndices_s::SWIMTRANS_SwimRightToSwimLeft] = AddAnimation("Swim_Medium_Right_to_Swim_Medium_Left");
	i[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwimBack] = AddAnimation("Swim_Idle_to_Swim_Medium_Backward");
	i[CUberData::animIndices_s::SWIMTRANS_SwimBackToSwimIdle] = AddAnimation("Swim_Medium_Backward_to_Swim_Idle");
	i[CUberData::animIndices_s::SWIMTRANS_SwimToSprint] = AddAnimation("Swim_Fast_Forward_to_Run_Str");
	i[CUberData::animIndices_s::SWIMTRANS_SprintToSwim] = AddAnimation("Run_Str_to_Swim_Fast_Forward");

	i = aid_.swimSlowLower;
	i[0] = AddAnimation("Swim_Idle_Tread_Water");
	i[1] = AddAnimation("Swim_Slow_Forward");
	i[2] = AddAnimation("Swim_Slow_Forward_Left");
	i[3] = AddAnimation("Swim_Slow_Forward_Right");
	i[4] = AddAnimation("Swim_Slow_Left");
	i[5] = AddAnimation("Swim_Slow_Right");
	i[6] = AddAnimation("Swim_Slow_Backward");
	i[7] = AddAnimation("Swim_Slow_Backward_Left");
	i[8] = AddAnimation("Swim_Slow_Backward_Right");

	i = aid_.swimLower;
	i[0] = AddAnimation("Swim_Idle_Tread_Water");
	i[1] = AddAnimation("Swim_Medium_Forward");
	i[2] = AddAnimation("Swim_Medium_Forward_Left");
	i[3] = AddAnimation("Swim_Medium_Forward_Right");
	i[4] = AddAnimation("Swim_Medium_Left");
	i[5] = AddAnimation("Swim_Medium_Right");
	i[6] = AddAnimation("Swim_Medium_Backward");
	i[7] = AddAnimation("Swim_Medium_Backward_Left");
	i[8] = AddAnimation("Swim_Medium_Backward_Right");

	i = aid_.swimFastLower;
	i[1] = AddAnimation("Swim_Fast_Forward");
	i[2] = AddAnimation("Swim_Fast_Forward_Left");
	i[3] = AddAnimation("Swim_Fast_Forward_Right");
	i[4] = 0;
	i[5] = 0;
	i[6] = 0;
	i[7] = 0;
	i[8] = 0;

	i = aid_.swimSlowUpper;
	i[0] = AddAnimation("Swim_Idle_Tread_Water");
	i[1] = AddAnimation("Swim_Slow_Forward");
	i[2] = AddAnimation("Swim_Slow_Forward_Left");
	i[3] = AddAnimation("Swim_Slow_Forward_Right");
	i[4] = AddAnimation("Swim_Slow_Left");
	i[5] = AddAnimation("Swim_Slow_Right");
	i[6] = AddAnimation("Swim_Slow_Backward");
	i[7] = AddAnimation("Swim_Slow_Backward_Left");
	i[8] = AddAnimation("Swim_Slow_Backward_Right");

	i = aid_.swimUpper;
	i[0] = AddAnimation("Swim_Idle_Tread_Water");
	i[1] = AddAnimation("Swim_Medium_Forward");
	i[2] = AddAnimation("Swim_Medium_Forward_Left");
	i[3] = AddAnimation("Swim_Medium_Forward_Right");
	i[4] = AddAnimation("Swim_Medium_Left");
	i[5] = AddAnimation("Swim_Medium_Right");
	i[6] = AddAnimation("Swim_Medium_Backward");
	i[7] = AddAnimation("Swim_Medium_Backward_Left");
	i[8] = AddAnimation("Swim_Medium_Backward_Right");

	i = aid_.swimFastUpper;
	i[1] = AddAnimation("Swim_Fast_Forward");
	i[2] = AddAnimation("Swim_Fast_Forward_Left");
	i[3] = AddAnimation("Swim_Fast_Forward_Right");
	i[4] = 0;
	i[5] = 0;
	i[6] = 0;
	i[7] = 0;
	i[8] = 0;
}

void CUberData::LoadWeaponAnim(int (&wid)[AIDX_COUNT], int (&wid_fps)[AIDX_COUNT], const char* names[AIDX_COUNT])
{
	for(int i=0; i<AIDX_COUNT; i++)
	{
		wid[i] = -1;
		wid_fps[i] = -1;
		
		if(*names[i] == 0)
			continue;
			
		// we need to create dummy upper body anim for idle and stand
		char aname[128];
		sprintf(aname, names[i]);
		if(i == AIDX_IdleUpper || i == AIDX_StandUpper)
			strcat(aname, "_Upper");
		wid[i] = TryToAddAnimation(aname, names[i]);
		
		if(i >= AIDX_IdleUpper && wid[i]!=-1)
		{
			// those animations is upper body
			r3dAnimData* ad = animPool_.Get(wid[i]);
			enableAnimBones(blendStartBones_[i].c_str(), bindSkeleton_, ad, true);
		}

		// load fps anims
		sprintf(aname, "FPS_%s", names[i]);
		wid_fps[i] = TryToAddAnimation(aname);

		if(i >= AIDX_IdleUpper && wid_fps[i]!=-1)
		{
			// those animations is upper body
			r3dAnimData* ad = animPool_.Get(wid_fps[i]);
			if(i == AIDX_ProneBlend) // Patrick's request
				enableAnimBones("Bip01_Spine2", bindSkeleton_, ad, true);
			else
				enableAnimBones(blendStartBones_[i].c_str(), bindSkeleton_, ad, true);
		}
	}
}

void CUberData::LoadGrenadeAnim()
{
	const static char* grenadeAnims[12] = {
		"Crouch_Grenade_Throw_01_A_Pullback",	// 0
		"Crouch_Blend_Grenade_Hold_01",		// 1
		"Crouch_Grenade_Throw_01_B_Release",	// 2 
		"Run_Grenade_Throw_01_A_Pullback",	// 3
		"Run_Blend_Grenade_Hold_01",		// 4
		"Run_Grenade_Throw_01_B_Release",	// 5
		"Stand_Grenade_Throw_01_B_Pullback",	// 6
		"Stand_Aim_Grenade_Hold_01",		// 7
		"Stand_Grenade_Throw_01_D_Release",	// 8
		"Walk_Grenade_Throw_01_A_Pullback",	// 9
		"Walk_Aim_Grenade_Hold_01",		// 10
		"Walk_Grenade_Throw_01_B_Release",	// 11
	};
	
	for(int i=0; i<R3D_ARRAYSIZE(grenadeAnims); i++)
	{
		AddAnimationWithFPS(grenadeAnims[i], aid_.grenades_tps[i], aid_.grenades_fps[i]);
		
		r3dAnimData* ad = animPool_.Get(aid_.grenades_tps[i]);
		enableAnimBones("Bip01_Spine1", bindSkeleton_, ad, true);
		
		if(aid_.grenades_tps[i] != aid_.grenades_fps[i])
		{
			ad = animPool_.Get(aid_.grenades_fps[i]);
			enableAnimBones("Bip01_Spine1", bindSkeleton_, ad, true);
		}
	}

	/*const static char* bombsAnims[] = {
		"Stand_Place_Bomb_01_A_ground_A_Plant",	// 0
		"Stand_Place_Bomb_01_A_ground_B_Set",	// 1
		"Stand_Place_Bomb_01_A_ground_C_Stand",	// 2
		// same order as enums in GRENADE_ANIM_. Check index in StartGrenadeThrowAnimation() as well
		"Stand_Place_Bomb_01_B_C4",		// GRENADE_ANIM_Claymore
		"Stand_Place_VS50",			// GRENADE_ANIM_VS50
		"Stand_Place_Valmara69",		// GRENADE_ANIM_V69
	};
	
	for(int i=0; i<R3D_ARRAYSIZE(bombsAnims); i++)
	{
		AddAnimationWithFPS(bombsAnims[i], aid_.bombs_tps[i], aid_.bombs_fps[i]);
	}*/
}

void CUberData::LoadJumpAnim()
{
	aid_.jumps[ 0] = AddAnimation("Jump_Stand_t1_Start");
	aid_.jumps[ 1] = AddAnimation("Jump_Stand_t1_Air");
	aid_.jumps[ 2] = AddAnimation("Jump_Stand_t1_Landing");
	aid_.jumps[ 3] = AddAnimation("Jump_Walk_t1_Start");
	aid_.jumps[ 4] = AddAnimation("Jump_Walk_t1_Air");
	aid_.jumps[ 5] = AddAnimation("Jump_Walk_t1_Landing");
	aid_.jumps[ 6] = AddAnimation("Jump_Run_t1_Start");
	aid_.jumps[ 7] = AddAnimation("Jump_Run_t1_Air");
	aid_.jumps[ 8] = AddAnimation("Jump_Run_t1_Landing");
	aid_.jumps[ 9] = AddAnimation("Jump_Sprint_t1_Start");
	aid_.jumps[10] = AddAnimation("Jump_Sprint_t1_Air");
	aid_.jumps[11] = AddAnimation("Jump_Sprint_t1_Landing");

	/*
	for(int i=0; i<12; i++)
	{
		r3dAnimData* ad = animPool_.Get(aid_.jumps[i]);
	}*/
}

void CUberData::LoadDeathAnim()
{
	/* all this animations is very very wierd
	aid_.deaths[ 0] = AddAnimation("Death_03_t1_SUP_AT4");
	aid_.deaths[ 1] = AddAnimation("Death_04_t1_SUP_RPG");
	aid_.deaths[ 2] = AddAnimation("Death_05_v1_Exp");
	aid_.deaths[ 3] = AddAnimation("Death_01");
	aid_.deaths[ 4] = AddAnimation("Death_01_v2_t1");
	aid_.deaths[ 5] = AddAnimation("Death_01_v2_t2");
	aid_.deaths[ 6] = AddAnimation("Death_01_v5_t1");
	aid_.deaths[ 7] = AddAnimation("Death_01_v6_t1");
	aid_.deaths[ 8] = AddAnimation("Death_01_v7_t1");
	aid_.deaths[ 9] = AddAnimation("Death_01_v8_t1");
	aid_.deaths[10] = AddAnimation("Death_01_v9_t1");
	*/
	aid_.deaths[11] = AddAnimation("Death_02_t1");
	aid_.deaths[12] = AddAnimation("Death_02_t1");//AddAnimation("Death_01_v1_s1");
}

void CUberData::LoadHandsAnim()
{
	memset(&wpn_hands, 0, sizeof(wpn_hands));

	int i = AIDX_ShootWalk;
	wpn_hands[i][0] = AddAnimation("Stand_Shooting_Mel_Hands_L_Hook");
	wpn_hands[i][1] = AddAnimation("Stand_Shooting_Mel_Hands_L_Jab");
	wpn_hands[i][2] = AddAnimation("Stand_Shooting_Mel_Hands_R_Punch");
	wpn_hands[i][3] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_L_Hook");
	wpn_hands[i][4] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_L_Jab");
	wpn_hands[i][5] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_R_Punch");
	i = AIDX_ShootAim;
	wpn_hands[i][0] = AddAnimation("Stand_Shooting_Mel_Hands_L_Hook");
	wpn_hands[i][1] = AddAnimation("Stand_Shooting_Mel_Hands_L_Jab");
	wpn_hands[i][2] = AddAnimation("Stand_Shooting_Mel_Hands_R_Punch");
	wpn_hands[i][3] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_L_Hook");
	wpn_hands[i][4] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_L_Jab");
	wpn_hands[i][5] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_R_Punch");
	i = AIDX_ShootCrouch;
	wpn_hands[i][0] = AddAnimation("Crouch_Mel_Hands_L_Hook");
	wpn_hands[i][1] = AddAnimation("Crouch_Mel_Hands_L_Jab");
	wpn_hands[i][2] = AddAnimation("Crouch_Mel_Hands_R_Punch");
	wpn_hands[i][3] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_L_Hook");
	wpn_hands[i][4] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_L_Jab");
	wpn_hands[i][5] = AddAnimation("FPS_Stand_Shooting_Mel_Hands_R_Punch");
	i = AIDX_ShootProne;
	wpn_hands[i][0] = AddAnimation("Prone_Mel_Hands_L_Jab");
	wpn_hands[i][1] = AddAnimation("Prone_Mel_Hands_R_Chop");
	wpn_hands[i][2] = AddAnimation("Prone_Mel_Hands_R_Punch");
	wpn_hands[i][3] = AddAnimation("FPS_Prone_Mel_Hands_L_Jab");
	wpn_hands[i][4] = AddAnimation("FPS_Prone_Mel_Hands_R_Chop");
	wpn_hands[i][5] = AddAnimation("FPS_Prone_Mel_Hands_R_Punch");

	for(int i=0; i<sizeof(wpn_hands) / sizeof(int); i++)
	{
		int idx = ((int*)wpn_hands)[i]; // ugh...
		if(!idx) continue;
		r3dAnimData* ad = animPool_.Get(idx);
		enableAnimBones("Bip01_Spine2", bindSkeleton_, ad, true);
	}
	aid_.swimFPSSlowUpper[0] = AddAnimation("Swim_Idle_Tread_Water");
	aid_.swimFPSSlowUpper[1] = AddAnimation("FPS_Swim_Slow_Forward");
	aid_.swimFPSSlowUpper[2] = AddAnimation("FPS_Swim_Slow_Forward_Left");
	aid_.swimFPSSlowUpper[3] = AddAnimation("FPS_Swim_Slow_Forward_Right");
	aid_.swimFPSSlowUpper[4] = AddAnimation("FPS_Swim_Slow_Left");
	aid_.swimFPSSlowUpper[5] = AddAnimation("FPS_Swim_Slow_Right");
	aid_.swimFPSSlowUpper[6] = AddAnimation("FPS_Swim_Slow_Backward");
	aid_.swimFPSSlowUpper[7] = AddAnimation("FPS_Swim_Slow_Backward_Left");
	aid_.swimFPSSlowUpper[8] = AddAnimation("FPS_Swim_Slow_Backward_Right");

	aid_.swimFPSUpper[0] = AddAnimation("Swim_Idle_Tread_Water");
	aid_.swimFPSUpper[1] = AddAnimation("FPS_Swim_Medium_Forward");
	aid_.swimFPSUpper[2] = AddAnimation("FPS_Swim_Medium_Forward_Left");
	aid_.swimFPSUpper[3] = AddAnimation("FPS_Swim_Medium_Forward_Right");
	aid_.swimFPSUpper[4] = AddAnimation("FPS_Swim_Medium_Left");
	aid_.swimFPSUpper[5] = AddAnimation("FPS_Swim_Medium_Right");
	aid_.swimFPSUpper[6] = AddAnimation("FPS_Swim_Medium_Backward");
	aid_.swimFPSUpper[7] = AddAnimation("FPS_Swim_Medium_Backward");
	aid_.swimFPSUpper[8] = AddAnimation("FPS_Swim_Medium_Backward");

	aid_.swimFPSFastUpper[0] = 0;
	aid_.swimFPSFastUpper[1] = AddAnimation("FPS_Swim_Fast_Forward");
	aid_.swimFPSFastUpper[2] = AddAnimation("FPS_Swim_Fast_Forward_Left");
	aid_.swimFPSFastUpper[3] = AddAnimation("FPS_Swim_Fast_Forward_Right");
	aid_.swimFPSFastUpper[4] = 0; //AddAnimation("FPS_Swim_Fast_Left");
	aid_.swimFPSFastUpper[5] = 0; //AddAnimation("FPS_Swim_Fast_Right");
	aid_.swimFPSFastUpper[6] = 0;
	aid_.swimFPSFastUpper[7] = 0;
	aid_.swimFPSFastUpper[8] = 0;

	// Enable Bone for each of the animations, except Idle
	// since it's the same anim as 3rd person uses, and
	// you'll never see the hands anyway.
	for( int i = 1; i < ANIMDIR_COUNT; ++i )
	{
		int idx = aid_.swimFPSSlowUpper[i];
		if( idx != 0 )
		{
			r3dAnimData* ad = animPool_.Get(idx);
			enableAnimBones("Bip01_Spine2", bindSkeleton_, ad, true);
		}
		idx = aid_.swimFPSUpper[i];
		if( idx != 0 )
		{
			r3dAnimData* ad = animPool_.Get(idx);
			enableAnimBones("Bip01_Spine2", bindSkeleton_, ad, true);
		}
		idx = aid_.swimFPSFastUpper[i];
		if( idx != 0 )
		{
			r3dAnimData* ad = animPool_.Get(idx);
			enableAnimBones("Bip01_Spine2", bindSkeleton_, ad, true);
		}
	}

	// Load animations and enable Bone for swimTransitions
	{
		int idx = aid_.swimTransitions[CUberData::animIndices_s::SWIMTRANS_FPSSwimToSprint] = AddAnimation("FPS_Swim_Fast_Forward_to_Run");
		r3dAnimData* ad = animPool_.Get(idx);
		enableAnimBones("Bip01_Spine2", bindSkeleton_, ad, true);
		idx = aid_.swimTransitions[CUberData::animIndices_s::SWIMTRANS_FPSSprintToSwim] = AddAnimation("FPS_Run_to_Swim_Fast_Forward");
		ad = animPool_.Get(idx);
		enableAnimBones("Bip01_Spine2", bindSkeleton_, ad, true);
	}
}

void CUberData::LoadUpperBlendStartBones()
{
	// init defaults
	for(int i=0; i<AIDX_COUNT; i++) {
		blendStartBones_[i] = "Bip01_Spine2";
	}

	const char* xml_file = "Data\\Animations5\\AnimUpperBlendStart.xml";
	r3dFile* f = r3d_open(xml_file, "rb");
	if(!f) {
		r3dError("Failed to open: %s\n", xml_file);
		return;
	}

	char* fileBuffer = game_new char[f->size + 1];
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlFile;
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
	if(!parseResult)
		r3dError("Failed to parse XML %s, error: %s", xml_file, parseResult.description());

	pugi::xml_node xmlBlends = xmlFile.child("UpperBlendStart");
	blendStartBones_[AIDX_CrouchBlend] = xmlBlends.attribute("AIDX_CrouchBlend").value();
	blendStartBones_[AIDX_CrouchAim]   = xmlBlends.attribute("AIDX_CrouchAim").value();
	blendStartBones_[AIDX_WalkBlend]   = xmlBlends.attribute("AIDX_WalkBlend").value();
	blendStartBones_[AIDX_WalkAim]     = xmlBlends.attribute("AIDX_WalkAim").value();
	blendStartBones_[AIDX_RunBlend]    = xmlBlends.attribute("AIDX_RunBlend").value();
	blendStartBones_[AIDX_SprintBlend] = xmlBlends.attribute("AIDX_SprintBlend").value();
	blendStartBones_[AIDX_ReloadWalk]  = xmlBlends.attribute("AIDX_Reload").value();
	blendStartBones_[AIDX_ReloadIdle]  = xmlBlends.attribute("AIDX_Reload").value();
	blendStartBones_[AIDX_ReloadCrouch]= xmlBlends.attribute("AIDX_Reload").value();
	blendStartBones_[AIDX_ShootWalk]   = xmlBlends.attribute("AIDX_Shoot").value();
	blendStartBones_[AIDX_ShootAim]    = xmlBlends.attribute("AIDX_Shoot").value();
	blendStartBones_[AIDX_ShootCrouch] = xmlBlends.attribute("AIDX_Shoot").value();
	blendStartBones_[AIDX_ProneBlend] = xmlBlends.attribute("AIDX_ProneBlend").value();
	blendStartBones_[AIDX_ProneAim] = xmlBlends.attribute("AIDX_ProneAim").value();
	blendStartBones_[AIDX_ReloadProne] = xmlBlends.attribute("AIDX_Reload").value();
	blendStartBones_[AIDX_ShootProne] = xmlBlends.attribute("AIDX_Shoot").value();
	blendStartBones_[AIDX_IdleProne] = xmlBlends.attribute("AIDX_ProneIdle").value();

		
	delete[] fileBuffer;
	fclose(f);
}

#ifdef VEHICLES_ENABLED
void CUberData::LoadVehicleAnim()
{
	int* i;
	i = aid_.vehicleDriver;
	i[0] = AddAnimation("Vehicle_Idle_Driver1");
	i[1] = 0;
	i[2] = 0;
	i[3] = 0;
	i[4] = AddAnimation("Vehicle_Driver1_to_TurnLeft");;
	i[5] = AddAnimation("Vehicle_Driver1_to_TurnRight");
	i[6] = 0;
	i[7] = 0;
	i[8] = 0;

	i = aid_.vehicleDriverToLeft;
	i[0] = AddAnimation("Vehicle_Driver1_to_TurnLeft");
	i[1] = 0;
	i[2] = 0;
	i[3] = 0;
	i[4] = 0;
	i[5] = 0;
	i[6] = 0;
	i[7] = 0;
	i[8] = 0;

	i = aid_.vehicleDriverToRight;
	i[0] = AddAnimation("Vehicle_Driver1_to_TurnRight");
	i[1] = 0;
	i[2] = 0;
	i[3] = 0;
	i[4] = 0;
	i[5] = 0;
	i[6] = 0;
	i[7] = 0;
	i[8] = 0;

	i = aid_.vehiclePassenger;
	i[0] = AddAnimation("Vehicle_Idle_Passenger1");
	i[1] = 0;
	i[2] = 0;
	i[3] = 0;
	i[4] = 0;
	i[5] = 0;
	i[6] = 0;
	i[7] = 0;
	i[8] = 0;
}

#endif

void CUberData::LoadAnimations()
{
	// reset all animation indices to -1
	memset(&aid_, 0xFF, sizeof(aid_));

	// load config
	LoadUpperBlendStartBones();
	
	// add zero index default anim
	AddAnimation("default", "Stand_Aim_ASR_M4");

	LoadLowerAnimations();
	LoadGrenadeAnim();
	LoadJumpAnim();
	LoadDeathAnim();
	LoadHandsAnim();

#ifdef VEHICLES_ENABLED
	LoadVehicleAnim();
#endif

	aid_.UI_IdleNoWeapon[0] = AddAnimation("UI_NoWeapon_Idle");
	aid_.UI_IdleNoWeapon[1] = AddAnimation("UI_NoWeapon_Idle");//AddAnimation("UI_Swim_Idle");

	aid_.attmMenuRiseWeapon[0] = AddAnimation("FPS_Attch_Raise_ASR");
	aid_.attmMenuRiseWeapon[1] = AddAnimation("FPS_Attch_Raise_SNP");
	aid_.attmMenuRiseWeapon[2] = AddAnimation("FPS_Attch_Raise_SHTG");
	aid_.attmMenuRiseWeapon[3] = AddAnimation("FPS_Attch_Raise_MG");
	aid_.attmMenuRiseWeapon[4] = AddAnimation("FPS_Attch_Raise_HG");
	aid_.attmMenuRiseWeapon[5] = AddAnimation("FPS_Attch_Raise_SMG");

	aid_.attmMenuIdleWeapon[0] = AddAnimation("FPS_Attch_Idle_ASR");
	aid_.attmMenuIdleWeapon[1] = AddAnimation("FPS_Attch_Idle_SNP");
	aid_.attmMenuIdleWeapon[2] = AddAnimation("FPS_Attch_Idle_SHTG");
	aid_.attmMenuIdleWeapon[3] = AddAnimation("FPS_Attch_Idle_MG");
	aid_.attmMenuIdleWeapon[4] = AddAnimation("FPS_Attch_Idle_HG");
	aid_.attmMenuIdleWeapon[5] = AddAnimation("FPS_Attch_Idle_SMG");
	
	// add default weapon anim
	const char* names1[] = {
		"UI_NoWeapon_Idle",
		"Idle_Hands_01",
		"Stand_Aim_Hands_01",
		"Swim_Idle_Tread_Water",
		"Swim_Idle_Tread_Water",
		"Idle_Hands_01",
		"Stand_Aim_Hands_01",
		"Crouch_Blend_Hands_01",
		"Crouch_Aim_Hands_01",
		"Walk_Blend_Hands_01",
		"Walk_Aim_Hands_01", //"Walk_Aim_ASR_M4",
		"Run_Blend_Hands_01",
		"Sprint_Blend_Hands_01",
		"Reload_Idle_ASR_M4_01",
		"Reload_Idle_ASR_M4_01",
		"Reload_Idle_ASR_M4_01",
		"Stand_Shooting_ASR_M4_Single",
		"Stand_Shooting_ASR_M4_Single",
		"Stand_Shooting_ASR_M4_Single",
		"Prone_Blend_Hands_01",
		"Prone_Aim_Hands_01",
		"Reload_Idle_ASR_M4_01",
		"Stand_Shooting_ASR_M4_Single",
		"Prone_Idle_Hands_01",
		"Swim_Idle_Tread_Water",
	};
	
	COMPILE_ASSERT( R3D_ARRAYSIZE(names1) == AIDX_COUNT ) ;

	LoadWeaponAnim(wpn1, wpn1_fps, names1);
}

void CUberData::LoadSkeleton()
{
	const char* skel_fname = "data/ObjectsDepot/Characters/ProperScale_AndBiped.skl";
	r3dSkeleton* skel = game_new r3dSkeleton();
	skel->LoadBinary(skel_fname);

	bindSkeleton_ = skel;

	return;
}

static int numWeaponAnimBugs = 0;
static void setWeaponAnimByFNAME(const char* FNAME, int* wid, int* wid_fps)
{
	// mimic store icon name
	char FNAME2[256];
	sprintf(FNAME2, "%s.dds", FNAME);

	// note, there is multiple entries with same FNAME, so set them all
	int numFounds = 0;
	g_pWeaponArmory->startItemSearch();
	while(g_pWeaponArmory->searchNextItem())
	{
		uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
		WeaponConfig* config = (WeaponConfig*)g_pWeaponArmory->getWeaponConfig(itemID);
		if(config)
		{
			const char* name = strrchr(config->m_StoreIcon, '/') + 1;
			if(stricmp(name, FNAME2) != 0)
				continue;

			// fill animation ids for that weapon
			if(config->m_animationIds) {
				r3dError("duplicate weapon anim data for %s\n", FNAME);
			}
			config->m_animationIds = game_new int[CUberData::AIDX_COUNT];
			memcpy(config->m_animationIds, wid, sizeof(int[CUberData::AIDX_COUNT]));

			if(config->m_animationIds_FPS) {
				r3dError("duplicate FPS weapon anim data for %s\n", FNAME);
			}
			config->m_animationIds_FPS = game_new int[CUberData::AIDX_COUNT];
			memcpy(config->m_animationIds_FPS, wid_fps, sizeof(int[CUberData::AIDX_COUNT]));

			numFounds++;
		}
	}
	
	if(!numFounds) 
	{
		if(numWeaponAnimBugs == 0)
			r3dPurgeArtBugs();
			
		r3dArtBug("AnimCSV: %s is not in game, remove it\n", FNAME);
		numWeaponAnimBugs++;
	}
}

static void checkIfAllWeaponsHaveAnimation()
{
	g_pWeaponArmory->startItemSearch();
	while(g_pWeaponArmory->searchNextItem())
	{
		uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
		const WeaponConfig* config = g_pWeaponArmory->getWeaponConfig(itemID);
		if(config)
		{
			const char* name = strrchr(config->m_StoreIcon, '/') + 1;

			if(!config->m_animationIds)
			{
				numWeaponAnimBugs++;
				r3dArtBug("AnimCSV: weapon %s does not have animation data\n", name);
			}

		}
	}

	if(numWeaponAnimBugs)
		r3dShowArtBugs();
}

static void fixUsableItemTPSAnim()
{
	// ok, here is idea. 
	// to avoid copying animation files for TPS version of usable items, we'll use EXP_Claymore animation for them
	// it will look crappy, but still it's better that nothing
	const WeaponConfig* wpn2 = g_pWeaponArmory->getWeaponConfig(101256);
	if(!wpn2) r3dError("fixTPSUsableItemAnim: no itemid");
	if(!wpn2->m_animationIds) r3dError("fixTPSUsableItemAnim: no animations");
	
	g_pWeaponArmory->startItemSearch();
	while(g_pWeaponArmory->searchNextItem())
	{
		uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
		const WeaponConfig* config = g_pWeaponArmory->getWeaponConfig(itemID);
		if(config && config->category != storecat_UsableItem && config->m_animationIds != NULL)
		{
			for(int j=0; j<CUberData::AIDX_COUNT; j++)
			{
				if(config->m_animationIds[j] == -1)
					config->m_animationIds[j] = wpn2->m_animationIds[j];
			}
		}
	}
}

void CUberData::LoadWeaponTable()
{
#ifndef FINAL_BUILD
	r3dOutToLog("LoadWeaponTable\n"); CLOG_INDENT;
#endif
   	float t1 = r3dGetTime();

	const char* alist_file = "Data\\Animations5\\Animation List.csv";
	r3dFile* f = r3d_open(alist_file, "rb");
	if(!f)
		r3dError("failed to open %s\n", alist_file);

	// skip first line = there is header descriptions
	char buf[4096] = "";
	fgets(buf, sizeof(buf), f);
	
	while(!feof(f))
	{
		buf[0] = 0;
		if(fgets(buf, sizeof(buf), f) == NULL)
			break;
		int slen = strlen(buf);
		if(slen < 2) 
			continue;
		if(buf[slen-1] == 0xA) { buf[slen-1] = 0; slen--; }
		if(buf[slen-1] == 0xD) { buf[slen-1] = 0; slen--; }
			
		// parse .CSV string
		const int expectedTokens = 21;
		const char* t[32];
		for(int i=0; i<expectedTokens; i++)
		  t[i] = "";
		
		int n = 0;
		char* token = strtok(buf, ",");
		while(token != NULL) {
			t[n++] = token;
			token = strtok(NULL, ",");
			if(token && strcmp(token, "none")==0) // we need special work to skip empty spaces in CSV file, otherwise if we have ,,,, in CSV, strtok will skip all of them
				token = "";
		}
		
		if(n == 0) {
			// empty line
			continue;
		}
#ifndef FINAL_BUILD		
		if(n != expectedTokens) {
			r3dOutToLog("Bad number of arguments: %d - %s\n", n, t[0]);
		}
		else
		{
			__asm nop;
		}
		
		//r3dOutToLog("Loading %s\n", t[0]);
#endif
		
		// create anim list table
		const char* names[] = {
		  t[2], //AIDX_UIIdle 
		  t[1], //AIDX_IdleLower,
		  t[3], //AIDX_StandLower,
		  t[2],//[21],	//AIDX_UISwimIdle
		  t[1],//[22],	//AIDX_SwimIdleLower
		  t[1], //AIDX_IdleUpper,
		  t[3], //AIDX_StandUpper,
		  t[6], //AIDX_CrouchBlend,
		  t[7], //AIDX_CrouchAim,
		  t[4], //AIDX_WalkBlend,
		  t[5], //AIDX_WalkAim,
		  t[8], //AIDX_RunBlend,
		  t[9], //AIDX_SprintBlend,
		  t[10], //AIDX_ReloadWalk,
		  t[11], //AIDX_ReloadIdle,
		  t[12], //AIDX_ReloadCrouch,
		  t[13], //AIDX_ShootWalk,
		  t[14],    //AIDX_ShootAim,
		  t[15],	 //AIDX_ShootCrouch
		  t[16],	//AIDX_ProneBlend
		  t[17],	//AIDX_ProneAim
		  t[18],	//AIDX_ProneReload
		  t[19],	//AIDX_ProneShoot
		  t[20],	//AIDX_ProneIdle
		  t[13],//[23],	//AIDX_SwimIdleUpper
		};
		COMPILE_ASSERT( R3D_ARRAYSIZE(names) == AIDX_COUNT ) ;
		
		int wid[AIDX_COUNT];
		int wid_fps[AIDX_COUNT];
		LoadWeaponAnim(wid, wid_fps, names);

		setWeaponAnimByFNAME(t[0], wid, wid_fps);
   	}
   	
#ifndef FINAL_BUILD
   	checkIfAllWeaponsHaveAnimation();

	r3dOutToLog("Loaded weapon animations: %f sec, %d anims\n", r3dGetTime() - t1, animPool_.Anims.size());
#endif

	fixUsableItemTPSAnim();
		
	fclose(f);
}

int CUberData::GetGrenadeAnimId(bool IsFPS, int PlayerState, int GrenadeState)
{
	// GrenadeState
	//  0 - pullback
	//  1 - hold
	//  2 - release

	int idx = 6;
	switch(PlayerState) 
	{
		default:
		case PLAYER_IDLE:
		case PLAYER_IDLEAIM:
			idx = 6;
			break;
		case PLAYER_MOVE_CROUCH:
		case PLAYER_MOVE_CROUCH_AIM:
			idx = 0;
			break;
		case PLAYER_MOVE_WALK_AIM:
			idx = 9;
			break;
		case PLAYER_MOVE_RUN:
			idx = 3;
			break;
	}

	if(IsFPS)
		return aid_.grenades_fps[idx + GrenadeState];
	else
		return aid_.grenades_tps[idx + GrenadeState];
}

int CUberData::GetJumpAnimId(int PlayerState, int JumpState)
{
	// JumpState
	//  0 - start
	//  1 - air
	//  2 - landing

	int idx = 6;
	switch(PlayerState) 
	{
		default:
		case PLAYER_IDLE:
		case PLAYER_IDLEAIM:
		case PLAYER_MOVE_CROUCH:
		case PLAYER_MOVE_CROUCH_AIM:
			idx = 0;
			break;
		case PLAYER_MOVE_WALK_AIM:
			idx = 3;
			break;
		case PLAYER_MOVE_RUN:
			idx = 6;
			break;
		case PLAYER_MOVE_SPRINT:
			idx = 9;
			break;
	}
	
	return aid_.jumps[idx + JumpState];
}

////////////////////////////////////////////////////////////////////////////
CUberEquip::CUberEquip(obj_Player* plr) : 
player(plr)
{
}

CUberEquip::~CUberEquip()
{
	for(int i=0; i<SLOT_Max; ++i)
	{
		// do not delete model, gear itself will handle that
		SAFE_DELETE(slots_[i].gear);
		slots_[i].wpn = NULL;
		slots_[i].mesh = NULL;
	}
	player = NULL;
}

void CUberEquip::SetSlot(ESlot slotId, r3dMesh* mesh)
{
	slot_s& sl = slots_[slotId];
	r3d_assert(sl.wpn == NULL);
	r3d_assert(sl.gear == NULL);

	sl.mesh = mesh;
}

void CUberEquip::SetSlot(ESlot slotId, Gear* gear)
{
	slot_s& sl = slots_[slotId];
	r3d_assert(sl.wpn == NULL); // must be a gear slot
	r3d_assert(sl.mesh == NULL);

	SAFE_DELETE(sl.gear); // release old one
	sl.gear = gear;
}

void CUberEquip::SetSlot(ESlot slotId, Weapon* wpn)
{
	slot_s& sl = slots_[slotId];
	r3d_assert(sl.gear == NULL); // must not be a gear slot!
	r3d_assert(sl.mesh == NULL);

	sl.wpn = wpn;
}

r3dMesh* CUberEquip::getSlotMesh(ESlot slotId)
{
	if(slots_[slotId].gear)
		return slots_[slotId].gear->getModel(g_camera_mode->GetInt()==2 && player->NetworkLocal);
	else if(slots_[slotId].wpn)
		return slots_[slotId].wpn->getModel(true, g_camera_mode->GetInt()==2 && player->NetworkLocal);
	else if(slots_[slotId].mesh)
		return slots_[slotId].mesh;

	return NULL;
}

D3DXMATRIX CUberEquip::getWeaponBone(const r3dSkeleton* skel, const D3DXMATRIX& offset)
{
	D3DXMATRIX mr1, world;
	D3DXMatrixRotationYawPitchRoll(&mr1, 0, R3D_PI/2, 0);
	skel->GetBoneWorldTM("PrimaryWeaponBone", &world, offset);
	world = mr1 * world;
	return world;
}

r3dPoint3D CUberEquip::getBonePos(int BoneID, const r3dSkeleton* skel, const D3DXMATRIX& offset)
{
	D3DXMATRIX mr1, world;
	D3DXMatrixRotationYawPitchRoll(&mr1, 0, R3D_PI/2, 0);
	skel->GetBoneWorldTM(BoneID, &world, offset);
	world = mr1 * world;
	return r3dPoint3D(world._41, world._42, world._43);
}

int	CUberEquip::IsLoaded()
{
	bool isFirstPerson = g_camera_mode->GetInt()==2 && player && player->NetworkLocal;
	for( int i = 0 ; i < SLOT_Max; i ++ )
	{
		if( slots_[ i ].gear )
		{
			if(slots_[ i ].gear->getModel(isFirstPerson) && slots_[ i ].gear->getModel(isFirstPerson)->IsDrawable()==false )
				return false ;
		}
		else if(slots_[i].mesh)
			if(!slots_[i].mesh->IsDrawable())
				return false;
	}

	return true ;
}

void CUberEquip::DrawSlot(ESlot slotId, const D3DXMATRIX& world, DrawType dt, bool skin, bool draw_firstperson, const r3dSkeleton* wpnSkeleton)
{
	if(slotId == SLOT_WeaponSide)
		return;
	if(draw_firstperson)
	{
		if(slotId == SLOT_Armor || slotId == SLOT_LowerBody || slotId == SLOT_Head || slotId == SLOT_Helmet || slotId == SLOT_Backpack)
			return;
	}

	bool isFirstPerson = g_camera_mode->GetInt()==2 && player && player->NetworkLocal;
	r3dMesh* mesh = NULL;
	if(slots_[slotId].gear)
		mesh = slots_[slotId].gear->getModel(isFirstPerson&&draw_firstperson);
	else if(slots_[slotId].wpn)
		mesh = slots_[slotId].wpn->getModel(true, isFirstPerson&&draw_firstperson);
	else if(slots_[slotId].mesh)
		mesh = slots_[slotId].mesh;

	if(mesh == NULL)
		return;
	if(!mesh->IsDrawable())
		return;

	DrawSlotMesh(mesh, world, dt, skin);

	if(slots_[slotId].wpn && wpnSkeleton)
	{
		Weapon* wpn = slots_[slotId].wpn;
		for(int i=0; i<WPN_ATTM_MAX; ++i)
		{
			mesh = wpn->getWeaponAttachmentMesh((WeaponAttachmentTypeEnum)i, player->m_isAiming && (i==WPN_ATTM_UPPER_RAIL));
			if(mesh && mesh->IsDrawable())
			{
				D3DXMATRIX attmWorld;
				wpnSkeleton->GetBoneWorldTM(WeaponAttachmentBoneNames[i], &attmWorld, world);
				DrawSlotMesh(mesh, attmWorld, dt, false);
			}
		}
	}
}

void CUberEquip::DrawSlotMesh(r3dMesh* mesh, const D3DXMATRIX& world, DrawType dt, bool skin)
{
	if(skin)
	{
		r3dMeshSetShaderConsts_Localized(world, NULL);
	}
	else
	{
		mesh->SetShaderConsts_Localized(world);

		// NOTE : needed for transparent camo only..
		// float4   WorldScale  		: register(c24);
		D3DXVECTOR4 scale(mesh->unpackScale.x, mesh->unpackScale.y, mesh->unpackScale.z, 0.f) ;
		D3D_V(r3dRenderer->pd3ddev->SetVertexShaderConstantF(24, (float*)&scale, 1)) ;
	}

	switch(dt)
	{
	case DT_DEFERRED:
		{
			r3dBoundBox worldBBox = mesh->localBBox;
			worldBBox.Transform(reinterpret_cast<const r3dMatrix *>(&world));
			// Vertex lights for forward transparent renderer.
			for (int i = 0; i < mesh->NumMatChunks; i++)
			{
				SetLightsIfTransparent(mesh->MatChunks[i].Mat, worldBBox);
			}

			mesh->DrawMeshDeferred(r3dColor::white, 0);
			break ;
		}

	case DT_DEPTH:
		if(mesh->IsSkeletal())
			r3dRenderer->SetVertexShader(VS_SKIN_DEPTH_ID) ;
		else
			r3dRenderer->SetVertexShader(VS_DEPTH_ID) ;

		// NOTE : no break on purpose

	case DT_AURA:
		mesh->DrawMeshWithoutMaterials();
		break ;

	case DT_SHADOWS:
		mesh->DrawMeshShadows();
		break ;
	case DT_DARKNESS:
		{
			int prevPS = r3dRenderer->CurrentPixelShaderID;
			SetFillGBufferConstantPixelShader( r3dColor::black, 0, gCam.vPointTo.NormalizeTo(), 0.f, 0.f, 0.f, 0.f );
			mesh->DrawMeshDeferred( r3dColor::white, R3D_MATF_NO_PIXEL_SHADER );
			r3dRenderer->SetPixelShader( prevPS );
		}
		break;
	}
}


void CUberEquip::Draw(const r3dSkeleton* skel, const D3DXMATRIX& CharMat, bool draw_weapon, DrawType dt, bool first_person)
{
	//todo: call extern void r3dMeshSetWorldMatrix(const D3DXMATRIX& world)
	// instead of mesh->SetWorldMatrix

    // in first person mode we need to render player and gun into different Z range
	if(dt == DT_AURA)
	{
		float expandConst[ 4 ] = { r_aura_extrude->GetFloat(), 0.f, 0.f, 0.f } ;
		D3D_V(r3dRenderer->pd3ddev->SetVertexShaderConstantF(23, expandConst, 1)) ;
	}

	skel->SetShaderConstants();

	for(int i=0; i<=SLOT_Backpack; i++)
	{
		if(d_disable_backpacks_draw->GetBool() && i==SLOT_Backpack)
			continue;

		DrawSlot((ESlot)i, CharMat, dt, true, first_person, NULL);
	}

	if(dt != DT_AURA)
	{
		D3DXMATRIX world = getWeaponBone(skel, CharMat);
		if(!first_person)
		{
			if(slots_[SLOT_WeaponBackRight].wpn)
			{
				skel->GetBoneWorldTM("Weapon_BackRight", &world, CharMat);
				DrawSlot(SLOT_WeaponBackRight, world, dt, false, first_person, NULL);
			}
			if(slots_[SLOT_WeaponSide].wpn)
			{
				skel->GetBoneWorldTM("Weapon_Side", &world, CharMat);
				DrawSlot(SLOT_WeaponSide, world, dt, false, first_person, NULL);
			}
		}

		if(draw_weapon)
		{
			world = getWeaponBone(skel, CharMat);
			bool skinned = false;
			r3dSkeleton* wpnSkel = NULL;
			Weapon* wpn = slots_[SLOT_Weapon].wpn;
			if(wpn)
			{
				r3dMesh* msh = wpn->getModel(true, first_person);
				if(msh->IsSkeletal() && wpn->getConfig()->getSkeleton())
				{
					wpn->getAnimation()->Recalc();
					wpn->getAnimation()->pSkeleton->SetShaderConstants();
					wpnSkel = wpn->getAnimation()->pSkeleton;
					skinned = true;
				}
			}
			DrawSlot(SLOT_Weapon, world, dt, skinned, first_person, wpnSkel);
		}
	}
}

CUberAnim::CUberAnim(obj_Player* in_player, CUberData* in_data)
{
	player = in_player;
	data_     = in_data;
	r3d_assert(data_->animPool_.Anims.size() > 0);

	extern void _player_AdjustBoneCallback(DWORD dwData, int boneId, D3DXMATRIX &mp, D3DXMATRIX &anim);
	anim.Init(data_->bindSkeleton_, &data_->animPool_, _player_AdjustBoneCallback, (DWORD)in_player);

	reloadAnimTrackID	= INVALID_TRACK_ID;
	recoilAnimTrackID	= INVALID_TRACK_ID;
	turnInPlaceTrackID	= INVALID_TRACK_ID;
	grenadePinPullTrackID	= INVALID_TRACK_ID;
	grenadeThrowTrackID	= INVALID_TRACK_ID;
	//bombPlantingTrackID	= INVALID_TRACK_ID;
	shootAnimTrackID = INVALID_TRACK_ID;
	
	jumpAnimSpeed           = 1.9f;
	jumpStartTime           = 0.3f;
	jumpStartTimeByState[0] = 0.3f; // idle
	jumpStartTimeByState[1] = 0.05f; // not idle
	jumpTrackID             = INVALID_TRACK_ID;
	jumpMoveTrackID         = INVALID_TRACK_ID;
	jumpMoveTrackID2		= INVALID_TRACK_ID;
	jumpState               = -1;
	jumpAirTime             = 0;
	
	scaleReloadAnimTime     = 1;

	FillAnimStatesSpeed();

	AnimPlayerState = -1;
	AnimMoveDir     = -1;
	CurrentWeapon       = 0;
	IsInUI          = false;
	IsSwimming		= false;
}

CUberAnim::~CUberAnim()
{
}

bool CUberAnim::IsFPSMode()
{
	if (player)
	{
		return g_camera_mode->GetInt() == 2 && player->NetworkLocal;
	}
	return false;
}

void CUberAnim::FillAnimStatesSpeed()
{
	// set animation speed for all states - so movement speed will be synched with animation speed
	for(int i=0; i<PLAYER_NUM_STATES; i++) {
		AnimSpeedStates[i] = 1.0f;
	};
	AnimSpeedRunFwd = 1.0f; 
	
#if 1 // animation speed overrides
	AnimSpeedStates[PLAYER_MOVE_CROUCH]     = 1.0f;
	AnimSpeedStates[PLAYER_MOVE_CROUCH_AIM] = 1.0f;
	AnimSpeedStates[PLAYER_MOVE_WALK_AIM]   = 1.0f;
	AnimSpeedStates[PLAYER_MOVE_RUN]        = 1.05f; // MUST BE 3.0f for correct feet placement;
	AnimSpeedStates[PLAYER_MOVE_SPRINT]     = 1.1f;
	AnimSpeedRunFwd                         = 1.05f; // MUST be 2.0 for correct feet placement

#endif
}

int CUberAnim::GetBoneID(const char* Name) const
{
	return anim.pSkeleton->GetBoneID(Name);
}

void CUberAnim::SwitchToState(int PlayerState, int MoveDir)
{
	r3d_assert(MoveDir >= 0 && MoveDir < CUberData::ANIMDIR_COUNT);

	const CUberData::animIndices_s& aid = data_->aid_;
	const int* wids = IsFPSMode()?data_->wpn1_fps:data_->wpn1;
	if(CurrentWeapon && CurrentWeapon->getConfig())
	{
		if(IsFPSMode() && CurrentWeapon->getWeaponAnimID_FPS())
			wids = CurrentWeapon->getWeaponAnimID_FPS();
		else if(CurrentWeapon->getConfig()->m_animationIds)
			wids = CurrentWeapon->getConfig()->m_animationIds;
	}
	
	// override hands weapon in fps idle state to idle aim (with raised hands),
	// as long as they are not swimming
	if(CurrentWeapon && CurrentWeapon->getItemID() == WeaponConfig::ITEMID_UnarmedMelee)
	{
		if(PlayerState == PLAYER_IDLE && IsFPSMode())
			PlayerState = PLAYER_IDLEAIM;
	}
	
	
	// special case for UI_Idle anims
	if(IsInUI)
	{
		if(CurrentWeapon == NULL)
		{
			int aid;
			if(IsSwimming)
				aid = data_->aid_.UI_IdleNoWeapon[1];
			else
				aid = data_->aid_.UI_IdleNoWeapon[0];

			if(anim.AnimTracks.size() > 0)
				anim.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
			else
				anim.StartAnimation(aid, ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
		}
		else
		{
			if(anim.AnimTracks.size() > 0)
				anim.StartAnimation(wids[CUberData::AIDX_UIIdle], ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.2f);
			else
				anim.StartAnimation(wids[CUberData::AIDX_UIIdle], ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
		}
		return;
	}

	// new animation idx
	int a1 = -1;
	int a2 = -1;
	// transition animation idx
	int t1 = -1;
	int t2 = -1;
	// lower/upper current frame numbers if we're switching to same anim
	float f1 = -1;
	float f2 = -1;
	// lower/upper current frame numbers if we're switching to synchronized anim
	float f3 = -1;
	float f4 = -1;
	
	IsSwimming = false;
	switch(PlayerState)
	{
		default:
			r3dOutToLog("Unknown PlayerState=%d\n", PlayerState);
			r3d_assert(0);
			break;

		case PLAYER_DIE:
			return;
			
		case PLAYER_IDLE:
			a1 = wids[CUberData::AIDX_IdleLower];
			a2 = wids[CUberData::AIDX_IdleUpper];
			break;
		case PLAYER_IDLEAIM:
			a1 = wids[CUberData::AIDX_StandLower];
			a2 = wids[CUberData::AIDX_StandUpper];
			break;
		case PLAYER_MOVE_CROUCH:
			a1 = aid.crouch[MoveDir];
			a2 = wids[CUberData::AIDX_CrouchBlend];
			break;
		case PLAYER_MOVE_CROUCH_AIM:
			a1 = aid.crouch[MoveDir];
			a2 = wids[CUberData::AIDX_CrouchAim];
			break;
		case PLAYER_MOVE_PRONE:
			a1 = aid.prone[MoveDir];
			a2 = wids[CUberData::AIDX_ProneBlend];
			if(!CurrentWeapon)
				a2 = -1;
			break;
		case PLAYER_PRONE_AIM:
			a1 = aid.prone[MoveDir];
			a2 = wids[CUberData::AIDX_ProneAim];
			break;
		case PLAYER_PRONE_UP:
			if(CurrentWeapon && CurrentWeapon->getItemID() != WeaponConfig::ITEMID_UnarmedMelee)
			{
				a1 = aid.prone_up_weapon;
				a2 = wids[CUberData::AIDX_ProneBlend];
			}
			else
			{
				a1 = aid.prone_up_noweapon;
				a2 = -1;
			}
			break;
		case PLAYER_PRONE_DOWN:
			if(CurrentWeapon && CurrentWeapon->getItemID() != WeaponConfig::ITEMID_UnarmedMelee)
			{
				a1 = aid.prone_down_weapon;
				a2 = wids[CUberData::AIDX_ProneBlend];
			}
			else
			{
				a1 = aid.prone_down_noweapon;
				a2 = -1;
			}
			break;
		case PLAYER_PRONE_IDLE:
			a1 = aid.prone[MoveDir];
			a2 = wids[CUberData::AIDX_IdleProne];
			break;
		case PLAYER_MOVE_WALK_AIM:
			a1 = aid.walk[MoveDir];
			a2 = wids[CUberData::AIDX_WalkAim];
			break;
		case PLAYER_MOVE_RUN:
			a1 = aid.run[MoveDir];
			a2 = wids[CUberData::AIDX_RunBlend];
			//ANIM_HACK: because we can move in air, we can switch from to any moving direction in this pose
			if(a1 == -1)
				a1 = aid.run[CUberData::ANIMDIR_Str];
			break;
		case PLAYER_MOVE_SPRINT:
			a1 = aid.sprint[MoveDir];
			a2 = wids[CUberData::AIDX_SprintBlend];
			//ANIM_HACK: because we can move in air, we can switch from to any moving direction in this pose
			if(a1 == -1)
				a1 = aid.sprint[CUberData::ANIMDIR_Str];
			if( PLAYER_SWIM_FAST == AnimPlayerState )
			{
				t1 = IsFPSMode()
					? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_FPSSwimToSprint]
					: aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimToSprint];
				t2 = -1; //IsFPSMode()
					//? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_FPSSwimToSprint]
					//: aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimToSprint];
			}
			break;
		// Don't need to load Upper anims for Third-Person, since swims are all
		// Full body anims, and loading them twice only produces synchronization
		// issues with resetting the ai.fCurFrame to 0 when it's restarted in upper.
		case PLAYER_SWIM_IDLE:
			IsSwimming = true;
			a1 = aid.swimLower[MoveDir];
			a2 = IsFPSMode() ? aid.swimFPSUpper[MoveDir] : -1; // aid.swimUpper[MoveDir];
			if( PLAYER_SWIM == AnimPlayerState || PLAYER_SWIM_FAST == AnimPlayerState )
			{
				if( CUberData::ANIMDIR_Str == AnimMoveDir || CUberData::ANIMDIR_StrLeft == AnimMoveDir || CUberData::ANIMDIR_StrRight == AnimMoveDir )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimToSwimIdle] : -1;
					t2 = -1;
				}
				else if( CUberData::ANIMDIR_Back == AnimMoveDir || CUberData::ANIMDIR_BackLeft == AnimMoveDir || CUberData::ANIMDIR_BackRight == AnimMoveDir )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimBackToSwimIdle] : -1;
					t2 = -1;
				}
				else if( CUberData::ANIMDIR_Left == AnimMoveDir )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimLeftToSwimIdle] : -1;
					t2 = -1;
				}
				else if( CUberData::ANIMDIR_Right == AnimMoveDir )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimRightToSwimIdle] : -1;
					t2 = -1;
				}
			}
			break;
		case PLAYER_SWIM_SLOW:
			IsSwimming = true;
			a1 = aid.swimSlowLower[MoveDir];
			a2 = IsFPSMode() ? aid.swimFPSSlowUpper[MoveDir] : -1; // aid.swimSlowUpper[MoveDir];
			break;
		case PLAYER_SWIM:
			IsSwimming = true;
			a1 = aid.swimLower[MoveDir];
			a2 = IsFPSMode() ? aid.swimFPSUpper[MoveDir] : -1; // aid.swimUpper[MoveDir];
			if( PLAYER_SWIM_IDLE == AnimPlayerState )
			{
				if( CUberData::ANIMDIR_Str == MoveDir || CUberData::ANIMDIR_StrLeft == MoveDir || CUberData::ANIMDIR_StrRight == MoveDir )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwim] : -1;
					t2 = -1;
				}
				else if( CUberData::ANIMDIR_Back == MoveDir || CUberData::ANIMDIR_BackLeft == MoveDir || CUberData::ANIMDIR_BackRight == MoveDir )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwimBack] : -1;
					t2 = -1;
				}
				else if( CUberData::ANIMDIR_Left == MoveDir  )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwimLeft] : -1;
					t2 = -1;
				}
				else if( CUberData::ANIMDIR_Right == MoveDir  )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwimRight] : -1;
					t2 = -1;
				}
			}
			else if( PLAYER_SWIM == AnimPlayerState )
			{
				if( CUberData::ANIMDIR_Left == AnimMoveDir && CUberData::ANIMDIR_Right == MoveDir )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimLeftToSwimRight] : -1;
					t2 = -1;
				}
				else if( CUberData::ANIMDIR_Right == AnimMoveDir && CUberData::ANIMDIR_Left == MoveDir )
				{
					t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimRightToSwimLeft] : -1;
					t2 = -1;
				}
			}
			break;
		case PLAYER_SWIM_FAST:
			IsSwimming = true;
			a1 = aid.swimFastLower[MoveDir];
			a2 = IsFPSMode() ? aid.swimFPSFastUpper[MoveDir] : -1; // aid.swimFastUpper[MoveDir];
			if( PLAYER_MOVE_SPRINT == AnimPlayerState )
			{
				t1 = IsFPSMode()
					? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_FPSSprintToSwim]
					: aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SprintToSwim];
				t2 = -1; //IsFPSMode()
					//? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_FPSSprintToSwim]
					//: aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SprintToSwim];
			}
			else if( PLAYER_SWIM_IDLE == AnimPlayerState )
			{
				t1 = (!IsFPSMode()) ? aid.swimTransitions[CUberData::animIndices_s::SWIMTRANS_SwimIdleToSwim] : -1;
				t2 = -1;
			}
			break;
#ifdef VEHICLES_ENABLED
		case PLAYER_VEHICLE_DRIVER:
			IsVehicleDriver = true;
			a1 = aid.vehicleDriver[MoveDir];
			break;
		case PLAYER_VEHICLE_PASSENGER:
			IsVehiclePassenger = true;
			a1 = aid.vehiclePassenger[0];
			break;
#endif
	}

	//r3d_assert(a1 >= 0);
	//r3d_assert(a2 >= 0);
	if(a1 == -1 && !IsFPSMode()) {	
		r3dOutToLog("no animation for state %d, dir: %d\n", PlayerState, MoveDir);
		a2 = -1;
	}

	// there was no animations, start new
	if(anim.AnimTracks.size() == 0) 
	{
		if(!IsFPSMode()) // do not play lower body anim in FPS mode
			anim.StartAnimation(a1, ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
		anim.StartAnimation(a2, ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
			
		return;
	}

	// set animation speed based on state
	float fAnimSpeed = AnimSpeedStates[PlayerState];
	if(PlayerState == PLAYER_MOVE_RUN && MoveDir == CUberData::ANIMDIR_Str)
		fAnimSpeed = AnimSpeedRunFwd;

	float inf1 = 0.0f; // start influence
	float inf2 = 1.0f; // end influence
	float inf3 = 0.1f; // time to blend

	r3dgameVector(r3dAnimation::r3dAnimInfo)::iterator it;
	
	// create two stacks for lower & upper animations
	r3dgameVector(r3dAnimation::r3dAnimInfo) lower;
	r3dgameVector(r3dAnimation::r3dAnimInfo) upper;
	r3dgameVector(r3dAnimation::r3dAnimInfo) top;
	r3dAnimation::r3dAnimInfo              jumpAnim;
	
	for(it=anim.AnimTracks.begin(); it!=anim.AnimTracks.end(); ++it) 
	{
		const r3dAnimation::r3dAnimInfo& ai = *it;
		if(ai.iTrackId == grenadePinPullTrackID || ai.iTrackId == grenadeThrowTrackID)
			top.push_back(ai);
//		else if(ai.iTrackId == bombPlantingTrackID)
//			top.push_back(ai);
		if(ai.iTrackId == reloadAnimTrackID)
			top.push_back(ai);
		else if(ai.iTrackId == shootAnimTrackID)
			top.push_back(ai);
		else if(ai.iTrackId == jumpTrackID)
			jumpAnim = ai;
		else if(ai.pAnim->pTracks[0].bEnabled)
			lower.push_back(ai);
		else
			upper.push_back(ai);
	}
	
	// expire all previous animations but be on lookout for same animation
	for(it = lower.begin(); it!=lower.end(); ++it)
	{
		r3dAnimation::r3dAnimInfo& ai = *it;
		if((ai.dwStatus & ANIMSTATUS_Expiring))
			continue;
		if(
			ai.pAnim->iAnimId == a1 || 
			( (PlayerState == PLAYER_PRONE_UP || PlayerState == PLAYER_PRONE_DOWN) && (ai.pAnim->iAnimId == aid.prone_down_weapon || ai.pAnim->iAnimId == aid.prone_up_weapon || ai.pAnim->iAnimId == aid.prone_down_noweapon || ai.pAnim->iAnimId == aid.prone_up_noweapon) ) 
		  )
		{
			// same lower body anim
			a1 = -1;
			f1 = ai.fCurFrame;

			// update speed for aim/walk states
			ai.fSpeed = fAnimSpeed;
			continue;
		}
		// different anim, but they are synchronized, so save the frame
		if( ( ( AnimPlayerState == PLAYER_SWIM_SLOW && PlayerState == PLAYER_SWIM_SLOW ) ||
			  ( AnimPlayerState == PLAYER_SWIM && PlayerState == PLAYER_SWIM ) ||
			  ( AnimPlayerState == PLAYER_SWIM_FAST && PlayerState == PLAYER_SWIM_FAST )
			) &&
			( ( ( AnimMoveDir == CUberData::ANIMDIR_Str && ( MoveDir == CUberData::ANIMDIR_StrLeft || MoveDir == CUberData::ANIMDIR_StrRight ) ) ||
				( AnimMoveDir == CUberData::ANIMDIR_StrLeft && ( MoveDir == CUberData::ANIMDIR_Str || MoveDir == CUberData::ANIMDIR_StrRight ) ) ||
				( AnimMoveDir == CUberData::ANIMDIR_StrRight && ( MoveDir == CUberData::ANIMDIR_Str || MoveDir == CUberData::ANIMDIR_StrLeft ) )
			  ) ||
			  ( ( AnimMoveDir == CUberData::ANIMDIR_Back && ( MoveDir == CUberData::ANIMDIR_BackLeft || MoveDir == CUberData::ANIMDIR_BackRight ) ) ||
				( AnimMoveDir == CUberData::ANIMDIR_BackLeft && ( MoveDir == CUberData::ANIMDIR_Back || MoveDir == CUberData::ANIMDIR_BackRight ) ) ||
				( AnimMoveDir == CUberData::ANIMDIR_BackRight && ( MoveDir == CUberData::ANIMDIR_Back || MoveDir == CUberData::ANIMDIR_BackLeft ) )
			  )
			)
		  )
		{
			f3 = ai.fCurFrame;
		}
#ifdef VEHICLES_ENABLED
		if ( AnimPlayerState == PLAYER_VEHICLE_DRIVER || AnimPlayerState == PLAYER_VEHICLE_PASSENGER)
		{
			f3 = ai.fCurFrame;
		}
#endif
		ai.dwStatus    |= ANIMSTATUS_Expiring;
		ai.fExpireTime  = inf3;
	}

	for(it = upper.begin(); it!=upper.end(); ++it)
	{
		r3dAnimation::r3dAnimInfo& ai = *it;
		if((ai.dwStatus & ANIMSTATUS_Expiring))
			continue;
		if(ai.pAnim->iAnimId == a2) 
		{
			// same upper body
			a2 = -1;
			f2 = ai.fCurFrame;

			// update speed for aim/walk states
			ai.fSpeed = fAnimSpeed;
			continue;
		}
		// different anim, but they are synchronized, so save the frame
		if( ( ( AnimPlayerState == PLAYER_SWIM_SLOW && PlayerState == PLAYER_SWIM_SLOW ) ||
			  ( AnimPlayerState == PLAYER_SWIM && PlayerState == PLAYER_SWIM ) ||
			  ( AnimPlayerState == PLAYER_SWIM_FAST && PlayerState == PLAYER_SWIM_FAST )
			) &&
			( ( ( AnimMoveDir == CUberData::ANIMDIR_Str && ( MoveDir == CUberData::ANIMDIR_StrLeft || MoveDir == CUberData::ANIMDIR_StrRight ) ) ||
				( AnimMoveDir == CUberData::ANIMDIR_StrLeft && ( MoveDir == CUberData::ANIMDIR_Str || MoveDir == CUberData::ANIMDIR_StrRight ) ) ||
				( AnimMoveDir == CUberData::ANIMDIR_StrRight && ( MoveDir == CUberData::ANIMDIR_Str || MoveDir == CUberData::ANIMDIR_StrLeft ) )
			  ) ||
			  ( ( AnimMoveDir == CUberData::ANIMDIR_Back && ( MoveDir == CUberData::ANIMDIR_BackLeft || MoveDir == CUberData::ANIMDIR_BackRight ) ) ||
				( AnimMoveDir == CUberData::ANIMDIR_BackLeft && ( MoveDir == CUberData::ANIMDIR_Back || MoveDir == CUberData::ANIMDIR_BackRight ) ) ||
				( AnimMoveDir == CUberData::ANIMDIR_BackRight && ( MoveDir == CUberData::ANIMDIR_Back || MoveDir == CUberData::ANIMDIR_BackLeft ) )
			  )
			)
		  )
		{
			f4 = ai.fCurFrame;
		}
		ai.dwStatus    |= ANIMSTATUS_Expiring;
		ai.fExpireTime  = inf3;
	}
	
	//ANIM_HACK: randomize IDLE animation frame
	float fIdleAnimFrame = 0;
	if(PlayerState == PLAYER_IDLE)
		fIdleAnimFrame = u_GetRandom(0.0f, 999.0f);
		
	// reassemble animation stack and add new ones
	anim.AnimTracks.clear();
		
	//
	// 1. add lower anims
	//
	for(it = lower.begin(); it!=lower.end(); ++it)	
	{
		anim.AnimTracks.push_back(*it);
	}
	
	// start new lower anim
	if(a1 >= 0 && !IsFPSMode()) // do not play lower body anim in FPS mode
	{
		uint32_t animFlag = ANIMFLAG_Looped;
		if(PlayerState==PLAYER_PRONE_DOWN || PlayerState == PLAYER_PRONE_UP)
			animFlag = ANIMFLAG_PauseOnEnd;
		anim.StartAnimation(a1, animFlag, inf1, inf2, inf3);
		r3dAnimation::r3dAnimInfo& ai = anim.AnimTracks[anim.AnimTracks.size() - 1];

		// sync changed lower body anim with saved upper body frame
		if(f2 >= 0)
			ai.fCurFrame = f2;

		// sync changed lower body anim with new lower body anim
		if(f3 >= 0)
			ai.fCurFrame = f3;

		if(PlayerState == PLAYER_IDLE)
			ai.fCurFrame = fIdleAnimFrame;
			
		ai.fSpeed = fAnimSpeed;
	}

	// if jump present, it must be last of lower anims
	if(jumpAnim.pAnim) 
	{
		anim.AnimTracks.push_back(jumpAnim);
	}

	// Transitions must be last of lower anims.  Since there is no transition to jump atm, jump will not be affected.
	if( t1 != -1 )
	{
		uint32_t animFlag = 0;
		anim.StartAnimation(t1, animFlag, inf1, inf2, inf3);
		r3dAnimation::r3dAnimInfo& ai = anim.AnimTracks[anim.AnimTracks.size() - 1];
		ai.fCurFrame = 0;
		ai.fSpeed = fAnimSpeed;
	}

	//
	// 2. add uppers
	//
	for(it = upper.begin(); it!=upper.end(); ++it) 
	{
		anim.AnimTracks.push_back(*it);
	}

	if(a2 >= 0) 
	{
		anim.StartAnimation(a2, ANIMFLAG_Looped, inf1, inf2, inf3);
		r3dAnimation::r3dAnimInfo& ai = anim.AnimTracks[anim.AnimTracks.size() - 1];

		// sync changed upper body anim with saved lower body frame
		if(f1 >= 0)
			ai.fCurFrame = f1;

		// sync changed upper body anim with new upper body anim
		if(f4 >= 0)
			ai.fCurFrame = f4;

		if(PlayerState == PLAYER_IDLE)
			ai.fCurFrame = fIdleAnimFrame;
			
		ai.fSpeed = fAnimSpeed;
	}
	

	//ANIM_HACK freeze/unfree crouch upper body at frame 0 in crouch standing poses
	// twisted logic here that because of our animation, upper walking crouch anim does not sync with lower body stand crouch
	if(!IsFPSMode()) // do not play lower body crouch in FPS mode
	{
		if(PlayerState == PLAYER_MOVE_CROUCH || PlayerState == PLAYER_MOVE_CROUCH_AIM)
		{
			// need to scan all animations, because switching from crouch_aim move to stand does not restart upper body anim
			for(size_t i = 0, size = anim.AnimTracks.size(); i < size; i++)
			{
				r3dAnimation::r3dAnimInfo& ai = anim.AnimTracks[i];
				if(ai.pAnim->iAnimId == wids[CUberData::AIDX_CrouchAim] || ai.pAnim->iAnimId == wids[CUberData::AIDX_CrouchBlend])
				{
					if(MoveDir == CUberData::ANIMDIR_Stand)
					{
						ai.fCurFrame  = 0;
						ai.dwStatus  |= ANIMSTATUS_Paused;
					}
					else
					{
						ai.dwStatus  &= ~ANIMSTATUS_Paused;
					}
				}
			}
		}
	}

	//
	// 3. add all top anims
	//
	for(it = top.begin(); it!=top.end(); ++it) {
		anim.AnimTracks.push_back(*it);
	}
	
	// Transitions must be last of upper anims.  Since there is no transition to jump atm, jump will not be affected.
	if( t2 != -1 )
	{
		uint32_t animFlag = 0;
		anim.StartAnimation(t2, animFlag, inf1, inf2, inf3);
		r3dAnimation::r3dAnimInfo& ai = anim.AnimTracks[anim.AnimTracks.size() - 1];
		ai.fCurFrame = 0;
		ai.fSpeed = fAnimSpeed;
	}
	
	/*r3dOutToLog("NEW STACK\n");
        for(size_t i=0; i<anim.AnimTracks.size(); i++) 
        {
          const r3dAnimation::r3dAnimInfo& ai = anim.AnimTracks[i];
	  r3dOutToLog("%p:%s: %.2f, f:%.1f, %04X\n", 
	    ai.pAnim, ai.pAnim->pAnimName, ai.fInfluence, ai.fCurFrame, ai.dwStatus);
	}*/
	
		
	return;
}

static bool isNeedToSkipWeaponSwitch(const Weapon* wpn)
{
	if(!wpn)
		return false;

	int cat = wpn->getCategory();
	return(cat == storecat_UsableItem || cat == storecat_GRENADE);
}

void CUberAnim::SyncAnimation(int PlayerState, int MoveDir, bool force, const Weapon* weap, bool isInAttmMenu)
{
	if(weap)
	{
		// switch state if we firing in idle mode
		if(PlayerState == PLAYER_IDLE && ((r3dGetTime() < weap->getLastTimeFired() + 5.0f) && !IsFPSMode())) // 5sec delay before returning back to idle
			PlayerState = PLAYER_IDLEAIM;

		// switch state if we firing in idle mode
		if(PlayerState == PLAYER_PRONE_IDLE && ((r3dGetTime() < weap->getLastTimeFired() + 5.0f) && !IsFPSMode())) // 5sec delay before returning back to idle
			PlayerState = PLAYER_PRONE_AIM;

// 		// if throwing grenades from idle, switch to idleaim
 		if(PlayerState == PLAYER_IDLE && (grenadePinPullTrackID != INVALID_TRACK_ID))
 			PlayerState = PLAYER_IDLEAIM;
		
		// if weapon was changed, recreate animation indices
		if(CurrentWeapon != weap)
		{
			// disable animation switch to/from grenades because of weird animatino transitions
			if(IsFPSMode() || IsInUI)
			{
				if(isNeedToSkipWeaponSwitch(weap) || isNeedToSkipWeaponSwitch(CurrentWeapon))
					anim.AnimTracks.clear();
			}

			CurrentWeapon = weap;
			force     = true;
		}
	}
	else
	{
		if(IsInUI && (CurrentWeapon != weap || (weap == NULL && anim.AnimTracks.size() == 0)))
		{
			CurrentWeapon = NULL;
			if(IsSwimming)
				anim.StartAnimation(data_->aid_.UI_IdleNoWeapon[1], ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
			else
				anim.StartAnimation(data_->aid_.UI_IdleNoWeapon[0], ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
			return;
		}

		CurrentWeapon = NULL;
	}

#ifdef VEHICLES_ENABLED	
	if (PlayerState == PLAYER_VEHICLE_DRIVER)
	{
		anim.StartAnimation(data_->aid_.vehicleDriver[MoveDir], ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
	}
	if ( PlayerState == PLAYER_VEHICLE_PASSENGER )
	{
		anim.StartAnimation(data_->aid_.vehiclePassenger[0], ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
	}
#endif

	if(isInAttmMenu && CurrentWeapon && IsFPSMode()
		&& !IsSwimming && PlayerState != PLAYER_SWIM_IDLE && PlayerState != PLAYER_SWIM_SLOW && PlayerState != PLAYER_SWIM && PlayerState != PLAYER_SWIM_FAST
		)
	{
		bool playingRiseAnim = false;
		bool finishedRiseAnim = true;
		bool playingIdleAnim = false;

		int animIdx = 0;
		switch(CurrentWeapon->getCategory())
		{
		case storecat_ASR:
			animIdx = 0;
			break;
		case storecat_SNP:
			animIdx = 1;
			break;
		case storecat_SHTG:
			animIdx = 2;
			break;
		case storecat_MG:
			animIdx = 3;
			break;
		case storecat_HG:
			animIdx = 4;
			break;
		case storecat_SMG:
			animIdx = 5;
			break;
		default:
			r3d_assert(false);
			break;
		}

		r3dgameVector(r3dAnimation::r3dAnimInfo)::iterator it;
		for(it=anim.AnimTracks.begin(); it!=anim.AnimTracks.end(); ++it) 
		{
			const r3dAnimation::r3dAnimInfo& ai = *it;
			if(ai.pAnim->iAnimId == data_->aid_.attmMenuRiseWeapon[animIdx])
			{
				playingRiseAnim = true;
				if((ai.dwStatus & ANIMSTATUS_Finished))
					finishedRiseAnim = true;
				break;
			}
			else if(ai.pAnim->iAnimId == data_->aid_.attmMenuIdleWeapon[animIdx])
			{
				playingIdleAnim = true;
				break;
			}
		}
		
		if(!playingIdleAnim && !playingRiseAnim) // just entered menu, play rise anim
			anim.StartAnimation(data_->aid_.attmMenuRiseWeapon[animIdx], ANIMFLAG_RemoveOtherFade | ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
		else if(finishedRiseAnim && !playingIdleAnim)
			anim.StartAnimation(data_->aid_.attmMenuIdleWeapon[animIdx], ANIMFLAG_RemoveOtherNow | ANIMFLAG_Looped, 0.0f, 1.0f, 0.1f);
		else // playing idle anim
		{
			// do nothing :)
		}

		AnimPlayerState = -1; // reset, so that after attm menu we will return back to proper animation
		return;
	}


	// switch anim state
	if(AnimPlayerState == PlayerState && AnimMoveDir == MoveDir && !force)
		return;

	SwitchToState(PlayerState, MoveDir);
	AnimPlayerState = PlayerState;
	AnimMoveDir     = MoveDir;

	//r3dOutToLog("%s anim -> %d, s:%d, a:%d\n", Name.c_str(), aid, PlayerState, anim.AnimTracks.size());
}

void CUberAnim::StartRecoilAnim()
{
	if(IsFPSMode())
	{
		/*
		int animType = m_WeaponArray[m_SelectedWeapon]->getAnimType();
		if(animType == WPN_ANIM_GRENADE)
		return;

		r3dAnimation::r3dAnimInfo* animInfo = uberAnim.GetTrack(animState_.recoilAnimTrackID);
		if(animInfo && (animInfo->GetStatus()&ANIMSTATUS_Playing))
		return; // let it finish before starting a new one

		r3d_assert(m_WeaponArray[m_SelectedWeapon]);

		anim.Stop( recoilAnimTrackID );
		int aid = uchar->GetRecoilAnimId( animType );
		recoilAnimTrackID = anim.StartAnimation(aid, 0, 0.0f, 1.0f, 0.1f);
		*/	
	}
}


void CUberAnim::StartTurnInPlaceAnim()
{
	if(IsFPSMode()) // no need for turn in place in FPS mode
		return;

	anim.Stop(turnInPlaceTrackID);

	int aid = data_->aid_.turnins[0];
	if(AnimPlayerState == PLAYER_MOVE_CROUCH || AnimPlayerState == PLAYER_MOVE_CROUCH_AIM)
	{
		aid = data_->aid_.turnins[CUberData::animIndices_s::ANIMTURNIN_CROUCH];
	}
	else if(AnimPlayerState == PLAYER_SWIM_SLOW)
	{
		aid = data_->aid_.turnins[CUberData::animIndices_s::ANIMTURNIN_SWIM];
	}
	turnInPlaceTrackID = anim.StartAnimation(aid, 0, 0.0f, 1.0f, 0.1f);

	// we play turn-in-place anim only at idle/crouch position. so right now we must have at least 3 tracks
	if(anim.AnimTracks.size() >= 3) {
		// move turn-in to 2nd lower body anim place
		r3dAnimation::r3dAnimInfo ai = anim.AnimTracks.back(); 
		anim.AnimTracks.pop_back();
		anim.AnimTracks.insert(anim.AnimTracks.begin() + 1, ai);
	}
}


void CUberAnim::StopTurnInPlaceAnim()
{
	if(IsFPSMode()) // no need for turn in place in FPS mode
		return;

	if(turnInPlaceTrackID != INVALID_TRACK_ID)
	{
		anim.FadeOut(turnInPlaceTrackID, 0.1f);
		turnInPlaceTrackID = INVALID_TRACK_ID;
	}
}

void CUberAnim::UpdateTurnInPlaceAnim()
{
	if(IsFPSMode()) // no need for turn in place in FPS mode
		return;

	if(anim.GetTrack(turnInPlaceTrackID) == NULL)
	{
		turnInPlaceTrackID = CUberAnim::INVALID_TRACK_ID;
	}
}

/*void CUberAnim::StartBombPlantingAnimation(int bombState) // bombState: 0-start, 1-loop, 2-end, 3-stop everything
{
	if(bombPlantingTrackID != INVALID_TRACK_ID)
	{
		anim.FadeOut(bombPlantingTrackID, 0.1f);
		bombPlantingTrackID = INVALID_TRACK_ID;
	}

	const int* bidx = IsFPSMode() ? data_->aid_.bombs_fps : data_->aid_.bombs_tps;
	if(bombState == 0)
		bombPlantingTrackID = anim.StartAnimation(bidx[0], ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
	else if(bombState == 1)
		bombPlantingTrackID = anim.StartAnimation(bidx[1], ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
	else if(bombState == 2)
		bombPlantingTrackID = anim.StartAnimation(bidx[2], ANIMFLAG_PauseOnEnd, 1.0f, 1.0f, 0.0f);
}*/

float CUberAnim::GetGrenadeLaunchFrame()
{
	if(!CurrentWeapon)
		return 0.0f;
		
	const static float GrenadeLaunchFrames[][2] = 
	{
		 {4, 2}, //PLAYER_IDLE,
		 {4, 2}, //PLAYER_IDLEAIM,
		 {4, 2}, //PLAYER_MOVE_CROUCH,
		 {4, 2}, //PLAYER_MOVE_CROUCH_AIM,
		 {4, 2}, //PLAYER_MOVE_WALK_AIM,
		 {4, 2}, //PLAYER_MOVE_RUN,
		 {4, 2}, //PLAYER_MOVE_SPRINT,
		 {0, 0}, //PLAYER_MOVE_PRONE,
		 {0, 0}, //PLAYER_PRONE_AIM,
		 {0, 0}, //PLAYER_PRONE_UP,
		 {0, 0}, //PLAYER_PRONE_DOWN,
		 {0, 0}, //PLAYER_PRONE_IDLE,
		 {0, 0}, //PLAYER_SWIM_IDLE,
		 {0, 0}, //PLAYER_SWIM_SLOW,
		 {0, 0}, //PLAYER_SWIM,
		 {0, 0}, //PLAYER_SWIM_FAST,
#ifdef VEHICLES_ENABLED
		 {0, 0}, //VEHICLE DRIVER
		 {0, 0}, //VEHICLE PASSENGER
#endif
		 {0, 0}, //PLAYER_DIE,
	};
	COMPILE_ASSERT( R3D_ARRAYSIZE(GrenadeLaunchFrames) == PLAYER_NUM_STATES ) ;

//	int type = CurrentWeapon->getConfig()->getGrenadeAnimType();
	int fps  = IsFPSMode() ? 1 : 0;
	
	// grenade
//	if(type == WeaponConfig::GRENADE_ANIM_Normal)
		return GrenadeLaunchFrames[AnimPlayerState][fps];
	
	// mines
// 	const static float MineLaunchFrames[WeaponConfig::GRENADE_ANIM_LASTTYPE][2] = {
// 	  {0,  0}, //GRENADE_ANIM_Normal,
// 	  {37, 37}, //GRENADE_ANIM_Claymore
// 	  {37, 37}, //GRENADE_ANIM_VS50
// 	  {37, 37}, //GRENADE_ANIM_V69
// 	};
// 	r3d_assert(type < R3D_ARRAYSIZE(MineLaunchFrames));
// 	return MineLaunchFrames[type][fps];
}

void CUberAnim::StartGrenadePinPullAnimation()
{
	// skip pinpull anim for mines
// 	if(CurrentWeapon && CurrentWeapon->getCategory() == storecat_GRENADES)
// 	{
// 		if(CurrentWeapon->getConfig()->getGrenadeAnimType() != WeaponConfig::GRENADE_ANIM_Normal)
// 		{
// 			grenadePinPullTrackID = 0xF000000;	// make some temporary track id
// 			return;
// 		}
// 	}

	int grIdx = data_->GetGrenadeAnimId(IsFPSMode(), AnimPlayerState, 0);
	
	anim.Stop(grenadePinPullTrackID);
	grenadePinPullTrackID = anim.StartAnimation(grIdx, ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
}

bool CUberAnim::IsGrenadePinPullActive()
{
	return grenadePinPullTrackID != INVALID_TRACK_ID ;
}

bool CUberAnim::IsGrenadePinPullFinished()
{
	r3dAnimation::r3dAnimInfo* ai = anim.GetTrack(grenadePinPullTrackID);
	if(!ai)
		return true;
	return (ai->GetStatus() & ANIMSTATUS_Finished) ? true : false;
}

void CUberAnim::StartGrenadeThrowAnimation()
{
	StopGrenadeAnimations();

// 	// check for mine special case
// 	if(CurrentWeapon && CurrentWeapon->getConfig()->isMine())
// 	{
// 		const int* bidx = IsFPSMode() ? data_->aid_.bombs_fps : data_->aid_.bombs_tps;
// 		int atype = CurrentWeapon->getConfig()->getGrenadeAnimType();
// 		int grIdx1 = bidx[2 + atype];
// 
// 		grenadeThrowTrackID = anim.StartAnimation(grIdx1, 0, 0.0f, 1.0f, 0.1f);
// 		return;
// 	}
	
	// note, no fading in, start with full influence
	int grIdx = data_->GetGrenadeAnimId(IsFPSMode(), AnimPlayerState, 2);
	grenadeThrowTrackID = anim.StartAnimation(grIdx, 0, 1.0f, 1.0f, 0.0f);
}

void CUberAnim::StopGrenadeAnimations()
{
	if(grenadePinPullTrackID != INVALID_TRACK_ID)
	{
		anim.Stop(grenadePinPullTrackID);
		grenadePinPullTrackID = INVALID_TRACK_ID;
	}

	if(grenadeThrowTrackID != INVALID_TRACK_ID)
	{
		anim.Stop(grenadeThrowTrackID);
		grenadeThrowTrackID = INVALID_TRACK_ID;
	}
}

bool CUberAnim::IsGrenadeLaunched()
{
	if(CurrentWeapon && grenadeThrowTrackID != INVALID_TRACK_ID)
	{
		r3dAnimation::r3dAnimInfo* ai = anim.GetTrack(grenadeThrowTrackID);
		if(ai && (int)ai->fCurFrame > GetGrenadeLaunchFrame())
			return true;
	}
		
	return false;
}

int CUberAnim::GetGrenadeAnimState()
{
	const r3dAnimation::r3dAnimInfo* ai1 = anim.GetTrack(grenadePinPullTrackID);
	if(ai1)
		return 1;

	const r3dAnimation::r3dAnimInfo* ai2 = anim.GetTrack(grenadeThrowTrackID);
	if(ai2)
		return 2;
		
	return 0;
}

void CUberAnim::StartReloadAnim()
{
	// check if we already have reloading animation
	r3dAnimation::r3dAnimInfo* animInfo = anim.GetTrack(reloadAnimTrackID);
	if(animInfo && (animInfo->GetStatus()&ANIMSTATUS_Playing)) {
		return;
	}

	if(!CurrentWeapon) {
		return;
	}
		
	// no reload for grenades or mines.
	if(CurrentWeapon->getConfig()->m_AnimType == WPN_ANIM_GRENADE || CurrentWeapon->getConfig()->m_AnimType == WPN_ANIM_MINE ) {
		return;
	}

	const int* wids = IsFPSMode()?data_->wpn1_fps:data_->wpn1;
	if(CurrentWeapon)
	{
		if(IsFPSMode() && CurrentWeapon->getWeaponAnimID_FPS())
			wids = CurrentWeapon->getWeaponAnimID_FPS();
		else if(CurrentWeapon->getConfig()->m_animationIds)
			wids = CurrentWeapon->getConfig()->m_animationIds;
	}
	
	// we have different upper reload blends for different states
	int reloadIdx;
	switch(AnimPlayerState)
	{
		default:
			reloadIdx = CUberData::AIDX_ReloadWalk;
			break;
		case PLAYER_IDLE:
			reloadIdx = CUberData::AIDX_ReloadIdle;
			break;
		case PLAYER_MOVE_CROUCH:
		case PLAYER_MOVE_CROUCH_AIM:
			reloadIdx = CUberData::AIDX_ReloadCrouch;
			break;
		case PLAYER_MOVE_PRONE:
		case PLAYER_PRONE_AIM:
		case PLAYER_PRONE_IDLE:
			reloadIdx = CUberData::AIDX_ReloadProne;
			break;
	}

	anim.Stop(reloadAnimTrackID);
	reloadAnimTrackID = anim.StartAnimation(wids[reloadIdx], 0, 0.0f, 1.0f, 0.1f);

	// scale reload anim to match weapon reload time
	r3dAnimation::r3dAnimInfo* ai = anim.GetTrack(reloadAnimTrackID);
	if(scaleReloadAnimTime && ai && CurrentWeapon->getReloadTime() > 0)
	{
		float animTime = (float)ai->pAnim->NumFrames / ai->pAnim->fFrameRate;
		float k = animTime / CurrentWeapon->getReloadTime();
		ai->SetSpeed(k);
	}
}

void CUberAnim::StopReloadAnim()
{
	if(reloadAnimTrackID != INVALID_TRACK_ID)
	{
		anim.Stop(reloadAnimTrackID);
		reloadAnimTrackID = INVALID_TRACK_ID;
	}
}

void CUberAnim::StartShootAnim()
{
	if(!CurrentWeapon)
		return;

	if(CurrentWeapon->getCategory() == storecat_GRENADE)
		return;

	// play shoot anim only in FPS mode
	if(!IsFPSMode() && CurrentWeapon->getCategory() != storecat_MELEE) // for melee we need to play fire anim all the time. Not sure why in TPS we don't want to play fire anim
		return;
		
	const int* wids = IsFPSMode()?data_->wpn1_fps:data_->wpn1;
	if(CurrentWeapon)
	{
		if(IsFPSMode() && CurrentWeapon->getWeaponAnimID_FPS())
			wids = CurrentWeapon->getWeaponAnimID_FPS();
		else if(CurrentWeapon->getConfig()->m_animationIds)
			wids = CurrentWeapon->getConfig()->m_animationIds;
	}

	int shootAnimIdx;
	switch(AnimPlayerState)
	{
	default:
		shootAnimIdx = CUberData::AIDX_ShootWalk;
		break;
	case PLAYER_MOVE_CROUCH_AIM:
	case PLAYER_IDLEAIM:
	case PLAYER_MOVE_WALK_AIM:
	case PLAYER_PRONE_AIM:
		shootAnimIdx = CUberData::AIDX_ShootAim;
		break;
	case PLAYER_MOVE_CROUCH:
		shootAnimIdx = CUberData::AIDX_ShootCrouch;
		break;
	case PLAYER_MOVE_PRONE:
	case PLAYER_PRONE_IDLE:
		shootAnimIdx = CUberData::AIDX_ShootProne;
		break;
	}
	
	// override for hands-to-hands anims
	if(CurrentWeapon && CurrentWeapon->getItemID() == WeaponConfig::ITEMID_UnarmedMelee)
	{
		// not remember why, but for normal weapons proneaim is different state
		if(AnimPlayerState == PLAYER_PRONE_AIM)
			shootAnimIdx = CUberData::AIDX_ShootProne;

		int idx = u_random(3) + (IsFPSMode() ? 3 : 0); // there is 3 anims per fps/tps
		
		anim.Stop(shootAnimTrackID);
		shootAnimTrackID = anim.StartAnimation(data_->wpn_hands[shootAnimIdx][idx], ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
		return;
	}

	// check if we already have this shoot animation
	r3dAnimation::r3dAnimInfo* animInfo = anim.GetTrack(shootAnimTrackID);
	if(animInfo && animInfo->GetAnim())
	{
		if(animInfo->GetAnim()->iAnimId == wids[shootAnimIdx])
		{
			updateShootAnim(false);
			return;
		}
	}
	
	anim.Stop(shootAnimTrackID);
	shootAnimTrackID = anim.StartAnimation(wids[shootAnimIdx], ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
}

void CUberAnim::updateShootAnim(bool disable_loop)
{
	r3dAnimation::r3dAnimInfo* animInfo = anim.GetTrack(shootAnimTrackID);
	if(animInfo)
	{
		DWORD status = animInfo->GetStatus();
		if(status&ANIMSTATUS_Finished) 
		{
			if(!disable_loop)
			{
				DWORD status = animInfo->GetStatus();
				status &= ~ANIMSTATUS_Finished;
				animInfo->SetStatus(status);
				animInfo->fCurFrame = 0.0f;
				animInfo->fInfluence = 1.0f;
			}
			else
			{
				animInfo->fInfluence = 0.0f;
			}
		}
	}
}

void CUberAnim::StopShootAnim()
{
	if(shootAnimTrackID != INVALID_TRACK_ID)
	{
		anim.Stop(shootAnimTrackID);
		shootAnimTrackID = INVALID_TRACK_ID;
	}
}

void CUberAnim::StartDeathAnim()
{
	StopReloadAnim();
	StopGrenadeAnimations();
	
	if(IsSwimming)
		anim.StartAnimation(data_->aid_.deaths[12], ANIMFLAG_PauseOnEnd | ANIMFLAG_RemoveOtherFade, 0.0f, 1.0f, 0.1f);
	else
		anim.StartAnimation(data_->aid_.deaths[11], ANIMFLAG_PauseOnEnd | ANIMFLAG_RemoveOtherFade, 0.0f, 1.0f, 0.1f);
}

void CUberAnim::StartJump()
{
	if(IsFPSMode()) // no need for jump in FPS mode
	{
		jumpStartTime = 0.0f;
		return;
	}

	if(AnimPlayerState == PLAYER_IDLE || AnimPlayerState == PLAYER_IDLEAIM)
		jumpStartTime = jumpStartTimeByState[0];
	else
		jumpStartTime = jumpStartTimeByState[1];

	int idx = data_->GetJumpAnimId(AnimPlayerState, 0);
	jumpTrackID = anim.StartAnimation(idx, ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
	jumpState   = 0;
	jumpWeInAir = false;
	jumpPlayerState = AnimPlayerState;
	anim.GetTrack(jumpTrackID)->fSpeed = jumpAnimSpeed;

	// resync animation, so jump track will be relocated to top of lower bodys anim
	SwitchToState(AnimPlayerState, AnimMoveDir);
}

void CUberAnim::UpdateJump(bool bOnGround)
{
	if(IsFPSMode()) // no need for jump in FPS mode
		return;

	// check if we're in air
	if(!jumpWeInAir && !bOnGround)
	{
		jumpWeInAir = true;
	}
	
	// update air time - used in free fall detection
	if(!bOnGround) {
		jumpAirTime += r3dGetFrameTime();
	} else {
		jumpAirTime = 0;
	}

	// started to jump
	if(jumpState == 0) 
	{
		const r3dAnimation::r3dAnimInfo* ai = anim.GetTrack(jumpTrackID);
		if(!ai || (ai->dwStatus & ANIMSTATUS_Finished)) 
		{
			// switch to AIR
			int idx = data_->GetJumpAnimId(jumpPlayerState, 1);
			anim.Stop(jumpTrackID);
			jumpTrackID = anim.StartAnimation(idx, ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
			jumpState   = 1;
			jumpWeInAir = true; // after initial jump, set us to in-air no matter what, in case if for some reason we jumped in such a way that we need left the ground, we would be stuck in first jump animation forever
	
			// resync animation, so jump track will be relocated to top of lower bodys anim
			SwitchToState(AnimPlayerState, AnimMoveDir);
		}
		return;
	}
	
	// in air
	if(jumpState == 1)
	{
		// switch to landing only when we actually started to jump
		if(jumpWeInAir && bOnGround)
		{
			int idx = data_->GetJumpAnimId(jumpPlayerState, 2);
			anim.Stop(jumpTrackID);
			jumpTrackID = anim.StartAnimation(idx, ANIMFLAG_PauseOnEnd, 1.0f, 1.0f, 0.0f);
			jumpState   = 2;
			
			anim.GetTrack(jumpTrackID)->fSpeed = jumpAnimSpeed;
	
			// resync animation, so jump track will be relocated to top of lower bodys anim
			SwitchToState(AnimPlayerState, AnimMoveDir);

			if(AnimPlayerState >= PLAYER_MOVE_CROUCH && AnimPlayerState <= PLAYER_MOVE_SPRINT)
			{
				//ANIM_HACK: animation before jump should be lower body walking anim
				// so set it to frame 1 and pause (because end of landing anim will be start of walk)
				for(size_t i=1; i<anim.AnimTracks.size(); i++) {
					if(anim.AnimTracks[i].iTrackId == jumpTrackID) {
						// lower
						anim.AnimTracks[i-1].fCurFrame = 1.0f;
						anim.AnimTracks[i-1].dwStatus |= ANIMSTATUS_Paused;
						jumpMoveTrackID = anim.AnimTracks[i-1].iTrackId;
						// upper
						anim.AnimTracks[i+1].fCurFrame = 1.0f;
						anim.AnimTracks[i+1].dwStatus |= ANIMSTATUS_Paused;
						jumpMoveTrackID2 = anim.AnimTracks[i+1].iTrackId;
					}
				}
			}

		}
		return;
	}
	
	// landing
	if(jumpState == 2)
	{
		const r3dAnimation::r3dAnimInfo* ai = anim.GetTrack(jumpTrackID);

		// if player state was changed in landing, abort it
		if(AnimPlayerState != jumpPlayerState)
		{
			ai = NULL;
		}

		if(!ai || (ai->dwStatus & ANIMSTATUS_Finished)) 
		{
			anim.FadeOut(jumpTrackID, 0.1f);
			jumpTrackID = INVALID_TRACK_ID;
			jumpState   = -1;

			// resume walking animation
			r3dAnimation::r3dAnimInfo* ai2 = anim.GetTrack(jumpMoveTrackID);
			if(ai2) {
				ai2->dwStatus &= ~ANIMSTATUS_Paused;
				jumpMoveTrackID = INVALID_TRACK_ID;
			}
			r3dAnimation::r3dAnimInfo* ai3 = anim.GetTrack(jumpMoveTrackID2);
			if(ai3) {
				ai3->dwStatus &= ~ANIMSTATUS_Paused;
				jumpMoveTrackID2 = INVALID_TRACK_ID;
			}
		}
		
		return;
	}
	
/*DISABLED for now: cause troubles with walking downside on ramps. need to have raycast down or something
	// we're falling from something without jump starting - switch to AIR
	if(jumpState == -1 && !bOnGround && jumpAirTime > 0.1f)
	{
		int idx = data_->GetJumpAnimId(AnimPlayerState, 1);
		jumpTrackID = anim.StartAnimation(idx, ANIMFLAG_Looped, 0.0f, 1.0f, 0.1f);
		jumpState   = 1;
		jumpWeInAir = true;
		jumpPlayerState = AnimPlayerState;
	
		// resync animation, so jump track will be relocated to top of lower bodys anim
		SwitchToState(AnimPlayerState, AnimMoveDir);
		return;
	}
*/	
}