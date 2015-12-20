#pragma once

#include "PostFX.h"

class PFX_MixIn : public PostFX
{
public:

	// Add more when you need it!
	// ( add settings array when you do. Like in PFX_Copy )
	enum ChannelSelection
	{
		SEL_RGBB,
		SEL_COUNT
	};

	struct Settings
	{
		ChannelSelection Sel ;

		int			BlendMode ;
		int			ColorWriteMask ;

		Settings();
	};

	// construction/ destruction
public:
	explicit PFX_MixIn();
	~PFX_MixIn();

	// manipulation /access
public:
	void SetSettings( const Settings& sts ) ;

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	Settings mSettings ;

	int PixelShaderIDs[ SEL_COUNT ] ;
};