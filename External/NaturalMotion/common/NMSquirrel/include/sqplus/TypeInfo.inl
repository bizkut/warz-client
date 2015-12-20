#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_TYPEINFO_INL_
#define _SQPLUS_TYPEINFO_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
inline const SQChar *RawTypeToTypeName(SQObjectType type)
{
	switch(_RAW_TYPE(type))
	{
	case _RT_NULL:return _SC("null");
	case _RT_INTEGER:return _SC("integer");
	case _RT_FLOAT:return _SC("float");
	case _RT_BOOL:return _SC("bool");
	case _RT_STRING:return _SC("string");
	case _RT_TABLE:return _SC("table");
	case _RT_ARRAY:return _SC("array");
	case _RT_GENERATOR:return _SC("generator");
	case _RT_CLOSURE:
	case _RT_NATIVECLOSURE:
		return _SC("function");
	case _RT_USERDATA:
	case _RT_USERPOINTER:
		return _SC("userdata");
	case _RT_THREAD: return _SC("thread");
	case _RT_FUNCPROTO: return _SC("function");
	case _RT_CLASS: return _SC("class");
	case _RT_INSTANCE: return _SC("instance");
	case _RT_WEAKREF: return _SC("weakref");
	default:
		return _SC("");
	}
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<bool>::TypeInfo()
: m_typeName(_SC("bool"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<bool>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<char>::TypeInfo()
: m_typeName(_SC("char"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<char>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<unsigned char>::TypeInfo()
: m_typeName(_SC("uchar"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<unsigned char>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<short>::TypeInfo()
: m_typeName(_SC("short"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<short>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<unsigned short>::TypeInfo()
: m_typeName(_SC("ushort"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<unsigned short>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<int32_t>::TypeInfo()
: m_typeName(_SC("int32_t"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<int32_t>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<uint32_t>::TypeInfo()
: m_typeName(_SC("uint32_t"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<uint32_t>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<int64_t>::TypeInfo()
: m_typeName(_SC("int64_t"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<int64_t>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<uint64_t>::TypeInfo()
: m_typeName(_SC("uint64_t"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<uint64_t>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<float>::TypeInfo()
: m_typeName(_SC("float"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<float>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<double>::TypeInfo()
: m_typeName(_SC("double"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<double>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<const SQChar*>::TypeInfo()
: m_typeName(_SC("string"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<const SQChar*>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<SQUserPointer>::TypeInfo()
: m_typeName(_SC("userpointer"))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline TypeInfo<SQUserPointer>::operator ScriptVarType() const
{
  return static_cast<ScriptVarType>(TypeID);
}

}

#endif
