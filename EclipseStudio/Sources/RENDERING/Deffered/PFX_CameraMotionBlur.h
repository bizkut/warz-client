#pragma once

#include "PostFX.h"

class PFX_CameraMotionBlur : public PostFX
{
	// types
public:
	struct Settings
	{
		float MotionScale;
		float MotionThreshold;
	};

	// construction/ destruction
public:
	PFX_CameraMotionBlur();
	~PFX_CameraMotionBlur();

	// manipulation/ access
public:
	const Settings& GetSettings() const;
	void			SetSettings( const Settings& settings );

	void			Update();
	bool			IsCameraInMotion() const;

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;	
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	Settings	mSettings;

	D3DXMATRIX	mMotionMatrix;
	D3DXMATRIX	mLastViewProj;
};