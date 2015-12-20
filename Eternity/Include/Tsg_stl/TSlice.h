#ifndef	TL_TSLICE_H
#define	TL_TSLICE_H

namespace r3dTL
{
	template <typename T>
	class TSlice
	{
		// construction/ destruction
	public:
		TSlice( T* start, T* end );
		TSlice( T* start, uint32_t count );

		// manipulation / access
	public:
		T& operator [] ( uint32_t idx );
		const T& operator [] ( uint32_t idx ) const;

		uint32_t Count() const;

		// data
	private:
		T*			mElems;
		uint32_t	mCount;
	};

	//------------------------------------------------------------------------

	template <typename T>
	TSlice<T>::TSlice( T* start, T* end ):
	mElems( start ),
	mCount( end - start )
	{
		
	}

	//------------------------------------------------------------------------

	template <typename T>
	TSlice<T>::TSlice( T* start, uint32_t count ) :
	mElems( start ),
	mCount( count )
	{

	}

	//------------------------------------------------------------------------

	template <typename T>
	T&
	TSlice<T>::operator [] ( uint32_t idx )
	{
		r3d_assert( idx < mCount );
		return mElems[ idx ];
	}

	//------------------------------------------------------------------------

	template <typename T>
	const T&
	TSlice<T>::operator [] ( uint32_t idx ) const
	{
		r3d_assert( idx < mCount );
		return mElems[ idx ];
	}

	//------------------------------------------------------------------------

	template <typename T>
	uint32_t
	TSlice<T>::Count() const
	{
		return mCount;
	}
}
#endif

