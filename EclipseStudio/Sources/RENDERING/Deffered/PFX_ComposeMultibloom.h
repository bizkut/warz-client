#pragma once

#include "PostFX.h"

class PFX_ComposeMultibloom : public PostFX
{
public:
	enum
	{
		MAX_IMAGES = 4
	};

	struct Settings
	{
		Settings();

		r3dTL::TFixedArray<PostFXChief::RTType, MAX_IMAGES> bloomParts;
	};

	typedef r3dTL::TArray< Settings > SettingsArr ;

	// construction/ destruction
public:
	explicit PFX_ComposeMultibloom(int numImages);
	~PFX_ComposeMultibloom();

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
	SettingsArr	mSettingsArr;
	int mColorWriteMask;
	int mNumImages;
};