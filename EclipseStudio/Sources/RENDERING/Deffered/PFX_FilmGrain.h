#pragma once

#include "PostFX.h"

class PFX_FilmGrain : public PostFX
{
	// type
public:
	struct Settings
	{
		float GrainScale;
		float Strength;
		float FPS;
	};

	// construction/ destruction
public:
	PFX_FilmGrain();
	~PFX_FilmGrain();

	// manipulation/ access
public:
	const Settings& GetSettings() const;
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
	r3dTexture* mNoiseTexture;
	Settings	mSettings;
	float		mLastDSP0_X;
	float		mLastDSP0_Y;
	float		mLastDSP1_X;
	float		mLastDSP1_Y;
	float		mLastNoiseDSP;
	clock_t		mLastTick;

};