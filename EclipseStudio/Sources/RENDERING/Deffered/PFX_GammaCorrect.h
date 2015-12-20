#pragma once

#include "PostFX.h"

class PFX_GammaCorrect : public PostFX
{
	// construction/ destruction
public:
	PFX_GammaCorrect();
	~PFX_GammaCorrect();

	void SetPower ( float Pow );

private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	float mPow;

};