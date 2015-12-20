#pragma once

#include "PostFX.h"

class PFX_FilmTone : public PostFX
{
	// construction/ destruction
public:
	PFX_FilmTone();
	~PFX_FilmTone();

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

