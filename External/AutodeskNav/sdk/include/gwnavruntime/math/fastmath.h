/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



#ifndef Navigation_FastMath_H
#define Navigation_FastMath_H

#include "gwnavruntime/base/types.h"


#if defined(KY_OS_XBOX360)
#include <ppcintrinsics.h>
#endif


#if defined(KY_OS_PS3)
#include <ppu_intrinsics.h>
#endif

#include <math.h>


namespace Kaim
{


//----------------------------------------------------------------------------------------
// Fsel

//-------------------------------------
#if defined(KY_OS_XBOX360)
KY_INLINE KyFloat32 Fsel(KyFloat32 cmp, KyFloat32 v1, KyFloat32 v2)
{
	return (KyFloat32)__fsel(cmp, v1, v2);
}

//-------------------------------------
#elif defined(KY_OS_PS3)
KY_INLINE KyFloat32 Fsel(KyFloat32 cmp, KyFloat32 v1, KyFloat32 v2)
{
	return __fsels(cmp, v1, v2);
}

//-------------------------------------
#elif defined(KY_OS_WII)
inline float Fsel(KyFloat32 cmp, KyFloat32 v1, KyFloat32 v2)
{
	return (KyFloat32)__fsel(cmp, v1, v2);
}

//-------------------------------------
#else
/// Ifcmp is greater than 0, returnsv1. Otherwise, returnsv2.
KY_INLINE KyFloat32 Fsel(KyFloat32 cmp, KyFloat32 v1, KyFloat32 v2)
{
	return cmp >= 0.0f ? v1 : v2;
}

#endif


//----------------------------------------------------------------------------------------
// Isel Lsel

/// If a is greater than 0, returns x. Otherwise, returns y.
KY_INLINE KyInt32 Isel(KyInt32 a, KyInt32 x, KyInt32 y)
{
	// C++ arithmetic-shift operator >> with signed int preserve the sing
	// thus, this first bit (0 if positive, 1 if negative) is duplicated
	// signMask becomes 0xFFFFFFFF if (a < 0) and 0x00000000 if (a >= 0)
	const KyInt32 signMask = a >> 31;
	return x + ((y - x) & signMask);
};

/// If a is greater than 0, returns x. Otherwise, returns y.
KY_INLINE KyInt64 Lsel(KyInt64 a, KyInt64 x, KyInt64 y)
{
	// C++ arithmetic-shift operator >> with signed int preserve the sing
	// thus, this first bit (0 if positive, 1 if negative) is duplicated
	// signMask becomes 0xFFFFFFFF if (a < 0) and 0x00000000 if (a >= 0)
	const KyInt64 signMask = a >> 63;
	return x + ((y - x) & signMask);
};


//---------------------------------------------------------------------------------------
// Sel

template <class T>
T Sel(T cmp, T v1, T v2);

template <>
KY_INLINE KyInt64 Sel<KyInt64>(KyInt64 a, KyInt64 x, KyInt64 y) { return Lsel(a, x, y); }

template <>
KY_INLINE KyInt32 Sel<KyInt32>(KyInt32 a, KyInt32 x, KyInt32 y) { return Isel(a, x, y); }

template <>
KY_INLINE KyFloat32 Sel<KyFloat32>(KyFloat32 a, KyFloat32 x, KyFloat32 y) { return Fsel(a, x, y); }



//----------------------------------------------------------------------------------------
// Min / Max

/// Returns the lesser of the two specified values.
/// \tparam T		The type of values provided.
template<typename T>
KY_INLINE T Min(const T& a, const T& b)
{
	return (a < b) ? a : b;
}

/// Returns the greater of the two specified values.
/// \tparam T		The type of values provided.
template<typename T>
KY_INLINE T Max(const T& a, const T& b)
{
	return (a < b) ? b : a;
}

/// Returns the least of the three specified values.
/// \tparam T		The type of values provided.
template <typename T>
KY_INLINE T Min(const T& a, const T& b, const T& c)
{
	return Min(Min(a, b), c);
}

/// Returns the greatest of the three specified values.
/// \tparam T		The type of values provided.
template <typename T>
KY_INLINE T Max(const T& a, const T& b, const T& c)
{
	return Max(Max(a, b), c);
}

//---------------------------------------------------------------------
// Fsel specialization for floats
#if defined(KY_OS_XBOX360) || defined(KY_OS_PS3) || defined(KY_OS_WII)

template<>
KY_INLINE KyFloat32 Min(const KyFloat32& a, const KyFloat32& b)
{
	return Fsel(a - b, b, a);
}

template<>
KY_INLINE KyFloat32 Max(const KyFloat32& a, const KyFloat32& b)
{
	return Fsel(a - b, a, b);
}

#endif
//---------------------------------------------------------------------

// WARNING : Can overflow
KY_INLINE KyInt32 FastMin(const KyInt32& a, const KyInt32& b)
{
	return Isel(a - b, b, a);
}

// WARNING : Can overflow
KY_INLINE KyInt32 FastMax(const KyInt32& a, const KyInt32& b)
{
	return Isel(a - b, a, b);
}

// WARNING : Can overflow
KY_INLINE KyInt64 FastMin(const KyInt64& a, const KyInt64& b)
{
	return Lsel(a - b, b, a);
}

// WARNING : Can overflow
KY_INLINE KyInt64 FastMax(const KyInt64& a, const KyInt64& b)
{
	return Lsel(a - b, a, b);
}

// WARNING : Can overflow
KY_INLINE KyUInt32 FastDivisionBy3(KyUInt32 i)
{
	return (i * 0x0000AAAB) >> 17;
}

// WARNING : Can overflow
KY_INLINE KyUInt32 FastModuloBy3(KyUInt32 i)
{
	return i - 3 * FastDivisionBy3(i);
}

// remove this function and replace it with template<class T> struct OperatorLess
template <typename T1, typename T2 = T1>
class DefaultLess
{
public:
	KY_INLINE bool operator()(const T1& lhs, const T2& rhs) const { return lhs < rhs; }
};



//----------------------------------------------------------------------------------------
// Float32 to signed int32

KY_INLINE KyInt32 NearestInt(KyFloat32 x)
{
	return (KyInt32)(x + Kaim::Fsel(x, 0.5f, -0.5f));
}

// FastLowerInt( 1.0f) =  1
// FastLowerInt(-1.0f) = -1
KY_INLINE KyInt32 LowerInt(KyFloat32 x)
{
	return (KyInt32)(floorf(x));
}

// FastLowerInt( 1.0f) =  1
// FastLowerInt(-1.0f) = -2
KY_INLINE KyInt32 FastLowerInt(KyFloat32 x)
{
	return (KyInt32)(x + Kaim::Fsel(x, 0.0f, -1.0f));
}

KY_INLINE KyFloat32 Acosf(KyFloat32 input)
{
	// Cap the input to [-1.0, 1.0] to be robust to float precision errors and to avoid returning a NaN
	const KyFloat32 normalizeDotProd = Kaim::Min(1.f, Kaim::Max(-1.f, input));
	return acosf(normalizeDotProd);
}

KY_INLINE KyFloat32 Asinf(KyFloat32 input)
{
	// Cap the input to [-1.0, 1.0] to be robust to float precision errors and to avoid returning a NaN
	const KyFloat32 normalizeDotProd = Kaim::Min(1.f, Kaim::Max(-1.f, input));
	return asinf(normalizeDotProd);
}

}


#endif
