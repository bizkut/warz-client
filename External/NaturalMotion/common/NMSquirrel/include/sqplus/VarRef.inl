#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_VARREF_INL_
#define _SQPLUS_VARREF_INL_
//----------------------------------------------------------------------------------------------------------------------
/// \file VarRef.h
/// Based on the VarRef class written by John Schultz and others in sqp.
/// \author John Schultz
//----------------------------------------------------------------------------------------------------------------------

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
inline VarRef::VarRef(
  void *offsetOrAddress,
  ScriptVarType type,
  ClassTypeBase *instanceType,
  ClassTypeBase *varType,
  int flags)
: m_offsetOrAddress(offsetOrAddress), m_type(type), m_instanceType(instanceType), m_varType(varType), m_flags(flags)
{
}

//----------------------------------------------------------------------------------------------------------------------
inline VarRef::VarRef()
: m_offsetOrAddress(0), m_type(VAR_TYPE_NONE), m_instanceType(0), m_varType(0), m_flags(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void VarRef::CreateTableSetGetHandlers(SquirrelVM& vm, SquirrelObject& object)
{
  SquirrelObject delegate = object.GetDelegate();
  if (!delegate.Exists(_SC("_set")))
  {
    if (delegate.IsNull())
    {
      delegate = vm.CreateTable();
    }
    vm.CreateFunction(delegate, &VarRef::SetVarFunc, _SC("_set"), 3, _SC("t|y s n|b|s|x"));
    vm.CreateFunction(delegate, &VarRef::GetVarFunc, _SC("_get"), 2, _SC("t|y s"));
    object.SetDelegate(delegate);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void VarRef::CreateInstanceSetGetHandlers(SquirrelVM& vm, SquirrelObject& object)
{
  if (!object.Exists(_SC("_set")))
  {
    vm.CreateFunction(object, &VarRef::SetInstanceVarFunc, _SC("_set"), 3, _SC("x s n|b|s|x"));
    vm.CreateFunction(object, &VarRef::GetInstanceVarFunc, _SC("_get"), 2, _SC("x s"));
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline SQUserPointer VarRef::Create(SquirrelObject& object, const SQChar *name)
{
  SQUserPointer up = 0;
  SQChar nameTag[256];
  GetNameTag(nameTag, 256, name);
  if (!object.GetUserData(nameTag, &up))
  {
    object.NewUserData(nameTag, sizeof(VarRef));
    if (!object.GetUserData(nameTag, &up))
    {
      // couldn't create user data.
      return 0;
    }
  }
  return up;
}

}

#endif
