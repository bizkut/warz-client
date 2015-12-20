#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_SQUIRRELFUNCTION_H_
#define _SQPLUS_SQUIRRELFUNCTION_H_

#include "sqplus/SquirrelObject.h"
#include "sqplus/StackUtils.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief Wrapper class for calling squirrel functions.
/// \code
/// sqp::SquirrelFunction func(vm, "print");
/// func(_SC("float: %f", 0.1f);
/// \endcode
//----------------------------------------------------------------------------------------------------------------------
template<typename RT>
class SquirrelFunction
{
public:
  /// \brief Default constructor does nothing.
  SquirrelFunction();
  /// \brief Copy constructor
  SquirrelFunction(const SquirrelFunction<RT>& rhs);
  /// \brief Initialises finding a named function in the root table.
  SquirrelFunction(HSQUIRRELVM vm, const SQChar *name);
  /// \brief Calls a global function using the root table of vm as the environment.
  SquirrelFunction(HSQUIRRELVM vm, const SquirrelObject& function);
  /// \brief Calls a named function that exists within the environment object.
  SquirrelFunction(HSQUIRRELVM vm, const SquirrelObject& environment, const SQChar *name);
  /// \brief Calls a function that exists within the environment object.
  SquirrelFunction(HSQUIRRELVM vm, const SquirrelObject& environment, const SquirrelObject& function);

  RT operator () (SQRESULT *pResult = 0);

  template<typename P1>
  RT operator () (P1 p1, SQRESULT *pResult = 0);

  template<typename P1, typename P2>
  RT operator () (P1 p1, P2 p2, SQRESULT *pResult = 0);

  template<typename P1, typename P2, typename P3>
  RT operator () (P1 p1, P2 p2, P3 p3, SQRESULT *pResult = 0);

  template<typename P1, typename P2, typename P3, typename P4>
  RT operator () (P1 p1, P2 p2, P3 p3, P4 p4, SQRESULT *pResult = 0);

  template<typename P1, typename P2, typename P3, typename P4, typename P5>
  RT operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, SQRESULT *pResult = 0);

  template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  RT operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, SQRESULT *pResult = 0);

  template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
  RT operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, SQRESULT *pResult = 0);

public:
  HSQUIRRELVM    m_vm;
  SquirrelObject m_environment; //< The environment the function belongs to, also the first hidden param to the function.
  SquirrelObject m_function;    //< The squirrel function to call.
  SquirrelObject m_result;      //< Keeps a reference to the return param after it has been popped from the stack.
};

}

#include "sqplus/SquirrelFunction.inl"

#endif
