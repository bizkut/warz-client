//=========================================================================
//	Module: obj_MissionArea.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================
#include "r3dPCH.h"
#include "r3d.h"

#include "obj_MissionArea.h"
#include "../../Editors/LevelEditor.h"

#ifndef FINAL_BUILD

void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_MissionArea, "obj_MissionArea", "Object");
AUTOREGISTER_CLASS(obj_MissionArea);

extern bool g_bEditMode;
static r3dTexture *MissionIcon = NULL;

//////////////////////////////////////////////////////////////////////////

namespace
{
//////////////////////////////////////////////////////////////////////////

	struct MissionAreaCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			MissionAreaCompositeRenderable *This = static_cast<MissionAreaCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_ZC | R3D_BLEND_PUSH);

			if((This->Parent->GetPosition() - Cam).Length() < 100)
				r3dDrawIcon3D(This->Parent->GetPosition(), MissionIcon, r3dColor(255,255,255), 32);

			const r3dColor24	color	= r3dColor24( 226.0f, 120.0f, 24.0f, 128.0f );
			const r3dColor24	color2	= r3dColor24( 255.0f, 120.0f, 24.0f, 255.0f );
			const r3dPoint3D&	pos		= This->Parent->GetPosition();
			if( MissionAreaType::AABB == This->Parent->m_areaType )
			{
				const r3dVector		extents	= 2 * This->Parent->m_extents;
				r3dDrawBox3D( pos.x, pos.y, pos.z, extents.x, extents.y, extents.z, color );

				// NOTE: The Box's Org is a lower corner, not the center, so we compensate
				//		 here to align the lines with the 3D Box.
				r3dBoundBox box;
				box.Org		= pos - This->Parent->m_extents;
				box.Size	= extents;
				r3dDrawBoundBox( box, Cam, color2, 0.01f );
			}
			else // ( MissionAreaType::Sphere == This->Parent->m_areaType )
			{
				// r3dDrawUniformSphere doesn't change Cam, but also doesn't specify const.
				r3dDrawSphereSolid( pos, This->Parent->m_extents.x, Cam, color, 8, 16, true, color2 );
				//r3dDrawUniformSphere( pos, This->Parent->m_extents.x, const_cast<r3dCamera&>(Cam), color2 );
			}

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_MissionArea *Parent;	
	};
}

//////////////////////////////////////////////////////////////////////////

obj_MissionArea::obj_MissionArea()
	: m_areaType( MissionAreaType::AABB )
	, m_extents( 1.0f, 1.0f, 1.0f )
{
	m_bEnablePhysics = false;

	serializeFile = SF_ServerData;

	// TODO: Change this out for a Mission specific icon, when/if we can get one.
	if (!MissionIcon) MissionIcon = r3dRenderer->LoadTexture("Data\\Images\\SiegeObjective.dds");
}

obj_MissionArea::~obj_MissionArea()
{
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_MissionArea::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
	if ( !g_bEditMode )
		return;

	if(r_hide_icons->GetInt())
		return;

	float idd = r_icons_draw_distance->GetFloat();
	idd *= idd;

	if( ( Cam - GetPosition() ).LengthSq() > idd )
		return;

	MissionAreaCompositeRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;

	render_arrays[ rsDrawDebugData ].PushBack( rend );
}

