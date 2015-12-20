#include "r3dPCH.h"
#include "r3d.h"

#include "../sf/RenderBuffer.h"

#include "r3dGeoSphere.h"
#include "r3dChamferBox.h"
#include "r3dConePrim.h"

static r3dVertexBuffer*				g_GeoSphereVB = NULL;
static r3dIndexBuffer*				g_GeoSphereIB = NULL;

static r3dVertexBuffer*				g_ChamferBoxVB = NULL;
static r3dIndexBuffer*				g_ChamferBoxIB = NULL;

static r3dVertexBuffer*				g_ConeVB = NULL;
static r3dIndexBuffer*				g_ConeIB = NULL;
static r3dIndexBuffer*				g_ConeLinesIB = NULL;

static IDirect3DVertexDeclaration9*	g_PositionVertexDeclaration = NULL;
static IDirect3DVertexDeclaration9*	g_PositionNormalVertexDeclaration = NULL;

static r3dVertexBuffer *			g_ScreenSpaceQuadsVB		= 0;
IDirect3DVertexDeclaration9*		g_ScreenSpaceQuadVertexDecl	= NULL;

static ID3DXMesh*					g_SolidBoxMesh;


int	g_FwdColorVS;
int	g_FwdColorPS;
int g_FwdDebugColorVS;
int g_FwdDebugColorPS;

int g_FwdColorTextureAndAlphaPS;

void r3dScreenTo3D(float scr_x, float scr_y, r3dVector *out_v) 
{
  D3DXMATRIXA16 matProj = r3dRenderer->ProjMatrix ;

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  ( ( ( 2.0f * scr_x ) / r3dRenderer->ScreenW) - 1 ) / matProj._11;
  v.y = -( ( ( 2.0f * scr_y ) / r3dRenderer->ScreenH) - 1 ) / matProj._22;
  v.z =  1.0f;

  // Get the inverse view matrix
  const D3DXMATRIX& m = r3dRenderer->InvViewMatrix;

  // Transform the screen space pick ray into 3D space
  out_v->x  = (v.x*m._11 + v.y*m._21 + v.z*m._31);
  out_v->y  = (v.x*m._12 + v.y*m._22 + v.z*m._32);
  out_v->z  = (v.x*m._13 + v.y*m._23 + v.z*m._33);
  out_v->Normalize();
  /*
  vPickRayOrig.x = m._41;
  vPickRayOrig.y = m._42;
  vPickRayOrig.z = m._43;
  */

  return;
}

void r3dScreenPosTo3DPos( float scr_x, float scr_y, r3dVector *out_p ) 
{
	D3DXMATRIX matProj = r3dRenderer->ProjMatrix ;

	const D3DXMATRIX& m = r3dRenderer->InvViewMatrix ;

	D3DXVECTOR4 vec;
	vec.x = +( 2.0f * scr_x / r3dRenderer->ScreenW - 1 );
	vec.y = -( 2.0f * scr_y / r3dRenderer->ScreenH - 1 );
	vec.z = 0;
	vec.w = 1;

	D3DXVec4Transform( &vec, &vec, &m );

	out_p->x = vec.x / vec.w;
	out_p->y = vec.y / vec.w;
	out_p->z = vec.z / vec.w;	

	return;
}


int r3dProjectToScreen(const r3dPoint3D& pos, r3dPoint3D* scrCoord)
{
  D3DVIEWPORT9 vp;
  r3dRenderer->GetViewport(&vp);
  
  D3DXMATRIX mWorld;
  D3DXMatrixIdentity(&mWorld);

  D3DXVECTOR3 p1(pos.x, pos.y , pos.z);
  D3DXVECTOR3 p2;
  D3DXVec3Project(&p2, &p1, &vp, &r3dRenderer->ProjMatrix, &r3dRenderer->ViewMatrix, &mWorld);
  
  if(scrCoord)
  {
	  scrCoord->x = p2.x;
	  scrCoord->y = p2.y;
	  scrCoord->z = p2.z;
  }
  
  if(p2.z < 0 || p2.z >= 1)
    return 0;
    
  if(p2.x < 0 || p2.x > r3dRenderer->ScreenW)
    return 0;
    
  if(p2.y < 0 || p2.y > r3dRenderer->ScreenH)
    return 0;
    
  return 1;    
}

