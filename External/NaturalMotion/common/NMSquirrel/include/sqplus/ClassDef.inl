#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_CLASSDEF_INL_
#define _SQPLUS_CLASSDEF_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
ClassDefBase<TypeClass, BaseClass>::ClassDefBase(
  SquirrelVM& vm,
  const SQChar *name,
  const SQChar *base,
  SQRELEASEHOOK hook)
: m_vm(vm), m_name(name), m_base(base)
{
  InitBase(TypeWrapper<BaseClass>());
  InitReleaseHook(hook);

  // look for the class namespace at the top position of the stack,
  // if it can't be found use the vm root table.
  SquirrelObject namespaceTable;
  namespaceTable.AttachToStackObject(vm.GetVMPtr(), -1);
  SQObjectType type = namespaceTable.GetType();
  if (type != OT_TABLE && type != OT_CLASS)
  {
    namespaceTable = vm.GetRootTable();
  }
  InitNamespace(namespaceTable);

  CheckInitDefaultNames();
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
template<typename FuncType>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::Func(
  FuncType func,
  const SQChar *name)
{
  m_vm.Push(m_class);
  m_vm.Push(name);
  
  // Also pushed on stack.
  unsigned char *up = static_cast<unsigned char*>(m_vm.NewUserData(sizeof(func)));
  memcpy(up, &func, sizeof(func));
  m_vm.NewClosure(&DirectCallInstanceMemberFunc<TypeClass, FuncType>::Dispatch, 1);

  m_vm.NewSlot(-3, false);
  m_vm.PopTop();
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
template<typename FuncType>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::StaticFunc(
  FuncType func,
  const SQChar *name)
{
  m_vm.Push(m_class);
  m_vm.Push(name);

  sq_pushdirectclosure(m_vm.GetVMPtr(), func, 0);

  m_vm.NewSlot(-3, true);
  m_vm.PopTop();
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::FuncVarArgs(
  HSQMETHOD func,
  const SQChar *name,
  SQInteger paramCount,
  const SQChar *typemask)
{
  m_vm.Push(m_class);
  m_vm.Push(name);

  // Also pushed on stack.
  unsigned char *up = static_cast<unsigned char*>(m_vm.NewUserData(sizeof(func)));
  memcpy(up, &func, sizeof(func));
  m_vm.NewClosure(
    &DirectCallInstanceMemberFuncVarArgs<TypeClass, HSQMETHOD>::Dispatch,
    1);

  if (typemask)
  {
    SQChar tm[64];
    scsprintf(tm, _SC("x%s"), typemask);

    if (paramCount != SQ_MATCHTYPEMASKSTRING)
    {
      if (paramCount >= 0)
      {
        ++paramCount;
      }
      else if (paramCount < 0)
      {
        --paramCount;
      }
    }

    sq_setparamscheck(m_vm.GetVMPtr(), paramCount, tm);
  }
  else
  {
    sq_setparamscheck(m_vm.GetVMPtr(), -1, _SC("x"));
  }

  // For debugging purposes
  m_vm.SetNativeClosureName(-1, name);
  m_vm.NewSlot(-3, false);
  m_vm.PopTop();
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::SquirrelFunc(
  SQFUNCTION func,
  const SQChar *name,
  bool isstatic)
{
  m_vm.CreateFunction(m_class, func, name, 0, 0, isstatic);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::SquirrelFunc(
  SQFUNCTION func,
  const SQChar *name,
  SQInteger nparamscheck,
  const SQChar *typemask,
  bool isstatic)
{
  m_vm.CreateFunction(m_class, func, name, nparamscheck, typemask, isstatic);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
template<typename FuncType>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::SingletonFunc(
  TypeClass *instance,
  FuncType func,
  const SQChar *name)
{
  // is instance is null then any calls to the function will crash the app
  squirrel_assert(instance != 0);
  m_vm.Push(m_class);
  m_vm.Push(name);

  // push the class instance and the function pointer as an up value to be used when the function is called.
  unsigned char *up = static_cast<unsigned char*>(m_vm.NewUserData(sizeof(TypeClass*) + sizeof(func)));
  const SQUserPointer instancePointer = static_cast<const SQUserPointer>(instance);
  memcpy(up, &instancePointer, sizeof(TypeClass*));
  memcpy(up + sizeof(TypeClass*), &func, sizeof(func));
  m_vm.NewClosure(&DirectCallMemberFunc<TypeClass, FuncType>::Dispatch, 1);

  // for debugging purposes
  m_vm.SetNativeClosureName(-1, name);
  m_vm.NewSlot(-3, true);
  m_vm.PopTop();
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::SingletonFuncVarArgs(
  TypeClass *instance,
  HSQMETHOD func,
  const SQChar *name)
{
  // is instance is null then any calls to the function will crash the app
  squirrel_assert(instance != 0);
  m_vm.Push(m_class);
  m_vm.Push(name);

  // push the class instance and the function pointer as an up value to be used when the function is called.
  unsigned char *up = static_cast<unsigned char*>(m_vm.NewUserData(sizeof(TypeClass*) + sizeof(func)));
  const SQUserPointer instancePointer = static_cast<const SQUserPointer>(&instance);
  memcpy(up, &instancePointer, sizeof(TypeClass*));
  memcpy(up + sizeof(TypeClass*), &func, sizeof(func));
  m_vm.NewClosure(&DirectCallMemberFuncVarArgs<TypeClass>::Dispatch, 1);

  // for debugging purposes
  m_vm.SetNativeClosureName(-1, name);
  m_vm.NewSlot(-3, true);
  m_vm.PopTop();
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
template<typename VarType>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::Var(
  VarType TypeClass:: *variable,
  const SQChar *name)
{
  RegisterInstanceVariable<TypeClass>(m_vm, m_class, variable, name);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
template<typename VarType>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::VarAsUserPointer(
  VarType TypeClass:: *var,
  const SQChar *name)
{
  RegisterInstanceVariable<TypeClass>(m_vm, m_class, reinterpret_cast<SQUserPointer TypeClass::*>(var), name);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
template<typename VarType>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::StaticVar(
  VarType TypeClass:: *var,
  const SQChar *name)
{
  RegisterInstanceVariable<TypeClass>(m_vm, m_class, var, name, VarRef::kStaticVar);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
ClassDefBase<TypeClass, BaseClass>& ClassDefBase<TypeClass, BaseClass>::EnumVar(SQInteger value, const SQChar *name)
{
  SQInteger top = sq_gettop(m_vm.GetVMPtr());
  m_vm.Push(m_class);
  sq_pushstring(m_vm.GetVMPtr(), name, -1);
  sq_pushinteger(m_vm.GetVMPtr(), value);
  sq_newslot(m_vm.GetVMPtr(), -3, SQTrue);
  sq_settop(m_vm.GetVMPtr(), top);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
template<typename Base>
inline void ClassDefBase<TypeClass, BaseClass>::InitBase(TypeWrapper<Base>)
{
  ClassType<TypeClass>::Get()->SetBase(TypeWrapper<Base>());
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
inline void ClassDefBase<TypeClass, BaseClass>::InitBase(TypeWrapper<NoBaseClass>)
{
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
inline void ClassDefBase<TypeClass, BaseClass>::InitReleaseHook(SQRELEASEHOOK hook)
{
  ClassType<TypeClass>::Get()->SetReleaseHook(hook);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
inline void ClassDefBase<TypeClass, BaseClass>::InitNamespace(const SquirrelObject &namespaceTable)
{
  ClassType<TypeClass>::Get()->m_namespace = namespaceTable.GetHandle();
  sq_addref(m_vm.GetVMPtr(), &ClassType<TypeClass>::Get()->m_namespace);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
inline void ClassDefBase<TypeClass, BaseClass>::CheckInitDefaultNames()
{
  if (m_name == 0)
  {
    m_name = TypeInfo<TypeClass>().m_typeName;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
inline ClassDef<TypeClass, BaseClass>::ClassDef(
  SquirrelVM& vm,
  const SQChar *name,
  const SQChar *base,
  SQRELEASEHOOK hook)
: ClassDefBase<TypeClass, BaseClass>(vm, name, base, hook)
{
  m_class = RegisterClassType<TypeClass>(vm, name, base);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename TypeClass, typename BaseClass>
inline ClassDefNoConstructor<TypeClass, BaseClass>::ClassDefNoConstructor(
  SquirrelVM& vm,
  const SQChar *name,
  const SQChar *base)
: ClassDefBase<TypeClass, BaseClass>(vm, name, base, 0)
{
  m_class = RegisterClassTypeNoConstructor<TypeClass>(vm, name, base);
}

}

#endif
