#include	"r3dPCH.h"
#include	"r3d.h"
#include	"r3dLight.h"

int		r3dLight_SkipNormalCheck = 0;
extern r3dLightSystem	WorldLightSystem;

const float r3dLight::MAX_INNER_OUTER_RADIUS_RATIO = 0.99f;

r3dSSSBParams::r3dSSSBParams()
: PhysRange	( 512.f	)
, Bias		( 0.22f	)
, Sense		( 32.f	)
, Radius	( 1.25f	)
{

}

r3dSSScatterParams::r3dSSScatterParams()
: On( 0 )
, distortion ( 0.33f )
, power ( 1.f  )
, scale ( 1.f )
, ambient ( 0.f )
, translucency ( 255, 255, 255, 127 )
{

}

r3dLight::r3dLight()
	: fTime( 0 )
	, id(0xffffffff)
	, bDoubleSided( 1 )
{
  SetPosition(0, 0, 0);
  SetColor(255, 255, 255);
  SetRadius(0, 100);

  SetWidthLength( 0.f, 0.f ) ;

  Direction		= r3dPoint3D( 0.f, 0.f, 1.f );
  ZDirection	= r3dPoint3D( 1.f, 0.f, 0.f );

  Flags             = R3D_LIGHT_ON | R3D_LIGHT_AUTOREMOVE;
  Type              = R3D_OMNI_LIGHT;

  pLightSystem      = NULL;
  ProjectMap        = NULL;
  ShadowDepth		= NULL;

  m_bQualityDisabled = false;

  Intensity             	= 1.0f;
  bAffectBump           	= 0;
  bCastShadows          	= 0;
  bSSShadowBlur				= 0;
  bUseGlobalSSSBParams		= 1;
  bLocalLight           	= 0;
  bAffectSpecular       	= 1;
  bAffectLightmap       	= 1;
  bDiffuseOnly				= 0;
  bIsShadowDepthFrozen		= 0;
  bFrozenShadowDepthDirty	= 1;
  FrozenShadowDepthSize		= 128;
  SpotAngleInner        	= 45;
  SpotAngleOuter        	= 60;
  SpotAngleFalloffPow   	= 1.0f;

  LightFunction     = R3D_LIGHTFUNC_NONE;

  OccluderID        = -100;
  bGlobalLight      = 0;

  Att0      = 1;
  Att1      = 0.01f;
  Att2      = 0;

  IsLevelLight = 0;

  for (int i=0;i<16;i++) LightFunctionParams[i] = 0;

  return;
}

r3dLight::~r3dLight()
{
  if (ProjectMap)
      r3dRenderer->DeleteTexture(ProjectMap);

  if(ShadowDepth)
	  r3dRenderer->DeleteTexture(ShadowDepth);

  if(Flags & R3D_LIGHT_AUTOREMOVE && pLightSystem)
    pLightSystem->Remove(this);

  OccluderID		= -100;
}

void
r3dLight::SetWidthLength( float w, float l )
{
	Width = w ;
	Length = l ;

	ExtraRadius = sqrtf( Width * Width + Length * Length ) ;
}

int r3dLight::IsShadowFrozen() const
{
	return bIsShadowDepthFrozen || ( r_force_frozen_shadows->GetInt() && IsLevelLight );
}

void r3dLight:: ProcessLightFunction(float CurrentTime)
{
	fTime += CurrentTime;

 switch (LightFunction)
 {
  case R3D_LIGHTFUNC_NONE:
	break;

  case R3D_LIGHTFUNC_STROBE:
	break;

  case R3D_LIGHTFUNC_THUNDER:
        LightFunctionParams[4] += CurrentTime;

	if (LightFunctionParams[0])
        {
         if (LightFunctionParams[4] > LightFunctionParams[6])
         {
           LightFunctionParams[4] = 0;
           LightFunctionParams[0] = 0;
           LightFunctionParams[5] = LightFunctionParams[1] + random(int(LightFunctionParams[2]));

           TurnOff();
         }
        }
        else
         {
          if (LightFunctionParams[4] > LightFunctionParams[5])
          {
           LightFunctionParams[4] = 0;
           LightFunctionParams[0] = 1;
           TurnOn();
          }
         }
  	break;
 }


}




