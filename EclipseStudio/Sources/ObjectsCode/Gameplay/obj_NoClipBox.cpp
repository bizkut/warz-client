//=========================================================================
//	Module: obj_NoClipbox.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "obj_NoClipBox.h"
#include "GameCommon.h"

#ifndef FINAL_BUILD
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_NoClipBox, "obj_NoClipBox", "Object");
AUTOREGISTER_CLASS(obj_NoClipBox);

// TODO: Change this to only be on the Server when we get the FairFight API.
#if !defined(WO_SERVER) && !defined(FINAL_BUILD)
r3dgameVector(NoClipData) obj_NoClipBox::s_vNoClipData;
#endif // !defined(WO_SERVER) && !defined(FINAL_BUILD)

#ifndef WO_SERVER

extern bool g_bEditMode;

//////////////////////////////////////////////////////////////////////////

static r3dTexture *NoClipBoxIcon = NULL;

void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);

//////////////////////////////////////////////////////////////////////////

namespace
{
	struct obj_NoClipBoxCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			obj_NoClipBoxCompositeRenderable *This = static_cast<obj_NoClipBoxCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(((r_show_no_clip_x_ray->GetBool()) ? R3D_BLEND_NZ : R3D_BLEND_ZC) | R3D_BLEND_PUSH);

			const r3dColor24	color	= r3dColor24( 226.0f, 24.0f, 24.0f, 128.0f );
			const r3dColor24	color2	= r3dColor24( 255.0f, 24.0f, 24.0f, 255.0f );

			if((This->Parent->GetPosition() - Cam).Length() < 100)
				r3dDrawIcon3D(This->Parent->GetPosition(), NoClipBoxIcon, r3dColor(255,255,255), 32);

			r3dBoundBox bbox_noclip;
			bbox_noclip.Size	= This->Parent->GetScale();
			bbox_noclip.Org		= This->Parent->GetPosition();

			// According to the ReverbZoneBox::DoDraw, "there's a bug with rotation so I've to swap the axis." 
			r3dVector rotation = This->Parent->GetRotationVector();
			float temp;
			temp = rotation.y;
			rotation.y = rotation.x;
			rotation.x = temp;

			r3dDrawOrientedBoundBoxSolid( bbox_noclip, rotation, Cam, color, 0.01f, true, color2, Font_Editor );

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_NoClipBox *Parent;	
	};
}

//////////////////////////////////////////////////////////////////////////
#endif // WO_SERVER

obj_NoClipBox::obj_NoClipBox()
{
	serializeFile = SF_ServerData;

#if !defined(WO_SERVER) && !defined(FINAL_BUILD)
	ObjTypeFlags |= OBJTYPE_NoClipBox;
#endif // !defined(WO_SERVER) && !defined(FINAL_BUILD)

#ifndef WO_SERVER
	// TODO: Change this out for a NoClipBox specific icon, when/if we can get one.
	if (!NoClipBoxIcon) NoClipBoxIcon = r3dRenderer->LoadTexture("Data\\Images\\SiegeObjective.dds");
#endif // WO_SERVER
}

obj_NoClipBox::~obj_NoClipBox()
{
}

BOOL obj_NoClipBox::Update()
{
#ifndef WO_SERVER
	parent::Update();

	return TRUE;
#else
	// Will immediately destroy itself.
	return FALSE;
#endif // WO_SERVER
}

BOOL obj_NoClipBox::OnCreate()
{
	parent::OnCreate();

	// Set the Pivot/Origin in the center of the local space.
	r3dBoundBox bboxLocal ;
	bboxLocal.Size = r3dPoint3D(1,1,1);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;
	SetBBoxLocal( bboxLocal ) ;
	UpdateTransform();

	UpdatePhysicsEnabled( false );

	//r3dPoint3D pos( GetPosition() );
	//r3dPoint3D scl( GetScale() );
	//r3dPoint3D rot( GetRotationVector() );
	//r3dOutToLog("!!! NoClipBox::OnCreate(0x%p) Pos:<%.2f, %.2f, %.2f>, Scl:<%.2f, %.2f, %.2f>, Rot:<%.2f, %.2f, %.2f>\n", this,
	//	pos.x, pos.y, pos.z, scl.x, scl.y, scl.z, rot.x, rot.y, rot.z);

#ifndef WO_SERVER
	return TRUE;
#else
	// TODO: Uncomment the code below when we get the FairFight API
	// Copy the No Clip Box data to a static array, so it can be given to FairFight, once all the boxes are done loading.
	//r3dPoint3D points[8];
	//GetPoints( points );
	//NoClipData c;
	//memcpy(c.points, points, sizeof( r3dPoint3D ) * 8);
	//s_vNoClipData.push_back(c);
	return FALSE;
#endif // WO_SERVER
}

