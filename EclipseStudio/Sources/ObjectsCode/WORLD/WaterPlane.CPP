#include "r3dpch.h"
#include "r3d.h"

#ifndef WO_SERVER
#include "UI/UIimEdit.h"
#endif // WO_SERVER

#include "GameCommon.h"
#include "GameObjects\gameobj.h"
#ifndef WO_SERVER
#include "GameObjects\objmanag.h"

#include "EnvmapProbes.h"
#endif // WO_SERVER
#include "GameLevel.h"
#include "WaterPlane.h"
#ifndef WO_SERVER
#include "../GameEngine/TrueNature2/Terrain3.h"
#include "../GameEngine/TrueNature2/Terrain2.h"

extern r3dITerrain* Terrain;

//////////////////////////////////////////////////////////////////////////

int GetWaterDownScale()
{
	return r_environment_quality->GetInt() == 1 ? 2 : 1 ;
}

void waterReloadTexture( r3dTexture*& tex )
{
	if( !tex )
		return;

	char FileName[ 1024 ];
	r3dscpy( FileName, tex->getFileLoc().FileName );
	FileName[ 1023 ] = 0;

	tex->Unload();
	tex->Load( FileName, D3DFMT_FROM_FILE, GetWaterDownScale() );
}

LPDIRECT3DVERTEXDECLARATION9 R3D_WATER_PLANE_VERTEX::pDecl = 0;
D3DVERTEXELEMENT9 R3D_WATER_PLANE_VERTEX::VBDecl[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 R3D_WATER_PLANE_VERTEX::getDecl()
{
	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}

//////////////////////////////////////////////////////////////////////////

static r3dVector WaterPlaneComputeNormal ( r3dTL::T2DArray<r3dVector> const &dPoints, r3dTL::T2DArray<uint8_t> const &dGrid, uint32_t iI, uint32_t iJ )
{
	uint32_t iW = dGrid.Width ();
	uint32_t iH = dGrid.Height ();

	r3dVector vNormal(0,0,0);
	int iCnt = 0;

	if(iI<iW&&iJ<iH&&dGrid[iJ][iI])
	{
		r3dVector const & v0 = dPoints[iJ][iI];
		r3dVector const & v1 = dPoints[iJ+1][iI];
		r3dVector const & v2 = dPoints[iJ+1][iI+1];
		r3dVector const & v3 = dPoints[iJ][iI+1];

		r3dVector vNormal0 = -(v0-v1).Cross(v2-v1).NormalizeTo();
		r3dVector vNormal1 = -(v2-v3).Cross(v0-v3).NormalizeTo();
		vNormal+=vNormal0;
		vNormal+=vNormal1;
		iCnt+=2;
	}

	if(iI>0&&iJ>0&&iI-1<iW&&iJ-1<iH&&dGrid[iJ-1][iI-1])
	{
		r3dVector const & v0 = dPoints[iJ-1][iI-1];
		r3dVector const & v1 = dPoints[iJ][iI-1];
		r3dVector const & v2 = dPoints[iJ][iI];
		r3dVector const & v3 = dPoints[iJ-1][iI];

		r3dVector vNormal0 = -(v0-v1).Cross(v2-v1).NormalizeTo();
		r3dVector vNormal1 = -(v2-v3).Cross(v0-v3).NormalizeTo();
		vNormal+=vNormal0;
		vNormal+=vNormal1;
		iCnt+=2;
	}

	if(iI>0&&iI-1<iW&&iJ<iH&&dGrid[iJ][iI-1])
	{
		r3dVector const & v0 = dPoints[iJ][iI-1];
		r3dVector const & v2 = dPoints[iJ+1][iI];
		r3dVector const & v3 = dPoints[iJ][iI];

		r3dVector vNormal1 = -(v2-v3).Cross(v0-v3).NormalizeTo();
		vNormal+=vNormal1;
		iCnt+=1;
	}

	if(iJ>0&&iI<iW&&iJ-1<iH&&dGrid[iJ-1][iI])
	{
		r3dVector const & v0 = dPoints[iJ-1][iI];
		r3dVector const & v1 = dPoints[iJ][iI];
		r3dVector const & v2 = dPoints[iJ][iI+1];

		r3dVector vNormal0 = -(v0-v1).Cross(v2-v1).NormalizeTo();
		vNormal+=vNormal0;
		iCnt+=1;
	}

	r3d_assert(iCnt>0);

	vNormal.Normalize ();

	return vNormal;
}
#endif // WO_SERVER
//////////////////////////////////////////////////////////////////////////

const char WATER_PLANES_DIR [] = "water_planes";
const WORD WATER_VERSION_ID = 4;
const WORD WATER_VERSION_BIN_ID = 5;

#ifndef WO_SERVER
extern r3dScreenBuffer*	DepthBuffer;
extern float terra_GetH(const r3dPoint3D &vPos);
#endif // WO_SERVER
//////////////////////////////////////////////////////////////////////////

//static
r3dTL::TArray<obj_WaterPlane*> obj_WaterPlane::m_dWaterPlanes;

//////////////////////////////////////////////////////////////////////////

obj_WaterPlane::Settings::Settings()
: WaterPlaneHeight( 10.0f )
, CellGridSize( 50.0f )
, PlaneXSize( 0.0f )
, PlaneZSize( 0.0f )
, PlaneCentreX( 0.0f )
, PlaneCentreZ( 0.0f )
, CoastSmoothLevels( 2 )
{

}

//------------------------------------------------------------------------

obj_WaterPlane::Info::Info()
: OffsetX( 0.0f )
, OffsetZ( 0.0f )
, CellCountX( 0 )
, CellCountZ( 0 )
{

}

//------------------------------------------------------------------------

IMPLEMENT_CLASS(obj_WaterPlane, "obj_WaterPlane", "Object");
AUTOREGISTER_CLASS(obj_WaterPlane);

obj_WaterPlane::obj_WaterPlane()
: m_fOldCellGridSize ( 0.0f )
, m_bResizeInfoExists ( false )
#ifndef WO_SERVER
, m_Plane ( NULL )
#endif // WO_SERVER
, m_fOldOffsetX( 0.f )
, m_fOldOffsetZ( 0.f )
{
	m_dOldGrid.Clear ();
	m_dWaterPlanes.PushBack ( this );
#ifndef WO_SERVER
	memset(WaterColor, 0, sizeof(WaterColor));
	FoamTexture = 0;
	SpecularTexture = 0;
	ColorTexture = 0;
	MaskTexture = 0;

	DrawDistanceSq = FLT_MAX;
#endif // WO_SERVER
}

obj_WaterPlane::~obj_WaterPlane()
{
	for ( uint32_t i = 0; i < m_dWaterPlanes.Count(); i++ )
	{
		if ( m_dWaterPlanes[i] == this )
		{
			m_dWaterPlanes.Erase ( i );
			break;
		}
	}
}




BOOL obj_WaterPlane::OnCreate()
{
	parent::OnCreate();
#ifndef WO_SERVER
	char Str[256];

	DrawOrder	= OBJ_DRAWORDER_LAST;

	setSkipOcclusionCheck(true);
	ObjFlags |= OBJFLAG_SkipCastRay | OBJFLAG_DisableShadows | OBJFLAG_ForceSleep; // todo: remove skipObstacle and add proper aabb!!!

	m_Plane = gfx_new r3dXPSObject<R3D_WATER_PLANE_VERTEX>;

	extern int bRenderRefraction;
	bRenderRefraction = 1;

	WaterBase::OnCreateBase();
	CreateWaterBuffers();

	if ( ! WaterColor[0] )
	{
		for(int i=0; i<25; ++i)
		{
			sprintf(Str,"Data\\Water\\waves_%.2d.dds",i);
			WaterColor[i] = r3dRenderer->LoadTexture( Str, D3DFMT_UNKNOWN, false, GetWaterDownScale() );
		}
		FoamTexture = r3dRenderer->LoadTexture("Data\\Water\\foam02.jpg");
		//SpecularTexture = r3dRenderer->LoadTexture("Data\\Water\\water_gloss.dds");
		ColorTexture = r3dRenderer->LoadTexture("Data\\Water\\LakeColor.dds", D3DFMT_UNKNOWN, false, GetWaterDownScale() );
		//MaskTexture = r3dRenderer->LoadTexture("Data\\Water\\LakeMask.dds");
	}
#endif // WO_SERVER

	UpdateWaterPlane();

	return true;
}

BOOL obj_WaterPlane::OnDestroy()
{
#ifndef WO_SERVER
	WaterBase::OnDestroyBase();
	for(int i=0; i<25; ++i)
		r3dRenderer->DeleteTexture(WaterColor[i]);

	r3dRenderer->DeleteTexture(FoamTexture);
	//r3dRenderer->DeleteTexture(SpecularTexture);
	r3dRenderer->DeleteTexture(ColorTexture);
	//r3dRenderer->DeleteTexture(MaskTexture);

	SAFE_DELETE(m_Plane);
#endif // WO_SERVER

	return parent::OnDestroy();
}

BOOL obj_WaterPlane::Update()
{
	// in case of changing follow-terrain or height

	r3dBoundBox bboxLocal = GetBBoxLocal() ;

	bboxLocal.Org.y = m_Settings.WaterPlaneHeight;

	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();
	return TRUE;
}

//------------------------------------------------------------------------
/*virtual*/
#ifndef WO_SERVER
struct WaterRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("WaterRenderable");
		WaterRenderable *This = static_cast<WaterRenderable*>( RThis );

		This->Parent->DoDraw( Cam );
	}

	obj_WaterPlane* Parent;
};

