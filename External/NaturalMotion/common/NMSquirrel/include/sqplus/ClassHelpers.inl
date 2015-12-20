#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_CLASSHELPERS_INL_
#define _SQPLUS_CLASSHELPERS_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
SQInteger DefaultReleaseHook(SQUserPointer up, SQInteger size)
{
  if (up)
  {
    T *instance = static_cast<T*>(up);
    (void)instance;
    instance->~T();
    sq_free(up, size);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
SQInteger TypeOf(HSQUIRRELVM v)
{
  sq_pushstring(v, TypeInfo<T>().m_typeName, -1);
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline SQInteger PostConstruct(HSQUIRRELVM vm, T *newClass)
{
  sq_setinstanceup(vm, 1, newClass);
  sq_setreleasehook(vm, 1, ClassType<T>::Get()->GetReleaseHook());
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
SQInteger DefaultConstructor(HSQUIRRELVM vm)
{
  StackHandler stack(vm);
  if (stack.GetParamCount() == 2)
  {
    T *original = stack.GetInstance<T>(2);
    if (original)
    {
      void *memory = sq_malloc(sizeof(T));
      T *copy = new (memory) T(*original);
      return PostConstruct<T>(vm, copy);
    }
    return stack.ThrowParamError(2, sqp::TypeInfo<T>().m_typeName);
  }
  void *memory = sq_malloc(sizeof(T));
  T *instance = new (memory) T();
  return PostConstruct<T>(vm, instance);
}

}

#endif
