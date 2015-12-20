//=========================================================================
//	Module: obj_DeerDummy.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "ui/UIimEdit.h"

#include "obj_DeerDummy.h"
#include "../AI/r3dPhysSkeleton.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_DeerDummy, "obj_DeerDummy", "Object");
AUTOREGISTER_CLASS(obj_DeerDummy);

extern r3dCamera gCam;

enum EDeerVariants
{
	 DEERVNT_Buck = 0
	,DEERVNT_Doe
};
static const char* g_deerVariants[] = {
	"Buck", "Doe"
};

int				g_deerInstances    = 0;
r3dSkeleton*	g_deerBindSkeleton = NULL;
r3dAnimPool*	g_deerAnimPool     = NULL;

extern r3dPhysSkeleton* AquireCacheSkeleton();
extern void ReleaseCachedPhysSkeleton(r3dPhysSkeleton* skel);

/////////////////////////////////////////////////////////////////////////
namespace
{
	struct DeerMeshesDeferredRenderable: public MeshDeferredRenderable
	{
		DeerMeshesDeferredRenderable()
		: Parent(0)
		{
		}

		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw(Renderable* RThis, const r3dCamera& Cam)
		{
			R3DPROFILE_FUNCTION("DeerMeshesDeferredRenderable");

			DeerMeshesDeferredRenderable* This = static_cast<DeerMeshesDeferredRenderable*>(RThis);

			r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

			This->Parent->OnPreRender();

			MeshDeferredRenderable::Draw(RThis, Cam);
		}

		obj_DeerDummy *Parent;
	};

//////////////////////////////////////////////////////////////////////////

	struct DeerMeshesShadowRenderable: public MeshShadowRenderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			R3DPROFILE_FUNCTION("DeerMeshesShadowRenderable");
			DeerMeshesShadowRenderable* This = static_cast< DeerMeshesShadowRenderable* >( RThis );

			r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

			This->Parent->OnPreRender();

			This->SubDrawFunc( RThis, Cam );
		}

		obj_DeerDummy *Parent;
	};
} // unnamed namespace

obj_DeerDummy::obj_DeerDummy()
{
	g_deerInstances++;
	if(g_deerBindSkeleton == NULL) 
	{
		g_deerBindSkeleton = game_new r3dSkeleton();
		g_deerBindSkeleton->LoadBinary("Data\\ObjectsDepot\\Characters\\Char_Deer_01.skl");
		
		g_deerAnimPool = game_new r3dAnimPool();
	}
	
	m_bEnablePhysics = false;
	
	m_isSerializable = true;

	variant = u_random(DEER_VARIANTS_COUNT - 1);
	mesh = NULL;
	
	physSkeleton = NULL;

	sAnimSelected[0] = 0;
}

obj_DeerDummy::~obj_DeerDummy()
{
	if(--g_deerInstances == 0)
	{
		SAFE_DELETE(g_deerAnimPool);
		SAFE_DELETE(g_deerBindSkeleton);
	}
}

BOOL obj_DeerDummy::OnCreate()
{
	parent::OnCreate();

	anim_.Init(g_deerBindSkeleton, g_deerAnimPool, NULL, (DWORD)this);

	ReloadMesh();

	AnimSpeed		= 1.0f;
	WalkSpeed		= 2.0f;
	Walking			= 0;
	ApplyWalkSpeed	= 0;
	WalkIdx			= -1;
	SpawnPos		= GetPosition();
	
	r3dBoundBox bbox;
	bbox.InitForExpansion();
	bbox.ExpandTo(mesh->localBBox);
	SetBBoxLocal(bbox);

	if(sAnimSelected[0] == 0)
		r3dscpy(sAnimSelected, random(2) == 0 ? "Deer_Idle.anm" : "Deer_Idle_Eating.anm");
	fAnimListOffset = 0;
	SwitchToSelectedAnim();

	anim_.Update(0.001f, r3dPoint3D(0,0,0), GetTransformMatrix());

	//physSkeleton = AquireCacheSkeleton(1);
	//physSkeleton->linkParent(anim_.GetCurrentSkeleton(), GetTransformMatrix(), this, PHYSCOLL_NETWORKPLAYER) ;
	//physSkeleton->SwitchToRagdoll(false);

	return TRUE;
}

BOOL obj_DeerDummy::Update()
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
BOOL obj_DeerDummy::OnDestroy()
{
	parent::OnDestroy();

	r3dGOBReleaseMesh(mesh);

	if(physSkeleton)
		ReleaseCachedPhysSkeleton(physSkeleton);

	return TRUE;
}

