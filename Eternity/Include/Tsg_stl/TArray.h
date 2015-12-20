#ifndef	TL_TARRAY_H
#define	TL_TARRAY_H

#include "r3dSys_WIN.h"
#include "THelpers.h"

#include "r3dAssert.h"

// need to undef new, as here we are using placement new which isn't compatible with memory tracking :( but it still will track malloc :)
#ifdef _DEBUG
#ifdef ENABLE_MEMORY_DEBUG
#undef new
#endif //ENABLE_MEMORY_DEBUG
#endif

extern bool r3dOutToLog(const char* Str, ...);

namespace r3dTL
{

#define R3D_TARRAY_CHECKBOUNDS(idx_)								\
	if( (idx_) >= mCount )											\
	{																\
		void* EIPVal;												\
		{ __asm call $ + 5 __asm pop eax __asm mov EIPVal, eax }	\
		ReportOutOfBounds( idx_, __FUNCTION__, EIPVal );			\
	}

#define R3D_TARRAY_CHECKNONZERO()									\
	if( !mCount )													\
	{																\
		void* EIPVal;												\
		{ __asm call $ + 5 __asm pop eax __asm mov EIPVal, eax }	\
		ReportOutOfBounds( 0, __FUNCTION__, EIPVal );				\
	}

	template <typename T>
	class TArray
	{
		// construction/ destruction
	public:
		TArray();
		explicit TArray( uint32_t size, const T& val = T() );
		TArray( const TArray& arr );
		~TArray();

		// manipulation/ access
	public:
		TArray& operator = ( const TArray& arr );

		R3D_FORCEINLINE const T& operator[] ( uint32_t idx ) const;
		R3D_FORCEINLINE T& operator[] ( uint32_t idx );

		void Swap( TArray& arr );
		void Resize( uint32_t count, const T& val = T() );
	
		void		PushBack		( const T& val );
		void		PopBack			();

		const T&	GetFirst		() const;
		const T&	GetLast			() const;

		T&			GetFirst		();
		T&			GetLast			();

		void 		Reserve			( uint32_t count );
		void		AdjustReserve	( uint32_t count );
		void 		Erase			( uint32_t idx );
		void 		Erase			( uint32_t start_idx, uint32_t count );
		void 		Clear			();

		void		Insert			( uint32_t idx, const T& val ) ;

		R3D_FORCEINLINE
		uint32_t	Count() const;

		R3D_FORCEINLINE
		uint32_t	GetSpace() const;

		int			FindItemIndex	( const T & item ) const;

		// helpers
	private:
		static T* Allocate( uint32_t count );
		static void Free( T* ptr );
		static void Destruct( T* ptr, uint32_t count );
		static void Construct( T* ptr, uint32_t count, const T& val );
		static void Construct( T* dest, T* src, uint32_t count );

		R3D_NO_INLINE void ReportOutOfBounds( int idx, const char* func, void* eip  ) const;

		// data
	private:
		T*			mElems;
		uint32_t	mCount;
		uint32_t	mSpace;
	};

	//------------------------------------------------------------------------

	template< typename T>
	TArray<T>::TArray():
	mElems( NULL ),
	mCount( 0 ),
	mSpace( 0 )
	{

	}

	//------------------------------------------------------------------------
	/*explicit*/

	template< typename T>
	TArray<T>::TArray( uint32_t size, const T& val /*= T()*/ ) :
	mElems( Allocate( size ) ),
	mCount( size ),
	mSpace( size )
	{
		Construct( mElems, size, val );
	}

	//------------------------------------------------------------------------

	template< typename T>
	TArray<T>::TArray( const TArray& arr ) :
	mElems( Allocate( arr.mCount ) ),
	mCount( arr.mCount ),
	mSpace( arr.mCount )
	{
		Construct( mElems, arr.mElems, mCount );
	}

	//------------------------------------------------------------------------

	template< typename T>
	TArray<T>::~TArray()
	{
		Destruct( mElems, mCount );
		Free( mElems );
	}

	//------------------------------------------------------------------------

	template< typename T>
	R3D_FORCEINLINE
	TArray<T>&
	TArray<T>::operator = ( const TArray<T>& arr )
	{
		TArray<T> temp( arr );
		Swap( temp );
		return *this;
	}

