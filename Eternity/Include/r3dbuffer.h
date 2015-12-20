#ifndef	__R3D_BUFFER3D_H
#define	__R3D_BUFFER3D_H

class r3dVBuffer;
class r3dIBuffer16;
class r3dIBuffer32;

class r3dD3DQuery : public r3dIResource
{
  protected:
	D3DQUERYTYPE	type_;
	IDirect3DQuery9* query_;
	
  protected:
	// you can't directly create it - use CreateClass()
	r3dD3DQuery(D3DQUERYTYPE type, const r3dIntegrityGuardian& ig = r3dIntegrityGuardian() );

  public:
	  virtual void		D3DCreateResource();
	  virtual void		D3DReleaseResource();

	  virtual	~r3dD3DQuery();

static	r3dD3DQuery*	CreateClass(D3DQUERYTYPE type) { 
		return gfx_new r3dD3DQuery(type);
	}
	
	IDirect3DQuery9* d3d() { 
	  return query_;
	}
	
	ULONG		Release() {
	  delete this;
	  return 0;
	}
	
	HRESULT		Issue(DWORD dwIssueFlags)
	{
		R3D_ENSURE_MAIN_THREAD();
		return query_->Issue(dwIssueFlags);
	}
	
	HRESULT		GetData(void* pData,DWORD dwSize,DWORD dwGetDataFlags) 
	{
		R3D_ENSURE_MAIN_THREAD();
		return query_->GetData(pData, dwSize, dwGetDataFlags);
	}
};


// direct access (lock/unlock) *dynamic* vertex / index buffers.
class r3dD3DBuffer : public r3dIResource
{
  R3D_NOCLASSCOPY(r3dD3DBuffer);
  public:
	enum type_e {
	  BUFFER_None,
	  BUFFER_Vertex,
	  BUFFER_Index,
	};

  protected:
	union {
	  IDirect3DIndexBuffer9	 *pIB;
	  IDirect3DVertexBuffer9 *pVB;
	};
	type_e		m_Type;
	  
	DWORD		m_Size;
	DWORD		m_Stride;
	DWORD		m_Pos;

	// for vertex buffer
	LPDIRECT3DVERTEXDECLARATION9		m_Decl;

	// locked start/size
	DWORD		m_LockStart;
	DWORD		m_LockSize;
	
	bool		m_NeedDiscard;
	
  protected:
	// you can't directly create it - use derived classes
	r3dD3DBuffer(type_e type, int size, int stride, LPDIRECT3DVERTEXDECLARATION9 decl, const r3dIntegrityGuardian& ig = r3dIntegrityGuardian() );
	virtual	~r3dD3DBuffer();

  public:
	  virtual void		D3DCreateResource();
	  virtual void		D3DReleaseResource();

	  void		ForceDiscard() {
	  m_NeedDiscard = true;
	  m_Pos         = 0;
	}
	void*		LockData(int size, int* lstart);
	void		Unlock();
	
	IDirect3DVertexBuffer9* GetVB() { 
	  r3d_assert(m_Type == BUFFER_Vertex);
	  return pVB;
	}

	IDirect3DVertexBuffer9* GetIB() { 
	  r3d_assert(m_Type == BUFFER_Index);
	  return pVB;
	}
	
	void		Activate();

	int		IsLocked() const {
	  return m_LockSize;
	}
	int		GetSize() const {
	  return m_Size;
	}
	int		GetRemaining() const {
	  return m_Size - m_Pos;
	}
};

class r3dIBuffer16 : public r3dD3DBuffer
{
  friend r3dIBuffer16* r3dCreateIndexBuffer16(int size);
  protected:
	r3dIBuffer16(int size) : 
	  r3dD3DBuffer(BUFFER_Index, size, sizeof(WORD), 0)
	{
	}
  public:
	WORD*		Lock(int size, int *lstart) { 
	  return (WORD*)LockData(size, lstart); 
	}
};

class r3dIBuffer32 : public r3dD3DBuffer
{
  friend r3dIBuffer32* r3dCreateIndexBuffer32(int size);
  protected:
	r3dIBuffer32(int size) : 
	  r3dD3DBuffer(BUFFER_Index, size, sizeof(DWORD), 0)
	{
	}
  public:
	DWORD*		Lock(int size, int *lstart) { 
	  return (DWORD*)LockData(size, lstart); 
	}
};

class r3dVBuffer : public r3dD3DBuffer
{
  friend r3dVBuffer*   r3dCreateVertexBuffer(int size, int stride, LPDIRECT3DVERTEXDECLARATION9 decl);
  protected:
	r3dVBuffer(int size, int stride, LPDIRECT3DVERTEXDECLARATION9 decl) :
	  r3dD3DBuffer(BUFFER_Vertex, size, stride, decl)
	{
	}
  public:
	void*		Lock(int size, int *lstart) { 
	  return LockData(size, lstart); 
	}
};

