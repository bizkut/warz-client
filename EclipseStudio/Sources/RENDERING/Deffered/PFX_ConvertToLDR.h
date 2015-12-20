#pragma once

#include "PostFX.h"

class PFX_ConvertToLDR : public PostFX
{
public:

	// construction/ destruction
public:
	explicit PFX_ConvertToLDR();
	~PFX_ConvertToLDR();

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:

};
