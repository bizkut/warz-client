#pragma once

#include "r3d.h"

float windFunction(float time, float maxForce);

class r3dWind
{
public:
	struct Settings
	{
		Settings() ;

		float DirX ;
		float DirZ ;
		float WindPatternScale ;
		float WindPatternSpeed ;

		int ForceWind ;
		float ForceWindVal ;
	};

public:
	r3dWind();
	~r3dWind();

	const Settings& GetSettings() const ;
	void			SetSettings( const Settings& sts ) ;

	void Load(const char* levelDir);
	void Update( float t );
	void SetAzimuth(float a);		
	r3dPoint3D VectorValue( const r3dPoint3D& pos ) const;
	r3dPoint3D GetWindDir() const ;
	float ScalarValue( const r3dPoint3D& pos ) const;

	void SetMapSize( float width, float height ) ;

	r3dTexture* GetWindTexture() ;

	D3DXVECTOR4 GetTexcXfm() const ;

	UINT8 GetWindAt( const r3dPoint3D& pos ) ;

private:
	Settings	settings ;

	r3dPoint3D	offset ;

	float		time ;

	r3dTL::T2DArray< UINT8 >	windMap ;
	r3dTexture*					windTex ;

};

extern r3dWind* g_pWind;
