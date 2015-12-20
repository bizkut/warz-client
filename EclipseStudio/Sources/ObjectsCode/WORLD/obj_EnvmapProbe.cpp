#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "GameLevel.h"
#include "EnvmapProbes.h"
#include "EnvmapGen.h"
#include "obj_EnvmapProbe.h"
#include "Editors/ObjectManipulator3d.h"

IMPLEMENT_CLASS(obj_EnvmapProbe, "obj_EnvmapProbe", "Object");
AUTOREGISTER_CLASS(obj_EnvmapProbe);

static r3dTexture *EnvProbeIcon = NULL;

namespace
{
	const int DEFAULT_ENVMAP_EDGE = 128;
}

obj_EnvmapProbe::obj_EnvmapProbe ()
: mTexture( NULL )
, mSelected ( false )
, mInfiniteRadius( 0 )
, mID( mIDGen++ )
, mEdgeSize( DEFAULT_ENVMAP_EDGE )
{
	g_EnvmapProbes.Register( this );

	ObjFlags |= OBJFLAG_IconSelection;

	mSize.x = 15.0f;
	mSize.y = 15.0f;
	mSize.z = 15.0f;

	mCurrentBBoxSize.x = -1.0f;
	mCurrentBBoxSize.y = -1.0f;
	mCurrentBBoxSize.z = -1.0f;
}

//------------------------------------------------------------------------

obj_EnvmapProbe::~obj_EnvmapProbe ()
{
	g_EnvmapProbes.Unregister( this );
	if( mTexture )
	{
		r3dRenderer->DeleteTexture( mTexture );
	}
}

//------------------------------------------------------------------------
/*virtual*/
BOOL
obj_EnvmapProbe::OnCreate()
{
	if (!EnvProbeIcon) EnvProbeIcon = r3dRenderer->LoadTexture("Data\\Images\\EnvProbe.dds");

	DrawOrder	= OBJ_DRAWORDER_LAST;

	setSkipOcclusionCheck(true);
	ObjFlags	|=	OBJFLAG_DisableShadows | OBJFLAG_ForceSleep;

	UpdateBBox();

	return GameObject::OnCreate();
}

//------------------------------------------------------------------------
/*virtual*/
BOOL
obj_EnvmapProbe::OnDestroy()
{
	return GameObject::OnDestroy();
}

//------------------------------------------------------------------------

void obj_EnvmapProbe::OnDelete()
{
	GameObject::OnDelete();

	remove( GetTextureName( false ).c_str() );
}

//------------------------------------------------------------------------

struct EnvmapProbeRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("EnvmapProbeRenderable");
		EnvmapProbeRenderable *This = static_cast<EnvmapProbeRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_EnvmapProbe*	Parent;	
};


/*virtual*/
void
obj_EnvmapProbe::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifdef FINAL_BUILD
	return;
#else
	// don't draw debug info if we're not in editor mode
	extern bool g_bEditMode;
	if ( !g_bEditMode )
		return;

	if( r_hide_icons->GetInt() )
		return;

	float idd = r_icons_draw_distance->GetFloat();
	idd *= idd;

	if( ( Cam - GetPosition() ).LengthSq() > idd )
		return;

	EnvmapProbeRenderable rend;

	rend.Init();
	rend.Parent = this;
	rend.SortValue = 14 * RENDERABLE_USER_SORT_VALUE;

	render_arrays[ rsDrawDebugData ].PushBack( rend );
#endif
}

