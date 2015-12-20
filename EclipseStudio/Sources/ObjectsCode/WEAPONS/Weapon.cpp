#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "Ammo.h"
#include "Weapon.h"
#include "WeaponArmory.h"

#include "Particle.h"
#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "ObjectsCode\ai\AI_Player.H"
#include "ObjectsCode\ai\AI_PlayerAnim.H"
#include "BulletShellManager.h"
#include "..\..\multiplayer\ClientGameLogic.h"

r3dMesh* WeaponAttachmentConfig::getMesh( bool allow_async_loading, bool aim_model) const
{
	if(m_type == WPN_ATTM_RECEIVER || m_type == WPN_ATTM_STOCK || m_type == WPN_ATTM_BARREL) // stats only attachments
		return NULL;

	if(!aim_model)
	{
		if(m_Model == 0)
		{
			//r3dOutToLog("WeaponAttachmentConfig->%s\n", m_ModelPath);
			m_Model = r3dGOBAddMesh(m_ModelPath, true, false, allow_async_loading, true);
			if(m_Model==0)
			{
				r3dError("ART: failed to load mesh '%s'\n", m_ModelPath);
			}
			r3d_assert(m_Model);
		}
		return m_Model;
	}
	else if(aim_model && m_type == WPN_ATTM_UPPER_RAIL)
	{
		if(m_Model_AIM == 0)
		{
			char aim_model[512]; 
			r3dscpy(aim_model, m_ModelPath);
			int len = strlen(aim_model);
			r3dscpy(&aim_model[len-4], "_AIM.sco");

			m_Model_AIM = r3dGOBAddMesh(aim_model, true, false, allow_async_loading, true);
			if(m_Model_AIM==0)
			{
				r3dError("ART: failed to load mesh '%s'\n", aim_model);
			}
			r3d_assert(m_Model_AIM);
		}
		return m_Model_AIM;
	}

	return NULL;
}

//------------------------------------------------------------------------
int WeaponAttachmentConfig::getAimMeshRefs() const
{
	return m_Model_AIM ? m_Model_AIM->RefCount : 0 ;
}

//------------------------------------------------------------------------
r3dMesh* ModelItemConfig::getMesh() const
{
	if(m_Model == 0)
	{
		r3d_assert(m_ModelPath); // shouldn't be null, caller have to make sure that item is of cat items

		m_Model = r3dGOBAddMesh(m_ModelPath, true, false, true, true );
		if(m_Model==0)
		{
			r3dError("ART: failed to load mesh '%s'\n", m_ModelPath);
		}
		r3d_assert(m_Model);
	}

	return m_Model;
}

//------------------------------------------------------------------------

int ModelItemConfig::getMeshRefs() const
{
	return m_Model ? m_Model->RefCount : 0 ;
}

//------------------------------------------------------------------------

r3dMesh* WeaponConfig::getMesh( bool allow_async_loading, bool first_person ) const
{
	extern bool g_bEditMode;
	if(!g_bEditMode) // do not check this in editor to allow artists to test models without changing DB
		if(!IsFPS)
			first_person = false;

	if( first_person )
	{
		if(m_Model_FPS == 0)
		{

#ifndef FINAL_BUILD
			if(g_bEditMode && !r3dFileExists(m_ModelPath_1st))
			{
				char buf[128];
				sprintf(buf, "FPS model isn't available for %s", FNAME);
				MessageBox(NULL, buf, "Warning", MB_OK);
				m_Model_FPS = m_Model;
			}
			else
#endif
			{
				m_Model_FPS = r3dGOBAddMesh(m_ModelPath_1st, true, false, allow_async_loading, true );
				if(m_Model_FPS==0)
				{
					r3dError("ART: failed to load mesh '%s'\n", m_ModelPath_1st);
				}
				r3d_assert(m_Model_FPS);
			}
		}

		return m_Model_FPS;
	}

	r3d_assert( m_Model ) ;

	return m_Model;
}

int WeaponConfig::getConfigMeshRefs() const
{
	return m_ModelRefCount ;
}


void WeaponConfig::detectMuzzleOffset(bool first_person) const
{
	if(category == storecat_UsableItem)
		return;

	const r3dMesh* msh = first_person?m_Model_FPS:m_Model;
	r3d_assert(msh);

	// detect muzzle offset as center of most far min and max vertices on x axis
	r3dPoint3D m1(-9999, 9999, 9999);  // min vertex on x axis
	r3dPoint3D m2(-9999, -9999, -9999); // max vertex on x axis

	for(int i=0; i<msh->NumVertices; i++)
	{
		const r3dPoint3D v = msh->GetVertexPos( i );
		if(v.x > m1.x)
		{
			m1.x = v.x;
			m1.z = R3D_MIN(m1.z, v.z);
			m1.y = R3D_MIN(m1.y, v.y);
		}
		if(v.x > m2.x)
		{
			m2.x = v.x;
			m2.z = R3D_MAX(m2.z, v.z);
			m2.y = R3D_MAX(m2.y, v.y);
		}
	}

	r3dPoint3D offset = (m1 + m2) / 2;
	offset -= msh->getPivot();
	muzzlerOffset = offset + adjMuzzlerOffset + r3dVector(0.05f, 0, 0); // slight offset

	// for shell offset for now just use a center of a gun
	shellOffset = msh->localBBox.Center();
	shellOffset -= msh->getPivot();
}

void WeaponConfig::updateMuzzleOffset(bool first_person) const
{
	if( !muzzleOffsetDetected )
	{
		const r3dMesh* msh = first_person?m_Model_FPS:m_Model;
		if( msh && msh->IsDrawable() )
		{
			detectMuzzleOffset(first_person) ;
			muzzleOffsetDetected = true ;
		}
	}
}

void WeaponConfig::aquireMesh( bool allow_async_loading ) const
{
	// load regular mesh no matter what, as we will need it even in FPS mode
	{
		if(m_Model == 0)
		{
			m_Model = r3dGOBAddMesh(m_ModelPath, true, false, allow_async_loading, true );
			if(m_Model==0)
			{
				r3dError("ART: failed to load mesh '%s'\n", m_ModelPath);
			}
			r3d_assert(m_Model);

			r3d_assert( !m_ModelRefCount ) ;
			m_ModelRefCount = 1 ;
		}
		else
		{
			r3d_assert( !r_allow_ingame_unloading->GetInt() || m_ModelRefCount ) ;
			m_ModelRefCount ++ ;
		}
	}	
}

void WeaponConfig::releaseMesh() const
{
	if( r_allow_ingame_unloading->GetInt() )
	{
		if( m_ModelRefCount == 1 )
		{
			r3d_assert( m_Model ) ;

			r3dGOBReleaseMesh( m_Model ) ;

			m_Model = 0 ;
		}
	}

	m_ModelRefCount -- ;

	r3d_assert( m_ModelRefCount >= 0 ) ;

}

int g_WeaponBalance ;

Weapon::Weapon(obj_Player* owner, int backpackIdx, const WeaponConfig* conf, bool first_person, bool allow_async_loading, const wiWeaponAttachment* attm) : m_pConfig(conf)
,m_isFirstPerson(first_person)
,m_fractionTimeLeftFromPreviousShot(0)
,m_lastTimeFired(0)
,m_lastReloadingTime(0)
,m_State(WPN_EMPTY)
,m_triggerPressed(0)
,m_triggerPressed2(0)
,m_needDelayedAction(0)
,m_needDelayedAction_startTime(0)
,m_firemode(WPN_FRM_AUTO)
,m_sndReload(0)
,m_sndNewFire(0)
,m_WeaponAnim_FPS(NULL)
,m_AnimTrack_idle(-1)
,m_AnimTrack_fire(-1)
,m_animationIds_FPS(NULL)
,m_LaserPointerParticle(NULL)
,m_FlashlightParticle(NULL)
,m_MuzzleParticle(NULL)
,m_isMeshLoaded(false)
{
	m_Owner = owner;
	r3d_assert(m_pConfig);
	r3d_assert(m_Owner);
	if(!(m_Owner->isObjType(OBJTYPE_Human)))
		r3dError("Weapon owner must be obj_Player");
	m_BackpackIdx = backpackIdx;

	g_WeaponBalance ++ ;

	m_pConfig->aquireMesh( allow_async_loading ) ;

	// init attachments
	for(int i=0; i<WPN_ATTM_MAX; ++i)
		m_Attachments[i] = NULL;
	memset(m_WeaponAttachmentStats, 0, sizeof(m_WeaponAttachmentStats));
	
	if(attm) {
		setWeaponAttachmentsByIDs(attm->attachments);
	}

	wiInventoryItem& wi = getPlayerItem();

	// check if we need to modify starting ammo (SERVER CODE SYNC POINT)
	// set only for actual weapons, not ones in quick slot. 
	if(backpackIdx == wiCharDataFull::CHAR_LOADOUT_WEAPON1 || backpackIdx == wiCharDataFull::CHAR_LOADOUT_WEAPON2)
	{
		const WeaponAttachmentConfig* clipCfg = getClipConfig();
		if(wi.Var1 < 0 && clipCfg) 
		{
			wi.Var1 = clipCfg->m_Clipsize;
		}
		// set itemid of clip (SERVER CODE SYNC POINT)
		if(wi.Var2 < 0 && clipCfg)
		{
			wi.Var2 = clipCfg->m_itemID;
		}
		// initialize durability (SERVER_SYNC_POINT DUR)
		if(wi.Var3 < 0)
		{
			wi.Var3 = wiInventoryItem::MAX_DURABILITY;
		}
	}
	
	Reset();

	reloadMuzzleParticle();

	if(m_pConfig->m_PrimaryAmmo->getShellExtractParticleName() && strlen(m_pConfig->m_PrimaryAmmo->getShellExtractParticleName())>2)
	{
		m_ShellExtractParticle = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", m_pConfig->m_PrimaryAmmo->getShellExtractParticleName(), r3dPoint3D(0,0,0));
		if(m_ShellExtractParticle->Torch != 0)
		{
			m_ShellExtractParticle->bKeepAlive = true; // do not allow it to be destroyed
			m_ShellExtractParticle->bRender = false;
		}
		else
			m_ShellExtractParticle = 0; // it failed to load, object manager will delete it
	}
	else
		m_ShellExtractParticle = 0;
}

