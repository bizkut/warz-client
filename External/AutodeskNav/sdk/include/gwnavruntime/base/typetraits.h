/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_TypeTraits_H
#define Navigation_TypeTraits_H

#include "gwnavruntime/base/types.h"
#include <string.h> // memcpy

#if defined (KY_OS_WIIU)
#pragma ghs nowarning 236
#endif

namespace Kaim
{

// IsIntegral

template <typename T>
struct IsIntegral
{
	enum { value = false };
};

template <>
struct IsIntegral<bool>
{
	enum { value = true };
};

template <>
struct IsIntegral<unsigned char>
{
	enum { value = true };
};

template <>
struct IsIntegral<signed char>
{
	enum { value = true };
};

template <>
struct IsIntegral<char>
{
	enum { value = true };
};

template <>
struct IsIntegral<KyUInt16>
{
	enum { value = true };
};

template <>
struct IsIntegral<KyInt16>
{
	enum { value = true };
};

template <>
struct IsIntegral<KyUInt32>
{
	enum { value = true };
};

template <>
struct IsIntegral<KyInt32>
{
	enum { value = true };
};

template <>
struct IsIntegral<KyUInt64>
{
	enum { value = true };
};

template <>
struct IsIntegral<KyInt64>
{
	enum { value = true };
};

// IsFloat

template <typename T>
struct IsFloat
{
	enum { value = false };
};

template <>
struct IsFloat<KyFloat32>
{
	enum { value = true };
};

template <>
struct IsFloat<KyFloat64>
{
	enum { value = true };
};

// IsVoid

template <typename T>
struct IsVoid
{
	enum { value = false };
};

template <>
struct IsVoid<void>
{
	enum { value = true };
};

//IsPointer

template <typename T>
struct IsPointer
{
	enum { value = false };
};

template <typename T>
struct IsPointer<T*>
{
	enum { value = true };
};

//IsArithmetic

template <typename T>
struct IsArithmetic
{
	enum { value = IsIntegral<T>::value || IsFloat<T>::value };
};

// IsFundamental

template <typename T>
struct IsFundamental
{
	enum { value = IsArithmetic<T>::value || IsVoid<T>::value };
};


template <typename T>
struct HasTrivialConstructor
{
	enum { value = IsArithmetic<T>::value || IsPointer<T>::value };
};

template <typename T>
struct HasTrivialDestructor
{
	enum { value = IsArithmetic<T>::value || IsPointer<T>::value };
};

// Optimized constructor && destructor for fundamental types

namespace Internal
{
	template <bool hasTrivialConstructor>
	struct DefaultConstructorImpl;

	template <>
	struct DefaultConstructorImpl</*hasTrivialConstructor*/false>
	{
		template <typename T>
		void PlacementNew(T* ptr) { new (ptr) T; }

		template <typename T>
		void PlacementNewRange(T* begin, T* end)
		{
			for (T* it = begin; it != end; ++it)
			{
				new (it) T;
			}
		}

		template <typename Iterator, typename T>
		void ConstructRange(Iterator begin, Iterator end, T* buffer)
		{
			for (Iterator it = begin; it != end; ++it)
			{
				new (buffer) T(*it);
				++buffer;
			}
		}

	};

	template <>
	struct DefaultConstructorImpl</*hasTrivialConstructor*/true>
	{
		template <typename T>
		void PlacementNew(T* /*ptr*/) {} // no call for fundamental types
		template <typename T>
		void PlacementNewRange(T* /*begin*/, T* /*end*/) {} // no call for fundamental types

		template <typename T>
		void ConstructRange(const T* begin, const T* end, T* buffer)
		{
			// NoOverlap
			memcpy(buffer, begin, (end - begin) * sizeof (T));
		}

	};

	// Destructor

	template <bool hasTrivialDestructor>
	struct DestructorImpl;

	template <>
	struct DestructorImpl</*hasTrivialDestructor*/false>
	{
		template <typename T>
		void Destroy(T* ptr)
		{
			KY_UNUSED(ptr); // to avoid warning with classes with trivial destructors
			ptr->~T();
		}
		template <typename T>
		void DestroyRange(T* begin, T* end)
		{
			for (T* it = begin; it != end; ++it)
			{
				(*it).~T();
			}
		}
	};

	template <>
	struct DestructorImpl</*hasTrivialDestructor*/true>
	{
		template <typename T>
		void Destroy(T* /*ptr*/) {} /* no call for fundamental types */
		template <typename T>
		void DestroyRange(T* /*begin*/, T* /*end*/) {}
	};

}

template <typename T>
inline void CallDefaultConstructor(T* ptr)
{
	Internal::DefaultConstructorImpl<HasTrivialConstructor<T>::value>().PlacementNew(ptr);
}

template <typename T>
inline void CallDefaultConstructorRange(T* begin, T* end)
{
	Internal::DefaultConstructorImpl<HasTrivialConstructor<T>::value>().PlacementNewRange(begin, end);
}

template <typename Iterator, typename T>
inline void ConstructRange(Iterator begin, Iterator end, T* buffer)
{
	Internal::DefaultConstructorImpl<IsPointer<Iterator>::value && HasTrivialConstructor<T>::value>().ConstructRange(begin, end, buffer);
}

template <typename T>
inline void CallDestructor(T* ptr)
{
	Internal::DestructorImpl<HasTrivialDestructor<T>::value>().Destroy(ptr);
}

template <typename T>
inline void DestroyRange(T* begin, T* end)
{
	Internal::DestructorImpl<HasTrivialDestructor<T>::value>().DestroyRange(begin, end);
}

} // namespace Kaim

#endif // Navigation_TypeTraits_H

