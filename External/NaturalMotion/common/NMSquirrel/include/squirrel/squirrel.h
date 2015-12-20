/*
Copyright (c) 2003-2009 Alberto Demichelis

This software is provided 'as-is', without any
express or implied warranty. In no event will the
authors be held liable for any damages arising from
the use of this software.

Permission is granted to anyone to use this software
for any purpose, including commercial applications,
and to alter it and redistribute it freely, subject
to the following restrictions:

1. The origin of this software must not be
misrepresented; you must not claim that
you wrote the original software. If you
use this software in a product, an
acknowledgment in the product
documentation would be appreciated but is
not required.

2. Altered source versions must be plainly
marked as such, and must not be
misrepresented as being the original
software.

3. This notice may not be removed or
altered from any source distribution.

*/
//---------------------------------------------------------------------------------------------------
/// \file squirrel.h
/// \author Alberto Demichelis
//---------------------------------------------------------------------------------------------------
#ifndef _SQUIRREL_H_
#define _SQUIRREL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SQUIRREL_API
# define SQUIRREL_API
#endif

#define SQUIRREL_UNUSED(X)



#ifdef __PPU__
#define squirrel_assert(sq_assert_cond) { (void)sizeof(sq_assert_cond); };

#else

#if defined(_DEBUG)
#include <cassert>
#define squirrel_assert(sq_assert_cond) \
  do \
  { \
  if (!!(sq_assert_cond)) \
  { \
  break; \
} \
    else \
  { \
  _wassert(_CRT_WIDE(#sq_assert_cond), _CRT_WIDE(__FILE__), __LINE__); \
} \
  __pragma(warning(push)) \
  __pragma(warning(disable: 4127)) \
} while(false) \
  __pragma(warning(pop))
#else
#define squirrel_assert(sq_assert_cond) \
  do \
  { \
  (void)sizeof(sq_assert_cond); \
  __pragma(warning(push)) \
  __pragma(warning(disable: 4127)) \
} while(false) \
  __pragma(warning(pop))
#endif

#endif


#ifdef _MSC_VER
  typedef signed __int32    int32_t;
  typedef unsigned __int32  uint32_t;
  typedef signed __int64    int64_t;
  typedef unsigned __int64  uint64_t;
#else
#include <stdint.h>
#include <stddef.h>
#endif

  /// \brief check for 64 bit architectures
#if (defined(_WIN64) || defined(_LP64))
#ifndef _SQ64
#define _SQ64
#endif
#endif

#ifdef _SQ64
  typedef int64_t SQInteger;
  typedef uint64_t SQUnsignedInteger;
  typedef uint64_t SQHash; /*should be the same size of a pointer*/
  typedef int32_t SQInt32;
#else
  typedef int32_t SQInteger;
  typedef int32_t SQInt32; /*must be 32 bits(also on 64bits processors)*/
  typedef uint32_t SQUnsignedInteger;
  typedef uint32_t SQHash; /*should be the same size of a pointer*/
#endif


#ifdef SQUSEDOUBLE
  typedef double SQFloat;
#else
  typedef float SQFloat;
#endif

#if defined(SQUSEDOUBLE) && !defined(_SQ64)
#ifdef _MSC_VER
  typedef __int64 SQRawObjectVal; //must be 64bits
#else
  typedef long SQRawObjectVal; //must be 64bits
#endif
#define SQ_OBJECT_RAWINIT() { _unVal.raw = 0; }
#else
  typedef SQUnsignedInteger SQRawObjectVal; //is 32 bits on 32 bits builds and 64 bits otherwise
#define SQ_OBJECT_RAWINIT()
#endif

  typedef void* SQUserPointer;
  typedef SQUnsignedInteger SQBool;
  typedef SQInteger SQRESULT;

#define SQTrue	(1)
#define SQFalse	(0)

  struct SQVM;
  struct SQTable;
  struct SQArray;
  struct SQString;
  struct SQClosure;
  struct SQGenerator;
  struct SQNativeClosure;
  struct SQUserData;
  struct SQFunctionProto;
  struct SQRefCounted;
  struct SQClass;
  struct SQInstance;
  struct SQDelegable;
  struct SQOuter;

#ifdef _UNICODE
#define SQUNICODE
#endif

#ifdef SQUNICODE
  // 1400 = VS8
#if (defined(_MSC_VER) && _MSC_VER >= 1400) || (defined(__GNUC__) && __GNUC__ >= 3) || __CYGWIN__
#include <wchar.h>
#include <wctype.h>
#else
  typedef unsigned short wchar_t;
#endif

  typedef wchar_t SQChar;
#define _SC(a) L##a
#define	scstrcmp	wcscmp
#define scsprintf	swprintf
#define scstrlen	wcslen
#define scstrtod	wcstod
#define scstrtol	wcstol
#define scatoi		_wtoi
#define scstrtoul	wcstoul
#define scvsprintf	vswprintf
#define scstrstr	wcsstr
#define scisspace	iswspace
#define scisdigit	iswdigit
#define scisxdigit	iswxdigit
#define scisalpha	iswalpha
#define sciscntrl	iswcntrl
#define scisalnum	iswalnum
#define scprintf	wprintf
#define MAX_CHAR 0xFFFF
#else
  typedef char SQChar;
#define _SC(a) a
#define	scstrcmp	strcmp
#if defined(_MSC_VER)
#define scsprintf sprintf_s
#else
#define scsprintf(SQ_STRING_ARG, SQ_SIZE_ARG, SQ_FORMAT_ARG, ...) sprintf(SQ_STRING_ARG, SQ_FORMAT_ARG, __VA_ARGS__)
#endif
#define scstrlen	strlen
#define scstrtod	strtod
#define scstrtol	strtol
#define scatoi		atoi
#define scstrtoul	strtoul
#if defined(_MSC_VER)
#define scvsprintf	vsprintf_s
#else
#define scvsprintf(SQ_STRING_ARG, SQ_SIZE_ARG, SQ_FORMAT_ARG, ...) vsprintf(SQ_STRING_ARG, SQ_FORMAT_ARG, __VA_ARGS__)
#endif
#define scstrstr	strstr
#define scisspace	isspace
#define scisdigit	isdigit
#define scisxdigit	isxdigit
#define sciscntrl	iscntrl
#define scisalpha	isalpha
#define scisalnum	isalnum
#define scprintf	printf
#define MAX_CHAR 0xFF
#endif

#define SQUIRREL_VERSION	_SC("Squirrel 3.0 beta 1")
#define SQUIRREL_COPYRIGHT	_SC("Copyright (C) 2003-2009 Alberto Demichelis")
#define SQUIRREL_AUTHOR		_SC("Alberto Demichelis")

#define SQ_VMSTATE_IDLE			0
#define SQ_VMSTATE_RUNNING		1
#define SQ_VMSTATE_SUSPENDED	2

#define SQUIRREL_EOB 0
#define SQ_BYTECODE_STREAM_TAG	0xFAFA

#define SQOBJECT_REF_COUNTED	0x08000000
#define SQOBJECT_NUMERIC		0x04000000
#define SQOBJECT_DELEGABLE		0x02000000
#define SQOBJECT_CANBEFALSE		0x01000000

#define SQ_MATCHTYPEMASKSTRING (-99999)

#define _RT_MASK 0x00FFFFFF
#define _RAW_TYPE(type) (type&_RT_MASK)

#define _RT_NULL			0x00000001
#define _RT_INTEGER			0x00000002
#define _RT_FLOAT			0x00000004
#define _RT_BOOL			0x00000008
#define _RT_STRING			0x00000010
#define _RT_TABLE			0x00000020
#define _RT_ARRAY			0x00000040
#define _RT_USERDATA		0x00000080
#define _RT_CLOSURE			0x00000100
#define _RT_NATIVECLOSURE	0x00000200
#define _RT_GENERATOR		0x00000400
#define _RT_USERPOINTER		0x00000800
#define _RT_THREAD			0x00001000
#define _RT_FUNCPROTO		0x00002000
#define _RT_CLASS			0x00004000
#define _RT_INSTANCE		0x00008000
#define _RT_WEAKREF			0x00010000
#define _RT_OUTER			0x00020000

  /// Possible types of stack objects.
  typedef enum tagSQObjectType{
    OT_NULL =			(_RT_NULL|SQOBJECT_CANBEFALSE), ///< Null type or nothing.
    OT_INTEGER =		(_RT_INTEGER|SQOBJECT_NUMERIC|SQOBJECT_CANBEFALSE), ///< 32 bit integer type.
    OT_FLOAT =			(_RT_FLOAT|SQOBJECT_NUMERIC|SQOBJECT_CANBEFALSE), ///< 32 bit floating point type.
    OT_BOOL =			(_RT_BOOL|SQOBJECT_CANBEFALSE), ///< Boolean type true or false.
    OT_STRING =			(_RT_STRING|SQOBJECT_REF_COUNTED), ///< A string.
    OT_TABLE =			(_RT_TABLE|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE), ///< A table made of key/value pairs.
    OT_ARRAY =			(_RT_ARRAY|SQOBJECT_REF_COUNTED), ///< An array.
    OT_USERDATA =		(_RT_USERDATA|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE), ///< User data.
    OT_CLOSURE =		(_RT_CLOSURE|SQOBJECT_REF_COUNTED), ///< Squirrel function.
    OT_NATIVECLOSURE =	(_RT_NATIVECLOSURE|SQOBJECT_REF_COUNTED), ///< Native C/C++ function.
    OT_GENERATOR =		(_RT_GENERATOR|SQOBJECT_REF_COUNTED), ///< A generator.
    OT_USERPOINTER =	_RT_USERPOINTER, ///< A user pointer.
    OT_THREAD =			(_RT_THREAD|SQOBJECT_REF_COUNTED), ///< A squirrel thread.
    /// \cond NO_DOXYGEN
    OT_FUNCPROTO =		(_RT_FUNCPROTO|SQOBJECT_REF_COUNTED), //internal usage only
    /// \endcond
    OT_CLASS =			(_RT_CLASS|SQOBJECT_REF_COUNTED), ///< A class definition.
    OT_INSTANCE =		(_RT_INSTANCE|SQOBJECT_REF_COUNTED|SQOBJECT_DELEGABLE), ///< An instance of a class.
    OT_WEAKREF =		(_RT_WEAKREF|SQOBJECT_REF_COUNTED), ///< A weak reference.
    OT_OUTER =			(_RT_OUTER|SQOBJECT_REF_COUNTED) //internal usage only
  }SQObjectType;

#define ISREFCOUNTED(t) (static_cast<int32_t>(t)&SQOBJECT_REF_COUNTED)

  typedef union tagSQObjectValue
  {
    struct SQTable *pTable;
    struct SQArray *pArray;
    struct SQClosure *pClosure;
    struct SQOuter *pOuter;
    struct SQGenerator *pGenerator;
    struct SQNativeClosure *pNativeClosure;
    struct SQString *pString;
    struct SQUserData *pUserData;
    SQInteger nInteger;
    SQFloat fFloat;
    SQUserPointer pUserPointer;
    struct SQFunctionProto *pFunctionProto;
    struct SQRefCounted *pRefCounted;
    struct SQDelegable *pDelegable;
    struct SQVM *pThread;
    struct SQClass *pClass;
    struct SQInstance *pInstance;
    struct SQWeakRef *pWeakRef;
    SQRawObjectVal raw;
  }SQObjectValue;

  /// \brief Squirrel object for maintaining a strong reference stack objects to
  /// prevent garbage collection.
  typedef struct tagSQObject
  {
    SQObjectType _type;
    SQObjectValue _unVal;
  }SQObject;

  typedef struct  tagSQMemberHandle{
    SQBool _static;
    SQInteger _index;
  }SQMemberHandle;

  typedef struct tagSQStackInfos{
    const SQChar* funcname;
    const SQChar* source;
    SQInteger line;
  }SQStackInfos;

  /// Handle to a squirrel virtual machine.
  typedef struct SQVM* HSQUIRRELVM;
  /// Handle to a squirrel object.
  typedef SQObject HSQOBJECT;
  /// Handle to a squirrel member.
  typedef SQMemberHandle HSQMEMBERHANDLE;
  /// Native closure function.
  typedef SQInteger (*SQFUNCTION)(HSQUIRRELVM);
  /// Release hook used for called when releasing user data or instance user pointers.
  typedef SQInteger (*SQRELEASEHOOK)(SQUserPointer,SQInteger size);
  /// Compiler error function for printing error compilation.
  typedef void (*SQCOMPILERERROR)(HSQUIRRELVM,const SQChar * /*desc*/,const SQChar * /*source*/,SQInteger /*line*/,SQInteger /*column*/);
  /// Print function for setting squirrel print function.
  typedef void (*SQPRINTFUNCTION)(HSQUIRRELVM,const SQChar * ,...);
  typedef void (*SQDEBUGHOOK)(HSQUIRRELVM /*v*/, SQInteger /*type*/, const SQChar * /*sourcename*/, SQInteger /*line*/, const SQChar * /*funcname*/);
  typedef SQInteger (*SQWRITEFUNC)(SQUserPointer,SQUserPointer,SQInteger);
  typedef SQInteger (*SQREADFUNC)(SQUserPointer,SQUserPointer,SQInteger);

  typedef SQInteger (*SQLEXREADFUNC)(SQUserPointer);

  typedef struct tagSQRegFunction{
    const SQChar *name;
    SQFUNCTION f;
    SQInteger nparamscheck;
    const SQChar *typemask;
  }SQRegFunction;

  typedef struct tagSQFunctionInfo {
    SQUserPointer funcid;
    const SQChar *name;
    const SQChar *source;
  }SQFunctionInfo;

  /*vm*/
  SQUIRREL_API HSQUIRRELVM sq_open(SQInteger initialstacksize);
  SQUIRREL_API HSQUIRRELVM sq_newthread(HSQUIRRELVM friendvm, SQInteger initialstacksize);
  SQUIRREL_API void sq_seterrorhandler(HSQUIRRELVM v);
  SQUIRREL_API void sq_close(HSQUIRRELVM v);
  SQUIRREL_API void sq_setforeignptr(HSQUIRRELVM v,SQUserPointer p);
  SQUIRREL_API SQUserPointer sq_getforeignptr(HSQUIRRELVM v);
  SQUIRREL_API void sq_setprintfunc(HSQUIRRELVM v, SQPRINTFUNCTION printfunc,SQPRINTFUNCTION errfunc);
  SQUIRREL_API SQPRINTFUNCTION sq_getprintfunc(HSQUIRRELVM v);
  SQUIRREL_API SQPRINTFUNCTION sq_geterrorfunc(HSQUIRRELVM v);
  SQUIRREL_API SQRESULT sq_suspendvm(HSQUIRRELVM v);
  SQUIRREL_API SQRESULT sq_wakeupvm(HSQUIRRELVM v,SQBool resumedret,SQBool retval,SQBool raiseerror,SQBool throwerror);
  SQUIRREL_API SQInteger sq_getvmstate(HSQUIRRELVM v);

  /*compiler*/
  SQUIRREL_API SQRESULT sq_compile(HSQUIRRELVM v,SQLEXREADFUNC read,SQUserPointer p,const SQChar *sourcename,SQBool raiseerror);
  SQUIRREL_API SQRESULT sq_compilebuffer(HSQUIRRELVM v,const SQChar *s,SQInteger size,const SQChar *sourcename,SQBool raiseerror);
  SQUIRREL_API void sq_enabledebuginfo(HSQUIRRELVM v, SQBool enable);
  SQUIRREL_API void sq_notifyallexceptions(HSQUIRRELVM v, SQBool enable);
  SQUIRREL_API void sq_setcompilererrorhandler(HSQUIRRELVM v,SQCOMPILERERROR f);

  /*stack operations*/
  SQUIRREL_API void sq_push(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API void sq_pop(HSQUIRRELVM v,SQInteger nelemstopop);
  SQUIRREL_API void sq_poptop(HSQUIRRELVM v);
  SQUIRREL_API void sq_remove(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQInteger sq_gettop(HSQUIRRELVM v);
  SQUIRREL_API void sq_settop(HSQUIRRELVM v,SQInteger newtop);
  SQUIRREL_API void sq_reservestack(HSQUIRRELVM v,SQInteger nsize);
  SQUIRREL_API SQInteger sq_cmp(HSQUIRRELVM v);
  SQUIRREL_API void sq_move(HSQUIRRELVM dest,HSQUIRRELVM src,SQInteger idx);

  /*object creation handling*/
  SQUIRREL_API SQUserPointer sq_newuserdata(HSQUIRRELVM v,SQUnsignedInteger size);
  SQUIRREL_API void sq_newtable(HSQUIRRELVM v);
  SQUIRREL_API void sq_newarray(HSQUIRRELVM v,SQInteger size);
  SQUIRREL_API void sq_newclosure(HSQUIRRELVM v,SQFUNCTION func,SQUnsignedInteger nfreevars);
  SQUIRREL_API SQRESULT sq_setparamscheck(HSQUIRRELVM v,SQInteger nparamscheck,const SQChar *typemask);
  SQUIRREL_API SQRESULT sq_bindenv(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API void sq_pushstring(HSQUIRRELVM v,const SQChar *s,SQInteger len);
  SQUIRREL_API void sq_pushfloat(HSQUIRRELVM v,SQFloat f);
  SQUIRREL_API void sq_pushinteger(HSQUIRRELVM v,SQInteger n);
  SQUIRREL_API void sq_pushbool(HSQUIRRELVM v,SQBool b);
  SQUIRREL_API void sq_pushuserpointer(HSQUIRRELVM v,SQUserPointer p);
  SQUIRREL_API void sq_pushnull(HSQUIRRELVM v);
  SQUIRREL_API SQObjectType sq_gettype(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQInteger sq_getsize(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_getbase(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQBool sq_instanceof(HSQUIRRELVM v);
  SQUIRREL_API void sq_tostring(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API void sq_tobool(HSQUIRRELVM v, SQInteger idx, SQBool *b);
  SQUIRREL_API SQRESULT sq_getstring(HSQUIRRELVM v,SQInteger idx,const SQChar **c);
  SQUIRREL_API SQRESULT sq_getinteger(HSQUIRRELVM v,SQInteger idx,SQInteger *i);
  SQUIRREL_API SQRESULT sq_getfloat(HSQUIRRELVM v,SQInteger idx,SQFloat *f);
  SQUIRREL_API SQRESULT sq_getbool(HSQUIRRELVM v,SQInteger idx,SQBool *b);
  SQUIRREL_API SQRESULT sq_getthread(HSQUIRRELVM v,SQInteger idx,HSQUIRRELVM *thread);
  SQUIRREL_API SQRESULT sq_getuserpointer(HSQUIRRELVM v,SQInteger idx,SQUserPointer *p);
  SQUIRREL_API SQRESULT sq_getuserdata(HSQUIRRELVM v,SQInteger idx,SQUserPointer *p,SQUserPointer *typetag);
  SQUIRREL_API SQRESULT sq_settypetag(HSQUIRRELVM v,SQInteger idx,SQUserPointer typetag);
  SQUIRREL_API SQRESULT sq_gettypetag(HSQUIRRELVM v,SQInteger idx,SQUserPointer *typetag);
  SQUIRREL_API void sq_setreleasehook(HSQUIRRELVM v,SQInteger idx,SQRELEASEHOOK hook);
  SQUIRREL_API SQChar *sq_getscratchpad(HSQUIRRELVM v,SQInteger minsize);
  SQUIRREL_API SQRESULT sq_getfunctioninfo(HSQUIRRELVM v,SQInteger idx,SQFunctionInfo *fi);
  SQUIRREL_API SQRESULT sq_getclosureinfo(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger *nparams,SQUnsignedInteger *nfreevars);
  SQUIRREL_API SQRESULT sq_setnativeclosurename(HSQUIRRELVM v,SQInteger idx,const SQChar *name);
  SQUIRREL_API SQRESULT sq_setinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer p);
  SQUIRREL_API SQRESULT sq_getinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer *p,SQUserPointer typetag);
  SQUIRREL_API SQRESULT sq_setclassudsize(HSQUIRRELVM v, SQInteger idx, SQInteger udsize);
  SQUIRREL_API SQRESULT sq_newclass(HSQUIRRELVM v,SQBool hasbase);
  SQUIRREL_API SQRESULT sq_createinstance(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_setattributes(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_getattributes(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_getclass(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API void sq_weakref(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_getdefaultdelegate(HSQUIRRELVM v,SQObjectType t);
  SQUIRREL_API SQRESULT sq_getmemberhandle(HSQUIRRELVM v,SQInteger idx,HSQMEMBERHANDLE *handle);
  SQUIRREL_API SQRESULT sq_getbyhandle(HSQUIRRELVM v,SQInteger idx,HSQMEMBERHANDLE *handle);
  SQUIRREL_API SQRESULT sq_setbyhandle(HSQUIRRELVM v,SQInteger idx,HSQMEMBERHANDLE *handle);

  /*object manipulation*/
  SQUIRREL_API void sq_pushroottable(HSQUIRRELVM v);
  SQUIRREL_API void sq_pushregistrytable(HSQUIRRELVM v);
  SQUIRREL_API void sq_pushconsttable(HSQUIRRELVM v);
  SQUIRREL_API SQRESULT sq_setroottable(HSQUIRRELVM v);
  SQUIRREL_API SQRESULT sq_setconsttable(HSQUIRRELVM v);
  SQUIRREL_API SQRESULT sq_newslot(HSQUIRRELVM v, SQInteger idx, SQBool bstatic);
  SQUIRREL_API SQRESULT sq_deleteslot(HSQUIRRELVM v,SQInteger idx,SQBool pushval);
  SQUIRREL_API SQRESULT sq_set(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_get(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_rawget(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_rawset(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_rawdeleteslot(HSQUIRRELVM v,SQInteger idx,SQBool pushval);
  SQUIRREL_API SQRESULT sq_arrayappend(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_arraypop(HSQUIRRELVM v,SQInteger idx,SQBool pushval);
  SQUIRREL_API SQRESULT sq_arrayresize(HSQUIRRELVM v,SQInteger idx,SQInteger newsize);
  SQUIRREL_API SQRESULT sq_arrayreverse(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_arrayremove(HSQUIRRELVM v,SQInteger idx,SQInteger itemidx);
  SQUIRREL_API SQRESULT sq_arrayinsert(HSQUIRRELVM v,SQInteger idx,SQInteger destpos);
  SQUIRREL_API SQRESULT sq_setdelegate(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_getdelegate(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_clone(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_setfreevariable(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger nval);
  SQUIRREL_API SQRESULT sq_next(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_getweakrefval(HSQUIRRELVM v,SQInteger idx);
  SQUIRREL_API SQRESULT sq_clear(HSQUIRRELVM v,SQInteger idx);

  /*calls*/
  SQUIRREL_API SQRESULT sq_call(HSQUIRRELVM v,SQInteger params,SQBool retval,SQBool raiseerror);
  SQUIRREL_API SQRESULT sq_resume(HSQUIRRELVM v,SQBool retval,SQBool raiseerror);
  SQUIRREL_API const SQChar *sq_getlocal(HSQUIRRELVM v,SQUnsignedInteger level,SQUnsignedInteger idx);
  SQUIRREL_API const SQChar *sq_getfreevariable(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger nval);
  SQUIRREL_API SQRESULT sq_throwerror(HSQUIRRELVM v,const SQChar *err);
  SQUIRREL_API void sq_reseterror(HSQUIRRELVM v);
  SQUIRREL_API void sq_getlasterror(HSQUIRRELVM v);

  /*raw object handling*/
  SQUIRREL_API SQRESULT sq_getstackobj(HSQUIRRELVM v,SQInteger idx,HSQOBJECT *po);
  SQUIRREL_API void sq_pushobject(HSQUIRRELVM v,HSQOBJECT obj);
  SQUIRREL_API void sq_addref(HSQUIRRELVM v,HSQOBJECT *po);
  SQUIRREL_API SQBool sq_release(HSQUIRRELVM v,HSQOBJECT *po);
  SQUIRREL_API void sq_resetobject(HSQOBJECT *po);
  SQUIRREL_API const SQChar *sq_objtostring(HSQOBJECT *o);
  SQUIRREL_API SQBool sq_objtobool(HSQOBJECT *o);
  SQUIRREL_API SQInteger sq_objtointeger(HSQOBJECT *o);
  SQUIRREL_API SQFloat sq_objtofloat(HSQOBJECT *o);
  SQUIRREL_API SQRESULT sq_getobjtypetag(HSQOBJECT *o,SQUserPointer * typetag);
  SQUIRREL_API SQRESULT sq_getobjinstanceup(HSQOBJECT *o,SQUserPointer *p,SQUserPointer typetag);

  /*GC*/
  SQUIRREL_API SQInteger sq_collectgarbage(HSQUIRRELVM v);

  /*serialization*/
  SQUIRREL_API SQRESULT sq_writeclosure(HSQUIRRELVM vm,SQWRITEFUNC writef,SQUserPointer up);
  SQUIRREL_API SQRESULT sq_readclosure(HSQUIRRELVM vm,SQREADFUNC readf,SQUserPointer up);

  /*mem allocation*/
  SQUIRREL_API void *sq_malloc(SQUnsignedInteger size);
  SQUIRREL_API void *sq_realloc(void* p,SQUnsignedInteger oldsize,SQUnsignedInteger newsize);
  SQUIRREL_API void sq_free(void *p,SQUnsignedInteger size);

  /*debug*/
  SQUIRREL_API SQRESULT sq_stackinfos(HSQUIRRELVM v,SQInteger level,SQStackInfos *si);
  SQUIRREL_API void sq_setdebughook(HSQUIRRELVM v);

  /*UTILITY MACRO*/
#define sq_isnumeric(o) ((o)._type&SQOBJECT_NUMERIC)
#define sq_istable(o) ((o)._type==OT_TABLE)
#define sq_isarray(o) ((o)._type==OT_ARRAY)
#define sq_isfunction(o) ((o)._type==OT_FUNCPROTO)
#define sq_isclosure(o) ((o)._type==OT_CLOSURE)
#define sq_isgenerator(o) ((o)._type==OT_GENERATOR)
#define sq_isnativeclosure(o) ((o)._type==OT_NATIVECLOSURE)
#define sq_isstring(o) ((o)._type==OT_STRING)
#define sq_isinteger(o) ((o)._type==OT_INTEGER)
#define sq_isfloat(o) ((o)._type==OT_FLOAT)
#define sq_isuserpointer(o) ((o)._type==OT_USERPOINTER)
#define sq_isuserdata(o) ((o)._type==OT_USERDATA)
#define sq_isthread(o) ((o)._type==OT_THREAD)
#define sq_isnull(o) ((o)._type==OT_NULL)
#define sq_isclass(o) ((o)._type==OT_CLASS)
#define sq_isinstance(o) ((o)._type==OT_INSTANCE)
#define sq_isbool(o) ((o)._type==OT_BOOL)
#define sq_isweakref(o) ((o)._type==OT_WEAKREF)
#define sq_isouter(o) ((o)._type==OT_OUTER)
#define sq_type(o) ((o)._type)

  /* deprecated */
#define sq_createslot(v,n) sq_newslot(v,n,SQFalse)

#define SQ_OK (0)
#define SQ_ERROR (-1)

#define SQ_FAILED(res) (res<0)
#define SQ_SUCCEEDED(res) (res>=0)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_SQUIRREL_H_*/