Weapon::~Weapon()
{
	g_WeaponBalance -- ;

	m_pConfig->releaseMesh() ;

	if(m_MuzzleParticle)
		m_MuzzleParticle->bKeepAlive = false;
	if(m_LaserPointerParticle)
		m_LaserPointerParticle->bKill = true;
	if(m_FlashlightParticle)
		m_FlashlightParticle->bKill = true;
	if(m_ShellExtractParticle)
		m_ShellExtractParticle->bKeepAlive = false;
	if(m_MuzzleLight.pLightSystem)
		WorldLightSystem.Remove(&m_MuzzleLight); 
	if(m_Flashlight.pLightSystem)
		WorldLightSystem.Remove(&m_Flashlight);
	if(m_sndNewFire)
	{
		SoundSys.KeyOff(m_sndNewFire, "trigger");
		m_sndNewFire = 0;
	}

	SAFE_DELETE(m_WeaponAnim_FPS);
}

void Weapon::reloadMuzzleParticle()
{
	// delete old one
	if(m_MuzzleParticle)
	{
		m_MuzzleParticle->bKeepAlive = false;
		m_MuzzleParticle = NULL;
	}
	if(m_MuzzleLight.pLightSystem)
		WorldLightSystem.Remove(&m_MuzzleLight);

	const char* muzzleParticle = m_pConfig->m_MuzzleParticle;
	if(m_Attachments[WPN_ATTM_MUZZLE] && m_Attachments[WPN_ATTM_MUZZLE]->m_MuzzleParticle[0]!='\0')
		muzzleParticle = m_Attachments[WPN_ATTM_MUZZLE]->m_MuzzleParticle;

	if(muzzleParticle[0]!='\0')
	{
		m_MuzzleParticle = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", muzzleParticle, r3dPoint3D(0,0,0));
		if(m_MuzzleParticle->Torch != 0)
		{
			m_MuzzleParticle->bKeepAlive = true; // do not allow it to be destroyed
			m_MuzzleParticle->bRender = false;

			// create light
			m_MuzzleLight.Assign(0,0,0);
			m_MuzzleLight.SetType(R3D_OMNI_LIGHT);
			m_MuzzleLight.SetRadius(0.4f, 11.2f);
			m_MuzzleLight.SetColor(255, 219, 204);
			m_MuzzleLight.bCastShadows = true;
			m_MuzzleLight.Intensity = 0.42f;
			m_MuzzleLight.TurnOff();
			WorldLightSystem.Add(&m_MuzzleLight); 
		}
		else
			m_MuzzleParticle = 0; // it failed to load, object manager will delete it
	}
	else
		m_MuzzleParticle = 0;
}

void Weapon::checkForSkeleton()
{
	extern bool g_bEditMode;
	if(!g_bEditMode)
		if(!m_pConfig->IsFPS)
			return;

	// should be called for FPS mode only
	if(!m_pConfig->hasFPSModel())
		return;
	if(!m_pConfig->isFPSModelSkeletal())
		return;

	// check if mesh needs skeleton and animations
	if(m_pConfig->m_Model_FPS_Skeleton==NULL)
	{
		m_pConfig->m_Model_FPS_Skeleton = game_new r3dSkeleton();
		char tmpStr[512];
		r3dscpy(tmpStr, m_pConfig->m_ModelPath);
		r3dscpy(&tmpStr[strlen(tmpStr)-4], "_FPS.skl");
		m_pConfig->m_Model_FPS_Skeleton->LoadBinary(tmpStr);

		m_pConfig->m_AnimPool_FPS = game_new r3dAnimPool();

		sprintf(tmpStr, "%s\\%s_FPS_Idle.anm", GLOBAL_ANIM_FOLDER, m_pConfig->m_AnimPrefix);
		int aid = m_pConfig->m_AnimPool_FPS->Add("Idle", tmpStr);
		sprintf(tmpStr, "%s\\%s_FPS_Fire.anm", GLOBAL_ANIM_FOLDER, m_pConfig->m_AnimPrefix);
		aid = m_pConfig->m_AnimPool_FPS->Add("Fire", tmpStr);
		sprintf(tmpStr, "%s\\%s_FPS_Reload.anm", GLOBAL_ANIM_FOLDER, m_pConfig->m_AnimPrefix);
		aid = m_pConfig->m_AnimPool_FPS->Add("Reload", tmpStr);
	}

	if(m_WeaponAnim_FPS==NULL)
	{
		m_WeaponAnim_FPS = game_new r3dAnimation();
		m_WeaponAnim_FPS->Init(m_pConfig->m_Model_FPS_Skeleton, m_pConfig->m_AnimPool_FPS);

		// start idle
		m_AnimTrack_idle = m_WeaponAnim_FPS->StartAnimation(0, ANIMFLAG_Looped, 0.0f, 0.0f, 0.0f);

		D3DXMATRIX identM; D3DXMatrixIdentity(&identM);
		m_WeaponAnim_FPS->Update(0.0f, r3dPoint3D(0, 0, 0), identM);
	}

}

void Weapon::Reset()
{
	m_fractionTimeLeftFromPreviousShot = 0;
	m_lastTimeFired = 0;

	m_lastReloadingTime = 0;
	m_State = WPN_EMPTY;
	if(getNumBulletsLeft() > 0)
		m_State = WPN_READY;

	m_triggerPressed = 0;
	m_triggerPressed2 = 0;
	m_needDelayedAction = 0;
	m_needDelayedAction_startTime = 0;

	// sergey's request: if gun doesn't have single mode, and has auto and triple, then select triple by default, otherwise use highest available
	if(!(m_pConfig->m_fireModeAvailable & WPN_FRM_SINGLE) && (m_pConfig->m_fireModeAvailable & WPN_FRM_AUTO) && (m_pConfig->m_fireModeAvailable & WPN_FRM_TRIPLE))
		m_firemode = WPN_FRM_TRIPLE;
	else if(m_pConfig->m_fireModeAvailable & WPN_FRM_AUTO)
		m_firemode = WPN_FRM_AUTO;
	else if(m_pConfig->m_fireModeAvailable & WPN_FRM_TRIPLE)
		m_firemode = WPN_FRM_TRIPLE;
	else
		m_firemode = WPN_FRM_SINGLE;

	m_sndReload = 0;

	if(m_sndNewFire)
	{
		SoundSys.KeyOff(m_sndNewFire, "trigger");
		m_sndNewFire = 0;
	}

	m_Owner->uberAnim_->StopShootAnim();

	if(m_pConfig->m_itemID == 101306) // hard coded for melee flashlight
	{
		if(m_FlashlightParticle) {
			m_FlashlightParticle->bKill = true;
			m_FlashlightParticle = NULL;
		}
		if(m_Flashlight.pLightSystem) {
			WorldLightSystem.Remove(&m_Flashlight);
		}

		{
			m_FlashlightParticle = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "weaponfx_flashlight", r3dPoint3D(0,0,0));
			if(m_FlashlightParticle->Torch != 0)
			{
				m_FlashlightParticle->bKeepAlive = true; // do not allow it to be destroyed
				m_FlashlightParticle->Torch->bInheretDirectionFromSystem = 1; // hard coded for ray particles
				m_FlashlightParticle->ObjFlags |= OBJFLAG_SkipDraw;
			}
		}

				m_Flashlight.Assign(0,0,0);
					m_Flashlight.SetType(R3D_SPOT_LIGHT);
					m_Flashlight.SetRadius(10.5f, 26.0f);
					m_Flashlight.SetColor(228, 229, 252);
					m_Flashlight.SpotAngleInner = 10.41f;
					m_Flashlight.SpotAngleOuter = 35.21f;
					m_Flashlight.bCastShadows = r_lighting_quality->GetInt() == 3 && r_shadows_quality->GetInt() >= 3;
					m_Flashlight.Intensity = 4.31f;
					m_Flashlight.SpotAngleFalloffPow = 1.6f;
					m_Flashlight.bSSShadowBlur = 1;
					m_Flashlight.bUseGlobalSSSBParams = 0;
					m_Flashlight.SSSBParams.Bias = 0.1f;
					m_Flashlight.SSSBParams.PhysRange = 43.12f;
					m_Flashlight.SSSBParams.Radius = 10.0f;
					m_Flashlight.SSSBParams.Sense = 635.18f;
					m_Flashlight.ProjectMap = r3dRenderer->LoadTexture("data\\ProjectionTextures\\flashlight_02.dds");

		m_Flashlight.TurnOff();
		WorldLightSystem.Add(&m_Flashlight); 
	}

}

