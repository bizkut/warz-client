#pragma once

#include "PostFX.h"

class PFX_Copy : public PostFX
{
public:
	struct Settings
	{
		Settings();

		float TexScaleX ;
		float TexScaleY ;

		float TexOffsetX ;
		float TexOffsetY ;

		bool SkyOnly ;
		bool ForceFiltering ;
	};

	typedef r3dTL::TArray< Settings > SettingsArr ;

	// construction/ destruction
public:
	explicit PFX_Copy( int BlendMode, int ColorWriteMask );
	~PFX_Copy();

	// manipulation /access
public:
	void PushSettings( const Settings& settings ) ;

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
	int			mBlendMode;
	int			mColorWriteMask;
	SettingsArr	mSettingsArr; 

	int			mNeedRestoreFiltering ;
};