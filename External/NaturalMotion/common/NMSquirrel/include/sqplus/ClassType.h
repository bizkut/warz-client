#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_CLASSTYPE_H_
#define _SQPLUS_CLASSTYPE_H_
//----------------------------------------------------------------------------------------------------------------------
/// \file ClassType.h
/// Based on the ClassType classes written by John Schultz and others in sqp.
/// \author John Schultz
//----------------------------------------------------------------------------------------------------------------------
#include "squirrel/squirrel.h"

#include "sqplus/BaseHeader.h"
#include "sqplus/Interface.h"
#include "sqplus/TypeInfo.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
typedef void (*CopyVarFunc)(void  *dst,void  *src);

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class SQPLUS_API ClassTypeBase
{
public:
  ClassTypeBase();
  virtual ~ClassTypeBase() {}

  /// \brief Many types cannot have offset, since "this" is the same for all base classes of 
  /// an instance. Detect this, to avoid sum up base offsets all the time.
  /// \note recursive function but will early out.
  bool MayHaveOffset();
  /// \brief Gets the offset to this class from the base class pointer
  /// \note recursive function
  size_t GetOffsetTo(ClassTypeBase *pbase);

  /// \brief Get the script name of the type
  const SQChar *GetTypeName() const;

  virtual CopyVarFunc GetCopyFunc() = 0;

public:
  /// \brief pointer to base class
  ClassTypeBase *m_pbase;
  /// \brief Name of type.
  const SQChar *m_name;
  /// \brief Adjustment of this pointer between this type and its base class.
  size_t m_offset;
  /// \brief Set to 0 for types that cannot possibly have offset.
  int m_mayHaveOffset;
  /// \brief Sets the release hook for the class.
  SQRELEASEHOOK m_releaseHook;
  /// \brief The table that the class belongs to.
  HSQOBJECT m_namespace;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief This level handles instance copying in different ways
//----------------------------------------------------------------------------------------------------------------------
template<typename T, bool copyable>
class ClassTypeCopyImpl;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Base class to do copying in ordinary C++ way
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class ClassTypeCopyImpl<T, true> : public ClassTypeBase
{
public:
  /// \brief this works for types with assignment operator
  static void copy(T *dst, T *src);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Base class to do copying with memcpy
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class ClassTypeCopyImpl<T, false> : public ClassTypeBase
{
public:
  /// \brief this works for raw data types, uses memcpy
  static void copy(T *dst, T *src);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Base class to do avoid copying altogether (void case)
//----------------------------------------------------------------------------------------------------------------------
template<>
class ClassTypeCopyImpl<void, true> : public ClassTypeBase
{
public:
  /// \brief no op
  static void copy(void *dst, void *src);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Base class to do avoid copying altogether (void case)
//----------------------------------------------------------------------------------------------------------------------
template<>
class ClassTypeCopyImpl<void, false> : public ClassTypeBase
{
public:
  /// \brief no op
  static void copy(void *dst, void *src);
};

/// \brief Used to query whether a type is copyable. To make a type non-copyable specialise this class
/// or use the macros #DECLARE_NON_COPYABLE_INSTANCE_TYPE_NAME or #DECLARE_NON_COPYABLE_INSTANCE_TYPE.
/// Copyable types will use the assignment operator to copy them, non-copyable types by default will use
/// memcpy and sizeof(T). To change the copy operation of a type specialise ClassTypeCopyImpl before
/// declaring the type.
template<typename T>
struct IsCopyable
{
  enum
  {
    value = true
  };
};

/// \brief Specialisation for void to make it non copyable.
template<>
struct IsCopyable<void>
{
  enum
  {
    value = false
  };
};

/// \brief Used to get info about types.
template<typename T>
class ClassType : public ClassTypeCopyImpl<T, IsCopyable<T>::value>
{
public:
  typedef ClassTypeCopyImpl<T, IsCopyable<T>::value> ClassTypeBase;
  
  // helps resolve unqualified names to members of the dependent base class.
  using ClassTypeBase::m_pbase;
  using ClassTypeBase::m_name;
  using ClassTypeBase::m_offset;
  using ClassTypeBase::m_releaseHook;
  using ClassTypeBase::m_namespace;

  ClassType();

#ifdef _MSC_VER
#pragma warning(push)
// nonstandard extension used: override specifier 'override'
#pragma warning(disable: 4481)
#endif

  /// \brief Override from ClassTypeBase
  CopyVarFunc GetCopyFunc() SQPLUS_OVERRIDE;

#ifdef _MSC_VER
#pragma warning(pop)
#endif

  /// \brief Sets the base class type for this type.
  /// \note Always use the instance returned by the static Get function to call this func.
  template<class BC>
  void SetBase(TypeWrapper<BC>);

  /// \brief Sets the release hook for the class.
  /// \note Always use the instance returned by the static Get function to call this func.
  void SetReleaseHook(SQRELEASEHOOK releaseHook);
  /// \brief Get the release hook for this class.
  /// \note Always use the instance returned by the static Get function to call this func.
  SQRELEASEHOOK GetReleaseHook() const;
  
  /// \brief Get the namespace that the class belongs to. This is required when trying to
  /// create the class via c/c++ the api.
  /// \code
  /// // example of creating a class
  /// HSQUIRRELVM vm;
  /// MyClass *instance;
  /// CreateNativeClassInstance(vm, ClassType<MyClass>::Get()->GetNamespace(), _SC("MyClass"), instance, 0);
  /// \endcode
  /// \note Always use the instance returned by the static Get function to call this func.
  HSQOBJECT GetNamespace() const;

  /// \brief Gets an instance of ClassType<T> to save many being created.
  static ClassType<T> *Get();
};

}

#include "sqplus/ClassType.inl"

#endif
