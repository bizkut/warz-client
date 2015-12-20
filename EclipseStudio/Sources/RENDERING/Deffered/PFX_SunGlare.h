#pragma once

#include "PostFX.h"

enum
{
	MAX_SUNGLARES	= 10
};

class PFX_SunGlare : public PostFX
{
public:
	struct Settings
	{
		int NumSunglares;

		r3dColor Tint		[ MAX_SUNGLARES ];
		float	 Opacity	[ MAX_SUNGLARES ];
		float	 TexScale	[ MAX_SUNGLARES ];
		float	 BlurScale	[ MAX_SUNGLARES ];
		float	 Threshold	[ MAX_SUNGLARES ];

		Settings();
	};

	// construction/ destruction
public:
	PFX_SunGlare();
	~PFX_SunGlare();

	// manipulation/ access
public:
	const Settings&	GetSettings() const;
	void			SetSettings( const Settings& settings );

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	r3dTexture*		mShadeTexture;
	Settings		mSettings;

	int				mPSIDs[ MAX_SUNGLARES ];
};