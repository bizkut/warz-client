#pragma once

#include "../SF/RenderBuffer.h"

template <class vert_s>
class r3dXPSObject
{ 
public:
	char		Name[64];
	HRESULT		hr;

	r3dVertexBuffer	*pVB;


	int		VertexSize;
	int		VertexCount;

	r3dIndexBuffer	*pIB;
	int		IndexSize;
	int		IndexCount;

	int		RenderType;

	LPDIRECT3DVERTEXDECLARATION9		pDecl;

#define POLYBUFFER_HW		(1<<1)
#define POLYBUFFER_SW		(1<<2)
#define POLYBUFFER_Dynamic	(1<<3)
#define POLYBUFFER_AutoFlush	(1<<4)
	int		Flags;

	int		LockMode;	// 0 - NoLocked, 1 - Lock(), 2 - LockAll(),
	vert_s		*pVerts;
	DWORD		*pIdxs;

public:
	r3dXPSObject();
	~r3dXPSObject();
	int		Init(int NumVerts, int NumIdxs, int _Flags, char *_DebugName = NULL);

	int		InitGrid(int StartX, int StartZ, int Width, int Height, float CellSize, float Y=0);
	// util functions for connecting grids of different resolutions to prevent gaps
	// assumtions: next grid is twice smaller than current, cell size is twice bigger
	int		InitGridSideConnectorUP(int StartX, int StartZ, int Width, int Height, float CellSize, float Y=0); // UP = +Y
	int		InitGridSideConnectorDOWN(int StartX, int StartZ, int Width, int Height, float CellSize, float Y=0); // DOWN = -Y
	int		InitGridSideConnectorLEFT(int StartX, int StartZ, int Width, int Height, float CellSize, float Y=0); // LEFT = -X
	int		InitGridSideConnectorRIGHT(int StartX, int StartZ, int Width, int Height, float CellSize, float Y=0); // RIGHT = +X

	int		InitDome( const float Radius, const float Height, const int Rings, const int Segments);

	int		Destroy();

	int		IsValid();

	//  LockAll() will lock whole buffer and you can use Add*() functions
	//  Lock() will lock part of buffer 
	//    if FirstIndex params is NULL, you can use Add() functions
	//    otherwise you supposed to fill data yourself
	int		LockAll();
	int		Lock(int NumVerts, int NumIdxs, vert_s **pV, DWORD **pI, int *FirstIndex, bool bReadOnly = false);
	void		Unlock();

	int		AddVertices(vert_s *V, int NumVerts);
	int		AddTriangle(vert_s *V);

	void		Draw();
};



inline void dl_assert(int var)
{
	MessageBox(NULL, "Oops", "Asser", MB_OK);
	_exit(0);
}

template <class vert_s>
r3dXPSObject<vert_s>::r3dXPSObject()
{
	Flags = 0;
	LockMode = 0;
	Name[0] = 0;
	VertexSize = 0;
	VertexCount = 0;
	IndexSize = 0;
	IndexCount = 0;
	
	pDecl = NULL;
	pVB = NULL;
	pIB = NULL;
	sprintf(Name, "[%p(Not Inited)]", this);
	RenderType = 0;
}

template <class vert_s>
r3dXPSObject<vert_s>::~r3dXPSObject()
{
	Destroy();
}

template <class vert_s>
int r3dXPSObject<vert_s>::Init(int NumVertices, int NumIndices, int _Flags, char *_DebugName)
{
	DWORD	dwFlags;

	pDecl     = vert_s::getDecl();
	Flags       = _Flags;

	VertexSize  = NumVertices;
	VertexCount = 0;

	IndexSize   = NumIndices;
	IndexCount  = 0;

	LockMode    = 0;

	dwFlags     = D3DUSAGE_WRITEONLY; 

	if(Flags & POLYBUFFER_Dynamic)
		dwFlags |= D3DUSAGE_DYNAMIC;

	/*hr = r3dRenderer->pd3ddev->CreateVertexBuffer(
		NumVertices * sizeof(vert_s),
		dwFlags,
		0,
		D3DPOOL_DEFAULT,
		&pVB, NULL);

	hr = r3dRenderer->pd3ddev->CreateIndexBuffer(
		NumIndices * sizeof(DWORD),
		dwFlags, 
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&pIB, NULL);
*/

	pVB = gfx_new r3dVertexBuffer( NumVertices, sizeof(vert_s), 0, (dwFlags & D3DUSAGE_DYNAMIC)?true:false );
	pIB = gfx_new r3dIndexBuffer( NumIndices, (dwFlags & D3DUSAGE_DYNAMIC)?true:false, sizeof(DWORD) );


	if(_DebugName)
		sprintf(Name, "%s", _DebugName);
	else 
		sprintf(Name, "[%p(v:%d i:%d)]", this, NumVertices, NumIndices);


	return 1;
}

