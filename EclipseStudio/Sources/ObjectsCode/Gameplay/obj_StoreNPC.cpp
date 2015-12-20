#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_StoreNPC.h"

IMPLEMENT_CLASS(obj_BaseNPC, "obj_BaseNPC", "Object");
AUTOREGISTER_CLASS(obj_BaseNPC);
IMPLEMENT_CLASS(obj_StoreNPC, "obj_StoreNPC", "Object");
AUTOREGISTER_CLASS(obj_StoreNPC);
IMPLEMENT_CLASS(obj_VaultNPC, "obj_VaultNPC", "Object");
AUTOREGISTER_CLASS(obj_VaultNPC);

obj_BaseNPC::obj_BaseNPC()
{
	DisablePhysX = true;
	m_BindSkeleton = NULL;
	m_NPCMeshName = NULL;
	m_NPCAnimName = NULL;
}

obj_BaseNPC::~obj_BaseNPC()
{
	SAFE_DELETE(m_BindSkeleton);
}

BOOL obj_BaseNPC::Load(const char *fname)
{
	r3d_assert(m_NPCMeshName);

	char cpMeshName[MAX_PATH];
	sprintf(cpMeshName, "Data\\ObjectsDepot\\Characters\\%s.sco", m_NPCMeshName);

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	// load skeleton
	char skelname[MAX_PATH];
	r3dscpy(skelname, cpMeshName);
	r3dscpy(&skelname[strlen(skelname)-3], "skl");

	m_BindSkeleton = game_new r3dSkeleton();
	m_BindSkeleton->LoadBinary(skelname);

	m_Animation.Init(m_BindSkeleton, &m_AnimPool);
	m_Animation.Update(0.0f, r3dPoint3D(0, 0, 0), mTransform);

	return TRUE;
}


BOOL obj_BaseNPC::OnCreate()
{
	if(!PhysicsConfig.ready)
		ReadPhysicsConfig();

	m_spawnPos = GetPosition();

	if(parent::OnCreate())
	{
		// load anim
		r3d_assert(m_NPCAnimName);
		char animNamePath[MAX_PATH];
		sprintf(animNamePath, "Data\\Animations5\\%s.anm", m_NPCAnimName);
		int aid = m_AnimPool.Add(m_NPCAnimName, animNamePath);
		r3d_assert(aid!=-1);
		m_Animation.StartAnimation(aid, ANIMFLAG_Looped, 0.0f, 0.0f, 0.0f);

		return TRUE;
	}
	return FALSE;
}

BOOL obj_BaseNPC::Update()
{
	m_Animation.Update(r3dGetAveragedFrameTime(), r3dPoint3D(0, 0, 0), mTransform);
	return parent::Update();
}

void obj_BaseNPC::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
}

void obj_BaseNPC::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
}

void obj_BaseNPC::DrawAnimated(const r3dCamera& Cam, bool shadow_pass)
{
	r3d_assert(m_Animation.pSkeleton);

	// recalc animation if it dirty
	m_Animation.Recalc();

	int oldVsId = r3dRenderer->GetCurrentVertexShaderIdx();

	m_Animation.pSkeleton->SetShaderConstants();

	r3dMesh* mesh = MeshLOD[0];
	r3d_assert( mesh->IsSkeletal() );

	D3DXMATRIX world;
	r3dPoint3D pos = GetPosition();
	D3DXMatrixTranslation(&world, pos.x, pos.y, pos.z );
	r3dMeshSetShaderConsts( world, NULL );

	if(!shadow_pass)
	{
		mesh->DrawMeshDeferred( m_ObjectColor, 0 );
	}
	else
	{
		mesh->DrawMeshShadows();
	}

	r3dRenderer->SetVertexShader(oldVsId);
}

struct StoreNPCAniDeferredRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		StoreNPCAniDeferredRenderable* This = static_cast<StoreNPCAniDeferredRenderable*>( RThis );
		This->Parent->DrawAnimated(Cam, false);
	}

	obj_BaseNPC* Parent;
};

struct StoreNPCAniShadowRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		StoreNPCAniShadowRenderable* This = static_cast<StoreNPCAniShadowRenderable*>( RThis );
		This->Parent->DrawAnimated(Cam, true);
	}

	obj_BaseNPC* Parent;
};

#define	RENDERABLE_STORENPC_SORT_VALUE (31*RENDERABLE_USER_SORT_VALUE)
void obj_BaseNPC::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam )
{
	if( !gDisableDynamicObjectShadows && sliceIndex != r_active_shadow_slices->GetInt() - 1 )
	{
		StoreNPCAniShadowRenderable rend;
		rend.Init();
		rend.Parent	= this;
		rend.SortValue	= RENDERABLE_STORENPC_SORT_VALUE;

		rarr.PushBack( rend );
	}
}

void obj_BaseNPC::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	{
		StoreNPCAniDeferredRenderable rend;
		rend.Init();
		rend.Parent	= this;
		rend.SortValue	= RENDERABLE_STORENPC_SORT_VALUE;

		render_arrays[ rsFillGBuffer ].PushBack( rend );
	}
}

obj_StoreNPC::obj_StoreNPC() : obj_BaseNPC()
{
	m_ActionUI_Title = gLangMngr.getString("ActionUI_StoreTitle");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToAccessStore");
	m_NPCMeshName = "Civ_Man_01";
	m_NPCAnimName = "Civ_Idle_01";
}

obj_VaultNPC::obj_VaultNPC() : obj_BaseNPC()
{
	m_ActionUI_Title = gLangMngr.getString("ActionUI_VaultTitle");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToAccessVault");
	m_NPCMeshName = "Civ_Man_02";
	m_NPCAnimName = "Civ_Idle_02";
}