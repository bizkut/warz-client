#include "r3dpch.h"
#include "r3d.h"
#include "GameLevel.h"

#ifndef WO_SERVER
#include "UI/UIimEdit.h"
#endif // WO_SERVER

#include "GameCommon.h"
#include "GameObjects\gameobj.h"
#ifndef WO_SERVER
#include "GameObjects\objmanag.h"
#endif // WO_SERVER

#include "Lake.h"
#ifndef WO_SERVER
#include "EnvmapProbes.h"
#include "TrueNature2\Terrain3.h"
#include "TrueNature2\Terrain2.h"


LPDIRECT3DVERTEXDECLARATION9 R3D_WATER_VERTEX::pDecl = 0;
D3DVERTEXELEMENT9 R3D_WATER_VERTEX::VBDecl[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 R3D_WATER_VERTEX::getDecl()
{
	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}
#endif // WO_SERVER

//
//
// 	class for  Lake object, as you may guess..
//
//

IMPLEMENT_CLASS(obj_Lake, "obj_Lake", "Object");
AUTOREGISTER_CLASS(obj_Lake);

	float		LakePlaneY = 101.0;
	float		LakeDepth  = 100;

#ifndef WO_SERVER
extern r3dScreenBuffer*	DepthBuffer;
#endif // WO_SERVER

obj_Lake		*objOceanPlane;


obj_Lake::obj_Lake()
: PlaneScale( 1.f )
, CurrentScale( 0.f )
{
#ifndef WO_SERVER
	memset(WaterColor,0, sizeof(WaterColor));
	FoamTexture = 0;
	SpecularTexture = 0;
	ColorTexture = 0;
	MaskTexture = 0;

	DrawDistanceSq = FLT_MAX;
#endif // WO_SERVER
}

#ifndef WO_SERVER
#ifndef FINAL_BUILD
float obj_Lake :: DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
 float starty = scry;

 starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected);

 if( IsParentOrEqual( &ClassData, startClass ) )
 {
	 starty += imgui_Value_Slider( scrx, starty, "Plane Scale", &PlaneScale, 1.0f, 8.0f, "%.2f" );
 }

 return starty-scry;
}
#endif
#endif // WO_SERVER

extern r3dITerrain*	Terrain;

BOOL obj_Lake::OnCreate()
{
	parent::OnCreate();
	objOceanPlane    = this;
#ifndef WO_SERVER
	char Str[256];

	DrawOrder	= OBJ_DRAWORDER_LAST;
	setSkipOcclusionCheck(true);
	ObjFlags      |= OBJFLAG_SkipCastRay | OBJFLAG_DisableShadows |	OBJFLAG_ForceSleep;

#ifndef FINAL_BUILD
	extern int __HaveOcean;
	__HaveOcean = 1;
#endif


	//extern  int bRenderReflection;
	extern int bRenderRefraction;

	//bRenderReflection = 1;
	bRenderRefraction = 1;

	WaterBase::OnCreateBase();
	CreateWaterBuffers();

	OceanPlane = game_new r3dXPSObject<R3D_WATER_VERTEX>;

	UpdateGrid();

	UpdateTransform();

	if ( ! WaterColor[0] )
	{
		for(int i=0; i<25; ++i)
		{
			sprintf(Str,"Data\\Water\\waves_%.2d.dds",i);
			WaterColor[i] = r3dRenderer->LoadTexture(Str);
		}
		ColorTexture = r3dRenderer->LoadTexture("Data\\Water\\LakeColor.dds");
		FoamTexture = r3dRenderer->LoadTexture("Data\\Water\\foam02.jpg");
		//SpecularTexture = r3dRenderer->LoadTexture("Data\\Water\\water_gloss.dds");
		//MaskTexture = r3dRenderer->LoadTexture("Data\\Water\\LakeMask.dds");
	}
#endif // WO_SERVER

	return true;
}



BOOL obj_Lake::OnDestroy()
{
	objOceanPlane    = NULL;

#ifndef WO_SERVER
	delete OceanPlane;

	WaterBase::OnDestroyBase();
	for(int i=0; i<25; ++i)
		r3dRenderer->DeleteTexture(WaterColor[i]);
	r3dRenderer->DeleteTexture(ColorTexture);
	r3dRenderer->DeleteTexture(FoamTexture);
	//r3dRenderer->DeleteTexture(SpecularTexture);
	//r3dRenderer->DeleteTexture(MaskTexture);
#endif // WO_SERVER

	return parent::OnDestroy();
}



BOOL obj_Lake::Update()
{
	//apply dynamically changed height

	UpdateGrid();

	r3dBoundBox bboxLocal = GetBBoxLocal();

	bboxLocal.Org = r3dPoint3D(0,LakePlaneY,0);

	SetBBoxLocal( bboxLocal );

	UpdateTransform();
	return TRUE;
}