struct WaterDepthRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("WaterDepthRenderable");
		WaterDepthRenderable *This = static_cast<WaterDepthRenderable*>( RThis );

		This->Parent->DoDrawDepth( Cam );
	}

	obj_WaterPlane* Parent;
};

void
obj_WaterPlane::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
	if(!m_Plane||!m_Plane->IsValid())
		return;

#define RENDERABLE_WATER_SORT_VALUE (19*RENDERABLE_USER_SORT_VALUE)

	// composite
	{
		WaterRenderable rend;

		rend.Init();

		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_WATER_SORT_VALUE;

		render_arrays[ rsDrawComposite2 ].PushBack( rend );
	}

	// depth
	{
		WaterDepthRenderable rend;

		rend.Init();

		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_WATER_SORT_VALUE;

		render_arrays[ rsDrawDepthEffect ].PushBack( rend );
	}


}

extern float RIPPLES_AREA_SIZE;

bool obj_WaterPlane::WaterSplash(const r3dPoint3D& waterSplashPos, float height, float size, float amount, int texIdx)
{
	if(nSplashes==MaxSplashes)	return false;

	const float heightBlend = 0.5f;

	const r3dBoundBox& bboxLocal = GetBBoxLocal() ;

	if(waterSplashPos.X >= bboxLocal.Org.X && waterSplashPos.X <= bboxLocal.Org.X + bboxLocal.Size.X &&
		 waterSplashPos.Z >= bboxLocal.Org.Z && waterSplashPos.Z <= bboxLocal.Org.Z + bboxLocal.Size.Z
		 && m_Settings.WaterPlaneHeight > waterSplashPos.y && m_Settings.WaterPlaneHeight < waterSplashPos.y+height+heightBlend
		 )
	{
		splashes[nSplashes].params.x = (waterSplashPos.x - gCam.x) / RIPPLES_AREA_SIZE + 0.5f;
		splashes[nSplashes].params.y = (waterSplashPos.z - gCam.z) / RIPPLES_AREA_SIZE + 0.5f;
		splashes[nSplashes].params.z = size;
		splashes[nSplashes].texIdx = texIdx;

		if(m_Settings.WaterPlaneHeight > waterSplashPos.y+height)
			amount *= 1 - (m_Settings.WaterPlaneHeight - waterSplashPos.y-height) / heightBlend;

		splashes[nSplashes].params.w = amount;
		nSplashes++;
		return true;
	}
	return false;
}

