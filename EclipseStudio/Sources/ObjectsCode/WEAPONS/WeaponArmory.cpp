#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "WeaponArmory.h"

#ifndef WO_SERVER
  #include "Ammo.h"
  #include "Weapon.h"
  #include "Gear.h"
#else
  #include "ServerWeapons/ServerAmmo.h"
  #include "ServerWeapons/ServerWeapon.h"
  #include "ServerWeapons/ServerGear.h"
#endif

WeaponArmory* g_pWeaponArmory = NULL;

WeaponArmory::WeaponArmory()
{
	m_NumWeaponsLoaded = 0;
	m_NumWeaponAttmLoaded = 0;
	m_NumGearLoaded = 0;
	m_NumBackpackLoaded = 0;
	m_NumHeroLoaded = 0;
	m_NumItemLoaded = 0;
	m_NumLootBoxLoaded = 0;
	m_NumFoodItemsLoaded = 0;
	m_NumVehiclesLoaded =0;
	m_NumCraftComponentsLoaded = 0;

	memset(m_AmmoArray, 0, sizeof(Ammo*)*MAX_NUMBER_AMMO);
	m_NumAmmoLoaded = 0;
	memset(m_ScopeArray, 0, sizeof(ScopeConfig*)*MAX_NUMBER_SCOPE);
	m_NumScopeLoaded = 0;
	memset(m_AchievementArray, 0, sizeof(AchievementConfig*)*MAX_NUMBER_ACHIEVEMENT);
	m_NumAchievementLoaded = 0;
}

WeaponArmory::~WeaponArmory()
{
	r3d_assert(m_NumWeaponsLoaded==0);
	r3d_assert(m_NumWeaponAttmLoaded==0);
	r3d_assert(m_NumGearLoaded==0);
	r3d_assert(m_NumBackpackLoaded==0);
	r3d_assert(m_NumHeroLoaded==0);
	r3d_assert(m_NumItemLoaded==0);
	r3d_assert(m_NumLootBoxLoaded==0);
	r3d_assert(m_NumFoodItemsLoaded==0);
	r3d_assert(m_NumVehiclesLoaded==0);
	r3d_assert(m_NumCraftComponentsLoaded==0);
	r3d_assert(m_NumCraftRecipesLoaded==0);

	r3d_assert(m_AmmoArray[0]==NULL);
	r3d_assert(m_NumAmmoLoaded==0);
	r3d_assert(m_ScopeArray[0]==NULL);
	r3d_assert(m_NumScopeLoaded==0);
	r3d_assert(m_AchievementArray[0]==NULL);
	r3d_assert(m_NumAchievementLoaded==0);
}

