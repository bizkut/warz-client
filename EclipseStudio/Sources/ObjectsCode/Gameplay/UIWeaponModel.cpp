#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "UIWeaponModel.h"

#include "../weapons/WeaponArmory.h"
#include "../weapons/Weapon.h"

#if 0
IMPLEMENT_CLASS(UIWeaponModel, "UIWeaponModel", "Object");
AUTOREGISTER_CLASS(UIWeaponModel);

UIWeaponModel::UIWeaponModel() :
 m_CurrentWeapon(NULL)
,enableRendering(false)
{
	rotationTimer = 0;
}

UIWeaponModel::~UIWeaponModel()
{
}

BOOL UIWeaponModel::Load(const char *fname)
{
	if(!GameObject::Load(fname)) return FALSE;

	return TRUE;
}

BOOL UIWeaponModel::OnCreate()
{
	GameObject::OnCreate();

	DrawOrder	= OBJ_DRAWORDER_LAST;
	setSkipOcclusionCheck(true);
	ObjFlags	|=	OBJFLAG_DisableShadows ;

	r3dBoundBox bboxLocal ;

	bboxLocal.Size = r3dPoint3D(2,2,2);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;

	SetBBoxLocal( bboxLocal ) ;
	UpdateTransform();

	return 1;
}

BOOL UIWeaponModel::Update()
{
	rotationTimer += r3dGetFrameTime() * 0.5f;

	UpdateTransform();
	return GameObject::Update();
}

void UIWeaponModel::setWeaponModel(uint32_t itemID, GameObject* owner)
{
	SAFE_DELETE(m_CurrentWeapon);

	m_CurrentWeapon = g_pWeaponArmory->createWeapon(itemID, owner, true, true);
	m_CurrentWeapon->getModel(false, true);
	m_CurrentWeapon->checkForSkeleton();
}

struct UIWeaponModelRenderable : Renderable
{
	void Init() { DrawFunc = Draw; }
	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		UIWeaponModelRenderable *This = static_cast<UIWeaponModelRenderable*>( RThis );
		if(This->Parent->m_CurrentWeapon)
		{
			Weapon* curWpn = This->Parent->m_CurrentWeapon;
			D3DXMATRIX world; 
			D3DXMatrixRotationYawPitchRoll(&world, R3D_DEG2RAD(15) * sinf(This->Parent->rotationTimer), 0, 0);
			world._42 = 1.25f;
			world._41 = -0.5f;

			D3DXMATRIX scale; D3DXMatrixScaling(&scale, 3, 3, 3);
			world = scale * world;
			
			bool skinned = false;
			r3dSkeleton* wpnSkel = NULL;
			{
				r3dMesh* msh = curWpn->getModel(false, true);
				r3d_assert(msh->IsSkeletal() && curWpn->getConfig()->getSkeleton());

				curWpn->getAnimation()->Recalc();
				curWpn->getAnimation()->GetCurrentSkeleton()->SetShaderConstants();
				wpnSkel = curWpn->getAnimation()->GetCurrentSkeleton();

				DrawSlotMesh(msh, world, true);

				{
					for(int i=0; i<WPN_ATTM_MAX; ++i)
					{
						msh = curWpn->getWeaponAttachmentMesh((WeaponAttachmentTypeEnum)i, false);
						if(msh)
						{
							D3DXMATRIX attmWorld;
							wpnSkel->GetBoneWorldTM(WeaponAttachmentBoneNames[i], &attmWorld, world);
							DrawSlotMesh(msh, attmWorld, false);
						}
					}
				}			
			}
		}
	}
	static void DrawSlotMesh(r3dMesh* mesh, const D3DXMATRIX& world, bool skin)
	{
		if(skin)
		{
			r3dMeshSetVSConsts(world, NULL, NULL);
		}
		else
		{
			mesh->SetVSConsts(world);

			// NOTE : needed for transparent camo only..
			// float4   WorldScale  		: register(c24);
			D3DXVECTOR4 scale(mesh->unpackScale.x, mesh->unpackScale.y, mesh->unpackScale.z, 0.f) ;
			D3D_V(r3dRenderer->pd3ddev->SetVertexShaderConstantF(24, (float*)&scale, 1)) ;
		}

		{
			r3dBoundBox worldBBox = mesh->localBBox;
			worldBBox.Transform(reinterpret_cast<const r3dMatrix *>(&world));
			// Vertex lights for forward transparent renderer.
			for (int i = 0; i < mesh->NumMatChunks; i++)
			{
				SetLightsIfTransparent(mesh->MatChunks[i].Mat, worldBBox);
			}
			mesh->DrawMeshDeferred(r3dColor::white, 0);
		}
	}

	UIWeaponModel* Parent;
};

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void UIWeaponModel::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	if(!enableRendering)
		return;

	UIWeaponModelRenderable rend;
	rend.Init();
	rend.SortValue = RENDERABLE_OBJ_USER_SORT_VALUE;
	rend.Parent = this;
	render_arrays[ rsFillGBuffer ].PushBack( rend );

}

#endif