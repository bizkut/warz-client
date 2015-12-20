#ifndef __AMMO_H__
#define __AMMO_H__

#ifdef WO_SERVER
  #error "client ammo.h included in SERVER"
#endif

#include "GameCommon.h"
#include "WeaponConfig.h"

// data driven class for different type of ammo
// shouldn't have any intermidate results inside of class, as same ammo can be used by different weapons, owned by different characters!
class Ammo
{
	friend class WeaponArmory;
	friend class Weapon;
public:
	Ammo(const char* ammoName);
	~Ammo();

	GameObject* Fire(const r3dPoint3D& hitPos, const r3dPoint3D& muzzlerPos, const D3DMATRIX& weaponBone, GameObject* owner, const WeaponConfig* weapon, float added_delay=0);

	r3dMesh* getModel(); // may be null
	void unloadModel();

	const char* getName() const ;

	int getModelRefCount() ;

	const char* getShellExtractParticleName() const { return m_ShellExtractParticle; }

	const char* getBulletClass() const { return m_BulletClass; }

	const char* getDecalSource() const { return m_DecalSource; }
	const char* getParticleTracer() const { return m_ParticleTracer; }
private:
	// config
	char m_Name[32];
	r3dMesh* m_PrivateModel; // may be null
	char*	m_ModelPath;
	char*	m_ParticleTracer; // may be null
	char*	m_ShellExtractParticle; // may be null
	char* m_DecalSource;
	char* m_BulletClass;
};

class AmmoShared : public GameObject
{
	friend Ammo;
	typedef GameObject parent ;

	bool weaponMeshAquired;
public:
	Ammo*		m_Ammo;
	const WeaponConfig*	m_Weapon;
protected: // shared data
	r3dVector	m_FireDirection;
	float		m_CreationTime;
	r3dPoint3D	m_CreationPos;
	r3dPoint3D	m_AppliedVelocity;
	float		m_AddedDelay; // for grenades
	r3dPoint3D	m_MuzzlerStartPos; // muzzler position at fire event
	class obj_ParticleSystem* m_ParticleTracer;

	virtual r3dMesh* getModel()
	{
		r3dMesh* res = m_Ammo->getModel();
		if(res==0)
			res = m_Weapon->getMesh( false, false );
		return res;
	}

public:
	AmmoShared() : weaponMeshAquired(false) {}
	void	AcquireWeaponMesh();
	void	ReleaseWeaponMesh();

	virtual	BOOL	OnCreate()	OVERRIDE ;
	virtual BOOL	OnDestroy()	OVERRIDE ;

public:
	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  OVERRIDE;
};
#endif //__AMMO_H__
