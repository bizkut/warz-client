#include "r3dPCH.h"
#include "r3d.h"

#include "../../../Eternity/sf/Console/config.h"
#include "HUDTrade.h"
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

HUDTrade::HUDTrade()
{
	isActive_ = false;
	isInit = false;
	isCloseRequested = false;
	prevKeyboardCaptureMovie = NULL;
}

HUDTrade::~HUDTrade()
{
}

void HUDTrade::eventBackpackGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	int gridFrom = args[0].GetInt();
	int gridTo = args[1].GetInt();
	// not implemented for now - way too complicate to validate on server side
}

void HUDTrade::eventTradeGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	int gridFrom = args[0].GetInt();
	int gridTo = args[1].GetInt();
	// not implemented for now - way too complicate to validate on server side
}

void HUDTrade::eventBackpackToTrade(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	// gridFrom:int, TradeGridTo:int, amount:int
	r3d_assert(argCount==3);
	int backpackGridFrom = args[0].GetInt();
	int tradeGridTo = args[1].GetInt();
	int quantity = args[2].GetInt();
	
	if(isCloseRequested)
		return;

	wiInventoryItem& wi1 = playerBackpackForTrade.Items[backpackGridFrom];
	if(wi1.itemID == 0 || quantity > wi1.quantity)
		return;
	
	// search where to put this item. no stacking. put only into free slot or into same trade slot with saved backpack slot
	int slot_free = -1;
	int slot_used = -1;
	for(int i=0; i<MAX_TRADE_SIZE; i++)
	{
		if(slot_free == -1 && TradeContent[i].Item.itemID == 0)
			slot_free = i;
		if(TradeContent[i].SlotFrom == backpackGridFrom)
			slot_used = i;
	}
	if(slot_used == -1 && slot_free == -1)
		return;
	
	// fill trade item
	wiInventoryItem wi2;
	if(slot_used != -1)
	{
		r3d_assert(TradeContent[slot_used].SlotFrom == backpackGridFrom);
		TradeContent[slot_used].Item.quantity += quantity;
	}
	else
	{
		r3d_assert(TradeContent[slot_free].SlotFrom == -1);
		TradeContent[slot_free].SlotFrom      = backpackGridFrom;
		TradeContent[slot_free].Item          = wi1;
		TradeContent[slot_free].Item.quantity = quantity;
	}

	// send it to server
	PKT_C2C_TradeItem_s n;
	n.BckSlot   = (BYTE)(backpackGridFrom);
	n.TradeSlot = (BYTE)(slot_used != -1 ? slot_used : slot_free);
	n.Item      = TradeContent[slot_used != -1 ? slot_used : slot_free].Item;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	
	// adjust backpack copy
	wi1.quantity -= quantity;
	if(wi1.quantity == 0)
		wi1.Reset();
	
	reloadTradeInfo();
	reloadBackpackInfo();
	updateSurvivorTotalWeight();
	gfxMovie.Invoke("_root.api.updateBackpackAndTradeWindows", "");

	// any modification reset both accept indicators
	gfxMovie.Invoke("_root.api.Main.setUserTradeIndicator", false);
	gfxMovie.Invoke("_root.api.Main.setOppositeTradeIndicator", false);
	isUserIndicatorOn = false;
	isOppositeIndicatorOn = false;
}