//------------------------------------------------------------------------

void
obj_WaterPlane::DoDraw( const r3dCamera& Cam )
{
#ifndef WO_SERVER
	R3DPROFILE_FUNCTION("WaterPlane::DoDraw");

	RenderBegin(Cam, m_Settings.WaterPlaneHeight);

	m_Plane->Draw();

	RenderEnd();

#endif
}

//------------------------------------------------------------------------

void
obj_WaterPlane::DoDrawDepth( const r3dCamera& Cam )
{
	return;
}
#endif // WO_SERVER

void obj_WaterPlane::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node lakeNode = node.child("lake");

	pugi::xml_node ch_node = node.child("new_lake");
	if ( ! ch_node.empty() )
	{
#ifndef WO_SERVER
		const char * szName = ch_node.attribute("wave_tex").value();
		if ( * szName )
		{
			char Str[256];
			for(int i=0; i<25; ++i)
			{
				sprintf(Str,"%s%.2d.dds", szName, i);
				WaterColor[i] = r3dRenderer->LoadTexture(Str, D3DFMT_UNKNOWN, false, GetWaterDownScale() );
			}
		}
		FoamTexture = r3dRenderer->LoadTexture("Data\\Water\\foam02.jpg");
		//SpecularTexture = r3dRenderer->LoadTexture("Data\\Water\\water_gloss.dds");
		ColorTexture = r3dRenderer->LoadTexture("Data\\Water\\LakeColor.dds", D3DFMT_UNKNOWN, false, GetWaterDownScale() );
		//MaskTexture = r3dRenderer->LoadTexture("Data\\Water\\LakeMask.dds");
#endif // WO_SERVER

		if(!ch_node.attribute("waterplaneheight").empty())
			m_Settings.WaterPlaneHeight =  ch_node.attribute("waterplaneheight").as_float();
		if(!ch_node.attribute("cellgridsize").empty())
			m_Settings.CellGridSize =  ch_node.attribute("cellgridsize").as_float();
		if(!ch_node.attribute("coastsmoothlevels").empty())
			m_Settings.CoastSmoothLevels =  ch_node.attribute("coastsmoothlevels").as_int();
		if(!ch_node.attribute("total_x_size").empty())
			m_Settings.PlaneXSize =  ch_node.attribute("total_x_size").as_float();
		if(!ch_node.attribute("total_z_size").empty())
			m_Settings.PlaneZSize =  ch_node.attribute("total_z_size").as_float();
		if(!ch_node.attribute("center_x").empty())
			m_Settings.PlaneCentreX =  ch_node.attribute("center_x").as_float();
		if(!ch_node.attribute("center_z").empty())
			m_Settings.PlaneCentreZ =  ch_node.attribute("center_z").as_float();

		// calculate m_Info members like this
		SetSettings( m_Settings );

		WaterBase::ReadSerializedData(ch_node);

		LoadWaterPlaneGrid (Name.c_str());

		UpdateWaterPlane();
	}
}

