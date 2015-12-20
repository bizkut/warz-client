#include "r3dPCH.h"
#include "r3d.h"

#include "../../../Eternity/sf/Console/config.h"
#include "HUDRepair.h"
#include "LangMngr.h"

#include "FrontendShared.h"

#include "../multiplayer/clientgamelogic.h"
#include "../ObjectsCode/AI/AI_Player.H"
#include "../ObjectsCode/weapons/Weapon.h"
#include "../ObjectsCode/weapons/WeaponArmory.h"
#include "../GameLevel.h"

HUDRepair::HUDRepair()
{
	isActive_ = false;
	isInit = false;
	prevKeyboardCaptureMovie = NULL;
}

HUDRepair::~HUDRepair()
{
}

void HUDRepair::eventRepairAll(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==0);
	
	if(!needRepair)
		return;

	// lock UI
	isLockedUI = true;
	Scaleform::GFx::Value var[2];
	var[0].SetString("");
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	
	//COMPILE_ASSERT(R3D_ARRAYSIZE(repairSlots) > wiCharDataFull::CHAR_MAX_BACKPACK_SIZE);

	PKT_C2S_RepairItemReq_s n3;
	n3.SlotFrom   = 0;
	n3.RepairMode = 1;
	p2pSendToHost(gClientLogic().localPlayer_, &n3, sizeof(n3));
}

void HUDRepair::eventRepairItemInSlot(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	uint32_t slotID = args[0].GetUInt();

	// lock UI
	isLockedUI = true;
	Scaleform::GFx::Value var[2];
	var[0].SetString("");
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	
	PKT_C2S_RepairItemReq_s n3;
	n3.SlotFrom   = (BYTE)slotID;
	n3.RepairMode = 0;
	p2pSendToHost(gClientLogic().localPlayer_, &n3, sizeof(n3));
}


void HUDRepair::eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Deactivate();
}

void HUDRepair::setErrorMsg(const char* errMsg)
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

void HUDRepair::unlockUI()
{
	r3d_assert(isLockedUI);
	isLockedUI = false;
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	lockedOp = 0;
}

bool HUDRepair::Init()
{
	if(!gfxMovie.Load("Data\\Menu\\WarZ_HUD_Repair.swf", false)) 
		return false;

#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<HUDRepair>(this, &HUDRepair::FUNC)
	gfxMovie.RegisterEventHandler("eventRepairAll", MAKE_CALLBACK(eventRepairAll));
	gfxMovie.RegisterEventHandler("eventRepairItemInSlot", MAKE_CALLBACK(eventRepairItemInSlot));
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

bool HUDRepair::Unload()
{
	gfxMovie.Unload();
	isActive_ = false;
	isInit = false;
	return true;
}

void HUDRepair::Update()
{
}

void HUDRepair::Draw()
{
	gfxMovie.UpdateAndDraw();
}

void HUDRepair::Deactivate()
{
	if(isLockedUI)
		return;

	if(prevKeyboardCaptureMovie)
	{
		prevKeyboardCaptureMovie->SetKeyboardCapture();
		prevKeyboardCaptureMovie = NULL;
	}

	gfxMovie.Invoke("_root.api.hideRepairScreen", "");

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
void HUDRepair::Activate()
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

	reloadBackpackInfo();

	updateSurvivorTotalWeight();

	{
		Scaleform::GFx::Value var[1];
		var[0].SetString("menu_open");
		gfxMovie.OnCommandCallback("eventSoundPlay", var, 1);
	}

	gfxMovie.Invoke("_root.api.showRepairScreen", "");
}

void HUDRepair::updateSurvivorTotalWeight()
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

void HUDRepair::reloadBackpackInfo()
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

		memset(&repairSlots, 0, sizeof(repairSlots));
		needRepair = false;
		
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

				// add repair info
				//public function addBackpackItemRepairInfo(slotID:uint, itemCondition:uint, price:uint)
				int repairPriceGD = 0;
				{
					const GearConfig* gc = g_pWeaponArmory->getGearConfig(slot.Items[a].itemID);
					if(gc)
						repairPriceGD = gc->m_RepairPriceGD;
					const WeaponConfig* wc = g_pWeaponArmory->getWeaponConfig(slot.Items[a].itemID);
					if(wc)
						repairPriceGD = wc->m_RepairPriceGD;
					const WeaponAttachmentConfig* wac = g_pWeaponArmory->getAttachmentConfig(slot.Items[a].itemID);
					if(wac)
						repairPriceGD = wac->m_RepairPriceGD;
				}
				if(slot.Items[a].Var3!=-1 && slot.Items[a].Var3/100!=100 && repairPriceGD>0)
				{
					needRepair     = true;
					repairSlots[a] = 1;

					var[0].SetUInt(a);
					uint32_t curCondition = slot.Items[a].Var3/100;
					var[1].SetUInt(curCondition);
					var[2].SetUInt(uint32_t(float(repairPriceGD)*(float(100-curCondition)/100.0f)));
					gfxMovie.Invoke("_root.api.addBackpackItemRepairInfo", var, 3);
				}

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

	gfxMovie.Invoke("_root.api.Main.setGD", gUserProfile.ProfileData.GameDollars);
	gfxMovie.Invoke("_root.api.Main.showBackpack", "");
}