float Weapon::getRateOfFire() const
{
	float fireDelay = m_pConfig->m_fireDelay;
	
	{
		int firerate = (int)ceilf(60.0f / fireDelay); // convert back to rate of fire
		firerate = (int)ceilf(float(firerate) * (1.0f+m_WeaponAttachmentStats[WPN_ATTM_STAT_FIRERATE])); // add bonus
		fireDelay = 60.0f / firerate; // convert back to fire delay
	}
	obj_Player* plr = (obj_Player*)m_Owner;
	
	// accumulated adjustment in percents for fire rate
	float fireRatePerc = 0.0f;
	
	// assault skill
	/*if(getCategory() == storecat_ASR)
	{
		if(plr->m_Skills[4] == 2)
			fireRatePerc += 4; // +2% to rate of fire
		else if(plr->m_Skills[4] == 3)
			fireRatePerc += 8; // +4% to rate of fire
		else if(plr->m_Skills[4] == 4)
			fireRatePerc += 12; // +6% to rate of fire
		else if(plr->m_Skills[4] == 5)
			fireRatePerc += 16; // +8% to rate of fire
	}*/
	/*int teamAssaultSkill = gClientLogic().CheckTeamSkillAvailable(plr, 4);
	if(teamAssaultSkill == 4)
		fireRatePerc += 4;
	else if(teamAssaultSkill == 5)
		fireRatePerc += 8;*/

	float adjInPerc = (fireDelay / 100.0f) * fireRatePerc;
	fireDelay -= adjInPerc;

	return fireDelay;
}

bool Weapon::isReadyToFire(bool triggerPressed, bool scopeMode, bool* wouldFire_butNoAmmo)
{	
	bool weaponReady = ((r3dGetTime()-m_lastTimeFired)>getRateOfFire());
	bool weaponReady2 = weaponReady;
	if(!(isUsableItem() || getCategory() == storecat_GRENADE))
	{
		weaponReady = weaponReady && !isReloading() && m_State == WPN_READY;
		weaponReady2 = weaponReady2 && !isReloading();
	}

	// reset counter only if we released fire button, fixed bugID 1231 by adjusting if(!triggerPressed) ahead of. 
	// to make *wouldFire_butNoAmmo = true, variable m_triggerPressed2 needs to be reset to 0 once trigger released to meet condition at line573.  
	if(!triggerPressed)
	{	
		m_triggerPressed = 0;
		m_triggerPressed2 = 0; 
	}
	
	if(triggerPressed || m_triggerPressed)
	{
		// do not allow fire for broken weapons
		if(getPlayerItem().Var3 == 0)
		{
			*wouldFire_butNoAmmo = true;
			return false;
		}

		// advance counter only if weapon is ready
		if(weaponReady)
			++m_triggerPressed;
		if(weaponReady2)
			++m_triggerPressed2;
		if(wouldFire_butNoAmmo && weaponReady2 && m_State == WPN_EMPTY && m_triggerPressed2 <= 1)
			*wouldFire_butNoAmmo = true;

		if(m_firemode == WPN_FRM_SINGLE && m_triggerPressed <= 1)
			return weaponReady;
		if(m_firemode == WPN_FRM_TRIPLE && m_triggerPressed <= 3)
			return weaponReady;
		if(m_firemode == WPN_FRM_AUTO && triggerPressed)
			return weaponReady;
	}
	return false;
}

void Weapon::OnEquip(bool isFlashlightOn)
{
	if(m_State == WPN_RELOADING) // if was reloading before, reset timer
		m_lastReloadingTime = r3dGetTime();

	/*if(m_pConfig->category == storecat_SUPPORT) // RPGs are always unloaded when you equip them
	{
		if(m_numBulletsLeftInClip > 0 ) // unload.
		{
			m_numBulletsLeft = m_numBulletsLeft + m_numBulletsLeftInClip;// return back ammo
			m_numBulletsLeftInClip = 0;
			Reload();
		}
	}*/

	if( m_MuzzleParticle )
	{
		m_MuzzleParticle->ObjFlags &= ~OBJFLAG_SkipDraw;
	}

	if( m_LaserPointerParticle )
	{
		m_LaserPointerParticle->ObjFlags &= ~OBJFLAG_SkipDraw;
	}

	if( m_FlashlightParticle && isFlashlightOn )
	{
		m_FlashlightParticle->ObjFlags &= ~OBJFLAG_SkipDraw;
		m_Flashlight.TurnOn();
	}

	m_triggerPressed = 0;
	m_triggerPressed2 = 0;   
}

void Weapon::switchFlashlight(bool isFlashlightOn)
{
	if(!m_FlashlightParticle)
		return;

	if(isFlashlightOn)
	{
		m_FlashlightParticle->ObjFlags &= ~OBJFLAG_SkipDraw;
		m_Flashlight.TurnOn();
	}
	else
	{
		m_FlashlightParticle->ObjFlags |= OBJFLAG_SkipDraw;
		m_Flashlight.TurnOff();
	}
	if(m_Owner->NetworkLocal)
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/NewWeapons/Melee/flashlight"), m_Owner->GetPosition());
}

void Weapon::OnUnequip()
{
	if( m_MuzzleParticle )
	{
		m_MuzzleParticle->ObjFlags |= OBJFLAG_SkipDraw;
		m_MuzzleLight.TurnOff();
	}

	if( m_LaserPointerParticle )
	{
		m_LaserPointerParticle->ObjFlags |= OBJFLAG_SkipDraw;
	}

	if( m_FlashlightParticle )
	{
		m_FlashlightParticle->ObjFlags |= OBJFLAG_SkipDraw;
		m_Flashlight.TurnOff();
	}

	if(m_sndNewFire)
	{
		SoundSys.KeyOff(m_sndNewFire, "trigger");
		m_sndNewFire = 0;
	}

	m_Owner->uberAnim_->StopShootAnim();
	m_triggerPressed = 0;
	m_triggerPressed2 = 0;
	m_needDelayedAction = false;
}


void Weapon::switchFiremode()
{
	int prevFiremode = m_firemode;
	while(1) // it should never be eternal loop, as I'm checking that weapon has at least one firing mode. so, in that case it will just loop once and exit
	{
		m_firemode = m_firemode << 1;
		m_firemode &= 0x00000007; // see WeaponFiremodeEnum, that all modes combined = 0x07
		if(m_firemode == 0)
			m_firemode = 1;

		if(m_firemode & m_pConfig->m_fireModeAvailable)
			break;
	}

	if(m_firemode != prevFiremode) // switched firemode, reset weapon
	{
		m_triggerPressed = 0;
		m_triggerPressed2 = 0;

		if(m_sndNewFire)
		{
			SoundSys.KeyOff(m_sndNewFire, "trigger");
			m_sndNewFire = 0;
		}

		m_Owner->uberAnim_->StopShootAnim();
	}
}