void obj_DeerDummy::OnPreRender()
{
	anim_.GetCurrentSkeleton()->SetShaderConstants();
}

void obj_DeerDummy::AppendRenderables(RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam)
{
	if (!mesh)
		return;

	uint32_t prevCount = render_arrays[ rsFillGBuffer ].Count();
	mesh->AppendRenderablesDeferred( render_arrays[ rsFillGBuffer ], r3dColor::white, GetSelfIllumMultiplier() );

	for( uint32_t i = prevCount, e = render_arrays[ rsFillGBuffer ].Count(); i < e; i ++ )
	{
		DeerMeshesDeferredRenderable& rend = static_cast<DeerMeshesDeferredRenderable&>( render_arrays[ rsFillGBuffer ][ i ] ) ;

		rend.Init();
		rend.Parent = this;
	}
}

void obj_DeerDummy::AppendShadowRenderables(RenderArray &rarr, int sliceIndex, const r3dCamera& cam)
{
	if (!mesh)
		return;

	float distSq = (gCam - GetPosition()).LengthSq();
	float dist = sqrtf( distSq );
	UINT32 idist = (UINT32) R3D_MIN( dist * 64.f, (float)0x3fffffff );

	uint32_t prevCount = rarr.Count();

	mesh->AppendShadowRenderables( rarr );

	for( uint32_t i = prevCount, e = rarr.Count(); i < e; i ++ )
	{
		DeerMeshesShadowRenderable& rend = static_cast<DeerMeshesShadowRenderable&>( rarr[ i ] );

		rend.Init() ;
		rend.SortValue |= idist;
		rend.Parent = this ;
	}
}

int obj_DeerDummy::AddAnimation(const char* anim)
{
	char buf[MAX_PATH];
	sprintf(buf, "Data\\Animations5\\%s.anm", anim);
	int aid = g_deerAnimPool->Add(anim, buf);
	
	return aid;
}	

void obj_DeerDummy::SwitchToSelectedAnim()
{
	char buf[MAX_PATH];
	sprintf(buf, "Data\\Animations5\\%s", sAnimSelected);
	int aid = g_deerAnimPool->Add(sAnimSelected, buf);
	anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherNow | ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
	
	anim_.AnimTracks[0].fSpeed = AnimSpeed * ((Walking && ApplyWalkSpeed) ? WalkSpeed : 1.0f);
}

void obj_DeerDummy::ReloadMesh()
{
	switch( variant )
	{
	default:
	case DEERVNT_Buck:
		mesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Characters\\Char_Deer_01.sco");
		break;
	case DEERVNT_Doe:
		mesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Characters\\Char_Deer_02_Doe.sco");
		break;
	}
}

#ifndef FINAL_BUILD
float obj_DeerDummy::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	//starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );
	if( ! IsParentOrEqual( &ClassData, startClass ) )	
		return starty - scry ;

	int idx = variant;
	starty += imgui_Value_SliderI(scrx, starty, "Variant", &variant, 0, 1, "%d");
	if(idx != variant) 
	{
		ReloadMesh();
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
		r3dSTLString sDirFind = "Data\\Animations5\\Deer_*.anm";
		if(imgui_FileList(scrx, starty, 360, flh, sDirFind.c_str(), sAnimSelected, &fAnimListOffset, true))
		{
			SwitchToSelectedAnim();
		}
		starty += flh;
	}

	return starty - scry ;
}
#endif

void obj_DeerDummy::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node deerdummy = node.child("deerdummy");
	variant = deerdummy.attribute("variant").as_int();
	r3dscpy(sAnimSelected, deerdummy.attribute("anim").value());
}

void obj_DeerDummy::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node deerdummy = node.append_child();
	deerdummy.set_name("deerdummy");
	deerdummy.append_attribute("variant") = variant;
	deerdummy.append_attribute("anim") = sAnimSelected;
}

void obj_DeerDummy::DoHit()
{
	//if(physSkeleton->IsRagdollMode())
	//	return;

	//static const char* anims[] = {
	//	"Zombie_Staggered_01_B.anm",
	//	"Zombie_Staggered_01_L.anm",
	//	"Zombie_Staggered_01_R.anm",
	//	"Zombie_Staggered_Small_01_B.anm",
	//	"Zombie_Staggered_Small_01_F.anm"
	//	};

	//const char* hitAnim = anims[u_random(5)];
	//
	//char buf[MAX_PATH];
	//sprintf(buf, "Data\\Animations5\\%s", hitAnim);
	//int aid = g_deerAnimPool->Add(hitAnim, buf);
	//anim_.StartAnimation(aid, 0, 0.0f, 1.0f, 0.1f);
}
