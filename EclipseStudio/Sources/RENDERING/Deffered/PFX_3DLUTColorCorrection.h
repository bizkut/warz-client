#pragma once

#include "PostFX.h"

class PFX_3DLUTColorCorrection : public PostFX
{
	// construction/ destruction
public:
	PFX_3DLUTColorCorrection();
	~PFX_3DLUTColorCorrection();

	// for color correction when hit (local player only)
	static r3dTexture* s_damageHitColorCorrectionTex;
	static float s_damageHitColorCorrectionVal;

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	int m3DLUTPSIDs[ 3 ];

};