//---------------------------------------------------------------------------------------------------
/// \file SquirrelVM.inl
/// Based on the SquirrelVM class written by Alberto Demichelis in his DXSquirrel example bindings
/// and the further work of John Schultz in sqp.
/// \author Alberto Demichelis
//---------------------------------------------------------------------------------------------------
#ifndef _SQPLUS_SQUIRREL_VM_INL_
#define _SQPLUS_SQUIRREL_VM_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
inline bool SquirrelVM::Initialise(SQInteger size)
{
  if (m_vm == 0)
  {
    m_vm = sq_open(size);
    return m_vm != 0;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool SquirrelVM::IsInitialised() const
{
  return m_vm != 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline HSQUIRRELVM SquirrelVM::GetVMPtr() const
{
  return m_vm;
}

//----------------------------------------------------------------------------------------------------------------------
inline SquirrelObject SquirrelVM::CreateFunction(
  SquirrelObject& object,
  SQFUNCTION func,
  const SQChar *funcName,
  SQInteger nparams,
  const SQChar *typeMask,
  bool isStatic)
{
  Push(object);
  SquirrelObject result = CreateFunction(func, funcName, nparams, typeMask, isStatic);
  Pop(1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename Func>
SquirrelObject SquirrelVM::CreateFunction(
  Func func,
  const SQChar *name,
  bool isstatic)
{
  SquirrelObject sqfunc(m_vm);
  sq_pushstring(m_vm, name, -1);
  sq_pushdirectclosure(GetVMPtr(), func, 0);
  sqfunc.AttachToStackObject(-1);
  NewSlot(-3, isstatic);
  return sqfunc;
}

//---------------------------------------------------------------------------------------------------
inline SquirrelObject SquirrelVM::CreateGlobalFunction(
  SQFUNCTION func,
  const SQChar *funcName,
  SQInteger nparams,
  const SQChar *typeMask)
{
  PushRootTable();
  SquirrelObject result = CreateFunction(func, funcName, nparams, typeMask);
  Pop(1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::NewClosure(SQFUNCTION func, SQUnsignedInteger nfreevars)
{
  sq_newclosure(m_vm, func, nfreevars);
}

//----------------------------------------------------------------------------------------------------------------------
inline SQUserPointer SquirrelVM::NewUserData(SQUnsignedInteger size)
{
  return sq_newuserdata(m_vm, size);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::NewSlot(SQInteger idx, bool isstatic)
{
  sq_newslot(m_vm, idx, (isstatic ? SQTrue : SQFalse));
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::Push(bool value)
{
  sq_pushbool(m_vm, value);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::Push(SQInteger value)
{
  sq_pushinteger(m_vm, value);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::Push(SQFloat value)
{
  sq_pushfloat(m_vm, value);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::Push(const SQChar *value)
{
  sq_pushstring(m_vm, value, -1);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::Push(SQUserPointer value)
{
  sq_pushuserpointer(m_vm, value);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::Push(const SquirrelObject& value)
{
  sq_pushobject(m_vm, value.m_obj);
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger SquirrelVM::GetTop()
{
  return sq_gettop(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::SetTop(SQInteger newtop)
{
  sq_settop(m_vm, newtop);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::PopTop()
{
  sq_poptop(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::Pop(SQInteger nelemstopop)
{
  sq_pop(m_vm, nelemstopop);
}

//----------------------------------------------------------------------------------------------------------------------
inline SQRESULT SquirrelVM::Get(SQInteger idx)
{
  return sq_get(m_vm, idx);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::SetNativeClosureName(SQInteger idx, const SQChar *name)
{
  sq_setnativeclosurename(m_vm, idx, name);
}

//----------------------------------------------------------------------------------------------------------------------
inline void SquirrelVM::SetParamsCheck(SQInteger nparams, const SQChar *typemask)
{
  sq_setparamscheck(m_vm, nparams, typemask);
}

}

#endif // _SQPLUS_SQUIRREL_VM_INL_
