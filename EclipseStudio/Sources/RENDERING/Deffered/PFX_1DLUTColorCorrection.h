#pragma once

#include "PostFX.h"

class PFX_1DLUTColorCorrection : public PostFX
{
	// construction/ destruction
public:
	explicit PFX_1DLUTColorCorrection( bool useHSV );
	~PFX_1DLUTColorCorrection();

	// polymorphism
private:
	virtual void InitImpl()							OVERRIDE;
	virtual	void CloseImpl()						OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src
												)	OVERRIDE;
	virtual void FinishImpl()						OVERRIDE;

	// data
private:
	bool		mUseHSV;

	r3dTexture*	mLUTTexture;

};