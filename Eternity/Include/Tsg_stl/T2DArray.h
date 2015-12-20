#ifndef	TL_T2DARRAY_H
#define	TL_T2DARRAY_H

#include "TArray.h"
#include "TSlice.h"

namespace r3dTL
{
	template< typename T>
	class T2DArray
	{
	public:
		typedef TSlice< T > Row;
		typedef TArray< T > TData;

		// construction/ destruction
	public:
		T2DArray();
		T2DArray( uint32_t width, uint32_t height );

		// manipulation/ access
	public:
		Row			operator[] (uint32_t idx );
		const Row	operator[] (uint32_t idx ) const;
		bool		operator== (const T2DArray<T> &o) const;
		bool		operator!= (const T2DArray<T> &o) const;

		uint32_t	Width() const;
		uint32_t	Height() const;
		uint32_t	Count() const;

		void		Clear();

		void		Resize( uint32_t width, uint32_t height, const T& val = T() );

		T&			At( uint32_t x, uint32_t z );
		T&			AtIndex( uint32_t idx );

		const void*	GetDataPtr() const;
		void*		GetDataPtr();
		void		Swap( TData& data, uint32_t width, uint32_t height );

		// data
	private:
		TData		mData;
		uint32_t	mWidth;
		uint32_t	mHeight;
	};

	//------------------------------------------------------------------------

	template< typename T >
	T2DArray<T>::T2DArray()
	: mWidth( 0 )
	, mHeight( 0 )
	{
		
	}

	//------------------------------------------------------------------------

	template< typename T >
	T2DArray<T>::T2DArray( uint32_t width, uint32_t height )
	: mWidth( 0 )
	, mHeight( 0 )
	{
		Resize( width, height );
	}

	//------------------------------------------------------------------------

	template< typename T >
	typename T2DArray<T>::Row
	T2DArray<T>::operator[] (uint32_t idx )
	{
		r3d_assert( idx < mHeight );
		return Row( &mData[ idx * mWidth ], mWidth );
	}

	//------------------------------------------------------------------------

	template< typename T >
	const typename T2DArray<T>::Row
	T2DArray<T>::operator[] (uint32_t idx ) const
	{
		r3d_assert( idx < mHeight );
		return Row( const_cast<T*>(&mData[ idx * mWidth ]), mWidth );
	}

	//------------------------------------------------------------------------

	template< typename T >
	uint32_t
	T2DArray<T>::Width() const
	{
		return mWidth;
	}

	//------------------------------------------------------------------------

	template< typename T >
	uint32_t
	T2DArray<T>::Height() const
	{
		return mHeight;
	}

	//------------------------------------------------------------------------

	template< typename T >
	uint32_t
	T2DArray<T>::Count() const
	{
		return mData.Count();
	}

	//------------------------------------------------------------------------

	template< typename T >
	void
	T2DArray<T>::Clear()
	{
		mWidth	= 0;
		mHeight = 0;
		mData.Clear();
	}

	//------------------------------------------------------------------------

	template< typename T >
	void
	T2DArray<T>::Resize( uint32_t width, uint32_t height, const T& val /*= T()*/ )
	{
		TData copy;

		copy.Reserve( width * height );

		uint32_t copyWidth	= Min( width, mWidth );
		uint32_t copyHeight	= Min( height, mHeight );

		uint32_t originalWidth = mWidth;

		mWidth	= width;
		mHeight	= height;

		for( uint32_t j = 0, e = copyHeight; j < e; j ++ )
		{
			for( uint32_t i = 0, e = copyWidth; i < e; i ++ )
			{
				copy.PushBack( mData[ i + j * originalWidth ] );
			}

			// fill appended area with defaults
			if( copyWidth < width )
			{
				copy.Resize( copy.Count() + width - copyWidth , val );
			}
		}

		// fill appended area with defaults
		if( copyHeight < height )
		{
			copy.Resize( copy.Count() + width * ( height - copyHeight ), val );
		}

		mData.Swap( copy );
	}

	//------------------------------------------------------------------------

	template< typename T >
	R3D_FORCEINLINE
	T&
	T2DArray<T>::At( uint32_t x, uint32_t z )
	{
		return mData[ x + z * mWidth ];
	}

	//------------------------------------------------------------------------

	template< typename T >
	R3D_FORCEINLINE
	T&
	T2DArray<T>::AtIndex( uint32_t idx )
	{
		return mData[ idx ];
	}

	//------------------------------------------------------------------------

	template< typename T >
	R3D_FORCEINLINE
	const void*
	T2DArray<T>::GetDataPtr() const
	{
		if( mData.Count() )
			return &mData[ 0 ];
		else
			return NULL;
	}

	//------------------------------------------------------------------------

	template< typename T >
	R3D_FORCEINLINE
	void*
	T2DArray<T>::GetDataPtr()
	{
		return &mData[ 0 ];
	}

	//------------------------------------------------------------------------

	template< typename T >
	R3D_FORCEINLINE
	void
	T2DArray<T>::Swap( TData& data, uint32_t width, uint32_t height )
	{
		data.Swap( mData );

		mWidth = width;
		mHeight = height;

		r3d_assert( mData.Count() == mWidth * mHeight );
	}

	//------------------------------------------------------------------------

	template< typename T >
	bool
	T2DArray<T>::operator== (const T2DArray<T> &o) const
	{
		if (o.Count() != Count())
			return false;

		return 0 == memcmp(&o.mData.GetFirst(), &mData.GetFirst(), Count() * sizeof(T));
	}

	//------------------------------------------------------------------------

	template< typename T >
	bool
	T2DArray<T>::operator!= (const T2DArray<T> &o) const
	{
		return !(o == *this);
	}
}

#endif
