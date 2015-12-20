#ifndef	TL_TFIXEDARRAY_H
#define	TL_TFIXEDARRAY_H

namespace r3dTL
{
#define R3D_TFIXEDARRAY_CHECKBOUNDS( idx_ )							\
	if( (idx_) >= N )												\
	{																\
		void* EIPVal;												\
		{ __asm call $ + 5 __asm pop eax __asm mov EIPVal, eax }	\
		ReportOutOfBounds( idx_, __FUNCTION__, EIPVal );			\
	}


	template < typename T, uint32_t N>
	class TFixedArray
	{
	public:
		static const uint32_t COUNT = N;

	public:
		TFixedArray();

	public:
		T& operator [] ( uint32_t idx );
		const T& operator [] ( uint32_t idx ) const;

		T* operator + ( uint32_t idx );
		const T* operator + ( uint32_t idx ) const;

		void Move( uint32_t targIdx, uint32_t srcIdx, uint32_t count );

		int Count() const;

	private:
		void ReportOutOfBounds( int idx, const char* func, void* eip ) const;

	private:
		T			mElems[ N ];
	};

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	TFixedArray< T, N > :: TFixedArray()
	{
		for( uint32_t i = 0, e = N; i < e; i ++ )
		{
			mElems[ i ] = T();
		}
	}

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	T&
	TFixedArray< T, N > :: operator [] ( uint32_t idx )
	{
		R3D_TFIXEDARRAY_CHECKBOUNDS(idx);
		return mElems[ idx ];
	}

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	const T&
	TFixedArray< T, N > :: operator [] ( uint32_t idx ) const
	{
		R3D_TFIXEDARRAY_CHECKBOUNDS(idx);
		return mElems[ idx ];
	}

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	T* 
	TFixedArray< T, N > :: operator + ( uint32_t idx )
	{
		R3D_TFIXEDARRAY_CHECKBOUNDS(idx);
		return mElems + idx ;
	}

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	const T*
	TFixedArray< T, N > :: operator + ( uint32_t idx ) const
	{
		R3D_TFIXEDARRAY_CHECKBOUNDS(idx);
		return mElems + idx ;
	}

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	void
	TFixedArray< T, N > :: Move( uint32_t targIdx, uint32_t srcIdx, uint32_t count )
	{
		for( uint32_t i = 0, e = count, t = targIdx, s = srcIdx ; i < e ; i ++, t ++, s ++ )
		{
			mElems[ t ] = mElems[ s ] ;
		}
	}

	//------------------------------------------------------------------------

	template < typename T, uint32_t N >
	int TFixedArray< T, N > :: Count() const
	{
		return COUNT;
	}

	//------------------------------------------------------------------------

	template< typename T, uint32_t N >
	R3D_NO_INLINE
	void TFixedArray< T, N > :: ReportOutOfBounds( int idx, const char* func, void* eip ) const
	{
		void* r3dGetMainModuleBaseAddress();
		void* base = r3dGetMainModuleBaseAddress();

		r3dOutToLog( "ERROR: Bounds check failed (%d of %d) in %s (EIP=0x%X, BASE=0x%X)\n", idx, N, func, eip, base );
#ifndef FINAL_BUILD
		r3d_actual_assert( "idx < N", __FILE__, __LINE__, 0 );
#endif
	}


}

#endif