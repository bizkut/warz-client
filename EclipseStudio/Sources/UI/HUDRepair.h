#pragma once

#include "APIScaleformGfx.h"

class HUDRepair
{
	bool	isActive_;
	bool	isInit;

	bool	isLockedUI;
	int		lockedOp;
	
	int	repairSlots[512]; //wiCharDataFull::CHAR_MAX_BACKPACK_SIZE undefined here;
	bool	needRepair;

private:
	r3dScaleformMovie gfxMovie;
	r3dScaleformMovie* prevKeyboardCaptureMovie;
public:
	HUDRepair();
	~HUDRepair();

	bool 	Init();
	bool 	Unload();

	bool	IsInited() const { return isInit; }

	void 	Update();
	void 	Draw();

	bool	isActive() const { return isActive_; }
	void	Activate();
	void	Deactivate();

	bool	isUILocked() const { return isLockedUI; }
	void	setErrorMsg(const char* errMsg);
	void	unlockUI();

	void	reloadBackpackInfo();
	
	void	updateSurvivorTotalWeight();

	void	eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventRepairAll(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventRepairItemInSlot(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
};
