#pragma once

#include "PostFX.h"

class PFX_Combine : public PostFX
{
	// settings
public:
	struct Settings
	{
		Settings();

		r3dScreenBuffer*	mask;
		float				choseChannel[ 4 ];
	};

	typedef r3dTL::TArray< Settings > SettingsVec;


	// construction/ destruction
public:
	explicit PFX_Combine( int BlendMode, int ColorWriteMask );
	~PFX_Combine();	

	// manipulation/ access
public:
	void PushSettings( const Settings& sts );

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	virtual void PushDefaultSettingsImpl()				OVERRIDE;

	// data
private:
	int					mBlendMode;
	int					mColorWriteMask;

	SettingsVec			mSettings;
};