int r3dProjectToScreenAlways(const r3dPoint3D& pos, r3dPoint3D* scrCoord, int offsetXpixels, int offsetYpixels)
{
	r3dProjectToScreen(pos, scrCoord);

	scrCoord->x = R3D_MAX((float)offsetXpixels, scrCoord->x);
	scrCoord->x = R3D_MIN((float)r3dRenderer->ViewW-offsetXpixels, scrCoord->x);

	scrCoord->y = R3D_MAX((float)offsetYpixels, scrCoord->y);
	scrCoord->y = R3D_MIN((float)r3dRenderer->ViewH-offsetYpixels, scrCoord->y);

	// check if icon is behind the camera
	if(scrCoord->z >= 1.0f || scrCoord->z < 0)
	{
		if( scrCoord->x > (r3dRenderer->ViewW/2))
			scrCoord->x = (float)offsetXpixels;
		else
			scrCoord->x = r3dRenderer->ViewW-offsetYpixels;
		scrCoord->y = r3dRenderer->ViewH - scrCoord->y;
	}	

	return 1;
}


void BBox_Grow(const r3dPoint3D& PrevPosition, const r3dPoint3D& Position, r3dBoundBox &OrgBox, r3dBoundBox &b)
{
	float	d;		// delta of coordinate movement

#define CALC_AXIS(n) 					\
  d = Position.##n - PrevPosition.##n;			\
  if(d > 0.0f) {					\
    b.Org.##n  = OrgBox.Org.##n;			\
    b.Size.##n = OrgBox.Size.##n + d;			\
  }                                                    	\
  else {                                                \
    b.Org.##n  = OrgBox.Org.##n  + d;      		\
    b.Size.##n = OrgBox.Size.##n - d;			\
  }

  CALC_AXIS( X );
  CALC_AXIS( Y );
  CALC_AXIS( Z );

#undef CALC_AXIS
}


void GetRotationMatrixFromVectorFast(const r3dPoint3D& vOrient, D3DXMATRIX *mOut)
{
  D3DXMATRIX &m = *mOut;

  r3dPoint3D vF, vU, vR;
  vF = vOrient.NormalizeTo();
  
  vU.Assign(0, 1, 0);
  vR = vU.Cross(vF).NormalizeTo();
  vU = vF.Cross(vR).NormalizeTo();

  m._11 = vR.x; m._12 = vR.y; m._13 = vR.z; m._14 = 0;
  m._21 = vU.x; m._22 = vU.y; m._23 = vU.z; m._24 = 0;
  m._31 = vF.x; m._32 = vF.y; m._33 = vF.z; m._34 = 0;
  m._41 = 0;    m._42 = 0;    m._43 = 0;    m._44 = 1;
}

void GetRotationMatrixFromVector(const r3dPoint3D& vOrient, D3DXMATRIX *mOut, float fBanking)
{
  D3DXMATRIX &m = *mOut;
  
  r3dPoint3D vF, vU, vR;
  vF = vOrient.NormalizeTo();
  
  if(vF.Y > 0.99f) {
    vU.Assign(0, 0, -1);
    vR.Assign(1, 0, 0);
    vF.Assign(0, 1, 0);
  } else if(vF.Y < -0.99f) {
    vU.Assign(0, 0, 1);
    vR.Assign(1, 0, 0);
    vF.Assign(0, -1, 0);
  } else {
    vU.Assign(0, 1, 0);
    vR = vU.Cross(vF).NormalizeTo();
    vU = vF.Cross(vR).NormalizeTo();
  }
  
  if(fabs(fBanking) > 0.001f) {
    vR.RotateAroundVector(vF, fBanking);
    vU.RotateAroundVector(vF, fBanking);
  }

  m._11 = vR.x; m._12 = vR.y; m._13 = vR.z; m._14 = 0;
  m._21 = vU.x; m._22 = vU.y; m._23 = vU.z; m._24 = 0;
  m._31 = vF.x; m._32 = vF.y; m._33 = vF.z; m._34 = 0;
  m._41 = 0;    m._42 = 0;    m._43 = 0;    m._44 = 1;
}