BOOL r3dLight::Load(r3dFile *f)
{
 char TempStr[279];
 char *Pointer;

 if(!f->IsValid()) 
   return FALSE;

 memset(TempStr,0,279);
 while(strnicmp(TempStr,"[LightBegin]",12) && !feof(f))
 {
   memset(TempStr,0,279);
   fgets(TempStr,255,f);
 }

 if(feof(f))
   return FALSE;

	float   	vRad1=100, vRad2=300;
	float		vR=255, vG=255, vB=255;
	r3dPoint3D 	vPos;

  while(1)
  {
    if(fgets(TempStr, sizeof(TempStr), f) == NULL)
      return FALSE;
    if(!strnicmp(TempStr,"[LightEnd]",10))
      break;

    if(!strnicmp(TempStr, "Name",  4))
    {
    }
    else
    if(!strnicmp(TempStr,"Position", 8))
    {
      Pointer = &TempStr[9];
      sscanf(Pointer,"%f %f %f", &vPos.X, &vPos.Y, &vPos.Z);
    }
    else
    if(!strnicmp(TempStr,"Strength",8))
    {
  //   Pointer = &TempStr[9];
  //   sscanf (Pointer,"%f",&Light);
    }
    else
    if(!strnicmp(TempStr,"Radius1",7)) 
    {
      Pointer = &TempStr[8];
      sscanf(Pointer,"%f", &vRad1);
    }
    else
    if(!strnicmp(TempStr,"Radius2",7)) 
    {
      Pointer = &TempStr[8];
      sscanf(Pointer,"%f", &vRad2);
    }
    else
    if(!strnicmp(TempStr,"Type",4)) 
    {
      int T;
      Pointer = &TempStr[5];
      sscanf(Pointer,"%d", &T);
      
      SetType(R3D_DIRECT_LIGHT);

      if(T == 1 ) SetType(R3D_OMNI_LIGHT);
      if(T == 2 ) SetType(R3D_SPOT_LIGHT);
    
    }
    else

    if (!strnicmp(TempStr,"Color",5))
    {
      Pointer = &TempStr[6];
      sscanf(Pointer,"%f %f %f", &vR, &vG, &vB);
    }
  }

  SetRadius(vRad1, vRad2);
  SetColor(vR, vG, vB);
  SetPosition(vPos);

  OccluderID		= -100;

  r3dOutToLog("3D Realty: Light R1=%d R2=%d %d %d %d\n", int(Radius1),int(Radius2), int(R), int(G), int(B));

  return 1;
}

void r3dLight::Add(r3dLightSystem &l)
{
 OccluderID		= -100;

  pLightSystem = &l;
  l.Add(this);
}


void r3dLight::SetD3DLight(int Idx, int bBright)
{ 
}

