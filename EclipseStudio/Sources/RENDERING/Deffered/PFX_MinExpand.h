#pragma once

#include "PostFX.h"

class PFX_MinExpand : public PostFX
{
	// construction/ destruction
public:
	explicit PFX_MinExpand();
	~PFX_MinExpand();

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