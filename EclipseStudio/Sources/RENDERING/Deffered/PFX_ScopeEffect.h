//=========================================================================
//	Module: PFX_ScopeEffect.h
//	Copyright (C) 2011.
//=========================================================================
#pragma once

#include "PostFX.h"

//////////////////////////////////////////////////////////////////////////

struct ScopeMaterialParams
{
	ScopeMaterialParams();

	float Metalness ;
	float Chromeness ;
	float SpecularPower ;

	int Lighting ;
};

class PFX_ScopeEffect: public PostFX
{

public:
	PFX_ScopeEffect();
	~PFX_ScopeEffect();

public:
	void SetScopeReticle(r3dTexture *tex);
	void SetScopeNormal(r3dTexture *tex);
	void SetScopeMask(r3dTexture *tex);
	void SetScopeBlurMask(r3dTexture *tex);
	void SetBlurredSceneImage(r3dScreenBuffer *bsi);

	void SetLightMatrix( const D3DXMATRIX& mtx );

	void	SetScopeScale( float scale ) ;
	float	GetScopeScale() const ;
	void	SetScopeOffset( const r3dPoint2D &uvOffset );

	void				SetMaterialParams( const ScopeMaterialParams& params ) ;
	ScopeMaterialParams	GetMaterialParams() const ;

private:
	virtual void InitImpl();
	virtual	void CloseImpl();
	virtual void PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src);
	virtual void FinishImpl();

private:
	int m_ps_NoShadows_ID;
	int m_ps_NormalShadows_ID;
	int m_ps_HWShadows_ID;
	int m_ps_FilterNoLighting;

	int m_vsID;
	r3dTexture *m_scopeReticuleTexture;
	r3dTexture *m_scopeNormalTexture;
	r3dTexture *m_scopeMaskTexture;
	r3dTexture *m_scopeBlurTexture;
	r3dScreenBuffer *m_blurredSceneImage;

	D3DXMATRIX m_lightMtx ;

	ScopeMaterialParams m_materialParams ;

	float				m_scopeScale ;
	r3dPoint2D			m_scopeOffset;
};
