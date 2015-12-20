#include "r3dPCH.h"
#include "r3d.h"

#include "Gameplay_Params.h"
#include "GameCommon.h"
#include "../ai/AI_Player.H"

#include "Particle.h"
#include "obj_PlayerSpawnPoint.h"
#include "gameobjects/obj_Mesh.h"

#include "multiplayer/P2PMessages.h"
#include "multiplayer/ClientGameLogic.h"

#include "Editors/ObjectManipulator3d.h"

#include "..\..\ui\HUDDisplay.h"
extern HUDDisplay*	hudMain;

IMPLEMENT_CLASS(obj_PlayerSpawnPoint, "obj_PlayerSpawnPoint", "Object");
AUTOREGISTER_CLASS(obj_PlayerSpawnPoint);

extern bool g_bEditMode;

int g_bShowDistanceToControlPoint = 0;

obj_PlayerSpawnPoint::obj_PlayerSpawnPoint()
{
	m_bEnablePhysics = false; // they do not need physics
	m_SelectedSpawnPoint = 0;
	m_bEditorCheckSpawnPointAtStart = true;
	ObjTypeFlags |= OBJTYPE_PlayerSpawnPoint;
}

obj_PlayerSpawnPoint::~obj_PlayerSpawnPoint()
{
}

BOOL obj_PlayerSpawnPoint::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\Capture_Points\\Flag_Pole_01.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
		ObjFlags |= OBJFLAG_SkipCastRay;

#ifndef FINAL_BUILD
	if(g_bEditMode) // to make it easier in editor to edit spawn points
	{
		setSkipOcclusionCheck(true);
		ObjFlags |= OBJFLAG_AlwaysDraw | OBJFLAG_ForceSleep ;
	}
#endif

	return TRUE;
}

BOOL obj_PlayerSpawnPoint::OnCreate()
{
	parent::OnCreate();

	r3dBoundBox bboxLocal ;
	bboxLocal.Size = r3dPoint3D(2, 2, 2);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;
	SetBBoxLocal(bboxLocal) ;
	UpdateTransform();

	return 1;
}


BOOL obj_PlayerSpawnPoint::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_PlayerSpawnPoint::Update()
{
	if(g_bEditMode && m_bEditorCheckSpawnPointAtStart)
	{
		m_bEditorCheckSpawnPointAtStart = false;
		for(int i=0; i<m_NumSpawnPoints; ++i)
		{
			PxVec3 from(m_SpawnPoints[i].pos.x, m_SpawnPoints[i].pos.y+1.0f, m_SpawnPoints[i].pos.z);
			PxRaycastHit hit;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
			if(g_pPhysicsWorld->raycastSingle(from, PxVec3(0,-1,0), 10000, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
			{
				r3dPoint3D hitPos(hit.impact.x, hit.impact.y, hit.impact.z);
				if(R3D_ABS(m_SpawnPoints[i].pos.y - hitPos.y) > 2.0f)
				{
					r3dArtBug("Player CP Spawn at %.2f, %.2f, %.2f has spawn position(index=%d) in the air at %.2f, %.2f, %.2f\n", 
						GetPosition().x, GetPosition().y, GetPosition().z, i+1, m_SpawnPoints[i].pos.x, m_SpawnPoints[i].pos.y, m_SpawnPoints[i].pos.z);
				}
			}
			else
			{
			}
			
			if(Terrain)
			{
				float y1 = m_SpawnPoints[i].pos.y;
				float y2 = Terrain->GetHeight(m_SpawnPoints[i].pos);
				float d  =  y1 - y2;
				if(d < 0)
				{
					m_SpawnPoints[i].pos.y = y2+0.1f;
					r3dArtBug("Player CP Spawn at %.2f, %.2f, %.2f has spawn position(index=%d) at %.2f, %.2f, %.2f under terrain, y:%f, terra:%f, diff: %f\nIT WAS AUTO CORRECTED TO BE ABOVE TERRAIN! Please save map\n\n", 
						GetPosition().x, GetPosition().y, GetPosition().z, i+1, m_SpawnPoints[i].pos.x, m_SpawnPoints[i].pos.y, m_SpawnPoints[i].pos.z,
						y1, y2, d);
				}
			}
		}
		r3dShowArtBugs();
	}
	return parent::Update();
}

//------------------------------------------------------------------------
struct ControlPointShadowGBufferRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ControlPointShadowGBufferRenderable *This = static_cast<ControlPointShadowGBufferRenderable*>( RThis );

		This->Parent->MeshGameObject::Draw( Cam, This->DrawState );
	}

	obj_PlayerSpawnPoint*	Parent;
	eRenderStageID		DrawState;
};