template <class vert_s>
int r3dXPSObject<vert_s>::InitGrid(int StartX, int StartZ,int Width, int Height, float CellSize, float Y )
{
	DWORD	dwFlags;

	pDecl     = vert_s::getDecl();
	Flags       = 0;
	VertexSize  = (Width+1)*(Height+1);
	VertexCount = 0;

	IndexSize   = Width*Height*2*3;
	IndexCount  = 0;

	LockMode    = 0;

	dwFlags     = D3DUSAGE_WRITEONLY; 

	if(Flags & POLYBUFFER_Dynamic)
		dwFlags |= D3DUSAGE_DYNAMIC;

/*	hr = r3dRenderer->pd3ddev->CreateVertexBuffer(
		VertexSize * sizeof(vert_s),
		dwFlags,
		0,
		D3DPOOL_DEFAULT,
		&pVB, NULL);

	hr = r3dRenderer->pd3ddev->CreateIndexBuffer(
		IndexSize * sizeof(DWORD),
		dwFlags, 
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&pIB, NULL);

		*/

	pVB = gfx_new r3dVertexBuffer( VertexSize, sizeof(vert_s), 0, (dwFlags & D3DUSAGE_DYNAMIC)?true:false );
	pIB = gfx_new r3dIndexBuffer( IndexSize, (dwFlags & D3DUSAGE_DYNAMIC)?true:false, sizeof(DWORD) );


	sprintf(Name, "XPS: GRID [%p(v:%d i:%d)]", this, VertexSize, IndexSize);

	LockAll();

	vert_s VV;

	// add vertices
	for(int i=0; i<Height+1; i++)
		for(int k=0; k<Width+1; k++)
		{
			VV.Position.Assign(StartX+k*CellSize, Y, StartZ+i*CellSize);

			*pVerts++ = VV;

			VertexCount ++;
		}

		for(int i=0; i<Height; i++)
			for(int k=0; k<Width; k++)
			{
				*pIdxs++ = i*(Width+1)+k;
				*pIdxs++ = i*(Width+1)+k+Width+1;
				*pIdxs++ = i*(Width+1)+k+1;

				*pIdxs++ = i*(Width+1)+k+Width+1;
				*pIdxs++ = i*(Width+1)+k+Width+2;
				*pIdxs++ = i*(Width+1)+k+1;

				IndexCount += 6;
			}


			Unlock();

			return 1;
}

template <class vert_s>
int r3dXPSObject<vert_s>::InitGridSideConnectorUP(int StartX, int StartZ,int Width, int Height, float CellSize, float Y )
{
	DWORD	dwFlags;

	pDecl     = vert_s::getDecl();
	Flags       = 0;
	int Width2 = Width/2;
	int Height2 = Height/2;
	float CellSize2 = CellSize*2;
	VertexSize  = ((Width2+1)*2)-1;
	VertexCount = 0;

	IndexSize   = Width2*3;
	IndexCount  = 0;

	LockMode    = 0;

	dwFlags     = D3DUSAGE_WRITEONLY; 

	if(Flags & POLYBUFFER_Dynamic)
		dwFlags |= D3DUSAGE_DYNAMIC;

/*	hr = r3dRenderer->pd3ddev->CreateVertexBuffer(
		VertexSize * sizeof(vert_s),
		dwFlags,
		VertFVF,
		D3DPOOL_DEFAULT,
		&pVB, NULL);

	hr = r3dRenderer->pd3ddev->CreateIndexBuffer(
		IndexSize * sizeof(DWORD),
		dwFlags, 
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&pIB, NULL);*/

	pVB = gfx_new r3dVertexBuffer( VertexSize, sizeof(vert_s), VertFVF, dwFlags & D3DUSAGE_DYNAMIC );
	pIB = gfx_new r3dIndexBuffer( IndexSize, dwFlags & D3DUSAGE_DYNAMIC, sizeof(DWORD) );

	r3dRenderer->Stats.BufferMem += VertexSize * sizeof(vert_s) + IndexSize * sizeof(DWORD);

	sprintf(Name, "XPS: GRID CONN UP [%p(v:%d i:%d)]", this, VertexSize, IndexSize);

	LockAll();

	vert_s VV;

	// add vertices
	for(int i=0; i<Width2+1; i++)
	{
		VV.Position.Assign(StartX+i*CellSize2, Y, StartZ+Height*CellSize); *pVerts++ = VV; VertexCount++;
		if(i!=Width2)
		{
			VV.Position.Assign(StartX+(i*2+1)*CellSize, Y, StartZ+Height*CellSize); *pVerts++ = VV; VertexCount++;
		}
	}

	for(int i=0; i<Width2; i++)
	{
		*pIdxs++ = i*2+0;
		*pIdxs++ = i*2+1;
		*pIdxs++ = i*2+2;

		IndexCount += 3;
	}

	Unlock();

	return 1;
}

