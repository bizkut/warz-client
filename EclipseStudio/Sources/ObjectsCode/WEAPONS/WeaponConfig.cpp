#include "r3dPCH.h"
#include "r3d.h"

#include "BaseItemConfig.h"
#include "WeaponConfig.h"
#include "GearConfig.h"

const char* WeaponAttachmentBoneNames[WPN_ATTM_MAX] = {
	"Bone_Attm_Muzzle",
	"Bone_Attm_Rail_Scopes",
	"Bone_Attm_Rail_Left",
	"Bone_Attm_Rail_Bottom",
	"Bone_Attm_Clip",
};

bool WeaponAttachmentConfig::loadBaseFromXml(pugi::xml_node& xmlAttachment)
{
	ModelItemConfig::loadBaseFromXml(xmlAttachment);
	m_type = (WeaponAttachmentTypeEnum)xmlAttachment.attribute("type").as_int();
	m_specID = xmlAttachment.attribute("SpecID").as_int();

	m_MuzzleParticle = strdup(xmlAttachment.child("Model").attribute("MuzzleParticle").value());
	m_ScopeAnimPath = strdup(xmlAttachment.child("Model").attribute("ScopeAnim").value());

	m_Damage = xmlAttachment.child("Upgrade").attribute("damage").as_float();
	m_Range = xmlAttachment.child("Upgrade").attribute("range").as_float();
	m_Firerate = xmlAttachment.child("Upgrade").attribute("firerate").as_int();
	m_Recoil = xmlAttachment.child("Upgrade").attribute("recoil").as_float();
	m_Spread = xmlAttachment.child("Upgrade").attribute("spread").as_float();
	m_Clipsize = xmlAttachment.child("Upgrade").attribute("clipsize").as_int();
	m_scopeZoom = R3D_CLAMP(xmlAttachment.child("Upgrade").attribute("ScopeMag").as_float(), 0.0f, 100.0f)/100.0f;

	m_DurabilityUse = xmlAttachment.child("Dur").attribute("u").as_float();
	m_RepairAmount = xmlAttachment.child("Dur").attribute("r1").as_float();
	m_PremRepairAmount = xmlAttachment.child("Dur").attribute("r2").as_float();
	m_RepairPriceGD = xmlAttachment.child("Dur").attribute("r3").as_int();
	m_DurabilityUse *= 10; //[TH] request for 10x durability
	if(m_DurabilityUse > 0) m_DurabilityUse = 100.0f / m_DurabilityUse; // convert from number of uses to percent per use

	return true;
}