struct ControlPointCompositeRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ControlPointCompositeRenderable *This = static_cast<ControlPointCompositeRenderable*>( RThis );

		This->Parent->DoDrawComposite( Cam );
	}

	obj_PlayerSpawnPoint*	Parent;	
};



#define RENDERABLE_CTRLPOINT_SORT_VALUE (6*RENDERABLE_USER_SORT_VALUE)

/*virtual*/

void
obj_PlayerSpawnPoint::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
		return;

	ControlPointShadowGBufferRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_CTRLPOINT_SORT_VALUE;
	rend.DrawState	= rsCreateSM;

	rarr.PushBack( rend );
}

//------------------------------------------------------------------------
/*virtual*/

void
obj_PlayerSpawnPoint::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
		return;

	// gbuffer
	{
		ControlPointShadowGBufferRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_CTRLPOINT_SORT_VALUE;
		rend.DrawState	= rsFillGBuffer;

		render_arrays[ rsFillGBuffer ].PushBack( rend );
	}

#ifdef FINAL_BUILD
	return;
#else
	if(r_hide_icons->GetInt())
		return;

	// composite
	if(g_bEditMode)
	{
		ControlPointCompositeRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_CTRLPOINT_SORT_VALUE;

		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
#endif

}

//------------------------------------------------------------------------
void obj_PlayerSpawnPoint::DoDrawComposite( const r3dCamera& Cam )
{
#ifndef FINAL_BUILD
	// draw them, so we can see them
	r3dPoint3D off(0, 4, 0);
	r3dColor   clr(255, 255, 255);
	if(spawnType_ == SPAWN_BASE) clr = r3dColor(255, 0, 0);
	
	r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);
	
	r3dDrawLine3D(GetPosition(), GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, clr);

	// draw circles
	for(int i=0; i<m_NumSpawnPoints; ++i)
	{
		D3DXMATRIX tempM;
		D3DXMatrixRotationY(&tempM, R3D_DEG2RAD(-m_SpawnPoints[i].dir));
		D3DXVECTOR3 tempV, tempV2; tempV2 = D3DXVECTOR3(0,0,1);
		D3DXVec3TransformNormal(&tempV, &tempV2, &tempM);

		r3dDrawCircle3D(m_SpawnPoints[i].pos, 2.0f, Cam, 0.4f, ((i==m_SelectedSpawnPoint&&g_Manipulator3d.PickedObject() == this)?r3dColor24::red:r3dColor24::grey));
		r3dDrawLine3D(m_SpawnPoints[i].pos, m_SpawnPoints[i].pos+r3dPoint3D(tempV.x, 0, tempV.z)*2.0f, Cam, 0.4f, r3dColor24::blue);
	}
	r3dRenderer->Flush();
	r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

	if(g_bShowDistanceToControlPoint)
	{
		r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_ALPHA | R3D_BLEND_PUSH);
		r3dPoint3D scrCoord;
		if(r3dProjectToScreen(GetPosition()+r3dPoint3D(0,15,0), &scrCoord))
			Font_Label->PrintF(scrCoord.x, scrCoord.y, r3dColor24::white, "%.2f", (Cam-GetPosition()).Length());
		r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		r3dRenderer->Flush();
	}
