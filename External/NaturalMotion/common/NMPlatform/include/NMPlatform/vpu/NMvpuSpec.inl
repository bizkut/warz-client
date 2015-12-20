// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------

// cache
NM_FORCEINLINE void prefetchCacheLine(const void* _addr_, int _offset_);

// conversions
NM_FORCEINLINE vector4_t c2int4f(const vector4_t vec);
NM_FORCEINLINE vector4_t c2float4i(const vector4_t vec);

// integer arithmetic, initialization, comparison
NM_FORCEINLINE vector4_t add4i(const vector4_t val1, const vector4_t val2);
NM_FORCEINLINE vector4_t sub4i(const vector4_t val1, const vector4_t val2);

NM_FORCEINLINE vector4_t set4i(const int u);
NM_FORCEINLINE vector4_t set4i(const int x, const int y, const int z, const int w);

NM_FORCEINLINE vector4_t sel4cmpiLT(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse);

// logical
NM_FORCEINLINE vector4_t and4(const vector4_t vecA, const vector4_t vecB);
NM_FORCEINLINE vector4_t or4(const vector4_t vecA, const vector4_t vecB);
NM_FORCEINLINE vector4_t xor4(const vector4_t vecA, const vector4_t vecB);
NM_FORCEINLINE vector4_t not4(const vector4_t vec);

// Shift
NM_FORCEINLINE vector4_t shr4im16(const vector4_t val);
NM_FORCEINLINE vector4_t shr4im8(const vector4_t val);
NM_FORCEINLINE vector4_t shr4(const vector4_t val, const vector4_t vshr);
NM_FORCEINLINE vector4_t shl4im16(const vector4_t val);
NM_FORCEINLINE vector4_t shl4im8(const vector4_t val);
NM_FORCEINLINE vector4_t shl4(const vector4_t val, const vector4_t vshl);

// floating point arithmetic
NM_FORCEINLINE vector4_t abs4f(const vector4_t val);
NM_FORCEINLINE vector4_t neg4f(const vector4_t val);
NM_FORCEINLINE vector4_t neg4f(const vector4_t val, const vector4_t mask);
NM_FORCEINLINE vector4_t add4f(const vector4_t val1, const vector4_t val2);
NM_FORCEINLINE vector4_t sub4f(const vector4_t val1, const vector4_t val2);
NM_FORCEINLINE vector4_t mul4f(const vector4_t val1, const vector4_t val2);
NM_FORCEINLINE vector4_t madd4f(const vector4_t val1, const vector4_t val2, const vector4_t vadd);

NM_FORCEINLINE vector4_t sqrt4f(const vector4_t val);
NM_FORCEINLINE vector4_t rcp4f(const vector4_t val);
NM_FORCEINLINE vector4_t rsqrt4f(const vector4_t val);

// for versatility, the user is responsible for abs(vec)
NM_FORCEINLINE vector4_t isBound4f(const vector4_t vec, const vector4_t tol);
NM_FORCEINLINE vector4_t isBound3f(const vector4_t vec, const vector4_t tol);

// mix
NM_FORCEINLINE vector4_t mixLoHi(const vector4_t vlo, const vector4_t vhi);

// rotate - shifts each element in the direction specified wrapping any elements that move off the end
NM_FORCEINLINE vector4_t rotL4(const vector4_t vec);
NM_FORCEINLINE vector4_t rotR4(const vector4_t vec);

// permute
NM_FORCEINLINE vector4_t swapLoHi(const vector4_t v);
NM_FORCEINLINE vector4_t reverse(const vector4_t v);
NM_FORCEINLINE vector4_t swapXY(const vector4_t v);
NM_FORCEINLINE vector4_t swapZW(const vector4_t v);
NM_FORCEINLINE vector4_t swapYZ(const vector4_t v);
NM_FORCEINLINE vector4_t swapXW(const vector4_t v);
NM_FORCEINLINE vector4_t swapYW(const vector4_t v);
NM_FORCEINLINE vector4_t swapXZ(const vector4_t v);

// broadcast
NM_FORCEINLINE vector4_t splatX(const vector4_t v);
NM_FORCEINLINE vector4_t splatY(const vector4_t v);
NM_FORCEINLINE vector4_t splatZ(const vector4_t v);
NM_FORCEINLINE vector4_t splatW(const vector4_t v);

// extract
NM_FORCEINLINE uint32_t elemX(const vector4_t v);
NM_FORCEINLINE uint32_t elemY(const vector4_t v);
NM_FORCEINLINE uint32_t elemZ(const vector4_t v);
NM_FORCEINLINE uint32_t elemW(const vector4_t v);

NM_FORCEINLINE float floatX(const vector4_t v);
NM_FORCEINLINE float floatY(const vector4_t v);
NM_FORCEINLINE float floatZ(const vector4_t v);
NM_FORCEINLINE float floatW(const vector4_t v);

// set initial values
NM_FORCEINLINE vector4_t zero4f();
NM_FORCEINLINE vector4_t one4f();
NM_FORCEINLINE vector4_t quat4f();
NM_FORCEINLINE vector4_t negOne4f();
NM_FORCEINLINE vector4_t half4f();
NM_FORCEINLINE vector4_t fltEpsilon4f();

NM_FORCEINLINE vector4_t set4f(const float f);
NM_FORCEINLINE vector4_t set4f(const float x, const float y, const float z, const float w);

