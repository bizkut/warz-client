#ifndef __R3D_HUD_SYSTEM_H_2
#define __R3D_HUD_SYSTEM_H_2

#include "hud_base.h"

class TPSGameHUD : public BaseHUD
{
public:
	TPSGameHUD();
	~TPSGameHUD();

	virtual	void    SetCameraDir (r3dPoint3D vPos );
	virtual r3dPoint3D GetCameraDir () const;
	
	virtual void	Process();

	virtual void	Draw();

protected:

	virtual void	InitPure();
	virtual void	DestroyPure();	
	virtual void	SetCameraPure (r3dCamera &Cam );

private:
	r3dVector	FPS_Acceleration;
	r3dVector	FPS_vViewOrig;
	r3dVector	FPS_ViewAngle;
	r3dVector	FPS_vVision;
	r3dVector	FPS_vRight;
	r3dVector	FPS_vUp;
	r3dVector	FPS_vForw;
	float		cameraRayLen;
};


#endif //__R3D_HUD_SYSTEM_H_2
