//=========================================================================
//	Module: obj_ZombieDummy.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "ui/UIimEdit.h"

#include "obj_ZombieDummy.h"
#include "../AI/r3dPhysSkeleton.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_ZombieDummy, "obj_ZombieDummy", "Object");
AUTOREGISTER_CLASS(obj_ZombieDummy);

extern r3dCamera gCam;

int						g_zombieInstances				= 0;
const uint32_t			g_NumZombieTypes				= 3;
r3dSkeleton*	g_zombieBindSkeleton[g_NumZombieTypes]	= { NULL, NULL };
r3dAnimPool*	g_zombieAnimPool[g_NumZombieTypes]		= { NULL, NULL };

static	int	g_zombieMaxPartIds[g_NumZombieTypes][3] = {0};

extern r3dPhysSkeleton* AquireCacheSkeleton();
extern void ReleaseCachedPhysSkeleton(r3dPhysSkeleton* skel);

/////////////////////////////////////////////////////////////////////////
namespace
{
	struct ZombieMeshesDeferredRenderable: public MeshDeferredRenderable
	{
		ZombieMeshesDeferredRenderable()
		: Parent(0)
		{
		}

		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw(Renderable* RThis, const r3dCamera& Cam)
		{
			R3DPROFILE_FUNCTION("ZombieMeshesDeferredRenderable");

			ZombieMeshesDeferredRenderable* This = static_cast<ZombieMeshesDeferredRenderable*>(RThis);

			r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

			This->Parent->OnPreRender();

			MeshDeferredRenderable::Draw(RThis, Cam);
		}

		obj_ZombieDummy *Parent;
	};

//////////////////////////////////////////////////////////////////////////

	struct ZombieMeshesShadowRenderable: public MeshShadowRenderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			R3DPROFILE_FUNCTION("ZombieMeshesShadowRenderable");
			ZombieMeshesShadowRenderable* This = static_cast< ZombieMeshesShadowRenderable* >( RThis );

			r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

			This->Parent->OnPreRender();

			This->SubDrawFunc( RThis, Cam );
		}

		obj_ZombieDummy *Parent;
	};
//////////////////////////////////////////////////////////////////////////

	struct ZombieDebugRenderable : Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			ZombieDebugRenderable* This = static_cast<ZombieDebugRenderable*>( RThis );

	#ifndef FINAL_BUILD
			This->Parent->DrawDebugInfo();
	#endif
		}

		obj_ZombieDummy* Parent;
	};
} // unnamed namespace


static const char* GetZombiePartName(int type, int slot, int idx)
{
	const static char* names[] = {"Head", "Body", "Legs"};
	
	static char buf[MAX_PATH];
	if( type == 1 ) // Super Zombie
	{
		sprintf(buf, "%s\\Zombie_Super_%s_%02d.sco",
			"Data\\ObjectsDepot\\Characters",
			names[slot], 
			idx);
	}
	else			// Normal Zombie
	{
		sprintf(buf, "%s\\Zombie_%s_%02d.sco",
			"Data\\ObjectsDepot\\Zombie_Test",
			names[slot], 
			idx);
	}		
	return buf;
}

static void zombie_ScanForMaxPartIds()
{
	for(int type= 0; type < g_NumZombieTypes-1; ++type)
	{
		for(int slot=0; slot<3; slot++)
		{
			for(int i=1; i<99; i++) 
			{
				if(! r3dFileExists(GetZombiePartName(type, slot, i)))
					break;
				g_zombieMaxPartIds[type][slot] = i;
			}
		}
	}
}