//------------------------------------------------------------------------
void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
void obj_EnvmapProbe::DoDraw()
{
#ifndef FINAL_BUILD
	D3DPERF_BeginEvent( 0, L"obj_EnvmapProbe::DoDraw" );

	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);

	r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

	r3dDrawIcon3D(GetPosition(), EnvProbeIcon, r3dColor(255,255,255), 24);

	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);

	if ( g_Manipulator3d.IsSelected( this ) )
	{
		int usedVSShader = r3dRenderer->CurrentVertexShaderID;
		int usedPSShader = r3dRenderer->CurrentPixelShaderID;

		if( mInfiniteRadius )
		{
			float radius = 33.f;

			float helperSphereRadius = R3D_MIN( 22.f, radius * 0.22f );

			r3dColor greyedColr = r3dColor( 0x7f, 0x7f, 0x7f, 0x99 );

			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );

			r3dDrawUniformSphere ( GetPosition(), radius, gCam, greyedColr );
			r3dDrawSphereSolid ( GetPosition(), helperSphereRadius, gCam, greyedColr );

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

			r3dColor useColr;

			// draw sphere
			useColr = mInfiniteRadius ? r3dColor( 0xd0, 0xd0, 0x70, 0xff ) : r3dColor( 0xa0, 0xa0, 0xd0, 0xff );
			r3dDrawUniformSphere ( GetPosition(), radius, gCam, useColr );

			r3dDrawSphereSolid ( GetPosition(), helperSphereRadius, gCam, useColr );

			r3dRenderer->Flush();
		}
		else
		{
			r3dColor greyedColr = r3dColor( 0x7f, 0x7f, 0x7f, 0x99 );

			D3DXMATRIX identity;
			D3DXMatrixIdentity( &identity );

			D3D_V( r3dRenderer->pd3ddev->SetTransform( D3DTS_WORLD, &identity ) );

			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );

			r3dBoundBox bbox;

			bbox.Org = GetPosition() - mSize * 0.5f;
			bbox.Size = mSize;

			r3dDrawUniformBoundBox( bbox, gCam, greyedColr );

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

			r3dColor useColr;

			// draw sphere
			useColr = r3dColor( 0xa0, 0xa0, 0xd0, 0xff );

			r3dDrawUniformBoundBox( bbox, gCam, useColr );
			r3dRenderer->Flush();

			D3DXMATRIX xfm;
			D3DXMatrixScaling( &xfm, mSize.x * 0.5f, mSize.y * 0.5f, mSize.z * 0.5f );

			const r3dPoint3D& pos = GetPosition();

			xfm._41 = pos.x;
			xfm._42 = pos.y;
			xfm._43 = pos.z;

			D3DXMATRIX compoundXForm = xfm * r3dRenderer->ViewProjMatrix;

			D3DXMatrixTranspose( &compoundXForm, &compoundXForm );

			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &compoundXForm._11, 4 ) );

			r3dDrawSolidBoxStart();
			r3dSetFwdColorShaders( r3dColor( 55, 55, 55, 96 ) );
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );
			r3dDrawSolidBox();
			r3dSetFwdColorShaders( r3dColor( 120, 120, 33, 96 ) );
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );
			r3dDrawSolidBox();
			r3dDrawSolidBoxEnd();

			D3D_V( r3dRenderer->pd3ddev->SetTransform( D3DTS_WORLD, &identity ) );
		}

		r3dRenderer->SetVertexShader( usedVSShader );
		r3dRenderer->SetPixelShader( usedPSShader );
	}

	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

BOOL obj_EnvmapProbe::Update()
{
#ifndef FINAL_BUILD
	r3dPoint3D vec = GetScale();

	if( vec.x != 1.0f || vec.y != 1.0f || vec.z != 1.0f )
	{
		SetScale( r3dPoint3D( 1.0f, 1.0f, 1.0f ) );
	}

	r3dPoint3D rot = GameObject::GetRotationVector();

	if( rot.x != 0 || rot.y != 0 || rot.z != 0 )
	{
		rot.x = rot.y = rot.z = 0;
		SetRotationVector( rot );
	}

	if( mSize.x != mCurrentBBoxSize.x 
			||
		mSize.y != mCurrentBBoxSize.y
			||
		mSize.z != mCurrentBBoxSize.z
		)
	{
		mCurrentBBoxSize = mSize;
		UpdateBBox();
	}
#endif

	return TRUE;
}