void Weapon::Update(const D3DXMATRIX& weaponBone)
{
	R3DPROFILE_FUNCTION("Weapon::Update");
	
	obj_Player* player = (obj_Player*)m_Owner;
	if(!m_isMeshLoaded)
	{
		m_isMeshLoaded = isLoaded();
		if(m_isMeshLoaded)
		{
			m_pConfig->updateMuzzleOffset(g_camera_mode->GetInt()==2 && player->NetworkLocal);
			if(g_camera_mode->GetInt()==2 && player->NetworkLocal)
				checkForSkeleton();
		}
	}

	r3dPoint3D muzzlerPoint = getMuzzlePos(weaponBone);

	bool stoppedFiring = (m_firemode == WPN_FRM_SINGLE && m_triggerPressed >= 1) || (m_firemode == WPN_FRM_TRIPLE && m_triggerPressed > 3) || m_triggerPressed==0 || m_State!=WPN_READY;
	if(getPlayerItem().Var3 == 0)
		stoppedFiring = true;
	
	if(m_WeaponAnim_FPS)
	{
		if(stoppedFiring && m_State!=WPN_RELOADING)
		{
			bool need_restart_idle = false;
			if(!m_WeaponAnim_FPS->GetTrack(m_AnimTrack_idle))
				need_restart_idle=true;
			else
				need_restart_idle = (m_WeaponAnim_FPS->GetTrack(m_AnimTrack_idle)->GetStatus() & ANIMSTATUS_Finished) ? true : false;

			if(m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire))
			{
				if(!(m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire)->GetStatus() & ANIMSTATUS_Finished))
				{
					need_restart_idle = false;
				}
				else
				{
					m_Owner->uberAnim_->StopShootAnim();
				}
			}

			if(need_restart_idle)
				m_AnimTrack_idle = m_WeaponAnim_FPS->StartAnimation(0, ANIMFLAG_Looped|ANIMFLAG_RemoveOtherNow, 1.0f, 1.0f, 0.0f);
		}
		else
			m_Owner->uberAnim_->updateShootAnim(false);
			
		// special hack for stopping weapon fire animation when zooming in/out
		// otherwise shoot animation interfere with blending between weapon idle/aim animations
		if(m_Owner->NetworkLocal && m_Owner->m_isChangedAiming1)
		{
			m_Owner->m_isChangedAiming1 = false;

			m_Owner->uberAnim_->StopShootAnim();
			m_WeaponAnim_FPS->Stop(m_AnimTrack_fire);
		}

		D3DXMATRIX ident; D3DXMatrixIdentity(&ident);
		m_WeaponAnim_FPS->Update(r3dGetFrameTime(), r3dPoint3D(0, 0, 0), ident);
	}
	else
	{
		m_Owner->uberAnim_->updateShootAnim(true);
	}

	m_fractionTimeLeftFromPreviousShot = 0;
	// update sound pos
	if(m_sndReload)
	{
		if(!SoundSys.SetSoundPos(m_sndReload, m_Owner->GetPosition()))
			m_sndReload = 0;
	}
	if(m_sndNewFire)
	{
		if(!m_Owner->NetworkLocal) // only for non local player
			SoundSys.SetParamValue(m_sndNewFire, "(distance)", (gCam-muzzlerPoint).Length());
		if(!SoundSys.SetSoundPos(m_sndNewFire, m_Owner->GetPosition()))
			m_sndNewFire = 0;
		else
		{
			if(stoppedFiring)
			{
				SoundSys.KeyOff(m_sndNewFire, "trigger");
				m_sndNewFire = 0;
			}
		}
	}

	if(m_State == WPN_RELOADING)
	{
		m_Owner->uberAnim_->StopShootAnim();
		if(getReloadProgress() > m_ReloadTime) // done reloading
		{
			m_triggerPressed = 0;
			m_triggerPressed2 = 0;
			m_State = WPN_READY;
		}
	}

	// special case for grenades
	if(m_needDelayedAction)
	{
		float launchDelay = player->GetGrenadeLaunchDelay();
		if((r3dGetTime()-m_needDelayedAction_startTime) > launchDelay)
		{
			m_needDelayedAction = 0;
			r3d_assert(m_pConfig->m_PrimaryAmmo);
			if(player->bDead==false)
			{
				GameObject* ammoSh = m_pConfig->m_PrimaryAmmo->Fire(m_needDelayedAction_pos, muzzlerPoint, weaponBone, m_Owner, m_pConfig, m_needDelayedAction_delay);
				if(ammoSh)
				{
					PKT_C2C_PlayerThrewGrenade_s n; 
					n.localId = ammoSh->GetSafeID();
					n.spawnID = 0;
					n.fire_from = muzzlerPoint;
					n.fire_to = m_needDelayedAction_pos;
					n.holding_delay = m_needDelayedAction_delay + launchDelay;
					n.wid = m_BackpackIdx;
					p2pSendToHost(m_Owner, &n, sizeof(n), true);

#ifndef FINAL_BUILD
					if( muzzlerPoint.Length() < 1.0f )
						r3dOutToLog("!!! BAD muzzlerPoint <%0.2f, %0.2f, %0.2f>\n", muzzlerPoint.x, muzzlerPoint.y, muzzlerPoint.z);
#endif
					// grenades are treated as items
					wiInventoryItem& wi = getPlayerItem();
					r3d_assert(wi.quantity > 0);
					wi.quantity--;
					if(wi.quantity <= 0)
						wi.Reset();

					player->GrenadeCallbackFromWeapon(wi);
					if(wi.quantity == 0) // this object might be deleted after GrenadeCallbackFromWeapon()
						return;
				}
			}
		}
	}

	if(m_MuzzleParticle)
	{
		if(player->bDead)
			m_MuzzleLight.TurnOff();
		else
		{
			m_MuzzleParticle->SetPosition(muzzlerPoint);
			if(m_MuzzleLight.IsOn())
			{
				m_MuzzleLight.Assign(muzzlerPoint);
				float light_lifetime = 0.15f;
				m_MuzzleLight.Intensity = R3D_CLAMP(1.0f-((r3dGetTime()-m_lastTimeFired)/light_lifetime), 0.0f, 1.0f)*4.0f;
				if(m_MuzzleLight.Intensity == 0.0f)
					m_MuzzleLight.TurnOff();
			}
		}
	}
	{
		r3dSkeleton* wpnSkel = NULL;
		if(getAnimation())
			wpnSkel = getAnimation()->GetCurrentSkeleton() ;
		if(wpnSkel)
		{
			if(m_LaserPointerParticle)
			{
				D3DXMATRIX mr1, world;
				D3DXMatrixRotationYawPitchRoll(&mr1, 0, R3D_PI/2, 0);
				player->GetSkeleton()->GetBoneWorldTM("PrimaryWeaponBone", &world, player->DrawFullMatrix);
				world = mr1 * world;

				// point laser particle in direction of actual model
				D3DXMATRIX attmWorld;
				wpnSkel->GetBoneWorldTM(WeaponAttachmentBoneNames[WPN_ATTM_LEFT_RAIL], &attmWorld, world);

				float laserLeftOffset = 0.0162f;
				float laserDirOffset = -0.05f;
				float laserUpOffset = 0.0f;
				r3d_assert(m_Attachments[WPN_ATTM_LEFT_RAIL]);
				if(m_Attachments[WPN_ATTM_LEFT_RAIL]->m_itemID == 400021) // pistol laser
				{
					laserLeftOffset = 0.001f;
					laserDirOffset = 0.03f;
					laserUpOffset = -0.012f;
				}

				r3dPoint3D laserDir = r3dPoint3D(attmWorld._11, attmWorld._12, attmWorld._13);
				r3dPoint3D laserLeft = r3dPoint3D( attmWorld._31, attmWorld._32, attmWorld._33 );
				r3dPoint3D laserUp = r3dPoint3D( attmWorld._21, attmWorld._22, attmWorld._23 );
				r3dPoint3D laserPos = r3dPoint3D(attmWorld._41, attmWorld._42, attmWorld._43) + laserLeftOffset*laserLeft + laserDirOffset * laserDir + laserUpOffset*laserUp;

				m_LaserPointerParticle->SetPosition(laserPos);
				m_LaserPointerParticle->HackedFOV = r_first_person_fov->GetFloat();
				if(m_LaserPointerParticle->Torch)
				{
					m_LaserPointerParticle->Torch->BeamTargetPosition = laserPos + 10 * laserDir;
				}

				// but cast laser pointer ray with actual shooting vector
				// (code from updateShootingTarget())
				r3dPoint3D laserCastPos = laserPos;
				r3dPoint3D laserCastDir;
				if(player->m_isInScope || g_camera_mode->GetInt() != 1)
					r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &laserCastDir);
				else
					r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH*0.32f, &laserCastDir);

				PxRaycastHit hit;
				PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER),0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
				bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(laserCastPos.x, laserCastPos.y, laserCastPos.z), PxVec3(laserCastDir.x, laserCastDir.y, laserCastDir.z), 500.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
				if( hitResult )
					m_LaserHitPoint = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);
				else
					m_LaserHitPoint.Assign(0,0,0);
			}
			if(m_FlashlightParticle)
			{
				D3DXMATRIX mr1, world;
				D3DXMatrixRotationYawPitchRoll(&mr1, 0, R3D_PI/2, 0);
				player->GetSkeleton()->GetBoneWorldTM("PrimaryWeaponBone", &world, player->DrawFullMatrix);
				world = mr1 * world;

				D3DXMATRIX attmWorld;
				wpnSkel->GetBoneWorldTM(WeaponAttachmentBoneNames[WPN_ATTM_LEFT_RAIL], &attmWorld, world);

				float flashLeftOffset = 0.0162f;
				float flashDirOffset = -0.05f;
				float flashUpOffset = -0.03f;
				if(m_Attachments[WPN_ATTM_LEFT_RAIL]->m_itemID == 400022) // pistol flashlight
				{
					flashLeftOffset = 0.0f;
					flashDirOffset = -0.05f;
					flashUpOffset = -0.03f;
				}

				r3dPoint3D flashDir = r3dPoint3D( attmWorld._11, attmWorld._12, attmWorld._13 );
				r3dPoint3D flashLeft = r3dPoint3D( attmWorld._31, attmWorld._32, attmWorld._33 );
				r3dPoint3D flashUp = r3dPoint3D( attmWorld._21, attmWorld._22, attmWorld._23 );
				r3dPoint3D flashPos = r3dPoint3D(attmWorld._41, attmWorld._42, attmWorld._43) + flashLeftOffset*flashLeft + flashDirOffset*flashDir + flashUpOffset*flashUp;
				m_FlashlightParticle->SetPosition(flashPos);
				m_FlashlightParticle->HackedFOV = r_first_person_fov->GetFloat();
				if(m_FlashlightParticle->Torch)
				{
					m_FlashlightParticle->Torch->Direction = flashDir;
				}

				m_Flashlight.Assign(flashPos);
				m_Flashlight.Direction = r3dPoint3D( attmWorld._11, attmWorld._12, attmWorld._13 );
			}
		}
		else // TPS mode
		{
			if(m_LaserPointerParticle)
			{
				// but cast laser pointer ray with actual shooting vector
				// (code from updateShootingTarget())
				r3dPoint3D laserCastPos = muzzlerPoint;
				r3dPoint3D laserCastDir;
				if(player->m_isInScope || g_camera_mode->GetInt() != 1)
					r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &laserCastDir);
				else
					r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH*0.32f, &laserCastDir);

				PxRaycastHit hit;
				PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER),0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
				bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(laserCastPos.x, laserCastPos.y, laserCastPos.z), PxVec3(laserCastDir.x, laserCastDir.y, laserCastDir.z), 500.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
				if( hitResult )
					m_LaserHitPoint = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);
				else
					m_LaserHitPoint.Assign(0,0,0);
			}
			if(m_FlashlightParticle)
			{
				m_Flashlight.Assign(muzzlerPoint);
				m_Flashlight.Direction = r3dPoint3D( weaponBone._11, weaponBone._12, weaponBone._13 );
			}
		}
	}
	r3dPoint3D shellPos = getShellPos(weaponBone);
	if(m_ShellExtractParticle)
		m_ShellExtractParticle->SetPosition(shellPos);
}

bool Weapon::hasLaserPointer(r3dPoint3D& laserPos)
{
	laserPos = m_LaserHitPoint;
	return m_LaserPointerParticle!=NULL && !laserPos.AlmostEqual(r3dPoint3D(0,0,0), 0.01f);
}

void Weapon::OnGameEnded() 
{
	if(m_sndNewFire)
	{
		SoundSys.KeyOff(m_sndNewFire, "trigger");
		m_sndNewFire = 0;
	}
}