bool WeaponArmory::Init()
{
	// material library will kill materials in previously loaded weapon meshes, so reload it
	Destroy();

	r3d_assert(m_NumWeaponsLoaded == 0);
	r3d_assert(m_NumWeaponAttmLoaded==0);
	r3d_assert(m_NumGearLoaded==0);
	r3d_assert(m_NumBackpackLoaded==0);
	r3d_assert(m_NumHeroLoaded==0);
	r3d_assert(m_NumItemLoaded==0);
	r3d_assert(m_NumLootBoxLoaded==0);
	r3d_assert(m_NumFoodItemsLoaded==0);
	r3d_assert(m_NumVehiclesLoaded==0);
	r3d_assert(m_NumCraftComponentsLoaded==0);
	r3d_assert(m_NumCraftRecipesLoaded==0);

	r3d_assert(m_AmmoArray[0]==NULL);
	r3d_assert(m_NumAmmoLoaded==0);
	r3d_assert(m_ScopeArray[0]==NULL);
	r3d_assert(m_NumScopeLoaded==0);
	r3d_assert(m_AchievementArray[0]==NULL);
	r3d_assert(m_NumAchievementLoaded==0);

	// load game stuff
	{
		const char* GameDBFile = "Data/Weapons/gameDB.xml";

		r3dFile* f = r3d_open(GameDBFile, "rb");
		if ( !f )
		{
			r3dError("Failed to open %s\n", GameDBFile);
			return false;
		}

		char* fileBuffer = game_new char[f->size + 1];
		r3d_assert(fileBuffer);
		fread(fileBuffer, f->size, 1, f);
		fileBuffer[f->size] = 0;

		pugi::xml_document xmlFile;
		pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
		fclose(f);
		if(!parseResult)
			r3dError("Failed to parse XML, error: %s", parseResult.description());

		pugi::xml_node xmlAchievement = xmlFile.child("AchievementDB").child("Achievement");
		while(!xmlAchievement.empty())
		{
			loadAchievement(xmlAchievement);
			xmlAchievement = xmlAchievement.next_sibling();
			if(strcmp(xmlAchievement.name(), "Achievement")!=0)
				break;
		}


		if( NUM_ACHIEVEMENTS != m_NumAchievementLoaded ) {
			r3dError(" Achievement count mismatch.");
		}

		//we don't want to start the server if we have 
		r3d_assert( NUM_ACHIEVEMENTS == m_NumAchievementLoaded );

		// delete only after we are done parsing xml!
		delete [] fileBuffer;
	}

	// load ammo firstly
	{
		r3dFile* f = r3d_open("Data/Weapons/AmmoDB.xml", "rb");
		if ( !f )
		{
			r3dOutToLog("Failed to open Data/Weapons/AmmoDB.xml\n");
			return false;
		}

		char* fileBuffer = game_new char[f->size + 1];
		r3d_assert(fileBuffer);
		fread(fileBuffer, f->size, 1, f);
		fileBuffer[f->size] = 0;

		pugi::xml_document xmlFile;
		pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
		fclose(f);
		if(!parseResult)
			r3dError("Failed to parse XML, error: %s", parseResult.description());
		pugi::xml_node xmlArmory = xmlFile.child("AmmoArmory");
		pugi::xml_node xmlAmmo = xmlArmory.child("Ammo");
		while(!xmlAmmo.empty())
		{
			loadAmmo(xmlAmmo);
			xmlAmmo = xmlAmmo.next_sibling();
			if(strcmp(xmlAmmo.name(), "Ammo")!=0)
				break;
		}

		// load scopes
		pugi::xml_node xmlScope = xmlArmory.child("Scope");
		while(!xmlScope.empty())
		{
			loadScope(xmlScope);
			xmlScope = xmlScope.next_sibling();
			if(strcmp(xmlScope.name(), "Scope")!=0)
				break;
		}

		// delete only after we are done parsing xml!
		delete [] fileBuffer;
	}

	const char* ItemsDBFile = "Data/Weapons/itemsDB.xml";
	
	r3dFile* f = r3d_open(ItemsDBFile, "rb");
	if ( !f )
	{
		r3dError("Failed to open %s\n", ItemsDBFile);
		return false;
	}

	char* fileBuffer = game_new char[f->size + 1];
	r3d_assert(fileBuffer);
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;
	pugi::xml_document xmlFile;
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
	fclose(f);
	if(!parseResult)
		r3dError("Failed to parse XML, error: %s", parseResult.description());

	pugi::xml_node xmlDB = xmlFile.child("DB");
	{
		pugi::xml_node xmlAttachments = xmlDB.child("AttachmentArmory");
		pugi::xml_node xmlAttm = xmlAttachments.child("Attachment");
		while(!xmlAttm.empty())
		{
			loadWeaponAttachment(xmlAttm);
			xmlAttm = xmlAttm.next_sibling();
		}
	}
	{
		pugi::xml_node xmlArmory = xmlDB.child("WeaponsArmory");
		pugi::xml_node xmlWeapon = xmlArmory.child("Weapon");
		while(!xmlWeapon.empty())
		{
			loadWeapon(xmlWeapon);
			xmlWeapon = xmlWeapon.next_sibling();
		}
	}
	{
		pugi::xml_node xmlArmory = xmlDB.child("GearArmory");
		pugi::xml_node xmlGear = xmlArmory.child("Gear");

		while(!xmlGear.empty())
		{
			loadGear(xmlGear);
			xmlGear = xmlGear.next_sibling();
		}
	}
	{
		pugi::xml_node xmlHeroes = xmlDB.child("HeroArmory");
		pugi::xml_node xmlHero = xmlHeroes.child("Hero");
		while(!xmlHero.empty())
		{
			loadHero(xmlHero);
			xmlHero = xmlHero.next_sibling();
		}
	}
	{
		pugi::xml_node xmlArmory = xmlDB.child("BackpackArmory");
		pugi::xml_node xmlItem = xmlArmory.child("Backpack");
		while(!xmlItem.empty())
		{
			loadBackback(xmlItem);
			xmlItem = xmlItem.next_sibling();
		}
	}
	{
		pugi::xml_node xmlItems = xmlDB.child("ItemsDB");
		pugi::xml_node xmlItem = xmlItems.child("Item");
		while(!xmlItem.empty())
		{
			loadItem(xmlItem);
			xmlItem = xmlItem.next_sibling();
		}
	}
	{
		pugi::xml_node xmlItems = xmlDB.child("LootBoxDB");
		pugi::xml_node xmlItem = xmlItems.child("LootBox");
		while(!xmlItem.empty())
		{
			loadLootBox(xmlItem);
			xmlItem = xmlItem.next_sibling();
		}
	}
	{
		pugi::xml_node xmlItems = xmlDB.child("FoodArmory");
		pugi::xml_node xmlItem = xmlItems.child("Item");
		while(!xmlItem.empty())
		{
			loadFoodItem(xmlItem);
			xmlItem = xmlItem.next_sibling();
		}
	}
	{
		pugi::xml_node xmlItems = xmlDB.child("VehicleArmory");
		pugi::xml_node xmlItem = xmlItems.child("Item");
		while(!xmlItem.empty())
		{
			loadVehicleItem(xmlItem);
			xmlItem = xmlItem.next_sibling();
		}
	}
	{
		pugi::xml_node xmlItems = xmlDB.child("CraftComponentsArmory");
		pugi::xml_node xmlItem = xmlItems.child("Item");
		while(!xmlItem.empty())
		{
			loadCraftComponentItem(xmlItem);
			xmlItem = xmlItem.next_sibling();
		}
	}
	{
		pugi::xml_node xmlItems = xmlDB.child("CraftRecipeArmory");
		pugi::xml_node xmlItem = xmlItems.child("Item");
		while(!xmlItem.empty())
		{
			loadCraftRecipeItem(xmlItem);
			xmlItem = xmlItem.next_sibling();
		}
	}
	// delete only after we are done parsing xml!
	delete [] fileBuffer;

	return true;
}

