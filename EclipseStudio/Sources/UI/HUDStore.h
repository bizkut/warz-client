#pragma once

#include "APIScaleformGfx.h"

class HUDStore
{
	bool	isActive_;
	bool	isInit;

	bool	isLockedUI;

private:
	r3dScaleformMovie gfxMovie;
	r3dScaleformMovie* prevKeyboardCaptureMovie;
public:
	HUDStore();
	~HUDStore();

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

	void	updateMoney(int gc, int dollars);

	void	eventBuyItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
};
