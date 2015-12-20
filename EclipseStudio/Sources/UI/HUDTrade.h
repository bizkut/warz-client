#pragma once

#include "APIScaleformGfx.h"
#include "GameCode/UserProfile.h"

class HUDTrade
{
	bool	isActive_;
	bool	isInit;
	bool	isCloseRequested;
	bool	isUserIndicatorOn;
	bool	isOppositeIndicatorOn;
	float	enableUserAcceptAt; // timer when to allow player to press Accept btn

private:
	r3dScaleformMovie gfxMovie;
	r3dScaleformMovie* prevKeyboardCaptureMovie;
public:
	enum { MAX_TRADE_SIZE = 16, };
	struct tradeSlot_s
	{
	  int		  SlotFrom;
	  wiInventoryItem Item;
	};
	tradeSlot_s	TradeContent[MAX_TRADE_SIZE];
	wiInventoryItem	OppositeTradeContent[MAX_TRADE_SIZE]; // what other player offers
	wiCharDataFull  playerBackpackForTrade; // local copy

public:
	HUDTrade();
	~HUDTrade();

	bool 	Init();
	bool 	Unload();

	bool	IsInited() const { return isInit; }

	void 	Update();
	void 	Draw();

	bool	isActive() const { return isActive_; }
	void	Activate(const char* plrNameToTradeWith);
	void	Deactivate();
	
	void	setOpposeTradeIndicator(bool set);
	void	setOffer(int slotTo, const wiInventoryItem& item);
	void	showNoSpace();
	void	showNoWeight();
	void	requestClose();

	void	reloadBackpackInfo();
	void	reloadTradeInfo();

	void	updateSurvivorTotalWeight();

	void	eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventBackpackToTrade(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventTradeToBackpack(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventBackpackGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventTradeGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventTradeAccept(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventTradeDecline(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
};
