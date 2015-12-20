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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_PLATFORM_H
#define NM_PLATFORM_H
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NaturalMotionPlatform Natural Motion platform specific core library.
///
/// Contains common core functionality with specific implementations for target platforms.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \namespace NMP
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Detect the compiler currently in use.
#if defined(__INTEL_COMPILER)
  #define NM_COMPILER_INTEL
#elif defined(_MSC_VER)
  #define NM_COMPILER_MSVC
  #if (_MSC_VER >= 1600)
    #define NM_MSVC_2010
  #endif
#elif defined(__SNC__)
  #define NM_COMPILER_SNC
#elif defined (__llvm__)
  // Note that __GNUC__ is also defined by LLVM so this check MUST happen first.
  #define NM_COMPILER_LLVM
#elif defined(__GNUC__)
  // We have to differentiate between GHS and regular GCC as GHS requires some additional compatibility code.
  #if defined(__ghs__)
    #define NM_COMPILER_GHS
  #else
    #define NM_COMPILER_GCC
  #endif
#else
  #error NM: No NM_COMPILER_... defined / detected
#endif

//----------------------------------------------------------------------------------------------------------------------
// Automatically configure NM_HOST_? and so forth from compiler defines.
// NB: if this include fails, the host-platform-specific NMPlatform include directory is missing from your include path.
#include "NMHost.h"

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_BIGENDIAN
  #error NM: NM_HOST_BIGENDIAN has not been defined!
#endif // NM_HOST_BIGENDIAN

//----------------------------------------------------------------------------------------------------------------------
/// \def NM_TARGET_BIGENDIAN
/// \ingroup NaturalMotionPlatform
/// \brief Set target endian-ness.
#ifndef NM_TARGET_BIGENDIAN
  #define NM_TARGET_BIGENDIAN NM_HOST_BIGENDIAN
#endif

#if NM_TARGET_BIGENDIAN != NM_HOST_BIGENDIAN
  #define NM_ENDIAN_SWAP
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NM_DEBUG
/// \ingroup NaturalMotionPlatform
/// \brief Release / Debug configuration flag.
#if defined(_DEBUG) || defined(DEBUG)
  #define NM_DEBUG
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NM_ENABLE_EXCEPTIONS
/// \ingroup NaturalMotionPlatform
/// \brief Determine how exceptions should be handled.
///
/// This can be overridden by defining NM_ENABLE_EXCEPTIONS to be 0 or 1 on the preprocessor command line.
///
#if !defined(NM_ENABLE_EXCEPTIONS)
  #define NM_ENABLE_EXCEPTIONS 0
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_UNUSED
/// \ingroup NaturalMotionPlatform
/// \brief Prevents arguments that are not used with the function body from producing compiler warnings.
#define NMP_UNUSED(x)

//----------------------------------------------------------------------------------------------------------------------
/// \def NM_FORCEINLINE
/// \ingroup NaturalMotionPlatform
/// \brief Configure inline options, with optional force inline.
#define NM_INLINE inline

#if defined(NM_COMPILER_MSVC)
  #define NM_FORCEINLINE  __forceinline
#else
  #define NM_FORCEINLINE  inline
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_SPRINTF
/// \ingroup NaturalMotionPlatform
/// \brief Secure sprintf function to avoid CRT incompatibility issues.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #define NMP_SPRINTF sprintf_s
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
  #define NMP_SPRINTF snprintf
#else
  #error Secure printf undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_STRUPR
/// \ingroup NaturalMotionPlatform
/// \brief Secure sprintf function to avoid CRT incompatibility issues.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
#define NMP_STRUPR(buffer, size) _strupr_s(buffer, size)
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
#define NMP_STRUPR(buffer, size) _strupr(buffer)
#else
#error Secure printf undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_VSPRINTF
/// \ingroup NaturalMotionPlatform
/// \brief Secure vsprintf function to avoid CRT incompatibility issues.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #define NMP_VSPRINTF vsprintf_s
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
  #define NMP_VSPRINTF vsnprintf
#else
  #error Secure vsprintf undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_STRNCPY_S
