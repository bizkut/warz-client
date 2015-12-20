#ifndef	__R3D_CMAP_H
#define	__R3D_CMAP_H

class r3dCubeMap : public r3dIResource
{
public:
	ID3DXRenderToEnvMap*	m_pRenderToEnvMap;
	IDirect3DCubeTexture9*	m_pCubeMap;
	int			size_;

protected:
	// you can't directly create it - use CreateClass()
	r3dCubeMap(int size, const r3dIntegrityGuardian& ig = r3dIntegrityGuardian() );

public:
	virtual void		D3DCreateResource();
	virtual void		D3DReleaseResource();
	virtual	~r3dCubeMap();

	static	r3dCubeMap*	CreateClass(int size) { 
		return gfx_new r3dCubeMap(size);
	}

	int 		Load(const char* FName);

	typedef void (*fn_DrawWorld)();
	int		Render(const r3dPoint3D& Pos, r3dCamera &Cam1, fn_DrawWorld draw); 

	void		GetCubeMapViewMatrix(D3DXMATRIX *m, D3DCUBEMAP_FACES face);

	void		GetCubeMapViewDir(D3DXVECTOR3 *v, D3DCUBEMAP_FACES face);

	int		Save(const char* fname);
};

void RenderCubeFaceMipChain( r3dScreenBuffer* cube, int face, r3dScreenBuffer* temp0, r3dScreenBuffer* temp1 );
void RenderCubeMapMipChainWithEdgeSmoothing( r3dScreenBuffer* cube, r3dScreenBuffer* temp0, r3dScreenBuffer* (&faces)[ 6 ] ) ;

#endif	// __R3D_CMAP_H