float obj_MissionArea::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry;

	y += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);
	y += 5.0f;

	y += imgui_Static(scrx, y, "Mission Area parameters:");

	int areaTypeAABB	= (int)((MissionAreaType::AABB == m_areaType) ? m_areaType : 0);
	int areaTypeSphere	= (int)((MissionAreaType::Sphere == m_areaType) ? m_areaType : 0);
	y += imgui_Static(scrx, y, "Area Type:");
	y += imgui_Checkbox(scrx, y, "AABB", &areaTypeAABB, MissionAreaType::AABB );
	y += imgui_Checkbox(scrx, y, "Sphere", &areaTypeSphere, MissionAreaType::Sphere );
	y += 1.0f;
	if( areaTypeAABB > 0 && !( areaTypeAABB == m_areaType ) )
	{
		PropagateChange( areaTypeAABB, &obj_MissionArea::UpdateAreaTypeChanged, selected );
	}
	else if( areaTypeSphere > 0 && !( areaTypeSphere == m_areaType ) )
	{
		PropagateChange( areaTypeSphere, &obj_MissionArea::UpdateAreaTypeChanged, selected );
	}

	if( MissionAreaType::AABB == m_areaType )
	{
		y += imgui_Value_Slider(scrx, y, "Width", &m_extents.x, 0.5f, 400.0f, "%.2f");
		y += imgui_Value_Slider(scrx, y, "Height", &m_extents.y, 0.5f, 100.0f, "%.2f");
		y += imgui_Value_Slider(scrx, y, "Depth", &m_extents.z, 0.5f, 400.0f, "%.2f");
	}
	else // ( MissionAreaType::Sphere == m_areaType )
	{
		y += imgui_Value_Slider(scrx, y, "Radius", &m_extents.x, 0.5f, 400.0f, "%.2f");
	}

	return y - scry;
}

BOOL obj_MissionArea::OnCreate()
{
	parent::OnCreate();

	//DrawOrder = OBJ_DRAWORDER_LAST;

	ObjFlags |= OBJFLAG_DisableShadows;

	r3dBoundBox bboxLocal ;
	bboxLocal.Size = r3dPoint3D(2, 2, 2);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;
	SetBBoxLocal(bboxLocal) ;
	UpdateTransform();

	return 1;
}

BOOL obj_MissionArea::Update()
{
	return parent::Update();

	// Ensure rotation is 0, Scale is 1.
	SetRotationVector( r3dVector( 0.0f, 0.0f, 0.0f ) );
	SetScale( r3dPoint3D( 1.0f, 1.0f, 1.0f ) );
}

BOOL obj_MissionArea::OnDestroy()
{
	return parent::OnDestroy();
}

void obj_MissionArea::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData( node );
	pugi::xml_node missionAreaNode = node.child("mission_area_parameters");
	int areaType = 0;
	while( areaType < MissionAreaType::MAX_AREA_TYPE &&
		   _tcsncmp( missionAreaNode.attribute("type").value(),
					 MissionAreaType::MissionAreaTypeNames[ areaType ],
					 _tcslen( MissionAreaType::MissionAreaTypeNames[ areaType ] ) ) )
	{
					 ++areaType;
	}
	r3d_assert( areaType != (int)MissionAreaType::MAX_AREA_TYPE );
	m_areaType = (MissionAreaType::EMissionAreaType)(areaType + 1); // Enum starts at 1, not 0, so that the checkboxes will work properly in the editor.
	if( MissionAreaType::AABB == m_areaType )
	{
		m_extents.x = fabs(missionAreaNode.attribute("x").as_float());
		m_extents.y = fabs(missionAreaNode.attribute("y").as_float());
		m_extents.z = fabs(missionAreaNode.attribute("z").as_float());
	}
	else // (MissionAreaType::Sphere == m_areaType )
	{
		m_extents.x = fabs(missionAreaNode.attribute("radius").as_float());
		m_extents.y = 0.0f;
		m_extents.z = 0.0f;
	}
}

void obj_MissionArea::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	pugi::xml_node missionAreaNode = node.append_child();
	missionAreaNode.set_name("mission_area_parameters");

	missionAreaNode.append_attribute("type") = MissionAreaType::MissionAreaTypeNames[ m_areaType - 1 ];
	if( MissionAreaType::AABB == m_areaType )
	{
		missionAreaNode.append_attribute("x") = m_extents.x;
		missionAreaNode.append_attribute("y") = m_extents.y;
		missionAreaNode.append_attribute("z") = m_extents.z;
	}
	else // ( MissionAreaType::Sphere == m_areaType )
	{
		missionAreaNode.append_attribute("radius") = m_extents.x;
	}
}

void obj_MissionArea::UpdateAreaTypeChanged( const int& areaType )
{
	m_areaType = (MissionAreaType::EMissionAreaType)areaType;
}

const char* obj_MissionArea::GetMissionAreaTypeName()
{
	return MissionAreaType::MissionAreaTypeNames[ m_areaType - 1 ];
}

#endif // FINAL_BUILD