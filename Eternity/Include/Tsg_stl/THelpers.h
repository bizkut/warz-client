#ifndef	TL_THELPERS_H
#define	TL_THELPERS_H

namespace r3dTL
{
	template <typename T>
	void Swap( T& a, T& b )
	{
		T tmp( a );
		a = b;
		b = tmp;
	}

	//------------------------------------------------------------------------

	template <typename T>
	const T& Min( const T& a, const T& b)
	{
		return a > b ? b : a;
	}

	//------------------------------------------------------------------------

	template <typename T>
	const T& Max( const T& a, const T& b)
	{
		return a < b ? b : a;
	}

	//--------------------------------------------------------------------------------------------------------
	template <typename T, typename U, typename V> 
	T Clamp (const T & v, const U & min, const V & max)
	{
		return (v < T (min)) ? T (min) : ((v > T (max)) ? T (max) : v);
	}

	template <typename T> class IsPOD
	{
	public:

		typedef char Yes;
		typedef struct {char a[2];} No;

		template <typename C> static Yes test(int)
		{
			union {T validPodType;} u;
		}
		template <typename C> static No test(...)
		{
		}
		enum {Value = (sizeof(test<T>(0)) == sizeof(Yes))};
	};
}


#endif