#ifndef WO_SERVER
void obj_WaterPlane::WriteSerializedData(pugi::xml_node& node)
{
	ResetResizeInfo ();

	GameObject::WriteSerializedData(node);
	pugi::xml_node lakeNode = node.append_child();
	lakeNode.set_name("lake");

	pugi::xml_node ch_node = node.append_child();
	ch_node.set_name( "new_lake" );
	if ( WaterColor[0] )
	{
		char fn[256];
		memcpy(fn, WaterColor[0]->getFileLoc().FileName, sizeof(WaterColor[0]->getFileLoc().FileName));
		fn[ strlen(fn) - strlen("00.dds") ] = 0;
		ch_node.append_attribute("wave_tex") = fn;
	}


	ch_node.append_attribute("waterplaneheight") = m_Settings.WaterPlaneHeight;
	ch_node.append_attribute("cellgridsize") = m_Settings.CellGridSize;
	ch_node.append_attribute("coastsmoothlevels") = m_Settings.CoastSmoothLevels;
	ch_node.append_attribute("total_x_size") = m_Settings.PlaneXSize;
	ch_node.append_attribute("total_z_size") = m_Settings.PlaneZSize;
	ch_node.append_attribute("center_x") = m_Settings.PlaneCentreX;
	ch_node.append_attribute("center_z") = m_Settings.PlaneCentreZ;

	WaterBase::WriteSerializedData(ch_node);

	SaveWaterPlaneGrid (Name.c_str());
}
#endif // WO_SERVER

bool obj_WaterPlane::FillWaterGrid ( uint32_t iW, uint32_t iH, bool bUpdateImmediately )
{
	ResetResizeInfo ();

	bool bChanged = m_dGrid[iH][iW] == 0;
	m_dGrid[iH][iW] = 1;

	if(bUpdateImmediately)
		UpdateWaterPlane ();

	return bChanged;
}

bool obj_WaterPlane::EraseWaterGrid ( uint32_t iW, uint32_t iH, bool bUpdateImmediately )
{
	ResetResizeInfo ();

	bool bChanged = m_dGrid[iH][iW] == 1;
	m_dGrid[iH][iW] = 0;

	if(bUpdateImmediately)
		UpdateWaterPlane ();

	return bChanged;
}

