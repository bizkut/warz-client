#pragma once

#include "PostFX.h"

class PFX_ExposureBlend : public PostFX
{
public:

	// construction/ destruction
public:
	explicit PFX_ExposureBlend();
	~PFX_ExposureBlend();

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	float mPrevTime ;

};
