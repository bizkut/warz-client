#include "r3dPCH.h"
#include "r3d.h"

#ifndef FINAL_BUILD

#include "GameCommon.h"

#include "obj_MechAnim.h"

CMechUberEquip::CMechUberEquip()
{
	for (int i = 0; i < MAX_NUM_MECH_WEAPONS; ++i)
	{
		weaponBoneNames[i] = 0;
	}
}

CMechUberEquip::~CMechUberEquip()
{
	Reset();
}

void CMechUberEquip::LoadSlot(int slotId, const char* fname)
{
	slot_s& sl = slots_[slotId];
	sl.mesh = NULL;
	sl.name = "";
	
	char buf[256];
	sprintf(buf, fname);
	strupr(buf);
	if(strstr(buf, "@EMPTY") != 0) {
		return;
	}

	sl.mesh = r3dGOBAddMesh(fname);
	const char *name = strrchr(fname, '\\');
	if (!name)
		name = strrchr(fname, '/');

	r3d_assert(name);
	sl.name = name + 1;
}

D3DXMATRIX CMechUberEquip::getWeaponBone(int idx, const r3dSkeleton* skel, const D3DXMATRIX& offset)
{
	D3DXMATRIX world;
	r3d_assert(idx < MAX_NUM_MECH_WEAPONS);
	r3d_assert(weaponBoneNames[idx]);

	skel->GetBoneWorldTM(weaponBoneNames[idx], &world, offset);

	return world;
}

void CMechUberEquip::Reset()
{
	for (int i = 0; i < MSLOT_Max; ++i)
	{
		UnloadSlot(i);
	}
}

void CMechUberEquip::UnloadSlot(int slotId)
{
	// do not delete model, cache will do it
	slots_[slotId].mesh = NULL;
	slots_[slotId].name = "";
	slots_[slotId].skel = 0;
}

void CMechUberEquip::DrawSlot(int slotId, const D3DXMATRIX& world, DrawType dt, bool skin)
{
	slot_s &sl = slots_[slotId];

	r3dMesh* mesh = sl.mesh;
	if(mesh == NULL)
		return;

	if(skin)
	{
		r3dMeshSetShaderConsts(world, NULL);
	}
	else
	{
		mesh->SetShaderConsts(world);

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
	}
}

//////////////////////////////////////////////////////////////////////////

void CMechUberEquip::DrawWeapons(const r3dSkeleton *skel, const D3DXMATRIX &CharMat, DrawType dt)
{
	r3dSkeleton *curSkel = 0;

	for (int i = 0; i < MAX_NUM_MECH_WEAPONS; ++i)
	{
		D3DXMATRIX world = getWeaponBone(i, skel, CharMat);

		int wpnIdx = MSLOT_Weapon1 + i;
		slot_s &sl = slots_[wpnIdx];

		r3dSkeleton *slotSkel = sl.skel;
		if (slotSkel != curSkel)
		{
			if (slotSkel)
				slotSkel->SetShaderConstants();
			else
				skel->SetShaderConstants();

			curSkel = slotSkel;
		}

		DrawSlot(wpnIdx, world, dt, slotSkel != 0);
	}
}

//////////////////////////////////////////////////////////////////////////

void CMechUberEquip::Draw(const r3dSkeleton* skel, const D3DXMATRIX& CharMat, bool draw_weapon, DrawType dt)
{
	//todo: call extern void r3dMeshSetWorldMatrix(const D3DXMATRIX& world)
	// instead of mesh->SetWorldMatrix

	skel->SetShaderConstants();

	for(int i=0; i<=MSLOT_Mech3; i++)
	{
		DrawSlot(i, CharMat, dt, true);
	}

	if(dt != DT_AURA)
	{
		DrawWeapons(skel, CharMat, dt);
	}
}

void CMechUberEquip::DrawSM(const r3dSkeleton* skel, const D3DXMATRIX& CharMat, bool draw_weapon)
{
	skel->SetShaderConstants();

	for(int i=0; i<=MSLOT_Mech3; i++) 
	{
		DrawSlot(i, CharMat, CMechUberEquip::DT_SHADOWS, true);
	}

	DrawWeapons(skel, CharMat, CMechUberEquip::DT_SHADOWS);
}

//////////////////////////////////////////////////////////////////////////

void CMechUberEquip::SetWeaponNames(const char * const wpNames[MAX_NUM_MECH_WEAPONS])
{
	for (int i = 0; i < MAX_NUM_MECH_WEAPONS; ++i)
	{
		weaponBoneNames[i] = wpNames[i];
	}
}

//////////////////////////////////////////////////////////////////////////

r3dBoundBox CMechUberEquip::GetBoundingBox() const
{
	r3dBoundBox rv;
	rv.InitForExpansion();

	for (int i = 0; i < _countof(slots_); ++i)
	{
		const slot_s &s = slots_[i];
		if (s.mesh)
		{
			rv.ExpandTo(s.mesh->localBBox);
		}
	}
	//	Zero out position offset
	rv.Org.Assign(-rv.Size.x / 2, 0, -rv.Size.z / 2);
	return rv;
}

#endif