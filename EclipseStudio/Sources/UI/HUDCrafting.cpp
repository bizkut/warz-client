#include "r3dPCH.h"
#include "r3d.h"

#include "../../../Eternity/sf/Console/config.h"
#include "HUDCrafting.h"
#include "LangMngr.h"

#include "FrontendShared.h"

#include "../multiplayer/clientgamelogic.h"
#include "../ObjectsCode/AI/AI_Player.H"
#include "../ObjectsCode/weapons/Weapon.h"
#include "../ObjectsCode/weapons/WeaponArmory.h"
#include "../GameLevel.h"

HUDCrafting::HUDCrafting()
{
	isActive_ = false;
	isInit = false;
	prevKeyboardCaptureMovie = NULL;
}

HUDCrafting::~HUDCrafting()
{
}

void HUDCrafting::eventCraftItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	uint32_t recipeID = args[0].GetUInt();
	Scaleform::GFx::Value var[2];

	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);
	// check if we have enough components
	{
		const CraftRecipeConfig* cfg = g_pWeaponArmory->getCraftRecipeConfig(recipeID);
		r3d_assert(cfg);  
		bool notEnough = false;
		for(uint32_t i=0; i<cfg->numComponents; ++i)
		{
			if(cfg->components[i].itemID == 301386)
			{
				notEnough = gUserProfile.ProfileData.ResMetal < int(cfg->components[i].quantity);
				break;
			}
			else if(cfg->components[i].itemID == 301387)
			{
				notEnough = gUserProfile.ProfileData.ResStone < int(cfg->components[i].quantity);
				break;
			}
			else if(cfg->components[i].itemID == 301388)
			{
				notEnough = gUserProfile.ProfileData.ResWood < int(cfg->components[i].quantity);
				break;
			}
			else if(!plr->CurLoadout.hasItemWithQuantity(cfg->components[i].itemID, cfg->components[i].quantity))
			{
				notEnough = true;
				break;
			}
		}
		if(notEnough)
		{
			var[0].SetString(gLangMngr.getString("FailedToCraftItemNotEnoughComponents"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return;
		}
	}

	// lock UI
	isLockedUI = true;
	var[0].SetString("");
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

 	PKT_C2S_CraftItem_s n3;
 	n3.recipeID= recipeID;
 	p2pSendToHost(gClientLogic().localPlayer_, &n3, sizeof(n3));
}


void HUDCrafting::eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Deactivate();
}

