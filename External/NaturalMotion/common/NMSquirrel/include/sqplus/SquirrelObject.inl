#ifndef _SQPLUS_SQUIRRELOBJECT_INL_
#define _SQPLUS_SQUIRRELOBJECT_INL_

#include "sqplus/ClassType.h"

namespace sqp
{

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelObject::AttachToStackObject(HSQUIRRELVM vm, SQInteger idx, SQRESULT *pResult)
{
  m_vm = vm;
  AttachToStackObject(idx, pResult);
}

//----------------------------------------------------------------------------------------------------------------------
inline HSQUIRRELVM SquirrelObject::GetVMPtr() const
{
  return m_vm;
}

//----------------------------------------------------------------------------------------------------------------------
inline HSQOBJECT SquirrelObject::GetHandle() const
{
  return m_obj;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool SquirrelObject::IsNull() const
{
  return sq_isnull(m_obj);
}

//----------------------------------------------------------------------------------------------------------------------
inline bool SquirrelObject::IsNumber() const
{
  return sq_isfloat(m_obj) || sq_isinteger(m_obj);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline bool SquirrelObject::IsInstanceType() const
{
  SQUserPointer up;
  return sq_getobjinstanceup(&m_obj, &up, sqp::ClassType<T>::Get()) == SQ_OK;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQObjectType SquirrelObject::GetType() const
{
  return m_obj._type;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool SquirrelObject::ToBool()
{
  return sq_objtobool(&m_obj) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger SquirrelObject::ToInteger()
{
  return sq_objtointeger(&m_obj);
}

//----------------------------------------------------------------------------------------------------------------------
inline SQFloat SquirrelObject::ToFloat()
{
  return sq_objtofloat(&m_obj);
}

//----------------------------------------------------------------------------------------------------------------------
inline const SQChar *SquirrelObject::ToString()
{
  return sq_objtostring(&m_obj);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
T *SquirrelObject::ToInstance()
{
  SQUserPointer up = 0;
  sq_getobjinstanceup(m_obj, &up, sqp::ClassType<T>::Get());
  return reinterpret_cast<T*>(up);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
T *SquirrelObject::ToInstanceNoTypeCheck()
{
  SQUserPointer up = 0;
  sq_getobjinstanceup(&m_obj, &up, 0);
  return reinterpret_cast<T*>(up);
}

//----------------------------------------------------------------------------------------------------------------------
inline bool SquirrelObject::ArrayResize(SQInteger size)
{
  sq_pushobject(m_vm, m_obj);
  bool result = SQ_SUCCEEDED(sq_arrayresize(m_vm, -1, size));
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool SquirrelObject::ArrayReverse()
{
  sq_pushobject(m_vm, m_obj);
  bool result = SQ_SUCCEEDED(sq_arrayreverse(m_vm, -1));
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool SquirrelObject::SetValue(SQInteger key, const T *value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);

  Push(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool SquirrelObject::SetValue(SQInteger key, const T &value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);

  Push(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool SquirrelObject::SetValue(const SQChar *key, const T *value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);

  Push(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool SquirrelObject::SetValue(const SQChar *key, const T &value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);

  Push(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool SquirrelObject::SetValue(const SquirrelObject &key, const T *value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);

  Push(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool SquirrelObject::SetValue(const SquirrelObject &key, const T &value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);

  Push(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t SquirrelObject::GetInteger32(SQInteger key) const
{
  return static_cast<int32_t>(GetInteger(key));
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t SquirrelObject::GetInteger32(const SQChar *key) const
{
  return static_cast<int32_t>(GetInteger(key));
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t SquirrelObject::GetInteger32(const SquirrelObject& key) const
{
  return static_cast<int32_t>(GetInteger(key));
}

}

#endif // _SQPLUS_SQUIRRELOBJECT_INL_
