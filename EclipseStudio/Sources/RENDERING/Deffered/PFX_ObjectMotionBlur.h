#pragma once

#include "PostFX.h"

class PFX_ObjectMotionBlur : public PostFX
{
	// types
public:
	struct Settings
	{
		float BlurStrength;
	};

	// construction/ destruction
public:
	PFX_ObjectMotionBlur();
	~PFX_ObjectMotionBlur();

public:
	const Settings&	GetSettings() const;
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

};