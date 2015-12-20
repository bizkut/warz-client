#pragma once

#include "PostFX.h"

class PFX_Transform : public PostFX
{
	// construction/ destruction
public:
	explicit PFX_Transform();
	~PFX_Transform();

public:
	void SetSettings( D3DXVECTOR4 a, D3DXVECTOR4 b );

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	D3DXVECTOR4 mTCoefs[ 2 ] ;

};