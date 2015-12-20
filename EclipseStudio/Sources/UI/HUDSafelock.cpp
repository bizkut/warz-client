#include "r3dPCH.h"
#include "r3d.h"

#include "../../../Eternity/sf/Console/config.h"
#include "HUDSafelock.h"
#include "HUDDisplay.h"
#include "HUDAttachments.h"
#include "LangMngr.h"

#include "FrontendShared.h"

#include "../multiplayer/clientgamelogic.h"
#include "../ObjectsCode/AI/AI_Player.H"
#include "../ObjectsCode/weapons/Weapon.h"
#include "../ObjectsCode/weapons/WeaponArmory.h"
#include "../GameLevel.h"

extern HUDAttachments*	hudAttm;
extern HUDDisplay* hudMain;

HUDSafelock::HUDSafelock()
{
	isActive_ = false;
	isInit = false;
	prevKeyboardCaptureMovie = NULL;
	CurrentAccessCodeString[0]=0;
}

HUDSafelock::~HUDSafelock()
{
}

void HUDSafelock::eventBackpackGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	int gridFrom = args[0].GetInt();
	int gridTo = args[1].GetInt();

	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);
	plr->BackpackGridSwap(gridFrom, gridTo);

	updateSurvivorTotalWeight();

	gfxMovie.Invoke("_root.api.backpackGridSwapSuccess", "");
}

void HUDSafelock::eventBackpackFromInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	//inventoryID:uint, gridTo:int, amount:int
	r3d_assert(argCount == 3);
	uint32_t inventoryID = args[0].GetUInt();
	int gridTo = args[1].GetInt();
	int amount = args[2].GetInt();

	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);

	r3d_assert(isLockedUI==false);
	isLockedUI = true;
	lockedOp = 1;

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);


	int itemID = plr->CurLoadout.Items[gridTo].itemID;

	if (itemID == 103025) //UAV
	{
		const wiCharDataFull& slot = plr->CurLoadout;
		for(int i=0; i<slot.BackpackSize; ++i)
		{
			if (slot.Items[i].itemID == itemID)
			{
				Scaleform::GFx::Value var[3];
				var[0].SetString(gLangMngr.getString("$NoMoreADroneOnBackpack"));
				var[1].SetBoolean(true);
				var[2].SetString("");
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
				return;
			}
		}
		amount = 1;
	}

	PKT_C2S_LockboxItemLockboxToBackpack_s n;
	n.lockboxID = CurrentLockboxID;
	r3dscpy(n.AccessCodeS, CurrentAccessCodeString);
	n.LockboxInventoryID = inventoryID;
	n.SlotTo             = gridTo == -1 ? 0xFF : gridTo;		// or 0xFF if server should decide it
	n.Amount             = amount;
	if(n.SlotTo != 0xFF)
	{
		n.dbg_ItemID      = plr->CurLoadout.Items[n.SlotTo].itemID;
		n.dbg_Quantity    = plr->CurLoadout.Items[n.SlotTo].quantity;
	}
	else
	{
		n.dbg_ItemID      = 0;
		n.dbg_Quantity    = 0;
	}

	n.dbg_InvItemID = 0;
	n.dbg_InvQuantity = 0;
	for(uint32_t i=0; i<NumItems; ++i)
	{
		if(LockboxContent[i].InventoryID == inventoryID)
		{
			n.dbg_InvItemID   = LockboxContent[i].itemID;
			n.dbg_InvQuantity = LockboxContent[i].quantity;
			break;
		}
	}
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

void HUDSafelock::eventBackpackToInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	//gridFrom:int, amount:int
	r3d_assert(argCount == 2);
	int gridFrom = args[0].GetInt();
	int amount = args[1].GetInt();

	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);

	r3d_assert(isLockedUI==false);
	isLockedUI = true;
	lockedOp = 2;

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	PKT_C2S_LockboxItemBackpackToLockbox_s n;
	n.lockboxID = CurrentLockboxID;
	r3dscpy(n.AccessCodeS, CurrentAccessCodeString);
	n.SlotFrom     = gridFrom;
	n.Amount       = amount; 
	n.dbg_ItemID   = plr->CurLoadout.Items[n.SlotFrom].itemID;
	n.dbg_Quantity = plr->CurLoadout.Items[n.SlotFrom].quantity;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

void HUDSafelock::eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Deactivate();
}

void HUDSafelock::eventChangeKeyCode(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Deactivate();
	hudMain->showSafelockPin(false, CurrentLockboxID, true);
}

