#ifndef	__R3DSYS_WIN_H
#define	__R3DSYS_WIN_H

#include "r3dTypedefs.h"

#define extern_nspace(nspace, var)  namespace nspace { extern var; };

#define INVALID_INDEX			(-1)

//
// Miscellaneous helper functions
//
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define PURE_DELETE(p)       { delete (p); }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define R3D_NOCLASSCOPY(xx)		\
	xx(const xx &);			\
	xx &operator= (const xx &);


//
// various templates
//
template<class T>
__forceinline  int R3D_SIGN(const T a) 	{ return (a < 0) ? -1 : (a > 0) ? 1 : 0; }

template<class T>
__forceinline void R3D_SWAP(T& one, T& two) { T temp; temp = one; one = two; two = temp; }

template<class T>
__forceinline T R3D_MAX(const T a, const T b) { return a > b ? a : b; }

template<class T>
__forceinline T R3D_MIN(const T a, const T b)	{ return a < b ? a : b; }

template<class T>
__forceinline T R3D_ABS(const T a) 		{ return a >= 0 ? a : -a; }

template<class T>
__forceinline T R3D_CLAMP(const T val, const T min, const T max) 
{
  return ((val < min) ? min : (val > max) ? max : val);
}
template <class T> 
__forceinline T R3D_LERP( T from, T to, float weight )
{
	return T( from + weight * ( to - from ) );
}
//
// linked list stuff
//
template <class T>
int LList_Create(T **base)
{
  *base = NULL;
  return 1;
}

template <class T>
int LList_Insert(T **base, T *what)
{
	T	*tmp;

  // check if it's already here..
  for(tmp=*base; tmp; tmp=tmp->pNext)
    if(tmp == what)
      return 0;

  what->pNext = *base;
  *base      = what;
  return 1;
};

template <class T>
int LList_InsertLast(T **base, T *what)
{
	T	*tmp;
  // traverse to last element and add there..
  for(tmp=*base; tmp && tmp->pNext; tmp=tmp->pNext)
    if(tmp == what)
      return 0;

  if(*base) tmp->pNext  = what;
  else      *base      = what;
  return 1;
};

template <class T>
int LList_Remove(T **base, T *what)
{
	T	*tmp;

  if(!*base)
    return 0;

  if(*base == what) {
    *base = what->pNext;
    return 1;
  }
  // scan thru list and see if that object can be removed
  for(tmp=*base; tmp && tmp->pNext; tmp=tmp->pNext) {
    if(tmp->pNext == what) {
      tmp->pNext = what->pNext;
      return 1;
    }
  }

  return 0;
}

template <class T>
int LList_Destroy(T **base)
{
	T	*tmp, *tmp2;
  for(tmp = *base; tmp; tmp=tmp2) {
    tmp2 = tmp->pNext;
    delete tmp;
  }
  *base = NULL;
  return 1;
}


//----------------------------------------------------------------------------
//	General math functions
//----------------------------------------------------------------------------
inline FLOAT r3dExp( FLOAT Value ) { return expf(Value); }
inline FLOAT r3dLoge( FLOAT Value ) {	return logf(Value); }
inline FLOAT r3dFmod( FLOAT Y, FLOAT X ) { return fmodf(Y,X); }
inline FLOAT r3dSin( FLOAT Value ) { return sinf(Value); }
inline FLOAT r3dAsin( FLOAT Value ) { return asinf(Value); }
inline FLOAT r3dCos( FLOAT Value ) { return cosf(Value); }
inline FLOAT r3dAcos( FLOAT Value ) { return acosf(Value); }
inline FLOAT r3dTan( FLOAT Value ) { return tanf(Value); }
inline FLOAT r3dAtan( FLOAT Value ) { return atanf(Value); }
inline FLOAT r3dAtan2( FLOAT Y, FLOAT X ) { return atan2f(Y,X); }
inline FLOAT r3dPow( FLOAT A, FLOAT B ) { return powf(A,B); }




inline int 	r3dFloatToInt_2(float _fvar)
{
  _fvar += (1l<<23l);
  return *((int *)&_fvar) & 0x7fffffl;
}