template <class vert_s>
int r3dXPSObject<vert_s>::InitGridSideConnectorDOWN(int StartX, int StartZ,int Width, int Height, float CellSize, float Y )
{
	DWORD	dwFlags;

	pDecl     = vert_s::getDecl();
	Flags       = 0;
	int Width2 = Width/2;
	int Height2 = Height/2;
	float CellSize2 = CellSize*2;
	VertexSize  = ((Width2+1)*2)-1;
	VertexCount = 0;

	IndexSize   = Width2*3;
	IndexCount  = 0;

	LockMode    = 0;

	dwFlags     = D3DUSAGE_WRITEONLY; 

	if(Flags & POLYBUFFER_Dynamic)
		dwFlags |= D3DUSAGE_DYNAMIC;

	/*hr = r3dRenderer->pd3ddev->CreateVertexBuffer(
		VertexSize * sizeof(vert_s),
		dwFlags,
		VertFVF,
		D3DPOOL_DEFAULT,
		&pVB, NULL);

	hr = r3dRenderer->pd3ddev->CreateIndexBuffer(
		IndexSize * sizeof(DWORD),
		dwFlags, 
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&pIB, NULL);*/

	pVB = gfx_new r3dVertexBuffer( VertexSize, sizeof(vert_s), VertFVF, dwFlags & D3DUSAGE_DYNAMIC );
	pIB = gfx_new r3dIndexBuffer( IndexSize, dwFlags & D3DUSAGE_DYNAMIC, sizeof(DWORD) );

	r3dRenderer->Stats.BufferMem += VertexSize * sizeof(vert_s) + IndexSize * sizeof(DWORD);

	sprintf(Name, "XPS: GRID CONN DOWN [%p(v:%d i:%d)]", this, VertexSize, IndexSize);

	LockAll();

	vert_s VV;

	// add vertices
	for(int i=0; i<Width2+1; i++)
	{
		VV.Position.Assign(StartX+i*CellSize2, Y, StartZ); *pVerts++ = VV; VertexCount++;
		if(i!=Width2)
		{
			VV.Position.Assign(StartX+(i*2+1)*CellSize, Y, StartZ); *pVerts++ = VV; VertexCount++;
		}
	}

	for(int i=0; i<Width2; i++)
	{
		*pIdxs++ = i*2+1;
		*pIdxs++ = i*2+0;
		*pIdxs++ = i*2+2;

		IndexCount += 3;
	}

	Unlock();

	return 1;
}

