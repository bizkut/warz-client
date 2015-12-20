
#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "GameObjects\GameObj.h"
#include "GameObjects\ObjManag.h"

#include "rendering/Deffered/DeferredHelpers.h"

#include "MapBorder.h"

IMPLEMENT_CLASS(obj_MapBorder, "obj_MapBorder", "Object");
AUTOREGISTER_CLASS(obj_MapBorder);

static r3dTexture *MapBorderIcon = NULL;

obj_MapBorder::obj_MapBorder()
{
	m_Width = 50.0f;
	m_Length = 50.0f;
	m_Timeout = 1.0f;
	m_RenderVisualization = false;
}

BOOL obj_MapBorder::Load(const char *fname)
//-----------------------------------------------------------------------
{
	if(!parent::Load(fname)) return FALSE;

	return TRUE;
}

extern GameObject *gPlayer;

BOOL obj_MapBorder::OnCreate()
{
	parent::OnCreate();

	ObjFlags	|= OBJFLAG_DisableShadows ;

	DrawOrder	= OBJ_DRAWORDER_LAST;

	if (!MapBorderIcon) 
		MapBorderIcon = r3dRenderer->LoadTexture("Data\\Images\\MapBorder.dds");

	r3dBoundBox bboxLocal ;

	bboxLocal.Size = r3dPoint3D(10,10,10);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;

	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();

	return 1;
}

#ifndef FINAL_BUILD

void obj_MapBorder::CenterOnMap()
{
	const r3dTerrainDesc& desc = Terrain->GetDesc() ;

	SetPosition(r3dPoint3D(desc.WorldSize/2, Terrain->GetHeight(r3dPoint3D(desc.WorldSize/2, 0, desc.WorldSize/2)), desc.WorldSize/2));
}

float obj_MapBorder::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected ) ;

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		static float widthVal ;
		widthVal = m_Width ;
		starty += imgui_Value_Slider(scrx, starty, "Width(X)", &widthVal,	0,100000,	"%.2f",1);
		PropagateChange( widthVal, &obj_MapBorder::m_Width, this, selected ) ;

		static float lengthVal ;
		lengthVal = m_Length ;
		starty += imgui_Value_Slider(scrx, starty, "Length(Z)", &lengthVal,	0,100000,	"%.2f",1);
		PropagateChange( lengthVal, &obj_MapBorder::m_Length, this, selected ) ;

		static float timeoutVal ;
		timeoutVal = m_Timeout ;
		starty += imgui_Value_Slider(scrx, starty, "Timeout(sec)", &timeoutVal,	0,200,	"%.2f",1);
		PropagateChange( timeoutVal, &obj_MapBorder::m_Timeout, this, selected ) ;

		int check = m_RenderVisualization ;
		starty += imgui_Checkbox(scrx, starty, "Render Visualization", &check, 1);
		PropagateChange( check, &obj_MapBorder::m_RenderVisualization, this, selected ) ;

		if(imgui_Button(scrx, starty, 300, 25, "Center on map"))
		{
			PropagateChange( &obj_MapBorder::CenterOnMap, selected ) ;
		}
	}

	return starty-scry;
}
#endif


BOOL obj_MapBorder::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_MapBorder::Update()
{
	return 1;
}


void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);

struct MapBorderCompositeRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		MapBorderCompositeRenderable *This = static_cast<MapBorderCompositeRenderable*>( RThis );

		This->Parent->DoDrawComposite();
	}

	obj_MapBorder* Parent;
};

struct MapBorderGBufferRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		MapBorderGBufferRenderable *This = static_cast<MapBorderGBufferRenderable*>( RThis );

		This->Parent->DoDrawGBuffer();
	}

	obj_MapBorder* Parent;
};

void obj_MapBorder::DoDrawComposite()
{
	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);

	r3dDrawIcon3D(GetPosition(), MapBorderIcon, r3dColor(255,255,255), 24);
}

void obj_MapBorder::DoDrawGBuffer()
{
	DrawDeferredBox3D(GetPosition().x, GetPosition().y-1000, GetPosition().z, m_Width, 5000, m_Length, r3dColor(32, 128, 255, 32), false );
}

#define RENDERABLE_MAPBORDER_SORT_VALUE (4*RENDERABLE_USER_SORT_VALUE)

/*virtual*/
void
obj_MapBorder::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
	// composite
	{
		MapBorderCompositeRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_MAPBORDER_SORT_VALUE;

		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}

	// gbuffer
	if( m_RenderVisualization )
	{
		MapBorderGBufferRenderable rend;

		rend.Init();
		rend.Parent = this;
		rend.SortValue = RENDERABLE_MAPBORDER_SORT_VALUE;

		render_arrays[ rsFillGBuffer ].PushBack( rend );
	}
}

void obj_MapBorder::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node mapBorderNode = node.child("mapBorder");
	m_Width = mapBorderNode.attribute("width").as_float();
	m_Length = mapBorderNode.attribute("length").as_float();
	m_Timeout = mapBorderNode.attribute("timeout").as_float();
}

void obj_MapBorder::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node mapBorderNode = node.append_child();
	mapBorderNode.set_name("mapBorder");
	mapBorderNode.append_attribute("width") = m_Width;
	mapBorderNode.append_attribute("length") = m_Length;
	mapBorderNode.append_attribute("timeout") = m_Timeout;
}

