#pragma once

#include "APIScaleformGfx.h"

class HUDAttachments
{
	bool	isActive_;
	bool	isInit;

private:
	r3dScaleformMovie gfxMovie;

public:
	HUDAttachments();
	~HUDAttachments();

	bool 	Init();
	bool 	Unload();

	bool	IsInited() const { return isInit; }

	void 	Update();
	void 	Draw();

	bool	isActive() const { return isActive_; }
	void	Activate();
	void	Deactivate();

	void	eventSelectAttachment(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
};
