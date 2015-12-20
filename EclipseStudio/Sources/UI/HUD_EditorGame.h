#pragma once

#include "hud_base.h"

class obj_Player;

class EditorGameHUD : public BaseHUD
{
public:
	static gobjid_t	editorPlayerId;

public:
	EditorGameHUD() 
	{ 
		editorPlayerId = invalidGameObjectID;
		cameraRayLen = 20000.0f;
	}
	
	~EditorGameHUD() { }
	

	virtual	void    SetCameraDir (r3dPoint3D vPos );
	virtual r3dPoint3D GetCameraDir () const;
	
	virtual void	Process();

	virtual void	Draw();
	virtual r3dPoint3D	GetCamOffset() const;
	virtual void		SetCamPos( const r3dPoint3D& pos );

protected:
	virtual	void	OnHudSelected();
	virtual void	OnHudUnselected();
	virtual void	InitPure();
	virtual void	DestroyPure();
	virtual void	SetCameraPure (r3dCamera &Cam );
public:
	static obj_Player*	AddPlayer	( int bControllable );

	float cameraRayLen;


};
