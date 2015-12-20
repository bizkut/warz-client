#pragma once

#include "hud_base.h"

class PhysicsHUD : public BaseHUD
{
	class GameObject*	PhysicsObject;
public:
	r3dVector	FPS_Acceleration;
	r3dVector	FPS_vViewOrig;
	r3dVector	FPS_ViewAngle;
	r3dVector	FPS_vVision;
	r3dVector	FPS_vRight;
	r3dVector	FPS_vUp;
	r3dVector	FPS_vForw;
public:
	PhysicsHUD() {PhysicsObject=0; }
	~PhysicsHUD() { }

	virtual	void    SetCameraDir (r3dPoint3D vPos );
	virtual r3dPoint3D GetCameraDir () const;
	
	virtual void	Process();

	void			ProcessPhysicsEditor();
	void			ProcessPick();

	virtual void	Draw();

protected:

	virtual void	InitPure();
	virtual void	DestroyPure();		
	virtual void	SetCameraPure (r3dCamera &Cam );


};