r3dPoint3D r3dEulerFromVector(const r3dPoint3D& vVision)
{
  const r3dPoint3D& v = vVision;
  if(v.z < -0.9998f) return r3dPoint3D(0, 1800, 0);
  if(v.z > 0.9998f)  return r3dPoint3D(0, 0, 0);
  if(v.y < -0.9998f) return r3dPoint3D(0, -900, 0);
  if(v.y > 0.9998f)  return r3dPoint3D(0,  900, 0);

  float f1  = -R3D_RAD2DEG(atan2(v.x, v.z));
  float f2  = R3D_RAD2DEG(asin(v.y));
  return r3dPoint3D(f1, f2, 0);
}


float r3dGetAngle2D(const r3dPoint3D& _v1, const r3dPoint3D& _v2)
{
 r3dVector	v1(_v1.x, 0, _v1.z);
 r3dVector	v2(_v2.x, 0, _v2.z);

 v1.Normalize();
 v2.Normalize();
 float angle =  v1.DegreesWith(v2);

 v1.RotateAroundVector(r3dVector(0, 1, 0), angle);

 if(R3D_ABS(v1.X - v2.X) < 0.005 && R3D_ABS(v1.Z - v2.Z) < 0.005)
    return -angle;

 return angle; 
}

//------------------------------------------------------------------------

static bool	g_VDeclPushed;

enum UtilGeomType
{
	GEOM_NONE,
	GEOM_GEO_SPHERE,
	GEOM_CONE,
	GEOM_CHAMFERBOX,
	GEOM_SOLIDBOX
};

static UtilGeomType					g_GeomStarted;
static IDirect3DVertexDeclaration9*	g_DeclarationPrevToUtilGeom = NULL;

void r3dDrawGeoSpheresStart()
{
	r3d_assert( g_GeomStarted == GEOM_NONE );
	g_GeomStarted = GEOM_GEO_SPHERE;

	g_GeoSphereVB->Set( 0 );
	g_GeoSphereIB->Set();

	r3dPushSetPosUtilVertexDecl();
}

//------------------------------------------------------------------------

void r3dDrawGeoSphere()
{
	r3d_assert( g_GeomStarted == GEOM_GEO_SPHERE );
	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, 0, NUM_GEOSPHERE_VERTICES, 0, NUM_GEOSPHERE_INDICES / 3 );
}

//------------------------------------------------------------------------

void r3dDrawGeoSpheresEnd()
{
	r3d_assert( g_GeomStarted == GEOM_GEO_SPHERE );
	g_GeomStarted = GEOM_NONE;

	r3dPopUtilVertexDecl();
}

//------------------------------------------------------------------------

void r3dDrawConesStart()
{
	r3d_assert( g_GeomStarted == GEOM_NONE );
	g_GeomStarted = GEOM_CONE;

	g_ConeVB->Set( 0 );
	g_ConeIB->Set();

	r3dPushSetPosUtilVertexDecl();
}

//------------------------------------------------------------------------

void r3dDrawCone()
{
	r3d_assert( g_GeomStarted == GEOM_CONE );
	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, 0, NUM_CONEPRIM_VERTICES, 0, NUM_CONEPRIM_INDICES / 3 );
}

//------------------------------------------------------------------------

void r3dDrawConesEnd()
{
	r3d_assert( g_GeomStarted == GEOM_CONE );
	g_GeomStarted = GEOM_NONE;

	r3dPopUtilVertexDecl();
}

//------------------------------------------------------------------------

void r3dDrawChamferBoxStart()
{
	r3d_assert( g_GeomStarted == GEOM_NONE );
	g_GeomStarted = GEOM_CHAMFERBOX;

	g_ChamferBoxVB->Set( 0 );
	g_ChamferBoxIB->Set();

	r3dPushSetPosUtilVertexDecl();
}

//------------------------------------------------------------------------

void r3dDrawChamferBox()
{
	r3d_assert( g_GeomStarted == GEOM_CHAMFERBOX );
	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, 0, NUM_CHAMFERBOX_VERTICES, 0, NUM_CHAMFERBOX_INDICES / 3 );
}

//------------------------------------------------------------------------

void r3dDrawChamferBoxEnd()
{
	r3d_assert( g_GeomStarted == GEOM_CHAMFERBOX );
	g_GeomStarted = GEOM_NONE;

	r3dPopUtilVertexDecl();
}

//------------------------------------------------------------------------

