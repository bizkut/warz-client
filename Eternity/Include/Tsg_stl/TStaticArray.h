#ifndef	TL_TSTATICARRAY_H
#define	TL_TSTATICARRAY_H

namespace r3dTL
{
	template < typename T, uint32_t N>
	class TStaticArray
	{
	public:
		TStaticArray();

	public:
		T& operator [] ( uint32_t idx );
		const T& operator [] ( uint32_t idx ) const;

		uint32_t	Count			() const		{ return mSize; };
		uint32_t	MaxSize			() const		{ return N; };

		void		Erase			( uint32_t idx );
		void		PushBack		( const T& val );
		void		PushFront		( const T& val );
		void		Resize			( uint32_t count, const T& val = T() );
		void		Clear();

	private:
		T			mElems[ N ];
		uint32_t	mSize;
	};

	
	

	//--------------------------------------------------------------------------------------------------------
	template< typename T, uint32_t N >
	void TStaticArray<T,N>::Resize( uint32_t count, const T& val /*= T()*/ )
	{
		r3d_assert( count <= N );

		mSize = N < count ? N : count;
	}
	//--------------------------------------------------------------------------------------------------------
	
	template< typename T, uint32_t N >
	void TStaticArray<T,N>::Clear()
	{
		mSize = 0;
	}

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	T& TStaticArray<T,N>::operator [] ( uint32_t idx )
	{
		r3d_assert( idx < mSize );

		return mElems[ idx ];
	}


	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	const T&
	TStaticArray<T,N>::operator [] ( uint32_t idx ) const
	{
		r3d_assert ( idx < mSize );

		return mElems[ idx ];
	}

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	TStaticArray<T, N>::TStaticArray()
		: mSize( 0 )
	{
		// force init for built in types ']
		for( uint32_t i = 0, e = N; i < N; i ++ )
		{
			mElems[ i ] = T();
		}
	}

	//--------------------------------------------------------------------------------------------------------

	template< typename T, uint32_t N >
	void TStaticArray<T,N>::Erase( uint32_t idx )
	{
		r3d_assert( idx < mSize );

		for( uint32_t i = idx, e = mSize - 1; i < e; i ++ )
			mElems[i] = mElems[ i + 1 ];

		mSize--;
	}

	//--------------------------------------------------------------------------------------------------------

	template< typename T, uint32_t N >
	void TStaticArray<T,N>::PushBack( const T& val )
	{
		if( mSize >= N )
		{
			r3d_assert( false );
			return;
		}

		mElems[ mSize++ ] = val;
	}

	//--------------------------------------------------------------------------------------------------------

	template< typename T, uint32_t N >
	void TStaticArray<T,N>::PushFront( const T& val )
	{
		if( mSize >= N )
		{
			assert( false );
			return;
		}

		for( uint32_t i = mSize; i-- > 0; )
			mElems[i+1] = mElems[i];

		mElems[ 0 ] = val;
		mSize++;
	}

}

#endif