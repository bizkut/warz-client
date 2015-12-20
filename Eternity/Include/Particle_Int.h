#ifndef __R3D_PARTICLE_INT_H
#define __R3D_PARTICLE_INT_H

#pragma once

#include "TimeGradient.h"

enum r3dPartEmitType_e
{
  R3D_EMITTYPE_POINT	= 0,
  R3D_EMITTYPE_LINE	= 1,
  R3D_EMITTYPE_MESH	= 2,
  R3D_EMITTYPE_RING	= 3,
  R3D_EMITTYPE_SPHERE	= 4,
};

enum r3dPartType_e
{
  R3D_PARTICLE_CAMERAQUAD	= 0,
  R3D_PARTICLE_ARBITARYQUAD	= 1,
  R3D_PARTICLE_RAY		= 2,
  R3D_PARTICLE_MESH		= 3,
  // need to skip one to remain compatible with existing .prt
  R3D_PARTICLE_TRAIL	= 5,
  R3D_PARTICLE_BEAM		= 6,
};

// A structure for our custom vertex type
#pragma pack(push)
#pragma pack(1)
struct R3D_PARTICLE_VERTEX
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	r3dPoint3D 	Pos;
	r3dColor24 	color;
	float 		tu, tv;		// The texture coordinates
	float		BlendMode;	// 0 - add, 1 - alpha
	float 		tu1, tv1;	// The texture coordinates
	float		uvmix;   

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct R3D_PARTICLE_VERTEX_BUMP
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	r3dPoint3D 	Pos;
	r3dVector   Tang;
	r3dColor24 	color;
	float 		tu, tv;		// The texture coordinates
	float		BlendMode;	// 0 - add, 1 - alpha
	float 		tu1, tv1;	// The texture coordinates
	float		uvmix;   

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
class r3dSingleParticle
{
  public:
  
  BYTE		Type;
  BYTE	   	Active;
  BYTE	   	lod;
  BYTE		StaticFrame;
  BYTE		unused0;

  float		BirthTime;
  float		LifeTime;

  r3dPoint3D	Position;
  r3dPoint3D	PrevPosition;
  r3dVector	Direction;
  float		Velocity;
  float		EmitTravelDistance;

  r3dVector	MotionRandSrc;
  r3dVector MotionRandTarg;

  float		BaseVelocity;
  float		BaseSize;
  float		BaseRotation;
  float		BaseGravity;

  float		LastSlaveSpawn;
  float		LastRandMotionChange;
  
  WORD		TrailIdx;
  WORD		unused1;
};

class r3dParticleTrailData
{
  public:
  WORD		bUsed;
  BYTE		CurPos;
  BYTE		MaxPos;
  
  enum { MAX_TRAIL_POINTS = 64 };
  r3dPoint3D	PrevPos[MAX_TRAIL_POINTS];
  r3dPoint3D	DriftDir[MAX_TRAIL_POINTS];
  float			SizeCoefs[MAX_TRAIL_POINTS];
  float			OpacityCoefs[MAX_TRAIL_POINTS];
  float			TimeBorn[MAX_TRAIL_POINTS];

  float		AccumDist;

  r3dParticleTrailData();
};

#pragma pack(pop)

class r3dParticleTrailCache
{
  public:
	enum { MAX_TRAIL_ENTRIES = 1024 };
	r3dParticleTrailData  data[MAX_TRAIL_ENTRIES];

	int	lastFree;

  public:
	r3dParticleTrailCache();
	~r3dParticleTrailCache();
	
	r3dParticleTrailData*	Acquire(int* out_idx);
	r3dParticleTrailData*	Get(int idx) {
	  r3d_assert(idx < MAX_TRAIL_ENTRIES);
	  return &data[idx];
	}

	void		Release(int idx) {
	  r3d_assert(idx < MAX_TRAIL_ENTRIES);
	  data[idx].bUsed = false;
	  lastFree = idx;
	}
};