void r3dDrawSolidBoxStart()
{
	r3d_assert( g_GeomStarted == GEOM_NONE );
	g_GeomStarted = GEOM_SOLIDBOX;


	IDirect3DVertexBuffer9* vbuff( NULL );
	D3D_V( g_SolidBoxMesh->GetVertexBuffer( &vbuff ) );

	d3dc._SetStreamSource( 0, vbuff, 0, g_SolidBoxMesh->GetNumBytesPerVertex() );
	vbuff->Release();

	IDirect3DIndexBuffer9* ibuff( NULL );
	D3D_V( g_SolidBoxMesh->GetIndexBuffer( &ibuff ) );

	d3dc._SetIndices( ibuff );
	ibuff->Release();

	r3dPushSetPosNormUtilVertexDecl();
}

//------------------------------------------------------------------------

void r3dDrawSolidBox()
{
	r3d_assert( g_GeomStarted == GEOM_SOLIDBOX );		
	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, 0, g_SolidBoxMesh->GetNumVertices(), 0, g_SolidBoxMesh->GetNumFaces() );
}

//------------------------------------------------------------------------

void r3dDrawSolidBoxEnd()
{
	r3d_assert( g_GeomStarted == GEOM_SOLIDBOX );
	g_GeomStarted = GEOM_NONE;

	r3dPopUtilVertexDecl();
}

//------------------------------------------------------------------------

void r3dSetIdentityTransform( int constant )
{
	D3DXMATRIX matrix ;
	D3DXMatrixIdentity( &matrix ) ;

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( constant, (float*)&matrix, 4 ) ) ;
}

//------------------------------------------------------------------------

void r3dSetFwdColorShaders( r3dColor colr )
{
	r3dSetFwdColorPixelShader( colr );
	r3dRenderer->SetVertexShader( g_FwdColorVS );
}

void r3dSetFwdColorPixelShader( r3dColor colr )
{
	float psConst[4] = { colr.R / 255.f, colr.G / 255.f, colr.B / 255.f, colr.A / 255.f };
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psConst, 1 );

	r3dRenderer->SetPixelShader( g_FwdColorPS );
}

//------------------------------------------------------------------------

void r3dSetColorAndAlphaPixelShader( r3dColor colr )
{
	float psConst[4] = { colr.R / 255.f, colr.G / 255.f, colr.B / 255.f, colr.A / 255.f };
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psConst, 1 );

	r3dRenderer->SetPixelShader( g_FwdColorTextureAndAlphaPS );
}

//------------------------------------------------------------------------

static void SetUtilDecl( IDirect3DVertexDeclaration9* decl )
{
	r3d_assert( !g_VDeclPushed ) ;
	g_VDeclPushed = true ;
	g_DeclarationPrevToUtilGeom = d3dc.pDecl;
	d3dc._SetDecl( decl );

}

void r3dPushSetPosUtilVertexDecl()
{
	SetUtilDecl( g_PositionVertexDeclaration );
}

void r3dPushSetPosNormUtilVertexDecl()
{
	SetUtilDecl( g_PositionNormalVertexDeclaration );
}

//------------------------------------------------------------------------

void r3dPopUtilVertexDecl()
{
	r3d_assert( g_VDeclPushed ) ;
	g_VDeclPushed = false ;
	d3dc._SetDecl( g_DeclarationPrevToUtilGeom );
}


//------------------------------------------------------------------------

void r3dDrawConeLines( r3dColor colr )
{
	IDirect3DVertexDeclaration9* prevDecl;

	//D3D_V( r3dRenderer->pd3ddev->GetVertexDeclaration( &prevDecl ) );
	prevDecl = d3dc.pDecl;
	d3dc._SetDecl( g_PositionVertexDeclaration );

	g_ConeVB->Set( 0 );
	g_ConeLinesIB->Set();

	r3dSetFwdColorShaders( colr );

	r3dRenderer->DrawIndexed( D3DPT_LINELIST, 0, 0, NUM_CONEPRIM_VERTICES, 0, NUM_CONELINES_INDICES / 2 );

	d3dc._SetDecl( prevDecl );
}

//------------------------------------------------------------------------

