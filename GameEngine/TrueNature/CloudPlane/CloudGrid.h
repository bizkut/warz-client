#pragma once

#include "SceneParameter.h"
#include "Shader.h"


//Grid for rendering cloud density and shadow.
class CloudGrid 
{
public :
	struct SVSParam 
	{
		D3DXVECTOR4 vUVParam;
		D3DXVECTOR4 vXZParam;
		D3DXVECTOR2 vHeight;
	};

	struct S_VERTEX
	{
		float x; // x index
		float z; // z index
		float u; // texture coordinate u 
		float v; // texture coordinate v 
	};

	CloudGrid();
	~CloudGrid();

	void Create();
	void Delete();

	void Update(float dt, float farPlane, const D3DXVECTOR3& eyePt, float windScale);
	void Draw(r3dTexture* cloudTex);

	void SetCloudCover(float fCloudCover);
	float GetCurrentCloudCover() const;
	const SBoundingBox& GetBoundingBox() const;
	void GetVSParam(SVSParam& param, const SSceneParamter& sceneParams) const;

protected :
	void CreateVertexBuffer(USHORT nCellNumX, USHORT nCellNumZ);
	void CreateIndexBuffer(USHORT nWidth, USHORT nHeight);

	r3dD3DVertexBufferTunnel      m_pVB;       // vertex buffer of the grid
	r3dD3DIndexBufferTunnel       m_pIB;       // index buffer of the grid
	LPDIRECT3DVERTEXDECLARATION9 m_pDecl;     // vertex declaration

	unsigned int m_nVertices;            // the number of vertices
	unsigned int m_nTriangles;           // the number of triangles

	D3DXVECTOR2 m_vStartXZ;      // minimum x,z position 
	D3DXVECTOR2 m_vCellSizeXZ;   // cell width in x and z axis.
	float m_fDefaultHeight;      // cloud height above the view position
	float m_fFallOffHeight;      // delta height.

	float m_fCloudCover;         // cloud cover
	D3DXVECTOR2 m_vVelocity;     // wind velocity
	D3DXVECTOR2 m_vOffset;       // current uv offset
							    
	SBoundingBox m_bound;        // bounding box of the grid
};