template <class T>
class r3dVBufferT : public r3dD3DBuffer
{
  protected:
	r3dVBufferT(int size) : 
	  r3dD3DBuffer(BUFFER_Vertex, size, sizeof(T), T::getDecl())
	{
	}
  public:
	// templated piece of shit. I wasn't able to make templated friend function
static r3dVBufferT*	CreateClass(int size) {
	  r3dVBufferT* b = gfx_new r3dVBufferT(size);
	  return b;
	}

	T*		Lock(int size, int *lstart) { 
	  return (T*)LockData(size, lstart); 
	}
};


class r3dVertexArray : r3dIResource
{
  R3D_NOCLASSCOPY(r3dVertexArray);

public:
	enum Type
	{
		TRIANGLE_LIST,
		LINE_LIST,
		LINE_STRIP
	};

	friend r3dVertexArray* r3dCreateVertexArray(int size, int stride, LPDIRECT3DVERTEXDECLARATION9 decl, r3dVertexArray::Type type );

  protected:
	int		size_;
	int		stride_;
	LPDIRECT3DVERTEXDECLARATION9		decl_;
	r3dD3DVertexBufferTunnel			vrtBuff_;
	unsigned char* lockedPtr_;
	
	int		pos_, actual_, unflushed_, needed_;
	int 	type ;

	R3D_FORCEINLINE void	AddVtx(const void* vtx, int num) {
		memcpy(lockedPtr_ + actual_ * stride_, (unsigned char*)vtx + (num * stride_), stride_);
		actual_++;
	}

	r3dVertexArray( const r3dIntegrityGuardian& ig );
	void Lock();
	void Unlock();
  public:
	~r3dVertexArray();

	template< typename T>
	R3D_FORCEINLINE void AddVtxT(const T& vtx) 
	{
#ifdef _DEBUG
		r3d_assert( sizeof(vtx) == stride_ ) ;
#endif

		*(T*)(lockedPtr_ + actual_ * stride_) = vtx ;
		actual_++;
	}

	void		Add(const void* vtx, int num);	//lock-set-unlock
	void		AddList( const void* vtx, int num);	//lock-set-unlock
	void		Flush();	//draw unflushed primitives
	int			GetCapacity() const { return size_; }

	void StartVertices(int numVertsInPoly, int numPoly);	//locks buffer
	void SetVtx(const void* vtx, int num);
	void EndVertices();	//unlocks buffer

	void StartListVertices(int num);	//locks buffer
	void SetListVtx(const void* vtx, int num);
	void EndListVertices();	//unlocks buffer

	virtual	void		D3DCreateResource()  OVERRIDE;
	virtual	void		D3DReleaseResource() OVERRIDE;

};


class r3dScreenBuffer : public r3dIResource
{
	friend struct ShadowZBuffer;
	friend void SetupSurfacePointers( void* params );
 public: 
	enum Face
	{
		FACE_PX,
		FACE_NX,
		FACE_PY,
		FACE_NY,
		FACE_PZ,
		FACE_NZ,
		FACE_COUNT
	};

	enum
	{
		MAX_MIPS = 16
	};

	enum ZType
	{
		Z_SYSTEM,
		Z_OWN,
		Z_NO_Z,
		Z_SHADOW
	};

	struct CreationParameters
	{
		const char* name;
		float width;
		float height;
		ZType ZT;
		D3DFORMAT Format;
		D3DFORMAT zFormat;
		int bCubemap;
		int numMipLevels;
		int allowAutoScaleDown;
		r3dIntegrityGuardian ig; 

		CreationParameters()
		: name(0)
		, width(0)
		, height(0)
		, ZT(Z_NO_Z)
		, Format(D3DFMT_A8R8G8B8)
		, bCubemap(0)
		, numMipLevels(1)
		, allowAutoScaleDown(0)
		{

		}
	};

private:

	static r3dD3DSurfaceTunnel ShadowZBuf ;
	static r3dD3DTextureTunnel ShadowZBufTex ;
	// when combined with ztexture hack
	static r3dD3DSurfaceTunnel NullRT ;
	static int ActivateGuard ;

public:
	r3dD3DTextureTunnel		OurBBuf ;
	r3dD3DSurfaceTunnel		OurZBuf ;

	r3dD3DSurfaceTunnel		Surfs[ FACE_COUNT ][ MAX_MIPS ] ;

	r3dD3DSurfaceTunnel 	BBuf ;
	r3dD3DSurfaceTunnel 	ZBuf ;
	r3dTexture				*Tex;

	ZType		zType ;

	int			bCubemap;

	// may be 0 to create full chain
	int			CreationNumMipLevels;
	int			ActualNumMipLevels;

	D3DFORMAT	BufferFormat;
	int			BufferID;

