#pragma once

#include "GameCommon.h"

/* disabled for now. need temp player to draw it, easy, but class isn't used now
class UIWeaponModel : public GameObject
{
	DECLARE_CLASS(UIWeaponModel, GameObject)
public:
	class Weapon* m_CurrentWeapon;
	bool enableRendering;
	float	rotationTimer;

	void	setWeaponModel(uint32_t itemID, GameObject* owner);

	UIWeaponModel();
	virtual ~UIWeaponModel();

	virtual	BOOL		OnCreate();
	virtual	BOOL		Load(const char *name);

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam );

	virtual BOOL		Update();
};*/