AchievementConfig* WeaponArmory::loadAchievement(pugi::xml_node& xmlAchievement)
{
	r3d_assert(!xmlAchievement.empty());

	const char* name = xmlAchievement.attribute("name").value();
	for(uint32_t i=0; i<m_NumAchievementLoaded; ++i) // todo: change to hash table
	{
		if(strcmp(m_AchievementArray[i]->name, name)==0)
		{
			r3dArtBug("Trying to load an achievement '%s' that is already loaded!", name);
			return NULL;
		}
	}
	if(m_NumAchievementLoaded > MAX_NUMBER_ACHIEVEMENT-1)
	{
		r3dArtBug("Trying to load more than maximum number of achievement. Maximum is '%d'\n", MAX_NUMBER_ACHIEVEMENT);
		return NULL;
	}
	
	AchievementConfig* ach= game_new AchievementConfig(xmlAchievement.attribute("id").as_int());
	ach->enabled = ( xmlAchievement.attribute("enabled").as_int() == 1 );
	ach->name = strdup(xmlAchievement.attribute("name").value());
	ach->desc= strdup(xmlAchievement.attribute("desc").value());
	ach->hud_icon = strdup(xmlAchievement.attribute("hudIcon").value());
	ach->value = xmlAchievement.attribute("value").as_int();

	m_AchievementArray[m_NumAchievementLoaded] = ach;
	m_NumAchievementLoaded++;

	return ach;
}

Ammo* WeaponArmory::loadAmmo(pugi::xml_node& xmlAmmo)
{
	r3d_assert(!xmlAmmo.empty());

	const char* ammoName = xmlAmmo.attribute("name").value();
	// check if we have that ammo in our database
	for(uint32_t i=0; i<m_NumAmmoLoaded; ++i) // todo: change to hash table
	{
		if(strcmp(m_AmmoArray[i]->m_Name, ammoName)==0)
		{
			r3dArtBug("Trying to load an ammo '%s' that is already loaded!", ammoName);
			return NULL;
		}
	}
	if(m_NumAmmoLoaded > MAX_NUMBER_AMMO-2)
	{
		r3dArtBug("Trying to load more than maximum number of ammo. Maximum is '%d'\n", MAX_NUMBER_AMMO);
		return NULL;
	}
	Ammo* ammo = game_new Ammo(ammoName);

	const char* particleTracer = xmlAmmo.child("ParticleTracer").attribute("file").value();
	if(particleTracer && particleTracer[0]!=0)
		ammo->m_ParticleTracer = strdup(particleTracer);

	const char* particleShellExtract = xmlAmmo.child("ShellExtractParticle").attribute("file").value();
	if(particleShellExtract && particleShellExtract[0]!=0)
		ammo->m_ShellExtractParticle = strdup(particleShellExtract);

	const char* modelName = xmlAmmo.child("Model").attribute("file").value();
	if(modelName && modelName[0]!=0)
		ammo->m_ModelPath = strdup(modelName);

	ammo->m_DecalSource = strdup(xmlAmmo.child("Decal").attribute("source").value());
	ammo->m_BulletClass = strdup(xmlAmmo.child("Bullet").attribute("class").value());

	m_AmmoArray[m_NumAmmoLoaded] = ammo;
	m_NumAmmoLoaded++;

	return ammo;
}

