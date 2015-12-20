#include "r3dPCH.h"
#include "r3d.h"

#include "sun.h"

#include "../GameObjects/ObjManag.h"

#include "../SF/Console/Config.h"


r3dSun :: r3dSun()
: r3dIResource( r3dIntegrityGuardian() )
{
	bLoaded = 0;
	OcclusionQuery = NULL;
}

r3dSun :: ~r3dSun()
{
	Unload();
}

void r3dSun :: Init()
{
 if (bLoaded) Unload();

 bLoaded = 1;

 SetLocation(0, 45);

 DawnTime = 4;
 DuskTime = 21;

 SunLight.SetType(R3D_DIRECT_LIGHT);
 //SunLight.SetColor(SunColor);
 SunLight.bAffectBump = 1;
 SunLight.bCastShadows = 0;

 SunDirAngle	= 0.f;
 SunElevAngle	= 0.f;

 AngleRange = 200.f;

 SunColor = r3dColor::white;

 OcclusionQueryRequired = 0;
 IsVisible = 0;
 OcclusionVDECL = NULL;
 QueryMade = 0;
}




void r3dSun :: Unload()
{
 D3DReleaseResource();
 SAFE_RELEASE( OcclusionVDECL );

 if ( !bLoaded ) return;

 bLoaded = 0;
}

void r3dSun :: D3DCreateResource()
{
	SAFE_RELEASE( OcclusionQuery );

	if( r3dRenderer->SupportsOQ )
	{
		D3D_V( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_OCCLUSION, &OcclusionQuery ) );

		if( !OcclusionVDECL )
		{
			D3DVERTEXELEMENT9 decl[] = 
			{
				{0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
				D3DDECL_END()
			};

			D3D_V( r3dRenderer->pd3ddev->CreateVertexDeclaration( decl, &OcclusionVDECL ) );
		}

	}

	QueryMade = 0;
}

void r3dSun :: D3DReleaseResource()
{
	SAFE_RELEASE( OcclusionQuery );

	QueryMade = 0;
}

void r3dSun :: SetOcclusionQueryRequired( int isReq )
{
	OcclusionQueryRequired = isReq;
	if( !OcclusionQuery )
	{
		D3DCreateResource();
	}
}

void r3dSun :: Update()
{
	if( OcclusionQueryRequired && OcclusionQuery )
	{
		if( QueryMade )
		{
			DWORD pixels;

			if( OcclusionQuery->GetData( &pixels, sizeof(DWORD), D3DGETDATA_FLUSH ) ==  S_OK )
			{
				IsVisible = pixels;
				QueryMade = 0;
			}
		}

		if( !QueryMade )
		{
			r3dPoint3D sunPoint = r3dRenderer->CameraPosition - SunDir.NormalizeTo() * R3D_MAX( GameWorld().GetRoot()->m_HalfSize.Length() * 2.f, 16384.f );

			D3DXVECTOR4 vec( sunPoint.x, sunPoint.y, sunPoint.z, 1.f );

			D3DXVec4Transform( &vec, &vec, &r3dRenderer->ViewProjMatrix );

			float D = 1.f / 64.f;

			vec.x /= vec.w;
			vec.y /= vec.w;

			if( vec.w > 0 && 
				vec.x > -1.0f + D && vec.x < +1.0f - D &&
				vec.y > -1.0f + D && vec.y < +1.0f - D
				)
			{
				vec.z = r3dRenderer->GetClearZValue();

				if( vec.z < 0.5f )
					vec.z += FLT_EPSILON;
				else
					vec.z -= FLT_EPSILON;

				D3D_V( OcclusionQuery->Issue( D3DISSUE_BEGIN ) );

				float4 vertices[ 4 ] = {
					float4( vec.x - D, vec.y - D, vec.z, 1.0f ),
					float4( vec.x - D, vec.y + D, vec.z, 1.0f ),
					float4( vec.x + D, vec.y - D, vec.z, 1.0f ),
					float4( vec.x + D, vec.y + D, vec.z, 1.0f ),
				};

				struct SaveRestoreStates
				{

					SaveRestoreStates()
					{
						prevDecl = d3dc.pDecl;
						r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH );
						D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, 0 ) );
					}

					~SaveRestoreStates()
					{
						r3dRenderer->SetVertexDecl( prevDecl );
						r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
						r3dRenderer->SetVertexShader();
						r3dRenderer->SetPixelShader();
						D3D_V( r3dRenderer->pd3ddev->SetRenderState(	D3DRS_COLORWRITEENABLE, 
																		D3DCOLORWRITEENABLE_RED | 
																		D3DCOLORWRITEENABLE_GREEN | 
																		D3DCOLORWRITEENABLE_BLUE |
																		D3DCOLORWRITEENABLE_ALPHA ) );
					}

					IDirect3DVertexDeclaration9* prevDecl;
				} saveRestoreStates; (void)saveRestoreStates;

				D3DXMATRIX identity;
				D3DXMatrixIdentity( &identity );

				r3dSetFwdColorShaders( r3dColor::white );

				r3dRenderer->SetVertexDecl( OcclusionVDECL );

				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &identity._11, 4 ) );

				r3dRenderer->DrawUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof vertices[ 0 ] );

				D3D_V( OcclusionQuery->Issue( D3DISSUE_END ) );

				QueryMade = 1;
			}
			else
			{
				IsVisible = 0;
			}
		}
	}
}

