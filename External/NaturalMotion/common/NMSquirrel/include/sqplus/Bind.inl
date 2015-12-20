#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_BIND_INL_
#define _SQPLUS_BIND_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline bool BindVariable(SquirrelVM &vm, SquirrelObject &object, T *variable, const SQChar *name, int flags)
{
  SQUserPointer up = VarRef::Create(object, name);
  if (up)
  {
    VarRef *ref = new (up) VarRef(vm, variable, TypeInfo<T>(), 0, ClassType<T>::type(), flags);
    VarRef::CreateInstanceSetGetHandlers(vm, object);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename InstanceType, typename VarType>
inline bool RegisterInstanceVariable(
  SquirrelVM &vm,
  SquirrelObject &object,
  VarType InstanceType:: *variable,
  const SQChar *name,
  int flags)
{
  SQUserPointer up = VarRef::Create(object, name);
  if (up)
  {
    // c++ won't let you cast from a member variable pointer to a void *so
    // this struct is used to help cast it. By casting the struct to a void**
    // and then dereferencing back to void *the variable pointer is effectively
    // cast to a void*. This uses the fact that the CastHelper::variable is the first
    // and only member of the struct so a pointer to the helper will also point to
    // helper::variable.
    struct CastHelper
    {
      VarType InstanceType:: *variable;
    };
    CastHelper helper;
    helper.variable = variable;

    VarRef *ref = new (up) VarRef(
      *reinterpret_cast<void**>(&helper),
      TypeInfo<VarType>(),
      ClassType<InstanceType>::Get(),
      ClassType<VarType>::Get(),
      flags);
    (void)ref;
    VarRef::CreateInstanceSetGetHandlers(vm, object);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
SquirrelObject RegisterClassType(SquirrelVM &vm, const SQChar *className, const SQChar *baseName)
{
  SQRESULT res;
  SquirrelObject newClass = vm.CreateClass(className, baseName, ClassType<T>::Get(), &res);
  if (SQ_SUCCEEDED(res))
  {
    vm.CreateFunction(newClass, &DefaultConstructor<T>, _SC("constructor"), -1, _SC("xx"));
    vm.CreateFunction(newClass, &TypeOf<T>, _SC("_typeof"), 1, _SC("x"));
  }
  return newClass;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
SquirrelObject RegisterClassTypeNoConstructor(SquirrelVM &vm, const SQChar *className, const SQChar *baseName)
{
  SQRESULT res;
  SquirrelObject newClass = vm.CreateClass(className, baseName, ClassType<T>::Get(), &res);
  if (SQ_SUCCEEDED(res))
  {
    vm.CreateFunction(newClass, &TypeOf<T>, _SC("_typeof"), 1, _SC("x"));
  }
  return newClass;
}

}

#endif
