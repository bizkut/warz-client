#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_TYPEINFO_H_
#define _SQPLUS_TYPEINFO_H_
//----------------------------------------------------------------------------------------------------------------------
/// \file TypeInfo.h
/// Based on the TypeInfo classes written by John Schultz and others in sqp.
/// \author John Schultz
//----------------------------------------------------------------------------------------------------------------------

#include "squirrel/squirrel.h"
#include "sqplus/BaseHeader.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief Converts from a SQObjectType to a string.
/// \code
/// // prints out "string"
/// scprintf(RawTypeToTypeName(OT_STRING));
/// \endcode
inline const SQChar *RawTypeToTypeName(SQObjectType type);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Template class for gaining info about a bound script type. Specialisation of TypeInfo
/// is done automatically when a type is declared using #DECLARE_INSTANCE_TYPEINFO or any of the other
/// macros that call it.
template<typename T>
struct TypeInfo
{
  enum
  {
    TypeID = VAR_TYPE_NONE,
    Size = sizeof(T),
    TypeMask = '?',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<bool>
{
  enum
  {
    TypeID = VAR_TYPE_BOOL,
    Size = sizeof(bool),
    TypeMask = 'b',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<char>
{
  enum
  {
    TypeID = VAR_TYPE_CHAR,
    Size = sizeof(char),
    TypeMask = 'i',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<unsigned char>
{
  enum
  {
    TypeID = VAR_TYPE_UCHAR,
    Size = sizeof(unsigned char),
    TypeMask = 'i',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<short>
{
  enum
  {
    TypeID = VAR_TYPE_SHORT,
    Size = sizeof(short),
    TypeMask = 'i',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<unsigned short>
{
  enum
  {
    TypeID = VAR_TYPE_USHORT,
    Size = sizeof(unsigned short),
    TypeMask = 'i',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<int32_t>
{
  enum
  {
    TypeID = VAR_TYPE_INT32,
    Size = sizeof(int32_t),
    TypeMask = 'n',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<uint32_t>
{
  enum
  {
    TypeID = VAR_TYPE_UINT32,
    Size = sizeof(uint32_t),
    TypeMask = 'n',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<int64_t>
{
  enum
  {
    TypeID = VAR_TYPE_INT64,
    Size = sizeof(int64_t),
    TypeMask = 'n',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<uint64_t>
{
  enum
  {
    TypeID = VAR_TYPE_UINT64,
    Size = sizeof(uint64_t),
    TypeMask = 'n',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<float>
{
  enum
  {
    TypeID = VAR_TYPE_FLOAT,
    Size = sizeof(float),
    TypeMask = 'n',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<double>
{
  enum
  {
    TypeID = VAR_TYPE_DOUBLE,
    Size = sizeof(double),
    TypeMask = 'n',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<const SQChar*>
{
  enum
  {
    TypeID = VAR_TYPE_STRING,
    Size = sizeof(const SQChar*),
    TypeMask = 's',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

//----------------------------------------------------------------------------------------------------------------------
template<>
struct TypeInfo<SQUserPointer>
{
  enum
  {
    TypeID = VAR_TYPE_USER_POINTER,
    Size = sizeof(SQUserPointer),
    TypeMask = 'p',
    IsInstance = SQFalse
  };

  const SQChar *m_typeName;

  TypeInfo();
  operator ScriptVarType() const;
};

}

#include "sqplus/TypeInfo.inl"

#endif
