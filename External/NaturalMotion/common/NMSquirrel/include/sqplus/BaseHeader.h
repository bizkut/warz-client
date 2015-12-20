#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_BASEHEADER_H_
#define _SQPLUS_BASEHEADER_H_
//----------------------------------------------------------------------------------------------------------------------
/// \file sqplus/BaseHeader.h
/// \author James Whitworth
//----------------------------------------------------------------------------------------------------------------------

/// \cond NO_DOXYGEN
#if _MSC_VER >= 1400
// Visual Studio 2005 and above.
#define SQPLUS_OVERRIDE override
#else
#define SQPLUS_OVERRIDE
#endif
/// \endcond

/// \brief Namespace that contains SquirrelPlus functionality.
namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief Class for passing around types.
//----------------------------------------------------------------------------------------------------------------------
template<class T>
struct TypeWrapper { };

/// \brief For binding classes that have no base class.
class NoBaseClass { };

//----------------------------------------------------------------------------------------------------------------------
/// \brief List of basic types that are supported.
enum ScriptVarType
{
  VAR_TYPE_NONE = -1,
  VAR_TYPE_BOOL = 0,
  VAR_TYPE_CHAR,
  VAR_TYPE_UCHAR,
  VAR_TYPE_SHORT,
  VAR_TYPE_USHORT,
  VAR_TYPE_INT32,
  VAR_TYPE_UINT32,
  VAR_TYPE_INT64,
  VAR_TYPE_UINT64,
  VAR_TYPE_FLOAT,
  VAR_TYPE_DOUBLE,
  VAR_TYPE_STRING,
  VAR_TYPE_USER_POINTER,
  VAR_TYPE_INSTANCE
};

/*lint -emacro(19,DECLARE_INSTANCE_TYPEINFO)*/
/*lint -emacro(1576,DECLARE_INSTANCE_TYPEINFO)*/
/*lint -emacro(1577,DECLARE_INSTANCE_TYPEINFO)*/

/*lint -emacro(19,DECLARE_INSTANCE_TYPE_BASE)*/
/*lint -emacro(1576,DECLARE_INSTANCE_TYPE_BASE)*/
/*lint -emacro(1577,DECLARE_INSTANCE_TYPE_BASE)*/

/*lint -emacro(19,DECLARE_INSTANCE_TYPE_NAME)*/
/*lint -emacro(1576,DECLARE_INSTANCE_TYPE_NAME)*/
/*line -emacro(1577,DECLARE_INSTANCE_TYPE_NAME)*/

/*lint -emacro(19,DECLARE_CUSTOM_INSTANCE_TYPE_NAME)*/
/*lint -emacro(1576,DECLARE_CUSTOM_INSTANCE_TYPE_NAME)*/
/*lint -emacro(1577,DECLARE_CUSTOM_INSTANCE_TYPE_NAME)*/

/// \defgroup INSTANCE_DECL Type Binding Declarations
/// \brief Macros for binding types to script.

/// \addtogroup INSTANCE_DECL
/// @{

