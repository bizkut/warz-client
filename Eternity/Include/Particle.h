#ifndef __R3D_PARTICLE_SYSTEM_H
#define __R3D_PARTICLE_SYSTEM_H

#include "TimeGradient.h"
#include "atlasDesc.h"

class r3dSingleParticle;
struct r3dSingleParticle_OverTime;
class r3dParticleEmitter;
class r3dParticleSystem;
class r3dParticleData;
struct R3D_PARTICLE_VERTEX;

extern	void	r3dParticleSystemInit();
extern	void	r3dParticleSystemClose();
extern	r3dParticleSystem* r3dParticleSystemLoad(const char* fname);
extern	r3dParticleData* r3dCacheParticleData( const char* fname );
extern	void	r3dParticleSystemStartFrame();
void SetParticleDepth();

class r3dParticleData
{
  public:
	int		NumInstances;
	char		FileName[256];

	r3dString		TextureFileName;
	r3dString		NormalTextureFileName;
	r3dString		DistortTextureFileName;

	r3dTexture*	Texture;

	AtlasDesc Atlas;

	int		ZSortEnabled;
	
	float		WarmUpTime;
	float		DepthBlendValue;
	
	float		OrgGlobalScale;

	float		EmitTime;
	int 		EmitterType;		// r3dPartEmitType_e,  0 - point, 1 - line, 2 - Mesh
	r3dMesh*	EmitMesh;
	r3dVector	EmitVector;
	float		EmitRadius;

	int			bAcceptsDynamicLights ;

	int			HasLight;

	int				LightCastsShadows;
	int				LightSSShadowBlur;

	r3dSSSBParams	SSSBParams;

	float		LightLifetime;
	float		LightIntensity;
	float		LightRadius1Base;
	float		LightRadius2Base;

	int			LightFunctionID;
	r3dTimeGradient2 LightRadius1;
	r3dTimeGradient2 LightRadius2;
	r3dTimeGradient2 LightColor;
	r3dPoint3D	ParticleLightOffset;


	int			bDeferredMeshParticlesShadows;
	int			bDeferredMeshParticles;

	int			bReceivesShadows ;

	int			bDistort;
	int			bNormalMap;
	float		fBumpPower;
	r3dTexture*	DistortTexture;
	r3dTexture*	NormalTexture;

	r3dPoint3D	OrgDirection;

	float		GlowIntensity;
	float		GlowFromDistortB_or_FromDiffuseA;

	float		CullDistance;

	enum { MAX_EMITTER_SLOTS = 16 };
	r3dParticleEmitter* PType[MAX_EMITTER_SLOTS];

  private:
	bool	m_CastsShadows;

  public:
	r3dParticleData();
	~r3dParticleData();

	void ReloadTextures();

	void	Reload ();
	void	Unload ();
	int		Load(const char* Fname);
	void	Save(const char* Fname);

	void	FixSubEmitters();

	void		UpdateCastsShadows();
	bool		CastsShadows() const ;
};

class r3dParticleSystem
{
  public:
	const r3dParticleData* PD;
	r3dLight	ParticleLight;
	
	int		NumTrisToDraw;

	int			bInheretDirectionFromSystem; // used for ray particles only right now. set from code

	r3dPoint3D	PrevPosition;
	r3dPoint3D	Position;
	r3dVector	Direction;
	float		DeflectorTop;
	float		DeflectorBottom;

	r3dBoundBox	BBox;
	float MaxBBoxSize;

	r3dPoint3D	SourceMoveDelta;

	bool IsVisible;

	float		StartTime;
	float		PrevTime;
	float		TimePassed;

	int			bEmit;
	int			bRenderUntextured;
	int			EmitersTypes;

	r3dPoint3D	BeamTargetPosition;

	float		GlobalScale;		// used with multiplying with r3dParticleData::GlobalScale
	float		WholeGlobalScale;	// GlobalScale * PD->GlobalScale
	float		GlobalRenderScale;
	float		BornScale;

	r3dSingleParticle*  Array;		// particle data
	int		ArraySize;
	int		CalculatedArraySize;	// used in editor, so it can display actual system capacity
	int		NumAliveParticles;
	int		NumAliveQuads;		// number of particle quads (counting trail particles)

	int		StartIB ;