uint8_t obj_WaterPlane::GetWaterGridValue(uint32_t iW, uint32_t iH)
{
	if(iW>=m_dGrid.Width() || iH>= m_dGrid.Height())
		return 0;

	return m_dGrid[iH][iW];
}

void obj_WaterPlane::UpdateWaterPlane ()
{
#ifndef WO_SERVER
	if(!m_Plane)
		return;
#endif // WO_SERVER

	if ( m_dGrid.Width() == 0 || m_dGrid.Height() == 0 )
		return;

	float totalW = m_Settings.PlaneXSize;
	float totalH = m_Settings.PlaneZSize;

	float offX = m_Info.OffsetX;
	float offZ = m_Info.OffsetZ;

#ifndef WO_SERVER
	m_Plane->Destroy();
#endif // WO_SERVER

	int iCount = 0;
	for ( uint32_t i = 0; i < m_dGrid.Width(); i++ )
		for ( uint32_t j = 0; j < m_dGrid.Height(); j++ )
			if(m_dGrid[j][i])
				iCount++;

	if(iCount == 0)
		return;

	uint32_t iW = m_dGrid.Width();
	uint32_t iH = m_dGrid.Height();

	r3dTL::T2DArray<r3dVector> dGridVertexesPos;
	ComputeWaterVertexesPos(dGridVertexesPos, offX, offZ);

#ifndef WO_SERVER
	m_Plane->Init(iCount*4,iCount*6, 0);

	R3D_WATER_PLANE_VERTEX * pV = NULL;
	DWORD * pI = NULL;
	int iFirstIndex = 0;
	m_Plane->Lock(iCount*4,iCount*6, &pV, &pI,&iFirstIndex);
	r3d_assert(pV);
	r3d_assert(pI);
	r3d_assert(iFirstIndex==0);
#endif // WO_SERVER

	int iCountAdded = 0;
	r3dVector vMinBound, vMaxBound;

	for ( uint32_t i = 0; i < iW; i++ )
	{
		for ( uint32_t j = 0; j < iH; j++ )
		{
			if(m_dGrid[j][i])
			{
				r3dVector const & v0 = dGridVertexesPos[j][i];
				r3dVector const & v1 = dGridVertexesPos[j+1][i];
				r3dVector const & v2 = dGridVertexesPos[j+1][i+1];
				r3dVector const & v3 = dGridVertexesPos[j][i+1];

#ifndef WO_SERVER
				r3dVector vNormal0 = -(v0-v1).Cross(v2-v1).NormalizeTo();
				r3dVector vNormal1 = -(v2-v3).Cross(v0-v3).NormalizeTo();
				r3dVector vNormalSred = (vNormal0+vNormal1).NormalizeTo();

				pV[0].Position = v0;pV[0].Normal = WaterPlaneComputeNormal(dGridVertexesPos,m_dGrid,i,j);
				pV[1].Position = v1;pV[1].Normal = WaterPlaneComputeNormal(dGridVertexesPos,m_dGrid,i,j+1);
				pV[2].Position = v2;pV[2].Normal = WaterPlaneComputeNormal(dGridVertexesPos,m_dGrid,i+1,j+1);
				pV[3].Position = v3;pV[3].Normal = WaterPlaneComputeNormal(dGridVertexesPos,m_dGrid,i+1,j);
				pI[0] = iCountAdded*4 + 0;
				pI[1] = iCountAdded*4 + 1;
				pI[2] = iCountAdded*4 + 2;
				pI[3] = iCountAdded*4 + 0;
				pI[4] = iCountAdded*4 + 2;
				pI[5] = iCountAdded*4 + 3;

				pV+=4;
				pI+=6;
#endif // WO_SERVER

				// bound box
				if(!iCountAdded)
				{
					vMinBound = v0;
					vMaxBound = v0;
				}

				vMinBound.x = r3dTL::Min(vMinBound.x, r3dTL::Min(v0.x,r3dTL::Min(v1.x,r3dTL::Min(v2.x,v3.x))));
				vMinBound.y = r3dTL::Min(vMinBound.y, r3dTL::Min(v0.y,r3dTL::Min(v1.y,r3dTL::Min(v2.y,v3.y))));
				vMinBound.z = r3dTL::Min(vMinBound.z, r3dTL::Min(v0.z,r3dTL::Min(v1.z,r3dTL::Min(v2.z,v3.z))));

				vMaxBound.x = r3dTL::Max(vMaxBound.x, r3dTL::Max(v0.x,r3dTL::Max(v1.x,r3dTL::Max(v2.x,v3.x))));
				vMaxBound.y = r3dTL::Max(vMaxBound.y, r3dTL::Max(v0.y,r3dTL::Max(v1.y,r3dTL::Max(v2.y,v3.y))));
				vMaxBound.z = r3dTL::Max(vMaxBound.z, r3dTL::Max(v0.z,r3dTL::Max(v1.z,r3dTL::Max(v2.z,v3.z))));

				iCountAdded++;
				r3d_assert(iCount>=0);
			}
		}
	}

	r3d_assert(iCount==iCountAdded);
#ifndef WO_SERVER
	m_Plane->Unlock();
#endif // WO_SERVER

	r3dBoundBox bboxLocal ;

	bboxLocal.Size = vMaxBound - vMinBound;
	m_meshHeight = vMinBound.y;
	bboxLocal.Org = vMinBound;
	bboxLocal.Org.y = m_Settings.WaterPlaneHeight;

	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();

#ifndef WO_SERVER
	/*if(HeightTexture)	r3dRenderer->DeleteTexture( HeightTexture );
	HeightTexture = r3dRenderer->AllocateTexture();
	HeightTexture->Create( 512, 512, D3DFMT_L8, 1 );

	D3DLOCKED_RECT lockedRect;
	IDirect3DTexture9* tex = HeightTexture->AsTex2D();
	D3D_V( tex->LockRect( 0, &lockedRect, NULL, 0 ) );

	unsigned char* data = (unsigned char* )lockedRect.pBits;
	for( int z = 0; z < 512; z ++ )
	{
		for( int x = 0; x < 512; x ++ )
		{
			r3dPoint3D wpoint = bbox_local.Org + bbox_local.Size * r3dPoint3D(float(x)/512.0f, 0.0f, float(z)/512.0f);
			float h = Terrain->GetHeight ( wpoint );
			if(h>m_fWaterPlaneHeight)	*data++ = 255;
			else	*data++ = 0;
		}
		data += lockedRect.Pitch - sizeof( data[ 0 ] ) * 512;
	}

	D3D_V( tex->UnlockRect( 0 ) );*/
#endif // WO_SERVER
}