struct r3dSingleParticle_OverTime
{
  float		Time;
  r3dColor24	Color;
  float		BlendMode;
  float		Size;
  float		Rotation;
  float		Weight;
  float		BindWeight;

  float		AngleX;
  float		AngleY;
  float		AngleZ;

  float		RayWidth;
};

class r3dParticleEmitter
{
 public:

	bool        bInited;

	char        Name[64];
	int         bActive;

	enum {DEFAULT_BLEND_MODE = 1 };
	//int		BlendMode;	// 0 - alpha, 1 - add

	int             StartFrame;

	int				PlayOnce;

	float           FrameRate;
	float           UVSpeed;

	float           StartTime;
	float           EndTime;

	int             ParticleType;
	int             bCastsShadows;

	float           RayWidth;
	int             bDirectionOriented;
	float           EmitDistance;
	r3dVector       vDirOrientedAdditRotation;

	int             bMeshUseDistortTexture;
	r3dMesh*        Mesh;
	int             bMeshDisableCulling;
	r3dString       MeshFName;

	int         bSingleParticle;
	int         bRandomDirection;
	int         bEmmiter;
	int         bEmitterDistanceSpawn;
	int         bUsedAsEmitter;
	int         bEmmiterTypeID;
	r3dVector   RandomVector;

	r3dVector   EmmiterOffset;

	float       ParticleLifeTime;
	float       ParticleBirthRate;
	float       ParticleSize;
	float       ParticleVelocity;
	float       ParticleGravity;
	float       ParticleSpin;
	r3dVector   MotionRand;
	float       MotionRandDelta;
	float       MotionRandSmooth;
	r3dPoint3D  BornPosRand;
	r3dVector   DirectionRand;

	float       ParticleLifeTimeVar;
	float       ParticleSizeVar;
	float       ParticleVelocityVar;
	float       ParticleGravityVar;
	float       ParticleSpinVar;

	float       TrailStepDist;
	float       TrailSizeCoefMin;
	float       TrailSizeCoefMax;
	float       TrailOpacityCoefMin;
	float       TrailOpacityCoefMax;
	float       TrailTaleFade;
	float       TrailTaleFadePow;
	float       TrailDrift;

// Over life
	r3dTimeGradient2 AngleXOverLife;
	r3dTimeGradient2 AngleYOverLife;
	r3dTimeGradient2 AngleZOverLife;

	r3dTimeGradient2 ColorOverLife;
	r3dTimeGradient2 OpacityOverLife;

	r3dTimeGradient2 SizeOverLife;
	r3dTimeGradient2 VelocityOverLife;
	r3dTimeGradient2 GravityOverLife;
	r3dTimeGradient2 SpinOverLife;
	r3dTimeGradient2 BindGravityOverLife;
	r3dTimeGradient2 BlendModeOverLife;
	r3dTimeGradient2 FramesOverLife;

	// given at birth size coef to create cones and stuff
	r3dTimeGradient2    BirthSizeOverLife;
	float               BirthChartsTimeLapse;

	R3D_FORCEINLINE void SetNumFrames( int NumFrames )
	{
		iNumFrames = NumFrames ;
		fNumFrames = (float)NumFrames;
	}

	R3D_FORCEINLINE int GetNumFramesI() const
	{
		return iNumFrames ;
	}

	R3D_FORCEINLINE float GetNumFramesF() const
	{
		return fNumFrames ;
	}


private:
	int		iNumFrames;
	float	fNumFrames;

	// make copy constructor and assignment operator inaccessible
	r3dParticleEmitter(const r3dParticleEmitter& rhs);
	r3dParticleEmitter& operator=(const r3dParticleEmitter& rhs);

	void		InitDefaults();
	void		Unload();

public:
	r3dParticleEmitter() 
	{
		InitDefaults(); 
	}

	~r3dParticleEmitter()
	{
		Unload();
	}

	void		Reset ();
	void		Copy(const r3dParticleEmitter* PE, const char* NewName);
	void		Load(const char* fname, const char* PartName);
	void		Save(const char* fname);
};

#endif	// __R3D_PARTICLE_INT_H