const WeaponAttachmentConfig* Weapon::getClipConfig()
{
	// (SERVER CODE SYNC POINT)
	return m_Attachments[WPN_ATTM_CLIP];
}

wiInventoryItem& Weapon::getPlayerItem()
{
	if(m_pConfig->m_itemID == WeaponConfig::ITEMID_UnarmedMelee) {
		static wiInventoryItem tempMeleeItem;
		return tempMeleeItem;
	}
	
	wiInventoryItem& wi = m_Owner->CurLoadout.Items[m_BackpackIdx];
	// ptumik: this assert is real pain in the ass. Is it really necessary? Because of it I have to save a copy of wiInventoryItem before calling Fire()
	r3d_assert(wi.itemID == m_pConfig->m_itemID);	// make sure it wasn't changed
	return wi;
}

int Weapon::getNumClipsLeft()
{
	const WeaponAttachmentConfig* clipCfg = getClipConfig();
	if(!clipCfg)
		return 0;

	int numClips = 0;
	for(int i=0; i<m_Owner->CurLoadout.BackpackSize; i++)
	{
		if(m_Owner->CurLoadout.Items[i].itemID == clipCfg->m_itemID)
		{
			numClips += m_Owner->CurLoadout.Items[i].quantity;
		}
	}
	
	return numClips;
}

r3dPoint3D Weapon::getMuzzlePos(const D3DXMATRIX& weaponBone) const
{
	D3DXVECTOR3 res;
	D3DXVECTOR3 offset = m_pConfig->muzzlerOffset ;
	D3DXVec3TransformNormal(&res, (D3DXVECTOR3*)&offset, &weaponBone);
	r3dPoint3D muzzlerPoint = r3dPoint3D(weaponBone._41, weaponBone._42, weaponBone._43)+r3dPoint3D(res.x, res.y, res.z);
	// PT: sometimes animation has QNAN in it, not sure where is it coming from. Seems like in some cases quaternion becomes fucked up and not able to transform it into a matrix. As all other data in skeleton is fine
	if(!(r3d_float_isFinite(muzzlerPoint.x) && r3d_float_isFinite(muzzlerPoint.y) && r3d_float_isFinite(muzzlerPoint.z)))
		muzzlerPoint.Assign(0,0,0);
	return muzzlerPoint;
}

r3dPoint3D Weapon::getShellPos(const D3DXMATRIX& weaponBone) const
{
	if(m_pConfig->IsFPS && m_WeaponAnim_FPS)
	{
		D3DXMATRIX boneMat;
		m_WeaponAnim_FPS->GetCurrentSkeleton()->GetBoneWorldTM("Bone_Attm_Shell", &boneMat, weaponBone);
		return r3dPoint3D(boneMat._41, boneMat._42, boneMat._43);
	}
	else
	{
		D3DXVECTOR3 res;
		D3DXVec3TransformNormal(&res, (D3DXVECTOR3*)&m_pConfig->shellOffset, &weaponBone);
		r3dPoint3D shellPoint = r3dPoint3D(weaponBone._41, weaponBone._42, weaponBone._43)+r3dPoint3D(res.x, res.y, res.z);
		// PT: sometimes animation has QNAN in it, not sure where is it coming from. Seems like in some cases quaternion becomes fucked up and not able to transform it into a matrix. As all other data in skeleton is fine
		if(!(r3d_float_isFinite(shellPoint.x) && r3d_float_isFinite(shellPoint.y) && r3d_float_isFinite(shellPoint.z)))
			shellPoint.Assign(0,0,0);
		return shellPoint;
	}
}

r3dPoint3D Weapon::getShellDir(const D3DXMATRIX& weaponBone) const
{
	if(m_pConfig->IsFPS && m_WeaponAnim_FPS)
	{
		D3DXMATRIX boneMat;
		m_WeaponAnim_FPS->GetCurrentSkeleton()->GetBoneWorldTM("Bone_Attm_Shell", &boneMat, weaponBone);
		return r3dPoint3D(boneMat._11, boneMat._12, boneMat._13);
	}
	else
	{
		D3DXVECTOR3 shellDir(0,1,-1);
		D3DXVECTOR3 randomV(u_GetRandom(-0.1f, 0.1f), u_GetRandom(-0.1f, 0.1f), u_GetRandom(-0.1f, 0.1f));
		shellDir += randomV;
		D3DXVECTOR3 res;
		D3DXVec3TransformNormal(&res, &shellDir, &weaponBone);

		return r3dPoint3D(res);
	}
}

int Weapon::getNumShotsRequired()
{
	int numShots = 1;

	// only ONE shot per grenade/support please.
	if(m_pConfig->category == storecat_GRENADE)
		return numShots;

	// active only when trigger is pressed for more than one frame
	if(m_triggerPressed>1)
	{
		float timeDelta = r3dGetTime()-m_lastTimeFired;
		float fNumShots = timeDelta/getRateOfFire();
		numShots = (int)floorf(fNumShots);
		m_fractionTimeLeftFromPreviousShot = (fNumShots-numShots)*getRateOfFire();
		//r3dOutToLog("NumShots %d, delta %.3f, rate %.3f, fps %.3f, delta %.3f\n", numShots, timeDelta, getRateOfFire(), r3dGetFrameTime(), m_fractionTimeLeftFromPreviousShot);
	}

	// check how many bullets left in clip
	numShots = R3D_MIN(numShots, getNumBulletsLeft());
	
	return numShots;
}

int Weapon::getWeaponFireSound()
{
	int sndFireID_single_player = m_pConfig->m_sndFireID_single_player;
	int sndFireID_single = m_pConfig->m_sndFireID_single;
	int sndFireID_auto_player = m_pConfig->m_sndFireID_auto_player;
	int sndFireID_auto = m_pConfig->m_sndFireID_auto;

	if(m_Attachments[WPN_ATTM_MUZZLE])
	{
		if(m_Attachments[WPN_ATTM_MUZZLE]->m_sndFireID_single_player!=-1)
			sndFireID_single_player = m_Attachments[WPN_ATTM_MUZZLE]->m_sndFireID_single_player;
		if(m_Attachments[WPN_ATTM_MUZZLE]->m_sndFireID_single!=-1)
			sndFireID_single = m_Attachments[WPN_ATTM_MUZZLE]->m_sndFireID_single;
		if(m_Attachments[WPN_ATTM_MUZZLE]->m_sndFireID_auto_player!=-1)
			sndFireID_auto_player = m_Attachments[WPN_ATTM_MUZZLE]->m_sndFireID_auto_player;
		if(m_Attachments[WPN_ATTM_MUZZLE]->m_sndFireID_auto!=-1)
			sndFireID_auto = m_Attachments[WPN_ATTM_MUZZLE]->m_sndFireID_auto;
	}

	if(m_firemode == WPN_FRM_SINGLE)
		return m_Owner->NetworkLocal?sndFireID_single_player:sndFireID_single;
	else if(m_firemode == WPN_FRM_TRIPLE && m_sndNewFire == 0)
		return m_Owner->NetworkLocal?sndFireID_auto_player:sndFireID_auto;
	else if(m_firemode == WPN_FRM_AUTO && m_sndNewFire == 0)
		return	m_Owner->NetworkLocal?sndFireID_auto_player:sndFireID_auto;
	
	return -1;
}

