#pragma once

#include "r3dTypedefs.h"
#include "r3dAssert.h"

class r3dBitMaskArray
{
public:
	class Proxy
	{
	public:
		Proxy( uint32_t * data, uint32_t bit );

		Proxy& operator = ( uint32_t val );
		operator uint32_t();

	private:
		uint32_t*	mData;
		uint32_t	mBit;
	};

	enum
	{
		ELEM_RESOLUTION = sizeof( uint32_t ) * 8 
	};

public:
	r3dBitMaskArray ( );
	r3dBitMaskArray ( const r3dBitMaskArray& rhs );
	~r3dBitMaskArray ( );

	r3dBitMaskArray& operator = ( const r3dBitMaskArray& arr );

	bool	operator [ ] ( uint32_t iBit ) const;
	Proxy	operator [ ] ( uint32_t iBit );

	bool Test ( uint32_t iBit ) const;

	void Set ( uint32_t iBit );
	void Reset ( uint32_t iBit );
	void Resize ( uint32_t _iBitCount );
	void ZeroAll ();
	void SetAll ();
	void Reset ();

	uint32_t Count() const;

	uint32_t*	GetDataPtr() const;
	uint32_t	GetDataSize() const;

	void		SetData( const void* bits, uint32_t dataSize );

private:

	uint32_t * pData;
	uint32_t   iBlockCount;
	uint32_t   iAllocatedBlockCount;
	uint32_t   iBitCount;	
};

// inline

R3D_FORCEINLINE bool r3dBitMaskArray::Test ( uint32_t iBit ) const
{
	r3d_assert ( iBit < iBitCount );
	uint32_t iBlock = iBit / ELEM_RESOLUTION;
	uint32_t iBitInBlock = iBit %  ELEM_RESOLUTION;
	return ( pData [iBlock] & (1 << iBitInBlock) ) ? true : false;
}

R3D_FORCEINLINE bool r3dBitMaskArray::operator [ ] ( uint32_t iBit ) const
{
	return Test( iBit );
}

R3D_FORCEINLINE r3dBitMaskArray::Proxy r3dBitMaskArray::operator[] ( uint32_t iBit )
{
	Proxy p( &pData[ iBit / ELEM_RESOLUTION ], iBit % ELEM_RESOLUTION );

	return p;
}

R3D_FORCEINLINE void r3dBitMaskArray::Set ( uint32_t iBit )
{
	r3d_assert ( iBit < iBitCount );
	uint32_t iBlock = iBit / ELEM_RESOLUTION;
	uint32_t iBitInBlock = iBit %  ELEM_RESOLUTION;
	
	pData [iBlock] |= (1 << iBitInBlock);
}

R3D_FORCEINLINE uint32_t r3dBitMaskArray::Count() const
{
	return iBitCount;
}

class r3dBitMaskArray2D
{
public:
	r3dBitMaskArray2D();
	~r3dBitMaskArray2D();

	void Resize( int width, int height );

	bool	Get( uint32_t x, uint32_t y ) const;
	void	Set( uint32_t x, uint32_t y, bool val );

	void	ZeroAll();
	void	SetAll();

	int		GetWidth() const;
	int		GetHeight() const;

	const void* GetDataPtr() const;
	int			GetDataSize() const;

	void		SetData( const void* bits, int byteSize );

private:
	int m_Width;
	int m_Height;

	r3dBitMaskArray m_BitMaskArray;
};

//------------------------------------------------------------------------

R3D_FORCEINLINE
bool
r3dBitMaskArray2D::Get( uint32_t x, uint32_t y ) const
{
	return m_BitMaskArray[ x + y * m_Width ];
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
void
r3dBitMaskArray2D::Set( uint32_t x, uint32_t y, bool val )
{
	m_BitMaskArray[ x + y * m_Width ] = val;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
void
r3dBitMaskArray2D::ZeroAll()
{
	m_BitMaskArray.ZeroAll();
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
void
r3dBitMaskArray2D::SetAll()
{
	m_BitMaskArray.SetAll();
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
int
r3dBitMaskArray2D::GetWidth() const
{
	return m_Width;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
int
r3dBitMaskArray2D::GetHeight() const
{
	return m_Height;
}

