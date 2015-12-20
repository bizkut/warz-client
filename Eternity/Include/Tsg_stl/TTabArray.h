#ifndef	TL_TTABARRAY_H
#define	TL_TABTARRAY_H

#include "r3dSys_WIN.h"
#include "THelpers.h"

#include "r3dAssert.h"

namespace r3dTL
{

	template < typename T, uint32_t TAB >
	class TTabArray
	{
		// constants & types
	public:
		static const uint32_t TAB_SIZE = TAB;

		// construction/ destruction
	public:
		TTabArray();
		explicit TTabArray( uint32_t size, const T& val = T(), uint32_t val_tab = sizeof(T) );
		TTabArray( const TTabArray& arr );
		~TTabArray();

		// manipulation/ access
	public:
		TTabArray& operator = ( const TTabArray& arr );

		const T& operator[] ( uint32_t idx ) const;
		T& operator[] ( uint32_t idx );

		void Swap( TTabArray& arr );
		void Resize( uint32_t count, const T& val = T(), uint32_t val_tab = sizeof(T) );
	
		void		PushBack		( const T& val, uint32_t size /*= sizeof val*/ );

		template <typename U>
		void		PushBack		( const U& val );

		void		PopBack			();

		const T&	GetFirst		() const;
		const T&	GetLast			() const;

		T&			GetFirst		();
		T&			GetLast			();

		void 		Reserve			( uint32_t count );
		void 		Erase			( uint32_t idx );
		void 		Erase			( uint32_t start_idx, uint32_t count );
		void 		Clear			();

		uint32_t	Count			() const;

		int			FindItemIndex	( const T & item ) const;

		// helpers
	private:
		static uint8_t* Allocate( uint32_t count );
		static void Free( uint8_t* ptr );
		static void Destruct( uint8_t* ptr, uint32_t count );
		static void Construct( uint8_t* ptr, uint32_t count, const T& val, uint32_t tab );
		static void Construct( uint8_t* dest, uint8_t* src, uint32_t count );

		// data
	private:
		uint8_t*	mElems;
		uint32_t	mCount;
		uint32_t	mSpace;
	};

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	TTabArray<T,TAB>::TTabArray():
	mElems( NULL ),
	mCount( 0 ),
	mSpace( 0 )
	{
		
	}

	//------------------------------------------------------------------------
	/*explicit*/

