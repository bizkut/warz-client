#pragma once

#include "PostFX.h"

class PFX_RadialBlur : public PostFX
{
	// types
public:
	struct Settings
	{
		float BlurStart;
		float BlurStrength;

		static const float	MIN_BLURSTART,
							MAX_BLURSTART;

		static const float	MIN_BLURSTRENGTH,
							MAX_BLURSTRENGTH;

		void Restrict();

		Settings()
		: BlurStart(0.33f)
		, BlurStrength(0.033f)
		{}
	};

	// construction/ destruction
public:
	PFX_RadialBlur();
	~PFX_RadialBlur();

public:
	void			PushSettings( const Settings& settings );
	void			SetDefaultSettings( const Settings& settings );
	const Settings&	GetDefaultSettings() const;

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	r3dTexture* mModulationTex;
	r3dTL::TArray<Settings> mSettingsArr;
	Settings mDefaultSettings;

};