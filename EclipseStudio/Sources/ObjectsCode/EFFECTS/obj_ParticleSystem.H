#ifndef __PWAR_OBJECTS_P1000_H
#define __PWAR_OBJECTS_P1000_H

#include "../../../../GameEngine/gameobjects/GameObj.h"

class r3dParticleSystem;

class obj_ParticleSystem : public GameObject
{
	DECLARE_CLASS(obj_ParticleSystem, GameObject)

public:
	r3dParticleSystem *Torch;

	enum BindType_e {
		BINDTYPE_NONE		= 0,
		BINDTYPE_POSITION	= 100,
		BINDTYPE_BONE		= 101,
	};

	BindType_e	BindType;
	gobjid_t	BindID;
	gobjid_t	BeamTargetID;
	int			bUseBone;
	char		BoneName[64];

	int			bKill;
	int			bKillDelayed; // kill only when emitter has no particles
	float		RenderScale;
	float		GlobalScale;
	int			bRender;
	bool		bKeepAlive;
	float		LightLifetime;
	float		HackedFOV;	// used for particles in FPS weapon view

	void DrawParticles( const r3dCamera& Cam );
	void DrawParticleShadows( const r3dCamera& Cam );
	void DrawMeshes( const r3dCamera& Cam, eRenderStageID DrawState );

	float		UpdateTime;

#ifndef FINAL_BUILD
	bool		DoFreese;
#endif

public:
	obj_ParticleSystem();
	~obj_ParticleSystem();

	static	void		GetParticlePath( char* oPath, const char* particleName );

	virtual	BOOL		Load(const char* name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();
	virtual	BOOL		Update();

	virtual void		AppendTransparentShadowRenderables( RenderArray & rarr, const r3dCamera& cam ) OVERRIDE ;
	virtual void		AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual	void		ReadSerializedData(pugi::xml_node& node);

	void		Reload();
	void		Restart();
	void		SetScale(const r3dBoundBox &Box);

	virtual void		OnPickerRotated();
	void		RotateParticleSystem(const D3DXMATRIX &rot);

protected:
	virtual void UpdateTransform()
	{
		// particles do not require transform, as particles are generated in world space
		D3DXMatrixIdentity(&mTransform);
		UpdateWorldBBox();
	}

};

extern INT64 RENDERABLE_EMITTER_USER_SORT_VALUE;

#endif	// __PWAR_OBJECTS_P1000_H

