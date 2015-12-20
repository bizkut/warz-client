//
//
//  General Particle Emmiter
//  Pass 
//
#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "GameLevel.h"

#include "particle.h"
#include "particle_int.h"
#include "obj_ParticleSystem.h"


IMPLEMENT_CLASS(obj_ParticleSystem, "obj_ParticleSystem", "Object");
AUTOREGISTER_CLASS(obj_ParticleSystem);

extern bool g_bEditMode;
extern int g_bForceQualitySelectionInEditor;

static r3dTexture *ParticleIcon = NULL;
//-----------------------------------------------------------------------
obj_ParticleSystem::obj_ParticleSystem()
{
	ObjTypeFlags |= OBJTYPE_Particle;
	ObjFlags |= OBJFLAG_IconSelection;
	LightLifetime = 0;
	Torch = 0;
	HackedFOV = -1;
	UpdateTime = r3dGetTime();

#ifndef FINAL_BUILD
	DoFreese = false;
#endif
}

obj_ParticleSystem::~obj_ParticleSystem()
{

}

//------------------------------------------------------------------------

/*static*/

void
obj_ParticleSystem::GetParticlePath( char* oPath, const char* particleName )
{
	sprintf( oPath, "Data\\Particles\\%s.prt", particleName );
}

//------------------------------------------------------------------------

BOOL obj_ParticleSystem::Load(const char* fname)
{
	char Str1[256];

	GetParticlePath( Str1, fname );

	if(!parent::Load(fname)) return FALSE;

	R3DPROFILE_START("Particles: CREATION");

	if (!ParticleIcon) ParticleIcon = r3dRenderer->LoadTexture("Data\\Images\\Particle.dds");


	DrawOrder      = OBJ_DRAWORDER_LAST;

	m_isSerializable = false;
	bKeepAlive   = false;

#ifndef FINAL_BUILD
	extern bool g_bEditMode;
	if(!g_bEditMode) 
#endif
		ObjFlags      |= OBJFLAG_SkipCastRay;

	Name = fname;

	BindID       = invalidGameObjectID;
	BeamTargetID = invalidGameObjectID;
	BindType     = BINDTYPE_NONE;
	bKill        = 0;
	bKillDelayed = 0;
	RenderScale  = 1;
	GlobalScale  = 1;
	bUseBone     = 0;
	bRender      = 1;

	Torch = r3dParticleSystemLoad(Str1);
	R3DPROFILE_END("Particles: CREATION");

	if(!Torch) return FALSE;

	return TRUE;
}

void obj_ParticleSystem::SetScale(const r3dBoundBox &Box)
{
	RenderScale = Box.Size.Y / 80.0f;
	if(RenderScale < 1.0f ) RenderScale = 1.0f;
}

//-----------------------------------------------------------------------
void obj_ParticleSystem::Reload()
//-----------------------------------------------------------------------
{
	SAFE_DELETE(Torch);

	char Str1[256];

	sprintf(Str1,"Data\\Particles\\%s.prt", Name.c_str());

	Torch = r3dParticleSystemLoad(Str1);
	Torch->Position = GetPosition();
	UpdateTime = r3dGetTime();
	Torch->Restart(UpdateTime);

	return;
}

void obj_ParticleSystem::Restart()
{
	float curTime = r3dGetTime() ;

	Torch->Position = GetPosition();
	UpdateTime = curTime;
	Torch->Restart( curTime );
}

BOOL obj_ParticleSystem::OnCreate()
{
	parent::OnCreate();

	setSkipOcclusionCheck(true); // until proper aabb will be generated for particles!!
	ObjFlags |= OBJFLAG_DisableShadows | OBJFLAG_ForceSleep;

	UpdateTime = r3dGetTime();
	Torch->GlobalScale = GlobalScale;
	Torch->Position = GetPosition();
	Torch->Restart(UpdateTime);

	float size = SRV_WORLD_SCALE(1.0f);

	r3dBoundBox bboxLocal ;

	bboxLocal.Size.Assign(size, size, size);
	bboxLocal.Org = - bboxLocal.Size * 0.5f;

	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();

	return TRUE;
}



