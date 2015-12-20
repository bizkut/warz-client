#pragma once

#include "PostFX.h"

class PFX_DOFExtractNear : public PostFX
{
public:
	struct Settings
	{	
		Settings();

		float		NearEnd;
		float		NearStart;
	};

	// construction/ destruction
public:
	PFX_DOFExtractNear();
	~PFX_DOFExtractNear();

	// manipulation/ access
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

};