	//------------------------------------------------------------------------

	template< typename T>
	R3D_FORCEINLINE
	const T&
	TArray<T>::operator[] ( uint32_t idx ) const
	{
		R3D_TARRAY_CHECKBOUNDS(idx);
		return mElems[ idx ];
	}

	//------------------------------------------------------------------------

	template< typename T>
	T&
	TArray<T>::operator[] ( uint32_t idx )
	{
		R3D_TARRAY_CHECKBOUNDS(idx);
		return mElems[ idx ];
	}

	//------------------------------------------------------------------------

	template< typename T>
	void
	TArray<T>::Swap( TArray& arr )
	{
		r3dTL::Swap( mElems, arr.mElems );
		r3dTL::Swap( mCount, arr.mCount );
		r3dTL::Swap( mSpace, arr.mSpace );
	}

	//------------------------------------------------------------------------

	template< typename T>
	void
	TArray<T>::Resize( uint32_t count, const T& val /*= T()*/ )
	{
		if( count > mCount )
		{
			Reserve( count );
			Construct( mElems + mCount, count - mCount, val );
		}
		else
		{
			Destruct( mElems + count, mCount - count );
		}

		mCount = count;
	}

	//------------------------------------------------------------------------

	template< typename T>
	void
	TArray<T>::PushBack( const T& val )
	{
		if( mCount >= mSpace )
		{
			TArray next;

			uint32_t newCount = mCount + 1;

			uint32_t newSpace = 0x80000000;

			while( !(newCount & newSpace) )
				newSpace >>= 1;

			newSpace <<= 1;

			next.Reserve( newSpace );

			// old ones
			Construct( next.mElems, mElems, mCount );

			// new one
			new ( next.mElems + mCount ) T ( val );

			next.mCount = newCount;

			Swap( next );
		}
		else
		{
			new ( mElems + mCount ++ ) T ( val );
		}
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T>
	void TArray<T>::PopBack()
	{
		if ( mCount > 0 )
			Resize( mCount - 1 );
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T>
	const T& TArray<T>::GetFirst() const
	{
		R3D_TARRAY_CHECKNONZERO();
		return mElems[ 0 ];		
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T>
	const T& TArray<T>::GetLast() const
	{
		R3D_TARRAY_CHECKNONZERO();
		return mElems[ mCount - 1 ];
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T>
	T& TArray<T>::GetFirst()
	{
		R3D_TARRAY_CHECKNONZERO();
		return mElems[ 0 ];
	}

	//--------------------------------------------------------------------------------------------------------
	template< typename T>
	T& TArray<T>::GetLast()
	{
		R3D_TARRAY_CHECKNONZERO();
		return mElems[ mCount - 1 ];
	}

	//------------------------------------------------------------------------

	template< typename T>
	void
	TArray<T>::Reserve( uint32_t count )
	{
		if( count > mSpace )
		{
			T* newElems = Allocate( count );

			mSpace = count;

			Construct( newElems, mElems, mCount );
			Destruct( mElems, mCount );
			Free( mElems );

			mElems = newElems;
		}
	}

	template< typename T>
	void
	TArray<T>::AdjustReserve( uint32_t count )
	{
		if( count < mCount )
			count = mCount;

		T* newElems = Allocate( count );

		mSpace = count;

		Construct( newElems, mElems, mCount );
		Destruct( mElems, mCount );
		Free( mElems );

		mElems = newElems;
	}

	//------------------------------------------------------------------------

	template< typename T>
	void
	TArray<T>::Erase( uint32_t idx )
	{
		R3D_TARRAY_CHECKBOUNDS(idx);

		for( uint32_t i = idx, e = mCount - 1; i < e; i ++ )
		{
			mElems[ i ].~T();
			new ( mElems + i ) T( mElems[ i + 1 ] );
		}

		// delete the last one
		mElems[ --mCount ].~T();
	}

	//------------------------------------------------------------------------

	template< typename T>
	void
	TArray<T>::Erase( uint32_t idx, uint32_t count )
	{
		r3d_assert( idx + count <= mCount );

		for( uint32_t i = idx, e = mCount - count; i < e; i ++ )
		{
			mElems[ i ].~T();
			new ( mElems + i ) T( mElems[ i + count ] );
		}

		// delete remaining
		for( uint32_t i = mCount - count, e = mCount; i < e; i ++ )
		{
			mElems[ i ].~T();
		}

		mCount -= count;
	}

	//------------------------------------------------------------------------

	template< typename T>
	void
	TArray<T>::Clear()
	{
		Destruct( mElems, mCount );
		mCount = 0;
	}

	//------------------------------------------------------------------------


	template< typename T>
	void
	TArray<T>::Insert( uint32_t idx, const T& val )
	{
		r3d_assert( idx <= mCount );

		Reserve( mCount +  1 ) ;

		for( int i = (int)mCount - 1 ; i >= (int)idx ; i -- )
		{
			new ( mElems + i + 1 ) T( mElems[ i ] );
			mElems[ i ].~T();
		}

		new ( mElems + idx ) T( val ) ;

		mCount ++ ;
	}

	//------------------------------------------------------------------------

	template< typename T>
	R3D_FORCEINLINE
	uint32_t
	TArray<T>::Count() const
	{
		return mCount;
	}

	//------------------------------------------------------------------------

	template< typename T>
	R3D_FORCEINLINE
	uint32_t
	TArray<T>::GetSpace() const
	{
		return mSpace;
	}

	//------------------------------------------------------------------------

	template< typename T>
	/*static*/
	T*
	TArray<T>::Allocate( uint32_t count )
	{
		T* res( NULL );

		if( count )
		{
#ifdef USE_R3D_MEMORY_ALLOCATOR
			res = (T*)r3dAllocateMemory( sizeof (T) * count, 1, ALLOC_TYPE_GAME );
#else
			res = (T*)malloc( sizeof (T) * count );
#endif
			r3d_assert( res );
		}

		return res;
	}

	//------------------------------------------------------------------------

	template< typename T>
	/*static*/
	void
	TArray<T>::Free( T* ptr )
	{
#ifdef USE_R3D_MEMORY_ALLOCATOR
		r3dDeallocateMemory( ptr, 1, ALLOC_TYPE_GAME );
#else
		free( ptr );
#endif
	}

	//------------------------------------------------------------------------

	template< typename T>
	/*static*/
	void
	TArray<T>::Destruct( T* ptr, uint32_t count )
	{
		for( uint32_t i = 0, e = count; i < e; i ++ )
		{
			ptr[ i ].~T();
		}
	}

	//------------------------------------------------------------------------

	template< typename T>
	/*static*/
	void
	TArray<T>::Construct( T* ptr, uint32_t count, const T& val )
	{
		for( uint32_t i = 0, e = count; i < e; i ++ )
		{
			new ( ptr + i ) T( val );
		}
	}

	//------------------------------------------------------------------------

	template< typename T>
	/*static*/
	void
	TArray<T>::Construct( T* dest, T* src, uint32_t count )
	{
		for( uint32_t i = 0, e = count; i < e; i ++ )
		{
			new ( dest + i ) T( src[ i ] );
		}
	}

	template< typename T>
	inline int TArray<T>::FindItemIndex( const T & item ) const
	{
		r3d_assert( /*( mCount >= 0 ) && ( mSpace >= 0 ) &&*/ ( mSpace >= mCount ) );
		int nIndex = mCount;
		while ( nIndex-- )
		{
			if ( mElems[ nIndex ] == item )
				return nIndex;
		}
		return -1;
	}

	template< typename T>
	R3D_NO_INLINE
	void TArray<T>::ReportOutOfBounds( int idx, const char* func, void* eip ) const
	{
		void* r3dGetMainModuleBaseAddress();
		void* base = r3dGetMainModuleBaseAddress();

		r3dOutToLog( "ERROR: Bounds check failed (%d of %d) in %s (EIP=0x%X, BASE=0x%X)\n", idx, mCount, func, eip, base );
#ifndef FINAL_BUILD
		r3d_actual_assert( "idx < mCount", __FILE__, __LINE__, 0 );
#endif
	}

}

#endif