__forceinline int 	r3dFloatToInt(float _fvar)
{
  int r3d__IConvTemp;
  _asm 
  {
    fld		dword ptr _fvar;
    fistp	dword ptr r3d__IConvTemp;
  }
  return r3d__IConvTemp;
}


/*
inline INT r3dFloatToInt( FLOAT F )
{
	__asm cvtss2si eax,[F]
	// return value in eax.
}
*/

inline INT r3dFloor( FLOAT F )
{
	const DWORD mxcsr_floor = 0x00003f80;
	const DWORD mxcsr_default = 0x00001f80;

	__asm ldmxcsr [mxcsr_floor]		// Round toward -infinity.
	__asm cvtss2si eax,[F]
	__asm ldmxcsr [mxcsr_default]	// Round to nearest
	// return value in eax.
}

//
// MSM: Fast float inverse square root using SSE.
// Accurate to within 1 LSB.
//
inline FLOAT r3dInvSqrt( FLOAT F )
{
	const FLOAT fThree = 3.0f;
	const FLOAT fOneHalf = 0.5f;
	FLOAT temp;

	__asm
	{
		movss	xmm1,[F]
		rsqrtss	xmm0,xmm1			// 1/sqrt estimate (12 bits)
		
		// Newton-Raphson iteration (X1 = 0.5*X0*(3-(Y*X0)*X0))
		movss	xmm3,[fThree]
		movss	xmm2,xmm0
		mulss	xmm0,xmm1			// Y*X0
		mulss	xmm0,xmm2			// Y*X0*X0
		mulss	xmm2,[fOneHalf]		// 0.5*X0
		subss	xmm3,xmm0			// 3-Y*X0*X0
		mulss	xmm3,xmm2			// 0.5*X0*(3-Y*X0*X0)
		movss	[temp],xmm3
	}

	return temp;
}

//
// MSM: Fast float square root using SSE.
// Accurate to within 1 LSB.
//
inline FLOAT r3dSqrt( FLOAT F )
{
	const FLOAT fZero = 0.0f;
	const FLOAT fThree = 3.0f;
	const FLOAT fOneHalf = 0.5f;
	FLOAT temp;

	__asm
	{
		movss	xmm1,[F]
		rsqrtss xmm0,xmm1			// 1/sqrt estimate (12 bits)
		
		// Newton-Raphson iteration (X1 = 0.5*X0*(3-(Y*X0)*X0))
		movss	xmm3,[fThree]
		movss	xmm2,xmm0
		mulss	xmm0,xmm1			// Y*X0
		mulss	xmm0,xmm2			// Y*X0*X0
		mulss	xmm2,[fOneHalf]		// 0.5*X0
		subss	xmm3,xmm0			// 3-Y*X0*X0
		mulss	xmm3,xmm2			// 0.5*X0*(3-Y*X0*X0)

		movss	xmm4,[fZero]
		cmpss	xmm4,xmm1,4			// not equal

		mulss	xmm3,xmm1			// sqrt(f) = f * 1/sqrt(f)

		andps	xmm3,xmm4			// seet result to zero if input is zero

		movss	[temp],xmm3
	}

	return temp;
}


//----------------------------------------------------------------------------
//	Time functions.
//----------------------------------------------------------------------------


	// our random generator functions
	unsigned long	u_random(unsigned long seed);
	void	      	u_srand(unsigned long seed);
	void		u_thread_rand_init();
	void		u_thread_rand_close();
	float 		u_GetRandom();
	float 		u_GetRandom(float r1, float r2);
	// helper for random numbers initialization in thread
	struct r3dRandInitInTread {
		r3dRandInitInTread() {
			u_thread_rand_init();
			u_srand(GetTickCount());
		}
		~r3dRandInitInTread() {
			u_thread_rand_close();
		}
	};

//----------------------------------------------------------------------------
//	Memory functions.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//	Callbacks
//----------------------------------------------------------------------------

extern void (*OnDblClick)();
extern void (*OnDrawClipboardCallback)(WPARAM wParam, LPARAM lParam);

#endif	//__R3DSYS_WIN_H