//------------------------------------------------------------------------
void obj_EnvmapProbe::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node envmapNode = node.child("envmapProbe");
	float fMaxRadius;
	int id ( WRONG_ID );
	int isInfinite;

	pugi::xml_attribute sizeXAttrib = envmapNode.attribute( "size_x" );

	if( !sizeXAttrib.empty() )
	{
		mSize.x = sizeXAttrib.as_float();
		mSize.y = envmapNode.attribute( "size_y" ).as_float();
		mSize.z = envmapNode.attribute( "size_z" ).as_float();
	}
	else
	{
		pugi::xml_attribute radAttrib = envmapNode.attribute( "radius" );

		if( !radAttrib.empty() )
			fMaxRadius = radAttrib.as_float();
		else
			fMaxRadius = 1.0f;

		mSize.x = fMaxRadius * 2.f;
		mSize.y = fMaxRadius * 2.f;
		mSize.z = fMaxRadius * 2.f;
	}

	isInfinite = envmapNode.attribute("infRadius").as_int();
	id = envmapNode.attribute("id").as_int();

	r3d_assert( id != WRONG_ID );

	pugi::xml_attribute edge_att = envmapNode.attribute("edge");

	if( !edge_att.empty() )
	{
		mEdgeSize = edge_att.as_int();
	}
	else
	{
		mEdgeSize = DEFAULT_ENVMAP_EDGE;
	}

	mEdgeSize = R3D_MAX( R3D_MIN( mEdgeSize, 2048 ), 4 );

	SetID( id );

	mInfiniteRadius = isInfinite;

	mTexture = r3dRenderer->LoadTexture( GetTextureName( false ).c_str() );

	if( !mTexture->IsValid() )
	{
		r3dRenderer->DeleteTexture( mTexture );
		mTexture = NULL;
	}

	UpdateBBox();
}
//------------------------------------------------------------------------
void obj_EnvmapProbe::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node envmapNode = node.append_child();
	envmapNode.set_name("envmapProbe");

	envmapNode.append_attribute("size_x") = mSize.x;
	envmapNode.append_attribute("size_y") = mSize.y;
	envmapNode.append_attribute("size_z") = mSize.z;

	envmapNode.append_attribute("infRadius") = mInfiniteRadius;
	envmapNode.append_attribute("id") = mID;
	envmapNode.append_attribute("edge") = mEdgeSize;
}


//------------------------------------------------------------------------
/*virtual*/

#ifndef FINAL_BUILD

static float listOffset = 0.f;

float
obj_EnvmapProbe::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float ystart = scry;

	ystart += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		ystart += 11;
		ystart += imgui_Static ( scrx, ystart, "Envmap Probe Properties" );

		int check = mInfiniteRadius;
		ystart += imgui_Checkbox( scrx, ystart, "Infinite radius", &check, 1 );
		PropagateChange( check, &obj_EnvmapProbe::mInfiniteRadius, this, selected );

		if( !mInfiniteRadius )
		{
			static r3dVector size ( 0.f, 0.f, 0.f );
			
			size.x = mSize.x;
			size.y = mSize.y;
			size.z = mSize.z;

			ystart += imgui_Value_Slider ( scrx, ystart, "X Size", &size.x, 15, 3000, "%-02.02f" );
			ystart += imgui_Value_Slider ( scrx, ystart, "Y Size", &size.y, 15, 3000, "%-02.02f" );
			ystart += imgui_Value_Slider ( scrx, ystart, "Z Size", &size.z, 15, 3000, "%-02.02f" );

			PropagateChange( size, &obj_EnvmapProbe::mSize, this, selected );
		}

		ystart += 11;
		ystart += imgui_Static( scrx, ystart, "Cubemap Edge Size" );

		stringlist_t edgeResolutions;

		edgeResolutions.push_back( "16" );
		edgeResolutions.push_back( "32" );
		edgeResolutions.push_back( "64" );
		edgeResolutions.push_back( "128" );
		edgeResolutions.push_back( "256" );
		edgeResolutions.push_back( "512" );
		edgeResolutions.push_back( "1024" );

		const float LIST_HEIGHT = 160;

		int selection = 0;
		int edge = mEdgeSize;
		for(; edge; edge /= 2, selection ++ );

		selection -= 5;

		if( selection < 0 ) selection = 0;

		imgui_DrawList( scrx, ystart, 360, LIST_HEIGHT, edgeResolutions, &listOffset, &selection );

		selection =	R3D_MIN( R3D_MAX( selection, 0 ), (int)edgeResolutions.size() - 1 );

		edge = 16 << selection;

		if( edge != mEdgeSize )
		{
			mEdgeSize = edge;
			r_need_gen_envmap->SetInt( mID + 1 );
		}

		ystart += LIST_HEIGHT;

		if( imgui_Button( scrx, ystart, 360.f, 22.f, "Regenerate Probe" ) )
		{
			r_need_gen_envmap->SetInt( mID + 1 );
		}

		ystart += 33.f;

		if( imgui_Button( scrx, ystart, 360.f, 22.f, "Regenerate all envmap probes") )
		{
			r_need_gen_envmap->SetInt( 0x7fffffff );
		}

		ystart += 33.0f;
	}
	return ystart - scry;
}
#endif