	template< typename T, uint32_t TAB >
	TTabArray<T,TAB>::TTabArray( uint32_t size, const T& val /*= T()*/, uint32_t val_tab /*= sizeof(T)*/ ) :
	mElems( Allocate( size ) ),
	mCount( size ),
	mSpace( size )
	{
		Construct( mElems, size, val, val_tab );
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	TTabArray<T,TAB>::TTabArray( const TTabArray& arr ) :
	mElems( Allocate( arr.mCount ) ),
	mCount( arr.mCount ),
	mSpace( arr.mCount )
	{
		Construct( mElems, arr.mElems, mCount );
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	TTabArray<T,TAB>::~TTabArray()
	{
		COMPILE_ASSERT( TAB >= sizeof(T) );

		Destruct( mElems, mCount );
		Free( mElems );
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	TTabArray<T,TAB>&
	TTabArray<T,TAB>::operator = ( const TTabArray<T,TAB>& arr )
	{
		TTabArray<T,TAB> temp( arr );
		Swap( temp );
		return *this;
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	const T&
	TTabArray<T,TAB>::operator[] ( uint32_t idx ) const
	{
		r3d_assert( idx < mCount );
		return (const T&)mElems[ idx * TAB ];
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	T&
	TTabArray<T,TAB>::operator[] ( uint32_t idx )
	{
		r3d_assert( idx < mCount );
		return (T&)mElems[ idx * TAB ];
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	void
	TTabArray<T,TAB>::Swap( TTabArray& arr )
	{
		r3dTL::Swap( mElems, arr.mElems );
		r3dTL::Swap( mCount, arr.mCount );
		r3dTL::Swap( mSpace, arr.mSpace );
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	void
	TTabArray<T,TAB>::Resize( uint32_t count, const T& val /*= T()*/, uint32_t val_tab /*= sizeof(T)*/ )
	{
		if( count > mCount )
		{
			Reserve( count );
			Construct( mElems + mCount * TAB, count - mCount, val, val_tab );
		}
		else
		{
			Destruct( mElems + count * TAB, mCount - count );
		}

		mCount = count;
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	void
	TTabArray<T,TAB>::PushBack( const T& val, uint32_t size /*= sizeof val*/ )
	{
		assert( size >= sizeof val && size <= TAB );

		if( mCount >= mSpace )
		{
			TTabArray next;

			uint32_t newCount = mCount + 1;

			uint32_t newSpace = 0x80000000;

			while( !(newCount & newSpace) )
				newSpace >>= 1;

			newSpace <<= 1;

			next.Reserve( newSpace );

			// old ones
			Construct( next.mElems, mElems, mCount );

			// new one
			memcpy( next.mElems + mCount * TAB, &val, size );

			next.mCount = newCount;

			Swap( next );
		}
		else
		{
			memcpy( mElems + mCount * TAB, &val, size );
			mCount ++;
		}
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	template <typename U>
	void
	TTabArray<T,TAB>::PushBack( const U& val )
	{
		COMPILE_ASSERT( sizeof val <= TAB );
		const T* checkDerivancy = &val; (void)checkDerivancy;

		// still require POD
		union
		{
			char	data[ TAB ];
			U		need_POD;
		} unused; (void) unused;

		PushBack( val, sizeof val );
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T, uint32_t TAB >
	void TTabArray<T,TAB>::PopBack()
	{
		if ( mCount > 0 )
			Resize( mCount - 1 );
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T, uint32_t TAB >
	const T& TTabArray<T,TAB>::GetFirst() const
	{
		assert( mCount );
		return (const T&)mElems[ 0 ];		
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T, uint32_t TAB >
	const T& TTabArray<T,TAB>::GetLast() const
	{
		assert( mCount );
		return (const T&)mElems[ ( mCount - 1 ) * TAB ];
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T, uint32_t TAB >
	T& TTabArray<T,TAB>::GetFirst()
	{
		assert( mCount );
		return (T&)mElems[ 0 ];
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T, uint32_t TAB >
	T& TTabArray<T,TAB>::GetLast()
	{
		assert( mCount );
		return (T&)mElems[ ( mCount - 1 ) * TAB ];
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	void
	TTabArray<T,TAB>::Reserve( uint32_t count )
	{
		if( count > mSpace )
		{
			uint8_t* newElems = Allocate( count );

			mSpace = count;

			Construct( newElems, mElems, mCount );
			Destruct( mElems, mCount );
			Free( mElems );

			mElems = newElems;
		}
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	void
	TTabArray<T,TAB>::Erase( uint32_t idx )
	{
		Erase( idx, 1 );
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	void
	TTabArray<T,TAB>::Erase( uint32_t idx, uint32_t count )
	{
		assert( idx + count <= mCount );

		memcpy( mElems + idx * TAB, mElems + ( idx + count ) * TAB, ( mCount - idx - count ) * TAB );

		mCount -= count;
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	void
	TTabArray<T,TAB>::Clear()
	{
		Destruct( mElems, mCount );
		mCount = 0;
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	uint32_t
	TTabArray<T,TAB>::Count() const
	{
		return mCount;
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	/*static*/
	uint8_t*
	TTabArray<T,TAB>::Allocate( uint32_t count )
	{
		uint8_t* res( NULL );

		if( count )
		{
			res = (uint8_t*)malloc( TAB * count );
			assert( res );
		}

		return res;
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	/*static*/
	void
	TTabArray<T,TAB>::Free( uint8_t* ptr )
	{
		free( ptr );
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	/*static*/
	void
	TTabArray<T,TAB>::Destruct( uint8_t* ptr, uint32_t count )
	{
		// error if T is not POD?
		union
		{
			char	data[ sizeof( T ) ];
			T		want_pod;
		} unused; (void)unused;
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	/*static*/
	void
	TTabArray<T,TAB>::Construct( uint8_t* ptr, uint32_t count, const T& val, uint32_t tab )
	{
		for( uint32_t i = 0, e = count; i < e; i ++ )
		{
			memcpy( ptr + i * TAB, &val, tab );
		}
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t TAB >
	/*static*/
	void
	TTabArray<T,TAB>::Construct( uint8_t* dest, uint8_t* src, uint32_t count )
	{
		memcpy( dest, src, count * TAB );
	}

	template< typename T, uint32_t TAB >
	inline int TTabArray<T,TAB>::FindItemIndex( const T & item ) const
	{
		assert( /*( mCount >= 0 ) && ( mSpace >= 0 ) &&*/ ( mSpace >= mCount ) );
		int nIndex = mCount;
		while ( nIndex-- )
		{
			if ( (*this)[ nIndex ] == item )
				return nIndex;
		}
		return -1;
	}

}

#endif