/// \ingroup NaturalMotionPlatform
///
/// \brief Secure strcpy function to avoid CRT incompatibility issues.
/// If the dest buffer is too small then the copied string will be truncated to fit.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #define NMP_STRNCPY_S(NMP_STRCPY_DST, NMP_STRCPY_DESTSIZE, NMP_STRNCPY_SRC) strncpy_s((NMP_STRCPY_DST), (NMP_STRCPY_DESTSIZE), (NMP_STRNCPY_SRC), NMP_STRCPY_DESTSIZE - 1)
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
  #define NMP_STRNCPY_S(NMP_STRCPY_DST, NMP_STRCPY_DESTSIZE, NMP_STRNCPY_SRC) strncpy((NMP_STRCPY_DST), (NMP_STRNCPY_SRC), NMP_STRCPY_DESTSIZE)
#else
  #error Secure strncpy_s undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_STRNCAT_S
/// \ingroup NaturalMotionPlatform
/// \brief Secure strcat_s function to avoid CRT incompatibility issues.
///
/// Note that this follows the MS usage on all platforms. strncat interprets the size parameter very differently.
/// If the dest buffer is too small then the concatenated string will be truncated to fit.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #define NMP_STRNCAT_S(NMP_STRCAT_DST, NMP_STRCAT_DESTSIZE, NMP_STRNCAT_SRC) strncat_s((NMP_STRCAT_DST), (NMP_STRCAT_DESTSIZE), (NMP_STRNCAT_SRC), (NMP_STRCAT_DESTSIZE - NMP_STRLEN((NMP_STRCAT_DST)) - 1))
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
  #define NMP_STRNCAT_S(NMP_STRCAT_DST, NMP_STRCAT_DESTSIZE, NMP_STRNCAT_SRC) strncat((NMP_STRCAT_DST), (NMP_STRNCAT_SRC), (NMP_STRCAT_DESTSIZE - NMP_STRLEN((NMP_STRCAT_DST)) - 1))
#else
  #error Secure strcat_s undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_STRLEN
/// \ingroup NaturalMotionPlatform
/// \brief Direct wrapper around strlen.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #include <string.h>
  #define NMP_STRLEN strlen
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
  #include <string.h>
  #define NMP_STRLEN strlen
#else
  #error strlen undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_STRCMP
/// \ingroup NaturalMotionPlatform
/// \brief Direct wrapper around strcmp.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #include <string.h>
  #define NMP_STRCMP strcmp
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
  #include <string.h>
  #define NMP_STRCMP strcmp
#else
  #error strcmp undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_STRNCMP
/// \ingroup NaturalMotionPlatform
/// \brief Direct wrapper around strncmp.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
#include <string.h>
#define NMP_STRNCMP strncmp
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
#include <string.h>
#define NMP_STRNCMP strncmp
#else
#error strcmp undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_STRTOK
/// \ingroup NaturalMotionPlatform
/// \brief Secure strtok_s function to avoid CRT incompatibility issues.
///
/// Note that this follows the MS usage on all platforms.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #define NMP_STRTOK(NMP_STRCAT_DST, NMP_STRNCAT_SRC, NMP_STRCAT_CONTEXT) strtok_s((NMP_STRCAT_DST), (NMP_STRNCAT_SRC), (NMP_STRCAT_CONTEXT))
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
  inline char* NMP_STRTOK(char* NMP_STRCAT_DST, const char* NMP_STRNCAT_SRC, char** NMP_STRCAT_CONTEXT)
  {
    (void)NMP_STRCAT_CONTEXT;
    return strtok((NMP_STRCAT_DST), (NMP_STRNCAT_SRC));
  }
#else
#error Secure strtok_s undefined for this compiler / platform
#endif

//----------------------------------------------------------------------------------------------------------------------
/// Advanced function modifiers, valid on any compilers that support them
/// \def NM_FNDEF_NORETURN
/// \ingroup NaturalMotionPlatform
/// \brief Tag a function as one that does not return - eg. exit/quit/fatal assert.
///
/// \def NM_FNDEF_DEPRECATED
/// \ingroup NaturalMotionPlatform
/// \brief Allow function use, but generate compiler warnings if so.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #define NM_FNDEF_NORETURN(fn)   __declspec(noreturn) fn;
  #define NM_FNDEF_DEPRECATED(fn) __declspec(deprecated) fn;