void obj_WaterPlane::LoadWaterPlaneGrid(const char*szName)
{
	char dir[MAX_PATH], file[MAX_PATH];
	sprintf(dir, "%s\\%s", r3dGameLevel::GetHomeDir(), WATER_PLANES_DIR);
	sprintf(file, "%s\\%s.dat", dir, szName);
	CreateDirectory(dir, NULL);

	r3dFile * f = r3d_open(file, "rb");
	r3d_assert(f);

	if(!f) {
		r3dArtBug("!!!WARNING!!! obj_WaterPlane %s have no data file %s\n", szName, file);

		setActiveFlag(0);
		return;
	}

	WORD version;
	fread(&version, sizeof(version), 1, f);
	if ( version != WATER_VERSION_ID
			&&
		version != 3 )
	{
		r3dError("invalid obj_WaterPlane version %d file %s", version, file);
		fclose(f);
		setActiveFlag(0);
		return;
	}

	uint32_t iW = 0;
	uint32_t iH = 0;

	fread(&iW, sizeof(iW), 1, f);
	fread(&iH, sizeof(iH), 1, f);

	m_dGrid.Resize(iW,iH);

	for(uint32_t j=0;j<iH;j++)
	{
		for(uint32_t i=0;i<iW;i++)
		{
			uint8_t iVal = 0;
			fread(&iVal, sizeof(iVal), 1, f);

			m_dGrid[j][i] = iVal;
		}
	}

	// update m_Info with proper cell counts.
	SetSettings( m_Settings );

	fclose(f);
}