ScopeConfig* WeaponArmory::loadScope(pugi::xml_node& xmlScope)
{
	r3d_assert(!xmlScope.empty());

	const char* scopeName = xmlScope.attribute("name").value();
	// check if we have that scope in our database
	for(uint32_t i=0; i<m_NumScopeLoaded; ++i) // todo: change to hash table
	{
		if(strcmp(m_ScopeArray[i]->name, scopeName)==0)
		{
			r3dError("Trying to load a scope '%s' that is already loaded!", scopeName);
			return NULL;
		}
	}
	if(m_NumScopeLoaded > MAX_NUMBER_SCOPE-1)
	{
		r3dError("Trying to load more than maximum number of scope. Maximum is '%d'\n", MAX_NUMBER_SCOPE);
		return NULL;
	}
	ScopeConfig* scope = game_new ScopeConfig(scopeName);

	m_ScopeArray[m_NumScopeLoaded] = scope;
	m_NumScopeLoaded++;

	return scope;
}

WeaponConfig* WeaponArmory::loadWeapon(pugi::xml_node& xmlWeapon)
{
	r3d_assert(!xmlWeapon.empty());

	uint32_t itemID = xmlWeapon.attribute("itemID").as_uint();
	// check if we have that weapon in our database
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load a weapon with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	WeaponConfig* weapon = game_new WeaponConfig(itemID);
	
	// load base stuff, common for all versions
	if(!weapon->loadBaseFromXml(xmlWeapon))
	{
		delete weapon;
		return NULL;
	}

	// things specific to game version of WeaponArmory
	const char* bulletName = xmlWeapon.child("PrimaryFire").attribute("bullet").value();
	weapon->m_PrimaryAmmo = getAmmo(bulletName);
	if(weapon->m_PrimaryAmmo == NULL)
	{
		r3dArtBug("Failed to find ammo '%s'. Make sure that you added it into AmmoDB.xml\n", bulletName);
		delete weapon;
		return NULL;
	}

	weapon->m_scopeConfig = getScopeConfig(xmlWeapon.child("PrimaryFire").attribute("ScopeType").value());
	if(weapon->m_scopeConfig == 0)
	{
		r3dArtBug("Weapon '%s' has no scope config!!\n", weapon->m_StoreName);
		delete weapon;
		return NULL;
	}

#ifndef WO_SERVER	
#ifdef DEBUG
	switch(weapon->category)
	{
		case storecat_UsableItem:
		case 24: //SUP
		case 27: //storecat_Grenades
		case storecat_MELEE:
			break;
		default:
			// validate that it has default clip attachments
			const WeaponAttachmentConfig* clipAttach = getAttachmentConfig(weapon->FPSDefaultID[WPN_ATTM_CLIP]);
			if(!clipAttach) {
				r3dArtBug("Weapon '%s' does not have default clip attachment!\n", weapon->m_StoreName);
				break;
			}
			if(clipAttach->m_Clipsize <= 0) {
				r3dArtBug("Weapon '%s' default clip attachment %d have 0 clipsize!\n", weapon->m_StoreName, weapon->FPSDefaultID[WPN_ATTM_CLIP]);
			}
			break;
	}
#endif
#endif

	m_itemsHash.Add(itemID, weapon);
	m_NumWeaponsLoaded++;

	return weapon;
}