	int			AllowAutoScaleDown ; // in case no vmem

	int			UsedMemory ;

	int			ActiveMip;

	struct Dims
	{
		float Width ;
		float Height ;
	};

	Dims			MipDims[ MAX_MIPS ] ;

	float			Width ;
	float			Height ;

	float			OldW;
	float			OldH;

	float GetActualMipWidth()
	{
		if (ActiveMip != -1)
		{
			r3d_assert(ActiveMip < GetNumMipLevels());
			return MipDims[ActiveMip].Width;
		}

		return Width;
	}

	float GetActualMipHeight()
	{
		if (ActiveMip != -1)
		{
			r3d_assert(ActiveMip < GetNumMipLevels());
			return MipDims[ActiveMip].Height;
		}

		return Height;
	}

	IDirect3DBaseTexture9*	AsBaseTex() const;
	IDirect3DTexture9*		AsTex2D() const;
	IDirect3DCubeTexture9*	AsTexCUBE() const;
	IDirect3DTexture9*		GetDepthTexture() const;

	IDirect3DSurface9*		GetTex2DSurface();

	IDirect3DSurface9*		GetTexCUBESurface( int face, int mip );

	static int				GetShadowZBufSize();

  protected:
	const char* debug_name; // save a point to static name of this surface
	r3dScreenBuffer(const CreationParameters &p);
	explicit r3dScreenBuffer(const char* name);

  public:
	virtual ~r3dScreenBuffer();

	virtual void		D3DCreateResource();
	virtual void		D3DReleaseResource();

	static void CreateShadowZBuffer( int X, int Y ) ;
	static void ReleaseShadowZBuffer();

	static r3dScreenBuffer* CreateClass(const r3dScreenBuffer::CreationParameters &p)
	{
		return gfx_new r3dScreenBuffer(p);
	}

	static r3dScreenBuffer* CreateEmptyClass(const char* name) {
		return gfx_new r3dScreenBuffer(name);
	}
	static r3dScreenBuffer* CreateClass(const char* name, float width, float height, D3DFORMAT format, ZType ZT = Z_NO_Z, int bIsCubeMap = 0, int numMipLevels = 1, int allowAutoScaleDown = 0)
	{
		r3dScreenBuffer::CreationParameters p;
		p.width = width ;
		p.height = height ;
		p.name = name;
		p.Format = format;
		p.ZT = ZT;
		p.bCubemap = bIsCubeMap;
		p.numMipLevels = numMipLevels;
		p.allowAutoScaleDown = allowAutoScaleDown;

		return gfx_new r3dScreenBuffer(p);
	}

	//! Sorry guys, i know it's retarded solution, but it's lifesaver for qick SSR test. I will fix this as soon as possible.
	void		SetActiveMip(int mip)
	{
		r3d_assert(mip < GetNumMipLevels());
		ActiveMip = mip;
	}

	void		Activate(int RTID = 0, int Face = FACE_PX, int Mip = 0  );
	void		Deactivate(int bReset= 1); 
	void 		SetMRT(int RTID, int bSet );

	bool		IsNull() const ;

	int			GetNumMipLevels() const; // for rendertargets that have mip levels
	void		SetDebugD3DComment(const char* text);

	const char* GetDebugName() const ;
};

//------------------------------------------------------------------------

R3D_FORCEINLINE
IDirect3DBaseTexture9*
r3dScreenBuffer::AsBaseTex() const
{
	return OurBBuf.AsBaseTex();
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
IDirect3DTexture9*
r3dScreenBuffer::AsTex2D() const
{
	r3d_assert( !bCubemap );
	return OurBBuf.AsTex2D();
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
IDirect3DCubeTexture9*
r3dScreenBuffer::AsTexCUBE() const
{
	r3d_assert( bCubemap );
	return OurBBuf.AsTexCube();
}


extern	r3dVertexArray*	r3dCreateVertexArray(int size, int stride, LPDIRECT3DVERTEXDECLARATION9 decl, r3dVertexArray::Type type );


inline r3dVBuffer* r3dCreateVertexBuffer(int size, int stride,LPDIRECT3DVERTEXDECLARATION9 decl) { 
  return gfx_new r3dVBuffer(size, stride, decl);
}
inline r3dIBuffer16*	r3dCreateIndexBuffer16(int size) {
  return gfx_new r3dIBuffer16(size);
}
inline r3dIBuffer32*	r3dCreateIndexBuffer32(int size) {
  return gfx_new r3dIBuffer32(size);
}

template <class T>
r3dVBufferT<T>*	r3dCreateVertexBuffer(int size) {
  return r3dVBufferT<T>::CreateClass(size);
}

bool IsDepthTextureFormat( D3DFORMAT fmt );

int r3dGetShadowZBufferSize() ;

#endif
