#ifndef __R3D_HUD_SYSTEM_H
#define __R3D_HUD_SYSTEM_H

#define VIEW_FOV 60

class CameraChannel;
class CameraStatic;

typedef void (*fn_VisualFX)(float Lerp);

struct PlayerStateVars_s
{
	r3dPoint3D	Position;
	r3dPoint3D	ScopePosition;
	float		FOV;
	float		ScopeFOV;
	float		FPS_FOV;
	float		MouseSensetivity;
	float		ScopeMouseSensitivity;
	float		LookUpLimit;
	float		LookDownLimit;
	bool		allowScope; // if this state allows using scope.Sprinting and jumping for example doesn't
	fn_VisualFX	FXFunc;

	void Lerp(class obj_Player* pl, PlayerStateVars_s& s1, PlayerStateVars_s& s2, float lerpV);
	//Need params for 
	// camera shaker effect
	// visual effect ( like radial blur or something else... )

	// Maybe add animation - but probably not....
};

class BaseHUD
{
public:
	int			bInited;
	r3dPoint3D	FPS_Position;

public:
	
					BaseHUD();
					virtual ~BaseHUD();

	void			Init ();
	void			Destroy ();
	void			SetCamera ( r3dCamera &Cam );
	void			HudSelected ();
	void			HudUnselected ();

	virtual	  void  SetCameraDir (r3dPoint3D vPos ) = 0;
	virtual r3dPoint3D GetCameraDir () const = 0;

	virtual   void	Process() = 0;
	virtual   void	ProcessPick() {};
	

	virtual   void	Draw() = 0;

	virtual r3dPoint3D	GetCamOffset() const;
	virtual void		SetCamPos( const r3dPoint3D& pos );

protected:

	virtual   void	InitPure() = 0;
	virtual   void	DestroyPure() = 0;	
	virtual   void	SetCameraPure (r3dCamera &Cam ) = 0;

	virtual	  void	OnHudSelected () {};
	virtual	  void	OnHudUnselected () {};
};


extern BaseHUD	*Hud;

void RegisterHUDCommands();

#endif //__R3D_HUD_SYSTEM_H
