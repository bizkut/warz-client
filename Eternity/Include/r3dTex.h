#ifndef	__ETERNITY_R3DTEXTURE_H
#define	__ETERNITY_R3DTEXTURE_H

// textures can be allocated only with helper functions from r3dRenderer class

#include "r3dRender.h"

struct DeviceQueueItem ;

class r3dTexture
{
	friend void _InsertTexture(r3dTexture **FirstTexture, r3dTexture *Tex);
	friend r3dTexture* _CreateTexture();
	friend void _DeleteTexture(r3dTexture* tex);
	friend void ProcessItem( const DeviceQueueItem& item );
public:
	enum Flags_e
	{
		fCreated        = (1<<3),
		fLocked         = (1<<4),
		fLockedForWrite = (1<<5),	// so, when Unlock() MipMaps will be regenerated
		fRenderTarget   = (1<<6),
		fPlayerTexture  = (1<<7),	// add to player textures when calculating memory stats
		fNeedReload     = (1<<8),	// temporary flag set by r3dRenderLayer when destroying/creating pool default textures
		fFileTexture    = (1<<9),
		fCheckPow2		= (1<<10)
	};

	static void Init() ;
	static void Close() ;

	static void LoadTexture( struct r3dTaskParams* taskParams ) ;
	int 		Load( const char* fname, D3DFORMAT targetTexFormat = D3DFMT_FROM_FILE, int downScale = 1, int maxDim = 0, D3DPOOL Pool = D3DPOOL_MANAGED );
	void		SetLoadData( const char* fname, int downScale, int maxDim, D3DPOOL pool, D3DFORMAT fmt );

	void		CheckPow2();

	void		Load();

	int			Save(const char* Name, bool bFullMipChain = false);
	void		Setup( int Width, int Height, int Depth, D3DFORMAT TexFmt, int NumMipMaps, r3dD3DTextureTunnel* texture, bool isRenderTarget );
	void		SetupCubemap( int EdgeLength, D3DFORMAT TexFmt, int NumMipMaps, r3dD3DTextureTunnel* texture, bool isRenderTarget );
	void		RegisterCreated();
	int			Create(int Width, int Height, D3DFORMAT TexFmt, int NumMipMaps, D3DPOOL Pool = D3DPOOL_MANAGED );
	void		Destroy() ;
	int			CreateVolume(int Width, int Height, int Depth, D3DFORMAT TargetTexFormat, int NumMipMaps, D3DPOOL Pool = D3DPOOL_MANAGED );
	int			CreateCubemap( int EdgeLength, D3DFORMAT TargetTexFormat, int NumMipMaps, D3DPOOL Pool = D3DPOOL_MANAGED );	

	bool		IsValid() { return m_TexArray && m_TexArray[0].Valid() && !IsMissing(); } // do not consider texture valid if it's missing
	bool		IsMissing() const { return Missing; }

	void		Unload( bool allowAsync = true );

	static void DestroyTextureResources( struct r3dTaskParams* taskParams );
	void		DestroyResources( bool allowAsync );
	void		DoDestroyResources();
	void		LoadResources();

	void		SetNeedReload( int bDoNeed );
	int			NeedsReload() const;

	int			PendingAsyncOps() const;
	int			IsDrawable() const;

	void*		Lock(int LockForWrite, const RECT *LocRect=NULL);
	void		Unlock();

	int			GetTextureSizeInVideoMemory(); // return size of this texture in video memory

	int			GetDownScale() const { return m_DownScale; }
	int			GetMaxDim() const { return m_MaxDim; }
	D3DPOOL		GetPool() const { return m_Pool; }


	R3D_FORCEINLINE r3dD3DTextureTunnel GetD3DTunnel() const { r3d_assert(m_TexArray[0].Valid()); return m_TexArray[0] ; };
	R3D_FORCEINLINE IDirect3DBaseTexture9* AsBaseTex() const { r3d_assert(m_TexArray[0].Valid()); return m_TexArray[0].AsBaseTex(); }
	R3D_FORCEINLINE IDirect3DTexture9* AsTex2D() const { r3d_assert(m_TexArray[0].Valid()); return m_TexArray[0].AsTex2D(); }
	R3D_FORCEINLINE IDirect3DCubeTexture9* AsTexCUBE() const { r3d_assert(m_TexArray[0].Valid()); return m_TexArray[0].AsTexCube() ; }
	R3D_FORCEINLINE IDirect3DVolumeTexture9* AsTexVolume() const { r3d_assert(m_TexArray[0].Valid()); return m_TexArray[0].AsTexVolume(); }
	R3D_FORCEINLINE int GetRefs() const { return Instances; }

	int			GetFlags() const { return Flags; }
	int			GetID() const { return ID; }
	int			GetWidth() const { return Width; }
	int			GetHeight() const { return Height; }
	int			GetDepth() const { return Depth; }
	int			GetNumMipmaps() const { return NumMipMaps; }
	int			GetLockPitch() const { return Pitch ; }
	bool		isCubemap() const { return bCubemap; }

	IDirect3DBaseTexture9* GetD3DTexture();
	void	SetNewD3DTexture(IDirect3DBaseTexture9* newTex);
	D3DFORMAT GetD3DFormat() const { return TexFormat; }

	void		SetDebugD3DComment(const char* text);
	void		MarkPlayerTexture() ;

	const r3dFileLoc& getFileLoc() const { return Location; }
	void OverwriteFileLocation(const char* file) { r3d_assert(file); r3dscpy(Location.FileName, file); }