void r3dLight :: SetShaderConstants(r3dCamera &Cam)
{
	const int MAX_CONSTANTS = 10;
	const int COLOR_INDEX = 1;

	D3DXVECTOR4 vColor;
	vColor = D3DXVECTOR4( (R/255.0f), (G/255.0f), (B/255.0f), 1 );

	vColor.x = powf( vColor.x, 2.2f ) ;
	vColor.y = powf( vColor.y, 2.2f ) ;
	vColor.z = powf( vColor.z, 2.2f ) ;
	vColor = vColor * Intensity;


	// Common params
	D3DXVECTOR4 vConsts[ MAX_CONSTANTS ] = 
	{
		D3DXVECTOR4( Cam.X, Cam.Y, Cam.Z, 0 ),
		D3DXVECTOR4( vColor )
	};

	int NUM_CONSTANTS = 2;

	float invFadeDistance = 1.0f / (GetOuterRadius() - GetInnerRadius());

	switch (Type)
	{
	case R3D_DIRECT_LIGHT:
		{
			r3dColor Amb = r3dRenderer->AmbientColor;

			r3d_assert( NUM_CONSTANTS == 2 );
			// float4   vAmbientColor  : register(c2);
			D3DXVECTOR4 amb ( Amb.R/255.0f, Amb.G/255.0f, Amb.B/255.0f, 1.0f ) ;

			// ambient intensity is stored in .w
			amb.w = Amb.A/(255.0f/4.0f);

			amb.x = powf( amb.x, 2.2f )*amb.w ;
			amb.y = powf( amb.y, 2.2f )*amb.w ;
			amb.z = powf( amb.z, 2.2f )*amb.w ;
			
			vConsts[ NUM_CONSTANTS ++ ] = amb ;

		}
		break;

	case R3D_OMNI_LIGHT:
		{
			r3d_assert( NUM_CONSTANTS == 2 );

			// float4	vLightPos: register(c2);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( X, Y, Z, 0 );
			// float4  vLightDistanceParams: register(c3);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4(invFadeDistance, GetInnerRadius() * invFadeDistance, 0, 0);
		}
		break;

	case R3D_SPOT_LIGHT:
		{
			Direction.Normalize();

			float spotAngleOuter = SpotAngleOuter;
			float spotAngleInner = R3D_MIN( SpotAngleInner, SpotAngleOuter - 0.33f );

			float cosInner = cosf( R3D_DEG2RAD( spotAngleInner ) );
			float cosOuter = cosf ( R3D_DEG2RAD( spotAngleOuter ) );

			float fallofX = 1.f / ( cosInner - cosOuter );
			float fallofY =  - cosOuter * fallofX;

			r3d_assert( NUM_CONSTANTS == 2 );

			// float4	vLightPos_RcpRad	: register(c2);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( X, Y, Z, 0 );
			// float3	vLightDir			: register(c3);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( Direction.x, Direction.y, Direction.z, 0 );
			// float4	vLightFalloff		: register(c4);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( fallofX, fallofY, SpotAngleFalloffPow, 0 );
			// float4  vLightDistanceParams	: register(c5);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4(invFadeDistance, GetInnerRadius() * invFadeDistance, 0, 0);;
		}
		break;

	case R3D_TUBE_LIGHT:
		{
			r3d_assert( NUM_CONSTANTS == 2 );

			// float4  vLightDistanceParams: register(c2);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( 0.5f * 0.03125f * Radius2 * Radius2 * Radius2 * Radius2 / Length , 0.5f * Length , 0, 0 );
			
			// float4x3 mLightMtx			: register(c3);
			D3DXMATRIX lightMtx ;

			GetTubeLightMatrix( &lightMtx ) ;

			D3DXMatrixTranspose( &lightMtx, &lightMtx ) ;

			vConsts[ NUM_CONSTANTS ++ ] = *(D3DXVECTOR4*)lightMtx.m[0] ;
			vConsts[ NUM_CONSTANTS ++ ] = *(D3DXVECTOR4*)lightMtx.m[1] ;
			vConsts[ NUM_CONSTANTS ++ ] = *(D3DXVECTOR4*)lightMtx.m[2] ;

			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( X, Y, Z, 1 ) ;

		}
		break ;

	case R3D_PLANE_LIGHT:
		{
			r3d_assert( NUM_CONSTANTS == 2 );

			// float4  vLightDistanceParams: register(c2);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( -1.f / Radius2 , 1.f, 0.5f * Length, 0.5f * Width );

			// float4x3 mLightMtx			: register(c3);
			D3DXMATRIX lightMtx ;

			GetTubeLightMatrix( &lightMtx ) ;

			D3DXMatrixTranspose( &lightMtx, &lightMtx ) ;

			vConsts[ NUM_CONSTANTS ++ ] = *(D3DXVECTOR4*)lightMtx.m[0] ;
			vConsts[ NUM_CONSTANTS ++ ] = *(D3DXVECTOR4*)lightMtx.m[1] ;
			vConsts[ NUM_CONSTANTS ++ ] = *(D3DXVECTOR4*)lightMtx.m[2] ;

			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( X, Y, Z, 1 ) ;
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( bDoubleSided ? 0.f : FLT_MAX, 0.f, 0.f, 0.f );
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( 0.f, 0.f, 0.f, 0.f  );

		}
		break ;
	case R3D_VOLUME_LIGHT:
		{
			vConsts[COLOR_INDEX] = D3DXVECTOR4( (R/255.0f), (G/255.0f), (B/255.0f), Intensity );
			
			r3d_assert( NUM_CONSTANTS == 2 );
			float DepthZ = r3dRenderer->FarClip * 0.9375f;

			// float4	vLightPos: register(c2);
			vConsts[ NUM_CONSTANTS ++ ] = D3DXVECTOR4( X, Y, Z, 1.0f / DepthZ );
		}
		break;

	case R3D_PROJECTOR_LIGHT:
		r3d_assert( false && "not implemented" );

#if 0
		LightVec = D3DXVECTOR4(X,Y,Z,0);

		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 15, (float *)&LightVec,	1 );
		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 4, (float *)&LightVec,	1 );

		Direction.Normalize();
		LightVec = D3DXVECTOR4(Direction.X,Direction.Y,Direction.Z,0);

		r3dVector vTo = r3dPoint3D(X,Y,Z) + Direction *100.0f;
		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 5, (float *)&LightVec,	1 );
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 16, (float *)&LightVec,	1 );

		r3dRenderer->SetTex(ProjectMap,3);
		r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );

		SetupProjectiveTransform(D3DXVECTOR3(X,Y,Z),D3DXVECTOR3(vTo.x,vTo.y, vTo.Z), SpotAngle);
		break;
#endif

	}

	r3d_assert( NUM_CONSTANTS < MAX_CONSTANTS );

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float *)vConsts, NUM_CONSTANTS );

}






