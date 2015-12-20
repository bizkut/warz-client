#pragma once

#include "PostFX.h"

class PFX_AnaglyphComposite : public PostFX
{
	// construction/ destruction
public:
	PFX_AnaglyphComposite();
	~PFX_AnaglyphComposite();

public:
	void SetRightBufferID( int BufferID );

private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	int mBufferID ;

};

