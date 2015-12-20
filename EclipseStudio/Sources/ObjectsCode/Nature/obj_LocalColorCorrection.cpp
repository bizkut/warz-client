#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_LocalColorCorrection.h"

IMPLEMENT_CLASS(obj_LocalColorCorrection, "obj_LocalColorCorrection", "Object");
AUTOREGISTER_CLASS(obj_LocalColorCorrection);

#pragma warning ( disable: 4244 )

obj_LocalColorCorrection::Objects_t obj_LocalColorCorrection::m_dObjects;

obj_LocalColorCorrection::obj_LocalColorCorrection ()
: m_fRadiusMin ( 50 )
, m_fRadiusMax ( 150 )
, m_pCCTexture ( NULL )
, m_bSelected( false )
{
	m_dObjects.PushBack(this);
	setSkipOcclusionCheck(true);
	ObjFlags |= OBJFLAG_SkipCastRay | OBJFLAG_DisableShadows | OBJFLAG_ForceSleep ;
}

obj_LocalColorCorrection::~obj_LocalColorCorrection ()
{
	int iEraseIndex = -1;
	for ( unsigned int i = 0; i < m_dObjects.Count (); i++ )
	{
		if ( m_dObjects[i] == this )
		{
			iEraseIndex = i;
			break;
		}
	}
	r3d_assert ( iEraseIndex >= 0 );
	if ( iEraseIndex >= 0 )
		m_dObjects.Erase(iEraseIndex);
}

/*virtual*/
BOOL
obj_LocalColorCorrection::OnDestroy()
{
	// always try to destroy texture so that we keep minimum of them at a given level
	DestroyTexture();

	return GameObject::OnDestroy();
}

/*virtual*/ 
GameObject*	
obj_LocalColorCorrection::Clone ()
{
	obj_LocalColorCorrection * pLocalCC = (obj_LocalColorCorrection*)( srv_CreateGameObject ( "obj_LocalColorCorrection", "_local_cc", GetPosition() ) );
	r3d_assert ( pLocalCC );

	pLocalCC->RadiusMinSet ( RadiusMin () );
	pLocalCC->RadiusMaxSet ( RadiusMax () );
	pLocalCC->CCTextureNameSet ( m_sCCTexFileName.c_str() );
	pLocalCC->Update ();

	return pLocalCC;
}

/*virtual*/
#ifndef FINAL_BUILD
float		
obj_LocalColorCorrection::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	const int CC_FILE_LIST_SIZE = 300;
	float starty = scry;

	starty += parent::DrawPropertyEditor( scrx, scry, scrw, scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Local CC Properties" );
		starty += 10;

		static float radiusMaxVal ;
		radiusMaxVal = m_fRadiusMax ;
		starty += imgui_Value_Slider ( scrx, starty, "Radius max", &radiusMaxVal, 15, 10000, "%-02.02f" );
		PropagateChange( radiusMaxVal, &obj_LocalColorCorrection::m_fRadiusMax, this, selected ) ;

		static float radiusMinVal ;
		radiusMinVal = m_fRadiusMin ;
		starty += imgui_Value_Slider ( scrx, starty, "Radius min", &radiusMinVal, 5, m_fRadiusMax - 5, "%-02.02f" );
		PropagateChange( radiusMinVal, &obj_LocalColorCorrection::m_fRadiusMin, this, selected ) ;

		char buf[512];
		sprintf(buf, "Texture: %s", m_sCCTexFileName.c_str());
		starty += imgui_Static ( scrx, starty, buf );

		char fullPath[ 512 ];
		GetCCLUT3DTextureFullPath( fullPath, "*.dds" );

		static char textureNameLocal[ 256 ] = {0};
		r3dscpy ( textureNameLocal, m_sCCTexFileName.c_str () ); 

		static float offset = 0.f ;

		if ( imgui_FileList(scrx, starty, 360, CC_FILE_LIST_SIZE, fullPath, textureNameLocal, &offset ) )
		{
			PropagateChange( (const char * )textureNameLocal, &obj_LocalColorCorrection::CCTextureNameSet, selected ) ;
		}

		starty += CC_FILE_LIST_SIZE;
	}

	return starty - scry;
}
#endif

struct LCCRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		LCCRenderable *This = static_cast<LCCRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_LocalColorCorrection*	Parent;	
};

/*virtual*/
void
obj_LocalColorCorrection::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	// don't draw debug info if we're not in editor mode
	extern bool g_bEditMode;
	if ( !g_bEditMode )
		return;

	LCCRenderable rend;

	rend.Init();

	rend.Parent		= this;
	rend.SortValue	= 12 * RENDERABLE_USER_SORT_VALUE;

	render_arrays[ rsDrawDebugData ].PushBack( rend );
#endif
}

void obj_LocalColorCorrection::DoDraw()
{
	r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );

	float helperSphereRadius = R3D_MIN( 22.f, m_fRadiusMin * 0.22f );

	r3dColor greyedColr = r3dColor( 0x7f, 0x7f, 0x7f, 0x99 );

	r3dDrawUniformSphere ( GetPosition(), m_fRadiusMax, gCam, greyedColr );
	r3dDrawUniformSphere ( GetPosition(), m_fRadiusMin, gCam, greyedColr );

	r3dDrawSphereSolid ( GetPosition(), helperSphereRadius, gCam, r3dColor( 0x7f, 0x7f, 0x7f, 0x99 ) );

	r3dRenderer->Flush();

	r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

	r3dColor useColr;

	if( m_bSelected )
	{
		useColr = r3dColor( 0xd0, 0x20, 0x20, 0xff  );
		r3dDrawUniformSphere ( GetPosition(), m_fRadiusMax, gCam, useColr );
		r3dDrawUniformSphere ( GetPosition(), m_fRadiusMin, gCam, r3dColor( 0xd0, 0xd0, 0x20, 0xff  ) );
	}
	else
	{
		useColr = r3dColor( 0xa0, 0xd0, 0xa0, 0xff );
		r3dDrawUniformSphere ( GetPosition(), m_fRadiusMax, gCam, useColr );
		r3dDrawUniformSphere ( GetPosition(), m_fRadiusMin, gCam, r3dColor( 0xdf, 0xff, 0xdf, 0xff ) );
	}

	r3dDrawSphereSolid ( GetPosition(), helperSphereRadius, gCam, useColr );

	r3dRenderer->Flush();
}

