#ifdef _MSC_VER
# pragma once
#endif
#ifndef _SQPLUS_RETURNSPECIALIZATION_INL_
#define _SQPLUS_RETURNSPECIALIZATION_INL_

#include "squirrel/squirrel.h"
#include "sqplus/StackUtils.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(), HSQUIRRELVM vm, SQInteger)
{
  RT ret = func();

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(), HSQUIRRELVM, SQInteger)
{
  func();

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(), HSQUIRRELVM vm, SQInteger)
{
  RT ret = (callee.*func)();

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(), HSQUIRRELVM, SQInteger)
{
  (callee.*func)();

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)() const, HSQUIRRELVM vm, SQInteger)
{
  RT ret = (callee.*func)();

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)() const, HSQUIRRELVM, SQInteger)
{
  (callee.*func)();

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1, typename P2>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1, P2), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1, typename P2>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1, P2), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1, typename P2, typename P3>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1, P2, P3), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1, typename P2, typename P3>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1, P2, P3), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1, typename P2, typename P3, typename P4>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1, P2, P3, P4), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1, typename P2, typename P3, typename P4>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1, P2, P3, P4), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1, P2, P3, P4, P5), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1, P2, P3, P4, P5), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1, P2, P3, P4, P5, P6), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1, P2, P3, P4, P5, P6), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5, P6), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5, P6) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1, P2, P3, P4, P5, P6, P7), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1, P2, P3, P4, P5, P6, P7), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
inline SQInteger ReturnSpecialization<RT>::Call(RT (*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  if (!Match(TypeWrapper<P9>(), vm, index + 9))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(10, TypeInfo<P9>().m_typeName);
  }

  RT ret = func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8),
    Get(TypeWrapper<P9>(), vm, index + 9)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
inline SQInteger ReturnSpecialization<void>::Call(void (*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  if (!Match(TypeWrapper<P9>(), vm, index + 9))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(10, TypeInfo<P9>().m_typeName);
  }

  func(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8),
    Get(TypeWrapper<P9>(), vm, index + 9)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  if (!Match(TypeWrapper<P9>(), vm, index + 9))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(10, TypeInfo<P9>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8),
    Get(TypeWrapper<P9>(), vm, index + 9)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9), HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  if (!Match(TypeWrapper<P9>(), vm, index + 9))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(10, TypeInfo<P9>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8),
    Get(TypeWrapper<P9>(), vm, index + 9)
    );

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
inline SQInteger ReturnSpecialization<RT>::Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  if (!Match(TypeWrapper<P9>(), vm, index + 9))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(10, TypeInfo<P9>().m_typeName);
  }

  RT ret = (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8),
    Get(TypeWrapper<P9>(), vm, index + 9)
    );

  Push(vm, ret);

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
inline SQInteger ReturnSpecialization<void>::Call(Callee &callee, void (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9) const, HSQUIRRELVM vm, SQInteger index)
{
  if (!Match(TypeWrapper<P0>(), vm, index + 0))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(1, TypeInfo<P0>().m_typeName);
  }

  if (!Match(TypeWrapper<P1>(), vm, index + 1))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(2, TypeInfo<P1>().m_typeName);
  }

  if (!Match(TypeWrapper<P2>(), vm, index + 2))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(3, TypeInfo<P2>().m_typeName);
  }

  if (!Match(TypeWrapper<P3>(), vm, index + 3))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(4, TypeInfo<P3>().m_typeName);
  }

  if (!Match(TypeWrapper<P4>(), vm, index + 4))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(5, TypeInfo<P4>().m_typeName);
  }

  if (!Match(TypeWrapper<P5>(), vm, index + 5))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(6, TypeInfo<P5>().m_typeName);
  }

  if (!Match(TypeWrapper<P6>(), vm, index + 6))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(7, TypeInfo<P6>().m_typeName);
  }

  if (!Match(TypeWrapper<P7>(), vm, index + 7))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(8, TypeInfo<P7>().m_typeName);
  }

  if (!Match(TypeWrapper<P8>(), vm, index + 8))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(9, TypeInfo<P8>().m_typeName);
  }

  if (!Match(TypeWrapper<P9>(), vm, index + 9))
  {
    sqp::StackHandler stack(vm);
    return stack.ThrowParamError(10, TypeInfo<P9>().m_typeName);
  }

  (callee.*func)(
    Get(TypeWrapper<P0>(), vm, index + 0),
    Get(TypeWrapper<P1>(), vm, index + 1),
    Get(TypeWrapper<P2>(), vm, index + 2),
    Get(TypeWrapper<P3>(), vm, index + 3),
    Get(TypeWrapper<P4>(), vm, index + 4),
    Get(TypeWrapper<P5>(), vm, index + 5),
    Get(TypeWrapper<P6>(), vm, index + 6),
    Get(TypeWrapper<P7>(), vm, index + 7),
    Get(TypeWrapper<P8>(), vm, index + 8),
    Get(TypeWrapper<P9>(), vm, index + 9)
    );

  return 0;
}

}

#endif
