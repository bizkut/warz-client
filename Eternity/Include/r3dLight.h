#ifndef	__R3D_LIGHT_H
#define	__R3D_LIGHT_H

class r3dLight;
class r3dLightSystem;

enum r3dLightTypes
{
  R3D_OMNI_LIGHT,
  R3D_DIRECT_LIGHT,		
  R3D_SPOT_LIGHT,		
  R3D_PROJECTOR_LIGHT,		
  R3D_CUBE_LIGHT,
  R3D_TUBE_LIGHT,
  R3D_PLANE_LIGHT,
  R3D_VOLUME_LIGHT
};


enum r3dLightFunction
{
  R3D_LIGHTFUNC_NONE,
  R3D_LIGHTFUNC_STROBE,
  R3D_LIGHTFUNC_THUNDER,
};


enum r3dLightFlags
{
  R3D_LIGHT_ON					= (1<<1),
  R3D_LIGHT_STATIC				= (1<<2),
  R3D_LIGHT_DYNAMIC				= (1<<3),
  R3D_LIGHT_HEAP                = (1<<4),	// light created via new and should be deleted in ::Remove()
  R3D_LIGHT_AUTOREMOVE			= (1<<5),	// on destruction, light will be removed from saved LightSystem
  R3D_LIGHT_ALWAYSVISIBLE       = (1<<6)	// light is always visible, dependless of bbox
};

struct r3dSSSBParams
{
	r3dSSSBParams();

	float	PhysRange;
	float	Bias;
	float	Sense;
	float	Radius;
};

struct r3dSSScatterParams
{
	int			On ;

	float 		distortion ;
	float 		power ;
	float 		scale ;
	float 		ambient ;

	r3dColor	translucency ;

	r3dSSScatterParams();
};

//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
//	General Light Class
//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
class r3dLight : public r3dPoint3D
{
  public:
	static const float MAX_INNER_OUTER_RADIUS_RATIO;

	enum
	{
		MAX_FORZEN_SHADOW_DEPTH_SIZE = 1024 
	};

	int			Flags;
	uint32_t	id;
	r3dBox3D	BBox;
	int			OccluderID;
	int			bGlobalLight;

	float		Intensity;	// for r3d_direct_light

	int			Type;
	float		Radius1;
	float		Radius2;

	float		_RTmp1;
	r3dLightSystem	*pLightSystem;

	// color
	float		R, G, B;
	r3dVector	Direction;
	r3dVector	ZDirection;
	float		SpotAngleInner;
	float		SpotAngleOuter;
	float		SpotAngleFalloffPow;
	float		Att0,
				Att1,
				Att2;

	float	fTime;

	int				bLocalLight;

	int				bAffectBump;
	int				bCastShadows;
	int				bSSShadowBlur;
	int				bUseGlobalSSSBParams;
	int				bAffectSpecular;
	int				bAffectLightmap;

	int				bDiffuseOnly ;
	int				bDoubleSided ;

	int				bIsShadowDepthFrozen ;
	int				bFrozenShadowDepthDirty ;
	int				FrozenShadowDepthSize ;
	int				IsLevelLight;

	r3dSSSBParams		SSSBParams;
	r3dSSScatterParams	SSScatParams;

	int		LightFunction;
	float	LightFunctionParams[16];

	r3dTexture*	ProjectMap ;
	r3dTexture*	ShadowDepth ;

	bool		m_bQualityDisabled; // if users quality is low, this flag will be set to true to disable this light

private:
	// for tube/ square lights
	float		Width ;
	float		Length ;

	float		ExtraRadius ;


  public:
			r3dLight();
			~r3dLight();

	float		GetWidth() const ;
	float		GetLength() const ;
	void		SetWidthLength( float w, float l ) ;

	int			IsShadowFrozen() const;

	BOOL 		Load(const char* fname);
	BOOL 		Load(r3dFile *f);

	// add to light system collection.
	void		Add(r3dLightSystem &l);

	void 		ProcessLightFunction(float CurrentTime);
	void 		SetD3DLight(int Idx, int bBright = 0);
	void		SetShaderConstants( r3dCamera &Cam);

	void		SetType(int type);
	int		GetType() { return Type; }

	void		SetRadius(float r1, float r2);
	void		GetRadius(float &r1, float &r2);
	float		GetInnerRadius() { return Radius1; }
	float		GetOuterRadius() const { return Radius2; }
	float		GetVisibilityRadius() const;