obj_ZombieDummy::obj_ZombieDummy()
{
	g_zombieInstances++;
	if(g_zombieBindSkeleton[0] == NULL) 
	{
		g_zombieBindSkeleton[0] = game_new r3dSkeleton();
		g_zombieBindSkeleton[0]->LoadBinary("Data\\ObjectsDepot\\Zombie_Test\\ProperScale_AndBiped.skl");
		
		g_zombieAnimPool[0] = game_new r3dAnimPool();
		
		zombie_ScanForMaxPartIds();
	}

	if(g_zombieBindSkeleton[1] == NULL) 
	{
		g_zombieBindSkeleton[1] = game_new r3dSkeleton();
		g_zombieBindSkeleton[1]->LoadBinary("Data\\ObjectsDepot\\Characters\\Super_Zombie.skl");
		
		g_zombieAnimPool[1] = game_new r3dAnimPool();
	}
	
	m_bEnablePhysics = false;
	
	m_isSerializable = true; // for Sergey to make video

	m_typeIndex = u_random(g_NumZombieTypes-1);
	for(int i=0; i < ZOMBIE_BODY_PARTS_COUNT; i++) {
		partIds[i]  = 1 + u_random(g_zombieMaxPartIds[m_typeIndex][i]);
		zombieParts[i] = NULL;
	}
	
	physSkeleton = NULL;

	sAnimSelected[0] = 0;
}

obj_ZombieDummy::~obj_ZombieDummy()
{
	if(--g_zombieInstances == 0)
	{
		SAFE_DELETE(g_zombieAnimPool[ 0 ]);
		SAFE_DELETE(g_zombieBindSkeleton[ 0 ]);
		SAFE_DELETE(g_zombieAnimPool[ 1 ]);
		SAFE_DELETE(g_zombieBindSkeleton[ 1 ]);
	}
}

BOOL obj_ZombieDummy::OnCreate()
{
	parent::OnCreate();

	m_isSuperZombie = m_typeIndex == 1;
	anim_.Init(g_zombieBindSkeleton[ m_typeIndex ], g_zombieAnimPool[ m_typeIndex ], NULL, (DWORD)this);

	for(int i=0; i < ZOMBIE_BODY_PARTS_COUNT; i++) {
		ReloadZombiePart(m_typeIndex, i);
	}

	AnimSpeed = 1.0f;
	WalkSpeed = 0.0;
	Walking   = 0;
	ApplyWalkSpeed = 0;
	WalkIdx   = -1;
	SpawnPos  = GetPosition();
	
	r3dBoundBox bbox;
	bbox.InitForExpansion();
	for (int i = 0; i < 3; ++i)
	{
		if (zombieParts[i])
			bbox.ExpandTo(zombieParts[i]->localBBox);
	}
	SetBBoxLocal(bbox);

	if(sAnimSelected[0] == 0)
	{
		if( m_isSuperZombie )
			r3dscpy(sAnimSelected, random(2) == 0 ? "Super_Zombie_Idle_01.anm" : "Super_Zombie_Idle_02.anm");
		else
			r3dscpy(sAnimSelected, random(2) == 0 ? "Zombie_Idle_01.anm" : "Zombie_Idle_02.anm");
	}
	fAnimListOffset = 0;
	SwitchToSelectedAnim();

	anim_.Update(0.001f, r3dPoint3D(0,0,0), GetTransformMatrix());

	physSkeleton = AquireCacheSkeleton();
 	physSkeleton->linkParent(anim_.GetCurrentSkeleton(), GetTransformMatrix(), this, PHYSCOLL_NETWORKPLAYER) ;
	physSkeleton->SwitchToRagdoll(false);

	return TRUE;
}

