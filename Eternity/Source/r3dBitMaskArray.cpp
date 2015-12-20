#include "r3dPCH.h"

#include "r3dBitMaskArray.h"

//------------------------------------------------------------------------

r3dBitMaskArray::Proxy::Proxy( uint32_t * data, uint32_t bit )
: mData( data )
, mBit( bit )
{

}

//------------------------------------------------------------------------

r3dBitMaskArray::Proxy&
r3dBitMaskArray::Proxy::operator = ( uint32_t val )
{
	if( val )
		*mData |= 1 << mBit ;
	else
		*mData &= ~( 1 << mBit );

	return *this ;
}

//------------------------------------------------------------------------

r3dBitMaskArray::Proxy::operator uint32_t()
{
	return *mData >> mBit & 1 ;
}

//------------------------------------------------------------------------

r3dBitMaskArray::r3dBitMaskArray ()
: pData ( NULL )
, iBlockCount ( 0 )
, iAllocatedBlockCount( 0 )
, iBitCount ( 0 )
{
}

r3dBitMaskArray::r3dBitMaskArray ( const r3dBitMaskArray& cpy )
{
	this->r3dBitMaskArray::r3dBitMaskArray() ;
	*this = cpy ;
}

//------------------------------------------------------------------------

r3dBitMaskArray::~r3dBitMaskArray ()
{
	Reset ();
}

//------------------------------------------------------------------------

r3dBitMaskArray&
r3dBitMaskArray::operator = ( const r3dBitMaskArray& rhs )
{
	if( rhs.iBitCount )
	{
		Resize( rhs.iBitCount ) ;
		memcpy( pData, rhs.pData, rhs.iBlockCount * sizeof( uint32_t ) ) ;
	}
	else
	{
		iBitCount = 0 ;
		iBlockCount = 0 ;
	}

	return *this ;
}

//------------------------------------------------------------------------

void r3dBitMaskArray::Reset ( uint32_t iBit )
{
	r3d_assert ( iBit < iBitCount );
	uint32_t iBlock = iBit / ELEM_RESOLUTION ;
	uint32_t iBitInBlock = iBit %  ELEM_RESOLUTION ;

	pData [iBlock] &= ~(1 << iBitInBlock);
}

//------------------------------------------------------------------------

void r3dBitMaskArray::Resize ( uint32_t _iBitCount )
{
	r3d_assert ( _iBitCount > 0 );

	uint32_t _iBlockCount = ( _iBitCount - 1 ) / ELEM_RESOLUTION + 1 ;

	if( _iBlockCount > iAllocatedBlockCount )
	{
		iAllocatedBlockCount = _iBlockCount ;

		void* oldData = pData ;

		size_t elemSize = sizeof ( pData[0] ) ;

		pData = (uint32_t*)malloc ( _iBlockCount * elemSize );
		memset ( pData + iBlockCount, 0x00, ( _iBlockCount - iBlockCount ) * elemSize );

		if ( oldData )
		{
			memcpy( pData, oldData, iBlockCount * sizeof ( pData[0] ) ) ;
			free ( oldData );
		}
	}

	iBitCount = _iBitCount;
	iBlockCount = ( iBitCount - 1 ) / ELEM_RESOLUTION + 1;

}

//------------------------------------------------------------------------

void r3dBitMaskArray::ZeroAll ()
{
	if( iBitCount > 0 )
	{
		memset ( pData, 0x00, iBlockCount * sizeof ( pData[0] ) );
	}
}

//------------------------------------------------------------------------

void r3dBitMaskArray::SetAll ()
{
	if( iBitCount > 0 )
	{
		memset ( pData, 0xff, iBlockCount * sizeof ( pData[0] ) );
	}
}

//------------------------------------------------------------------------

void r3dBitMaskArray::Reset ()
{
	iBitCount = 0;
	iBlockCount = 0;
	iAllocatedBlockCount = 0 ;
	if ( pData )
		free ( pData );
	pData = NULL;
}

//------------------------------------------------------------------------

uint32_t* r3dBitMaskArray::GetDataPtr() const
{
	return pData ;
}

//------------------------------------------------------------------------

uint32_t r3dBitMaskArray::GetDataSize() const
{
	return iBitCount / 8 + ( iBitCount % 8 ? 1 : 0 ) ;
}

//------------------------------------------------------------------------

void r3dBitMaskArray::SetData( const void* bits, uint32_t dataSize )
{
	r3d_assert( dataSize == GetDataSize() ) ;

	memcpy( pData, bits, dataSize ) ;
}

//------------------------------------------------------------------------

r3dBitMaskArray2D::r3dBitMaskArray2D()
: m_Width( 0 )
, m_Height( 0 )
{

}

//------------------------------------------------------------------------

r3dBitMaskArray2D::~r3dBitMaskArray2D()
{

}

//------------------------------------------------------------------------

void
r3dBitMaskArray2D::Resize( int width, int height )
{
	m_Width = width ;
	m_Height = height ;

	m_BitMaskArray.Resize( width * height ) ;
}

//------------------------------------------------------------------------

const void*
r3dBitMaskArray2D::GetDataPtr() const
{
	return m_BitMaskArray.GetDataPtr() ;
}

//------------------------------------------------------------------------

int
r3dBitMaskArray2D::GetDataSize() const
{
	return m_BitMaskArray.GetDataSize() ;
}

//------------------------------------------------------------------------

void
r3dBitMaskArray2D::SetData( const void* bits, int byteSize )
{
	int nominalSize = m_Width * m_Height ;
	nominalSize = nominalSize / 8 + ( nominalSize % 8 ? 1 : 0 ) ;

	r3d_assert( nominalSize == byteSize ) ;

	m_BitMaskArray.SetData( bits, byteSize ) ;
}
