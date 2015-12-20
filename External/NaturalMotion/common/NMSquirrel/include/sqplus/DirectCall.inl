#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_DIRECTCALL_INL_
#define _SQPLUS_DIRECTCALL_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// DirectCallFunc
//----------------------------------------------------------------------------------------------------------------------
template<typename Func>
inline SQInteger DirectCallFunc<Func>::Dispatch(HSQUIRRELVM vm)
{
  SQInteger count = sq_gettop(vm);

  unsigned char *ud;
  sq_getuserdata(vm, count, reinterpret_cast<SQUserPointer *>(&ud), static_cast<SQUserPointer *>(0));

  Func *func = reinterpret_cast<Func*>(ud);
  return Call(*func, vm, 2);
}

//----------------------------------------------------------------------------------------------------------------------
/// DirectCallMemberFunc
//----------------------------------------------------------------------------------------------------------------------
template<typename Callee, typename Func>
inline SQInteger DirectCallMemberFunc<Callee, Func>::Dispatch(HSQUIRRELVM vm)
{
  SQInteger count = sq_gettop(vm);

  unsigned char *ud;
  sq_getuserdata(vm, count, reinterpret_cast<SQUserPointer *>(&ud), static_cast<SQUserPointer *>(0));

  Callee **instance = reinterpret_cast<Callee**>(ud);
  Func* func = reinterpret_cast<Func*>(ud + sizeof(Callee *));
  return Call(**instance, *func, vm, 2);
}

//----------------------------------------------------------------------------------------------------------------------
/// DirectCallMemberFuncVarArgs
//----------------------------------------------------------------------------------------------------------------------
template<typename Callee>
inline SQInteger DirectCallMemberFuncVarArgs<Callee>::Dispatch(HSQUIRRELVM vm)
{
  SQInteger count = sq_gettop(vm);

  unsigned char *ud;
  sq_getuserdata(vm, count, reinterpret_cast<SQUserPointer *>(&ud), static_cast<SQUserPointer *>(0));

  typedef SQInteger (Callee:: *HSQMETHOD)(HSQUIRRELVM);

  Callee **instance = reinterpret_cast<Callee**>(ud);
  HSQMETHOD *func = reinterpret_cast<HSQMETHOD*>(ud + sizeof(Callee *));
  return (**instance.**func)(vm);
}

//----------------------------------------------------------------------------------------------------------------------
/// DirectCallInstanceFuncPicker
//----------------------------------------------------------------------------------------------------------------------
template<typename Callee, typename Func>
DirectCallInstanceFuncPicker<Callee, Func>::DirectCallInstanceFuncPicker(HSQUIRRELVM vm)
{
  sq_getinstanceup(vm, 1, reinterpret_cast<SQUserPointer*>(&m_instance), static_cast<SQUserPointer>(0));

  ClassTypeBase *instanceTypeTag;
  sq_gettypetag(vm, 1, reinterpret_cast<SQUserPointer*>(&instanceTypeTag));

  // count is also the index of the first free variable of the function
  // which is a userdata to c/c++ func pointer bound when the function is bound
  SQInteger count = sq_gettop(vm);
  sq_getuserdata(vm, count, reinterpret_cast<SQUserPointer*>(&m_func), 0);

  ClassTypeBase *calleeType = ClassType<Callee>::Get();
  if (instanceTypeTag != calleeType && instanceTypeTag->MayHaveOffset())
  {
    // instance type is more derived than callee and there may be an offset, adjust pointer
    ptrdiff_t offset = instanceTypeTag->GetOffsetTo(calleeType);
    m_instance = reinterpret_cast<Callee *>((reinterpret_cast<unsigned char *>(m_instance) - offset));
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// DirectCallInstanceMemberFunc
//----------------------------------------------------------------------------------------------------------------------
template<typename Callee, typename Func>
inline SQInteger DirectCallInstanceMemberFunc<Callee, Func>::Dispatch(HSQUIRRELVM vm)
{
  DirectCallInstanceFuncPicker<Callee, Func> picker(vm);
  if (picker.m_instance && picker.m_func)
  {
    return Call(*picker.m_instance, *picker.m_func, vm, 2);
  }
  return sq_throwerror(vm, _SC("Invalid instance type."));
}

//----------------------------------------------------------------------------------------------------------------------
/// DirectCallInstanceMemberFuncVarArgs
//----------------------------------------------------------------------------------------------------------------------
template<typename Callee, typename Func>
inline SQInteger DirectCallInstanceMemberFuncVarArgs<Callee, Func>::Dispatch(HSQUIRRELVM vm)
{
  DirectCallInstanceFuncPicker<Callee, Func> picker(vm);
  if (picker.m_instance && picker.m_func)
  {
    return (picker.m_instance->*(*picker.m_func))(vm);
  }
  return sq_throwerror(vm, _SC("Invalid instance type."));
}

//----------------------------------------------------------------------------------------------------------------------
template<typename Func>
inline void sq_pushdirectclosure(HSQUIRRELVM vm, Func func, SQUnsignedInteger nupvalues)
{
  // Also pushed on stack.
  SQUserPointer up = sq_newuserdata(vm, sizeof(func));
  memcpy(up, &func, sizeof(func));
  sq_newclosure(vm, &DirectCallFunc<Func>::Dispatch, nupvalues + 1);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename Callee, typename Func>
inline void sq_pushdirectinstanceclosure(
  HSQUIRRELVM vm,
  const Callee& SQUIRREL_UNUSED(callee),
  Func func,
  SQUnsignedInteger nupvalues)
{
  // Also pushed on stack.
  SQUserPointer *up = sq_newuserdata(vm, sizeof(func));
  memcpy(up, &func, sizeof(func));
  sq_newclosure(vm, &DirectCallInstanceMemberFunc<Callee, Func>::Dispatch, nupvalues + 1);
}

}

#endif