WeaponAttachmentConfig* WeaponArmory::loadWeaponAttachment(pugi::xml_node& xmlWeaponAttachment)
{
	r3d_assert(!xmlWeaponAttachment.empty());

	uint32_t itemID = xmlWeaponAttachment.attribute("itemID").as_uint();
	// check if we have that weapon in our database
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load a weapon attachment with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	WeaponAttachmentConfig* attm = game_new WeaponAttachmentConfig(itemID);

	// load base stuff, common for all versions
	if(!attm->loadBaseFromXml(xmlWeaponAttachment))
	{
		delete attm;
		return NULL;
	}
	attm->category = storecat_FPSAttachment; // in .xml it doesn't have category 

	const char* scopeType = xmlWeaponAttachment.child("Upgrade").attribute("ScopeType").value();
	attm->m_scopeConfig = getScopeConfig(scopeType);
	{
		size_t scopeTypeLen = strlen(scopeType);
		if(scopeTypeLen>4 && strcmp(&scopeType[scopeTypeLen-4], "_fps")==0)
		{
			char tmpStr[64] = {0};
			memcpy(tmpStr, scopeType, strlen(scopeType)-4);
			attm->m_scopeConfigTPS = getScopeConfig(tmpStr);
		}
		if(attm->m_scopeConfigTPS == NULL)
			attm->m_scopeConfigTPS = attm->m_scopeConfig;
	}

	m_itemsHash.Add(itemID, attm);
	m_NumWeaponAttmLoaded++;

	return attm;
}

GearConfig* WeaponArmory::loadGear(pugi::xml_node& xmlGear)
{
	r3d_assert(!xmlGear.empty());

	uint32_t itemID = xmlGear.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load a gear with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	GearConfig* gear = game_new GearConfig(itemID);
	// load base stuff, common for all versions
	if(!gear->loadBaseFromXml(xmlGear))
	{
		delete gear;
		return NULL;
	}

	m_itemsHash.Add(itemID, gear);
	m_NumGearLoaded++;

	return gear;
}

BackpackConfig* WeaponArmory::loadBackback(pugi::xml_node& xmlItem)
{
	r3d_assert(!xmlItem.empty());

	uint32_t itemID = xmlItem.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load a backpack with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	BackpackConfig* backpack = game_new BackpackConfig(itemID);
	// load base stuff, common for all versions
	if(!backpack->loadBaseFromXml(xmlItem))
	{
		delete backpack;
		return NULL;
	}

	m_itemsHash.Add(itemID, backpack);
	m_NumBackpackLoaded++;

	return backpack;
}

HeroConfig* WeaponArmory::loadHero(pugi::xml_node& xmlHero)
{
	r3d_assert(!xmlHero.empty());

	uint32_t itemID = xmlHero.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load a hero with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	HeroConfig* hero = game_new HeroConfig(itemID);
	// load base stuff, common for all versions
	if(!hero->loadBaseFromXml(xmlHero))
	{
		delete hero;
		return NULL;
	}

	m_itemsHash.Add(itemID, hero);
	m_NumHeroLoaded++;

	return hero;
}

ModelItemConfig* WeaponArmory::loadItem(pugi::xml_node& xmlItem)
{
	r3d_assert(!xmlItem.empty());

	uint32_t itemID = xmlItem.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load an item with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	ModelItemConfig* item = game_new ModelItemConfig(itemID);
	item->category = (STORE_CATEGORIES)xmlItem.attribute("category").as_int();

	// load base stuff, common for all versions
	if(!item->loadBaseFromXml(xmlItem))
	{
		delete item;
		return NULL;
	}

	m_itemsHash.Add(itemID, item);
	m_NumItemLoaded++;

	return item;
}

static void renormalizeLootBox(LootBoxConfig* lootBox)
{
	if(lootBox->entries.size() == 0)
		return;
		
	double sum = 0.0;
	for(size_t i=0; i<lootBox->entries.size(); i++)
		sum += lootBox->entries[i].chance;
	for(size_t i=0; i<lootBox->entries.size(); i++)
		lootBox->entries[i].chance /= sum;

	// bring to [0..1]
	sum = lootBox->entries[0].chance;
        for(size_t i=1; i<lootBox->entries.size(); i++)
        {
            double chance = lootBox->entries[i].chance;
            lootBox->entries[i].chance += sum;
            sum += chance;
        }
        
        return;
}

