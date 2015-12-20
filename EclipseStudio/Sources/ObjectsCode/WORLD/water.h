
#ifndef __PWAR_OBJECT_WATER_H
#define __PWAR_OBJECT_WATER_H

#ifndef WO_SERVER
#include "XPSObject.h"

//////////////////////////////////////////////////////////////////////////

extern r3dScreenBuffer *gWaterRefractionBuffer;

//////////////////////////////////////////////////////////////////////////

struct R3D_WATER_VERTEX
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	r3dPoint3D	Position;


	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct R3D_WATER_PLANE_VERTEX
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	r3dPoint3D	Position;
	r3dVector	Normal;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};
#endif // WO_SERVER

class WaterBase
{
public:
#ifndef WO_SERVER
	r3dTexture*	WaterColor[25];
	r3dTexture*	FoamTexture;
	r3dTexture*	SpecularTexture;
	r3dTexture*	ColorTexture;
	r3dTexture*	MaskTexture;
	static IDirect3DVertexDeclaration9* pScreenDecl;

	r3dColor shallowColor;
	r3dColor deepColor;
	r3dColor attenColor;
	float shallowDepth;
	float attenDist;
	float refractionIndex;
	float bumpness;
	float tileSize;
	float specularCosinePower;
	float specularTiling;
	float causticStrength;
	float causticDepth;
	float causticTiling;
	int curQualitySettings;

	float waterColorTile;
	float waterColorBlend;
	float refractionPerturbation;
	float foamTile;
	float foamPerturbation;
	float foamSpeed;
	float foamIntensity;
	float surfTile;
	float surfPeriod;
	float surfWidth;
	float surfRange;
	float surfPerturbation;
	float surfIntensity;
	float surf2Coast;

	float uRefractionIndex;
	float uRefractionPerturbation;
	float uAttenDist;
	r3dColor uShallowColor;
	r3dColor uDeepColor;
	r3dColor uAttenColor;

	float PlaneOffsetX ;
	float PlaneOffsetZ ;

	float specBumpiness ;
	float specIntensity ;
	float reflectionIntensity ;
	float fresnelPow ;
	float fresnelBumpiness ;

	float farTileScale ;
	float farTileFadeStart ;
	float farTileFadeEnd ;
	float farTileAmmount ;
	float farTileBumpiness ;

	float lastWaterLevel ;
#endif // WO_SERVER

	static r3dTL::TArray< WaterBase* > AllWaters ;
#ifndef WO_SERVER
	static void UpdateAllRipples() ;
#endif // WO_SERVER

	WaterBase();
	~WaterBase();

#ifndef WO_SERVER
	void ReloadTextures() ;
	void UpdateRipples() ;
#endif // WO_SERVER

	void ReadSerializedData(pugi::xml_node& node);
#ifndef WO_SERVER
	void WriteSerializedData(pugi::xml_node& node);
	float SetNormalTextures(unsigned int t0, unsigned int t1, float fps) const;
	void CreateWaterBuffers();
	static void UpdateRefractionBuffer( bool allowZero );
	static void FlushRefractionBuffer();

	void RenderBegin(const r3dCamera& Cam, float waterLevel);
	void RenderEnd();

	enum {MaxSplashes=4};
	struct Splash
	{
		D3DXVECTOR4 params;
		int texIdx;
	};
	Splash splashes[MaxSplashes];
	int nSplashes;
	float lastSplashTime;
	bool Ripples(D3DXVECTOR4& camd);
	int curRipplesRT;
	r3dScreenBuffer* ripplesRT[2];
	r3dScreenBuffer* rainRipplesRT[2];
	r3dTexture* RipplesTex;

	r3dTexture* HeightTexture;
	bool isRippled ;
	D3DXVECTOR4 rippleCamd ;


	int showSurf, showFoam;
#ifndef FINAL_BUILD
	float DrawPropertyEditorWater(float scrx, float scry, float scrw, float scrh, float DEFAULT_CONTROLS_WIDTH, float DEFAULT_CONTROLS_HEIGHT);
#endif
	void OnCreateBase();
	void OnDestroyBase();
	void DeleteRippleBuffers();
#endif // WO_SERVER
};

#endif	// __PWAR_OBJECT_Reflect_H