void Weapon::Fire(const r3dPoint3D& hitPos, const D3DXMATRIX& weaponBone, bool executeWeaponFireCode, float cookingTime, const r3dPoint3D& grenadeFireFrom, GameObject** out_bulletObj)
{
	if(isUsableItem())
	{
		m_lastTimeFired = r3dGetTime() - m_fractionTimeLeftFromPreviousShot;
		if(m_Owner->NetworkLocal && m_pConfig->m_isConsumable) 
		{
			wiInventoryItem& wi = getPlayerItem();
			r3d_assert(wi.quantity > 0);
			wi.quantity--;
			if(wi.quantity <= 0)
				wi.Reset();
		}
		return;
	}

	if(executeWeaponFireCode && m_Owner->NetworkLocal && getCategory() != storecat_GRENADE) // do this check only for local player, we are not tracking this correctly for network players
	{
		if(m_State != WPN_READY) // because we can shoot more than one bullet per frame, let's double check that weapon isn't reloading
			return;
	}

	m_lastTimeFired = r3dGetTime() - m_fractionTimeLeftFromPreviousShot;

	const float shootTime = getRateOfFire();

	// start firing anim
	if(executeWeaponFireCode)
	{
		m_Owner->uberAnim_->StartShootAnim();

		// and set it's speed to rate of fire.
		r3dAnimation::r3dAnimInfo* ai = m_Owner->uberAnim_->anim.GetTrack(m_Owner->uberAnim_->shootAnimTrackID);
		if(ai) // TPS mode does not have shoot anim track
			ai->fSpeed = ((float)ai->pAnim->NumFrames / ai->pAnim->fFrameRate) / shootTime;
	}

	if(executeWeaponFireCode)
	{
		if(m_pConfig->m_sndFireID_single!=-1 || m_pConfig->m_sndFireID_auto!=-1) // we need at least one of those to switch to new weapon sound system for this gun
		{
			int fireSoundID = getWeaponFireSound();
			if(m_firemode == WPN_FRM_SINGLE)
				m_sndNewFire = SoundSys.Play(fireSoundID, m_Owner->GetPosition());
			else if(m_firemode == WPN_FRM_TRIPLE && m_sndNewFire == 0)
				m_sndNewFire = SoundSys.Play(fireSoundID, m_Owner->GetPosition());
			else if(m_firemode == WPN_FRM_AUTO && m_sndNewFire == 0)
				m_sndNewFire = SoundSys.Play(fireSoundID, m_Owner->GetPosition());
#ifndef FINAL_BUILD
			if(fireSoundID == -1 && m_sndNewFire==NULL)
				r3dOutToLog("Failed to get fire sound for: %s, firemode: %s\n", m_pConfig->m_StoreName, (m_firemode == WPN_FRM_SINGLE)?"single":(m_firemode == WPN_FRM_TRIPLE?"triple":"auto"));
#endif
		}
	}

	if(getCategory() == storecat_GRENADE) 
	{
		if( m_Owner->NetworkLocal ) 
		{
			m_needDelayedAction = true;
			m_needDelayedAction_pos = hitPos;
			m_needDelayedAction_delay = cookingTime;
			m_needDelayedAction_startTime = r3dGetTime();
		} 
		else 
		{
			r3d_assert(m_pConfig->m_PrimaryAmmo);
			obj_Player* player = (obj_Player*)m_Owner;
			if ( player->bDead == false ) 
			{
				GameObject* pGrenade = NULL;
				pGrenade = m_pConfig->m_PrimaryAmmo->Fire(hitPos, grenadeFireFrom, weaponBone, m_Owner, m_pConfig, cookingTime);
				if( out_bulletObj )
					*out_bulletObj = pGrenade;
			}
		}
	}
	else
	{
		if(m_WeaponAnim_FPS && executeWeaponFireCode)
		{
			bool need_restart_fire = false;
			if(!m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire))
				need_restart_fire=true;
			else
				need_restart_fire = (m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire)->GetStatus() & ANIMSTATUS_Finished) ? true : false;

			if(need_restart_fire)
			{
				if(m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire) && m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire)->GetStatus() & ANIMSTATUS_Finished)
				{
					m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire)->fCurFrame = 0.0f;
					DWORD status = m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire)->GetStatus();
					status &= ~ANIMSTATUS_Finished;
					m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire)->SetStatus(status);
				}
				else
				{
					m_AnimTrack_fire = m_WeaponAnim_FPS->StartAnimation(1, ANIMFLAG_PauseOnEnd|ANIMFLAG_RemoveOtherNow, 1.0f, 1.0f, 0.0f);

					// and set it's speed to rate of fire
					r3dAnimation::r3dAnimInfo* ai = m_WeaponAnim_FPS->GetTrack(m_AnimTrack_fire);
					if(ai)
						ai->fSpeed = ((float)ai->pAnim->NumFrames / ai->pAnim->fFrameRate) / shootTime;
				}
			}
		}

		r3d_assert(m_pConfig->m_PrimaryAmmo);
		r3dPoint3D muzzlerPoint = getMuzzlePos(weaponBone);
		GameObject* bulletObj = m_pConfig->m_PrimaryAmmo->Fire(hitPos, muzzlerPoint, weaponBone, m_Owner, m_pConfig);
		if(out_bulletObj)
			*out_bulletObj = bulletObj;
		
		const obj_Player* localPlayer = gClientLogic().localPlayer_;
		float distToLocalPlayer = 0.0f;
		bool playerVisible = r3dRenderer->IsSphereInsideFrustum(m_Owner->GetPosition()+r3dPoint3D(0,1,0), 1.0f)>0 || (m_Owner->NetworkLocal && g_camera_mode->GetInt()==2);
		if(localPlayer)
			distToLocalPlayer = (localPlayer->GetPosition() - m_Owner->GetPosition()).Length();

		bool localPlayerInSniperMode = false;
		extern bool g_bEditMode;
		if(((m_Owner == localPlayer) || g_bEditMode) && ((obj_Player*)m_Owner)->m_isInScope) // do not spawn shell when in sniper mode
			localPlayerInSniperMode = true;

		extern bool	g_CameraInsidePlayer;
		if(((m_Owner == localPlayer) || g_bEditMode) && g_CameraInsidePlayer)
			localPlayerInSniperMode = true;
		
		// add shell
		r3dPoint3D shellPos = getShellPos(weaponBone);
		r3dPoint3D shellDir = getShellDir(weaponBone);
		if(m_pConfig->category != storecat_MELEE && m_pConfig->category != storecat_GRENADE && distToLocalPlayer < 30.0f && executeWeaponFireCode && m_pConfig->m_itemID != 101322 && m_pConfig->m_itemID != 101392) //101322 - crossbow, 101392 - nailgun
		{
			if(!localPlayerInSniperMode) 
			{
				BulletShellType shellType = BST_Pistol;
				if(m_pConfig->category == storecat_ASR || m_pConfig->category == storecat_SNP || m_pConfig->category == storecat_MG)
					shellType = BST_Rifle;
				else if(m_pConfig->category == storecat_SHTG)
					shellType = BST_Shotgun;

				D3DXMATRIX shellOrient = weaponBone;
				shellOrient._41 = 0; shellOrient._42 = 0; shellOrient._43 = 0; shellOrient._41 = 1;

#if !R3D_NO_BULLET_MGR
				GameWorld().m_BulletMngr->AddShell(shellPos, shellDir*u_GetRandom(0.15f, 0.25f), shellOrient, shellType);
#endif
			}
		}

		if(m_MuzzleParticle && distToLocalPlayer < 1000.0f && playerVisible && executeWeaponFireCode)
		{
			if(!localPlayerInSniperMode)
			{
				m_MuzzleParticle->bRender = true;
				m_MuzzleParticle->SetPosition(muzzlerPoint);
				D3DXVECTOR3 weaponAxis(1,0,0);
				D3DXVECTOR3 res;
				D3DXVec3TransformNormal(&res, &weaponAxis, &weaponBone);
				m_MuzzleParticle->Torch->Direction.Assign(res.x, res.y, res.z);
				m_MuzzleParticle->Restart();

				if(!(m_Attachments[WPN_ATTM_MUZZLE] && (m_Attachments[WPN_ATTM_MUZZLE]->m_itemID == 400012 || m_Attachments[WPN_ATTM_MUZZLE]->m_itemID == 400013 || m_Attachments[WPN_ATTM_MUZZLE]->m_itemID == 400156 || m_Attachments[WPN_ATTM_MUZZLE]->m_itemID == 400159)))  // flash hider or silsencer
				{
					m_MuzzleLight.TurnOn();
					m_MuzzleLight.Assign(muzzlerPoint);
					m_MuzzleLight.Intensity = 4.0f;
				}
			}
		}

		if(m_ShellExtractParticle && distToLocalPlayer < 100.0f && playerVisible && executeWeaponFireCode)
		{
			if(!localPlayerInSniperMode)
			{
				m_ShellExtractParticle->bRender = true;
				m_ShellExtractParticle->SetPosition(shellPos);
				D3DXVECTOR3 shellDir(0,0,-1);
				D3DXVECTOR3 res;
				D3DXVec3TransformNormal(&res, &shellDir, &weaponBone);

				m_ShellExtractParticle->Torch->Direction.Assign(res.x, res.y, res.z);
				m_ShellExtractParticle->Restart();
			}
		}

		/*if(isImmediateShooting() && getCategory()!=storecat_SHTG)
		{
			float len = (m_Owner->GetPosition() - hitPos).Length();
			if(len > 25 && len < 120 && (m_pConfig->category == storecat_ASR || m_pConfig->category == storecat_MG) && (m_numBulletsLeftInClip%5)==0) // every 5th bullet renders tracer
			{
				// add tracer
				obj_ParticleSystem* tracer = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", m_pConfig->m_PrimaryAmmo->m_ParticleTracer, muzzlerPoint);
				tracer->Torch->BeamTargetPosition = hitPos;
			}
		}*/

#if APEX_ENABLED
		//	Add apex destruction
		g_pApexWorld->ApplyDamage(1.0f, 1.0f, muzzlerPoint, hitPos - muzzlerPoint);
#endif
	}

	if(m_Owner->NetworkLocal && m_pConfig->category != storecat_MELEE && m_pConfig->category != storecat_GRENADE && executeWeaponFireCode)
	{
		wiInventoryItem& wi = getPlayerItem();
		r3d_assert(wi.Var1 > 0);

		// decrease number of bullets
		wi.Var1--;
	}
	
	if(m_Owner->NetworkLocal && executeWeaponFireCode)
	{
		// adjust durability (SERVER_SYNC_POINT DUR), melee will be degraded on hit
		if(getCategory() != storecat_MELEE)
			AdjustWeaponDurability();
	}
}

void Weapon::AdjustWeaponDurability()
{
	if(m_BackpackIdx == HANDS_WEAPON_IDX)
		return;

	getPlayerItem().adjustDurability(-getConfig()->m_DurabilityUse);

	// adjust durability for all attachments except clip
	for(int atm=0; atm<WPN_ATTM_MAX; atm++)
	{
		if(atm == WPN_ATTM_CLIP)
			continue;

		const WeaponAttachmentConfig* wac = m_Attachments[atm];
		if(wac == NULL)
			continue;

		// search for that attachment in player inventory
		for(int i=0; i<m_Owner->CurLoadout.BackpackSize; i++)
		{
			wiInventoryItem& itm = m_Owner->CurLoadout.Items[i];
			if(itm.itemID == wac->m_itemID)
			{
				// important semi-hack, do not adjust durability for OLD stacked attachmnts. 
				if(itm.quantity > 1)
					continue;
				
				itm.adjustDurability(-wac->m_DurabilityUse);
				break;
			}
		}
	}
}

void Weapon::ReloadIfNeeded()
{
	if(m_Owner->NetworkLocal && m_pConfig->category != storecat_MELEE && m_pConfig->category != storecat_GRENADE)
	{
		wiInventoryItem& wi = getPlayerItem();
		if(wi.Var1 <= 0)
		{
			m_State = WPN_EMPTY;
			Reload();
		}
	}
}

