#ifndef __R3D_HUD_SYSTEM_H_1
#define __R3D_HUD_SYSTEM_H_1

#include "hud_base.h"

enum HUD_CameraMode {
	hud_TopdownCamera = 0,
	hud_FPSCamera,
	hud_TPSCamera,
};


class CameraHUD : public BaseHUD
{
public:
	HUD_CameraMode	CameraMode;

	r3dVector	FPS_Acceleration;
	r3dVector	FPS_vViewOrig;
	r3dVector	FPS_ViewAngle;
	r3dVector	FPS_vVision;
	r3dVector	FPS_vRight;
	r3dVector	FPS_vUp;
	r3dVector	FPS_vForw;
	
	r3dVector	TPS_Target;
	float		TPS_DistanceToTarget;
	float		TPS_SideOffset;
	float		TPS_LockedDistance;

	const char *customTextToShow;

public:
	CameraHUD();
	~CameraHUD();

	virtual	void	SetCameraDir (r3dPoint3D vPos );
	virtual r3dPoint3D GetCameraDir () const;

	void			SetFPSCamera(r3dCamera &Cam );

	virtual void	Process();

  	void			ProcessPick();
  	void			ProcessCheats();
  	void			ProcessFPSCamera();
	void			ProcessTPSCamera();

	virtual void	Draw();

	virtual r3dPoint3D GetCamOffset() const OVERRIDE;

protected:

	virtual void	InitPure();
	virtual void	DestroyPure();	
	virtual void	SetCameraPure (r3dCamera &Cam );
};


#endif //__R3D_HUD_SYSTEM_H_1
