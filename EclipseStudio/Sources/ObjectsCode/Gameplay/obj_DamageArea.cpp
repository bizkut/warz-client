#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_DamageArea.h"
#include "../../multiplayer/ClientGameLogic.h"
#include "../ai/AI_Player.H"

#include "Editors/ObjectManipulator3d.h"

IMPLEMENT_CLASS(obj_DamageArea, "obj_DamageArea", "Object");
AUTOREGISTER_CLASS(obj_DamageArea);

extern bool g_bEditMode;

static r3dTexture *DamageAreaIcon = NULL;

/*static*/ obj_DamageArea::Arr obj_DamageArea::ms_DamageAreaArr ;

obj_DamageArea::obj_DamageArea()
{
	m_Radius = 5.0f;
	m_Damage = 1.0f;

	ms_DamageAreaArr.PushBack( this ) ;
}

obj_DamageArea::~obj_DamageArea()
{
	int erased = 0 ;

	for( int i = 0, e = ms_DamageAreaArr.Count() ; i < e ; i ++ )
	{
		if( ms_DamageAreaArr[ i ] == this )
		{
			ms_DamageAreaArr.Erase( i ) ;
			erased = 1 ;
			break ;
		}
	}

	r3d_assert( erased ) ;
}

BOOL obj_DamageArea::Load(const char *fname)
{
	if(!parent::Load(fname)) return FALSE;

	if(g_bEditMode)
		if (!DamageAreaIcon) DamageAreaIcon = r3dRenderer->LoadTexture("Data\\Images\\DamageArea.dds");

	return TRUE;
}

BOOL obj_DamageArea::OnCreate()
{
	parent::OnCreate();

	DrawOrder	= OBJ_DRAWORDER_LAST;
	setSkipOcclusionCheck(true);
	ObjFlags	|=	OBJFLAG_DisableShadows | OBJFLAG_ForceSleep;

	r3dBoundBox bboxLocal ;

	bboxLocal.Size = r3dPoint3D(2,2,2);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;

	SetBBoxLocal( bboxLocal ) ;
	UpdateTransform();

	return 1;
}

extern bool g_bExplicitlyShowBattleZoneWarning;
BOOL obj_DamageArea::Update()
{
	return parent::Update();
}

void obj_DamageArea::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node damageAreaNode = node.child("damageArea");
	m_Radius = damageAreaNode.attribute("radius").as_float();
	m_Damage = damageAreaNode.attribute("damage").as_float();
}

void obj_DamageArea::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node damageAreaNode = node.append_child();
	damageAreaNode.set_name("damageArea");
	damageAreaNode.append_attribute("radius") = m_Radius;
	damageAreaNode.append_attribute("damage") = m_Damage;
}

struct DamageAreaRenderable : Renderable
{
	void Init() { DrawFunc = Draw; }
	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		DamageAreaRenderable *This = static_cast<DamageAreaRenderable*>( RThis );

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);

		r3dColor clr = r3dColor::red; clr.A = 128;
		r3dRenderer->SetRenderingMode( R3D_BLEND_PUSH| R3D_BLEND_ALPHA | R3D_BLEND_ZC );
		r3dDrawSphereSolid ( This->Parent->GetPosition(), This->Parent->GetRadius(), gCam, clr );
		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

		r3dRenderer->Flush();
	}

	obj_DamageArea* Parent;
};

void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
struct DamageAreaRenderableHelper : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		DamageAreaRenderableHelper *This = static_cast<DamageAreaRenderableHelper*>( RThis );

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );

		if((This->Parent->GetPosition() - Cam).Length() < 100)
			r3dDrawIcon3D(This->Parent->GetPosition(), DamageAreaIcon, r3dColor(255,255,255), 24);
	}

	obj_DamageArea* Parent;	
};

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_DamageArea::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
#ifdef FINAL_BUILD
	return;
#else
	// don't draw debug info if we're not in editor mode
	if ( !g_bEditMode )
		return;

	if( r_hide_icons->GetInt() )
		return ;

	float idd = r_icons_draw_distance->GetFloat();
	idd *= idd;

	if( ( Cam - GetPosition() ).LengthSq() > idd )
		return;

	if(g_Manipulator3d.PickedObject() == this)
	{
		DamageAreaRenderable rend;
		rend.Init();
		rend.SortValue = RENDERABLE_OBJ_USER_SORT_VALUE;
		rend.Parent = this;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}

	// helper
	extern int CurHUDID;
	if(CurHUDID == 0)
	{
		DamageAreaRenderableHelper rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}
#endif
}

#ifndef FINAL_BUILD
float obj_DamageArea::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor( scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		starty += imgui_Static ( scrx, starty, "Damage Area Properties" );

		static float radiusVal = 0 ;
		
		radiusVal = m_Radius ;
		starty += imgui_Value_Slider(scrx, starty, "Radius", &radiusVal,	0.0f,300,	"%.2f",1);
		PropagateChange( radiusVal, &obj_DamageArea::m_Radius, this, selected ) ;

		static float damageVal = 0 ;
		damageVal = m_Damage ;
		starty += imgui_Value_Slider(scrx, starty, "Damage per second", &damageVal,	0.0f,200.0f,	"%.2f",1);
		PropagateChange( damageVal, &obj_DamageArea::m_Damage, this, selected ) ;
	}

	return starty-scry;
}
#endif