void HUDTrade::eventTradeToBackpack(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	// TradeGridFrom:int, gridTo:int, amount:int
	r3d_assert(argCount==3);
	int tradeGridFrom = args[0].GetInt();
	int backpackGridTo = args[1].GetInt();
	int quantity = args[2].GetInt();
	
	if(isCloseRequested)
		return;
	
	if(TradeContent[tradeGridFrom].SlotFrom == -1 || TradeContent[tradeGridFrom].Item.itemID == 0)
		return;
		
	// override backpack slot with saved slot from trade
	backpackGridTo = TradeContent[tradeGridFrom].SlotFrom;
	if(backpackGridTo == -1)
		return;
		
	wiInventoryItem& wi1 = TradeContent[tradeGridFrom].Item;
	wiInventoryItem& wi2 = playerBackpackForTrade.Items[backpackGridTo];
	if(quantity > wi1.quantity)
		return;

	// adjust backpack	
	if(wi2.itemID == 0) {
		wi2          = wi1;
		wi2.quantity = quantity;
	} else {
		wi2.quantity += quantity;
	}
	
	// adjust trade
	wi1.quantity -= quantity;
	if(wi1.quantity == 0) {
		wi1.Reset();
		TradeContent[tradeGridFrom].SlotFrom = -1;
	}

	// send it to server
	PKT_C2C_TradeItem_s n;
	n.BckSlot   = (BYTE)(backpackGridTo);
	n.TradeSlot = (BYTE)(tradeGridFrom);
	n.Item      = wi1;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));

	reloadTradeInfo();
	reloadBackpackInfo();
	updateSurvivorTotalWeight();
	gfxMovie.Invoke("_root.api.updateBackpackAndTradeWindows", "");

	// any modification reset both accept indicators
	gfxMovie.Invoke("_root.api.Main.setUserTradeIndicator", false);
	gfxMovie.Invoke("_root.api.Main.setOppositeTradeIndicator", false);
	isUserIndicatorOn = false;
	isOppositeIndicatorOn = false;
}

void HUDTrade::eventTradeAccept(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==0);

	if(isCloseRequested)
		return;

	gfxMovie.Invoke("_root.api.Main.setUserTradeIndicator", true);
	isUserIndicatorOn = true;

	PKT_C2C_TradeRequest_s n;
	n.code     = PKT_C2C_TradeRequest_s::TRADE_Confirm;
	n.targetId = 0;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

void HUDTrade::eventTradeDecline(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	eventReturnToGame(pMovie, args, argCount);
}

void HUDTrade::eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	requestClose();
	//DO NOT deactivate - wait for server to respond.
	//Deactivate();
}

bool HUDTrade::Init()
{
	if(!gfxMovie.Load("Data\\Menu\\WarZ_HUD_Trade.swf", false)) 
		return false;

#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<HUDTrade>(this, &HUDTrade::FUNC)
	gfxMovie.RegisterEventHandler("eventReturnToGame", MAKE_CALLBACK(eventReturnToGame));
	gfxMovie.RegisterEventHandler("eventBackpackToTrade", MAKE_CALLBACK(eventBackpackToTrade));
	gfxMovie.RegisterEventHandler("eventTradeToBackpack", MAKE_CALLBACK(eventTradeToBackpack));
	gfxMovie.RegisterEventHandler("eventBackpackGridSwap", MAKE_CALLBACK(eventBackpackGridSwap));
	gfxMovie.RegisterEventHandler("eventTradeGridSwap", MAKE_CALLBACK(eventTradeGridSwap));
	gfxMovie.RegisterEventHandler("eventTradeAccept", MAKE_CALLBACK(eventTradeAccept));
	gfxMovie.RegisterEventHandler("eventTradeDecline", MAKE_CALLBACK(eventTradeDecline));

	gfxMovie.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );

	// add items
	{
		addItemCategoriesToUI(gfxMovie);
		addItemsAndCategoryToUI(gfxMovie);
	}

	isActive_ = false;
	isInit = true;
	return true;
}

bool HUDTrade::Unload()
{
	gfxMovie.Unload();
	isActive_ = false;
	isInit = false;
	return true;
}

void HUDTrade::Update()
{
	if(r3dGetTime() >= enableUserAcceptAt && enableUserAcceptAt>0)
	{
		enableUserAcceptAt = -1;
		gfxMovie.Invoke("_root.api.Main.enableUserTradeIndicator", 1);
	}
}

void HUDTrade::Draw()
{
	gfxMovie.UpdateAndDraw();
}