bool WeaponConfig::loadBaseFromXml(pugi::xml_node& xmlWeapon)
{
	ModelItemConfig::loadBaseFromXml(xmlWeapon);

	FNAME = strdup(xmlWeapon.attribute("FNAME").value());

	{
		int len = strlen(m_ModelPath);
		m_ModelPath_1st = (char*)malloc(sizeof(char)*(len+32));
		r3dscpy(m_ModelPath_1st, m_ModelPath);
		r3dscpy(&m_ModelPath_1st[len-4], "_FPS.sco");
	}

	m_AnimPrefix = strdup(xmlWeapon.child("Model").attribute("AnimPrefix").value());

	// .xml offset used as adjustment for detected one
	adjMuzzlerOffset = r3dPoint3D(xmlWeapon.child("Model").attribute("muzzlerOffset.x").as_float(),
						xmlWeapon.child("Model").attribute("muzzlerOffset.y").as_float(),
						xmlWeapon.child("Model").attribute("muzzlerOffset.z").as_float());
	
	m_MuzzleParticle = strdup(xmlWeapon.child("MuzzleModel").attribute("file").value());

	m_AmmoDamage = xmlWeapon.child("PrimaryFire").attribute("damage").as_float();
	m_AmmoImmediate = xmlWeapon.child("PrimaryFire").attribute("immediate").as_bool();
	m_AmmoMass = xmlWeapon.child("PrimaryFire").attribute("mass").as_float();
	m_AmmoDecay = xmlWeapon.child("PrimaryFire").attribute("decay").as_float();
	m_AmmoSpeed = xmlWeapon.child("PrimaryFire").attribute("speed").as_float();
	m_AmmoArea = xmlWeapon.child("PrimaryFire").attribute("area").as_float();
	m_AmmoDelay = xmlWeapon.child("PrimaryFire").attribute("delay").as_float();
	m_AmmoTimeout = xmlWeapon.child("PrimaryFire").attribute("timeout").as_float();

	m_isConsumable = category==storecat_UsableItem;
	m_ShopStackSize = xmlWeapon.child("PrimaryFire").attribute("clipSize").as_int();

	m_reloadTime= xmlWeapon.child("PrimaryFire").attribute("reloadTime").as_float();
	m_reloadActiveTick= xmlWeapon.child("PrimaryFire").attribute("activeReloadTick").as_float();
	m_fireDelay = 60.0f/(xmlWeapon.child("PrimaryFire").attribute("rateOfFire").as_float());
	m_spread = xmlWeapon.child("PrimaryFire").attribute("spread").as_float();
	m_recoil = xmlWeapon.child("PrimaryFire").attribute("recoil").as_float();
	const char* firemode = xmlWeapon.child("PrimaryFire").attribute("firemode").value();
	r3d_assert(firemode && strlen(firemode)==3);
	m_fireModeAvailable = 0;
	m_fireModeAvailable |= (firemode[0] == '1')?WPN_FRM_SINGLE:0;
	m_fireModeAvailable |= (firemode[1] == '1')?WPN_FRM_TRIPLE:0;
	m_fireModeAvailable |= (firemode[2] == '1')?WPN_FRM_AUTO:0;
	if(m_fireModeAvailable == 0)
	{
		r3dArtBug("Weapon '%s' has no firemode selected!!\n", m_StoreName);
		return false;
	}

	m_scopeZoom = R3D_CLAMP(xmlWeapon.child("PrimaryFire").attribute("ScopeZoom").as_float(), 0.0f, 100.0f)/100.0f;

	const char* animType = xmlWeapon.child("Animation").attribute("type").value();
	m_AnimType = WPN_ANIM_ASSAULT;
	if(!strcmp(animType, "assault"))
		m_AnimType = WPN_ANIM_ASSAULT;
	else if(!strcmp(animType, "pistol"))
		m_AnimType = WPN_ANIM_PISTOL;
	else if(!strcmp(animType, "grenade"))
		m_AnimType = WPN_ANIM_GRENADE;
	else if(!strcmp(animType, "mine"))
		m_AnimType = WPN_ANIM_MINE;
	else if(!strcmp(animType, "rpg"))
		m_AnimType = WPN_ANIM_RPG;
	else if(!strcmp(animType, "smg"))
		m_AnimType = WPN_ANIM_SMG;
	else if(!strcmp(animType, "melee"))
		m_AnimType = WPN_ANIM_MELEE;

	pugi::xml_node fpsNode = xmlWeapon.child("FPS");
	IsFPS = fpsNode.attribute("IsFPS").as_int();
	for(int i=0; i<WPN_ATTM_MAX; i++) {
		char buf[64];
		sprintf(buf, "i%d", i);
		FPSSpecID[i] = fpsNode.attribute(buf).as_int();
		sprintf(buf, "d%d", i);
		FPSDefaultID[i] = fpsNode.attribute(buf).as_int();
	}

	m_DurabilityUse = xmlWeapon.child("Dur").attribute("u").as_float();
	m_RepairAmount = xmlWeapon.child("Dur").attribute("r1").as_float();
	m_PremRepairAmount = xmlWeapon.child("Dur").attribute("r2").as_float();
	m_RepairPriceGD = xmlWeapon.child("Dur").attribute("r3").as_int();
	if(m_DurabilityUse > 0) m_DurabilityUse = 100.0f / m_DurabilityUse; // convert from number of uses to percent per use
		
	return true;
}

bool WeaponConfig::isAttachmentValid(const WeaponAttachmentConfig* wac) const
{
	if(wac)
	{
		// check if this is default attachment
		if(FPSDefaultID[wac->m_type] == wac->m_itemID)
			return true;
		else
		{
			if(wac->m_specID == FPSSpecID[wac->m_type] && FPSSpecID[wac->m_type] > 0) // specID equal zero means nothing goes in it except for default attm
				return true;
		}
	}

	return false;
}