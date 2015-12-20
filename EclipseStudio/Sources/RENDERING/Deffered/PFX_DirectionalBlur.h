#pragma once

#include "PostFX.h"

class PFX_DirectionalBlur : public PostFX
{
public:

	struct Settings
	{
		Settings();

		float AlphaMul;
	};

	typedef r3dTL::TArray< Settings > SettingsVec;

	static const int COEFS_PS_CONST_START	=	4;
	static const int NUM_PS_CONSTS			=	9;

	// construction/ destruction
public:
	PFX_DirectionalBlur( float xDir, float yDir, int tapCount );

public:
	void PushSettings( const Settings& settings );
	int		TapCount	() const;

	// polymorphism
protected:
	virtual	void CloseImpl()	OVERRIDE;

	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()				OVERRIDE;
	virtual void PushDefaultSettingsImpl()	OVERRIDE;

	virtual void DoPrepare( r3dScreenBuffer* src, float stepAmplify );

	float		mConstants[ NUM_PS_CONSTS ][ 4 ];

	float		mDirX;
	float		mDirY;

	int			mTapCount;

	SettingsVec	mSettings;
};

//////////////////////////////////////////////////////////////////////////

class PFX_DirectionalBlurOptimized: public PFX_DirectionalBlur
{
public:
	PFX_DirectionalBlurOptimized(float xDir, float yDir, int tapCount);
	~PFX_DirectionalBlurOptimized() {}

protected:
	virtual void InitImpl();
	virtual void DoPrepare(r3dScreenBuffer* src, float stepAmplify);
	virtual void FinishImpl();
};

//////////////////////////////////////////////////////////////////////////


class PFX_DirectionalDepthBlur : public PFX_DirectionalBlur
{
public:
	struct Settings
	{
		Settings();

		float Start;
		float End;

		bool				UseStencil;
		bool				Reverse;
		r3dScreenBuffer*	ReverseMask;
	};

	typedef r3dTL::TArray< Settings > SettingsVec;


	PFX_DirectionalDepthBlur( float xDir, float yDir, int tapCount );
	~PFX_DirectionalDepthBlur();

public:
	void		PushSettings		( const Settings& settings );

private:
	virtual void InitImpl()		OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl() OVERRIDE;

	virtual void PushDefaultSettingsImpl()	OVERRIDE;

private:
	SettingsVec mSettings;

	int mPSForwardID;
	int mPSReverseID;
};

//////////////////////////////////////////////////////////////////////////

class PFX_DirectionalBloomWithGlowBlur: public PFX_DirectionalBlurOptimized
{
public:
	PFX_DirectionalBloomWithGlowBlur(float xDir, float yDir);

private:
	virtual void InitImpl();
};