template <class vert_s>
int r3dXPSObject<vert_s>::InitGridSideConnectorLEFT(int StartX, int StartZ,int Width, int Height, float CellSize, float Y )
{
	DWORD	dwFlags;

	pDecl     = vert_s::getDecl();
	Flags       = 0;
	int Width2 = Width/2;
	int Height2 = Height/2;
	float CellSize2 = CellSize*2;
	VertexSize  = ((Height2+1)*2)-1;
	VertexCount = 0;

	IndexSize   = Height2*3;
	IndexCount  = 0;

	LockMode    = 0;

	dwFlags     = D3DUSAGE_WRITEONLY; 

	if(Flags & POLYBUFFER_Dynamic)
		dwFlags |= D3DUSAGE_DYNAMIC;

	/*hr = r3dRenderer->pd3ddev->CreateVertexBuffer(
		VertexSize * sizeof(vert_s),
		dwFlags,
		VertFVF,
		D3DPOOL_DEFAULT,
		&pVB, NULL);

	hr = r3dRenderer->pd3ddev->CreateIndexBuffer(
		IndexSize * sizeof(DWORD),
		dwFlags, 
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&pIB, NULL);*/

	pVB = gfx_new r3dVertexBuffer( VertexSize, sizeof(vert_s), VertFVF, dwFlags & D3DUSAGE_DYNAMIC );
	pIB = gfx_new r3dIndexBuffer( IndexSize, dwFlags & D3DUSAGE_DYNAMIC, sizeof(DWORD) );

	r3dRenderer->Stats.BufferMem += VertexSize * sizeof(vert_s) + IndexSize * sizeof(DWORD);

	sprintf(Name, "XPS: GRID CONN LEFT [%p(v:%d i:%d)]", this, VertexSize, IndexSize);

	LockAll();

	vert_s VV;

	// add vertices
	for(int i=0; i<Height2+1; i++)
	{
		VV.Position.Assign(StartX, Y, StartZ+i*CellSize2); *pVerts++ = VV; VertexCount++;
		if(i!=Height2)
		{
			VV.Position.Assign(StartX, Y, StartZ+(i*2+1)*CellSize); *pVerts++ = VV; VertexCount++;
		}
	}

	for(int i=0; i<Height2; i++)
	{
		*pIdxs++ = i*2+0;
		*pIdxs++ = i*2+1;
		*pIdxs++ = i*2+2;

		IndexCount += 3;
	}

	Unlock();

	return 1;
}

template <class vert_s>
int r3dXPSObject<vert_s>::InitGridSideConnectorRIGHT(int StartX, int StartZ,int Width, int Height, float CellSize, float Y )
{
	DWORD	dwFlags;

	pDecl     = vert_s::getDecl();
	Flags       = 0;
	int Width2 = Width/2;
	int Height2 = Height/2;
	float CellSize2 = CellSize*2;
	VertexSize  = ((Height2+1)*2)-1;
	VertexCount = 0;

	IndexSize   = Height2*3;
	IndexCount  = 0;

	LockMode    = 0;

	dwFlags     = D3DUSAGE_WRITEONLY; 

	if(Flags & POLYBUFFER_Dynamic)
		dwFlags |= D3DUSAGE_DYNAMIC;

/*	hr = r3dRenderer->pd3ddev->CreateVertexBuffer(
		VertexSize * sizeof(vert_s),
		dwFlags,
		VertFVF,
		D3DPOOL_DEFAULT,
		&pVB, NULL);

	hr = r3dRenderer->pd3ddev->CreateIndexBuffer(
		IndexSize * sizeof(DWORD),
		dwFlags, 
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&pIB, NULL);
		*/

	pVB = gfx_new r3dVertexBuffer( VertexSize, sizeof(vert_s), VertFVF, dwFlags & D3DUSAGE_DYNAMIC );
	pIB = gfx_new r3dIndexBuffer( IndexSize, dwFlags & D3DUSAGE_DYNAMIC, sizeof(DWORD) );

	r3dRenderer->Stats.BufferMem += VertexSize * sizeof(vert_s) + IndexSize * sizeof(DWORD);

	sprintf(Name, "XPS: GRID CONN RIGHT [%p(v:%d i:%d)]", this, VertexSize, IndexSize);

	LockAll();

	vert_s VV;

	// add vertices
	for(int i=0; i<Height2+1; i++)
	{
		VV.Position.Assign(StartX + Width*CellSize, Y, StartZ+i*CellSize2); *pVerts++ = VV; VertexCount++;
		if(i!=Height2)
		{
			VV.Position.Assign(StartX + Width*CellSize, Y, StartZ+(i*2+1)*CellSize); *pVerts++ = VV; VertexCount++;
		}
	}

	for(int i=0; i<Height2; i++)
	{
		*pIdxs++ = i*2+1;
		*pIdxs++ = i*2+0;
		*pIdxs++ = i*2+2;

		IndexCount += 3;
	}

	Unlock();

	return 1;
}