void HUDCrafting::setErrorMsg(const char* errMsg)
{
	if(isLockedUI)
	{
		isLockedUI = false;
		lockedOp = 0;
		gfxMovie.Invoke("_root.api.hideInfoMsg", "");
		Scaleform::GFx::Value var[3];
		var[0].SetString(errMsg);
		var[1].SetBoolean(true);
		var[2].SetString("ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
}

void HUDCrafting::unlockUI()
{
	r3d_assert(isLockedUI);
	isLockedUI = false;
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	lockedOp = 0;
}

bool HUDCrafting::Init()
{
	if(!gfxMovie.Load("Data\\Menu\\WarZ_HUD_CraftingMenu.swf", false)) 
		return false;

#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<HUDCrafting>(this, &HUDCrafting::FUNC)
	gfxMovie.RegisterEventHandler("eventCraftItem", MAKE_CALLBACK(eventCraftItem));
	gfxMovie.RegisterEventHandler("eventReturnToGame", MAKE_CALLBACK(eventReturnToGame));

	gfxMovie.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );

	// add items
	{
		addItemCategoriesToUI(gfxMovie);
		addItemsAndCategoryToUI(gfxMovie);
	}

	isActive_ = false;
	isInit = true;
	isLockedUI = false;
	lockedOp = 0;
	return true;
}

void HUDCrafting::refreshRecipeList()
{
	if(!isInit) return;

	obj_Player* plr = gClientLogic().localPlayer_;
	if(!plr) return;

	gfxMovie.Invoke("_root.api.clearRecipes", "");
	
	Scaleform::GFx::Value var[4];
	for(int i=0; i<plr->CurLoadout.NumRecipes; ++i)
	{
		const CraftRecipeConfig* cfg = g_pWeaponArmory->getCraftRecipeConfig(plr->CurLoadout.RecipesLearned[i]);
		r3d_assert(cfg);
		var[0].SetUInt(cfg->m_itemID);
		var[1].SetString(cfg->m_StoreName);
		var[2].SetString(cfg->m_Description);
		var[3].SetString(g_pWeaponArmory->getConfig(cfg->craftedItemID)->m_StoreIcon);
		//public function addRecipe(recipeID:uint, name:String, desc:String, icon:String)
		gfxMovie.Invoke("_root.api.addRecipe", var, 4);

		for(uint32_t k=0; k<cfg->numComponents; ++k)
		{
			var[1].SetUInt(cfg->components[k].itemID);
			var[2].SetUInt(cfg->components[k].quantity);
			//public function addRecipeComponent(recipeID:uint, compID:uint, quantity:uint)
			gfxMovie.Invoke("_root.api.addRecipeComponent", var, 3);
		}
	}

	{
		var[0].SetInt(gUserProfile.ProfileData.ResWood);
		var[1].SetInt(gUserProfile.ProfileData.ResStone);
		var[2].SetInt(gUserProfile.ProfileData.ResMetal);
		gfxMovie.Invoke("_root.api.setResources", var, 3);
	}
}

bool HUDCrafting::Unload()
{
	gfxMovie.Unload();
	isActive_ = false;
	isInit = false;
	return true;
}

void HUDCrafting::Update()
{
}

void HUDCrafting::Draw()
{
	gfxMovie.UpdateAndDraw();
}

void HUDCrafting::Deactivate()
{
	if(isLockedUI)
		return;

	if(prevKeyboardCaptureMovie)
	{
		prevKeyboardCaptureMovie->SetKeyboardCapture();
		prevKeyboardCaptureMovie = NULL;
	}

	gfxMovie.Invoke("_root.api.hideCraftScreen", "");

	{
		Scaleform::GFx::Value var[1];
		var[0].SetString("menu_close");
		gfxMovie.OnCommandCallback("eventSoundPlay", var, 1);
	}

	if( !g_cursor_mode->GetInt() )
	{
		r3dMouse::Hide();
	}

	isActive_ = false;
}

extern const char* getReputationString(int reputation);
void HUDCrafting::Activate()
{
	r3d_assert(!isActive_);
	r3dMouse::Show();
	isActive_ = true;

	prevKeyboardCaptureMovie = gfxMovie.SetKeyboardCapture(); // for mouse scroll events

	// add player info
	{
		Scaleform::GFx::Value var[22];

		obj_Player* plr = gClientLogic().localPlayer_;
		r3d_assert(plr);

		// reset barricade placement if going into UI
		plr->m_needToDrawBarricadePlacement = 0; 
		plr->m_BarricadeMeshPlacement = NULL;

		wiCharDataFull& slot = plr->CurLoadout;
		char tmpGamertag[128];
		if(plr->ClanID != 0)
			sprintf(tmpGamertag, "[%s] %s", plr->ClanTag, slot.Gamertag);
		else
			r3dscpy(tmpGamertag, slot.Gamertag);
		var[0].SetString(tmpGamertag);
		var[1].SetNumber(slot.Health);
		var[2].SetNumber(slot.Stats.XP);
		var[3].SetNumber(slot.Stats.TimePlayed);
		var[4].SetNumber(slot.Hardcore);
		var[5].SetNumber(slot.HeroItemID);
		var[6].SetNumber(slot.HeadIdx);
		var[7].SetNumber(slot.BodyIdx);
		var[8].SetNumber(slot.LegsIdx);
		var[9].SetNumber(slot.Alive);
		var[10].SetNumber(slot.Hunger);
		var[11].SetNumber(slot.Thirst);
		var[12].SetNumber(slot.Toxic);
		var[13].SetNumber(slot.BackpackID);
		var[14].SetNumber(slot.BackpackSize);

		float totalWeight = slot.getTotalWeight();
		if(slot.Skills[CUserSkills::SKILL_Physical3])
			totalWeight *= 0.95f;
		if(slot.Skills[CUserSkills::SKILL_Physical7])
			totalWeight *= 0.9f;

		var[15].SetNumber(totalWeight);		// weight
		var[16].SetNumber(0);		// zombies Killed
		var[17].SetNumber(0);		// bandits killed
		var[18].SetNumber(0);		// civilians killed
		var[19].SetString(getReputationString(slot.Stats.Reputation));	// alignment
		var[20].SetString("");	// last Map
		var[21].SetBoolean(true); // global inventory

		gfxMovie.Invoke("_root.api.addClientSurvivor", var, 22);
	}

	refreshRecipeList();
	reloadBackpackInfo();

	updateSurvivorTotalWeight();

	{
		Scaleform::GFx::Value var[1];
		var[0].SetString("menu_open");
		gfxMovie.OnCommandCallback("eventSoundPlay", var, 1);
	}

	gfxMovie.Invoke("_root.api.showCraftScreen", "");
}

void HUDCrafting::updateSurvivorTotalWeight()
{
	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);

	float totalWeight = plr->CurLoadout.getTotalWeight();
	if(plr->CurLoadout.Skills[CUserSkills::SKILL_Physical3])
		totalWeight *= 0.95f;
	if(plr->CurLoadout.Skills[CUserSkills::SKILL_Physical7])
		totalWeight *= 0.9f;

	Scaleform::GFx::Value var[2];
	wiCharDataFull& slot = plr->CurLoadout;
	char tmpGamertag[128];
	if(plr->ClanID != 0)
		sprintf(tmpGamertag, "[%s] %s", plr->ClanTag, slot.Gamertag);
	else
		r3dscpy(tmpGamertag, slot.Gamertag);

	var[0].SetString(tmpGamertag);
	var[1].SetNumber(totalWeight);
	gfxMovie.Invoke("_root.api.updateClientSurvivorWeight", var, 2);
}

void HUDCrafting::reloadBackpackInfo()
{
	updateSurvivorTotalWeight();
	// reset backpack
	{
		gfxMovie.Invoke("_root.api.clearBackpack", "");
		gfxMovie.Invoke("_root.api.clearBackpacks", "");
	}

	r3dgameVector(uint32_t) uniqueBackpacks; // to filter identical backpack
	int backpackSlotIDInc = 0;
	// add backpack content info
	{
		obj_Player* plr = gClientLogic().localPlayer_;
		r3d_assert(plr);
		wiCharDataFull& slot = plr->CurLoadout;

		Scaleform::GFx::Value var[7];
		for (int a = 0; a < slot.BackpackSize; a++)
		{
			if (slot.Items[a].itemID != 0)
			{
				var[0].SetInt(a);
				var[1].SetUInt(0); // not used for game
				var[2].SetUInt(slot.Items[a].itemID);
				var[3].SetInt(slot.Items[a].quantity);
				var[4].SetInt(slot.Items[a].Var1);
				var[5].SetInt(slot.Items[a].Var2);
				var[6].SetString(getAdditionalDescForItem(slot.Items[a].itemID, slot.Items[a].Var1, slot.Items[a].Var2, slot.Items[a].Var3));
				gfxMovie.Invoke("_root.api.addBackpackItem", var, 7);
			}
		}
	}

	gfxMovie.Invoke("_root.api.Main.refreshRecipeList", "");
}