// set element
NM_FORCEINLINE vector4_t setxf(const vector4_t d, const vector4_t s);
NM_FORCEINLINE vector4_t setyf(const vector4_t d, const vector4_t s);
NM_FORCEINLINE vector4_t setzf(const vector4_t d, const vector4_t s);
NM_FORCEINLINE vector4_t setwf(const vector4_t d, const vector4_t s);

// load one broadcast, load aligned, unaligned; aligned store
NM_FORCEINLINE vector4_t load1fp(float const* fp);
NM_FORCEINLINE vector4_t load4f(float const* fp);
NM_FORCEINLINE vector4_t loadu4f(float const* fp);
NM_FORCEINLINE void      store4f(float* fp, const vector4_t v);

// dot cross products, quat multiplier
NM_FORCEINLINE vector4_t sum4f(const vector4_t v);
NM_FORCEINLINE vector4_t dot4f(const vector4_t a, const vector4_t b);
NM_FORCEINLINE vector4_t dot3f(const vector4_t a, const vector4_t b);
NM_FORCEINLINE vector4_t cross3f(const vector4_t a, const vector4_t b);

NM_FORCEINLINE vector4_t qqMul(const vector4_t a, const vector4_t b);

// 4 way float comparison selection/merge
NM_FORCEINLINE vector4_t sel4cmpEQ(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse);
NM_FORCEINLINE vector4_t sel4cmpLT(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse);
NM_FORCEINLINE vector4_t sel4cmp2LT(const vector4_t& vecA1, const vector4_t& vecA2, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse);

// 4 way bitwise comparison selection/merge
NM_FORCEINLINE vector4_t sel4cmpBits(const vector4_t& vecVal, const vector4_t& vecBits, const vector4_t& selTrue, const vector4_t& selFalse);
NM_FORCEINLINE vector4_t sel4cmpMask(const vector4_t& mask, const vector4_t& selTrue, const vector4_t& selFalse);

// vector masks
NM_FORCEINLINE vector4_t mask4cmpLT(const vector4_t vecA, const vector4_t vecB);
NM_FORCEINLINE vector4_t mask4cmpEQ(const vector4_t vecA, const vector4_t vecB);

NM_FORCEINLINE vector4_t mask4i();
NM_FORCEINLINE vector4_t mask4i(const uint32_t mask);
NM_FORCEINLINE vector4_t maskr4i(const uint32_t mask);
NM_FORCEINLINE vector4_t mask4i(const uint8_t mask0, const uint8_t mask1, const uint8_t mask2, const uint8_t mask3);

// 4 vectors at once, aligned
NM_FORCEINLINE void load4v(float const* data, vector4_t& v0, vector4_t& v1, vector4_t& v2, vector4_t& v3);
NM_FORCEINLINE void store4v(float* data, const vector4_t& v0, const vector4_t& v1, const vector4_t& v2, const vector4_t& v3);

// 'matrix' transpose
NM_FORCEINLINE void transpose4v(vector4_t& r1, vector4_t& r2, vector4_t& r3, vector4_t& r4);
NM_FORCEINLINE void transpose4vConst(vector4_t& r1, vector4_t& r2, vector4_t& r3, vector4_t& r4,
                                     const vector4_t& a, const vector4_t& b, const vector4_t& c, const vector4_t& d);

// matrix stuff
NM_FORCEINLINE vector4_t horzadd4(const vector4_t& a, const vector4_t& b, const vector4_t& c, const vector4_t& d);
NM_FORCEINLINE vector4_t horzadd3(const vector4_t& a, const vector4_t& b, const vector4_t& c);

NM_FORCEINLINE void buildQMP(const vector4_t& quat,
                             vector4_t& M1x, vector4_t& M1y, vector4_t& M1z, vector4_t& M1w,
                             vector4_t& M2x, vector4_t& M2y, vector4_t& M2z, vector4_t& M2w);

// unpack functions
NM_FORCEINLINE vector4_t unpacku4i8(uint8_t const* data);

NM_FORCEINLINE vector4_t unpack4i16(uint16_t const* data);
NM_FORCEINLINE vector4_t unpacku4i16(uint16_t const* data);

// unpack with float conversion
NM_FORCEINLINE vector4_t unpack4i16f(uint16_t const* data);
NM_FORCEINLINE vector4_t unpacku4i16f(uint16_t const* data);

NM_FORCEINLINE void unpacku6i16f(vector4_t& r1, vector4_t& r2, uint16_t const* data);

NM_FORCEINLINE void unpack8i16f(vector4_t& r1,  vector4_t& r2, uint16_t const* data);
NM_FORCEINLINE void unpacku8i16f(vector4_t& r1, vector4_t& r2, uint16_t const* data);

// unquantise
NM_FORCEINLINE vector4_t unpackQuantizedVectorData(uint32_t const* qVec);
NM_FORCEINLINE vector4_t unpackQuantizedVector(uint32_t const* qVec);

NM_FORCEINLINE void unpack2QuantizedVectors(vector4_t& r1, vector4_t& r2, uint32_t const* qVec);
NM_FORCEINLINE void unpack4QuantizedVectorsPacked(vector4_t& r1, vector4_t& r2, vector4_t& r3, uint32_t const* qVec);

//----------------------------------------------------------------------------------------------------------------------
