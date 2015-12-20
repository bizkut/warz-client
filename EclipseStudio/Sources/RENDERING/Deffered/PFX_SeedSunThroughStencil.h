#pragma once

#include "PostFX.h"

class PFX_SeedSunThroughStencil : public PostFX
{
	// types
public:
	struct Settings
	{
		Settings();

		float Weight;
		float Radius;
	};

	// construction/ destruction
public:
	explicit PFX_SeedSunThroughStencil();
	~PFX_SeedSunThroughStencil();

	// manipulation/ access
public:
	const Settings&	GetSettings() const;
	void			SetSettings( const Settings& sts );

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
};