//------------------------------------------------------------------------

float obj_EnvmapProbe::GetRadius () const
{
	return sqrtf( 0.25f * ( mSize.x * mSize.x + mSize.y * mSize.y + mSize.z * mSize.z ) );
}

//------------------------------------------------------------------------

float
obj_EnvmapProbe::GetRadiusSq() const
{
	return 0.25f * ( mSize.x * mSize.x + mSize.y * mSize.y + mSize.z * mSize.z );
}

//------------------------------------------------------------------------

r3dTexture*
obj_EnvmapProbe::GetTexture() const
{
	return mTexture;
}

//------------------------------------------------------------------------

void
obj_EnvmapProbe::SetSelected( bool selected )
{
	mSelected = selected;
}

//------------------------------------------------------------------------

void
obj_EnvmapProbe::Generate()
{
	obj_EnvmapProbe::CreateFolder( false );

	if( mTexture )
	{
		r3dRenderer->DeleteTexture( mTexture, true );
	}

	mTexture = r3dRenderer->AllocateTexture();

	int w = mEdgeSize;

	int mips = 0;

	for(; w; w /= 2 )
	{
		mips ++;	
	}

	mTexture->CreateCubemap( mEdgeSize, D3DFMT_DXT1, mips );

	GenerateEnvmap64F( mTexture, GetTextureName( false ), GetPosition() );
}

//------------------------------------------------------------------------

int
obj_EnvmapProbe::IsInfinite() const
{
	return mInfiniteRadius;
}

//------------------------------------------------------------------------

namespace
{
	const char* ENVMAP_FOLDER = "\\Envmaps";
}

r3dString
obj_EnvmapProbe::GetTextureName( bool forSaving ) const
{
	r3d_assert( mID != WRONG_ID );

	char buffer[64];

	sprintf( buffer, "envmap%d.dds", mID );

	return r3dString( forSaving ? r3dGameLevel::GetSaveDir() : r3dGameLevel::GetHomeDir() ) + ENVMAP_FOLDER + "\\" + buffer;
}

//------------------------------------------------------------------------

int
obj_EnvmapProbe::GetID() const
{
	return mID;
}

//------------------------------------------------------------------------

/*static*/
void	
obj_EnvmapProbe::CreateFolder( bool forSaving )
{
	struct _stat st;
	if( _stat( ENVMAP_FOLDER, &st ) )
	{
		mkdir( (r3dString( forSaving ? r3dGameLevel::GetSaveDir() : r3dGameLevel::GetHomeDir() ) + ENVMAP_FOLDER).c_str() );
	}
	else
	{
		r3d_assert( st.st_mode & _S_IFDIR );
	}
}

//------------------------------------------------------------------------

void
obj_EnvmapProbe::SetID( int id )
{
	mID = id;
	mIDGen = R3D_MAX( mIDGen, id + 1 );
}

//------------------------------------------------------------------------

void
obj_EnvmapProbe::UpdateBBox()
{
	r3dBoundBox bboxLocal;

	bboxLocal.Size = mSize;
	bboxLocal.Org = -bboxLocal.Size * 0.5f;

	SetBBoxLocal( bboxLocal );
	UpdateTransform();
}

//------------------------------------------------------------------------
/*static*/ int obj_EnvmapProbe::mIDGen = 0;
