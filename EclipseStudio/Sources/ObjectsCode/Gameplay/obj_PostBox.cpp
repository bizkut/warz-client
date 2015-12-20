#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_PostBox.h"
#include "XMLHelpers.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_PostBox, "obj_PostBox", "Object");
AUTOREGISTER_CLASS(obj_PostBox);

r3dgameVector(obj_PostBox*) obj_PostBox::LoadedPostboxes;

namespace
{
	struct obj_PostBoxCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			obj_PostBoxCompositeRenderable *This = static_cast<obj_PostBoxCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::grey);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->useRadius, Cam, 0.1f, r3dColor(0, 0, 255));

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_PostBox *Parent;	
	};
}

obj_PostBox::obj_PostBox()
{
	useRadius = 10.0f;
}

obj_PostBox::~obj_PostBox()
{
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_PostBox::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
	parent::AppendRenderables(render_arrays, Cam);
#ifdef FINAL_BUILD
	return;
#else
	if(g_bEditMode)
	{
		obj_PostBoxCompositeRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
#endif
}

void obj_PostBox::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node objNode = node.child("post_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

void obj_PostBox::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);

	pugi::xml_node objNode = node.append_child();
	objNode.set_name("post_box");
	SetXMLVal("useRadius", objNode, &useRadius);
}

BOOL obj_PostBox::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\WZ_Settlement\\ItemDrop_PostalBox.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_PostBox::OnCreate()
{
	parent::OnCreate();

	ObjFlags |= OBJFLAG_DisableShadows;

	LoadedPostboxes.push_back(this);
	return 1;
}


BOOL obj_PostBox::OnDestroy()
{
	LoadedPostboxes.erase(std::find(LoadedPostboxes.begin(), LoadedPostboxes.end(), this));
	return parent::OnDestroy();
}

BOOL obj_PostBox::Update()
{
	return parent::Update();
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
float obj_PostBox::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Post Box Parameters" );
		starty += imgui_Value_Slider(scrx, starty, "Safe Zone Radius", &useRadius, 0, 500.0f, "%.0f");
	}

	return starty-scry;
}
#endif