void r3dSun :: SetLocation(float Angle1, float Angle2)
{
 if (!bLoaded) return;

 r3dVector V = r3dVector(0,0,1);

 V.RotateAroundX(-Angle2);
 V.RotateAroundY(Angle1);
 V.Normalize();
 SunDir = V;

 //SunLight.SetColor(SunColor);
 SunLight.Direction = -SunDir;
}

r3dPoint3D r3dSun :: GetSunVecAtNormalizedTime( float CurTime )
{
	float Angle = GetAngleAtNormalizedTime( CurTime );

	D3DXMATRIX rotY, rotZ, rotY2;

	D3DXMatrixRotationY( &rotY, R3D_DEG2RAD( SunDirAngle ) );
	D3DXMatrixRotationZ( &rotZ, R3D_DEG2RAD( SunElevAngle ) - float(M_PI)*0.5f );
	D3DXMatrixRotationY( &rotY2, R3D_DEG2RAD( Angle ) - float(M_PI)*0.5f );

	D3DXMATRIX compoundRot = rotY * rotZ * rotY2;

	D3DXMatrixInverse( &compoundRot, NULL, &compoundRot );

	D3DXVECTOR3 DXSunVec( 1.0f, 0, 0 );

	D3DXVec3TransformNormal( &DXSunVec, &DXSunVec, &compoundRot );

	r3dPoint3D SunVec( DXSunVec.x, DXSunVec.y, DXSunVec.z );

	SunVec.Normalize();

	return SunVec;
}

r3dPoint3D	r3dSun :: GetCurrentSunVec()
{
	return GetSunVecAtNormalizedTime( TimeToValD( Time ) );
}

float r3dSun :: TimeToValD( float Time )
{
	float ValD = (Time-DawnTime) / (DuskTime-DawnTime);
	if (ValD <0 ) ValD = 0;
	if (ValD >1 ) ValD = 1;

	return ValD;
}

void r3dSun :: SetParams(float Hour, float NewDawnTime, float NewDuskTime, float NewSunDirAngle, float NewSunElevAngle, r3dColor NewSunColor, float NewAngleRange)
{
 if (!bLoaded) return;

 Time = Hour;
 DawnTime = NewDawnTime;
 DuskTime = NewDuskTime;

 SunDirAngle = NewSunDirAngle;
 SunElevAngle = NewSunElevAngle;

 AngleRange = NewAngleRange;

 SunColor = NewSunColor;

 if (Time < 0 ) Time = 0;
 if (Time > 24 ) Time = 0;

 float ValD = TimeToValD( Time );

 //V.RotateAroundX(-Angle2);
 //V.RotateAroundY(Angle1);
 //V.Normalize();

 r3dVector SunVec = GetSunVecAtNormalizedTime( ValD );


// 	if ( d_sun_rotate->GetBool() )
// 	{
// 		float fPhase = timeGetTime() * 0.001f;
// 		SunVec.x = cosf( fPhase );
// 		SunVec.y = 0.5f;
// 		SunVec.z = sinf( fPhase );
// 		SunVec.Normalize();
// 	}


 float Mult = -1.0f;

 SunDir = SunVec*Mult;
 SunLight.Direction = SunVec*Mult;

 SunLight.SetColor(SunColor);
// r3dRenderer->AmbientColor = AmbientColorG.GetColorValue(ValD);

  #if 1
  // FOR TEST: setup external particle system sun color/direction
  extern r3dColor  gPartShadeColor;
  extern r3dVector gPartShadeDir;
  gPartShadeDir = SunLight.Direction;
  gPartShadeDir.Y = 0; gPartShadeDir.Normalize();	// in 2D

  gPartShadeColor = SunColor;
  //gPartShadeColor = r3dColor(255, 255, 0);
  #endif
}

float r3dSun :: GetAngleAtNormalizedTime( float CurTime )
{
	return (CurTime-0.5f)*AngleRange+90.f;
}


void r3dSun :: DrawSun(const r3dCamera &Cam, int bReplicate)
{
}