void obj_Lake::UpdateGrid()
{
	if( CurrentScale != PlaneScale )
	{
		r3dBoundBox bboxLocal ;

		float Width, Height, CellSize;

		if(Terrain)
		{
			const r3dTerrainDesc& desc = Terrain->GetDesc() ;

			Width = desc.XSize;
			Height = desc.ZSize;
		}
		else
		{
			Width = 1024.0f * 8.0f;
			Height = 1024.0f * 8.0f ;
		}

		CellSize = R3D_MAX( Width / 64.0f, Height / 64.0f );

		float startX = ( Width - Width * PlaneScale ) * 0.5f;
		float startY = ( Height - Height * PlaneScale ) * 0.5f;

		Width *= PlaneScale;
		Height *= PlaneScale;

#ifndef WO_SERVER
		OceanPlane->InitGrid( (int)startX,(int)startY, int( Width / CellSize ), int( Height / CellSize ), CellSize, 0.0f);
#endif // WO_SERVER
		bboxLocal.Size = r3dPoint3D( Width, 1.0f, Height ) ;

		bboxLocal.Org = r3dPoint3D( -startX * CellSize, LakePlaneY, -startY * CellSize );

		SetBBoxLocal( bboxLocal ) ;

		CurrentScale = PlaneScale;
	}
}

#ifndef WO_SERVER
void obj_Lake::DrawComposite( const r3dCamera& Cam )
{
	R3DPROFILE_FUNCTION("Lake::DoDraw");

	RenderBegin(Cam, LakePlaneY);

	OceanPlane->Draw();

	RenderEnd();
}

//------------------------------------------------------------------------

void obj_Lake::DrawDepth( const r3dCamera& Cam )
{
	return;
}
//------------------------------------------------------------------------
/*virtual*/

struct LakeCompositeRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		LakeCompositeRenderable *This = static_cast<LakeCompositeRenderable*>( RThis );

		This->Parent->DrawComposite( Cam );
	}

	obj_Lake* Parent;
};

struct LakeDepthRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		LakeDepthRenderable *This = static_cast<LakeDepthRenderable*>( RThis );

		This->Parent->DrawDepth( Cam );
	}

	obj_Lake* Parent;
};

void
obj_Lake::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
#define RENDERABLE_LAKE_SORT_VALUE (16 * RENDERABLE_USER_SORT_VALUE)
	// composite
	{
		LakeCompositeRenderable rend;

		rend.Init();

		rend.Parent = this;
		rend.SortValue = RENDERABLE_LAKE_SORT_VALUE;

		render_arrays[ rsDrawComposite2 ].PushBack( rend );
	}

	// depth effect
	{
		LakeDepthRenderable rend;

		rend.Init();

		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_LAKE_SORT_VALUE;

		render_arrays[ rsDrawDepthEffect ].PushBack( rend );
	}
}
#endif // WO_SERVER

void obj_Lake::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node lakeNode = node.child("lake");
	LakePlaneY = lakeNode.attribute("lakePlaneY").as_float();

	pugi::xml_node ch_node = node.child("new_lake");

#ifndef WO_SERVER
	if( !ch_node.attribute( "offset_x" ).empty() )
	{
		PlaneOffsetX = ch_node.attribute( "offset_x" ).as_float();
	}

	if( !ch_node.attribute( "offset_z" ).empty() )
	{
		PlaneOffsetZ = ch_node.attribute( "offset_z" ).as_float();
	}
#endif // WO_SERVER

	if( !ch_node.attribute( "plane_scale" ).empty() )
	{
		PlaneScale = ch_node.attribute( "plane_scale" ).as_float();
	}

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
				WaterColor[i] = r3dRenderer->LoadTexture(Str);
			}
		}
		ColorTexture = r3dRenderer->LoadTexture("Data\\Water\\LakeColor.dds");
		FoamTexture = r3dRenderer->LoadTexture("Data\\Water\\foam02.jpg");
		//SpecularTexture = r3dRenderer->LoadTexture("Data\\Water\\water_gloss.dds");
		//MaskTexture = r3dRenderer->LoadTexture("Data\\Water\\LakeMask.dds");
#endif // WO_SERVER

		WaterBase::ReadSerializedData(ch_node);
	}
}

#ifndef WO_SERVER
void obj_Lake::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node lakeNode = node.append_child();
	lakeNode.set_name("lake");
	lakeNode.append_attribute("lakePlaneY") = LakePlaneY;


	pugi::xml_node ch_node = node.append_child();
	ch_node.set_name( "new_lake" );

	ch_node.append_attribute( "offset_x" ) = PlaneOffsetX;
	ch_node.append_attribute( "offset_z" ) = PlaneOffsetZ;

	ch_node.append_attribute( "plane_scale" ) = PlaneScale;

	if ( WaterColor[0] )
	{
		char fn[256];
		memcpy(fn, WaterColor[0]->getFileLoc().FileName, sizeof(WaterColor[0]->getFileLoc().FileName));
		fn[ strlen(fn) - strlen("00.dds") ] = 0;
		ch_node.append_attribute("wave_tex") = fn;
	}

	WaterBase::WriteSerializedData(ch_node);
}
#endif // WO_SERVER