void HUDTrade::Deactivate()
{
	if(!isCloseRequested && (!isOppositeIndicatorOn || !isUserIndicatorOn))
	{
		hudMain->addChatMessage(0, "<trade>", (gLangMngr.getString("TradeWasCancelled")), 0);
	}

	if(prevKeyboardCaptureMovie)
	{
		prevKeyboardCaptureMovie->SetKeyboardCapture();
		prevKeyboardCaptureMovie = NULL;
	}

	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	gfxMovie.Invoke("_root.api.hideScreen", "");

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
void HUDTrade::Activate(const char* plrNameToTradeWith)
{
	r3d_assert(!isActive_);
	r3dMouse::Show();
	isActive_ = true;
	isCloseRequested = false;
	isUserIndicatorOn = false;
	isOppositeIndicatorOn = false;
	enableUserAcceptAt = -1;
	gfxMovie.Invoke("_root.api.Main.enableUserTradeIndicator", 1);

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

		playerBackpackForTrade = slot;
	}

	// reset trade info
	{
		for(int i=0; i<MAX_TRADE_SIZE; ++i)
		{
			TradeContent[i].Item     = wiInventoryItem();
			TradeContent[i].SlotFrom = -1;

			OppositeTradeContent[i] = wiInventoryItem();
		}
	}

	reloadTradeInfo();
	reloadBackpackInfo();
	updateSurvivorTotalWeight();

	// set opposite player name
	{
		gfxMovie.Invoke("_root.api.Main.setOppositePlayerName", plrNameToTradeWith);
	}


	{
		Scaleform::GFx::Value var[1];
		var[0].SetString("menu_open");
		gfxMovie.OnCommandCallback("eventSoundPlay", var, 1);
	}

	gfxMovie.Invoke("_root.api.Main.setUserTradeIndicator", false);
	gfxMovie.Invoke("_root.api.Main.setOppositeTradeIndicator", false);
	isUserIndicatorOn = false;
	isOppositeIndicatorOn = false;

	gfxMovie.Invoke("_root.api.showScreen", "");
}

void HUDTrade::reloadTradeInfo()
{
	Scaleform::GFx::Value var[8];
	// clear Trade DB
	gfxMovie.Invoke("_root.api.Main.clearTradeInfo", NULL, 0);

	enableUserAcceptAt = r3dGetTime()+5.0f; // 5 second delay before player can accept trade to prevent scam
	gfxMovie.Invoke("_root.api.Main.enableUserTradeIndicator", 0);

	for(int i=0; i<MAX_TRADE_SIZE; ++i)
	{
		const wiInventoryItem& itm = TradeContent[i].Item;
		if(itm.itemID == 0)
			continue;

		var[0].SetInt(i);
		var[1].SetUInt(0);
		var[2].SetUInt(itm.itemID);
		var[3].SetNumber(itm.quantity);
		var[4].SetNumber(itm.Var1);
		var[5].SetNumber(itm.Var2);
		bool isConsumable = false;
		{
			const WeaponConfig* wc = g_pWeaponArmory->getWeaponConfig(itm.itemID);
			if(wc && wc->category == storecat_UsableItem && wc->m_isConsumable)
				isConsumable = true;
		}
		var[6].SetBoolean(isConsumable);
		var[7].SetString(getAdditionalDescForItem(itm.itemID, itm.Var1, itm.Var2, itm.Var3));
		gfxMovie.Invoke("_root.api.Main.addItemToUserTrade", var, 8);
	}

	for(int i=0; i<MAX_TRADE_SIZE; ++i)
	{
		const wiInventoryItem& itm = OppositeTradeContent[i];
		if(itm.itemID == 0)
			continue;

		var[0].SetInt(i);
		var[1].SetUInt(0);
		var[2].SetUInt(itm.itemID);
		var[3].SetNumber(itm.quantity);
		var[4].SetNumber(itm.Var1);
		var[5].SetNumber(itm.Var2);
		bool isConsumable = false;
		{
			const WeaponConfig* wc = g_pWeaponArmory->getWeaponConfig(itm.itemID);
			if(wc && wc->category == storecat_UsableItem && wc->m_isConsumable)
				isConsumable = true;
		}
		var[6].SetBoolean(isConsumable);
		var[7].SetString(getAdditionalDescForItem(itm.itemID, itm.Var1, itm.Var2, itm.Var3));
		gfxMovie.Invoke("_root.api.Main.addItemToOppositeTrade", var, 8);
	}
}

