#pragma once

#include "PostFX.h"

class PFX_StereoReproject : public PostFX
{
	// types
public:
	struct Settings
	{
		D3DXMATRIX ViewMtx ;
		D3DXMATRIX ProjMtx ;

		float ViewDelta ;

		bool IsRight ;
	};

	typedef r3dTL::TArray< Settings > SettingsVec ;

	// construction/ destruction
public:
	PFX_StereoReproject();
	~PFX_StereoReproject();

public:
	void PushSettings( const Settings& sts );

private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	SettingsVec mSettingsVec ;

	int			mLeftPSID ;
	int			mRightPSID ;
};

//------------------------------------------------------------------------

void CalcStereoViewProj( float strength, D3DXMATRIX* oLeftView, D3DXMATRIX* oLeftProj, D3DXMATRIX* oRightView, D3DXMATRIX* oRightProj, float* oLeftCamOffset, float* oRightCamOffset );