//=========================================================================
//	Module: obj_FakeGlass.cpp
//	Copyright (C) CoDeXPo 2015.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "obj_FakeGlass.h"
#include "GameCommon.h"
#include "../../GameEngine/gameobjects/obj_Vehicle.h"
#include "../ai/AI_Player.H"

//#ifndef FINAL_BUILD
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_FakeGlass, "obj_FakeGlass", "Object");
AUTOREGISTER_CLASS(obj_FakeGlass);

extern bool g_bEditMode;

//////////////////////////////////////////////////////////////////////////

struct FakeGlassRenderable : MeshDeferredRenderable
{
	void Init( int a_transparent )
	{
		DrawFunc = Draw;
		Transparent = a_transparent ;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		FakeGlassRenderable* This = static_cast< FakeGlassRenderable* >( RThis );
		r3dApplyPreparedMeshVSConsts( This->Parent->m_FakeGlassMeshVSConsts );
		int wasTransparent = 0 ;

		float oldTranspMultiplier = 1.0f ;

		bool hasMatForTrasnparency = This && This->Transparent && This->Mesh && This->Mesh->MatChunks[This->BatchIdx].Mat ;

		if ( hasMatForTrasnparency )
		{
			// because previous material may be the same
			// and thus it will filter our transparency hacks that follow
			r3dMaterial::ResetMaterialFilter() ;

			r3dMaterial* mat = This->Mesh->MatChunks[This->BatchIdx].Mat ;

			wasTransparent = mat->Flags & R3D_MAT_TRANSPARENT ;
			oldTranspMultiplier = mat->TransparencyMultiplier ;

			if( !wasTransparent )
				mat->TransparencyMultiplier = 0.75f ;

			mat->Flags |= R3D_MAT_TRANSPARENT ;

			SetLightsIfTransparent( mat, This->Parent->GetBBoxWorld() ) ;
		}

		if(This->Mesh->IsSkeletal())
			r3dSkeleton::SetDefaultShaderConstants();

		MeshDeferredRenderable::Draw( RThis, Cam );

		if ( hasMatForTrasnparency )
		{
			r3dMaterial* mat = This->Mesh->MatChunks[This->BatchIdx].Mat ;

			int& flags = mat->Flags ;
			flags &= ~R3D_MAT_TRANSPARENT ;
			flags |= wasTransparent ;

			mat->TransparencyMultiplier = oldTranspMultiplier ;
		}
	}

	obj_FakeGlass* Parent ;
	int Transparent ;
};

//////////////////////////////////////////////////////////////////////////


obj_FakeGlass::obj_FakeGlass()
{
	m_FakeGlassID = 0;
	m_FakeGlass = NULL;
	m_changeglass = false;
}

obj_FakeGlass::~obj_FakeGlass()
{
}

BOOL obj_FakeGlass::Update()
{
	return parent::Update();
}

BOOL obj_FakeGlass::OnCreate()
{

	if( g_bEditMode )
	{
		return parent::OnCreate();
	}

	D3DXMatrixIdentity(&DrawRotMatrix);
	r3dscpy(MeshFilenameGlass, FileName.c_str());

	return parent::OnCreate();
}

void obj_FakeGlass::SetFakeGlassObject(GameObject* obj)
{
	if (!obj)
		return;

	if( g_bEditMode )
	{
		return;
	}

	//if (!r3dGOBAddMesh(MeshFilenameGlass, true, false, true, true ))
	if (!r3dMesh::CanLoad(MeshFilenameGlass))
	{
		return;
	}

	SetPosition(obj->GetPosition());
	SetRotationVector(obj->GetRotationVector());
	m_FakeGlassRotation = obj->GetRotationVector();
	m_FakeGlassPosition = obj->GetPosition();
	m_FakeGlassID = 1;
	if(obj->isObjType(OBJTYPE_Vehicle))
	{
		obj_Vehicle* vehicle = (obj_Vehicle*)obj;
		if (vehicle)
		{
			if (vehicle->isExplosionLoaded == true)
			{
				m_FakeGlassID = 0;
			}
			else {
				if (m_changeglass != vehicle->enableInteriorCam)
				{
					m_changeglass = vehicle->enableInteriorCam;
					SetInteriorGlass(m_changeglass);
				}
			}
		}
	}
	/*else {

	}*/

	//r3dOutToLog("######## position X:%f Y:%f Z:%f\n",m_FakeGlassPosition.x,m_FakeGlassPosition.y,m_FakeGlassPosition.z);
}

void obj_FakeGlass::SetInteriorGlass(bool enable)
{
	char interior[512]="";
	strcpy(interior,MeshFilenameGlass);
	int len = strlen(interior);
	r3dscpy(&interior[len-4], "_int.sco");

	//if (!r3dGOBAddMesh(interior, true, false, true, true ))
	if (!r3dMesh::CanLoad(interior))
	{
		return;
	}

	if (enable)
	{
		m_FakeGlass = r3dGOBAddMesh(interior, true, false, true, true );
	}
	else {
		m_FakeGlass = r3dGOBAddMesh(MeshFilenameGlass, true, false, true, true );
	}
}

BOOL obj_FakeGlass::OnDestroy()
{
	m_FakeGlassID = 0;
	m_FakeGlass = NULL;
	return TRUE;
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_FakeGlass::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	if (m_FakeGlassID == 0)
		return;

	if(m_FakeGlass == NULL)
	{
		m_FakeGlass = r3dGOBAddMesh(MeshFilenameGlass, true, false, true, true );
	}
	if(m_FakeGlass && m_FakeGlass->IsDrawable())
	{
		r3dPoint3D *scale ;
		r3dPoint2D *texcScale ;
		scale = m_FakeGlass->IsSkeletal() ? 0 : &m_FakeGlass->unpackScale;
		texcScale = &m_FakeGlass->texcUnpackScale ;

		D3DXMATRIX m1, m2, m3, world;

		D3DXMatrixTranslation(&m1, m_FakeGlassPosition.x, m_FakeGlassPosition.y, m_FakeGlassPosition.z);
		D3DXMatrixRotationY(&m2, R3D_PI);
		D3DXMatrixMultiply(&m3, &DrawRotMatrix, &m2);

		D3DXMATRIX RotateMatrix,rt2;
		D3DXMatrixRotationYawPitchRoll(&RotateMatrix,R3D_DEG2RAD(m_FakeGlassRotation.x),R3D_DEG2RAD(m_FakeGlassRotation.y),R3D_DEG2RAD(m_FakeGlassRotation.z));
		D3DXMatrixMultiply(&m3, &m3, &RotateMatrix);

		D3DXMatrixMultiply(&world, &m3, &m1);
		r3dPrepareMeshShaderConsts(m_FakeGlassMeshVSConsts, world, scale, texcScale, r3dRenderer->ViewMatrix, r3dRenderer->ViewProjMatrix, NULL);

		{
			float dist = (Cam - GetPosition()).Length();

			uint32_t prevTranspCount = render_arrays[rsDrawTransparents].Count();

			m_FakeGlass->AppendTransparentRenderablesColors( render_arrays[rsDrawTransparents], dist, 1 );
			
			for( uint32_t i = prevTranspCount, e = render_arrays[rsDrawTransparents].Count(); i < e; i ++ )
			{
				FakeGlassRenderable& rend = static_cast<FakeGlassRenderable&>( render_arrays[rsDrawTransparents][ i ] ) ;
				rend.Init( 1 ) ;
				rend.Parent = this;
			}

		}
	}
}
//#endif