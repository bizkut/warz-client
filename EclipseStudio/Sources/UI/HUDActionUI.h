#pragma once

#include "APIScaleformGfx.h"

class HUDActionUI
{
	bool	isActive_;
	bool	isInit;

private:
	r3dScaleformMovie gfxMovie;
public:
	HUDActionUI();
	~HUDActionUI();

	bool 	Init();
	bool 	Unload();

	bool	IsInited() const { return isInit; }

	void 	Update();
	void 	Draw();
	void	setScreenPos(int x, int y);

	bool	isActive() const { return isActive_; }
	void	Activate();
	void	Deactivate();

	void	setText(const char* title, const char* msg, const char* letter);
	void	enableRegularBlock();
	void	enableProgressBlock();
	void	setProgress(int value); // [0,100]

	// durability: [0-5], speed,armor,weight:[0,100], gas: [0,100]
	void	setCarInfo(int durability, int speed, int armor, int weight, int gas);
	void	showCarInfo(bool visible);
};
