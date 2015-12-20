#include "r3dPCH.h"
#include "r3d.h"
#include "GameCommon.h"

#include "BulletShellManager.h"

BulletShell::BulletShell()
{
	m_Type = BST_Pistol;
	m_physObj = 0;
	m_Pos.Assign(0,0,0);
	D3DXMatrixIdentity(&m_RenderMatrix);
}

BulletShell::~BulletShell()
{
	r3d_assert(m_physObj==0);
}

const float BULLET_LIFETIME = 15.0f;

void BulletShell::Init(const r3dPoint3D& pos, const r3dPoint3D& vel,  const PhysicsObjectConfig& config, r3dMesh* mesh, BulletShellType type, const D3DXMATRIX& rotation)
{
	Destroy();

	m_physObj = BasePhysicsObject::CreateDynamicObject(config, this, &pos, &mesh->localBBox.Size, mesh, &rotation);
	r3d_assert(m_physObj!=0);

	m_physObj->addImpulse(vel);
	
	D3DXMatrixTranslation(&m_RenderMatrix, pos.x, pos.y, pos.z);

	m_Type = type;
	m_StartTime = r3dGetTime();
	m_PlayedBrassSound = false;
}

void BulletShell::Destroy()
{
	SAFE_DELETE(m_physObj);
}

void BulletShell::Update()
{
	if(m_physObj)
	{
		if(!m_physObj->IsSleeping())
		{
			r3dVector physPos = m_physObj->GetPosition();
			// sometimes physics returns QNAN position, not sure why
			if(r3d_float_isFinite(physPos.x) && r3d_float_isFinite(physPos.y) && r3d_float_isFinite(physPos.z))
			{
				m_Pos = physPos;
				D3DXMatrixTranslation(&m_RenderMatrix, m_Pos.x, m_Pos.y, m_Pos.z);

				D3DXMATRIX mat = m_physObj->GetRotation();
				mat.m[3][0] = 0.0f; mat.m[3][1] = 0.0f; mat.m[3][2] = 0.0f; mat.m[3][3] = 1.0f;
				m_RenderMatrix = mat * m_RenderMatrix;
 			}
		}
		if((r3dGetTime() - m_StartTime) > BULLET_LIFETIME)
			Destroy();
	}
}

void BulletShell::OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace)
{
	static int ejectBrassSoundID = -1;
	if(ejectBrassSoundID==-1)
		ejectBrassSoundID = SoundSys.GetEventIDByPath("Sounds/Misc/EjectBrass");
	if(!m_PlayedBrassSound)
	{
		SoundSys.PlayAndForget(ejectBrassSoundID, m_Pos);
		m_PlayedBrassSound = true;
	}
}

//////////////////////////////////////////////////////////////////////////
BulletShellMngr::BulletShellMngr()
{
	m_numActiveShells = 0;

	m_shellMeshes[0] = r3dGOBAddMesh("Data/ObjectsDepot/Weapons/Shell_Pistol.sco", true, false, false, true); r3d_assert(m_shellMeshes[0]);
	m_shellMeshes[1] = r3dGOBAddMesh("Data/ObjectsDepot/Weapons/Shell_Rifle.sco", true, false, false, true); r3d_assert(m_shellMeshes[1]);
	m_shellMeshes[2] = r3dGOBAddMesh("Data/ObjectsDepot/Weapons/Shell_Shotgun.sco", true, false, false, true); r3d_assert(m_shellMeshes[2]);

	GameObject::LoadPhysicsConfig(m_shellMeshes[0]->FileName.c_str(), m_shellPhysConfigs[0]); r3d_assert(m_shellPhysConfigs[0].ready);
	GameObject::LoadPhysicsConfig(m_shellMeshes[1]->FileName.c_str(), m_shellPhysConfigs[1]); r3d_assert(m_shellPhysConfigs[1].ready);
	GameObject::LoadPhysicsConfig(m_shellMeshes[2]->FileName.c_str(), m_shellPhysConfigs[2]); r3d_assert(m_shellPhysConfigs[2].ready);
	m_shellPhysConfigs[0].group = PHYSCOLL_TINY_GEOMETRY;
	m_shellPhysConfigs[1].group = PHYSCOLL_TINY_GEOMETRY;
	m_shellPhysConfigs[2].group = PHYSCOLL_TINY_GEOMETRY;

	m_shellPhysConfigs[0].isFastMoving = m_shellPhysConfigs[1].isFastMoving = m_shellPhysConfigs[2].isFastMoving = true;
};

BulletShellMngr::~BulletShellMngr()
{
	for(int i=0; i<MAX_SHELLS; ++i)
		m_Shells[i].Destroy();
}

void BulletShellMngr::AddShell(const r3dPoint3D& pos, const r3dPoint3D& vel, const D3DXMATRIX& rotation, BulletShellType shellType)
{
	// PT: sometimes animation has QNAN in it, not sure where is it coming from. Seems like in some cases quaternion becomes fucked up and not able to transform it into a matrix. As all other data in skeleton is fine
	if(!(r3d_float_isFinite(vel.x) && r3d_float_isFinite(vel.y) && r3d_float_isFinite(vel.z)))
		return;

	m_Shells[m_numActiveShells].Init(pos, vel, m_shellPhysConfigs[(int)shellType], m_shellMeshes[(int)shellType], shellType, rotation);
	++m_numActiveShells;
	if(m_numActiveShells == MAX_SHELLS)
		m_numActiveShells = 0;
}

void BulletShellMngr::Update()
{
	for(int i=0; i<MAX_SHELLS; ++i)
		if(m_Shells[i].Active())
			m_Shells[i].Update();
}

struct BulletShellDeferredRenderable : MeshDeferredRenderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("BulletShellDeferredRenderable");
		BulletShellDeferredRenderable* This = static_cast< BulletShellDeferredRenderable* >( RThis );

		This->Mesh->SetShaderConsts( This->Parent->getDrawMatrix() );
		MeshDeferredRenderable::Draw( RThis, Cam );
	}

	BulletShell* Parent;
};

void BulletShellMngr::AppendRenderables(RenderArray(&render_arrays)[rsCount], const r3dCamera& Cam)
{
	R3DPROFILE_FUNCTION("BulletShellMngr::AppendRenderables");
	
	COMPILE_ASSERT( sizeof(BulletShellDeferredRenderable) <= MAX_RENDERABLE_SIZE );
	for(int i=0; i<MAX_SHELLS; ++i)
	{
		if(!m_Shells[i].Active())
			continue;

		float distSq = (Cam - m_Shells[i].getPosition()).LengthSq();
		float dist = sqrtf( distSq );

		int idist = R3D_MIN( (int)dist, 0xffff );

		uint32_t prevCount = render_arrays[ rsFillGBuffer ].Count();
		m_shellMeshes[(int)m_Shells[i].m_Type]->AppendRenderablesDeferred( render_arrays[ rsFillGBuffer ], r3dColor::white, 1.0f);
		for( uint32_t j = prevCount, e = render_arrays[ rsFillGBuffer ].Count(); j < e; j++ )
		{
			BulletShellDeferredRenderable& rend = static_cast<BulletShellDeferredRenderable&>( render_arrays[ rsFillGBuffer ][j] ) ;
			rend.SortValue |= idist ;
			rend.Init() ;
			rend.Parent = &m_Shells[i];
		}
	}
}