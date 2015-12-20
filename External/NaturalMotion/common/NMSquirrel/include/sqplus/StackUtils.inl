#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_STACKUTILS_INL_
#define _SQPLUS_STACKUTILS_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, bool value)
{
  sq_pushbool(vm, value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, char value)
{
  sq_pushinteger(vm, static_cast<SQInteger>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, unsigned char value)
{
  sq_pushinteger(vm, static_cast<SQInteger>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, short value)
{
  sq_pushinteger(vm, static_cast<SQInteger>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, unsigned short value)
{
  sq_pushinteger(vm, static_cast<SQInteger>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, int value)
{
  sq_pushinteger(vm, static_cast<SQInteger>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, unsigned int value)
{
  sq_pushinteger(vm, static_cast<SQInteger>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, long value)
{
  sq_pushinteger(vm, static_cast<SQInteger>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, unsigned long value)
{
  sq_pushinteger(vm, static_cast<SQInteger>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, float value)
{
  sq_pushfloat(vm, static_cast<SQFloat>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, double value)
{
  sq_pushfloat(vm, static_cast<SQFloat>(value));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, const SQChar *value)
{
  sq_pushstring(vm, value, -1);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, SQFUNCTION value)
{
  // c-style cast as reinterpret is forbidden
  sq_pushuserpointer(vm, (void*)value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Push(HSQUIRRELVM vm, SquirrelObject& value)
{
  sq_pushobject(vm, value.GetHandle());
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<bool>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_BOOL;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<char>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_INTEGER;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<unsigned char>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_INTEGER;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<short>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_INTEGER;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<unsigned short>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_INTEGER;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<int>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_INTEGER;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<unsigned int>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_INTEGER;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<long>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_INTEGER;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<unsigned long>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_INTEGER;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<float>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_FLOAT;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<double>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_FLOAT;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<const SQChar*>, HSQUIRRELVM vm, SQInteger index)
{
  return sq_gettype(vm, index) == OT_STRING;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Match(TypeWrapper<SquirrelObject>, HSQUIRRELVM, SQInteger)
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Get(TypeWrapper<bool>, HSQUIRRELVM vm, SQInteger index)
{
  SQBool value;
  sq_getbool(vm, index, &value);
  return value != 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline char Get(TypeWrapper<char>, HSQUIRRELVM vm, SQInteger index)
{
  SQInteger value = 0;
  sq_getinteger(vm, index, &value);
  return static_cast<char>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline unsigned char Get(TypeWrapper<unsigned char>, HSQUIRRELVM vm, SQInteger index)
{
  SQInteger value = 0;
  sq_getinteger(vm, index, &value);
  return static_cast<unsigned char>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline short Get(TypeWrapper<short>, HSQUIRRELVM vm, SQInteger index)
{
  SQInteger value = 0;
  sq_getinteger(vm, index, &value);
  return static_cast<short>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline unsigned short Get(TypeWrapper<unsigned short>, HSQUIRRELVM vm, SQInteger index)
{
  SQInteger value = 0;
  sq_getinteger(vm, index, &value);
  return static_cast<unsigned short>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline int Get(TypeWrapper<int>, HSQUIRRELVM vm, SQInteger index)
{
  SQInteger value;
  sq_getinteger(vm, index, &value);
  return static_cast<int>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline unsigned int Get(TypeWrapper<unsigned int>, HSQUIRRELVM vm, SQInteger index)
{
  SQInteger value;
  sq_getinteger(vm, index, &value);
  return static_cast<unsigned int>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline long Get(TypeWrapper<long>, HSQUIRRELVM vm, SQInteger index)
{
  SQInteger value;
  sq_getinteger(vm, index, &value);
  return static_cast<long>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline unsigned long Get(TypeWrapper<unsigned long>, HSQUIRRELVM vm, SQInteger index)
{
  SQInteger value;
  sq_getinteger(vm, index, &value);
  return static_cast<unsigned long>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline float Get(TypeWrapper<float>, HSQUIRRELVM vm, SQInteger index)
{
  SQFloat value;
  sq_getfloat(vm, index, &value);
  return static_cast<float>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline double Get(TypeWrapper<double>, HSQUIRRELVM vm, SQInteger index)
{
  SQFloat value;
  sq_getfloat(vm, index, &value);
  return static_cast<double>(value);
}

//----------------------------------------------------------------------------------------------------------------------
inline const SQChar *Get(TypeWrapper<const SQChar*>, HSQUIRRELVM vm, SQInteger index)
{
  const SQChar *value;
  sq_getstring(vm, index, &value);
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
inline SquirrelObject Get(TypeWrapper<SquirrelObject>, HSQUIRRELVM vm, SQInteger index)
{
  SquirrelObject object(vm);
  
  object.AttachToStackObject(index);

  return object;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
inline RT GetRet(TypeWrapper<RT>, HSQUIRRELVM vm, SQInteger index, SquirrelObject *ref)
{
  // stops the value being freed when the call to sq_pop is made
  ref->AttachToStackObject(vm, index);
  RT ret = Get(TypeWrapper<RT>(), vm, index);
  sq_pop(vm, 2);
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline void GetRet<void>(
  TypeWrapper<void>,
  HSQUIRRELVM vm,
  SQInteger SQUIRREL_UNUSED(index),
  SquirrelObject *SQUIRREL_UNUSED(ref))
{
  sq_pop(vm, 2);
}

}

#endif