void WeaponArmory::updateLootBoxContent(pugi::xml_node& xmlLootBox, int srvLootType)
{
	xmlLootBox = xmlLootBox.first_child();
	while(!xmlLootBox.empty())
	{
		uint32_t itemId = xmlLootBox.attribute("itemID").as_uint();
		LootBoxConfig* lootBox = const_cast<LootBoxConfig*>(getLootBoxConfig(itemId));
		if(lootBox == NULL)
		{
			r3dOutToLog("There is no LootBox %d\n", itemId);
			xmlLootBox = xmlLootBox.next_sibling();
			continue;
		}
		
		double lootMods[3];
		lootMods[0] = xmlLootBox.attribute("SrvNormal").as_double() / 100.0;
		lootMods[1] = xmlLootBox.attribute("SrvTrial").as_double() / 100.0;
		lootMods[2] = xmlLootBox.attribute("SrvPremium").as_double() / 100.0;
		r3d_assert(srvLootType >= 0 && srvLootType <= 2);
		r3d_assert(lootMods[0] > 0.0);
		r3d_assert(lootMods[1] > 0.0);
		r3d_assert(lootMods[2] > 0.0);

		lootBox->entries.clear();
		pugi::xml_node xmlContent = xmlLootBox.first_child();
		while(!xmlContent.empty())
		{
			LootBoxConfig::LootEntry le;
			le.chance = xmlContent.attribute("c").as_double();
			le.itemID = xmlContent.attribute("i").as_uint();
			le.GDMin  = xmlContent.attribute("g1").as_int();
			le.GDMax  = xmlContent.attribute("g2").as_int();
			if(le.itemID != -1) le.chance *= lootMods[srvLootType]; // adjust chance for actual items (-1 is "no item")
			lootBox->entries.push_back(le);
		
			xmlContent = xmlContent.next_sibling();
		}
		
		// renormalize lootbox chances
		renormalizeLootBox(lootBox);

		xmlLootBox = xmlLootBox.next_sibling();
	}
}

LootBoxConfig* WeaponArmory::loadLootBox(pugi::xml_node& xmlItem)
{
	r3d_assert(!xmlItem.empty());

	uint32_t itemID = xmlItem.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load lootbox with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	LootBoxConfig* lootBox = game_new LootBoxConfig(itemID);
	lootBox->loadBaseFromXml(xmlItem);

	m_itemsHash.Add(itemID, lootBox);
	m_NumLootBoxLoaded++;

	return lootBox;
}

FoodConfig* WeaponArmory::loadFoodItem(pugi::xml_node& xmlItem)
{
	r3d_assert(!xmlItem.empty());

	uint32_t itemID = xmlItem.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load food item with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	FoodConfig* food = game_new FoodConfig(itemID);
	food->loadBaseFromXml(xmlItem);

	m_itemsHash.Add(itemID, food);
	m_NumFoodItemsLoaded++;

	return food;
}

VehicleInfoConfig* WeaponArmory::loadVehicleItem(pugi::xml_node& xmlItem)
{
	r3d_assert(!xmlItem.empty());

	uint32_t itemID = xmlItem.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load vehicle item with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}
	
	VehicleInfoConfig* veh= game_new VehicleInfoConfig(itemID);
	veh->loadBaseFromXml(xmlItem);

	m_itemsHash.Add(itemID, veh);
	m_NumVehiclesLoaded++;

	return veh;
}

CraftComponentConfig* WeaponArmory::loadCraftComponentItem(pugi::xml_node& xmlItem)
{
	r3d_assert(!xmlItem.empty());

	uint32_t itemID = xmlItem.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load craft component item with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	CraftComponentConfig* comp = game_new CraftComponentConfig(itemID);
	comp->loadBaseFromXml(xmlItem);

	m_itemsHash.Add(itemID, comp);
	m_NumCraftComponentsLoaded++;

	return comp;
}

CraftRecipeConfig* WeaponArmory::loadCraftRecipeItem(pugi::xml_node& xmlItem)
{
	r3d_assert(!xmlItem.empty());

	uint32_t itemID = xmlItem.attribute("itemID").as_uint();
	{
		BaseItemConfig* temp = NULL;
		if(m_itemsHash.GetObject(itemID, &temp))
		{
			r3dArtBug("Trying to load craft recipe item with id '%d' that is already loaded!", itemID);
			return NULL;
		}
	}

	CraftRecipeConfig* recipe = game_new CraftRecipeConfig(itemID);
	recipe->loadBaseFromXml(xmlItem);

	m_itemsHash.Add(itemID, recipe);
	m_NumCraftRecipesLoaded++;

	return recipe;
}

