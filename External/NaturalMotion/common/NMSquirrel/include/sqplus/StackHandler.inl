#ifndef _SQPLUS_STACKHANDLER_INL_
#define _SQPLUS_STACKHANDLER_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
inline StackHandler::StackHandler(HSQUIRRELVM vm)
: m_vm(vm)
{
  m_top = sq_gettop(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
inline StackHandler::StackHandler(SquirrelVM &vm)
: m_vm(vm.GetVMPtr())
{
  m_top = sq_gettop(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
inline HSQUIRRELVM StackHandler::GetVMPtr() const
{
  return m_vm;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::GetParamCount() const
{
  return m_top;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQObjectType StackHandler::GetType(SQInteger idx) const
{
  if (idx > 0 && idx <= m_top)
  {
    return sq_gettype(m_vm, idx);
  }

  return OT_NULL;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool StackHandler::IsNumber(SQInteger idx) const
{
  if (idx > 0 && idx <= m_top)
  {
    return (sq_gettype(m_vm, idx) & SQOBJECT_NUMERIC) == SQOBJECT_NUMERIC;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline bool StackHandler::IsInstanceType(SQInteger idx) const
{
  SQUserPointer up;
  return sq_getinstanceup(m_vm, idx, &up, ClassType<T>::Get()) == SQ_OK;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool StackHandler::GetBool(SQInteger idx)
{
  SQBool ret;
  if (idx > 0 && idx <= m_top)
  {
    if (SQ_SUCCEEDED(sq_getbool(m_vm, idx, &ret)))
    {
      return (ret != 0);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::GetInt(SQInteger idx)
{
  SQInteger x = 0;
  if (idx > 0 && idx <= m_top)
  {
    sq_getinteger(m_vm, idx, &x);
  }

  return x;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQFloat StackHandler::GetFloat(SQInteger idx)
{
  SQFloat x = 0.0f;
  if (idx > 0 && idx <= m_top)
  {
    sq_getfloat(m_vm, idx, &x);
  }

  return x;
}

//----------------------------------------------------------------------------------------------------------------------
inline const SQChar *StackHandler::GetString(SQInteger idx)
{
  const SQChar *x = 0;
  if (idx > 0 && idx <= m_top)
  {
    sq_getstring(m_vm, idx, &x);
  }

  return x;
}

//----------------------------------------------------------------------------------------------------------------------
inline SquirrelObject StackHandler::GetObject(SQInteger idx)
{
  SquirrelObject obj(m_vm);

  if (idx > 0 && idx <= m_top)
  {
    obj.AttachToStackObject(idx);
  }

  return obj;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQUserPointer StackHandler::GetUserPointer(SQInteger idx)
{
  SQUserPointer x = 0;
  if (idx > 0 && idx <= m_top)
  {
    sq_getuserpointer(m_vm, idx, &x);
  }

  return x;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQUserPointer StackHandler::GetInstanceUp(SQInteger idx, SQUserPointer typetag)
{
  SQUserPointer instance;
  if (SQ_FAILED(sq_getinstanceup(m_vm, idx, &instance, typetag)))
  {
    return 0;
  }
  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T *StackHandler::GetInstance(SQInteger idx)
{
  SQUserPointer instance;
  if (SQ_FAILED(sq_getinstanceup(m_vm, idx, &instance, ClassType<T>::Get())))
  {
    return 0;
  }
  return reinterpret_cast<T *>(instance);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T *StackHandler::GetInstanceNoTypeCheck(SQInteger idx)
{
  SQUserPointer instance;
  if (SQ_FAILED(sq_getinstanceup(m_vm, idx, &instance, 0)))
  {
    return 0;
  }
  return reinterpret_cast<T *>(instance);
}

//----------------------------------------------------------------------------------------------------------------------
inline SQUserPointer StackHandler::GetUserData(SQInteger idx, SQUserPointer *tag)
{
  SQUserPointer otag;
  SQUserPointer up;
  if (idx > 0 && idx <= m_top)
  {
    if (SQ_SUCCEEDED(sq_getuserdata(m_vm, idx, &up, &otag)))
    {
      if (tag == otag)
      {
        return up;
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::ThrowParamError(SQInteger idx, const SQChar *expectedType)
{
  return ThrowError(
    _SC("parameter %d has an invalid type '%s' ; expected: '%s'"),
    idx - 1,
    GetTypeName(idx),
    expectedType);
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::Return()
{ 
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::Return(bool b)
{
  sq_pushbool(m_vm, (b ? SQTrue : SQFalse));
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::Return(SQInteger i)
{
  sq_pushinteger(m_vm, i);
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::Return(SQFloat f)
{
  sq_pushfloat(m_vm, f);
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::Return(const SQChar *s, SQInteger length)
{
  sq_pushstring(m_vm, s, length);
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::Return(SQUserPointer p)
{
  sq_pushuserpointer(m_vm, p);
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::Return(SquirrelObject &o)
{
	sq_pushobject(m_vm, o.GetHandle());
	return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline SQInteger StackHandler::Return(const T *instance)
{
  if (Push(m_vm, instance))
  {
    return 1;
  }
  return ThrowError(_SC("error returning instance of type '%s'"), TypeInfo<T>().m_typeName);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline SQInteger StackHandler::Return(const T &instance)
{
  if (Push(m_vm, instance))
  {
    return 1;
  }
  return ThrowError(_SC("error returning instance of type '%s'"), TypeInfo<T>().m_typeName);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
SQInteger StackHandler::Return(const T *instance, SQRELEASEHOOK hook)
{
  if (instance != 0)
  {
    ClassType<T> *typeinfo = ClassType<T>::Get();
    if (CreateNativeClassInstance(m_vm, typeinfo->GetNamespace(), TypeInfo<T>().m_typeName, const_cast<T *>(instance), hook))
    {
      return 1;
    }
    if (hook != 0)
    {
      hook(const_cast<T *>(instance), sizeof(T));
    }
    return ThrowError(_SC("error returning instance of type '%s'"), TypeInfo<T>().m_typeName);
  }

  sq_pushnull(m_vm);
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
inline SQInteger StackHandler::SuspendVM()
{
  return sq_suspendvm(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
}

#endif //_SQPLUS_STACKHANDLER_INL_