BOOL obj_ZombieDummy::Update()
{
	parent::Update();
	
	const float TimePassed = r3dGetFrameTime();

	if(WalkSpeed > 0.01) 
	{
		r3dPoint3D pos = GetPosition();
		pos.z += r3dGetFrameTime() * WalkSpeed * (Walking ? -1 : 0);
		if(pos.z < SpawnPos.z - 4) pos.z = SpawnPos.z + 4;
		SetPosition(pos);
	}
	
#if ENABLE_RAGDOLL
	bool ragdoll = physSkeleton && physSkeleton->IsRagdollMode();
	if (!ragdoll)
#endif
	{
		D3DXMATRIX CharDrawMatrix;
		D3DXMatrixIdentity(&CharDrawMatrix);
		anim_.Update(TimePassed, r3dPoint3D(0,0,0), CharDrawMatrix);
		anim_.Recalc();
	}

	if(physSkeleton)
		physSkeleton->syncAnimation(anim_.GetCurrentSkeleton(), GetTransformMatrix(), anim_);

#if ENABLE_RAGDOLL
	if (ragdoll)
	{
		r3dBoundBox bbox = physSkeleton->getWorldBBox();
		bbox.Org -= GetPosition();
		SetBBoxLocal(bbox);
	}
#endif

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL obj_ZombieDummy::OnDestroy()
{
	parent::OnDestroy();

	for (int i = 0; i < ZOMBIE_BODY_PARTS_COUNT; ++i)
	{
		if(zombieParts[i])
			r3dGOBReleaseMesh(zombieParts[i]);
	}

	ReleaseCachedPhysSkeleton(physSkeleton);

	return TRUE;
}

void obj_ZombieDummy::OnPreRender()
{
	anim_.GetCurrentSkeleton()->SetShaderConstants();
}

#ifndef FINAL_BUILD
extern CD3DFont 	*Font_Label;
void obj_ZombieDummy::DrawDebugInfo() const
{
	r3dRenderer->SetMaterial(NULL);

	struct PushRestoreStates
	{
		PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
		}

		~PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}
	} pushRestoreStates; (void)pushRestoreStates;

	r3dPoint3D scrCoord(10, 200, 0);
	//if(r3dProjectToScreen(GetPosition() + r3dPoint3D(0, 1, 0), &scrCoord))
	{
		r3dString animsPlaying;
		for(r3dAnimation::AnimTracksVec::const_iterator iter = anim_.AnimTracks.begin();
			iter != anim_.AnimTracks.end(); ++iter)
		{
			if( iter->GetStatus() & ANIMSTATUS_Playing )
			{
				const char* shortName =  strrchr( iter->GetAnim()->GetAnimFileName(), '\\' );
				if( shortName == NULL )
					shortName = iter->GetAnim()->GetAnimFileName();
				else
					shortName = shortName + 1;
				char animSpeed[ 10 ];
				char animCurFrame[ 10 ];
				char animFrames[ 10 ];
				char animRate[ 10 ];
				sprintf( animSpeed, "(%.2f", iter->GetSpeed() );
				sprintf( animCurFrame, "[%.0f/", iter->GetCurFrame() );
				sprintf( animFrames, "%d,", iter->GetAnim()->GetNumFrames() );
				sprintf( animRate, "%.2f])", iter->GetAnim()->GetFrameRate() );
				if( 0 < animsPlaying.Length() )
					animsPlaying = animsPlaying + r3dString(", ") + r3dString( shortName ) + r3dString( animSpeed ) + r3dString( animCurFrame ) + r3dString( animFrames ) + r3dString( animRate );
				else
					animsPlaying = r3dString( shortName ) + r3dString( animSpeed ) + r3dString( animCurFrame ) + r3dString( animFrames ) + r3dString( animRate );
			}
		}
		Font_Label->PrintF(scrCoord.x, scrCoord.y,   r3dColor(255,255,255), "%s", animsPlaying.c_str() );
	}
}
#endif