template <class vert_s>
int r3dXPSObject<vert_s>::InitDome( const float Radius, const float Height, const int Rings, const int Segments)
{
	DWORD	dwFlags;

	//int rings    = Max( a_rings, 1 ) + 1;
	//int segments = Max( a_segments, 4 ) + 1;

	pDecl     = vert_s::getDecl();
	Flags       = 0;
	VertexSize  = (Rings+1) * (Segments + 1);
	VertexCount = 0;

	IndexSize   = Rings*Segments*2*3;
	IndexCount  = 0;

	LockMode    = 0;

	dwFlags     = D3DUSAGE_WRITEONLY; 

	if(Flags & POLYBUFFER_Dynamic)
		dwFlags |= D3DUSAGE_DYNAMIC;

	pVB = gfx_new r3dVertexBuffer( VertexSize, sizeof(vert_s), 0, (dwFlags & D3DUSAGE_DYNAMIC)?true:false );
	pIB = gfx_new r3dIndexBuffer( IndexSize, (dwFlags & D3DUSAGE_DYNAMIC)?true:false, sizeof(DWORD) );

	sprintf(Name, "XPS: SKYDOME [%p(v:%d i:%d)]", this, VertexSize, IndexSize);

	LockAll();

	vert_s VV;

	// add vertices
	float    g_dDegToRad     =   3.1415926f / 180.0f;

	for( int y = 0; y < Rings+1; y++)
	{
		float phi = (90.0f - float( y ) / float(Rings) * 90.0f )* g_dDegToRad;

		for( int x = 0; x<Segments+1; x++ )
		{
			float theta = float( x ) / float(Segments) * 360.0f * g_dDegToRad;


			VV.Position.Assign(	static_cast< float >( sinf( phi ) * cosf( theta ) * Radius ),
				static_cast< float >( cosf( phi ) * Height ),
				static_cast< float >( sinf( phi ) * sinf( theta ) * Radius ) );

			VV.tu = float(x) / float(Segments);
			VV.tv = 1 - float(y) / float(Rings);

			*pVerts++ = VV;
			VertexCount ++;
		}
	}


	for(int i=0; i<Rings; i++)
		for(int k=0; k<Segments; k++)
		{
			*pIdxs++ = i*(Segments+1)+k;
			*pIdxs++ = i*(Segments+1)+k+1;
			*pIdxs++ = i*(Segments+1)+k+Segments+1;

			*pIdxs++ = i*(Segments+1)+k+1;
			*pIdxs++ = i*(Segments+1)+k+Segments+2;
			*pIdxs++ = i*(Segments+1)+k+Segments+1;


			IndexCount += 6;
		}

		Unlock();

		RenderType = 2;

		return 1;
}







template <class vert_s>
int r3dXPSObject<vert_s>::Destroy()
{
	if(LockMode) 
		Unlock();

	SAFE_DELETE(pVB);
	SAFE_DELETE(pIB);

	return 1;
}



template <class vert_s>
int r3dXPSObject<vert_s>::IsValid()
{
	return pVB && pIB;
}




template <class vert_s>
void r3dXPSObject<vert_s>::Draw()
{
	if(LockMode)
		Unlock();

	if(VertexCount < 3)
		return;

	d3dc._SetDecl(pDecl);

	pIB->Set();
	pVB->Set( 0 );

	if (RenderType != 2)
	{
		r3dRenderer->DrawIndexed(
			D3DPT_TRIANGLELIST,0,
			0,
			VertexCount - 0,
			0,
			(IndexCount - 0) / 3
			);
	}
	else
		r3dRenderer->DrawIndexed(
		D3DPT_TRIANGLELIST,0,
		0,
		VertexCount - 0,
		0,
		(IndexCount - 0) /3
		);

}


template <class vert_s>
int r3dXPSObject<vert_s>::AddVertices(vert_s *v, int NumVerts)
{
	int	NumIdxs = (NumVerts - 2) * 3;
	switch(LockMode) {
case 0:
	if(!Lock(NumVerts, NumIdxs, &pVerts, &pIdxs, NULL))
		return 0;
	break;
case 2:
	if((VertexCount + NumVerts + 1 >= VertexSize) || (IndexCount + NumIdxs + 1 >= IndexSize)) {
		if(!(Flags & POLYBUFFER_AutoFlush))
			return 0;
		Flush();
		LockAll();
	}
	break;
	}

	// add indices
	for(int i = 1; i < NumVerts-1; i++) {
		*pIdxs++ = VertexCount + 0;
		*pIdxs++ = VertexCount + i;
		*pIdxs++ = VertexCount + i + 1;
	}
	IndexCount += NumIdxs;

	// add vertices
	for(i=0; i<NumVerts; i++) {
		*pVerts++ = *v++;
	}
	VertexCount += NumVerts;

	if(LockMode == 1)
		Unlock();

	return 1;
}

