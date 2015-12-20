#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_DIRECTCALL_H_
#define _SQPLUS_DIRECTCALL_H_

#include <memory.h>

#include "sqplus/Call.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// \defgroup DISPATCHERS Squirrel Native Closure Wrappers
/// \brief Classes to help wrap c/c++ functions and class functions in native squirrel closures.

//----------------------------------------------------------------------------------------------------------------------
/// \brief Wraps a call to a standard c/c++ function, use #sq_pushdirectclosure rather than binding
/// DirectCallFunc#Dispatch directly.
/// \note The function pointer is assumed to be stored as the first up value of the bound closure
/// stored as userdata.
/// \ingroup DISPATCHERS
template<typename Func>
class DirectCallFunc
{
public:
  /// \brief Function to be bound as a squirrel closure, use #sq_pushdirectclosure rather than binding
  /// this function directly.
  /// \code
  /// bool Func(char p1, char p2, float p3);
  /// ...
  /// bool (*funcPointer)(char, char, float) = &Func; // pointer to function
  /// SQUserPointer up = sq_newuserdata(vm, sizeof(funcPointer));
  /// memcpy(up, &funcPointer, sizeof(funcPointer));
  /// sq_newclosure(vm, &DirectCallFunc::Dispatch<Func>, 1); // create new closure with one free variable.
  /// \endcode
  static SQInteger Dispatch(HSQUIRRELVM vm);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Wraps a call on a specific instance of the class type Callee to a class method.
/// \note The function pointer and class instance are assumed to be stored as the first variable value of
/// the bound closure stored as userdata, with the layout class + func. Use #ClassDefBase::SingletonFunc
/// rather than binding this function directly.
/// \ingroup DISPATCHERS
template<typename Callee, typename Func>
class DirectCallMemberFunc
{
public:
  /// \brief function to be bound as a squirrel closure.
  static SQInteger Dispatch(HSQUIRRELVM vm);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Wraps a call on a specific instance of the class type Callee to a class method
/// with the signature:
/// \code
/// class Callee
/// {
///   SQInteger Func(HSQUIRRELVM vm);
/// }
/// \endcode
/// Allows a class function to be written in the same style as a squirrel native closure without the
/// need for creating a static function outside the class. The first index in the stack will be the
/// class instance this rather than the root table. Use #ClassDefBase::SingletonFuncVarArgs rather than
/// binding this function directly.
/// \note The function pointer and class instance are assumed to be stored as the first free variable of
/// the bound closure stored as userdata, with the layout class + func.
/// \ingroup DISPATCHERS
template<typename Callee>
class DirectCallMemberFuncVarArgs
{
public:
  /// \brief function to be bound as a squirrel closure.
  static SQInteger Dispatch(HSQUIRRELVM vm);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Helper class to get a class instance and method function from the squirrel vm stack
/// passed in.
/// \note Assumes class instance is the first thing in the stack and the function is assumed
/// to have been bound as the first free variable of the squirrel closure.
/// \ingroup DISPATCHERS
template<typename Callee, typename Func>
class DirectCallInstanceFuncPicker
{
public:
  Callee  *m_instance; ///< the instance that will call the class method.
  Func    *m_func; ///< the method that will be called.

  /// \brief Constructor pulls the instance and method from the stack of vm.
  DirectCallInstanceFuncPicker(HSQUIRRELVM vm);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Wraps a call to a Callee method getting the instance from the first parameter on
/// the stack, uses #DirectCallInstanceFuncPicker to get the method and instance. Use
/// #ClassDefBase::Func rather than binding DirectCallInstanceMemberFunc#Dispatch.
/// \ingroup DISPATCHERS
template<typename Callee, typename Func>
class DirectCallInstanceMemberFunc
{
public:
  /// \brief function to be bound as a squirrel closure.
  static SQInteger Dispatch(HSQUIRRELVM v);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Wraps a call to a Callee method getting the instance from the first parameter on
/// the stack, uses #DirectCallInstanceFuncPicker to get the method and instance. Allows a class
/// function to be written in the same style as a squirrel native closure without the need for
/// creating a static function outside the class. The first index in the stack will be the class
/// instance this rather than the root table. Use #ClassDefBase::FuncVarArgs rather than binding
/// DirectCallInstanceMemberFuncVarArgs#Dispatch.
/// \code
/// class MyClass
/// {
/// public:
///   SQInteger Func(HSQUIRRELVM vm);
/// }
/// \endcode
/// \ingroup DISPATCHERS
template<typename Callee, typename Func>
class DirectCallInstanceMemberFuncVarArgs
{
public:
  /// \brief function to be bound as a squirrel closure.
  static SQInteger Dispatch(HSQUIRRELVM v);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Pushes a normal function as a closure on the stack using #DirectCallFunc to wrap it in a
/// native squirrel closure.
/// \code
/// float Func(int p1, float p2);
/// ...
/// sq_pushdirectclosure(vm, &Func, 0);
/// \endcode
/// \ingroup DISPATCHERS
template<typename Func>
void sq_pushdirectclosure(HSQUIRRELVM vm, Func func, SQUnsignedInteger nupvalues);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Pushes a class method as a closure on the stack using #DirectCallInstanceMemberFunc to wrap
/// it in a native squirrel closure.
/// \code
/// class MyClass
/// {
/// public:
///   float Func(int p1, float p2);
/// }
/// ...
/// sq_pushdirectclosure(vm, static_cast<MyClass *>(0), &MyClass::Func, 0);
/// \endcode
/// \ingroup DISPATCHERS
template<typename Callee, typename Func>
void sq_pushdirectinstanceclosure(HSQUIRRELVM vm, const Callee & callee, Func func, SQUnsignedInteger nupvalues);

}

#include "sqplus/DirectCall.inl"

#endif