#ifndef WO_SERVER
void obj_WaterPlane::SaveWaterPlaneGrid(const char*szName)
{
	char dir[MAX_PATH], file[MAX_PATH];
	sprintf(dir, "%s\\%s", r3dGameLevel::GetSaveDir(), WATER_PLANES_DIR);
	sprintf(file, "%s\\%s.dat", dir, szName);
	CreateDirectory(dir, NULL);

	FILE* f = fopen_for_write(file, "wb");
	r3d_assert(f);

	WORD version = WATER_VERSION_ID;
	fwrite(&version, sizeof(version), 1, f);

	uint32_t iW = m_dGrid.Width();
	uint32_t iH = m_dGrid.Height();

	fwrite(&iW, sizeof(iW), 1, f);
	fwrite(&iH, sizeof(iH), 1, f);

	for(uint32_t j=0;j<iH;j++)
	{
		for(uint32_t i=0;i<iW;i++)
		{
			uint8_t iVal = m_dGrid[j][i];
			fwrite(&iVal, sizeof(iVal), 1, f);
		}
	}
	fclose(f);
}
#endif // WO_SERVER

void obj_WaterPlane::ComputeCoastSmoothInfo (r3dTL::T2DArray<uint8_t> & tInfo)
{
	r3d_assert( m_Settings.CoastSmoothLevels<7 );

	tInfo.Resize(m_dGrid.Width(),m_dGrid.Height());

	uint32_t iW = tInfo.Width();
	uint32_t iH = tInfo.Height();
	for(uint32_t j=0;j<iH;j++)
		for(uint32_t i=0;i<iW;i++)
			tInfo[j][i] = m_dGrid[j][i]?m_Settings.CoastSmoothLevels+1:0;

	for ( uint32_t k = 1; k < (uint32_t)m_Settings.CoastSmoothLevels+1; k++ )
	{
		for(uint32_t j=0;j<iH;j++)
			for(uint32_t i=0;i<iW;i++)
			{
				if(tInfo[j][i]<k)
					continue;

				if((i>0 && tInfo[j][i-1] == k-1)||
					(i<iW-1 && tInfo[j][i+1] == k-1)||
					(j>0 && tInfo[j-1][i] == k-1)||
					(j<iH-1 && tInfo[j+1][i] == k-1))
					tInfo[j][i] = k;
			}
	}
}

void obj_WaterPlane::ComputeWaterVertexesPos( r3dTL::T2DArray<r3dVector> & tInfo, float offX, float offZ )
{
	// FILL ONLY EXISTS VERTEXES INFO
	uint32_t iW = m_dGrid.Width();
	uint32_t iH = m_dGrid.Height();

	r3dTL::T2DArray<uint8_t> dGridCoastInfo;
	ComputeCoastSmoothInfo(dGridCoastInfo);

	tInfo.Clear ();
	tInfo.Resize(iW+1,iH+1);

	float cellSize = m_Settings.CellGridSize;

	for ( uint32_t i = 0; i < iW; i++ )
	{
		for ( uint32_t j = 0; j < iH; j++ )
		{
			if(m_dGrid[j][i])
			{
				r3dVector v0 = r3dVector((i+0)*cellSize + offX, 0, (j+0)*cellSize + offZ);
				r3dVector v1 = r3dVector((i+0)*cellSize + offX, 0, (j+1)*cellSize + offZ);
				r3dVector v2 = r3dVector((i+1)*cellSize + offX, 0, (j+1)*cellSize + offZ);
				r3dVector v3 = r3dVector((i+1)*cellSize + offX, 0, (j+0)*cellSize + offZ);

				tInfo[j][i] = v0;
				tInfo[j+1][i] = v1;
				tInfo[j+1][i+1] = v2;
				tInfo[j][i+1] = v3;
			}
		}
	}
}