void HUDSafelock::eventPickupLockbox(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	if(NumItems > 0)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("HUD_Safelock_LockboxShouldBeEmpty"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	isLockedUI = true;
	lockedOp   = 0;

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	
	PKT_C2S_LockboxPickup_s n;
	n.lockboxID  = CurrentLockboxID;
	r3dscpy(n.AccessCodeS, CurrentAccessCodeString);
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

void HUDSafelock::setErrorMsg(const char* errMsg)
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

void HUDSafelock::unlockUI()
{
	r3d_assert(isLockedUI);
	isLockedUI = false;
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	if(lockedOp == 0)
	{
	}
	else if(lockedOp==1)
	{
		gfxMovie.Invoke("_root.api.backpackFromInventorySuccess", "");
	}
	else if(lockedOp==2)
	{
		gfxMovie.Invoke("_root.api.backpackToInventorySuccess", "");
	}
	else
		r3d_assert(false);

	lockedOp = 0;
}

bool HUDSafelock::Init()
{
	if(!gfxMovie.Load("Data\\Menu\\WarZ_HUD_Safelock.swf", false)) 
		return false;

#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<HUDSafelock>(this, &HUDSafelock::FUNC)
	gfxMovie.RegisterEventHandler("eventBackpackFromInventory", MAKE_CALLBACK(eventBackpackFromInventory));
	gfxMovie.RegisterEventHandler("eventBackpackToInventory", MAKE_CALLBACK(eventBackpackToInventory));
	gfxMovie.RegisterEventHandler("eventBackpackGridSwap", MAKE_CALLBACK(eventBackpackGridSwap));
	gfxMovie.RegisterEventHandler("eventReturnToGame", MAKE_CALLBACK(eventReturnToGame));
	gfxMovie.RegisterEventHandler("eventChangeKeyCode", MAKE_CALLBACK(eventChangeKeyCode));
	gfxMovie.RegisterEventHandler("eventPickupLockbox", MAKE_CALLBACK(eventPickupLockbox));

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
	NumItems = 0;
	return true;
}

bool HUDSafelock::Unload()
{
	gfxMovie.Unload();
	isActive_ = false;
	isInit = false;
	return true;
}

void HUDSafelock::Update()
{
}

void HUDSafelock::Draw()
{
	gfxMovie.UpdateAndDraw();
}

void HUDSafelock::Deactivate()
{
	r3dOutToLog("isLocked: %d\n", isLockedUI);
	if(isLockedUI)
		return;

	if(prevKeyboardCaptureMovie)
	{
		prevKeyboardCaptureMovie->SetKeyboardCapture();
		prevKeyboardCaptureMovie = NULL;
	}

	gfxMovie.Invoke("_root.api.hideInventoryScreen", "");

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
void HUDSafelock::Activate()
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

	reloadLockboxInfo();

	reloadBackpackInfo();

	updateSurvivorTotalWeight();

	{
		Scaleform::GFx::Value var[1];
		var[0].SetString("menu_open");
		gfxMovie.OnCommandCallback("eventSoundPlay", var, 1);
	}

	gfxMovie.Invoke("_root.api.showInventoryScreen", "");
}

void HUDSafelock::reloadLockboxInfo()
{
	Scaleform::GFx::Value var[7];
	// clear inventory DB
	gfxMovie.Invoke("_root.api.clearInventory", NULL, 0);

	// add all items
	for(uint32_t i=0; i<NumItems; ++i)
	{
		if(LockboxContent[i].itemID == 0)
			continue;

		var[0].SetUInt(uint32_t(LockboxContent[i].InventoryID));
		var[1].SetUInt(LockboxContent[i].itemID);
		var[2].SetNumber(LockboxContent[i].quantity);
		var[3].SetNumber(LockboxContent[i].Var1);
		var[4].SetNumber(LockboxContent[i].Var2);
		bool isConsumable = false;
		{
			const WeaponConfig* wc = g_pWeaponArmory->getWeaponConfig(LockboxContent[i].itemID);
			if(wc && wc->category == storecat_UsableItem && wc->m_isConsumable)
				isConsumable = true;
		}
		var[5].SetBoolean(isConsumable);
		var[6].SetString(getAdditionalDescForItem(LockboxContent[i].itemID, LockboxContent[i].Var1, LockboxContent[i].Var2, LockboxContent[i].Var3));
		gfxMovie.Invoke("_root.api.addInventoryItem", var, 7);
	}
}

void HUDSafelock::updateSurvivorTotalWeight()
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

void HUDSafelock::reloadBackpackInfo()
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

				const BackpackConfig* bpc = g_pWeaponArmory->getBackpackConfig(slot.Items[a].itemID);
				if(bpc)
				{
					if(std::find<r3dgameVector(uint32_t)::iterator, uint32_t>(uniqueBackpacks.begin(), uniqueBackpacks.end(), slot.Items[a].itemID) != uniqueBackpacks.end())
						continue;

					// add backpack info
					var[0].SetInt(backpackSlotIDInc++);
					var[1].SetUInt(slot.Items[a].itemID);
					gfxMovie.Invoke("_root.api.addBackpack", var, 2);

					uniqueBackpacks.push_back(slot.Items[a].itemID);
				}
			}
		}
	}

	gfxMovie.Invoke("_root.api.Main.showBackpack", "");
}