/// \brief Declares the TypeInfo struct for a particular class type.
#define DECLARE_INSTANCE_TYPEINFO(TYPE, NAME) \
  namespace sqp \
  { \
    template<> \
    struct TypeInfo<TYPE> \
    { \
      enum \
      { \
        TypeID = VAR_TYPE_INSTANCE, \
        Size = sizeof(TYPE), \
        TypeMask = 'x', \
        IsInstance = SQTrue \
      }; \
      const SQChar *m_typeName; \
      inline TypeInfo() \
      : m_typeName(_SC(#NAME)) \
      { \
      } \
      inline operator ScriptVarType() const \
      { \
        return static_cast<ScriptVarType>(TypeID); \
      } \
    }; \
  }

/// \brief Declares the Match and Get function specialisations for a class type so functions with TYPE as
/// arguments or returning TYPE can be automatically bound to script. Does not define any Push specialisations.
#define DECLARE_INSTANCE_TYPE_BASE(TYPE, NAME) \
  DECLARE_INSTANCE_TYPEINFO(TYPE, NAME) \
  namespace sqp \
  { \
  template<> \
    inline bool Match(TypeWrapper<TYPE *>, HSQUIRRELVM vm, SQInteger idx) \
    { \
      SQUserPointer up; \
      return ((sq_gettype(vm, idx) == OT_NULL) || (sq_getinstanceup(vm, idx, &up, ClassType<TYPE>::Get()) == SQ_OK)); \
    } \
    template<> \
    inline bool Match(TypeWrapper<TYPE &>, HSQUIRRELVM vm, SQInteger idx) \
    { \
      SQUserPointer up; \
      return (sq_getinstanceup(vm, idx, &up, ClassType<TYPE>::Get()) == SQ_OK); \
    } \
    inline bool Match(TypeWrapper<const TYPE *>, HSQUIRRELVM vm, SQInteger idx) \
    { \
      SQUserPointer up; \
      return ((sq_gettype(vm, idx) == OT_NULL) || (sq_getinstanceup(vm, idx, &up, ClassType<TYPE>::Get()) == SQ_OK)); \
    } \
    template<> \
    inline bool Match(TypeWrapper<const TYPE &>, HSQUIRRELVM vm, SQInteger idx) \
    { \
      SQUserPointer up; \
      return (sq_getinstanceup(vm, idx, &up, ClassType<TYPE>::Get()) == SQ_OK); \
    } \
    template<> \
    inline TYPE *Get(TypeWrapper<TYPE *>, HSQUIRRELVM vm, SQInteger idx) \
    { \
      SQUserPointer up; \
      if (sq_gettype(vm, idx) == OT_NULL) \
      { \
        return 0; \
      } \
      sq_getinstanceup(vm, idx, &up, 0); \
      return reinterpret_cast<TYPE *>(up); \
    } \
    template<> \
    inline TYPE &Get(TypeWrapper<TYPE &>, HSQUIRRELVM vm, SQInteger idx) \
    { \
      SQUserPointer up; \
      sq_getinstanceup(vm, idx, &up, 0); \
      return *reinterpret_cast<TYPE *>(up); \
    } \
    template<> \
    inline const TYPE *Get(TypeWrapper<const TYPE *>, HSQUIRRELVM vm, SQInteger idx) \
    { \
      SQUserPointer up; \
      if (sq_gettype(vm, idx) == OT_NULL) \
      { \
        return 0; \
      } \
      sq_getinstanceup(vm, idx, &up, 0); \
      return reinterpret_cast<const TYPE *>(up); \
    } \
    template<> \
    inline const TYPE &Get(TypeWrapper<const TYPE &>, HSQUIRRELVM vm, SQInteger idx) \
    { \
      SQUserPointer up; \
      sq_getinstanceup(vm, idx, &up, 0); \
      return *reinterpret_cast<const TYPE *>(up); \
    } \
  }

/// \brief Declares
#define DECLARE_INSTANCE_TYPE_BASE2(TYPE, NAME) \
  DECLARE_INSTANCE_TYPE_BASE(TYPE, NAME) \
  namespace sqp \
  { \
  template<> \
  inline bool Push(HSQUIRRELVM vm, TYPE *value) \
    { \
      if (value != 0) \
      { \
        ClassType<TYPE> *typeinfo = ClassType<TYPE>::Get(); \
        return CreateNativeClassInstance(vm, typeinfo->GetNamespace(), TypeInfo<TYPE>().m_typeName, value, 0); \
      } \
      sq_pushnull(vm); \
      return true; \
    } \
    template<> \
    inline bool Push(HSQUIRRELVM vm, const TYPE *value) \
    { \
      if (value != 0) \
      { \
        ClassType<TYPE> *typeinfo = ClassType<TYPE>::Get(); \
        return CreateNativeClassInstance(vm, typeinfo->GetNamespace(), TypeInfo<TYPE>().m_typeName, const_cast<TYPE *>(value), 0); \
      } \
      sq_pushnull(vm); \
      return true; \
    } \
  }

/// \brief blah
#define DECLARE_INSTANCE_TYPE_NAME(TYPE, NAME) \
  DECLARE_INSTANCE_TYPE_BASE2(TYPE, NAME) \
  namespace sqp \
  { \
    template<> \
    inline bool Push(HSQUIRRELVM vm, TYPE &value) \
    { \
      void *memory = sq_malloc(sizeof(TYPE)); \
      TYPE *copy = new (memory) TYPE(value); \
      ClassType<TYPE> *typeinfo = ClassType<TYPE>::Get(); \
      SQRELEASEHOOK hook = typeinfo->GetReleaseHook(); \
      if (CreateNativeClassInstance(vm, typeinfo->GetNamespace(), TypeInfo<TYPE>().m_typeName, copy, hook)) \
      { \
        return true; \
      } \
      if (hook != 0) \
      { \
        hook(memory, sizeof(TYPE)); \
      } \
      return false; \
    } \
    template<> \
    inline bool Push(HSQUIRRELVM vm, const TYPE &value) \
    { \
      void *memory = sq_malloc(sizeof(TYPE)); \
      TYPE *copy = new (memory) TYPE(value); \
      ClassType<TYPE> *typeinfo = ClassType<TYPE>::Get(); \
      SQRELEASEHOOK hook = typeinfo->GetReleaseHook(); \
      if (CreateNativeClassInstance(vm, typeinfo->GetNamespace(), TypeInfo<TYPE>().m_typeName, copy, hook)) \
      { \
        return true; \
      } \
      if (hook != 0) \
      { \
        hook(memory, sizeof(TYPE)); \
      } \
      return false; \
    } \
  }

/// \brief balh
#define DECLARE_NON_COPYABLE_INSTANCE_TYPE_NAME(TYPE, NAME) \
  namespace sqp \
  { \
    template<> \
    struct IsCopyable<TYPE> \
    { \
      enum \
      { \
        value = false \
      }; \
    }; \
  } \
  DECLARE_INSTANCE_TYPE_BASE2(TYPE, NAME) \
  namespace sqp \
  { \
    template<> \
    inline bool Push(HSQUIRRELVM SQUIRREL_UNUSED(vm), TYPE &SQUIRREL_UNUSED(value)) \
    { \
      return false; \
    } \
    template<> \
    inline bool Push(HSQUIRRELVM SQUIRREL_UNUSED(vm), const TYPE &SQUIRREL_UNUSED(value)) \
    { \
      return false; \
    } \
  }

/// \brief declare
#define DECLARE_CUSTOM_INSTANCE_TYPE_NAME(TYPE, NAME) \
  DECLARE_INSTANCE_TYPE_BASE(TYPE, NAME) \
  namespace sqp \
  { \
    template<> \
    bool Push(HSQUIRRELVM vm, TYPE *value); \
    template<> \
    bool Push(HSQUIRRELVM vm, TYPE &value); \
    template<> \
    bool Push(HSQUIRRELVM vm, const TYPE *value); \
    template<> \
    bool Push(HSQUIRRELVM vm, const TYPE &value); \
  }

/// \brief
#define DECLARE_INSTANCE_TYPE(TYPE) \
  DECLARE_INSTANCE_TYPE_NAME(TYPE, TYPE)

/// \brief 
#define DECLARE_NON_COPYABLE_INSTANCE_TYPE(TYPE) \
  DECLARE_NON_COPYABLE_INSTANCE_TYPE_NAME(TYPE, TYPE) \

/// \brief Declare a type without specialising sqp#Push, the types name will be TYPE.
/// \note sqp#Push must be manually defined elsewhere.
#define DECLARE_CUSTOM_INSTANCE_TYPE(TYPE) \
  DECLARE_CUSTOM_INSTANCE_TYPE_NAME(TYPE, TYPE)

/// \brief Declares an enum type by providing specialisations to sqp#Push, sqp#Match and sqp#Get.
#define DECLARE_ENUM_TYPE(TYPE) \
  namespace sqp \
  { \
    inline bool Push(HSQUIRRELVM vm, TYPE value) \
    { \
      sq_pushinteger(vm, static_cast<SQInteger>(value)); \
      return true; \
    } \
    inline bool Match(TypeWrapper<TYPE>, HSQUIRRELVM vm, SQInteger index) \
    { \
      return sq_gettype(vm, index) == OT_INTEGER; \
    } \
    inline TYPE Get(TypeWrapper<TYPE>, HSQUIRRELVM vm, SQInteger index) \
    { \
      SQInteger value = 0; \
      sq_getinteger(vm, index, &value); \
      return static_cast<TYPE>(value); \
    } \
  }

/// @}

}

#endif