#endif
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
float obj_PlayerSpawnPoint::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Control Point Parameters" );
		starty += imgui_Checkbox(scrx, starty, "Show Distances", &g_bShowDistanceToControlPoint, 1);

		static stringlist_t names;
		static float offset = 0;

		if( !names.size() )
		{
			names.push_back( "BASE" );
			names.push_back( "NEUTRAL" );
		}

		int sel = spawnType_ ;

		const float LIST_HEIGHT = 55;

		starty += imgui_Static( scrx, starty, "TYPE" );

		imgui_DrawList( scrx, starty, 360, LIST_HEIGHT, names, &offset, &sel );

		starty += LIST_HEIGHT;

		PropagateChange( sel, &obj_PlayerSpawnPoint::spawnType_, this, selected ) ;

		// don't allow multi edit of this
		if( selected.Count() <= 1 )
		{
			if(m_NumSpawnPoints < MAX_SPAWN_POINTS)
			{
				if(imgui_Button(scrx+110, starty, 100, 25, "Add Location"))
				{
					m_SpawnPoints[m_NumSpawnPoints].pos = GetPosition() + r3dPoint3D(2, 0, 2);
					m_SpawnPoints[m_NumSpawnPoints].dir = 0;
					m_SelectedSpawnPoint = m_NumSpawnPoints;
					m_NumSpawnPoints++;
				}
			}

			starty += 25;

			for(int i=0; i<m_NumSpawnPoints; ++i)
			{
				// selection button
				char tempStr[32];
				sprintf(tempStr, "Location %d", i+1);
				if(imgui_Button(scrx, starty, 100, 25, tempStr, i==m_SelectedSpawnPoint))
				{
					// shift click on location will set camera to it
					if(Keyboard->IsPressed(kbsLeftShift))
					{
						extern BaseHUD* HudArray[6];
						extern int CurHUDID;
						HudArray[CurHUDID]->FPS_Position = m_SpawnPoints[i].pos;
						HudArray[CurHUDID]->FPS_Position.y += 0.1f;
					}
					
					m_SelectedSpawnPoint = i;
				}
				
				// delete button
				if(m_NumSpawnPoints > 1)
				{
					if(imgui_Button(scrx + 110, starty, 100, 25, "DEL"))
					{
						if(i!=m_NumSpawnPoints-1)
						{
							m_SpawnPoints[i] = m_SpawnPoints[m_NumSpawnPoints-1]; // just move last point into this one
						}
						
						m_NumSpawnPoints--;
					}
				}
				imgui_Value_Slider(scrx+220, starty, "Direction", &m_SpawnPoints[i].dir, 0.0f, 360.0f, "%.2f", false, true, 160);
				starty += 26;

				m_SelectedSpawnPoint = R3D_CLAMP(m_SelectedSpawnPoint, 0, m_NumSpawnPoints-1);
			}

			extern r3dPoint3D UI_TargetPos;
			extern gobjid_t UI_TargetObjID;
			if((Mouse->IsPressed(r3dMouse::mLeftButton)) && Keyboard->IsPressed(kbsLeftControl) && UI_TargetObjID != this->ID && m_NumSpawnPoints>0)
				m_SpawnPoints[m_SelectedSpawnPoint].pos = UI_TargetPos + r3dPoint3D(0.0f, 0.1f, 0.0f);

			starty += imgui_Static(scrx, starty, "Tip: SHIFT-Location button click - navigate to location");
			starty += imgui_Static(scrx, starty, "Tip: CTRL-click - reposition current location");
		}
	}

	return starty-scry;
}
#endif

BOOL obj_PlayerSpawnPoint::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

void obj_PlayerSpawnPoint::OnPickerPick(int locationId)
{
	if (locationId >= 0 && locationId < static_cast<int>(m_NumSpawnPoints))
	{
		m_SelectedSpawnPoint = locationId;
	}
}

GameObject *obj_PlayerSpawnPoint::Clone()
{
	obj_PlayerSpawnPoint * pNew = (obj_PlayerSpawnPoint*)srv_CreateGameObject("obj_PlayerSpawnPoint", FileName.c_str(), GetPosition () );
	GameObject::PostCloneParamsCopy(pNew);
	pNew->spawnType_ = spawnType_;

	return pNew;
}
