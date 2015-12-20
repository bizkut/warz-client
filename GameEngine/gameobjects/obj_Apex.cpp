//=========================================================================
//	Module: obj_Apex.cpp
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if APEX_ENABLED

#include "GameObj.h"
#include "GameCommon.h"

#include "obj_Apex.hpp"

//////////////////////////////////////////////////////////////////////////

extern bool g_bEditMode;

//////////////////////////////////////////////////////////////////////////

struct ApexDestructibleRenderable: public Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw (Renderable* RThis, const r3dCamera& Cam)
	{
		ApexDestructibleRenderable* This = static_cast<ApexDestructibleRenderable*>(RThis);
		ApexActorBase *aa = This->Parent->destructibleActor;
		if (aa)
			aa->Draw();
	}

	obj_ApexDestructible* Parent;
};

//////////////////////////////////////////////////////////////////////////

struct ApexDestructibleShadowRenderable: public Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw (Renderable* RThis, const r3dCamera& Cam)
	{
		ApexDestructibleShadowRenderable* This = static_cast<ApexDestructibleShadowRenderable*>(RThis);
		ApexActorBase *aa = This->Parent->destructibleActor;
		if (aa)
			aa->DrawShadow();
	}

	obj_ApexDestructible* Parent;
};

//////////////////////////////////////////////////////////////////////////

obj_ApexDestructible::obj_ApexDestructible()
: destructibleActor(0)
, makePreviewActor(true)
{
	ObjTypeFlags |= OBJTYPE_ApexDestructible;
}

//////////////////////////////////////////////////////////////////////////

obj_ApexDestructible::~obj_ApexDestructible()
{
	delete destructibleActor;
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_ApexDestructible::Update()
{
	if (!parent::Update()) return FALSE;
	if (!destructibleActor) return FALSE;
	destructibleActor->Update();

	r3dBoundBox bbox_local = destructibleActor->GetBBox();
	SetBBoxLocal(bbox_local);

	UpdateTransform();

	r3dBoundBox bbox_world = bbox_local;
	bbox_local.Org -= GetPosition();

	SetBBoxLocalAndWorld(bbox_local, bbox_world);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

#define	RENDERABLE_APEX_SORT_VALUE (30*RENDERABLE_USER_SORT_VALUE)

void obj_ApexDestructible::AppendRenderables(RenderArray(&render_arrays)[rsCount], const r3dCamera& Cam)
{
	if (destructibleActor)
	{
		ApexDestructibleRenderable rend;
		rend.Init();
		rend.Parent = this;
		rend.SortValue = RENDERABLE_APEX_SORT_VALUE;
		render_arrays[ rsFillGBuffer ].PushBack( rend );
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_ApexDestructible::AppendShadowRenderables(RenderArray & rarr, int sliceIndex, const r3dCamera& Cam)
{
	float dist = (gCam - GetPosition()).Length();
	UINT32 idist = (UINT32)R3D_MIN(dist * 64.f, (float)0x3fffffff);

	ApexDestructibleShadowRenderable rend;
	rend.Init();
	rend.Parent = this;
	rend.SortValue |= idist;
	rarr.PushBack(rend);
}


//////////////////////////////////////////////////////////////////////////

BOOL obj_ApexDestructible::OnCreate()
{
	if (!parent::OnCreate())
		return FALSE;

	ApexActorParameters ap;
	ap.assetPath = FileName;
	ap.SetPose(GetPosition(), GetRotationVector());
	ap.userData = this;

	destructibleActor = CreateApexActor(ap, makePreviewActor);
	if (!destructibleActor)
		return FALSE;
		
	r3dBoundBox bbox_local = destructibleActor->GetBBox();
	bbox_local.Org -= GetPosition();

	SetBBoxLocal(bbox_local);

	UpdateTransform();

	setSkipOcclusionCheck(true);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void obj_ApexDestructible::SetPosition(const r3dPoint3D& pos)
{
	parent::SetPosition(pos);
	if (!destructibleActor) return;

	destructibleActor->SetPosition(GetPosition());
}

//////////////////////////////////////////////////////////////////////////

void obj_ApexDestructible::SetRotationVector(const r3dVector& Angles)
{
	parent::SetRotationVector(Angles);
	if (destructibleActor)
	{
		destructibleActor->SetRotation(Angles);
	}
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
float obj_ApexDestructible::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );
	
	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		if (!destructibleActor) return starty;

		char buf[256] = {0};
		switch (destructibleActor->Type())
		{
		case TypeApexDestructible:
			sprintf(buf, "Convert to %s", "preview object");
			break;
		case TypeApexPreview:
			sprintf(buf, "Convert to %s", "destructible object");
			break;
		}

		if (imgui_Button(scrx, starty, 360, 20, buf))
		{
			for( int i = 0, e = selected.Count() ; i < e; i ++ )
			{
				obj_ApexDestructible* ad = (obj_ApexDestructible*)selected[ i ] ;

				if (ad->destructibleActor)
				{
					ApexActorBase *newDA = ad->destructibleActor->ConvertToOpposite();
					delete ad->destructibleActor;
					ad->destructibleActor = newDA;
				}
			}
		}

		starty += 22.0f;
	}
	return starty;
}
#endif

//////////////////////////////////////////////////////////////////////////

BOOL obj_ApexDestructible::Load(const char* name)
{
	FileName = name;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_ApexDestructible::Clone()
{
	if (!destructibleActor)
		return 0;

	obj_ApexDestructible *clone = static_cast<obj_ApexDestructible*>(srv_CreateGameObject("obj_ApexDestructible", FileName.c_str(), GetPosition()));
	if (!clone)
		return 0;

	clone->SetRotationVector(GetRotationVector());
	clone->setSkipOcclusionCheck(true);

	return clone;
}

//////////////////////////////////////////////////////////////////////////

void obj_ApexDestructible::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	makePreviewActor = g_bEditMode;
	Load(FileName.c_str());
	OnCreate();
}

//////////////////////////////////////////////////////////////////////////

void obj_ApexDestructible::WriteSerializedData(pugi::xml_node& node)
{
	if (!destructibleActor)
		return;

	parent::WriteSerializedData(node);
}

//////////////////////////////////////////////////////////////////////////

r3dMaterial * obj_ApexDestructible::GetMaterial(uint32_t faceID)
{
	if (!destructibleActor)
		return 0;

	return destructibleActor->GetMaterial(faceID);
}

//////////////////////////////////////////////////////////////////////////

void obj_ApexDestructible::ConvertToDestructible()
{
	if (!destructibleActor) return;

	if (destructibleActor->Type() != TypeApexDestructible)
	{
		ApexActorBase *newDA = destructibleActor->ConvertToOpposite();
		delete destructibleActor;
		destructibleActor = newDA;
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_ApexDestructible::ConvertToPreview()
{
	if (!destructibleActor) return;

	if (destructibleActor->Type() != TypeApexPreview)
	{
		ApexActorBase *newDA = destructibleActor->ConvertToOpposite();
		delete destructibleActor;
		destructibleActor = newDA;
	}
}

#endif