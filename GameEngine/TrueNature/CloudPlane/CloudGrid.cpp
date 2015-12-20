#include "r3dPCH.h"
#include "r3d.h"
#include "CloudGrid.h"

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

CloudGrid::CloudGrid()
: m_pDecl(NULL)
{
	m_fCloudCover = 0.5f;
	m_vVelocity = D3DXVECTOR2( 0.01f, 0.01f );
	m_vOffset = D3DXVECTOR2( 0.0f, 0.0f );

	m_nVertices = 0;
	m_nTriangles = 0;

}

CloudGrid::~CloudGrid()
{
	Delete();
}

void CloudGrid::Delete()
{
	R3D_ENSURE_MAIN_THREAD();

	if( m_pIB.Get() )
	{
		r3dRenderer->Stats.AddBufferMem( -(int)sizeof(USHORT)*(m_nTriangles+2) );
	}

	if( m_pVB.Get() )
	{
		r3dRenderer->Stats.AddBufferMem( -(int)sizeof(S_VERTEX)*m_nVertices );
	}

	m_pVB.ReleaseAndReset();
	m_pIB.ReleaseAndReset();
}

void CloudGrid::Create()
{
	USHORT nCellNumX = 16;
	USHORT nCellNumZ = 16;

	// Create Buffers
	CreateVertexBuffer(nCellNumX, nCellNumZ);
	CreateIndexBuffer(nCellNumX, nCellNumZ );

	// Create vertex declaration
	static const D3DVERTEXELEMENT9 s_elements[] = {
		{ 0,  0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};

	r3dDeviceTunnel::CreateVertexDeclaration( s_elements, &m_pDecl );
}
	
void CloudGrid::CreateVertexBuffer(USHORT nCellNumX, USHORT nCellNumZ)
{
	m_vStartXZ = D3DXVECTOR2( -20000.0f, -20000.0f );
	m_vCellSizeXZ = D3DXVECTOR2( (80000.0f)/(float)nCellNumX, (80000.0f)/(float)nCellNumZ );

	m_nVertices = (nCellNumX + 1)*(nCellNumZ + 1);

	r3dDeviceTunnel::CreateVertexBuffer( sizeof(S_VERTEX)*m_nVertices, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB ) ;
	r3dDeviceTunnel::SetD3DResourcePrivateData( &m_pVB, "CloudPlane: VB");

	r3dRenderer->Stats.AddBufferMem ( sizeof(S_VERTEX)*m_nVertices );

	S_VERTEX* pVertices = NULL;
	m_pVB.Lock( 0, 0, (void**)&pVertices, 0 );

	// The vertex buffer includes only x and z index in the grid and they are scaled in the vertex shader.
	// The height y is computed in the vertex shader using horizontal distance from view point.
	float fScaleX = 1.0f/(float)(nCellNumX+1);
	float fScaleZ = 1.0f/(float)(nCellNumZ+1);
	for (int z = 0; z < nCellNumZ+1; ++z) {
		for (int x = 0; x < nCellNumX+1; ++x) {
			pVertices->x = (float)x;     // x index
			pVertices->z = (float)z;     // z index
			pVertices->u = x * fScaleX;  // texture coordinate u
			pVertices->v = z * fScaleZ;  // texture coordinate v
			++pVertices;
		}
	}
	m_pVB.Unlock();

	// Initialize x and z components of the bounding box 
	// MaxY is changed at every frame according to the eye height.
	m_bound.vMin = D3DXVECTOR3( m_vStartXZ.x, 0.0f, m_vStartXZ.y );
	D3DXVECTOR2 vEndXZ( m_vCellSizeXZ.x * nCellNumX, m_vCellSizeXZ.y * nCellNumZ );
	D3DXVec2Add( &vEndXZ, &vEndXZ, &m_vStartXZ );
	m_bound.vMax = D3DXVECTOR3( vEndXZ.x, 0.0f, vEndXZ.y  );
}

void CloudGrid::CreateIndexBuffer(USHORT nCellNumX, USHORT nCellNumZ)
{
	unsigned int nNumIndex = (nCellNumX+2) * 2 * nCellNumZ - 2;
	
	r3dDeviceTunnel::CreateIndexBuffer( sizeof(USHORT)*nNumIndex, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB);
	r3dRenderer->Stats.AddBufferMem ( sizeof(USHORT)*nNumIndex );

	USHORT* pIndex = NULL;
	m_pIB.Lock( 0, 0, (void**)&pIndex, 0 );

	USHORT nVertexNumX = (USHORT)(nCellNumX+1);
	for ( SHORT x = (SHORT)nCellNumX; 0 <= x; --x ) {
		*pIndex++ = x;
		*pIndex++ = nVertexNumX + x;
	}
	for ( USHORT z = 1; z < (SHORT)nCellNumZ; ++z ) {
		*pIndex++ = z*nVertexNumX;
		*pIndex++ = z*nVertexNumX + nCellNumX;
		for ( SHORT x = nCellNumX; 0 <= x; --x ) {
			*pIndex++ = z*nVertexNumX + x;
			*pIndex++ = (z+1)*nVertexNumX + x;
		}
	}


	m_pIB.Unlock();
	m_nTriangles = nNumIndex-2;
}

// Update cloud position.
// The cloud is animated by scrolling uv 
void CloudGrid::Update(float dt, float farPlane, const D3DXVECTOR3& eyePt, float windScale)
{
	// increment uv scrolling parameters 
	D3DXVECTOR2 vec;
	D3DXVec2Scale( &vec, &m_vVelocity, dt * windScale ); 
	D3DXVec2Add( &m_vOffset, &m_vOffset, &vec );

	// Adjust the height so that clouds are always above.
	// cloud height = m_fDefaultHeight + m_fFallOffHeight * squaredistance_in_horizontal
	float fRange = 0.5f * farPlane;
	float fHeight = fRange * 0.12f;
	m_fDefaultHeight = fHeight + eyePt.y;
	m_fFallOffHeight  = - ( 0.1f / fRange ) * (  eyePt.y / fHeight + 1.0f );

	// Update Bounding Box
	m_bound.vMax.y = m_fDefaultHeight;

}

void CloudGrid::Draw(r3dTexture* cloudTex)
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	r3dRenderer->SetTex(cloudTex, 0);
 	pDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
 	pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	d3dc._SetDecl( m_pDecl );
	d3dc._SetStreamSource( 0, m_pVB.Get(), 0, sizeof(S_VERTEX) );
	d3dc._SetIndices( m_pIB.Get() );

	r3dRenderer->DrawIndexed(D3DPT_TRIANGLESTRIP, 0, 0, m_nVertices, 0, m_nTriangles);
}

// cloud cover = [0.0 1.0]
void CloudGrid::SetCloudCover(float fCloudCover)
{
	m_fCloudCover = fCloudCover;
}

float CloudGrid::GetCurrentCloudCover() const
{
	return m_fCloudCover;
}

const SBoundingBox& CloudGrid::GetBoundingBox() const
{
	return m_bound;
}

void CloudGrid::GetVSParam(SVSParam& param, const SSceneParamter& sceneParams) const
{
	param.vUVParam = D3DXVECTOR4( sceneParams.m_fUVScale, sceneParams.m_fUVScale, m_vOffset.x, m_vOffset.y );
	param.vXZParam = D3DXVECTOR4( m_vCellSizeXZ.x, m_vCellSizeXZ.y, m_vStartXZ.x, m_vStartXZ.y );
	param.vHeight  = D3DXVECTOR2( m_fFallOffHeight, m_fDefaultHeight );
}


