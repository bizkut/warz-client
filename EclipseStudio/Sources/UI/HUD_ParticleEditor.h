#pragma once

#include "hud_base.h"

typedef r3dgameVector(r3dSTLString) stringlist_t;


class ParticleHUD : public BaseHUD
{
public:

	r3dVector	FPS_Acceleration;
	r3dVector	FPS_vViewOrig;
	r3dVector	FPS_ViewAngle;
	r3dVector	FPS_vVision;
	r3dVector	FPS_vRight;
	r3dVector	FPS_vUp;
	r3dVector	FPS_vForw;
	

public:
	ParticleHUD() { }
	~ParticleHUD() { }	

	virtual	void    SetCameraDir (r3dPoint3D vPos );
	virtual r3dPoint3D GetCameraDir () const;
	
	virtual void	Process();
	virtual void	Draw();

	virtual void	ProcessParticleEditor();
	virtual void	ParticleEditorStart();
	virtual void	OnProcess();
	void			ProcessPick( bool bSimple = false );

protected:
	virtual void	InitPure();
	virtual void	DestroyPure();	
	virtual void	SetCameraPure (r3dCamera &Cam );
	virtual	  void	OnHudUnselected ();
};