void r3dLight::RecalcBoundBox()
{
	if( Type == R3D_TUBE_LIGHT || Type == R3D_PLANE_LIGHT )
	{
		D3DXVECTOR3 p[ 8 ] = 
		{
			D3DXVECTOR3( - 1, - 1, - 1 ),
			D3DXVECTOR3( + 1, - 1, - 1 ),
			D3DXVECTOR3( - 1, + 1, - 1 ),
			D3DXVECTOR3( + 1, + 1, - 1 ),
			D3DXVECTOR3( - 1, - 1, + 1 ),
			D3DXVECTOR3( + 1, - 1, + 1 ),
			D3DXVECTOR3( - 1, + 1, + 1 ),
			D3DXVECTOR3( + 1, + 1, + 1 )
		};

		D3DXMATRIX lm ;

		GetTubeWorldMatrix( &lm, true );

		r3dPoint3D mi( FLT_MAX, FLT_MAX, FLT_MAX );
		r3dPoint3D ma( -FLT_MAX, -FLT_MAX, -FLT_MAX );

		for( int i = 0, e = R3D_ARRAYSIZE( p ); i < e; i ++ )
		{
			D3DXVECTOR3 pt ;

			D3DXVec3TransformCoord( &pt, p + i, &lm );

			mi.x = R3D_MIN( mi.x, pt.x );
			mi.y = R3D_MIN( mi.y, pt.y );
			mi.z = R3D_MIN( mi.z, pt.z );

			ma.x = R3D_MAX( ma.x, pt.x );
			ma.y = R3D_MAX( ma.y, pt.y );
			ma.z = R3D_MAX( ma.z, pt.z );			
		}

		BBox.Org = mi ;
		BBox.Size = ma - mi ;
	}
	else
	{
		BBox.Org.Assign(X - Radius2, Y - Radius2, Z - Radius2);
		BBox.Size.Assign(Radius2 * 2, Radius2 * 2, Radius2 * 2);		
	}
}


void r3dLight::SetType(int type)
{
  Type = type;

  switch(Type) {
    case R3D_DIRECT_LIGHT:
      SetRadius(1000000, 2000000);
      RecalcBoundBox();
      Flags |= R3D_LIGHT_ALWAYSVISIBLE;
      break;
  }

  return;
}

r3dColor r3dLight::GetColor() const
{
	return r3dColor( BYTE( R ), BYTE( G ), BYTE( B ) );
}

void r3dLight::SetColor(r3dColor &clr)
{
  R = _r3d_b2f[clr.R];
  G = _r3d_b2f[clr.G];
  B = _r3d_b2f[clr.B];
}


void r3dLight::SetRadius(float r1, float r2)
{
  Radius1 = r1;
  Radius2 = r2;

  Radius1 = R3D_MIN( Radius1, Radius2 * MAX_INNER_OUTER_RADIUS_RATIO ) ;

  _RTmp1  = (1.0f / (Radius2 - Radius1));
}


float r3dLight::GetVisibilityRadius() const
{
	return 0.5f * sqrtf( Length * Length + Width * Width ) + Radius2 ;
}

void r3dLight::AdjustRadius(float r1, float r2)
{
  Radius1 += r1;
  if(Radius1 < 0) Radius1 = 0;
  Radius2 += r2;
  if(Radius2 < 0) Radius2 = 0;

  Radius1 = R3D_MIN( Radius1, Radius2 * MAX_INNER_OUTER_RADIUS_RATIO ) ;

  _RTmp1  = (1.0f / (Radius2 - Radius1));
}

void r3dLight::AdjustColor(float _R, float _G, float _B)
{
  R += _R; if(R < 0) R = 0; if(R > 255) R = 255;
  G += _G; if(G < 0) G = 0; if(G > 255) G = 255;
  B += _B; if(B < 0) B = 0; if(B > 255) B = 255;
}

float r3dLight::CalcLightIntensity(const r3dPoint3D& Pos, r3dVector &Norm)
{
  switch(Type)
  {
    case R3D_OMNI_LIGHT:
    {
	r3dPoint3D	Dist;

      // check for max distance
      Dist.Z = R3D_ABS(Z - Pos.Z);
      if(Dist.Z > Radius2)
	return 0.0f;
      Dist.X = R3D_ABS(X - Pos.X);
      if(Dist.X > Radius2)
	return 0.0f;
      Dist.Y = R3D_ABS(Y - Pos.Y);
      if(Dist.Y > Radius2)
	return 0.0f;

		float	dot;

      // if it's dynamic light, skip visibility check
      if(!(Flags & R3D_LIGHT_DYNAMIC) && !r3dLight_SkipNormalCheck) {
		r3dVector 	CurNorm;
	CurNorm = (*this) - Pos;
	CurNorm.Normalize();
	if((dot = Norm.Dot(CurNorm)) <= 0)
	  return 0.0f;

        // calc light intensity
        float d = Dist.Length();
        //if(d > Radius2) return 0.05f;
        //if(d < Radius1) return 1.0f;

		float Intensity  = (1.0f - ((d - Radius1) * _RTmp1)) * dot;
		if (Intensity < 0 ) Intensity = 0;
		if (Intensity > 1 ) Intensity = 1;
        return Intensity;
      }

      // calc light intensity
      float d = Dist.Length();
      if(d > Radius2) return 0.0f;
      if(d < Radius1) return 1.0f;

      return (1.0f - ((d - Radius1) * _RTmp1));
    }

    case R3D_DIRECT_LIGHT:
    {
     r3dVector 	CurNorm;

     CurNorm = (*this) - Pos;
     CurNorm.Normalize();
     
     float dot = Norm.Dot(CurNorm);
     
     if(dot < -0.1f) 
       return 0.0f;

      if(dot < 0 && dot > -0.1f) 
      {
        return 0.07f * Intensity;
      }  

      return dot * Intensity;
    }

  }

  return 1.0f;
}