void obj_WaterPlane::UpdateGridOnCellCountChange ()
{
	r3d_assert(m_bResizeInfoExists);

	float cellSize = m_Settings.CellGridSize;

	int iCellCountX = int( m_Settings.PlaneXSize / cellSize ) + 1;
	int iCellCountZ = int( m_Settings.PlaneZSize / cellSize ) + 1;

	m_dGrid.Resize( iCellCountX, iCellCountZ );

	for(uint32_t j = 0; j < m_dGrid.Height (); j++ )
	{
		for(uint32_t i = 0; i < m_dGrid.Width (); i++)
		{
			float coord_x0 = ( i + 0.125f ) * cellSize + m_Info.OffsetX;
			float coord_z0 = ( j + 0.125f ) * cellSize + m_Info.OffsetZ;

			float coord_x1 = ( i + 1.0f - 0.125f ) * cellSize + m_Info.OffsetX;
			float coord_z1 = ( j + 1.0f - 0.125f ) * cellSize + m_Info.OffsetZ;

			int i_old0 = int( ( coord_x0 - m_fOldOffsetX ) / m_fOldCellGridSize );
			int j_old0 = int( ( coord_z0 - m_fOldOffsetZ ) / m_fOldCellGridSize );

			int i_old1 = int( ( coord_x1 - m_fOldOffsetX ) / m_fOldCellGridSize );
			int j_old1 = int( ( coord_z1 - m_fOldOffsetZ ) / m_fOldCellGridSize );

			int found = 0;

			for( int i_old = i_old0; i_old <= i_old1; i_old ++ )
			{
				for( int j_old = j_old0; j_old <= j_old1; j_old ++ )
				{
					if( i_old >= 0 && i_old < (int)m_dOldGrid.Width () && j_old >= 0 && j_old < (int)m_dOldGrid.Height() )
					{
						found = R3D_MAX( (int)found, (int)m_dOldGrid[j_old][i_old] );
					}
				}
			}

			m_dGrid[j][i] = found;
		}
	}
}

void obj_WaterPlane::ResetResizeInfo ()
{
	if(m_bResizeInfoExists)
	{
		m_dOldGrid.Clear ();
		m_bResizeInfoExists = false;
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_WaterPlane::SetGrid(const GridVecType &v)
{
	m_dGrid = v;
#ifndef WO_SERVER
	UpdateWaterPlane();
#endif // WO_SERVER
}


//------------------------------------------------------------------------

const obj_WaterPlane::Settings&	obj_WaterPlane::GetSettings() const
{
	return m_Settings;
}

//------------------------------------------------------------------------

void obj_WaterPlane::SetSettings( const Settings& settings )
{
	int cellCountChange = 0;
	int needUpdatePlane = 0;

	if( fabsf( m_Settings.CellGridSize - settings.CellGridSize ) > FLT_EPSILON )
	{
		cellCountChange = 1;
		needUpdatePlane = 1;
	}

	if( fabsf( m_Settings.WaterPlaneHeight - settings.WaterPlaneHeight ) > FLT_EPSILON )
	{
		needUpdatePlane = 1;
	}

	if( m_Settings.CoastSmoothLevels != settings.CoastSmoothLevels )
	{
		needUpdatePlane = 1;
	}

	if( m_Settings.PlaneXSize != settings.PlaneXSize
			||
		m_Settings.PlaneZSize != settings.PlaneZSize
			)
	{
		cellCountChange = 1;
		needUpdatePlane = 1;
	}

	//------------------------------------------------------------------------

	if( cellCountChange )
	{
		if( !m_bResizeInfoExists )
		{
			m_fOldCellGridSize = m_Settings.CellGridSize;
			m_fOldOffsetX = m_Info.OffsetX;
			m_fOldOffsetZ = m_Info.OffsetZ;
			m_dOldGrid = m_dGrid;
			m_bResizeInfoExists = true;
		}
	}

	m_Settings = settings;

	m_Info.OffsetX = m_Settings.PlaneCentreX - m_Settings.PlaneXSize * 0.5f;
	m_Info.OffsetZ = m_Settings.PlaneCentreZ - m_Settings.PlaneZSize * 0.5f;

	if( cellCountChange )
	{
		UpdateGridOnCellCountChange();
	}

	m_Info.CellCountX = (int)m_dGrid.Width();
	m_Info.CellCountZ = (int)m_dGrid.Height();

#ifndef WO_SERVER
	if( needUpdatePlane )
	{
		UpdateWaterPlane();
	}
#endif // WO_SERVER
}

//------------------------------------------------------------------------

const obj_WaterPlane::Info&	obj_WaterPlane::GetInfo() const
{
	return m_Info;
}

//------------------------------------------------------------------------

void obj_WaterPlane::GetCellIndexes( const r3dPoint3D& pos, int* oCellX, int* oCellZ ) const
{
	*oCellX = int( ( pos.x - m_Info.OffsetX ) / m_Settings.CellGridSize );
	*oCellZ = int( ( pos.z - m_Info.OffsetZ ) / m_Settings.CellGridSize );
}