BOOL obj_NoClipBox::OnDestroy()
{
	//r3dPoint3D pos( GetPosition() );
	//r3dPoint3D scl( GetScale() );
	//r3dPoint3D rot( GetRotationVector() );
	//r3dOutToLog("!!! NoClipBox::OnDestroy(0x%p) Pos:<%.2f, %.2f, %.2f>, Scl:<%.2f, %.2f, %.2f>, Rot:<%.2f, %.2f, %.2f>\n", this,
	//	pos.x, pos.y, pos.z, scl.x, scl.y, scl.z, rot.x, rot.y, rot.z);

	return TRUE;
}

void obj_NoClipBox::GetPoints(r3dPoint3D (&points)[8])
{
	r3dBoundBox bbox_noclip;
	bbox_noclip.Size	= GetScale();
	bbox_noclip.Org		= GetPosition();

	// According to the ReverbZoneBox::DoDraw, "there's a bug with rotation so I've to swap the axis." 
	r3dVector rotation = GetRotationVector();
	float temp;
	temp = rotation.y;
	rotation.y = rotation.x;
	rotation.x = temp;

	r3dGenerateOrientedBoundBoxPoints( points, 8, bbox_noclip, rotation );
}

void obj_NoClipBox::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData( node );
}

#ifndef WO_SERVER
void obj_NoClipBox::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
}

float obj_NoClipBox::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected )
{
	float starty = scry;

	//starty += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);
	//starty += 5.0f;

	starty += imgui_Static(scrx, starty, "No Clip Box Params:");

	// Stupid Physics keeps getting enabled; allow user to turn it off.
	int physicsEnabled = m_bEnablePhysics;
	starty += imgui_Checkbox(scrx, starty, "Enable Physics", &physicsEnabled, 1);

	if( physicsEnabled != (int)m_bEnablePhysics )
	{
		PropagateChange( physicsEnabled, &GameObject::UpdatePhysicsEnabled, selected ) ;
	}

	r3dPoint3D scale( GetScale() );
	starty += imgui_Value_Slider(scrx, starty, "Scale X", &scale.x, 0.01f, 100.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Scale Y", &scale.y, 0.01f, 100.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Scale Z", &scale.z, 0.01f, 100.0f, "%-02.3f");
	starty += 1;

	r3dPoint3D vScaleOrig( GetScale() );
	if ( fabsf( scale.x - vScaleOrig.x ) > 0.00001f ||
		 fabsf( scale.y - vScaleOrig.y ) > 0.00001f ||
		 fabsf( scale.z - vScaleOrig.z ) > 0.00001f )
	{
		PropagateChange( scale, &GameObject::UpdateScale, selected ) ;
	}

	r3dVector vRotation( GetRotationVector() );
	if ( vRotation.x < 0.0f )
		vRotation.x += 360.0f;
	if ( vRotation.y < 0.0f )
		vRotation.y += 360.0f;
	if ( vRotation.z < 0.0f )
		vRotation.z += 360.0f;
	// According to the ReverbZoneBox::DoDraw, "there's a bug with rotation so I've to swap the axis." 
	starty += imgui_Value_Slider(scrx, starty, "Rotation X", &vRotation.y,	0.0f, 360.0f,	"%3.2f", 1);
	starty += imgui_Value_Slider(scrx, starty, "Rotation Y", &vRotation.x,	0.0f, 360.0f,	"%3.2f", 1);
	starty += imgui_Value_Slider(scrx, starty, "Rotation Z", &vRotation.z,	0.0f, 360.0f,	"%3.2f", 1);
	starty += 1;

	r3dPoint3D vRotOrig( GetRotationVector() );
	if ( fabsf( vRotation.x - vRotOrig.x ) > 0.00001f ||
		 fabsf( vRotation.y - vRotOrig.y ) > 0.00001f ||
		 fabsf( vRotation.z - vRotOrig.z ) > 0.00001f )
	{
		PropagateChange( vRotation, &GameObject::UpdateRotation, selected ) ;
	}

	r3dPoint3D vPos( GetPosition() );
	starty += imgui_Value_Slider(scrx, starty, "Position X", &vPos.x, -FLT_MAX, FLT_MAX,	"%3.2f", 0);
	starty += imgui_Value_Slider(scrx, starty, "Position Y", &vPos.y, -FLT_MAX, FLT_MAX,	"%3.2f", 0);
	starty += imgui_Value_Slider(scrx, starty, "Position Z", &vPos.z, -FLT_MAX, FLT_MAX,	"%3.2f", 0);
	starty += 1;

	r3dPoint3D vPosOrig( GetPosition() );
	if ( fabsf( vPosOrig.x - vPos.x ) > 0.00001f ||
		 fabsf( vPosOrig.y - vPos.y ) > 0.00001f ||
		 fabsf( vPosOrig.z - vPos.z ) > 0.00001f )
	{
		PropagateChange( vPos, &GameObject::UpdatePosition, selected ) ;
	}

	return starty - scry;
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_NoClipBox::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam  )
{
	(void)cam;

	if ( !g_bEditMode )
		return;

	obj_NoClipBoxCompositeRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;

	render_arrays[ rsDrawDebugData ].PushBack( rend );
}
#endif // WO_SERVER

#endif // FINAL_BUILD
