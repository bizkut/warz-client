#ifndef	__PWAR_SKYDOME_H
#define	__PWAR_SKYDOME_H

#include "hoffmanscatter.h"
#include "CloudPlane/CloudPlane.h"

struct R3D_SKY_VERTEX
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	r3dPoint3D	Position;
	float		tu,tv;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};


struct StaticSkySettings
{
	StaticSkySettings();

	int				bEnabled;
	int				bPlanarMapping;

	r3dTexture*		tex0;
	r3dTexture*		tex1;

	r3dTexture*		glowTex0;
	r3dTexture*		glowTex1;

	float			texLerpT;

	r3dMesh*		mesh;

	float 			texScaleX;
	float 			texScaleY;
	float 			texOffsetX;
	float 			texOffsetY;

	float			SunCtrlX;
	float			SunCtrlY;

	r3dColor		SunSpotColor;
	float			SunSpotIntensity;
	float			dayT;

	int				bIsNight;

	float			SunIntensity;

	float			DomeRotationY;
};

class r3dSkyDome : public r3dIResource
{
public:
	int			StatidSkyVS_ID ;
	int			StatidSkyTexgVS_ID ;

	StaticSkySettings SSTSettings ;

	int		bLoaded;
	int		LastTime;
	float	m_fFogHeight;
	float	m_fFogOffset;
	float	m_fWindFactor;
	r3dColor m_volumeFogColor;

public:
	r3dSkyDome( const r3dIntegrityGuardian& ig );
	virtual ~r3dSkyDome();

	void	SetParams(float fFogHeight, float fFogOffset, r3dColor volumeFogColor, float fWindFactor);
	void	SetStaticSkyParams( const StaticSkySettings& sts );

	int		Load(const char* fname);
	int		Unload();

	void	Update(const r3dCamera &Cam);
	void	Draw(const r3dCamera &Cam, bool normals, bool hemisphere );
	void	DrawDome( const r3dCamera &Cam, const D3DXMATRIX& viewProj, float mieScale, bool normals, bool hemisphere, bool hdr );

	virtual	void		D3DCreateResource();
	virtual	void		D3DReleaseResource();

	r3dScreenBuffer* cubemap;

	r3dScreenBuffer* faceRTs[ 6 ];

	r3dScreenBuffer* tempRt0;

private:
	float	m_fCloadAnim;
	r3dTexture* cloudTex; 
	r3dTexture* cloudCoverTex;

	void DrawCubemap(const r3dCamera& Cam);
	// public:
	// 	r3dSkyDome();
	// 	void	SetParams(float fFogHeight, float fFogOffset, r3dColor volumeFogColor, float fWindFactor);
	// 
	// 	int		Load(const char* fname);
	// 	int		Unload();
	// 
	// 	void	Draw(const r3dCamera &Cam);
	// 
	// private:
	// 	
};

#endif	// __PWAR_SKYDOME_H