	r3dTexture	*pNext, *pPrev;
	bool		bPersistent;
private:
	void	DestroyInternal();
	int 	DoLoad();

	void	LoadTextureInternal(int index, void* FileInMemoryData, uint32_t FileInMemorySize, const char* DEBUG_NAME );
	void	LoadTextureInternal(int index, const char* FName);

	void	UpdateTextureStats( int size ) ;

	int		Flags;
	int		ID;

	int		Width;
	int		Height;
	int		Depth;
	int		NumMipMaps;
	bool	bCubemap;
	bool	Missing;

	volatile LONG Instances;
	volatile LONG m_PendingAsyncOps;

	int m_DownScale;
	int m_MaxDim;

	D3DPOOL m_Pool;

	r3dD3DTextureTunnel*	m_TexArray ;
	int						m_iNumTextures;
	float*					m_pDelayTextureArray; // array of delays before switching to next texture
	float					m_LastAccess;
	int						m_AccessCounter;

	D3DFORMAT	TexFormat;	
	int			Pitch;				// pitch of line, in bytes;

	r3dFileLoc	Location;

	// you can't directly create or destroy texture... protected from accidents
protected:
	r3dTexture();
	~r3dTexture();
};

R3D_FORCEINLINE
int
r3dTexture::PendingAsyncOps() const
{
	return m_PendingAsyncOps;
}

R3D_FORCEINLINE
int
r3dTexture::IsDrawable() const
{
	return !m_PendingAsyncOps && Instances;
}

float GetD3DTexFormatSize(D3DFORMAT Fmt);

typedef void (*TextureReloadListener)( r3dTexture* ) ;

void AddTextureReloadListener( TextureReloadListener listener ) ;
void RemoveTextureReloadListener( TextureReloadListener listener ) ;

bool HasTextureReloadListener( TextureReloadListener listener ) ;

class r3dThumbnailTexture
{
	friend void MakeThumbNailInMainThread( void * param );

public:
	enum
	{
		THUMBNAIL_DIM = 64,
		THUMBNAIL_MIPS = 6
	};

	r3dThumbnailTexture();
	~r3dThumbnailTexture();

public:
	void Init( const char* path, int fullSizeTextureDownScale, int fullSizeTextureMaxDim, D3DPOOL pool, int gameResourcePool );

	void UpdateTexture( r3dTexture* newFullSizeTexture );

	void LoadFullSizeTexture();
	void MakeThumbnail();
	void RecreatePoolDefaultThumbnail();
	void ReleasePoolDefaultThumnail();
	void Unload();

	void Update();

	int HasPoolDefaultThumbnail() const;
	int IsFullSize() const;

	bool IsValid() const;
	bool IsDrawable() const;

	D3DFORMAT GetD3DFormat() const;

	int GetNumMipmaps();

	int GetThumbnailByteSize();

	const char* GetFilePath() const;

	IDirect3DBaseTexture9* GetD3DTex();

	r3dTexture* GetFullSizeTexture();

	int GetRefs() const;

	static void AddThumbToVMEM( int size );

	int GetWidth() const;
	int GetHeight() const;

	static int TotalThumbnailSizeInVMem;
	static int TotalThumbnailSizeInSysMem;

private:
	void DoMakeThumbnail();
	void ReleaseSysMemThumbnail();

private:
	r3dTexture* FullSizeTex;
	
	r3dD3DTextureTunnel PoolDefaultThumbnail;
	r3dD3DTextureTunnel SysMemThumbnail;
};

//------------------------------------------------------------------------

R3D_FORCEINLINE bool r3dThumbnailTexture::IsValid() const
{
	if( !FullSizeTex->PendingAsyncOps() && FullSizeTex->GetRefs() )
		return FullSizeTex->IsValid();
	else
		return true;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE bool r3dThumbnailTexture::IsDrawable() const
{
	return true;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE IDirect3DBaseTexture9* r3dThumbnailTexture::GetD3DTex()
{
	if( FullSizeTex->IsDrawable() )
		return FullSizeTex->AsBaseTex();
	else
	{
		if( !PoolDefaultThumbnail.Valid() )
		{
			MakeThumbnail();
		}

		return PoolDefaultThumbnail.AsBaseTex();
	}
}

//------------------------------------------------------------------------

R3D_FORCEINLINE r3dTexture* r3dThumbnailTexture::GetFullSizeTexture()
{
	return FullSizeTex;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE int r3dThumbnailTexture::GetRefs() const
{
	return FullSizeTex->GetRefs();
}

//------------------------------------------------------------------------

R3D_FORCEINLINE int r3dThumbnailTexture::GetWidth() const
{
	if( FullSizeTex->IsDrawable() )
		return FullSizeTex->GetWidth();

	D3DSURFACE_DESC sdesc;
	SysMemThumbnail.GetLevelDesc2D( 0, &sdesc );

	return sdesc.Width;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE int r3dThumbnailTexture::GetHeight() const
{
	if( FullSizeTex->IsDrawable() )
		return FullSizeTex->GetHeight();

	D3DSURFACE_DESC sdesc;
	SysMemThumbnail.GetLevelDesc2D( 0, &sdesc );

	return sdesc.Height;
}

#ifndef FINAL_BUILD
void DEBUG_ReportTexStats();
#endif

#endif // __ETERNITY_R3DTEXTURE_H