template <class vert_s>
int r3dXPSObject<vert_s>::AddTriangle(vert_s *v)
{
	switch(LockMode) {
case 0:
	if(!Lock(3, 3, &pVerts, &pIdxs, NULL))
		return 0;
	break;
case 2:
	if((VertexCount + 3 + 1 >= VertexSize) || (IndexCount + 3 + 1 >= IndexSize)) {
		if(!(Flags & POLYBUFFER_AutoFlush))
			return 0;
		Flush();
		LockAll();
	}
	break;
	}

	// add vertices
	*pVerts++ = *v++;
	*pVerts++ = *v++;
	*pVerts++ = *v++;
	IndexCount += 3;

	// add indices
	*pIdxs++ = VertexCount + 0;
	*pIdxs++ = VertexCount + 1;
	*pIdxs++ = VertexCount + 2;
	VertexCount += 3;

	if(LockMode == 1)
		Unlock();

	return 1;
}


template <class vert_s>
int r3dXPSObject<vert_s>::LockAll()
{
	if(LockMode)
		return 0;

	// lock the whole buffer
	/*DWORD lock_flags = D3DLOCK_NOSYSLOCK;
	if(Flags & POLYBUFFER_Dynamic) // you can use DISCARD only on DYNAMIC buffers
		lock_flags |= D3DLOCK_DISCARD;
	hr = pVB->Lock(0, VertexSize * sizeof(vert_s), (void **)&pVerts, lock_flags);
	hr = pIB->Lock(0, IndexSize  * sizeof(DWORD),   (void **)&pIdxs,  lock_flags);*/

	pVerts = ( vert_s * )pVB->Lock( 0, VertexSize );
	pIdxs = ( DWORD * )pIB->Lock( 0, IndexSize );

	VertexCount = 0;
	IndexCount  = 0;

	LockMode = 2;
	return 1;
}

template <class vert_s>
int r3dXPSObject<vert_s>::Lock(int NumVerts, int NumIdxs, vert_s **pV, DWORD **pI, int *FirstIndex, bool bReadOnly)
{
	R3D_ENSURE_MAIN_THREAD();

	int		dwFlags = D3DLOCK_NOSYSLOCK;

	if(VertexCount != 0) 
		dwFlags = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE;

	if(bReadOnly)
		dwFlags = D3DLOCK_READONLY;

	if(!bReadOnly && ((VertexCount + NumVerts > VertexSize) || (IndexCount + NumIdxs > IndexSize)) ) {
		if(!(Flags & POLYBUFFER_AutoFlush))
			return 0;
		//Flush(); // I dont know what is it. But it is not compiling.
		dwFlags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
	}

	//hr = pVB->Lock(VertexCount * sizeof(vert_s), NumVerts * sizeof(vert_s), (void **)&pVerts, dwFlags);
	//hr = pIB->Lock(IndexCount  * sizeof(DWORD),   NumIdxs  * sizeof(WORD),   (void **)&pIdxs,  dwFlags);

	pVerts = ( vert_s * )pVB->Lock( bReadOnly?0:VertexCount, NumVerts );
	pIdxs = ( DWORD * )pIB->Lock( bReadOnly?0:IndexCount, NumIdxs );

	if(bReadOnly)
	{
		if(FirstIndex)
			*FirstIndex  = 0;
		*pV          = pVerts;
		*pI          = pIdxs;
	}
	else
	{
		if(FirstIndex) 
		{
			*FirstIndex  = VertexCount;
			VertexCount += NumVerts;
			IndexCount  += NumIdxs;
			*pV          = pVerts;
			*pI          = pIdxs;
		}
	}

	LockMode = 1;
	return 1;
}

template <class vert_s>
void r3dXPSObject<vert_s>::Unlock()
{
	if(!LockMode) 
		return;
	pIB->Unlock();
	pVB->Unlock();

	LockMode = 0;
}
