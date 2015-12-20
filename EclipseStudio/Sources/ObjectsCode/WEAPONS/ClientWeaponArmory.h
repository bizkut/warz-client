#pragma once

#include "WeaponArmory.h"

#ifdef WO_SERVER
	COMPILE_ASSERT(0); // server shouldn't include this file
#endif

// client side armory, loads client data only
class ClientWeaponArmory : public WeaponArmory
{
public:
	ClientWeaponArmory();
	virtual ~ClientWeaponArmory();

	// loads weapon library
	virtual bool Init();

	// releases all weapons.
	virtual void Destroy();

	virtual void UnloadMeshes();

protected:
	virtual Ammo* loadAmmo(pugi::xml_node& xmlAmmo);
	virtual WeaponConfig* loadWeapon(pugi::xml_node& xmlWeapon);
	virtual WeaponAttachmentConfig* loadWeaponAttachment(pugi::xml_node& xmlWeaponAttachment);
	virtual GearConfig* loadGear(pugi::xml_node& xmlGear);
	virtual ModelItemConfig* loadItem(pugi::xml_node& xmlItem);
	virtual ScopeConfig* loadScope(pugi::xml_node& xmlScope);
	virtual AchievementConfig* loadAchievement(pugi::xml_node& xmlAchievement);
};