float Weapon::getSpread() const 
{ 
	float spread = m_pConfig->m_spread;
	spread = spread * (1.0f+m_WeaponAttachmentStats[WPN_ATTM_STAT_SPREAD]);

	return spread;
} 

float Weapon::getRecoil() const 
{ 
	float recoil = m_pConfig->m_recoil;
	recoil = recoil * (1.0f+m_WeaponAttachmentStats[WPN_ATTM_STAT_RECOIL]);

	return recoil * 0.05f;
}


void Weapon::StartReloadSequence()
{
	// reload anim
	if(m_pConfig->m_sndReloadID)
		m_sndReload = SoundSys.Play(m_pConfig->m_sndReloadID, m_Owner->GetPosition());
	m_State = WPN_RELOADING;
	m_lastReloadingTime = r3dGetTime();
	m_ReloadTime = m_pConfig->m_reloadTime;
	m_triggerPressed = 0; // reset burst fire counter
	m_triggerPressed2 = 0;

	if(m_WeaponAnim_FPS)
	{
		int reloadTrack = m_WeaponAnim_FPS->StartAnimation(2, ANIMFLAG_RemoveOtherNow, 1.0f, 1.0f, 0.0f);

		// scale reload anim to match weapon reload time
		r3dAnimation::r3dAnimInfo* ai = m_WeaponAnim_FPS->GetTrack(reloadTrack);
		if(ai && m_ReloadTime > 0)
		{
			float animTime = (float)ai->pAnim->NumFrames / ai->pAnim->fFrameRate;
			float k = animTime / m_ReloadTime;
			ai->SetSpeed(k);
		}
	}
}

void Weapon::Reload(int optionalAmmoSlotToUse)
{
	r3d_assert(m_Owner->NetworkLocal && "Weapon::Reload() only for local players");

	const WeaponAttachmentConfig* clipCfg = getClipConfig();
	if(!clipCfg)
	{
		r3dOutToLog("!!! weapon can't be reloaded - there is no clip defined\n");
		return;
	}

	// do not reload if weapon has broken down and it will be removed by server once we receive the packet
	if(getPlayerItem().Var3 == 0)
		return;

	r3d_assert(m_State != WPN_RELOADING);
	
#ifndef FINAL_BUILD
	extern bool g_bEditMode;
	if(g_bEditMode)
	{
		getPlayerItem().Var1 = clipCfg->m_Clipsize;
		StartReloadSequence();
		return;
	}
#endif

#ifndef FINAL_BUILD
	r3dOutToLog("reloading %s, need clip %d\n", m_pConfig->m_StoreName, clipCfg->m_itemID);
#endif	

	// if current clip is full, do not reload
	if(getNumBulletsLeft() == clipCfg->m_Clipsize)
		return;
	
	// search for same clip
	int ammoSlot = optionalAmmoSlotToUse;
	if(ammoSlot==-1)
	{
		for(int i=0; i<m_Owner->CurLoadout.BackpackSize; i++)
		{
			if(m_Owner->CurLoadout.Items[i].itemID == clipCfg->m_itemID)
			{
				ammoSlot = i;
				break;
			}
		}
	}
	if(ammoSlot == -1) {
		return;
	}
	
	// calc reloaded amount and update weapon item ammo (SERVER CODE SYNC POINT)
	wiInventoryItem& ammoItm = m_Owner->CurLoadout.Items[ammoSlot];
	int amount = ammoItm.Var1 < 0 ? clipCfg->m_Clipsize : ammoItm.Var1;
	getPlayerItem().Var1 = amount;
	getPlayerItem().Var2 = clipCfg->m_itemID;
	
	m_Owner->ReloadWeaponFromSlot(m_BackpackIdx, ammoSlot, amount);
	
	StartReloadSequence();
}

bool Weapon::isImmediateShooting() const 
{ 
	return m_pConfig->m_AmmoImmediate;
}

// dist = distance to target
float Weapon::calcDamage(float dist) const
{
	r3d_assert(false); // shouldn't be called on client side. Server side has ServerWeapon::calcDamage
	return m_pConfig->m_AmmoDamage;
}

float Weapon::getAmmoSpeed() const 
{
	return m_pConfig->m_AmmoSpeed;
}

float Weapon::getAmmoMass() const 
{ 
	return m_pConfig->m_AmmoMass;
}

const char* Weapon::getDecalSource() const 
{ 
	return m_pConfig->m_PrimaryAmmo->m_DecalSource; 
}

bool Weapon::isLoaded() const
{
	r3dMesh* mesh = m_pConfig->getMesh(true, m_isFirstPerson) ;
	return mesh && mesh->IsDrawable() ;
}

extern r3dSec_type<CUberData*, 0xA45AdFCa> AI_Player_UberData;
void enableAnimBones(const char* boneName, const r3dSkeleton* skel, r3dAnimData* ad, int enable);

uint32_t Weapon::getWeaponAttachmentID(WeaponAttachmentTypeEnum attm_type)
{
	if(m_Attachments[(int)attm_type])
		return m_Attachments[(int)attm_type]->m_itemID;
	return 0;
}

bool Weapon::getWeaponAttachmentPos(WeaponAttachmentTypeEnum attm_type, D3DXMATRIX& res)
{
	if(m_Attachments[(int)attm_type])
	{
		r3dSkeleton* wpnSkel = NULL;
		if(getAnimation())
			wpnSkel = getAnimation()->GetCurrentSkeleton() ;
		if(wpnSkel)
		{
			D3DXMATRIX mr1, world;
			D3DXMatrixRotationYawPitchRoll(&mr1, 0, R3D_PI/2, 0);
			m_Owner->GetSkeleton()->GetBoneWorldTM("PrimaryWeaponBone", &world, m_Owner->DrawFullMatrix);
			world = mr1 * world;

			D3DXMATRIX attmWorld;
			wpnSkel->GetBoneWorldTM(WeaponAttachmentBoneNames[(int)attm_type], &attmWorld, world);

			res = attmWorld;
			return true;
		}
	}

	return false;
}

void Weapon::setWeaponAttachmentsByIDs(const uint32_t* ids)
{
	r3d_assert(ids);
	const WeaponAttachmentConfig* configs[WPN_ATTM_MAX] = {0};
	for(int i=0; i<WPN_ATTM_MAX; ++i)
	{
		if(ids[i]>0)
			configs[i] = g_pWeaponArmory->getAttachmentConfig(ids[i]);
		else
			configs[i] = g_pWeaponArmory->getAttachmentConfig(m_pConfig->FPSDefaultID[i]);
	}
	setWeaponAttachments(configs);
}

