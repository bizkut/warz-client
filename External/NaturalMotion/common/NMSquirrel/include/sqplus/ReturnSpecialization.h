#ifdef _MSC_VER
# pragma once
#endif
#ifndef _SQPLUS_RETURNSPECIALIZATION_H_
#define _SQPLUS_RETURNSPECIALIZATION_H_

#include "squirrel/squirrel.h"
#include "sqplus/StackUtils.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
class ReturnSpecialization
{
public:
  static SQInteger Call(RT (*func)(), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee>
  static SQInteger Call(Callee &callee, RT (Callee::*func)() const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0>
  static SQInteger Call(RT (*func)(P0), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1>
  static SQInteger Call(RT (*func)(P0, P1), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1, typename P2>
  static SQInteger Call(RT (*func)(P0, P1, P2), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1, typename P2, typename P3>
  static SQInteger Call(RT (*func)(P0, P1, P2, P3), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1, typename P2, typename P3, typename P4>
  static SQInteger Call(RT (*func)(P0, P1, P2, P3, P4), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
  static SQInteger Call(RT (*func)(P0, P1, P2, P3, P4, P5), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  static SQInteger Call(RT (*func)(P0, P1, P2, P3, P4, P5, P6), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
  static SQInteger Call(RT (*func)(P0, P1, P2, P3, P4, P5, P6, P7), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
  static SQInteger Call(RT (*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8) const, HSQUIRRELVM vm, SQInteger index);

  template<typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
  static SQInteger Call(RT (*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9), HSQUIRRELVM vm, SQInteger index);
  template<typename Callee, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
  static SQInteger Call(Callee &callee, RT (Callee::*func)(P0, P1, P2, P3, P4, P5, P6, P7, P8, P9) const, HSQUIRRELVM vm, SQInteger index);
};

}

#include "sqplus/ReturnSpecialization.inl"

#endif