int r3dLight::IsVisible(r3dBox3D &Box, float MaxRadius)
{
 if (!IsOn()) return 0;

 RecalcBoundBox();
  
 r3dBoundBox BB;
 BB.Org.Assign(X, Y, Z);
 BB.Size.Assign(1, 1, 1);

 switch (Type )
 {
  case R3D_OMNI_LIGHT:
    if(Box.Intersect(BB)) return 1;
    else
     if(Box.Intersect(BBox))
      if ( BBox.Size.X < MaxRadius*2 ) return 1;
    break;

    default:
     return 1;
    break;
 } 

 return 0;
}

void
r3dLight::Conform()
{
	if( Type != R3D_OMNI_LIGHT )
	{
		bDiffuseOnly = 0 ;
	}
}

//------------------------------------------------------------------------

void
r3dLight::GetTubeLightMatrix( D3DXMATRIX* oLM ) const
{

	D3DXVECTOR3 v0 = Direction ;
	D3DXVECTOR3 v1 ;
	D3DXVECTOR3 v2 = ZDirection ;
	
	D3DXVec3Cross( &v1, &v2, &v0 );

	D3DXMATRIX tra ;

	D3DXMatrixTranslation( &tra, -X, -Y, -Z );

	D3DXMATRIX ro ;

	*(D3DXVECTOR3*)ro.m[0] = v0 ; ro.m[0][3] = 0 ;
	*(D3DXVECTOR3*)ro.m[1] = v1 ; ro.m[1][3] = 0 ;
	*(D3DXVECTOR3*)ro.m[2] = v2 ; ro.m[2][3] = 0 ;

	ro.m[3][0] = 0 ;
	ro.m[3][1] = 0 ;
	ro.m[3][2] = 0 ;
	ro.m[3][3] = 1 ;

	D3DXMatrixTranspose( &ro, &ro ) ;

	*oLM = tra * ro ;
}

//------------------------------------------------------------------------

void
r3dLight::GetTubeWorldMatrix( D3DXMATRIX* oWM, bool applyScale ) const
{
	D3DXMATRIX scale ;

	const float extra = 1.05f ;

	D3DXMatrixScaling( &scale, ( 0.5f * Length + Radius2 ) * extra, Radius2 * extra, Radius2 * extra );

	D3DXVECTOR3 v0 = Direction ;
	D3DXVECTOR3 v1 ;
	D3DXVECTOR3 v2 = ZDirection ;

	D3DXVec3Cross( &v1, &v2, &v0 );

	D3DXMATRIX ro_tra ;

	*(D3DXVECTOR3*)ro_tra.m[0] = v0 ; ro_tra.m[0][3] = 0 ;
	*(D3DXVECTOR3*)ro_tra.m[1] = v1 ; ro_tra.m[1][3] = 0 ;
	*(D3DXVECTOR3*)ro_tra.m[2] = v2 ; ro_tra.m[2][3] = 0 ;

	ro_tra.m[3][0] = X ;
	ro_tra.m[3][1] = Y ;
	ro_tra.m[3][2] = Z ;
	ro_tra.m[3][3] = 1 ;

	if( applyScale )
		*oWM = scale * ro_tra ;
	else
		*oWM = ro_tra ;
}

//------------------------------------------------------------------------

void
r3dLight::GetPlaneLightMatix( D3DXMATRIX* oLM ) const
{

	D3DXVECTOR3 v0 = Direction ;
	D3DXVECTOR3 v1 ;
	D3DXVECTOR3 v2 = ZDirection ;

	D3DXVec3Cross( &v1, &v2, &v0 );

	D3DXMATRIX tra ;

	D3DXMatrixTranslation( &tra, -X, -Y, -Z );

	D3DXMATRIX ro ;

	*(D3DXVECTOR3*)ro.m[0] = v0 ; ro.m[0][3] = 0 ;
	*(D3DXVECTOR3*)ro.m[1] = v1 ; ro.m[1][3] = 0 ;
	*(D3DXVECTOR3*)ro.m[2] = v2 ; ro.m[2][3] = 0 ;

	ro.m[3][0] = 0 ;
	ro.m[3][1] = 0 ;
	ro.m[3][2] = 0 ;
	ro.m[3][3] = 1 ;

	D3DXMatrixTranspose( &ro, &ro ) ;

	*oLM = tra * ro ;
}

//------------------------------------------------------------------------

