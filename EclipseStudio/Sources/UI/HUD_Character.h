#pragma once

#include "UI\hud_base.h"
#include "GameObjects\GameObj.h"
#include "GameObjects\ObjManag.h"

typedef r3dgameVector(r3dSTLString) stringlist_t;


class CharacterHUD : public BaseHUD
{
public:

	r3dVector	FPS_Acceleration;
	r3dVector	FPS_vViewOrig;
	r3dVector	FPS_ViewAngle;
	r3dVector	FPS_vVision;
	r3dVector	FPS_vRight;
	r3dVector	FPS_vUp;
	r3dVector	FPS_vForw;
	r3dVector	cameraPosition;
	float currentDist;

	// for character rendering
	class obj_Player* m_Player;

public:
	CharacterHUD();
	~CharacterHUD() { }	

	virtual	void    SetCameraDir (r3dPoint3D vPos );
	virtual r3dPoint3D GetCameraDir () const;

	virtual void	Process();
	virtual void	Draw();
	void		 DrawAllAnims(float& Y);
	void		 DrawPlayerStates(float& Y);

	virtual void	OnProcess();
	void			ProcessPick( bool bSimple = false );

protected:
	virtual void	InitPure();
	virtual void	DestroyPure();	
	virtual void	SetCameraPure (r3dCamera &Cam );
	virtual	  void	OnHudUnselected ();
	
	void CreateCharacter();
	r3dPoint2D DrawCurrentAnimInfo(float BaseY);
	void DrawSkeleton(r3dSkeleton& skel);
	void StartDefaultAnim();

	bool paused;
	bool blendLooped;
	float curTime;
	float srcTime, dstTime;
};