void HUDTrade::updateSurvivorTotalWeight()
{
	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);

	float totalWeight = playerBackpackForTrade.getTotalWeight();
	// add their trade offers
	for(int i=0; i<MAX_TRADE_SIZE; ++i)
	{
		const wiInventoryItem& itm = OppositeTradeContent[i];
		if(itm.itemID == 0)
			continue;
			
		const BaseItemConfig* bic = g_pWeaponArmory->getConfig(itm.itemID);
		if(bic)
			totalWeight += bic->m_Weight * itm.quantity;
	}
		
	if(playerBackpackForTrade.Skills[CUserSkills::SKILL_Physical3])
		totalWeight *= 0.95f;
	if(playerBackpackForTrade.Skills[CUserSkills::SKILL_Physical7])
		totalWeight *= 0.9f;

	Scaleform::GFx::Value var[2];
	char tmpGamertag[128];
	if(plr->ClanID != 0)
		sprintf(tmpGamertag, "[%s] %s", plr->ClanTag, playerBackpackForTrade.Gamertag);
	else
		r3dscpy(tmpGamertag, playerBackpackForTrade.Gamertag);

	var[0].SetString(tmpGamertag);
	var[1].SetNumber(totalWeight);
	gfxMovie.Invoke("_root.api.updateClientSurvivorWeight", var, 2);
}

void HUDTrade::setOpposeTradeIndicator(bool set)
{
	gfxMovie.Invoke("_root.api.Main.setOppositeTradeIndicator", set);
	isOppositeIndicatorOn = set;
}

void HUDTrade::setOffer(int slotTo, const wiInventoryItem& item)
{
	OppositeTradeContent[slotTo] = item;
	reloadTradeInfo();
	updateSurvivorTotalWeight();
	gfxMovie.Invoke("_root.api.updateBackpackAndTradeWindows", "");
	
	// any movement clear both accept indicators
	gfxMovie.Invoke("_root.api.Main.setUserTradeIndicator", false);
	gfxMovie.Invoke("_root.api.Main.setOppositeTradeIndicator", false);
	isUserIndicatorOn = false;
	isOppositeIndicatorOn = false;
}

void HUDTrade::showNoSpace()
{
	Scaleform::GFx::Value var[3];
	var[0].SetString(gLangMngr.getString("TradeOtherNoSpace"));
	var[1].SetBoolean(true);
	var[2].SetString("");
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);

	gfxMovie.Invoke("_root.api.Main.setUserTradeIndicator", false);
	isUserIndicatorOn = false;
}

void HUDTrade::showNoWeight()
{
	Scaleform::GFx::Value var[3];
	var[0].SetString(gLangMngr.getString("TradeOtherNoWeight"));
	var[1].SetBoolean(true);
	var[2].SetString("");
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);

	gfxMovie.Invoke("_root.api.Main.setUserTradeIndicator", false);
	isUserIndicatorOn = false;
}

void HUDTrade::requestClose()
{
	if(isCloseRequested)
		return;
	isCloseRequested = true;
	
	PKT_C2C_TradeRequest_s n;
	n.code     = PKT_C2C_TradeRequest_s::TRADE_Close;
	n.targetId = 0;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

void HUDTrade::reloadBackpackInfo()
{
	updateSurvivorTotalWeight();
	// reset backpack
	{
		gfxMovie.Invoke("_root.api.clearBackpack", "");
	}

	// add backpack content info
	{
		Scaleform::GFx::Value var[7];
		for (int a = 0; a < playerBackpackForTrade.BackpackSize; a++)
		{
			if (playerBackpackForTrade.Items[a].itemID != 0)
			{
				var[0].SetInt(a);
				var[1].SetUInt(0); // not used for game
				var[2].SetUInt(playerBackpackForTrade.Items[a].itemID);
				var[3].SetInt(playerBackpackForTrade.Items[a].quantity);
				var[4].SetInt(playerBackpackForTrade.Items[a].Var1);
				var[5].SetInt(playerBackpackForTrade.Items[a].Var2);
				var[6].SetString(getAdditionalDescForItem(playerBackpackForTrade.Items[a].itemID, playerBackpackForTrade.Items[a].Var1, playerBackpackForTrade.Items[a].Var2, playerBackpackForTrade.Items[a].Var3));
				gfxMovie.Invoke("_root.api.addBackpackItem", var, 7);
			}
		}
	}

	gfxMovie.Invoke("_root.api.Main.showBackpack", "");
}
