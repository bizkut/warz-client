//=========================================================================
//	Module: PFX_LensDirt.h
//	Copyright (C) 2011.
//=========================================================================
#pragma once

#include "PostFX.h"


class PFX_LensDirt: public PostFX
{
public:
	struct Settings
	{
		Settings();

		float Area;
		float GlowStrength;
		float LightStrength;
		float Pow;

		float Opacity;
	};

	struct Light
	{
		r3dPoint3D Pos;
		r3dPoint3D ProjectedPos;
		r3dColor Color;
		float Strength;
		float Radius;

		int IsDir;
	};

	typedef r3dTL::TArray< Light > LightArr;

public:
	PFX_LensDirt();
	~PFX_LensDirt();

public:
	const Settings& GetSettings() const;
	void SetSettings( const Settings& sts );

	void SetVPMatrix( const D3DXMATRIX& mtx );

	void PurgeLights();

	void AddLight( r3dPoint3D pos, r3dColor colr, float strength, float radius, int isDir );

	void ProjectLights();
	int IsActive() const;

	int IsFull() const;

	void SetSunOn( int on );

private:
	virtual void InitImpl();
	virtual	void CloseImpl();
	virtual void PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src);
	virtual void FinishImpl();

private:
	r3dTexture *m_lengthDirtTexture;
	Settings	m_Settings;

	D3DXMATRIX	m_VPMtx;
	LightArr	m_Lights;

	int m_SunOn;
	float m_SunIntensity;

	float m_LastTime;
};
