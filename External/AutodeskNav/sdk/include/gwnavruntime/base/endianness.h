/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Endianness_H
#define Navigation_Endianness_H


#include "gwnavruntime/base/types.h"


namespace Kaim
{


/// Collects endianness types and utilities used in the blob serialization framework.
/// Endianness is a class with static functions only.
class Endianness 
{
public:
	/// Enumerates possible endianness types.  
	enum Type
	{
		LittleEndian = 0, ///< Little-endian format (used, for example, for Windows, Linux). 
		BigEndian    = 1, ///< Big-endian format (used, for example, for PlayStation 3, Xbox 360). 
	};

	/// Enumerates the possible endianness types relative to the current platform.  
	enum Target
	{
		SYSTEM_ENDIANNESS = 0, ///< The same endianness type as the current platform. 
		INVERSE_SYSTEM_ENDIANNESS = 1, ///< The opposite endianness type from the current platform. 
	};

	/// Retrieves the endianness of the current platform.
	static Type GetSystemEndianness()
	{
#if defined(KY_OS_XBOX360) || defined(KY_OS_PS3) || defined(KY_OS_WII) || defined(KY_OS_WIIU)
		return BigEndian;
#else
		return LittleEndian;
#endif
	}

	/// Retrieves the endianness opposite to that of the current platform.  
	static Type GetInverseSystemEndianness()
	{
		return GetInverseEndianness(GetSystemEndianness());
	}

	/// Retrieves the endianness opposite to the specified type.  
	static Type GetInverseEndianness(Type endianness)
	{
		return (endianness == BigEndian) ? LittleEndian : BigEndian;
	}

	/// Retrieves the endianness from a 4-byte memory location. 
	static Type GetEndiannessFromMem32(const KyUInt32* mem)
	{
		return (*mem == 0) ? LittleEndian : BigEndian;
	}

	/// Sets a 4-byte memory location to the specified endianness. 
	static void SetEndiannessToMem32(Type endianness, KyUInt32* mem)
	{
		*mem = 0;
		if (endianness == BigEndian)
			*((char*)mem) = 1; // set first byte to 1 regardless of the endianness
	}

	/// Swaps the endianness in a 4-byte memory location. 
	static void SwapEndiannessInMem32(KyUInt32* mem)
	{
		Type endianness = GetEndiannessFromMem32(mem);
		Type inverseEndianness = GetInverseEndianness(endianness);
		SetEndiannessToMem32(inverseEndianness, mem);
	}

	/// Swaps the endianness of the data in a 16-bit buffer. 
	static inline void Swap16(void* x)
	{
		(*(KyUInt16*)x) =
			( (*(KyUInt16*)x) >> 8 ) |
			( (*(KyUInt16*)x) << 8 );
	}

	/// Swaps the endianness of the data in a 32-bit buffer. 
	static inline void Swap32(void* x)
	{
		(*(KyUInt32*)x) =
			( ( (*(KyUInt32*)x) >> 24 )              ) |
			( ( (*(KyUInt32*)x) <<  8 ) & 0x00FF0000 ) |
			( ( (*(KyUInt32*)x) >>  8 ) & 0x0000FF00 ) |
			( ( (*(KyUInt32*)x) << 24 )              );
	}

	/// Swaps the endianness of the data in a 64-bit buffer. 
	static inline void Swap64(void* x)
	{
		(*(KyUInt64*)x) =
			( ( (*(KyUInt64*)x) << 56 )                         ) |
			( ( (*(KyUInt64*)x) << 40 ) & 0x00FF000000000000ULL ) |
			( ( (*(KyUInt64*)x) << 24 ) & 0x0000FF0000000000ULL ) |
			( ( (*(KyUInt64*)x) <<  8 ) & 0x000000FF00000000ULL ) |
			( ( (*(KyUInt64*)x) >>  8 ) & 0x00000000FF000000ULL ) |
			( ( (*(KyUInt64*)x) >> 24 ) & 0x0000000000FF0000ULL ) |
			( ( (*(KyUInt64*)x) >> 40 ) & 0x000000000000FF00ULL ) |
			( ( (*(KyUInt64*)x) >> 56 )                         );
	}
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyInt32&  x ) { Endianness::Swap32(&x); }

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyUInt32& x ) { Endianness::Swap32(&x); }

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyInt16&  x ) { Endianness::Swap16(&x); }

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyUInt16& x ) { Endianness::Swap16(&x); }

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyInt8&     ) {}

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyUInt8&    ) {}

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, char&       ) {}

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyFloat32& x) { Endianness::Swap32(&x); }

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyInt64&  x ) { Endianness::Swap64(&x); }

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target, KyUInt64& x ) { Endianness::Swap64(&x); }


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, Endianness::Type& endianess)
{
	switch (endianess)
	{
	case Endianness::LittleEndian: os << "LittleEndian"; break;
	case Endianness::BigEndian:    os << "BigEndian";    break;
	}
	return os;
}


}


#endif