#elif defined(NM_COMPILER_LLVM) || defined(NM_COMPILER_GCC) || defined(NM_COMPILER_SNC)
  #define NM_FNDEF_NORETURN(fn)   fn __attribute__ ((noreturn));
  #define NM_FNDEF_DEPRECATED(fn) fn __attribute__ ((deprecated));
#else
  #define NM_FNDEF_NORETURN(fn) fn
  #define NM_FNDEF_DEPRECATED(fn) fn
#endif

// we disable warning 4481 on VC, as we want to use the 'override', 'sealed', etc keywords;
// on other compilers, the macro safely evaluates to empty, so the keywords shouldn't spill out of MSVC builds.
#if defined NM_COMPILER_MSVC
  #pragma warning(disable: 4481) // nonstandard extension used: override specifier 'override'
#endif
#if defined(NM_COMPILER_INTEL)
  #pragma warning(disable: 1684) // conversion from pointer to same-sized integral type (potential portability problem)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NM_OVERRIDE
/// \ingroup NaturalMotionPlatform
///
/// NM_OVERRIDE can be optionally specified at the end of a virtual
/// method prototype to indicate that the method is overriding a
/// a virtual method in a base class. Then, if the base class method
/// prototype changes a compilation error is generated.
#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_LLVM)
  #define NM_OVERRIDE override
#else
  #define NM_OVERRIDE
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NM_SEALED
/// \ingroup NaturalMotionPlatform
///
/// NM_SEALED can be optionally specified at the end of a virtual
/// method prototype to indicate that it cannot be overridden.
/// This is useful when you want a method to be overridden up to a certain inheritance level and not beyond.
/// This can also be used to indicate that a class should not be inherited from.
#if defined NM_COMPILER_MSVC
  #define NM_SEALED sealed
#else
  #define NM_SEALED
#endif

//----------------------------------------------------------------------------------------------------------------------
// Assertions and built-in error checking definitions
//----------------------------------------------------------------------------------------------------------------------
/// \def NM_BREAK
/// \ingroup NaturalMotionPlatform
///
/// Define a NM_BREAK macro that inserts a breakpoint/pause during execution
/// NM_BREAK is always defined, debug or otherwise.
#ifndef NM_BREAK
  #error NM_BREAK has not been defined on this platform!
#endif // NM_BREAK
  


//----------------------------------------------------------------------------------------------------------------------
/// \def NM_ASSERT_COMPILE_TIME
/// \ingroup NaturalMotionPlatform
/// \brief Always define the compile-time assert. 2 Concat functions are required because ## is evaluated before the
/// terms are. Without the second define the typedef resolves to __NM_CTA__LINE__.
#define NM_ASSERT_CONCAT_(a, b) a##b
#define NM_ASSERT_CONCAT(a, b) NM_ASSERT_CONCAT_(a, b)
#define NM_ASSERT_COMPILE_TIME(exp) typedef char NM_ASSERT_CONCAT(__NM_CTA, __LINE__)[(exp)?1:-1]

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_ENABLE_ASSERTS
/// \ingroup NaturalMotionPlatform
/// \brief Can't use asserts on the SPU currently as it will run out of memory.
#ifndef NM_HOST_CELL_SPU
  #ifdef NM_DEBUG
    #define NMP_ENABLE_ASSERTS
  #else
    // Don't use asserts in release by default
    #define NMP_ENABLE_ASSERTSx
  #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
