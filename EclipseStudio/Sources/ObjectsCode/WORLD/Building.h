#ifndef __PWAR_OBJECT_BUILDING_H
#define __PWAR_OBJECT_BUILDING_H

#include "UI\UIimEdit.h"

class obj_ParticleSystem ;

class obj_Building : public MeshGameObject
{
	DECLARE_CLASS(obj_Building, MeshGameObject)

public:

	int				m_bAnimated;
	int				m_bGlobalAnimFolder;
	char			m_sAnimName[256];
	r3dSkeleton*	m_BindSkeleton; // //@TODO: make it shared for every same object name
	int				m_IsSkeletonShared;
	r3dAnimPool		m_AnimPool;
	r3dAnimation	m_Animation;

	r3dAnimation	m_DestroyedAnimation;
	r3dSkeleton*	m_DestroyedBindSkeleton;
	int				m_DestroyedBindSkeletonShared;
	int				m_DestroyedAnimationId;

	obj_ParticleSystem*	m_DestructionParticles;
	int				m_DestructionSoundID;

	bool			NeedDrawAnimated(const r3dCamera& Cam);
	void			DrawAnimated(const r3dCamera& Cam, bool shadow_pass);
	void			GetAnimPath( char* oPath, int isGlobal, const char* animName );
	void			ChangeAnim();
	void			ChangeDestroyedAnim();

	bool			CanBeDestroyed();
	void			DestroyBuilding();
	void			ZeroHitpoints();
	void			FixBuilding();
	
public:
	obj_Building();
	virtual	~obj_Building();

	static void	LoadSkeleton( const char* baseMeshFName, r3dSkeleton** oSkeleton, int * oIsSkeletonShared ) ;

	void				ReloadAnimations( const char* fname );

	virtual	BOOL		OnCreate();
	virtual	BOOL		Load(const char* fname);
	virtual	BOOL		Update();

	virtual GameObject *Clone ();

	virtual BOOL		GetObjStat ( char * sStr, int iLen );

	virtual	void 		SetPosition(const r3dPoint3D& pos);

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected ) OVERRIDE;
	virtual void		DrawSelected( const r3dCamera& Cam, eRenderStageID DrawState );
#endif

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendRenderablesInstanced( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam ) OVERRIDE;

	virtual void		AppendShadowRenderables( RenderArray& rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;

	virtual void		PreLoadReadSerializedData(pugi::xml_node& node);

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

	void				ReloadDestructionAnimation();

	virtual void		UpdateDestructionData() OVERRIDE;
	virtual int			IsStatic() OVERRIDE;

	virtual void		OnMeshDestroyed() OVERRIDE;

	// Return true if gameobject is eligible for instancing rendering
	virtual bool		IsInstancingEligible( const r3dCamera& Cam )OVERRIDE;
	// Return true if number of inctances is more than threshold to render utilize insdtancing 
	virtual bool		IsInstancedRendered() const OVERRIDE;
	// Increment count of instanced objects
	virtual void		SetInstancingFlag( const r3dCamera& Cam ) OVERRIDE;

protected:
	virtual void		PostCloneParamsCopy(GameObject *pNew);
};

#endif	// __PWAR_OBJECT_BUILDING_H
