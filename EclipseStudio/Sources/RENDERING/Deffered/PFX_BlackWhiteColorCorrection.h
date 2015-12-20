#pragma once

#include "PostFX.h"

class PFX_BlackWhiteColorCorrection : public PostFX
{
	// construction/ destruction
public:
	PFX_BlackWhiteColorCorrection();
	~PFX_BlackWhiteColorCorrection();

	void SetPower ( float fPwr );

private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	float m_fPower;
	int mPSID;
};