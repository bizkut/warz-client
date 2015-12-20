#pragma once

#include "PostFX.h"

class PFX_CalcLuma : public PostFX
{
public:

	// construction/ destruction
public:
	explicit PFX_CalcLuma();
	~PFX_CalcLuma();

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	r3dTexture* mDebugChecker ;

};