void
r3dLight::GetPlaneWorldMatrix( D3DXMATRIX* oWM, bool applyScale  ) const
{
	D3DXMATRIX scale ;

	const float extra = 1.05f ;

	D3DXMatrixScaling( &scale, ( 0.5f * Length + Radius2 ) * extra, Radius2 * extra, ( 0.5f * Width + Radius2 ) * extra );

	D3DXVECTOR3 v0 = Direction ;
	D3DXVECTOR3 v1 ;
	D3DXVECTOR3 v2 = ZDirection ;

	D3DXVec3Cross( &v1, &v2, &v0 );

	D3DXMATRIX ro_tra ;

	*(D3DXVECTOR3*)ro_tra.m[0] = v0 ; ro_tra.m[0][3] = 0 ;
	*(D3DXVECTOR3*)ro_tra.m[1] = v1 ; ro_tra.m[1][3] = 0 ;
	*(D3DXVECTOR3*)ro_tra.m[2] = v2 ; ro_tra.m[2][3] = 0 ;

	ro_tra.m[3][0] = X ;
	ro_tra.m[3][1] = Y ;
	ro_tra.m[3][2] = Z ;
	ro_tra.m[3][3] = 1 ;

	if( applyScale )
		*oWM = scale * ro_tra ;
	else
		*oWM = ro_tra ;
}

//------------------------------------------------------------------------

void r3dLight::UpdateDepthMap()
{
	if( ShadowDepth && ( FrozenShadowDepthSize != ShadowDepth->GetWidth() || !IsShadowFrozen() ) )
	{
		r3dRenderer->DeleteTexture( ShadowDepth ) ;
		ShadowDepth = 0 ;
	}

	if( !ShadowDepth && IsShadowFrozen() )
	{
		ShadowDepth = r3dRenderer->AllocateTexture();

		switch( this->Type )
		{
		case R3D_OMNI_LIGHT:
			ShadowDepth->CreateCubemap( FrozenShadowDepthSize, D3DFMT_L16, 1 ) ;
			break ;
		case R3D_SPOT_LIGHT:
			ShadowDepth->Create( FrozenShadowDepthSize, FrozenShadowDepthSize, D3DFMT_L16, 1 ) ;
			break ;

		default:
			r3dOutToLog( "r3dLight::UpdateDepthMap: unsupported light type!" ) ;
		}
	}
}

//------------------------------------------------------------------------

//
//
// r3dLightSystem
//
//

r3dLightSystem::r3dLightSystem()
{
  Init();
}

r3dLightSystem::~r3dLightSystem()
{
  if(Lights.Count() > 0)
    r3dOutToLog("---- WARNING: r3dLightSystem::~r3dLightSystem called with active lights\n");
}

int r3dLightSystem::Init()
{
	Lights.Reserve(1000);
	lightsFreeSlots.Reserve(1000);
	return 1;
}

int r3dLightSystem::Destroy()
{
	for (uint32_t i = 0; i < Lights.Count(); ++i)
	{		
		r3dLight *tmp = Lights[i];
		Remove(tmp);
	}
	Lights.Clear();
	lightsFreeSlots.Clear();

	Init();

	return 1;
}

int r3dLightSystem::Add(r3dLight *l)
{
	if (!l) return 0;

	l->pLightSystem = this;
	if (lightsFreeSlots.Count() > 0)
	{
		uint32_t id = lightsFreeSlots.GetLast();
		Lights[id] = l;
		lightsFreeSlots.Resize(lightsFreeSlots.Count() - 1);
		if (l)
			l->id = id;
	}
	else
	{
		Lights.PushBack(l);
		if (l)
			l->id = Lights.Count() - 1;
	}
	return 1;
}

int r3dLightSystem::Remove(r3dLight *l)
{
  if (!l) return 0;
  if (l->id == 0xffffffff) return 0;

  l->Flags &= ~R3D_LIGHT_AUTOREMOVE;
  l->pLightSystem = NULL;

  lightsFreeSlots.PushBack(l->id);
  Lights[l->id] = 0;

  l->id = 0xffffffff;

  if (l->Flags & R3D_LIGHT_HEAP)
	delete l;

  return 1;
}

int r3dLightSystem::FillVisibleArrayWithLights()
{
	nVisibleLights = 0;
	for (uint32_t i = 0; i < Lights.Count(); ++i)
	{
		r3dLight *l = Lights[i];
		if(!l || !l->IsOn())
			continue;

		l->RecalcBoundBox();

		r3dVector VV = *l - r3dRenderer->CameraPosition;

		int bView = r3dRenderer->IsBoxInsideFrustum(l->BBox);
		if( !bView ) bView = ( VV.Length() < l->GetOuterRadius() * 1.24f );

		VisibleLights[nVisibleLights++] = l;

		if( nVisibleLights >= R3D_ARRAYSIZE( VisibleLights ) )
			break ;

	}

	return nVisibleLights;
}