void r3dUtilInit()
{
	g_GeoSphereVB = gfx_new r3dVertexBuffer( NUM_GEOSPHERE_VERTICES, sizeof g_GeoSphereVertices[ 0 ] );
	g_GeoSphereIB = gfx_new r3dIndexBuffer( NUM_GEOSPHERE_INDICES );

	g_ChamferBoxVB = gfx_new r3dVertexBuffer( NUM_CHAMFERBOX_VERTICES, sizeof g_ChamferBoxVertices[ 0 ] );
	g_ChamferBoxIB = gfx_new r3dIndexBuffer( NUM_CHAMFERBOX_INDICES );

	g_ConeVB		= gfx_new r3dVertexBuffer( NUM_CONEPRIM_INDICES, sizeof g_ConeVertices[ 0 ] );
	g_ConeIB		= gfx_new r3dIndexBuffer( NUM_CONEPRIM_INDICES );
	g_ConeLinesIB	= gfx_new r3dIndexBuffer( NUM_CONELINES_INDICES );

	// GeoSphere
	{
		// fill VB
		{
			void* ptr = g_GeoSphereVB->Lock( 0, NUM_GEOSPHERE_VERTICES );
			memcpy( ptr, g_GeoSphereVertices, sizeof g_GeoSphereVertices );
			g_GeoSphereVB->Unlock();
		}

		// fill IB
		{
			void* ptr = g_GeoSphereIB->Lock();
			memcpy( ptr, g_GeoSphereIndices, sizeof g_GeoSphereIndices );
			g_GeoSphereIB->Unlock();
		}
	}

	// Chamfer Box
	{
		// fill VB
		{
			void* ptr = g_ChamferBoxVB->Lock( 0, NUM_CHAMFERBOX_VERTICES );
			memcpy( ptr, g_ChamferBoxVertices, sizeof g_ChamferBoxVertices );
			g_ChamferBoxVB->Unlock();
		}

		// fill IB
		{
			void* ptr = g_ChamferBoxIB->Lock();
			memcpy( ptr, g_ChamferBoxIndices, sizeof g_ChamferBoxIndices );
			g_ChamferBoxIB->Unlock();
		}
	}

	// Solid Box
	{
		D3D_V( D3DXCreateBox( r3dRenderer->pd3ddev, 2.0f, 2.0f, 2.0f, &g_SolidBoxMesh, NULL ) );
	}

	// Cone
	{
		// fill VB
		{
			void* ptr = g_ConeVB->Lock( 0, NUM_CONEPRIM_VERTICES );
			memcpy( ptr, g_ConeVertices, sizeof g_ConeVertices );
			g_ConeVB->Unlock();
		}

		// fill IB
		{
			void* ptr = g_ConeIB->Lock();
			memcpy( ptr, g_ConeIndices, sizeof g_ConeIndices );
			g_ConeIB->Unlock();
		}

		// fill line IB
		{
			void* ptr = g_ConeLinesIB->Lock();
			memcpy( ptr, g_ConeLineIndices, sizeof g_ConeLineIndices );
			g_ConeLinesIB->Unlock();
		}
	}

	// VDecl
	{
		D3DVERTEXELEMENT9 VertexElements[] =
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			D3DDECL_END()
		};

		r3dDeviceTunnel::CreateVertexDeclaration( VertexElements, &g_PositionVertexDeclaration );
	}

	// VDecl
	{
		D3DVERTEXELEMENT9 VertexElements[] =
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			D3DDECL_END()
		};

		r3dDeviceTunnel::CreateVertexDeclaration( VertexElements, &g_PositionNormalVertexDeclaration );
	}

	g_FwdColorPS = r3dRenderer->GetPixelShaderIdx( "PS_FWD_COLOR" );
	g_FwdColorVS = r3dRenderer->GetVertexShaderIdx( "VS_FWD_COLOR" );

	g_FwdDebugColorPS = r3dRenderer->GetPixelShaderIdx( "PS_FWD_DEBUG_COLOR" );
	g_FwdDebugColorVS = r3dRenderer->GetVertexShaderIdx( "VS_FWD_DEBUG_COLOR" );

	g_FwdColorTextureAndAlphaPS = r3dRenderer->GetPixelShaderIdx( "PS_COLOR_AND_ALPHATEXTURE" );

	//------------------------------------------------------------------------

	D3DVERTEXELEMENT9 elems[] = 
	{
		{ 0, 0,		D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0 },
		{ 0, 4,		D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0 },
		D3DDECL_END()
	};

	// VDECL
	{
		if( g_ScreenSpaceQuadVertexDecl == NULL )
		{
			r3dDeviceTunnel::CreateVertexDeclaration( elems, &g_ScreenSpaceQuadVertexDecl );
		}
	}

	// VBuffer
	{
		short FS_QUAD[] = 
		{
			-32767	, +32767	, 0			, 0			,
			+32767	, +32767	, +32767	, 0			,
			-32767	, -32767	, 0			, +32767	,
			+32767	, -32767	, +32767	, +32767	,
		};

		short HS_QUAD[] = 
		{
			-32767	, +32767	, 0			, 0			,
			+0		, +32767	, +32767	, 0			,
			-32767	, -0		, 0			, +32767	,
			+0		, -0		, +32767	, +32767	,
		};

		//	Create screen space vertex buffers
		if (g_ScreenSpaceQuadsVB == 0)
		{
			size_t fsQuadDataSize = sizeof(FS_QUAD);
			size_t hsQuadDataSize = sizeof(HS_QUAD);
			UINT vertexSize = D3DXGetDeclVertexSize(elems, 0);
			UINT vertexCount = (fsQuadDataSize + hsQuadDataSize) / vertexSize;
			g_ScreenSpaceQuadsVB = gfx_new r3dVertexBuffer(vertexCount, vertexSize);
			char * data = reinterpret_cast<char*>(g_ScreenSpaceQuadsVB->Lock());
			//	FS_QUAD
			memcpy_s(data, g_ScreenSpaceQuadsVB->GetDataLength(), FS_QUAD, fsQuadDataSize);
			//	HS_QUAD
			memcpy_s(data + fsQuadDataSize, g_ScreenSpaceQuadsVB->GetDataLength() - fsQuadDataSize, HS_QUAD, hsQuadDataSize);

			g_ScreenSpaceQuadsVB->Unlock();
		}
	}
}

