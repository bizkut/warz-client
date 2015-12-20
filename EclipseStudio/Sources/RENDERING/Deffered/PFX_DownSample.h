#pragma once

#include "PostFX.h"

class PFX_DownSample : public PostFX
{
public:
	struct Settings
	{
		int ColorWriteMask ;

		float TexMultiplyU ;
		float TexMultiplyV ;

		float TexAddU ;
		float TexAddV ;

		Settings() ;
	};

	typedef r3dTL::TArray< Settings > SettingsVec ;

	// construction/ destruction
public:
	PFX_DownSample();
	~PFX_DownSample();

public:
	void PushSettings( const Settings& sts ) ;

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
	SettingsVec mSettingsVec ;

};