void WeaponArmory::Destroy()
{
	m_itemsHash.IterateStart();
	while(m_itemsHash.IterateNext())
	{
		const BaseItemConfig* item = m_itemsHash.IterateGet();
		delete item;
		item = NULL;
	}

	m_NumLootBoxLoaded = 0;
	m_NumCraftComponentsLoaded = 0;
	m_NumCraftRecipesLoaded = 0;
	m_NumFoodItemsLoaded = 0;
	m_NumVehiclesLoaded = 0;
	m_NumItemLoaded = 0;
	m_NumGearLoaded = 0;
	m_NumBackpackLoaded = 0;
	m_NumHeroLoaded = 0;
	m_NumWeaponsLoaded = 0;
	m_NumWeaponAttmLoaded = 0;

	for(uint32_t i=0; i<m_NumAmmoLoaded; ++i)
	{
		delete m_AmmoArray[i];
		m_AmmoArray[i] = NULL;
	}
	m_NumAmmoLoaded = 0;
	for(uint32_t i=0; i<m_NumScopeLoaded; ++i)
	{
		delete m_ScopeArray[i];
		m_ScopeArray[i] = NULL;
	}
	m_NumScopeLoaded = 0;
	for(uint32_t i=0; i<m_NumAchievementLoaded; ++i)
	{
		delete m_AchievementArray[i];
		m_AchievementArray[i] = NULL;
	}
	m_NumAchievementLoaded= 0;
}

void WeaponArmory::UnloadMeshes()
{
}

const WeaponConfig* WeaponArmory::getWeaponConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category >= storecat_ASR && item->category <= storecat_MELEE)
			return (WeaponConfig*)item;
	}
	return NULL;
}

const FoodConfig* WeaponArmory::getFoodConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_Food || item->category == storecat_Water)
			return (FoodConfig*)item;
	}
	return NULL;
}

const VehicleInfoConfig* WeaponArmory::getVehicleConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_Vehicle)
			return (VehicleInfoConfig*)item;
	}
	return NULL;
}

const CraftComponentConfig* WeaponArmory::getCraftComponentConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_Components)
			return (CraftComponentConfig*)item;
	}
	return NULL;
}

const CraftRecipeConfig* WeaponArmory::getCraftRecipeConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_CraftRecipe)
			return (CraftRecipeConfig*)item;
	}
	return NULL;
}

const WeaponAttachmentConfig* WeaponArmory::getAttachmentConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_FPSAttachment)
			return (WeaponAttachmentConfig*)item;
	}
	return NULL;
}

const GearConfig* WeaponArmory::getGearConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_Armor || item->category == storecat_Helmet)
			return (GearConfig*)item;
	}
	return NULL;
}

const BackpackConfig* WeaponArmory::getBackpackConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_Backpack)
			return (BackpackConfig*)item;
	}
	return NULL;
}

const HeroConfig* WeaponArmory::getHeroConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_HeroPackage)
			return (HeroConfig*)item;
	}
	return NULL;
}

const ModelItemConfig* WeaponArmory::getAccountItemConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category >= storecat_Account && item->category <= storecat_LootBox)
			return (ModelItemConfig*)item;
	}
	return NULL;
}

const LootBoxConfig* WeaponArmory::getLootBoxConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	if(m_itemsHash.GetObject(itemID, &item))
	{
		if(item->category == storecat_LootBox)
			return (const LootBoxConfig*)item;
	}
	return NULL;
}

const BaseItemConfig* WeaponArmory::getConfig(uint32_t itemID)
{
	BaseItemConfig* item = NULL;
	m_itemsHash.GetObject(itemID, &item);
	return item;
}

const ScopeConfig* WeaponArmory::getScopeConfig(const char* name)
{
	for(uint32_t i=0; i<m_NumScopeLoaded; ++i)
	{
		if(strcmp(m_ScopeArray[i]->name, name)==0)
		{
			return m_ScopeArray[i];
		}
	}
	return NULL;
}

const AchievementConfig* WeaponArmory::getAchievementConfig(const char* name)
{
	for(uint32_t i=0; i<m_NumAchievementLoaded; ++i)
	{
		if(strcmp(m_AchievementArray[i]->name, name)==0)
		{
			return m_AchievementArray[i];
		}
	}
	return NULL;
}

Gear* WeaponArmory::createGear(uint32_t itemID)
{
	if(itemID == 0)
		return NULL;

	const GearConfig* gc = getGearConfig(itemID);
	if(gc)
		return game_new Gear(gc);

	r3dError("Failed to get gear with ID %d\nVersion mismatch!\n", itemID);
	return NULL;
}

