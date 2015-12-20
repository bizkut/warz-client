#include "r3dPCH.h"
#include "r3d.h"

#include "../../../Eternity/sf/Console/config.h"
#include "HUDStore.h"
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

HUDStore::HUDStore()
{
	isActive_ = false;
	isInit = false;
	prevKeyboardCaptureMovie = NULL;
}

HUDStore::~HUDStore()
{
}

void HUDStore::eventBuyItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	// itemID:uint, price:int, priceGD:int
	r3d_assert(argCount == 3);
	uint32_t itemID = args[0].GetUInt();
	int priceGC = args[1].GetInt();
	int priceGD = args[2].GetInt();

	if(gUserProfile.ProfileData.GameDollars < priceGD || gUserProfile.ProfileData.GamePoints < priceGC)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("NotEnougMoneyToBuyItem"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	if(gUserProfile.haveFreeInventorySlot(false) == false)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("InGameUI_ErrorMsg_NoInventorySpace"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	int buyIdx = getStoreBuyIdxUI(priceGD, priceGC);
	if(buyIdx == 0)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("BuyItemFailNoIndex"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	PKT_C2S_ShopBuyReq_s n;
	n.ItemID = itemID;
	n.BuyIdx = buyIdx;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	
	isLockedUI = true;
}

void HUDStore::eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Deactivate();
}

void HUDStore::setErrorMsg(const char* errMsg)
{
	if(isLockedUI)
	{
		isLockedUI = false;
		gfxMovie.Invoke("_root.api.hideInfoMsg", "");
		Scaleform::GFx::Value var[3];
		var[0].SetString(errMsg);
		var[1].SetBoolean(true);
		var[2].SetString("ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
}

void HUDStore::unlockUI()
{
	r3d_assert(isLockedUI);
	isLockedUI = false;
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	gfxMovie.Invoke("_root.api.buyItemSuccessful", "");
}

bool HUDStore::Init()
{
 	if(!gfxMovie.Load("Data\\Menu\\WarZ_HUD_GeneralStore.swf", false)) 
 		return false;
 
#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<HUDStore>(this, &HUDStore::FUNC)
 	gfxMovie.RegisterEventHandler("eventBuyItem", MAKE_CALLBACK(eventBuyItem));
	gfxMovie.RegisterEventHandler("eventReturnToGame", MAKE_CALLBACK(eventReturnToGame));

	gfxMovie.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );

	addItemCategoriesToUI(gfxMovie);
	addItemsAndCategoryToUI(gfxMovie);
	addStoreToUI(gfxMovie);

	isActive_ = false;
	isInit = true;
	isLockedUI = false;
	return true;
}

bool HUDStore::Unload()
{
 	gfxMovie.Unload();
	isActive_ = false;
	isInit = false;
	return true;
}

void HUDStore::Update()
{
}

void HUDStore::Draw()
{
 	gfxMovie.UpdateAndDraw();
}

void HUDStore::Deactivate()
{
	if(isLockedUI)
		return;

	if(prevKeyboardCaptureMovie)
	{
		prevKeyboardCaptureMovie->SetKeyboardCapture();
		prevKeyboardCaptureMovie = NULL;
	}

	gfxMovie.Invoke("_root.api.hideMarketplace", "");

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

void HUDStore::Activate()
{
#ifdef DISABLE_STORE_ACCESS_FOR_DEV_EVENT_SERVER
	if(gClientLogic().m_gameInfo.gameServerId==148353 || gClientLogic().m_gameInfo.gameServerId==150340 || gClientLogic().m_gameInfo.gameServerId==150341|| gClientLogic().m_gameInfo.gameServerId==151732 || gClientLogic().m_gameInfo.gameServerId==151733 || gClientLogic().m_gameInfo.gameServerId==151734 || gClientLogic().m_gameInfo.gameServerId==151736)
		return;
#endif

	r3d_assert(!isActive_);
	r3dMouse::Show();
	isActive_ = true;

	prevKeyboardCaptureMovie = gfxMovie.SetKeyboardCapture(); // for mouse scroll events

	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);

	// reset barricade placement if going into UI
	plr->m_needToDrawBarricadePlacement = 0; 
	plr->m_BarricadeMeshPlacement = NULL;


	{
		Scaleform::GFx::Value var[1];
		var[0].SetString("menu_open");
		gfxMovie.OnCommandCallback("eventSoundPlay", var, 1);
	}

	updateMoney(gUserProfile.ProfileData.GamePoints, gUserProfile.ProfileData.GameDollars);

	gfxMovie.Invoke("_root.api.showMarketplace", "");
}

void HUDStore::updateMoney(int gc, int dollars)
{
	gfxMovie.Invoke("_root.api.setDollars", dollars);
	gfxMovie.Invoke("_root.api.setGC", gc);
	gfxMovie.Invoke("_root.api.setCells", 0);
}