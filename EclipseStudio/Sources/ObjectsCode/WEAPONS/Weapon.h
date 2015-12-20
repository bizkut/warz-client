#ifndef __WEAPON_H__
#define __WEAPON_H__

#ifdef WO_SERVER
  #error "client weapon.h included in SERVER"
#endif

#include "r3dProtect.h"

#include "..\..\GameCode\UserProfile.h"
#include "WeaponConfig.h"

class Ammo;
class GameObject;
class obj_Player;
class obj_ParticleSystem;

class Weapon
{
	friend class WeaponArmory;
	friend class obj_Player;
	enum WeaponState
	{
		WPN_EMPTY, // no more ammo
		WPN_READY, // ready to fire
		WPN_RELOADING, // reloading
	};
public:
	Weapon(obj_Player* owner, int backpackIdx, const WeaponConfig* conf, bool first_person, bool allow_async_loading, const wiWeaponAttachment* attm);
	~Weapon();
	
	void Reset();

	bool isReadyToFire(bool triggerPressed, bool scopeMode, bool* wouldFire_butNoAmmo=NULL);

	int  getNumShotsRequired(); // because we have weapons with firerate of 900+ and our fps is around 30 we need to be able to shoot more than one bullet per frame
	void Fire(const r3dPoint3D& hitPos, const D3DXMATRIX& weaponBone, bool executeWeaponFireCode, float holdingDelay=0, const r3dPoint3D& grenadeFireFrom = R3D_ZERO_VECTOR, GameObject** out_bulletObj = NULL);
	void   AdjustWeaponDurability();
	void ReloadIfNeeded();
	void Update(const D3DXMATRIX& weaponBone); // call before fire
	void Reload(int optionalAmmoSlotToUse=-1);
	void   StartReloadSequence(); // used for network players

	void OnEquip(bool isFlashlightOn); // called when player equips that weapon
	void OnUnequip(); // called when the player is changing weapons.  
	void switchFlashlight(bool isFlashlightOn);

	const WeaponAttachmentConfig* getClipConfig();
	wiInventoryItem& getPlayerItem();
	int getNumBulletsLeft()
	{
		// override client logic without modifying Var1, melee should always have one bullet left
		if(m_pConfig->category == storecat_MELEE)
			return 1;
	
		return getPlayerItem().Var1; 
	}
	int getNumClipsLeft();

	bool	isReloading() const { return m_State == WPN_RELOADING; }

	r3dMesh*	getModel( bool allow_async_loading, bool first_person ) const { return m_pConfig->getMesh( allow_async_loading, first_person ); }
	const r3dPoint3D& getMuzzleOffset() const { return m_pConfig->muzzlerOffset; }
	r3dPoint3D getMuzzlePos(const D3DXMATRIX& weaponBone) const;
	const char* getMuzzlePartilce() const { return m_pConfig->m_MuzzleParticle; }

	r3dPoint3D getShellPos(const D3DXMATRIX& weaponBone) const;
	r3dPoint3D getShellDir(const D3DXMATRIX& weaponBone) const;

	WeaponAnimTypeEnum getAnimType() const { return m_pConfig->m_AnimType; }
	
	bool isUsableItem() const { return m_pConfig->category == storecat_UsableItem; }

	uint32_t getItemID() const { return m_pConfig->m_itemID; }

	const char* getDecalSource() const;

	bool		isImmediateShooting() const;

	float	getAmmoSpeed() const;
	float	getAmmoMass() const;
	float	getAmmoDmgDecay() const { return m_pConfig->m_AmmoDecay; }
	
	float	calcDamage(float dist) const; // return 0 if weapon isn't immediate
	
	float getLastTimeFired() const { return m_lastTimeFired; }

	float getSpread() const; // returns diameter of spread at 50meter range
	float getRecoil() const; // recoil is from 0 to 100, convert it to value usable by code
	float getReloadTime() const { return m_ReloadTime; }
	float getReloadProgress() const { return (r3dGetTime()-m_lastReloadingTime); }

	STORE_CATEGORIES getCategory() const { return m_pConfig->category; }

	const char* getStoreName() const { return m_pConfig->m_StoreName; }
	const char* getStoreIcon() const { return m_pConfig->m_StoreIcon; }

	void switchFiremode();
	WeaponFiremodeEnum getFiremode() const { return (WeaponFiremodeEnum)m_firemode; }

