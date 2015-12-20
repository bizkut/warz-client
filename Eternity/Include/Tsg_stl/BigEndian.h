#ifndef TL_BIGENDIAN_H_INCLUDED
#define TL_BIGENDIAN_H_INCLUDED

#include "r3dPoint.h"
#include "r3dTypeTuples.h"

namespace r3dTL
{


	//------------------------------------------------------------------------

	template< typename T >
	size_t fwrite_be( const T& val, FILE* fout )
	{
		const size_t size = sizeof val;

		char rbuf[ size ];

		memcpy( rbuf, &val, size );

		for( size_t i = 0, e = size / 2; i < e; i ++ )
		{
			std::swap( rbuf[ i ], rbuf[ size - i - 1 ] );
		}

		return fwrite( rbuf, size, 1, fout );
	}

	template< typename T >
	size_t fwrite_be( const T* arr, size_t count, FILE* fout )
	{
		size_t written = 0;

		for( size_t i = 0, e = count; i < e; i ++ )
		{
			written += fwrite_be( arr[ i ], fout );
		}

		return written;
	}

	template< typename T >
	size_t fwrite_be( const r3dgameVector(T)& cont, FILE* fout )
	{
		size_t res = 0;
		for( size_t i = 0, e = cont.size(); i < e; i ++ )
		{
			res += fwrite_be( cont[ i ], fout );
		}

		return res;
	}

	template< typename T >
	size_t fwrite_be( const r3dTL::TArray<T>& cont, FILE* fout )
	{
		size_t res = 0;
		for( r3dTL::uint32_t i = 0, e = cont.Count(); i < e; i ++ )
		{
			res += fwrite_be( cont[ i ], fout );
		}

		return res;
	}

	template< typename T >
	size_t fread_be( T& val, r3dFile* fin )
	{
		const size_t size = sizeof val;

		size_t result = fread( &val, sizeof val, 1, fin );

		char* rbuf = (char*)&val;

		if( result )
		{
			for( size_t i = 0, e = size / 2; i < e; i ++ )
			{
				std::swap( rbuf[ i ], rbuf[ size - i - 1 ] );
			}				
		}

		return result;
	}

	template< typename T >
	size_t fread_be( r3dgameVector(T)& cont, size_t count, r3dFile* fin )
	{
		cont.resize( count );

		size_t res = 0;
		for( size_t i = 0, e = cont.size(); i < e; i ++ )
		{
			res += fread_be( cont[ i ], fin );
		}

		return res;
	}

	//------------------------------------------------------------------------

	inline size_t fwrite_be( const r3dPoint3D& val, FILE* fout )
	{
		return 
			fwrite_be( val.x, fout ) == 1 &&
			fwrite_be( val.y, fout ) == 1 && 
			fwrite_be( val.z, fout ) == 1;
	}

	inline size_t fwrite_be( const r3dPoint2D& val, FILE* fout )
	{
		return 
			fwrite_be( val.x, fout ) == 1 &&
			fwrite_be( val.y, fout ) == 1;
	}

	inline void to_be( short& ioS )
	{
		ioS = (ioS >> 8) + (ioS << 8);
	}

	inline void to_be( short2& ioS )
	{
		to_be( ioS.x );
		to_be( ioS.y );
	}

	inline void to_be( short4& ioS )
	{
		to_be( ioS.x );
		to_be( ioS.y );
		to_be( ioS.z );
		to_be( ioS.w );
	}

	inline size_t fread_be( r3dPoint3D& val, r3dFile* fin )
	{
		return 
			fread_be( val.x, fin ) == 1 &&
			fread_be( val.y, fin ) == 1 && 
			fread_be( val.z, fin ) == 1;
	}


}


#endif