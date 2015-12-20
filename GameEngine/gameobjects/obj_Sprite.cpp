//=========================================================================
//	Module: obj_Sprite.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "obj_Sprite.h"
#include "GameCommon.h"

//////////////////////////////////////////////////////////////////////////

void MatrixGetYawPitchRoll ( const D3DXMATRIX & mat, float & fYaw, float & fPitch, float & fRoll );

//////////////////////////////////////////////////////////////////////////

obj_Sprite::obj_Sprite()
: customMaterial(0)
{
	ObjTypeFlags |= OBJTYPE_Sprite;
}

//////////////////////////////////////////////////////////////////////////

obj_Sprite::~obj_Sprite()
{

}

//////////////////////////////////////////////////////////////////////////

void obj_Sprite::UpdateTransform()
{
	// calc mTransform
	D3DXMATRIX camM = r3dRenderer->ViewMatrix;

	D3DXMatrixInverse(&camM, 0, &camM);
	camM._41 = GetPosition().x;
	camM._42 = GetPosition().y;
	camM._43 = GetPosition().z;

	D3DXMATRIX mR;
	D3DXMatrixRotationYawPitchRoll(&mR, 0, -D3DX_PI / 2, 0);

	const r3dPoint3D &s = GetScale();
	D3DXMATRIX mS;
	D3DXMatrixScaling(&mS, s.x, s.y, s.z);

	mTransform  = mS * mR * camM;

	r3dVector angles;
	MatrixGetYawPitchRoll(mTransform, angles.x, angles.y, angles.z);
	angles = D3DXToDegree(angles);
	SetRotationVector(angles);

	UpdateWorldBBox();

}

//////////////////////////////////////////////////////////////////////////

BOOL obj_Sprite::Update()
{
	UpdateTransform();
	return parent::Update();
}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_Sprite::Clone()
{
	obj_Sprite * pNew = reinterpret_cast<obj_Sprite*>(srv_CreateGameObject("obj_Sprite", FileName.c_str(), GetPosition()));
	if (pNew && customMaterial)
	{
		pNew->LoadSpriteCustomMaterial(customMaterial->Name);
	}
	return pNew;
}

//////////////////////////////////////////////////////////////////////////

void obj_Sprite::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);

	pugi::xml_node customMat = node.child("custom_material");

	if (!customMat.empty())
	{
		const char* matName = customMat.attribute("name").value();
		if (matName)
		{
			LoadSpriteCustomMaterial(matName);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Sprite::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	if (customMaterial)
	{
		pugi::xml_node customMatName = node.append_child();
		customMatName.set_name("custom_material");
		customMatName.append_attribute("name") = customMaterial->Name;
	}
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD

float obj_Sprite::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry + parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		static char materialFileSelected[256] = {0};
		static float materialOffset = 0;

		y += imgui_Static(scrx, y, "Custom material");

		if (imgui_FileList(scrx, y, 360, 200, "Data//ObjectsDepot//SPRITES//Materials//*", materialFileSelected, &materialOffset, true))
		{
			size_t s = strlen(materialFileSelected);
			if (s > 3)
				materialFileSelected[s - 4] = 0;
			LoadSpriteCustomMaterial(materialFileSelected);
		}
		y += 200.0f;
	}
	return y;
}
#endif

//////////////////////////////////////////////////////////////////////////

void obj_Sprite::LoadSpriteCustomMaterial(const char *name)
{
	if (!name)
		return;

	customMaterial = r3dMaterialLibrary::RequestMaterialByName( name, "SPRITES", false, false );

	for (int i = 0; i < NUM_LODS; ++i)
	{
		r3dMesh *m = MeshLOD[i];
		if (m)
		{
			for (int j = 0; j < m->NumMatChunks; ++j)
			{
				r3dTriBatch &tb = m->MatChunks[j];
				tb.Mat = customMaterial;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_Sprite::Load(const char* name)
{
	BOOL rv = parent::Load(name);
	if (rv)
	{
		MakeMeshUnique();
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////

void obj_Sprite::MakeMeshUnique()
{
	static int counter = 0;
	static char buf[0xf] = {0};
	itoa(counter, buf, _countof(buf));
	++counter;

	for (int i = 0; i < NUM_LODS; ++i)
	{
		r3dMesh *m = MeshLOD[i];
		if (m)
		{
			m->FileName += buf;
		}
	}
}