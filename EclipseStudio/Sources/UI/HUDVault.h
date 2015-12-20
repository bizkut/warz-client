#pragma once

#include "APIScaleformGfx.h"

class HUDVault
{
	bool	isActive_;
	bool	isInit;

	bool	isLockedUI;
	int		lockedOp;

private:
	r3dScaleformMovie gfxMovie;
	r3dScaleformMovie* prevKeyboardCaptureMovie;
public:
	HUDVault();
	~HUDVault();

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
	void	updateInventory();

	void	updateSurvivorTotalWeight();

	void	eventBackpackFromInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventBackpackToInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventBackpackGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
};
