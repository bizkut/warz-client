#pragma once

#include "PostFX.h"

class PFX_GodRays : public PostFX
{
	// types
public:
	struct Settings
	{
		float Density;
		float Decay;
		float Exposure;
	};

	// construction/ destruction
public:
	PFX_GodRays();
	~PFX_GodRays();

	// manipulation /access
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
	Settings mSettings;
	DWORD prevAddressU;
	DWORD prevAddressV;

};