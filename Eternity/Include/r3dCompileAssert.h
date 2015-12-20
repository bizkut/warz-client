#pragma once

// compile time assert
#ifdef __cplusplus
#define JOIN( X, Y ) JOIN2(X,Y)
#define JOIN2( X, Y ) X##Y
namespace static_assert
{
	template <bool> struct STATIC_ASSERT_FAILURE;
	template <> struct STATIC_ASSERT_FAILURE<true> { enum { value = 1 }; };

	template<int x> struct static_assert_test{};
}
#define COMPILE_ASSERT(x) \
	typedef ::static_assert::static_assert_test<\
	sizeof(::static_assert::STATIC_ASSERT_FAILURE< (bool)( x ) >)>\
	JOIN(_static_assert_typedef, __LINE__)
#else // __cplusplus
#define COMPILE_ASSERT(x) extern int __dummy[(int)x]
#endif // __cplusplus
#define VERIFY_EXPLICIT_CAST(from, to) COMPILE_ASSERT(sizeof(from) == sizeof(to)) 
