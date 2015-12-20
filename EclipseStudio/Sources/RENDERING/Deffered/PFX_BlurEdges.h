#pragma once

#include "PostFX.h"

class PFX_BlurEdges : public PostFX
{
public:
	static const int MAX_TAP_COUNT				=	8;
	static const int COEFS_PS_CONST_START		=	4;
	static const int DEPTH_THRESHOLD_PS_CONST	=	6;
	static const int NUM_PS_CONSTS				=	7;

	struct Params
	{
		Params();

		float threshold;
		float maxd;
		float amplify;
	};

	// construction/ destruction
public:
	PFX_BlurEdges( float xDir, float yDir, int tapCount );
	~PFX_BlurEdges();

	// manipulation/ access
public:
	void			SetParams( const Params& parms );
	Params			GetParams() const;

	// polymorphism
protected:
	virtual void InitImpl()		OVERRIDE;
	virtual	void CloseImpl()	OVERRIDE;

	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()	OVERRIDE;

	int		TapCount	();

	// data
private:

	float	mConstants[ NUM_PS_CONSTS ][ 4 ];

	float	mDirX;
	float	mDirY;

	int		mTapCount;
};