void obj_ZombieDummy::AppendRenderables(RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam)
{
	for (int i = 0; i < 3; ++i)
	{
		r3dMesh *mesh = zombieParts[i];

		if (!mesh)
			continue;

		uint32_t prevCount = render_arrays[ rsFillGBuffer ].Count();
		mesh->AppendRenderablesDeferred( render_arrays[ rsFillGBuffer ], r3dColor::white, GetSelfIllumMultiplier() );

		for( uint32_t i = prevCount, e = render_arrays[ rsFillGBuffer ].Count(); i < e; i ++ )
		{
			ZombieMeshesDeferredRenderable& rend = static_cast<ZombieMeshesDeferredRenderable&>( render_arrays[ rsFillGBuffer ][ i ] ) ;

			rend.Init();
			rend.Parent = this;
		}
	}
#ifndef FINAL_BUILD
	if(r_show_zombie_stats->GetBool())
	{
		ZombieDebugRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= 2*RENDERABLE_USER_SORT_VALUE;
		render_arrays[ rsDrawFlashUI ].PushBack( rend );
	}
#endif
}

void obj_ZombieDummy::AppendShadowRenderables(RenderArray &rarr, int sliceIndex, const r3dCamera& cam)
{
	float distSq = (gCam - GetPosition()).LengthSq();
	float dist = sqrtf( distSq );
	UINT32 idist = (UINT32) R3D_MIN( dist * 64.f, (float)0x3fffffff );

	for (int i = 0; i < 3; ++i)
	{
		r3dMesh *mesh = zombieParts[i];
		
		if (!mesh)
			continue;

		uint32_t prevCount = rarr.Count();

		mesh->AppendShadowRenderables( rarr );

		for( uint32_t i = prevCount, e = rarr.Count(); i < e; i ++ )
		{
			ZombieMeshesShadowRenderable& rend = static_cast<ZombieMeshesShadowRenderable&>( rarr[ i ] );

			rend.Init() ;
			rend.SortValue |= idist;
			rend.Parent = this ;
		}
	}
}

int obj_ZombieDummy::AddAnimation(const char* anim)
{
	char buf[MAX_PATH];
	sprintf(buf, "Data\\Animations5\\%s.anm", anim);
	int aid = g_zombieAnimPool[ m_typeIndex ]->Add(anim, buf);
	
	return aid;
}	

void obj_ZombieDummy::SwitchToSelectedAnim()
{
	char buf[MAX_PATH];
	sprintf(buf, "Data\\Animations5\\%s", sAnimSelected);
	int aid = g_zombieAnimPool[ m_typeIndex ]->Add(sAnimSelected, buf);
	anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherNow | ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
	
	anim_.AnimTracks[0].fSpeed = AnimSpeed * ((Walking && ApplyWalkSpeed) ? WalkSpeed : 1.0f);
}

void obj_ZombieDummy::ReloadZombiePart(int type, int slot)
{
	const char* buf = GetZombiePartName(type, slot, partIds[slot]);
	//r3dOutToLog("%d to %s\n", slot, buf);
	zombieParts[slot] = r3dGOBAddMesh(buf);
}

