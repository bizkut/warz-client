#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_SQUIRRELFUNCTION_INL_
#define _SQPLUS_SQUIRRELFUNCTION_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
SquirrelFunction<RT>::SquirrelFunction()
: m_vm(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
SquirrelFunction<RT>::SquirrelFunction(const SquirrelFunction<RT>& rhs)
: m_vm(rhs.m_vm), m_environment(rhs.m_environment), m_function(rhs.m_function), m_result(rhs.m_vm)
{
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
SquirrelFunction<RT>::SquirrelFunction(HSQUIRRELVM vm, const SQChar *name)
: m_vm(vm), m_environment(m_vm)
{
  sq_pushroottable(m_vm);   
  m_environment.AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  m_function = m_environment.GetValue(name);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
SquirrelFunction<RT>::SquirrelFunction(HSQUIRRELVM vm, const SquirrelObject& function)
: m_vm(vm), m_environment(m_vm)
{
  sq_pushroottable(m_vm);    
  m_environment.AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  m_function = function;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
SquirrelFunction<RT>::SquirrelFunction(
  HSQUIRRELVM vm,
  const SquirrelObject& environment,
  const SQChar *name)
: m_vm(vm), m_environment(environment)
{
  m_function = m_environment.GetValue(name);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
SquirrelFunction<RT>::SquirrelFunction(
  HSQUIRRELVM vm,
  const SquirrelObject& environment,
  const SquirrelObject& function)
: m_vm(vm), m_environment(environment), m_function(function)
{
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
RT SquirrelFunction<RT>::operator () (SQRESULT *pResult)
{
  sq_pushobject(m_vm, m_function.GetHandle());
  sq_pushobject(m_vm, m_environment.GetHandle());
  
  SQRESULT res = sq_call(m_vm, 1, SQTrue, SQFalse);
  if (pResult)
  {
    *pResult = res;
  }

  return GetRet(TypeWrapper<RT>(), m_vm, -1, &m_result);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P1>
RT SquirrelFunction<RT>::operator () (P1 p1, SQRESULT *pResult)
{
  sq_pushobject(m_vm, m_function.GetHandle());
  sq_pushobject(m_vm, m_environment.GetHandle());
  Push(m_vm, p1);

  SQRESULT res = sq_call(m_vm, 2, SQTrue, SQFalse);
  if (pResult)
  {
    *pResult = res;
  }

  return GetRet(TypeWrapper<RT>(), m_vm, -1, &m_result);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P1, typename P2>
RT SquirrelFunction<RT>::operator () (P1 p1, P2 p2, SQRESULT *pResult)
{
  sq_pushobject(m_vm, m_function.GetHandle());
  sq_pushobject(m_vm, m_environment.GetHandle());
  Push(m_vm, p1);
  Push(m_vm, p2);

  SQRESULT res = sq_call(m_vm, 3, SQTrue, SQFalse);
  if (pResult)
  {
    *pResult = res;
  }

  return GetRet(TypeWrapper<RT>(), m_vm, -1, &m_result);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P1, typename P2, typename P3>
RT SquirrelFunction<RT>::operator () (P1 p1, P2 p2, P3 p3, SQRESULT *pResult)
{
  sq_pushobject(m_vm, m_function.GetHandle());
  sq_pushobject(m_vm, m_environment.GetHandle());
  Push(m_vm, p1);
  Push(m_vm, p2);
  Push(m_vm, p3);

  SQRESULT res = sq_call(m_vm, 4, SQTrue, SQFalse);
  if (pResult)
  {
    *pResult = res;
  }

  return GetRet(TypeWrapper<RT>(), m_vm, -1, &m_result);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P1, typename P2, typename P3, typename P4>
RT SquirrelFunction<RT>::operator () (P1 p1, P2 p2, P3 p3, P4 p4, SQRESULT *pResult)
{
  sq_pushobject(m_vm, m_function.GetHandle());
  sq_pushobject(m_vm, m_environment.GetHandle());
  Push(m_vm, p1);
  Push(m_vm, p2);
  Push(m_vm, p3);
  Push(m_vm, p4);

  SQRESULT res = sq_call(m_vm, 5, SQTrue, SQFalse);
  if (pResult)
  {
    *pResult = res;
  }

  return GetRet(TypeWrapper<RT>(), m_vm, -1, &m_result);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P1, typename P2, typename P3, typename P4, typename P5>
RT SquirrelFunction<RT>::operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, SQRESULT *pResult)
{
  sq_pushobject(m_vm, m_function.GetHandle());
  sq_pushobject(m_vm, m_environment.GetHandle());
  Push(m_vm, p1);
  Push(m_vm, p2);
  Push(m_vm, p3);
  Push(m_vm, p4);
  Push(m_vm, p5);

  SQRESULT res = sq_call(m_vm, 6, SQTrue, SQFalse);
  if (pResult)
  {
    *pResult = res;
  }

  return GetRet(TypeWrapper<RT>(), m_vm, -1, &m_result);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
RT SquirrelFunction<RT>::operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, SQRESULT *pResult)
{
  sq_pushobject(m_vm, m_function.GetHandle());
  sq_pushobject(m_vm, m_environment.GetHandle());
  Push(m_vm, p1);
  Push(m_vm, p2);
  Push(m_vm, p3);
  Push(m_vm, p4);
  Push(m_vm, p5);
  Push(m_vm, p6);

  SQRESULT res = sq_call(m_vm, 7, SQTrue, SQFalse);
  if (pResult)
  {
    *pResult = res;
  }

  return GetRet(TypeWrapper<RT>(), m_vm, -1, &m_result);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
RT SquirrelFunction<RT>::operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, SQRESULT *pResult)
{
  sq_pushobject(m_vm, m_function.GetHandle());
  sq_pushobject(m_vm, m_environment.GetHandle());
  Push(m_vm, p1);
  Push(m_vm, p2);
  Push(m_vm, p3);
  Push(m_vm, p4);
  Push(m_vm, p5);
  Push(m_vm, p6);
  Push(m_vm, p7);

  SQRESULT res = sq_call(m_vm, 8, SQTrue, SQFalse);
  if (pResult)
  {
    *pResult = res;
  }

  return GetRet(TypeWrapper<RT>(), m_vm, -1, &m_result);
}

}

#endif