BOOL obj_ParticleSystem::OnDestroy()
{
	r3dGameLevel::Environment->OnParticleSystemDelete( this );

	SAFE_DELETE(Torch);

	return parent::OnDestroy();
}

struct ParticleRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ParticleRenderable *This = static_cast<ParticleRenderable*>( RThis );

#ifndef FINAL_BUILD
		if( r_show_draw_order->GetInt() == 2 )
		{
			extern Positions gDEBUG_DrawPositions ;
			gDEBUG_DrawPositions.PushBack( This->Parent->GetPosition() );
		}
#endif

		This->Parent->DrawParticles( Cam );
	}

	obj_ParticleSystem*	Parent;	
};

struct ParticleShadowRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ParticleShadowRenderable *This = static_cast<ParticleShadowRenderable*>( RThis );

		This->Parent->DrawParticleShadows( Cam );
	}

	obj_ParticleSystem*	Parent;	
};

struct ParticleMeshRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ParticleMeshRenderable *This = static_cast<ParticleMeshRenderable*>( RThis );

		This->Parent->DrawMeshes( Cam, This->DrawState );
	}

	obj_ParticleSystem*	Parent;
	eRenderStageID		DrawState;
};


extern INT64 RENDERABLE_EMITTER_USER_SORT_VALUE;

/*virtual*/
void
obj_ParticleSystem::AppendTransparentShadowRenderables( RenderArray & rarr, const r3dCamera& cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	if( !r_draw_particles->GetInt() )
		return ;
#endif

	// particles
	{
		ParticleShadowRenderable rend;

		rend.Init();

		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_EMITTER_USER_SORT_VALUE ;

		rarr.PushBack( rend );
	}
}

/*virtual*/
void
obj_ParticleSystem::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	if( !r_draw_particles->GetInt() )
		return ;

	if(!g_bEditMode || g_bForceQualitySelectionInEditor)
#endif
	{
		int effectQuality = (int)m_MinQualityLevel;
		if(effectQuality > (int)r_environment_quality->GetMaxVal()) effectQuality = (int)r_environment_quality->GetMaxVal();
		if(effectQuality > r_environment_quality->GetInt())
			return;
	}

	if (!bRender) return;
	r3dVector V = Cam - GetPosition();
	if (V.Length() > 2600 ) return; 

	ParticleMeshRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.DrawState	= rsCreateSM;
	rend.SortValue	= RENDERABLE_EMITTER_USER_SORT_VALUE;

	rarr.PushBack( rend );
}

void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
struct ParticleHelperRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ParticleHelperRenderable *This = static_cast<ParticleHelperRenderable*>( RThis );

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

		if((This->Parent->GetPosition() - Cam).Length() < 100)
			r3dDrawIcon3D(This->Parent->GetPosition(), ParticleIcon, r3dColor(255,255,255), 32);

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
	}

	obj_ParticleSystem* Parent;	
};

//------------------------------------------------------------------------
/*virtual*/
void
obj_ParticleSystem::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	if( !r_draw_particles->GetInt() )
		return ;

	if(!g_bEditMode || g_bForceQualitySelectionInEditor)
#endif
	{
		int effectQuality = (int)m_MinQualityLevel;
		if(effectQuality > (int)r_environment_quality->GetMaxVal()) effectQuality = (int)r_environment_quality->GetMaxVal();
		if(effectQuality > r_environment_quality->GetInt())
			return;
	}

	if (!bRender) return;
	r3dVector V = Cam - GetPosition();
	float len = V.Length();
	if (len > Torch->PD->CullDistance )
		return; 

	int idist = GetRevIDist( len ) ;

	// particles
	{
		ParticleRenderable rend;

		rend.Init();

		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_EMITTER_USER_SORT_VALUE | idist;

		render_arrays[ rsDrawTransparents ].PushBack( rend );
	}

	// gbuffer
	{
		ParticleMeshRenderable rend;

		rend.Init();

		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_EMITTER_USER_SORT_VALUE | idist;
		rend.DrawState	= rsFillGBuffer;

		render_arrays[ rsFillGBuffer ].PushBack( rend );
	}