int r3dLightSystem::FillVisibleArrayWithLightsInsideBox(const r3dBox3D &Box, float MaxRadius)
{
  nVisibleLights = 0;
  for (uint32_t i = 0; i < Lights.Count(); ++i)
  {
    r3dLight *l = Lights[i];
    if(!l || !l->IsOn())
      continue;

//    if(l->Flags & R3D_LIGHT_ALWAYSVISIBLE) {
//      VisibleLights[nVisibleLights++] = l;
//      continue;
//    }

    l->RecalcBoundBox();
  
    r3dBoundBox BB;
    BB.Org.Assign(l->X, l->Y, l->Z);
    BB.Size.Assign(1, 1, 1);

    r3dVector VV = *l - r3dRenderer->CameraPosition;
 
    int bView = 1;//r3dRenderer->IsBoxInsideFrustum(BB);
    bView += r3dRenderer->IsBoxInsideFrustum(l->BBox);
    if (!bView) bView = (VV.Length() < l->GetOuterRadius()*1.24f);

    if (l->IsOn())
    switch (l->GetType() )
    {
     case R3D_OMNI_LIGHT:
     //case R3D_SPOT_LIGHT:
     {
      if ( bView )
      {
        if(Box.Intersect(BB))
           VisibleLights[nVisibleLights++] = l;
        else
        {
         if(Box.Intersect(l->BBox))
         {
         if ( l->BBox.Size.X < MaxRadius*2 && (!l->bLocalLight))
            VisibleLights[nVisibleLights++] = l;
         }
        }
      }
     }
     break;

     default:
      //VisibleLights[nVisibleLights++] = l;
      break;
    } // Light type

	if( nVisibleLights >= R3D_ARRAYSIZE( VisibleLights ) )
		break ;

  }

  return nVisibleLights;
}

//////////////////////////////////////////////////////////////////////////