#ifndef FINAL_BUILD
float obj_ZombieDummy::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	//starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );
	if( ! IsParentOrEqual( &ClassData, startClass ) )	
		return starty - scry ;

	{
		int idx = m_typeIndex;
		starty += imgui_Checkbox(scrx, starty, "Super Zombie", &m_typeIndex, 1);
		if(idx != m_typeIndex)
		{
			// Destroy and Re-Create the zombie.
			m_isSuperZombie = m_typeIndex == 1;
			OnDestroy();
			sAnimSelected[0] = 0;
			for(int i=0; i < ZOMBIE_BODY_PARTS_COUNT; i++) {
				partIds[i]  = 1 + u_random(g_zombieMaxPartIds[m_typeIndex][i]);
				zombieParts[i] = NULL;
			}
			ObjFlags |= OBJFLAG_JustCreated;
			OnCreate();
		}
	}
	if( !m_isSuperZombie )
	{
		for(int i=0; i < ZOMBIE_BODY_PARTS_COUNT; i++)
		{
			const static char* names[] = {"Head", "Body", "Legs"};

			int idx = partIds[i];
			starty += imgui_Value_SliderI(scrx, starty, names[i], &partIds[i], 1, (float)g_zombieMaxPartIds[m_typeIndex][i], "%d");
			if(idx != partIds[i]) 
			{
				ReloadZombiePart(m_typeIndex, i);
			}
		}
	}

	starty += imgui_Checkbox(scrx, starty, "Walking", &Walking, 1);
	starty += imgui_Value_Slider(scrx, starty, "WalkSpeed", &WalkSpeed, 0, 6, "%f");
	starty += imgui_Checkbox(scrx, starty, "ApplyWalkSpeed", &ApplyWalkSpeed, 1);
	starty += imgui_Value_Slider(scrx, starty, "AnimSpeed", &AnimSpeed, 0, 5, "%f");
	if(anim_.AnimTracks.size() > 0)
		anim_.AnimTracks[0].fSpeed = AnimSpeed * ((Walking && ApplyWalkSpeed) ? WalkSpeed : 1.0f);

	starty += imgui_Static(scrx, starty, " Select Animation");
	{
		float flh = 250;
		r3dSTLString sDirFind;
		if( m_isSuperZombie )
			sDirFind = "Data\\Animations5\\Super_Zombie*.anm";
		else
			sDirFind = "Data\\Animations5\\*.anm";
		if(imgui_FileList(scrx, starty, 360, flh, sDirFind.c_str(), sAnimSelected, &fAnimListOffset, true))
		{
			SwitchToSelectedAnim();
		}
		starty += flh;
	}

	return starty - scry ;
}
#endif

void obj_ZombieDummy::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node zombiedummy = node.child("zombiedummy");
	partIds[0] = zombiedummy.attribute("part0").as_int();
	partIds[1] = zombiedummy.attribute("part1").as_int();
	partIds[2] = zombiedummy.attribute("part2").as_int();
	r3dscpy(sAnimSelected, zombiedummy.attribute("anim").value());
}

void obj_ZombieDummy::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node zombiedummy = node.append_child();
	zombiedummy.set_name("zombiedummy");
	zombiedummy.append_attribute("part0") = partIds[0];
	zombiedummy.append_attribute("part1") = partIds[1];
	zombiedummy.append_attribute("part2") = partIds[2];
	zombiedummy.append_attribute("anim") = sAnimSelected;
}

void obj_ZombieDummy::DoHit()
{
	if(physSkeleton->IsRagdollMode())
		return;

	if( m_isSuperZombie )
	{
		static const char* anims[] = {
			"Super_Zombie_Staggered_01_B.anm",
			"Super_Zombie_Staggered_01_F.anm",
			"Super_Zombie_Staggered_01_L.anm",
			"Super_Zombie_Staggered_01_R.anm",
			"Super_Zombie_StaggerStep_01_B.anm",
			"Super_Zombie_StaggerStep_01_F.anm",
			"Super_Zombie_StaggerStep_01_L.anm",
			"Super_Zombie_StaggerStep_01_R.anm"
			};

		const char* hitAnim = anims[u_random(8)];
		
		char buf[MAX_PATH];
		sprintf(buf, "Data\\Animations5\\%s", hitAnim);
		int aid = g_zombieAnimPool[ m_typeIndex ]->Add(hitAnim, buf);
		anim_.StartAnimation(aid, 0, 0.0f, 1.0f, 0.1f);
	}
	else
	{
		static const char* anims[] = {
			"Zombie_Staggered_01_B.anm",
			"Zombie_Staggered_01_L.anm",
			"Zombie_Staggered_01_R.anm",
			"Zombie_Staggered_Small_01_B.anm",
			"Zombie_Staggered_Small_01_F.anm"
			};

		const char* hitAnim = anims[u_random(5)];
		
		char buf[MAX_PATH];
		sprintf(buf, "Data\\Animations5\\%s", hitAnim);
		int aid = g_zombieAnimPool[ m_typeIndex ]->Add(hitAnim, buf);
		anim_.StartAnimation(aid, 0, 0.0f, 1.0f, 0.1f);
	}
}
