#pragma once

#include "PostFX.h"

class PFX_CopyOutput : public PostFX
{
public:

	// construction/ destruction
public:
	explicit PFX_CopyOutput();
	~PFX_CopyOutput();

public:
	void SetFiltering( bool forceFiltering ) ;
	
	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	bool mForceFiltering ;

};