#ifndef FINAL_BUILD
	// helper
	extern int CurHUDID;

	float idd = r_icons_draw_distance->GetFloat();

	if(CurHUDID == 0 && !r_hide_icons->GetInt() && ( Cam - GetPosition() ).LengthSq() < idd * idd && g_bEditMode )
	{
		ParticleHelperRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_EMITTER_USER_SORT_VALUE;
		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}
#endif
}

void
obj_ParticleSystem::DrawParticles( const r3dCamera& Cam )
{
	R3DPROFILE_FUNCTION("DrawParticles");
	r3dRenderer->SetMaterial(NULL);

	Torch->GlobalRenderScale = RenderScale;

	r3dCamera Cam1 = Cam;
	if(HackedFOV > 0)
	{
		Cam1.FOV = HackedFOV;
		r3dRenderer->SetCamera(Cam1, false);
	}

	Torch->Draw(Cam1, false);

	r3dRenderer->Flush();
	r3dRenderer->SetMaterial(NULL);

	if(HackedFOV > 0)
		r3dRenderer->SetCamera(Cam, false);

	Torch->GlobalRenderScale = 1.0f;

#if 0
	r3dDrawBoundBox(BBox, Cam, r3dColor(255,0,0));
#endif

}

void obj_ParticleSystem::DrawParticleShadows( const r3dCamera& Cam )
{
	R3DPROFILE_FUNCTION("DrawParticleShadows");
	r3dRenderer->SetMaterial(NULL);

	Torch->GlobalRenderScale = RenderScale;
	Torch->Draw(Cam, true);

	Torch->GlobalRenderScale = 1.0f;

}

void obj_ParticleSystem::DrawMeshes( const r3dCamera& Cam, eRenderStageID DrawState )
{
	R3DPROFILE_FUNCTION("DrawPartilceMeshes");
	Torch->GlobalRenderScale = RenderScale;
	Torch->DrawDefferedMeshes(Cam, DrawState == rsCreateSM);
	Torch->GlobalRenderScale = 1.0f;
}

#ifndef FINAL_BUILD
float obj_ParticleSystem::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		int temp = m_isSerializable?1:0;
		starty += imgui_Checkbox(scrx, starty, "Serializable", &temp, 1);
		bool btemp = temp?true:false;

		PropagateChange( btemp, &obj_ParticleSystem::m_isSerializable, this, selected ) ;

		temp = DoFreese ? 1 : 0;
		static float freeseTimeOffset = 0;
		starty += imgui_Value_Slider(scrx, starty, "Pause time", &freeseTimeOffset, 0.0f, Torch->PD->EmitTime, "%.2f");
		starty += imgui_Checkbox(scrx, starty, "Pause", &temp, 1);
		if (!!temp != DoFreese)
		{
			float freeseTime = r3dGetTime() + freeseTimeOffset;
			bool doFreese = !!temp;
			if (doFreese)
			{
				Restart();
				//	Advance to particle pause position
				while (UpdateTime < freeseTime)
				{
					Update();
				}
			}
			DoFreese = doFreese;
		}

		if( selected.Count() <= 1 )
		{
			void ParticleEditorSetDefaultParticle ( const char * );
			ParticleEditorSetDefaultParticle ( Name.c_str() );

			if ( imgui_Button ( scrx,starty, 180.0f, 25.0f, "Edit Particle" ) )
			{
				void ParticleEditorSetDefaultParticle ( const char * );
				void SetHud ( int );

				ParticleEditorSetDefaultParticle ( Name.c_str() );
				SetHud ( 3 );
			}
		}

		starty += 25.0f;
	}
	return starty-scry;
}
#endif