// Utility class to support NM_CHECK_NO_VTABLE macro.
template <typename T>
class FailToCompileIfHasVTable
{
  class Virtual : public T
  {
  public:
    Virtual() {}
    virtual ~Virtual() {}
  };

public:
  FailToCompileIfHasVTable()
  {
    NM_ASSERT_COMPILE_TIME(sizeof(T) != sizeof(Virtual));
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// \def NM_CHECK_NO_VTABLE
/// \ingroup NaturalMotionPlatform
///
/// Makes compilation fail with a negative subscript error if a class has a V table.
/// Use when you want to have tight control of V Tables creeping in where they are
/// not supposed to be. You wont be able to use this macro if the class you are testing
/// has a private destructor.
#define NM_CHECK_NO_VTABLE(class)  { FailToCompileIfHasVTable<class> check; }

#ifdef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_ENABLE_SPU_SPECIFIC_ASSERTS
/// \ingroup NaturalMotionPlatform
/// \brief We do enable SPU-specific asserts in debug on SPU.
  #ifdef NM_DEBUG
    #define NMP_ENABLE_SPU_SPECIFIC_ASSERTS
  #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_USED_FOR_ASSERTS
/// \ingroup NaturalMotionPlatform
/// \brief Indicates to that arguments are only used when asserts are enabled.
///
/// Prevents compiler warnings when asserts are not enabled.
#if defined(NMP_ENABLE_ASSERTS)
  #define NMP_USED_FOR_ASSERTS(x) x
#else
  #define NMP_USED_FOR_ASSERTS(x) NMP_UNUSED(x)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_MSG
/// \ingroup NaturalMotionPlatform
/// \brief Outputs a message string to stdout in all build configurations. 
///
/// Automatically includes the file and line name where the message originated.
/// May be implemented already in NMHost.h
#ifndef NMP_MSG
  void NMMessagef(const char* file, int32_t line, const char* fmt, ...);
  #define NMP_MSG(fmt, ...) { NMMessagef(__FILE__, __LINE__, fmt, ##__VA_ARGS__); }
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_STDOUT
/// \ingroup NaturalMotionPlatform
/// \brief Outputs a plain message string to stdout in all build configurations.
///
/// May be implemented already in NMHost.h
#ifndef NMP_STDOUT
  void NMMessagef(const char* file, int32_t line, const char* fmt, ...);
  #define NMP_STDOUT(fmt, ...) { NMMessagef(0, 0, fmt, ##__VA_ARGS__); }
#endif // NMP_STDOUT

//----------------------------------------------------------------------------------------------------------------------
/// Assertions. These only work if NM_ENABLE_ASSERTS is defined.
///
/// \def NMP_ASSERT
/// \ingroup NaturalMotionPlatform
///
/// \def NMP_ASSERT_MSG
/// \ingroup NaturalMotionPlatform
///
/// \def NMP_ASSERT_FAIL
/// \ingroup NaturalMotionPlatform
#ifdef NMP_ENABLE_ASSERTS
  // Enable debug messages if NMP_ENABLE_ASSERTS is on.
  #define NMP_DEBUG_MSG NMP_MSG
  #define NMP_ASSERT(exp)                { if (!(exp)) { NMP_MSG("Assertion Failed : %s", #exp); NM_BREAK(); } }
  #define NMP_ASSERT_FAIL()              { NMP_MSG("NMP_ASSERT_FAIL\n"); NM_BREAK(); }
  #define NMP_ASSERT_MSG(exp, ...)       { if (!(exp)) { NMP_MSG("Assertion Failed : %s", #exp); NMP_MSG( __VA_ARGS__ ); NM_BREAK(); } }
  #define NMP_ASSERT_FAIL_MSG(fmt, ...)  { NMP_MSG(fmt, ##__VA_ARGS__); NM_BREAK(); }
#else  // NMP_ENABLE_ASSERTS
  // No asserts in release builds
  #define NMP_ASSERT(exp) {}
  #define NMP_ASSERT_MSG(exp, ...) {}
  #define NMP_ASSERT_FAIL() {}
  #define NMP_ASSERT_FAIL_MSG(...) {}
  #define NMP_DEBUG_MSG(...) {}
#endif // NMP_ENABLE_ASSERTS

//----------------------------------------------------------------------------------------------------------------------
/// SPU-specific assertions. These are only enabled on SPU, in debug.  This allows us to catch common SPU problems
///  without turning on the full suite of assertions (which would eat up too much memory).
#ifdef NMP_ENABLE_SPU_SPECIFIC_ASSERTS
  #define NMP_SPU_ASSERT(exp)                { if (!(exp)) { NMP_MSG("Assertion Failed : %s", #exp); NM_BREAK(); } }
  #define NMP_SPU_ASSERT_FAIL()              { NMP_MSG("NMP_ASSERT_FAIL\n"); NM_BREAK(); }
  #define NMP_SPU_ASSERT_MSG(exp, ...)  { if (!(exp)) { NMP_MSG("Assertion Failed : %s", #exp); NMP_MSG( __VA_ARGS__ ); NM_BREAK(); } }
  #define NMP_SPU_ASSERT_FAIL_MSG(fmt, ...)  { NMP_MSG(fmt, ##__VA_ARGS__); NM_BREAK(); }
#else
  #define NMP_SPU_ASSERT(exp) {}
  #define NMP_SPU_ASSERT_MSG(exp, ...) {}
  #define NMP_SPU_ASSERT_FAIL() {}
  #define NMP_SPU_ASSERT_FAIL_MSG(msg, ...) {}
#endif // NMP_ENABLE_SPU_SPECIFIC_ASSERTS

//----------------------------------------------------------------------------------------------------------------------
/// Exceptions.
/// NMP_VERIFY behavior:
///    NM_ENABLE_EXCEPTIONS == TRUE - Throws an exception.
///    NM_ENABLE_EXCEPTIONS == FALSE && NMP_ENABLE_ASSERTS == TRUE - asserts with a message.
///    NM_ENABLE_EXCEPTIONS == FALSE && NMP_ENABLE_ASSERTS == FALSE - Do nothing.
///
/// \def NMP_THROW_ERROR
/// \def NMP_VERIFY
/// \def NMP_VERIFY_MSG
/// \def NMP_VERIFY_FAIL
/// \ingroup NaturalMotionPlatform
#if NM_ENABLE_EXCEPTIONS
  // Exceptions ON
  //--------------
namespace NMP
{
  class Exception
  {
  public:
    Exception(const char* message, const char* file, int line) :
      m_message(message),
      m_file(file),
      m_line(line)
    {
      sm_alreadyDealingWithAnException = true;
    }
    // Though the copy constructor is trivial, it looks like the destructor is being called twice
    // if it's not present. Maybe it's just a problem with the debugger, but leaving it doesn't hurt.
    Exception(const Exception& other)
    {
      m_message = other.m_message;
      m_file = other.m_file;
      m_line = other.m_line;
    }
    ~Exception()
    {
      sm_alreadyDealingWithAnException = false;
    }
    const char* getMessage() const { return m_message; }
    const char* getFile() const { return m_file; }
    uint32_t getLine() const { return m_line; }
    static bool alreadyDealingWithAnException() { return sm_alreadyDealingWithAnException; }
  protected:
    const char* m_message,
          *m_file;
    uint32_t    m_line;
    static bool sm_alreadyDealingWithAnException;
  };
} // namespace NMP
  
extern void NMThrowError(const char* file, int line, const char* fmt, ...);

  #define NMP_THROW_ERROR(msg, ...) NMThrowError(__FILE__, __LINE__, msg, ##__VA_ARGS__);
  #define NMP_VERIFY(cond) { if (!(cond)) { NMP_THROW_ERROR(#cond); } }
  #define NMP_VERIFY_MSG(cond, ...) { if (!(cond)) { NMThrowError(__FILE__, __LINE__, ##__VA_ARGS__); } }
  #define NMP_VERIFY_FAIL(...) { NMThrowError(__FILE__, __LINE__, ##__VA_ARGS__); }

#else // NM_ENABLE_EXCEPTIONS
  // Exceptions OFF
  //---------------
  
  #if defined(NMP_ENABLE_ASSERTS)
    // Exceptions are off but asserts are enabled, so show messages and assert.
    #define NMP_THROW_ERROR(msg, ...) { NMP_MSG(msg, ##__VA_ARGS__); NMP_ASSERT_FAIL(); }
    #define NMP_VERIFY(cond) { if (!(cond)) { NMP_THROW_ERROR(#cond); } }
    #define NMP_VERIFY_MSG(cond, msg, ...) { if (!(cond)) { NMP_THROW_ERROR(msg, ##__VA_ARGS__); } }
    #define NMP_VERIFY_FAIL(msg, ...) { NMP_THROW_ERROR(msg, ##__VA_ARGS__); }
  #else   // NMP_ENABLE_ASSERTS
    // Exceptions are off and asserts are off don't do anything. 
    #define NMP_THROW_ERROR(msg, ...)
    #define NMP_VERIFY(cond)
    #define NMP_VERIFY_MSG(cond, ...)
    #define NMP_VERIFY_FAIL(...)
  #endif  // NMP_ENABLE_ASSERTS

#endif // NM_ENABLE_EXCEPTIONS

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_USED_FOR_VERIFY
/// \ingroup NaturalMotionPlatform
/// \brief Indicates to that arguments are only used when verifies are enabled.
///
/// Prevents compiler warnings when exceptions or asserts are not enabled.
#if NM_ENABLE_EXCEPTIONS || defined(NMP_ENABLE_ASSERTS)
  #define NMP_USED_FOR_VERIFY(x) x
#else
  #define NMP_USED_FOR_VERIFY(x) NMP_UNUSED(x)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_ALIGN_PREFIX
/// \def NMP_ALIGN_SUFFIX
/// \ingroup NaturalMotionPlatform
///
/// Alignment macros; as MSVC / GCC require the tag at different ends of a declaration
/// NMP_ALIGN_PREFIX(16) class Vec3 { ... } NMP_ALIGN_SUFFIX(16)
#ifdef NMP_NO_ALIGNMENT
  #define NMP_ALIGN_PREFIX(sz)
  #define NMP_ALIGN_SUFFIX(sz)
#else
  #if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
    #define NMP_ALIGN_PREFIX(sz) __declspec(align(sz))
    #define NMP_ALIGN_SUFFIX(sz)
  #elif defined(NM_COMPILER_LLVM) || defined(NM_COMPILER_GCC) || defined(NM_COMPILER_SNC) || defined(NM_COMPILER_GHS)
    #define NMP_ALIGN_PREFIX(sz)
    #define NMP_ALIGN_SUFFIX(sz) __attribute__ ((aligned(sz)))
  #else
    #error "No alignment attributes defined for this platform."
  #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \def NM_CheckAligned16
/// \ingroup NaturalMotionPlatform
///
/// Breaks if the current object is not aligned on a 16 byte boundary.
#define NM_CheckAligned16 { if (((size_t)this&15)!=0) NM_BREAK(); }

//----------------------------------------------------------------------------------------------------------------------
/// \brief Check if a value is aligned to the specified byte boundary.
NM_INLINE bool NMP_IS_ALIGNED(const void* ptr, size_t alignment)
{
#ifdef NM_COMPILER_INTEL
  const unsigned char* cptr = (const unsigned char*)ptr;
  const unsigned char* nullptr = 0;
  const ptrdiff_t address = (cptr - nullptr);
  return (address & (--alignment)) == 0;
#else
  return (((size_t)ptr) & (--alignment)) == 0;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_NATURAL_TYPE_ALIGNMENT
/// \ingroup NaturalMotionPlatform
///
/// Used to tell compiler the required alignment and hence stride (in bytes) that is most commonly required,
/// for structures, classes etc.
/// Compiler will pack the end of the structure to this stride width.
#define NMP_NATURAL_TYPE_ALIGNMENT 4

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_VECTOR_ALIGNMENT
/// \ingroup NaturalMotionPlatform
///
/// Used to tell compiler the required alignment and hence stride (in bytes) for a vector type.
/// NOTE: if a vector type is included as a member of a structure then the structure would need to use this alignment.
#define NMP_VECTOR_ALIGNMENT 16

/// Determine a string extension for assets compiled for a specific platform format.
#if NM_TARGET_BIGENDIAN
  //Note that we have no big-endian 64 bit platforms supported at the moment
  #define NM_PLATFORM_FORMAT_STRING "BE32"
#else // NM_TARGET_BIGENDIAN
  #ifdef NM_HOST_64_BIT
    #define NM_PLATFORM_FORMAT_STRING "LE64"
  #else
    #define NM_PLATFORM_FORMAT_STRING "LE32"
  #endif
#endif // NM_TARGET_BIGENDIAN

#ifndef NM_PLATFORM_FORMAT_STRING
  #error NM: No valid platform format string has been supplied.
#endif // NM_PLATFORM_FORMAT_STRING


#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  #define NMP_DLL_EXPORT __declspec(dllexport)
#else
  // Unsupported on other platforms
  #define NMP_DLL_EXPORT NM_ASSERT_COMPILE_TIME(0)
#endif//NMP_DLL_EXPORT

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_PLATFORM_H
