#pragma once

#include "PostFX.h"

class PFX_StencilToMask : public PostFX
{
	// types
public:
	struct Settings
	{
		Settings();

		int		Ref ;
		float4	Value ;
		int		ColorWriteMask ;
	};

	typedef r3dTL::TArray< Settings > SettingsStack;

	// construction/ destruction
public:
	explicit PFX_StencilToMask();
	~PFX_StencilToMask();

	// manipulation/ access
public:
	void			PushSettings( const Settings& sts );

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	SettingsStack mSettings;
};