	void		AdjustRadius(float rd1, float rd2);
	r3dColor	GetColor() const;
	void		SetColor(r3dColor &clr);
inline	void		SetColor(float _R, float _G, float _B);
	void		AdjustColor(float _R, float _G, float _B);
inline	void		SetPosition(const r3dPoint3D& Pos);
inline	void		SetPosition(float _X, float _Y, float _Z);

	void		RecalcBoundBox();

	void 		TurnOn()	{ Flags |= R3D_LIGHT_ON; 	}
	void 		TurnOff()	{ Flags &= ~R3D_LIGHT_ON; 	}
	BOOL 		IsOn()		{ return (Flags & R3D_LIGHT_ON) && !m_bQualityDisabled;	}

	int		IsVisible(r3dBox3D &Box, float MaxRadius);

	void		Conform();

	float 		CalcLightIntensity(const r3dPoint3D& Pos, r3dVector &Norm);

	void		GetTubeLightMatrix( D3DXMATRIX* oLM ) const;
	void		GetTubeWorldMatrix( D3DXMATRIX* oWM, bool applyScale ) const;

	void		GetPlaneLightMatix( D3DXMATRIX* oLM ) const ;
	void		GetPlaneWorldMatrix( D3DXMATRIX* oWM, bool applyScale  ) const ;

	void		UpdateDepthMap() ;
};

R3D_FORCEINLINE
void r3dLight::SetPosition(const r3dPoint3D& Pos)
{
  X = Pos.X;
  Y = Pos.Y;
  Z = Pos.Z;
}

R3D_FORCEINLINE
void r3dLight::SetPosition(float _X, float _Y, float _Z)
{
  X = _X;
  Y = _Y;
  Z = _Z;
}

R3D_FORCEINLINE
void r3dLight::SetColor(float _R, float _G, float _B)
{
  R = _R;
  G = _G;
  B = _B;
}

R3D_FORCEINLINE
float r3dLight::GetWidth() const
{
	return Width ;
}

R3D_FORCEINLINE
float r3dLight::GetLength() const
{
	return Length ;
}


inline void r3dGetSpotLightScaleMatrix( D3DXMATRIX& Mtx, float radius, float zScale )
{
	D3DXMatrixScaling( &Mtx, radius, radius, zScale );
}

void r3dComposeLightMatrix( D3DXMATRIX& Mtx, r3dLight* l );
void r3dGetSpotLightScaleMatrix( D3DXMATRIX& Mtx, r3dLight* l );
void r3dGetSpotLightScaleAttribs( float& Radius, float& ZScale, r3dLight* l );
void r3dGetSpotLightInnerScaleAttribs( float& Radius, float& ZScale, r3dLight* l );

class r3dLightSystem
{
	r3dTL::TArray<uint32_t> lightsFreeSlots;
  public:
	r3dTL::TArray<r3dLight*> Lights;

	// internal array which store visible-to-boundbox lights.
	r3dLight	*VisibleLights[1024];
	int		nVisibleLights;

  public:
			r3dLightSystem();

	// NOTE: destructor will not clear Lights array - it should be done
	// by explicitly calling Destroy()
	// -calling reset can cause problem when adding static r3dLigh to 
	// -static r3dLightSystem. if r3dLight destructor called before, we messed up.
			~r3dLightSystem();

	int		Init();
	int		Destroy();

	int		Add(r3dLight *l);
	int		Remove(r3dLight *l);


	void		SetD3DLights(int bEnable = 1);

	// store all visible lights to internal array.
	// and recalc light values in that array
	int	        FillVisibleArrayWithLights();
	int	        FillVisibleArrayWithLightsInsideBox(const r3dBox3D &Box, float MaxRadius=100000);
	void		CalcVisibleLightValues(const r3dPoint3D& Pos, r3dVector &Normal, float &R, float &G, float &B);
	void		FillVisibleArray();
	void		SortVisibleLightsByDistance();
	void		SortVisibleLightsByDistanceTo( const r3dPoint3D& point );

#ifndef FINAL_BUILD
	void		RegenAllFrozenLightShadows();
#endif
};

struct EllipsoidParams
{
	r3dPoint3D	Focal1 ;
	r3dPoint3D	Focal2 ;

	float		Ax2 ;

	EllipsoidParams();
};

void r3dConvertTubeLightToFocalParams( const r3dLight& l, const D3DXMATRIX& lmat, EllipsoidParams* oEP );

R3D_FORCEINLINE float r3dGetLightSMDepthCoef( r3dLight* l )
{
	return l->IsShadowFrozen() ? 1.f / l->GetOuterRadius() : 1.f ;
}

void SetLightsIfTransparent(r3dMaterial *m, const r3dBoundBox &worldBBox);

#endif //__R3D_LIGHT_H