void r3dUtilClose()
{
	g_DeclarationPrevToUtilGeom = NULL;
	
	delete g_ConeIB;
	delete g_ConeVB;
	delete g_ConeLinesIB;

	delete g_GeoSphereIB;
	delete g_GeoSphereVB;

	delete g_ChamferBoxVB;
	delete g_ChamferBoxIB;

	delete g_ScreenSpaceQuadsVB;

	SAFE_RELEASE( g_SolidBoxMesh );
}

void r3dDrawSetFullScreenQuadVB()
{
	g_ScreenSpaceQuadsVB->Set(0);
}

void r3dDrawFullScreenQuad( bool halfRes )
{
	int offset = halfRes ? 4 : 0;
	g_ScreenSpaceQuadsVB->Set(0);
	r3dRenderer->Draw(D3DPT_TRIANGLESTRIP, offset, 2);
}


void r3dGenerateOrientedBoundBoxPoints( r3dPoint3D* points, const int pointsSize, const r3dBoundBox & Box,  const r3dPoint3D &rotation ) 
{
	r3d_assert(pointsSize >= 8);
	float x = Box.Size.x/2;
	float y = Box.Size.y/2;
	float z = Box.Size.z/2;

	// DO NOT CHANGE THE ORDER OF THESE WITH OUT CHECKING ALL THE CODE CALLING THIS! 
	points[0].Assign(  x,  y,  z ); 
	points[1].Assign( -x,  y,  z );
	points[2].Assign( -x,  y, -z );
	points[3].Assign(  x,  y, -z );
	points[4].Assign(  x, -y,  z );
	points[5].Assign( -x, -y,  z );
	points[6].Assign( -x, -y, -z );
	points[7].Assign(  x, -y, -z );

	r3dMatrix Matrix;
	
	r3dBuildRotationMatrix(Matrix, rotation);

	for( int pointIndex = 0; pointIndex < 8; pointIndex++ )
	{
		points[pointIndex] *= Matrix;
		points[pointIndex] += Box.Org;
	}
}

void r3dGenerateOrientedPlanePoints(r3dPoint3D* points, const int pointsSize, const r3dBoundBox& Box, const r3dPoint3D& rotation)
{
	r3d_assert(pointsSize >= 4);
	float x = Box.Size.x/2;
	float y = Box.Size.y/2;
	float z = Box.Size.z/2;

	points[0].Assign(x,-y,z);
	points[1].Assign(x,-y,-z);
	points[2].Assign(x,y,z);
	points[3].Assign(x,y,-z);

	r3dMatrix matrix;

	r3dBuildRotationMatrix(matrix, rotation);

	for (int i = 0; i < 4; ++i)
	{
		points[i] *= matrix;
		points[i] += Box.Org;
	}
}