Ammo* WeaponArmory::getAmmo(const char* ammoName)
{
	for(uint32_t i=0; i<m_NumAmmoLoaded; ++i)
	{
		if(strcmp(m_AmmoArray[i]->m_Name, ammoName) == 0)
			return m_AmmoArray[i];
	}
	return NULL;
}

void WeaponArmory::startItemSearch()
{
	m_itemsHash.IterateStart();
}

uint32_t WeaponArmory::getCurrentSearchItemID()
{
	return m_itemsHash.IterateGetKey();
}

bool WeaponArmory::searchNextItem()
{
	return m_itemsHash.IterateNext();
}

uint32_t WeaponArmory::getNumItemsInHash()
{
	return m_itemsHash.Size();
}

const AchievementConfig* WeaponArmory::getAchievementByIndex(uint32_t index)
{
	r3d_assert(/*index >= 0 &&*/ index < m_NumAchievementLoaded);
	return m_AchievementArray[index];
}

const AchievementConfig* WeaponArmory::getAchievementByID(uint32_t ID)
{
	for( uint32_t achievementIndex = 0; achievementIndex < m_NumAchievementLoaded; achievementIndex++ )
	{
		if( m_AchievementArray[achievementIndex]->id == ID )
		{
			return m_AchievementArray[achievementIndex];
		}
	}

	return NULL;
}
/*
void WeaponArmory::dumpStats()
{
#ifndef WO_SERVER
#ifndef FINAL_BUILD
	FILE* fout = fopen( "playermeshes.txt", "wt" ) ;
	
	char buf[ 4096 ] ;

	void ConPrint( const char*, ... ) ;

	for( int i = 0, e = (int)m_NumAmmoLoaded  ; i < e ; i ++ )
	{
		Ammo* ammo = m_AmmoArray[ i ] ;

		if( int refs = ammo->getModelRefCount() )
		{
			sprintf( buf, "Ammo: %s with %d references\n", ammo->getName(), refs ) ;
			fputs( buf, fout ) ;
			ConPrint( "%s", buf ) ;
		}
	}

	for( int i = 0, e = (int)m_NumWeaponsLoaded ; i < e ; i ++ )
	{
		WeaponConfig* cfg = m_WeaponArray[ i ] ;

		int refs = cfg->getMeshRefs() ;
		int cfgRefs = cfg->getConfigMeshRefs() ;
		
		if( refs || cfgRefs )
		{			
			sprintf( buf, "Weapon: %s with %d references, %d mesh references\n", cfg->m_StoreName, cfgRefs, refs ) ;
			fputs( buf, fout ) ;
			ConPrint( "%s", buf ) ;
		}
	}

	for( int i = 0, e = (int)m_NumWeaponAttmLoaded ; i < e ; i ++ )
	{
		WeaponAttachmentConfig* att = m_WeaponAttmArray[ i ] ;

		int refs = att->getMeshRefs() ;
		int aimRefs = att->getAimMeshRefs() ;

		if( refs || aimRefs )
		{
			sprintf( buf, "WeaponAttachment: %s with %d references, %d aim references\n", att->m_StoreName, refs, aimRefs ) ;
			fputs( buf, fout ) ;
			ConPrint( "%s", buf ) ;
		}
	}

	for( int i = 0, e = (int)m_NumGearLoaded ; i < e ; i ++ )
	{
		GearConfig* gear = m_GearArray[ i ] ;

		int refs = gear->getMeshRefs() ;
		int cfgRefs = gear->getConfigMeshRefs() ;

		if( refs || cfgRefs )
		{
			sprintf( buf, "Gear: %s with %d references, %d fps references\n", gear->m_StoreName, cfgRefs, refs ) ;
			fputs( buf, fout ) ;
			ConPrint( "%s", buf ) ;
		}
	}

	for( int i = 0, e = (int)m_NumItemLoaded  ; i < e ; i ++ )
	{
		ModelItemConfig* itm = m_ItemArray[ i ] ;

		int refs = itm->getMeshRefs() ;

		if( refs )
		{
			sprintf( buf, "Item: %s with %d references\n", itm->m_StoreName, refs ) ;
			fputs( buf, fout ) ;
			ConPrint( "%s", buf ) ;
		}
	}

	extern int g_WeaponBalance ;
	extern int g_GearBalance ;

	ConPrint( "WB: %d, GB: %d\n", g_WeaponBalance, g_GearBalance ) ;

	fclose( fout ) ;
#endif
#endif
}*/

void DumpArmoryStats()
{
	//gWeaponArmory.dumpStats() ;
}