	const WeaponConfig* getConfig() const { return m_pConfig; }

	int getTriggerPressedCounter() const { return m_triggerPressed; }

	float getRateOfFire() const; // returns delay before next fire available

	bool isLoaded() const ;

	void checkForSkeleton();
	bool isAnimated() const { return m_WeaponAnim_FPS!=NULL; }
	r3dAnimation* getAnimation() const { return m_WeaponAnim_FPS; }
	
	void OnGameEnded();

	void setWeaponAttachmentsByIDs(const uint32_t* ids);
	void setWeaponAttachments(const WeaponAttachmentConfig** wpnAttmConfigs);
	uint32_t getWeaponAttachmentID(WeaponAttachmentTypeEnum attm_type); // if any
	void getCurrentAttachments(const WeaponAttachmentConfig** attms) 
	{ 
		r3d_assert(attms);
		memcpy((void*)attms, m_Attachments, sizeof(m_Attachments)); 
	}
	void getCurrentAttachmentIDs(uint32_t* ids) 
	{
		r3d_assert(ids);
		memset(ids, 0, sizeof(uint32_t)*WPN_ATTM_MAX);
		for(int i=0; i<WPN_ATTM_MAX; ++i)
		{
			if(m_Attachments[i])
				ids[i] = m_Attachments[i]->m_itemID;
		}
	}

	r3dMesh* getWeaponAttachmentMesh(WeaponAttachmentTypeEnum attm_type, bool aim_model);
	bool	 getWeaponAttachmentPos(WeaponAttachmentTypeEnum attm_type, D3DXMATRIX& res);

	float	getWeaponScopeZoom() const;
	bool	hasScopeMode(bool isFPS) const;
	const ScopeConfig* getScopeConfig(bool isFPS) const;

	const int* getWeaponAnimID_FPS() const;

	bool hasLaserPointer(r3dPoint3D& laserPos); 

	bool m_needDelayedAction; // for grenades, to sync with animation

	void ResetMeshLoadedFlag() { m_isMeshLoaded = false; }
	void setFirstPersonMeshFlag(bool flag) { m_isFirstPerson = flag; }
private:
	// data
	const WeaponAttachmentConfig* m_Attachments[WPN_ATTM_MAX];
	r3dSec_type<const WeaponConfig*, 0xFCA5439A> m_pConfig;

	obj_Player*	m_Owner; // todo: change that to safeID!
	int		m_BackpackIdx;
	
	WeaponState	m_State;
	float m_lastReloadingTime;
	
	float m_fractionTimeLeftFromPreviousShot; // for accurate fire rate
	
	obj_ParticleSystem*	m_MuzzleParticle;
	r3dLight		m_MuzzleLight;
	obj_ParticleSystem*	m_LaserPointerParticle;
	r3dPoint3D		m_LaserHitPoint;
	obj_ParticleSystem*	m_FlashlightParticle;
	r3dLight		m_Flashlight;

	r3dSec_type<float, 0x4F2B7AD5> m_ReloadTime;

	float m_lastTimeFired;
	class obj_ParticleSystem* m_ShellExtractParticle;
	int m_firemode;
	int m_triggerPressed; // for how many shots trigger is pressed
	int m_triggerPressed2; // for empty clip sound

	r3dPoint3D m_needDelayedAction_pos;
	float m_needDelayedAction_delay;
	float m_needDelayedAction_startTime;

	bool	m_isFirstPerson;
	bool	m_isMeshLoaded;

	enum WeaponAttachmentStatsEnum
	{
		WPN_ATTM_STAT_DAMAGE=0, // %
		WPN_ATTM_STAT_RANGE, // %
		WPN_ATTM_STAT_FIRERATE, // %
		WPN_ATTM_STAT_RECOIL, // %
		WPN_ATTM_STAT_SPREAD, // %
		
		WPN_ATTM_STAT_MAX
	};
	float		m_WeaponAttachmentStats[WPN_ATTM_STAT_MAX];

	void*	m_sndReload;
	void*	m_sndNewFire;

	// animation for FPS mode
	r3dAnimation* m_WeaponAnim_FPS;
	int			  m_AnimTrack_idle;
	int			  m_AnimTrack_fire;

	// replacement anims for FPS mode, when weapon has scope attached
	int*		m_animationIds_FPS;

	int getWeaponFireSound();
	void reloadMuzzleParticle();
};

#endif //__WEAPON_H__