void r3dLightSystem::FillVisibleArray()
{
	nVisibleLights = 0;
	for (uint32_t i = 0; i < Lights.Count(); ++i)
	{
		r3dLight *l = Lights[i];
		if (!l || !l->IsOn())
			continue;

		if (r3dRenderer->IsSphereInsideFrustum(*l, l->GetOuterRadius()))
		{
			VisibleLights[nVisibleLights++] = l;

			if( nVisibleLights >= R3D_ARRAYSIZE( VisibleLights ) )
				break ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void r3dLightSystem::SortVisibleLightsByDistance()
{
	struct LightCompare
	{
		bool operator() (const r3dLight *l, const r3dLight *r)
		{
			if (!l) return true;
			if (!r) return false;
			return l->LengthSq() - l->GetOuterRadius() > r->LengthSq() - r->GetOuterRadius();
		}
	} lc;

	std::sort(&VisibleLights[0], &VisibleLights[nVisibleLights], lc);
}

//------------------------------------------------------------------------

void r3dLightSystem::SortVisibleLightsByDistanceTo( const r3dPoint3D& point )
{
	struct LightCompare
	{
		bool operator() (const r3dLight *l, const r3dLight *r)
		{
			if (!l) return true;
			if (!r) return false;
			return (*l - point).LengthSq() < (*r - point).LengthSq();
		}

		r3dPoint3D point;
	} lc;

	lc.point = point;

	std::sort(&VisibleLights[0], &VisibleLights[nVisibleLights], lc);
}

//////////////////////////////////////////////////////////////////////////

void r3dLightSystem::SetD3DLights(int bEnable)
{
}

void r3dLightSystem::CalcVisibleLightValues(const r3dPoint3D& Pos, r3dVector &Normal, float &vR, float &vG, float &vB)
{
	r3dLight	*l;
	int		i;

  for(i = 0; i < nVisibleLights; i++)
  {
	    float	koef;

    // break if colors is fully white
    if(vR + vG + vB >= 255*3)
      break;

    l = VisibleLights[i];
    koef = l->CalcLightIntensity(Pos, Normal);
    if(koef <= R3D_EPSILON)
      continue;

    vR += l->R * koef;
    vG += l->G * koef;
    vB += l->B * koef;
  }

  if(vR > 255) vR = 255;
  if(vG > 255) vG = 255;
  if(vB > 255) vB = 255;

  return;
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
void r3dLightSystem::RegenAllFrozenLightShadows()
{
	for (uint32_t i = 0; i < Lights.Count(); ++i)
	{
		r3dLight *l = Lights[i];
		if (l && l->IsShadowFrozen())
			l->bFrozenShadowDepthDirty = 1;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////

void r3dComposeLightMatrix( D3DXMATRIX& Mtx, r3dLight* l )
{
	r3d_assert( l->Type == R3D_SPOT_LIGHT ||
				l->Type == R3D_DIRECT_LIGHT );

	D3DXMATRIX w;

	D3DXVECTOR3 zaxis = l->Direction;
	D3DXVECTOR3 xaxis;
	D3DXVECTOR3 yaxis;

	D3DXVECTOR3 up( 0, 1, 0 );
	float dt = D3DXVec3Dot( &up, &zaxis );

	if( fabs( dt ) >= 0.9999999f )
	{
		up = D3DXVECTOR3( 1, 0, 0 );
		dt = D3DXVec3Dot( &up, &zaxis );

		if( fabs( dt ) >= 0.9999999f )
		{
			up = D3DXVECTOR3( 0, 0, 1 );
		}
	}

	D3DXVec3Normalize( &xaxis, D3DXVec3Cross( &xaxis, &up, &zaxis ) );
	D3DXVec3Cross( &yaxis, &zaxis, &xaxis );

	D3DXMATRIX res;

	*(D3DXVECTOR3*) res.m[0] = xaxis;
	*(D3DXVECTOR3*) res.m[1] = yaxis;
	*(D3DXVECTOR3*) res.m[2] = zaxis;

	res.m[0][3] = 0.0f;
	res.m[1][3] = 0.0f;
	res.m[2][3] = 0.0f;
	res.m[3][3] = 1.0f;

	*(D3DXVECTOR3*)res.m[3] = D3DXVECTOR3( l->x, l->y, l->z );

	Mtx = res;
}

//------------------------------------------------------------------------

void r3dGetSpotLightScaleMatrix( D3DXMATRIX& Mtx, r3dLight* l )
{
	float zScale, radius;
	r3dGetSpotLightScaleAttribs( radius, zScale, l );
	r3dGetSpotLightScaleMatrix( Mtx, radius, zScale );
}

//------------------------------------------------------------------------

void r3dGetSpotLightScaleAttribs( float& Radius, float& ZScale, r3dLight* l )
{
	r3d_assert( l->Type == R3D_SPOT_LIGHT );

	ZScale = l->GetOuterRadius();
	Radius = ZScale * tanf( R3D_DEG2RAD( R3D_MIN( l->SpotAngleOuter, 89.9f ) ) );
}

//------------------------------------------------------------------------

void r3dGetSpotLightInnerScaleAttribs( float& Radius, float& ZScale, r3dLight* l )
{
	r3d_assert( l->Type == R3D_SPOT_LIGHT );

	ZScale = l->GetOuterRadius();
	Radius = ZScale * tanf( R3D_DEG2RAD( R3D_MIN( l->SpotAngleInner, l->SpotAngleOuter - 0.33f ) ) );
}

//------------------------------------------------------------------------

EllipsoidParams::EllipsoidParams()
{
	memset( this, 0, sizeof *this );
}

//------------------------------------------------------------------------

void r3dConvertTubeLightToFocalParams( const r3dLight& l, const D3DXMATRIX& lmat, EllipsoidParams* oEP  )
{
	float len = l.GetLength() ;

	D3DXVECTOR4 p0( -len * 0.5f, 0, 0, 1 );
	D3DXVECTOR4 p1( +len * 0.5f, 0, 0, 1 );

	D3DXVec4Transform( &p0, &p0, &lmat );
	D3DXVec4Transform( &p1, &p1, &lmat );

	oEP->Focal1 = r3dPoint3D( p0.x, p0.y, p0.z ) ;
	oEP->Focal2 = r3dPoint3D( p1.x, p1.y, p1.z ) ;

	oEP->Ax2 = len + 2 * l.Radius2 ;
}

//////////////////////////////////////////////////////////////////////////

void SetLightsIfTransparent(r3dMaterial *m, const r3dBoundBox &worldBBox)
{
	extern int gNumForwardLightsForTransparent;

	if (m && m->Flags & R3D_MAT_TRANSPARENT)
	{
		WorldLightSystem.FillVisibleArrayWithLightsInsideBox(worldBBox);
		static const int maxLights = 2;
		D3DXVECTOR4 lightParams[maxLights * 3];
		ZeroMemory(&lightParams[0], sizeof(lightParams));

		gNumForwardLightsForTransparent = std::min(maxLights, WorldLightSystem.nVisibleLights);

		for (int i = 0; i < gNumForwardLightsForTransparent; ++i)
		{
			r3dLight *l = WorldLightSystem.VisibleLights[i];
			if (!l) continue;
			//	Pos
			lightParams[i * 3] = D3DXVECTOR4(l->x, l->y, l->z, 0);
			//	Color
			lightParams[i * 3 + 1] = D3DXVECTOR4(l->R / 255, l->G / 255, l->B / 255, 1);
			//	Attenuation params x = 1 / (endFade - startFade); y = startFade / (endFade - startFade); zw = unused
			float invFadeDistance = 1.0f / (l->GetOuterRadius() - l->GetInnerRadius());
			lightParams[i * 3 + 2] = D3DXVECTOR4(invFadeDistance, l->GetInnerRadius() * invFadeDistance, 0, 0);
		}
		// PointLight ptLights[NUM_POINT_LIGHTS]: register(c243);
		r3dRenderer->pd3ddev->SetVertexShaderConstantF(243, &lightParams[0].x, _countof(lightParams));
	}

}
