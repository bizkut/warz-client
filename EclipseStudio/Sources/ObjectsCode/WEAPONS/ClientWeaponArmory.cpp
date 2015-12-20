#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "ClientWeaponArmory.h"

#ifdef WO_SERVER
	COMPILE_ASSERT(0); // server shouldn't include this file
#endif

#include "Ammo.h"
#include "Weapon.h"
#include "Gear.h"

ClientWeaponArmory::ClientWeaponArmory() 
:WeaponArmory()
{
}

ClientWeaponArmory::~ClientWeaponArmory()
{
}

bool ClientWeaponArmory::Init()
{
	return WeaponArmory::Init();
}

AchievementConfig* ClientWeaponArmory::loadAchievement(pugi::xml_node& xmlAchievement)
{
	return WeaponArmory::loadAchievement(xmlAchievement);
}

Ammo* ClientWeaponArmory::loadAmmo(pugi::xml_node& xmlAmmo)
{
	return WeaponArmory::loadAmmo(xmlAmmo);
}

ScopeConfig* ClientWeaponArmory::loadScope(pugi::xml_node& xmlScope)
{
	ScopeConfig* scope = WeaponArmory::loadScope(xmlScope);
	if(scope)
	{
		const char* maskTex = xmlScope.attribute("scopeMask").value();
		if(maskTex && maskTex[0]!=0)
			scope->scope_mask = r3dRenderer->LoadTexture(maskTex);

		const char* blurTex = xmlScope.attribute("scopeBlurMask").value();
		if(blurTex && blurTex[0]!=0)
			scope->scopeBlur_mask = r3dRenderer->LoadTexture(blurTex);

		const char* reticuleTex = xmlScope.attribute("scopeReticule").value();
		if(reticuleTex && reticuleTex[0]!=0)
			scope->scope_reticle = r3dRenderer->LoadTexture(reticuleTex);

		const char* normalTex = xmlScope.attribute("scopeNormal").value();
		if(normalTex && normalTex[0]!=0)
			scope->scope_normal = r3dRenderer->LoadTexture(normalTex);

		reticuleTex = xmlScope.attribute("reticule").value();
		if(reticuleTex && reticuleTex[0]!=0)
			scope->reticule = r3dRenderer->LoadTexture(reticuleTex);

		scope->hasScopeMode = xmlScope.attribute("hasScope").as_bool();

		if( !xmlScope.attribute("scale").empty() )
		{
			scope->scale = xmlScope.attribute("scale").as_float();
		}

		pugi::xml_attribute lighting = xmlScope.attribute("lighting") ;

		if( !lighting.empty() )
		{
			scope->lighting = lighting.as_bool() ;
		}
		else
		{
			scope->lighting = true ;
		}

		pugi::xml_attribute hide_player = xmlScope.attribute("hide_player") ;

		if( !hide_player.empty() )
		{
			scope->hide_player_model = hide_player.as_bool() ;
		}
		else
		{
			scope->hide_player_model = true ;
		}

		if(scope->hasScopeMode)
		{
			r3d_assert(scope->scope_mask);
			r3d_assert(scope->scopeBlur_mask);
			r3d_assert(scope->scope_reticle);
		}
	}

	return scope;
}

WeaponConfig* ClientWeaponArmory::loadWeapon(pugi::xml_node& xmlWeapon)
{
	WeaponConfig* weapon = WeaponArmory::loadWeapon(xmlWeapon);
	if(weapon)
	{
		weapon->m_sndReloadID = SoundSys.GetEventIDByPath(xmlWeapon.child("Sound").attribute("reload").value());

		// new weapon sounds
		if(strlen(xmlWeapon.child("Sound").attribute("shoot").value())>1)
		{
			char tmpStr[512];
			if(weapon->m_fireModeAvailable & WPN_FRM_SINGLE)
			{
				sprintf(tmpStr, "%s_single", xmlWeapon.child("Sound").attribute("shoot").value());
				weapon->m_sndFireID_single = SoundSys.GetEventIDByPath(tmpStr);
				if(weapon->m_sndFireID_single == -1)
					r3dOutToLog("WEAPON ERROR: %s trying to load non existent sound event %s\n", weapon->m_StoreName, tmpStr);
				sprintf(tmpStr, "%s_single_Player", xmlWeapon.child("Sound").attribute("shoot").value());
				weapon->m_sndFireID_single_player = SoundSys.GetEventIDByPath(tmpStr);
			}
			if(weapon->m_fireModeAvailable & (WPN_FRM_TRIPLE | WPN_FRM_AUTO))
			{
				sprintf(tmpStr, "%s_auto", xmlWeapon.child("Sound").attribute("shoot").value());
				weapon->m_sndFireID_auto = SoundSys.GetEventIDByPath(tmpStr);
				if(weapon->m_sndFireID_auto == -1)
					r3dOutToLog("WEAPON ERROR: %s trying to load non existent sound event %s\n", weapon->m_StoreName, tmpStr);
				sprintf(tmpStr, "%s_auto_Player", xmlWeapon.child("Sound").attribute("shoot").value());
				weapon->m_sndFireID_auto_player = SoundSys.GetEventIDByPath(tmpStr);
			}
		}
	}
	return weapon;
}

WeaponAttachmentConfig* ClientWeaponArmory::loadWeaponAttachment(pugi::xml_node& xmlWeaponAttachment)
{
	WeaponAttachmentConfig* attm = WeaponArmory::loadWeaponAttachment(xmlWeaponAttachment);
	if(attm)
	{
		if(strlen(xmlWeaponAttachment.child("Model").attribute("FireSound").value())>3)
		{
			char tmpStr[512];
			sprintf(tmpStr, "%s_single", xmlWeaponAttachment.child("Model").attribute("FireSound").value());
			attm->m_sndFireID_single = SoundSys.GetEventIDByPath(tmpStr);
			sprintf(tmpStr, "%s_auto", xmlWeaponAttachment.child("Model").attribute("FireSound").value());
			attm->m_sndFireID_auto = SoundSys.GetEventIDByPath(tmpStr);
			sprintf(tmpStr, "%s_single_Player", xmlWeaponAttachment.child("Model").attribute("FireSound").value());
			attm->m_sndFireID_single_player = SoundSys.GetEventIDByPath(tmpStr);
			sprintf(tmpStr, "%s_auto_Player", xmlWeaponAttachment.child("Model").attribute("FireSound").value());
			attm->m_sndFireID_auto_player = SoundSys.GetEventIDByPath(tmpStr);
		}
	}

	return attm;
}

GearConfig* ClientWeaponArmory::loadGear(pugi::xml_node& xmlGear)
{
	GearConfig* gear = WeaponArmory::loadGear(xmlGear);
	return gear;
}

ModelItemConfig* ClientWeaponArmory::loadItem(pugi::xml_node& xmlItem)
{
	ModelItemConfig* item = WeaponArmory::loadItem(xmlItem);
	return item;
}

void ClientWeaponArmory::Destroy()
{
	WeaponArmory::Destroy();
}

void ClientWeaponArmory::UnloadMeshes()
{
	m_itemsHash.IterateStart();
	while(m_itemsHash.IterateNext())
	{
		BaseItemConfig* item = m_itemsHash.IterateGet();
		item->resetMesh();
	}

	for(uint32_t i=0; i<m_NumAmmoLoaded; ++i)
		m_AmmoArray[i]->unloadModel();

	WeaponArmory::UnloadMeshes();
}