void Weapon::setWeaponAttachments(const WeaponAttachmentConfig** wpnAttmConfigs)
{
	extern bool g_bEditMode;
	if(!g_bEditMode) // also do this check only if in game, to allow artists to test attachments in editor
		if(!m_pConfig->IsFPS)
			return;

	for(int i=0; i<WPN_ATTM_MAX; ++i)
	{
		const WeaponAttachmentConfig* wpnAttmConfig = wpnAttmConfigs[i];
		WeaponAttachmentTypeEnum attm_type = (WeaponAttachmentTypeEnum)i;

		// verify that attachment is legit and can go into this weapon
		if(!m_pConfig->isAttachmentValid(wpnAttmConfig))
			wpnAttmConfig = NULL;

		if(m_Attachments[i] == wpnAttmConfig) // if that atttachment already installed - then skip
			continue;

		m_Attachments[i] = wpnAttmConfig;
		// start loading mesh
		if(wpnAttmConfig && m_Owner->NetworkLocal) // load attachment meshes ONLY for local player, as others are using TPS models anyway
		{
			wpnAttmConfig->getMesh(true, false);
			wpnAttmConfig->getMesh(true, true);
		}

		if(attm_type == WPN_ATTM_MUZZLE)
			reloadMuzzleParticle();

		if(attm_type == WPN_ATTM_LEFT_RAIL)
		{
			// remove previous laser/flashlights
			if(m_LaserPointerParticle) {
				m_LaserPointerParticle->bKill = true;
				m_LaserPointerParticle = NULL;
			}
			if(m_FlashlightParticle) {
				m_FlashlightParticle->bKill = true;
				m_FlashlightParticle = NULL;
			}
			if(m_Flashlight.pLightSystem) {
				WorldLightSystem.Remove(&m_Flashlight);
			}

			// load new one
			if(wpnAttmConfig)
			{
				if(wpnAttmConfig->m_itemID == 400004 || wpnAttmConfig->m_itemID == 400021) // laser attachment, hard coded for now
				{
					r3d_assert(m_LaserPointerParticle == NULL);
					if(m_Owner->NetworkLocal) // load only for local player
					{
						m_LaserPointerParticle = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "weaponfx_laserpointer", r3dPoint3D(0,0,0));
						if(m_LaserPointerParticle->Torch != 0)
						{
							m_LaserPointerParticle->bKeepAlive = true; // do not allow it to be destroyed
						}
					}
				}
				if(wpnAttmConfig->m_itemID == 400018 || wpnAttmConfig->m_itemID == 400022) // flashlight attachment, hard coded for now
				{
					r3d_assert(m_FlashlightParticle == NULL);
					{
						m_FlashlightParticle = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "weaponfx_flashlight", r3dPoint3D(0,0,0));
						if(m_FlashlightParticle->Torch != 0)
						{
							m_FlashlightParticle->bKeepAlive = true; // do not allow it to be destroyed
							m_FlashlightParticle->Torch->bInheretDirectionFromSystem = 1; // hard coded for ray particles
							m_FlashlightParticle->ObjFlags |= OBJFLAG_SkipDraw; // turned off until equipped
						}
					}
			
					m_Flashlight.Assign(0,0,0);
					m_Flashlight.SetType(R3D_SPOT_LIGHT);
					m_Flashlight.SetRadius(10.5f, 36.0f);
					m_Flashlight.SetColor(228, 229, 252);
					m_Flashlight.SpotAngleInner = 10.41f;
					m_Flashlight.SpotAngleOuter = 26.21f;
					m_Flashlight.bCastShadows = r_lighting_quality->GetInt() == 3 && r_shadows_quality->GetInt() >= 3;
					m_Flashlight.Intensity = 4.31f;
					m_Flashlight.SpotAngleFalloffPow = 1.6f;
					m_Flashlight.bSSShadowBlur = 1;
					m_Flashlight.bUseGlobalSSSBParams = 0;
					m_Flashlight.SSSBParams.Bias = 0.1f;
					m_Flashlight.SSSBParams.PhysRange = 43.12f;
					m_Flashlight.SSSBParams.Radius = 10.0f;
					m_Flashlight.SSSBParams.Sense = 635.18f;
					m_Flashlight.ProjectMap = r3dRenderer->LoadTexture("data\\ProjectionTextures\\flashlight_02.dds");
					//m_Flashlight.TurnOff();Cynthia:fixed bugID 725. keep light on what it is.
					WorldLightSystem.Add(&m_Flashlight); 
				}
			}
		}
		if(attm_type == WPN_ATTM_UPPER_RAIL)
		{
			if(m_Owner->NetworkLocal) // load only for local player
			{
				if(m_animationIds_FPS == NULL)
				{
					m_animationIds_FPS = game_new int[CUberData::AIDX_COUNT];
					memcpy(m_animationIds_FPS, m_pConfig->m_animationIds_FPS, sizeof(int[CUberData::AIDX_COUNT]));
				}

#ifndef FINAL_BUILD
				r3dOutToLog("Trying to load zoom anims for: %s, %s\n", m_pConfig->FNAME, m_pConfig->m_StoreName);
#endif
				// now replace aim anims
				char aname[128];
				sprintf(aname, "FPS_Zoom_%s_Walk", m_pConfig->m_AnimPrefix);
				int animID = AI_Player_UberData->TryToAddAnimation(aname);
				if(animID!=-1)
				{
					m_animationIds_FPS[CUberData::AIDX_ProneAim] = m_animationIds_FPS[CUberData::AIDX_CrouchAim] = m_animationIds_FPS[CUberData::AIDX_WalkAim] = animID;
					{
						// those animations is upper body
						r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_WalkAim]);
						enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_WalkAim].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
					}
				}

				sprintf(aname, "FPS_Zoom_%s_Fire", m_pConfig->m_AnimPrefix);
				animID = AI_Player_UberData->TryToAddAnimation(aname);
				if(animID!=-1)
				{
					m_animationIds_FPS[CUberData::AIDX_ShootAim] = animID;
					{
						// those animations is upper body
						r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_ShootAim]);
						enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_ShootAim].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
					}
				}
				sprintf(aname, "FPS_Zoom_%s_Idle", m_pConfig->m_AnimPrefix);
				animID = AI_Player_UberData->TryToAddAnimation(aname);
				if(animID!=-1)
				{
					m_animationIds_FPS[CUberData::AIDX_StandUpper] = animID;
					{
						// those animations is upper body
						r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_StandUpper]);
						enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_StandUpper].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
					}
				}
			}
		}
		if(attm_type == WPN_ATTM_BOTTOM_RAIL && wpnAttmConfig)
		{
			if(wpnAttmConfig->m_ScopeAnimPath[0] != '\0' && m_Owner->NetworkLocal) // only for local player
			{
				if(m_animationIds_FPS == NULL)
				{
					m_animationIds_FPS = game_new int[CUberData::AIDX_COUNT];
					memcpy(m_animationIds_FPS, m_pConfig->m_animationIds_FPS, sizeof(int[CUberData::AIDX_COUNT]));
				}

				// now replace grip anims
				char aname[128];
				sprintf(aname, "FPS_GRIP_%s_%s_Sprint", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_SprintBlend] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_SprintBlend]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_SprintBlend].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Run", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_RunBlend] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_RunBlend]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_RunBlend].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Fire", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_ShootWalk] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_ShootWalk]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_ShootWalk].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Reload", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_ReloadIdle] = m_animationIds_FPS[CUberData::AIDX_ReloadWalk] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_ReloadWalk]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_ReloadWalk].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Idle", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_IdleUpper] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_IdleUpper]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_IdleUpper].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Crouch_Run", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_CrouchBlend] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_CrouchBlend]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_CrouchBlend].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Crouch_Fire", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_ShootCrouch] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_ShootCrouch]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_ShootCrouch].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Crouch_Reload", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_ReloadCrouch] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_ReloadCrouch]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_ReloadCrouch].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Prone_Fire", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_ShootProne] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_ShootProne]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_ShootProne].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Prone_Idle", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_IdleProne] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_IdleProne]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_IdleProne].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Prone_Reload", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_ReloadProne] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_ReloadProne]);
					enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_ReloadProne].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}

				sprintf(aname, "FPS_GRIP_%s_%s_Prone_Run", m_pConfig->m_AnimPrefix, wpnAttmConfig->m_ScopeAnimPath);
				m_animationIds_FPS[CUberData::AIDX_ProneBlend] = AI_Player_UberData->AddAnimation(aname);
				{
					// those animations is upper body
					// per PMoore request, disabling this
					//r3dAnimData* ad = AI_Player_UberData->animPool_.Get(m_animationIds_FPS[CUberData::AIDX_ProneBlend]);
					//enableAnimBones(AI_Player_UberData->blendStartBones_[CUberData::AIDX_ProneBlend].c_str(), AI_Player_UberData->bindSkeleton_, ad, true);
				}
			}
		}
	}

	memset(m_WeaponAttachmentStats, 0, sizeof(m_WeaponAttachmentStats));

	// recalculate all weapon attachment stats. Maybe not 100% optimal, but will work for now, do not want to add additional function for that
	for(int i=0; i<WPN_ATTM_MAX; ++i)
	{
		if(m_Attachments[i])
		{
			m_WeaponAttachmentStats[WPN_ATTM_STAT_DAMAGE] += m_Attachments[i]->m_Damage;
			m_WeaponAttachmentStats[WPN_ATTM_STAT_RANGE] += m_Attachments[i]->m_Range;
			m_WeaponAttachmentStats[WPN_ATTM_STAT_FIRERATE] += m_Attachments[i]->m_Firerate;
			m_WeaponAttachmentStats[WPN_ATTM_STAT_RECOIL] += m_Attachments[i]->m_Recoil;
			m_WeaponAttachmentStats[WPN_ATTM_STAT_SPREAD] += m_Attachments[i]->m_Spread;
		}
	}
	// convert stats to percents (from -100..100 range to -1..1)
	m_WeaponAttachmentStats[WPN_ATTM_STAT_DAMAGE] *= 0.01f; 
	m_WeaponAttachmentStats[WPN_ATTM_STAT_RANGE] *= 0.01f; 
	m_WeaponAttachmentStats[WPN_ATTM_STAT_FIRERATE] *= 0.01f; 
	m_WeaponAttachmentStats[WPN_ATTM_STAT_RECOIL] *= 0.01f; 
	m_WeaponAttachmentStats[WPN_ATTM_STAT_SPREAD] *= 0.01f;
}

r3dMesh* Weapon::getWeaponAttachmentMesh(WeaponAttachmentTypeEnum attm_type, bool aim_model)
{
	if(m_Attachments[(int)attm_type])
	{
		return m_Attachments[(int)attm_type]->getMesh(true, aim_model);
	}

	return NULL;
}

float Weapon::getWeaponScopeZoom() const
{
	float wpnBaseScope = m_pConfig->m_scopeZoom;
	if(m_Attachments[WPN_ATTM_UPPER_RAIL])
	{
		float scopeZoom = m_Attachments[WPN_ATTM_UPPER_RAIL]->m_scopeZoom;
		return scopeZoom;
	}

	return wpnBaseScope;
}

bool Weapon::hasScopeMode(bool isFPS) const
{
	// in TPS mode, do not show scopes except for sniper rifles
	bool hasBaseScope = m_pConfig->m_scopeConfig->hasScopeMode;
	if(m_Attachments[WPN_ATTM_UPPER_RAIL])
	{
		return isFPS?m_Attachments[WPN_ATTM_UPPER_RAIL]->m_scopeConfig->hasScopeMode:(m_pConfig->category==storecat_SNP?m_Attachments[WPN_ATTM_UPPER_RAIL]->m_scopeConfigTPS->hasScopeMode:false);
	}

	if(m_pConfig->category == storecat_SNP || m_pConfig->category == storecat_UsableItem) // usable item for binoculars, etc
		return hasBaseScope;
	else
		return isFPS?hasBaseScope:false;
}

const ScopeConfig* Weapon::getScopeConfig(bool isFPS) const
{
	if(m_Attachments[WPN_ATTM_UPPER_RAIL])
	{
		return isFPS?m_Attachments[WPN_ATTM_UPPER_RAIL]->m_scopeConfig:m_Attachments[WPN_ATTM_UPPER_RAIL]->m_scopeConfigTPS;
	}
	return m_pConfig->m_scopeConfig;
}

const int* Weapon::getWeaponAnimID_FPS() const
{
	if(m_animationIds_FPS)
		return m_animationIds_FPS;

	return m_pConfig->m_animationIds_FPS;
}