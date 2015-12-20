#pragma once

#ifndef FINAL_BUILD

const int MAX_NUM_MECH_WEAPONS = 6;

//////////////////////////////////////////////////////////////////////////

enum Playerstate_e
{
	PLAYER_INVALID = -1,

	PLAYER_IDLE = 0,
	PLAYER_MOVE_WALK,
	PLAYER_DIE,

	PLAYER_NUM_STATES,
};

enum EMechSlot
{
	MSLOT_Mech1 = 0,
	MSLOT_Mech2,
	MSLOT_Mech3,
	MSLOT_Weapon1,
	MSLOT_Weapon2,
	MSLOT_Weapon3,
	MSLOT_Weapon4,
	MSLOT_Weapon5,
	MSLOT_Weapon6,
	MSLOT_Max,
};

class CMechUberEquip
{
public:
	// equipped things
	class slot_s
	{
	public:
		float fOffset;
		int iIdx;
		r3dSTLString name;
		
		r3dMesh*	mesh;
		r3dSkeleton *skel;

		slot_s()
		{
			skel	= 0;
			fOffset = 0;
			iIdx    = -1;
			mesh    = NULL;
		}
	};
	slot_s		slots_[MSLOT_Max];

	enum DrawType
	{
		DT_DEFERRED,
		DT_SHADOWS,
		DT_AURA,
		DT_DEPTH,
	};

	const char * weaponBoneNames[MAX_NUM_MECH_WEAPONS];
	
private:
	void		DrawSlot(int slotId, const D3DXMATRIX& world, DrawType dt, bool skin);
	void		DrawWeapons(const r3dSkeleton *skel, const D3DXMATRIX &CharMat, DrawType dt);

public:
	CMechUberEquip();
	~CMechUberEquip();

	void		LoadSlot(int slotId, const char* fname);
	void		UnloadSlot(int slotId);
	void		SetWeaponNames(const char * const wpNames[MAX_NUM_MECH_WEAPONS]);

	D3DXMATRIX	getWeaponBone(int idx, const r3dSkeleton* skel, const D3DXMATRIX& offset);

	void		Draw(const r3dSkeleton* skel, const D3DXMATRIX& CharMat, bool draw_weapon, DrawType dt);
	void		DrawSM(const r3dSkeleton* skel, const D3DXMATRIX& CharMat, bool draw_weapon);
	
	void		Reset();

	r3dBoundBox GetBoundingBox() const;
};

#endif