	float	LastTimeCreated[r3dParticleData::MAX_EMITTER_SLOTS];
	int		PTypeSpawned[r3dParticleData::MAX_EMITTER_SLOTS];
	float	ToEmitWithDistance[r3dParticleData::MAX_EMITTER_SLOTS];;

  private:
	r3dParticleSystem(const r3dParticleData* data);
	friend r3dParticleSystem* r3dParticleSystemLoad(const char* fname);

	void		Init(float CurTime);
	void		 CreateBuffers();

	r3dPoint3D	GetEmitterPosition(const r3dPoint3D& center, const r3dParticleEmitter& PE);
	int			AddType(int EmitterIdx, float CurTime, const r3dPoint3D& BPosition, bool bDistanceBased, bool bUpdate, int iCurLod);
	void		CreateParticle(r3dSingleParticle* P, const r3dParticleEmitter& PE, int EmitterIdx, float CurTime, const r3dPoint3D& BPosition);
	void		ProcessParticle(r3dSingleParticle* P, float CurTime);
	void		 GetOverTimeParams(const r3dSingleParticle* P, r3dSingleParticle_OverTime* pot);

	template <class T>
	void		FillBuffers(const r3dCamera& Cam);

	void FillBuffersNew( const r3dCamera& cam, int bOnlyShadowCasters );
	static void DoFillBuffers( void* Data, size_t Start, size_t Count );

	template <class T>
	void		PrepareCommon(const r3dPoint3D* corners, T* V, float BlendMode, const r3dColor24& Col, float UVScroll, float Frame, float Frame1, float Trans, const r3dCamera& Cam );
	template <class T>
	void 		PrepareVerticesCamera(r3dPoint3D* corners, T* V, const r3dCamera &Cam, const r3dPoint3D& Pos, float Size, float rot);
	template <class T>
	void		PrepareVerticesArbitrary(r3dPoint3D* corners, T* V, const r3dPoint3D& Pos, float Size, const D3DXMATRIX* mr);
	template <class T>
	void		PrepareVerticesRay(r3dPoint3D* corners, T* V, const r3dCamera& Cam, const r3dPoint3D& Pos, const r3dPoint3D& PrevPos, float SizeW, float SizeH);
	template <class T>
	void		PrepareVerticesBeam(r3dPoint3D* corners, T* V, const r3dCamera& Cam, const r3dPoint3D& Pos, const r3dPoint3D& nextPos, float SizeW, float SizeH);
	template <class T>
	void		PrepareVerticesTrail(r3dPoint3D* corners, T* V, const r3dCamera& Cam, const r3dPoint3D& Pos, const r3dPoint3D& PrevPos, float SizeW, float SizeH);
	
	void		DrawMeshParticles();

  public:
	~r3dParticleSystem();
	void		Restart(float CurTime);
	void		Update(float CurTime, bool bUpdate=true);

	void		Draw( const r3dCamera &cam, bool bShadowMap );

	void		DrawDefferedMeshes(const r3dCamera &Cam, bool bShadowMap);

	void		ClearParticlesOfType( BYTE type );

	void		TurnLightOff();
	void		TurnLightOn();
};

void r3dSetParticlesDepthRef( r3dScreenBuffer* FullSizeBuffer, r3dScreenBuffer* HalfSizeBuffer );

void r3dSetParticleVars ( r3dVector vSunDir, r3dScreenBuffer* shadowMap, r3dScreenBuffer* distortBuf, const D3DXMATRIX& lightMtx ) ;

void r3dTransparentSetDistort( int bEnable ) ;
void r3dTransparentDistortFlush();

struct ParticlePShaderId
{
	union
	{
		struct
		{
			unsigned normal : 1 ;
			unsigned distort_tex : 1 ;
			unsigned distort_buf : 1 ;
			unsigned half_res : 1 ;
			unsigned shadow_type : 2 ;
		};

		unsigned Id ;
	};

	ParticlePShaderId()
	{
		Id = 0 ;
	}

	void ToString( char* str ) ;
	void FillMacros( ShaderMacros& defines ) ;
};

typedef r3dTL::TFixedArray< int, 64 > ParticlePSIds ;

extern ParticlePSIds gParticlePSIds ;

#endif	// __R3D_PARTICLE_SYSTEM_H
