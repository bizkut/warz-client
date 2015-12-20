#ifndef __R3DSUN_H
#define __R3DSUN_H


#include "TimeGradient.h"

class r3dSun : r3dIResource
{
 public: 

	r3dVector		SunDir;
	int				bLoaded;
	
	r3dLight		SunLight;
	
	float			DawnTime;
	float			DuskTime;
	float			Time;			

	float			SunDirAngle;
	float			SunElevAngle;

	float			AngleRange;

	r3dColor		SunColor;

	IDirect3DQuery9*				OcclusionQuery;
	IDirect3DVertexDeclaration9*	OcclusionVDECL;
	int								OcclusionQueryRequired;
	int								QueryMade;
	int								IsVisible;

	r3dSun();
	~r3dSun();


	void	Init();
	void	Unload();

	virtual	void D3DCreateResource() OVERRIDE;
	virtual	void D3DReleaseResource() OVERRIDE;

	void	SetOcclusionQueryRequired( int isReq );

	void	Update();
	
	void		SetLocation(float Angle1, float Angle2);
	r3dPoint3D	GetSunVecAtNormalizedTime( float CurTime );
	r3dPoint3D	GetCurrentSunVec();
	float		TimeToValD( float Time );
	void		SetParams(float Hour, float NewDawnTime, float NewDuskTime, float NewSunDirAngle, float NewSunElevAngle, r3dColor NewSunColor, float NewAngleRange );
	float		GetAngleAtNormalizedTime( float CurTime );

	void	DrawSun(const r3dCamera &Cam, int bReplicate = 1);
};

#endif // R3DSUN