void obj_ParticleSystem::OnPickerRotated()
{
	// update Torch, so that artists can rotate directional particles in the editor (when placing them in the world)
	D3DXMATRIX mRot = GetRotationMatrix();
	RotateParticleSystem(mRot);
}

//////////////////////////////////////////////////////////////////////////

void obj_ParticleSystem::RotateParticleSystem(const D3DXMATRIX &rot)
{
	r3dVector yDir(rot._21, rot._22, rot._23);
	if(Torch)
	{
		Torch->Direction.Assign(yDir);

		for( int i = 0, e = Torch->ArraySize ; i < e ; i ++ )
		{
			Torch->Array[ i ].Direction.Assign( yDir );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_ParticleSystem::Update()
{
//	float TimePassed = r3dGetFrameTime();

#ifndef FINAL_BUILD
	if (!DoFreese)
	{
		UpdateTime += r3dGetFrameTime();
	}
#else
	UpdateTime = r3dGetTime();
#endif

	if( !!IsTransparentShadowCaster() ^ !!Torch->PD->CastsShadows() )
	{
		SetTransparentShadowCasting( !!Torch->PD->CastsShadows() ) ;
	}

	if(bKill) 
		return FALSE;

	if(bKillDelayed)
	{
		if(Torch) // just to make sure, stop emiting and turn off light
		{
			Torch->ParticleLight.TurnOff();
			Torch->bEmit = 0;
		}
	}
	if(bKillDelayed && Torch->NumAliveParticles == 0)
		return FALSE;

	if(!Torch->bEmit && !Torch->NumAliveParticles && !bKeepAlive) 
		return FALSE;

	R3DPROFILE_START("Particles: Update");
	// update particle emitter position
	Torch->Position = GetPosition();

	do {
		if(BindID == invalidGameObjectID) 
			break;

		const GameObject *tobj = GameWorld().GetObject(BindID);
		if(!tobj) {
			bKill = 1;     
			break;
		}

		Torch->Position = tobj->GetPosition();
		SetPosition(tobj->GetPosition());
		break;

	} while(0);

	if(BeamTargetID != invalidGameObjectID)
	{
		const GameObject* tobj1 = GameWorld().GetObject(BeamTargetID);
		if(tobj1) 
		{
			Torch->Position.Y += tobj1->GetBBoxWorld().Size.Y;
			Torch->BeamTargetPosition = tobj1->GetPosition() + r3dPoint3D(0,tobj1->GetBBoxWorld().Size.Y,0);
		}
	}


	Torch->GlobalScale = GlobalScale;
	Torch->Update(UpdateTime);

#ifndef FINAL_BUILD
	// Zero BBox will remove renderables from rendering, but we need particle icon to be drawn, so force correct bbox position
	r3dBoundBox bb = Torch->BBox;
	if (bb.Org.Dot(bb.Org) == 0 && bb.Size.Dot(bb.Size) == 0)
	{
		bb.Org = GetPosition();
	}
	SetBBoxLocal( bb ) ;
#else
	SetBBoxLocal( Torch->BBox ) ;
#endif

	extern int CurHUDID;
	/*if(g_bEditMode && CurHUDID == 0) // in editor mode, force small bbox for easier selection
	{
		bbox_local.Org = GetPosition();
		bbox_local.Size.Assign(2, 2, 2);
	}*/
#ifndef FINAL_BUILD
	if(g_bEditMode && CurHUDID == 3) // in particle editor mode skip collision check with particles
		ObjFlags |= OBJFLAG_SkipCastRay;
#endif

	// this code prevented particle placement to work correctly in editor
#if 0
	else
		ObjFlags &= ~OBJFLAG_SkipCastRay;
#endif

	UpdateTransform();

	R3DPROFILE_END("Particles: Update");
	return TRUE;
}

void obj_ParticleSystem::ReadSerializedData(pugi::xml_node& node)
{
	m_isSerializable = true; // if read from xml, than this particle should be saved
	bKeepAlive = true;
	parent::ReadSerializedData(node);
}