BOOL obj_LocalColorCorrection::Update()
{
	float f = m_fRadiusMax * 2.f;

	r3dBoundBox bboxLocal ;

	bboxLocal.Size = r3dPoint3D ( f, f, f );
	bboxLocal.Org = -bboxLocal.Size * 0.5f;
	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();

	return TRUE;
}

void obj_LocalColorCorrection::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node localNode = node.child("localColorCorrection");
	float fRadiusMin, fRadiusMax;
	fRadiusMin = localNode.attribute("radiusMin").as_float();
	fRadiusMax = localNode.attribute("radiusMax").as_float();
	RadiusMinSet(fRadiusMin);
	RadiusMaxSet(fRadiusMax);
	CCTextureNameSet (localNode.attribute("textureFilename").value());
}

void obj_LocalColorCorrection::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node localNode = node.append_child();
	localNode.set_name("localColorCorrection");
	localNode.append_attribute("radiusMin") = m_fRadiusMin;
	localNode.append_attribute("radiusMax") = m_fRadiusMax;
	localNode.append_attribute("textureFilename") = m_sCCTexFileName.c_str();
}


void obj_LocalColorCorrection::RadiusMinSet ( float fRadius )
{
	r3d_assert ( fRadius > 0.0f );
	m_fRadiusMin = fRadius;
}

float obj_LocalColorCorrection::RadiusMin () const
{
	return m_fRadiusMin;
}

void obj_LocalColorCorrection::RadiusMaxSet ( float fRadius )
{
	r3d_assert ( fRadius > 0.0f );
	m_fRadiusMax = fRadius;
}

float obj_LocalColorCorrection::RadiusMax () const
{
	return m_fRadiusMax;
}

void obj_LocalColorCorrection::CCTextureNameSet ( const char * const &  sName )
{
	DestroyTexture();

	m_sCCTexFileName = sName;

	if( !m_sCCTexFileName.IsEmpty() )
	{
		char fullPath[ 512 ];
		GetCCLUT3DTextureFullPath( fullPath, m_sCCTexFileName.c_str() );

		m_pCCTexture = r3dRenderer->LoadTexture( fullPath );
	}
}

r3dTexture * obj_LocalColorCorrection::CCTexture () const
{
	return m_pCCTexture;
}

void
obj_LocalColorCorrection::SetSelected( bool selected )
{
	m_bSelected = selected;
}

//static 
unsigned int obj_LocalColorCorrection::LocalColorCorrectionCount ()
{
	return m_dObjects.Count();
}

//static 
obj_LocalColorCorrection * obj_LocalColorCorrection::LocalColorCorrectionGet ( int i )
{
	return m_dObjects[i];
}

//static
obj_LocalColorCorrection * obj_LocalColorCorrection::FindNearestColorCorrection ()
{
	obj_LocalColorCorrection * pNearestLocalCC = NULL;

	float fMinLenSq = 0.0f;
	for ( unsigned int i = 0; i < obj_LocalColorCorrection::LocalColorCorrectionCount(); i++ )
	{
		obj_LocalColorCorrection * pLocal = obj_LocalColorCorrection::LocalColorCorrectionGet(i);
		r3d_assert ( pLocal );
		float fMinLenSqNew = (gCam - pLocal->GetPosition()).LengthSq();
		if ( fMinLenSqNew < fMinLenSq || !pNearestLocalCC )
		{
			pNearestLocalCC = pLocal;
			fMinLenSq = fMinLenSqNew;
		}
	}

	if ( pNearestLocalCC && (fMinLenSq >= pNearestLocalCC->RadiusMax()*pNearestLocalCC->RadiusMax()) )
		pNearestLocalCC = NULL;

	return pNearestLocalCC;
}

//------------------------------------------------------------------------
/*static*/

int
obj_LocalColorCorrection::GetTextureCount( const FixedString& name )
{
	int count = 0;

	for ( unsigned int i = 0; i < obj_LocalColorCorrection::LocalColorCorrectionCount(); i++ )
	{
		obj_LocalColorCorrection * pLocal = obj_LocalColorCorrection::LocalColorCorrectionGet(i);
		r3d_assert ( pLocal );

		if( !strcmpi( pLocal->m_sCCTexFileName.c_str(), name ) )
			count ++;
	}

	return count;
}

//------------------------------------------------------------------------
/*static*/
void
obj_LocalColorCorrection::DeselectAll()
{
	for( uint32_t i = 0, e = LocalColorCorrectionCount(); i < e; i ++ )
	{
		LocalColorCorrectionGet( i )->SetSelected( false );
	}
}

//------------------------------------------------------------------------

void
obj_LocalColorCorrection::DestroyTexture()
{
	if( m_pCCTexture )
	{
		r3d_assert( !m_sCCTexFileName.IsEmpty() );
		r3dRenderer->DeleteTexture( m_pCCTexture );
	}

	m_pCCTexture = NULL;
	m_sCCTexFileName.SetEmpty();
}

//------------------------------------------------------------------------
