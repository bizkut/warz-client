#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_STACKHANDLER_H_
#define _SQPLUS_STACKHANDLER_H_
//----------------------------------------------------------------------------------------------------------------------
/// \file StackHandler.h
/// Based on the StackHandler class written by Alberto Demichelis in his DXSquirrel example bindings
/// and the further work of John Schultz in sqp.
/// \author Alberto Demichelis
//----------------------------------------------------------------------------------------------------------------------

#include <sqplus/SquirrelVM.h>

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// Wraps common functionality for handling the squirrel stack passed into a native closure.
//----------------------------------------------------------------------------------------------------------------------
class StackHandler
{
public:
  /// \brief Initialises the stack handler with a \link #HSQUIRRELVM vm\endlink.
  StackHandler(HSQUIRRELVM v);

  /// \brief Initialises the stack handler with the \link #HSQUIRRELVM vm\endlink of the sqp::SquirrelVM.
  StackHandler(SquirrelVM& vm);

  /// \brief Gets the virtual machine the stack belongs to.
  HSQUIRRELVM GetVMPtr() const;

  /// \brief Gets the number of arguments on the stack.
  SQInteger GetParamCount() const;

  /// \brief Gets the type of the object at the specified index.
  SQObjectType GetType(SQInteger idx) const;

  /// \brief Same as GetType(idx) == OT_INTEGER || GetType(idx) == OT_FLOAT
  bool IsNumber(SQInteger idx) const;

  /// \brief Checks if the object at stack index idx is an instance of a particular bound class type.
  /// \code
  /// sqp::StackHandler stack(vm)l
  /// ...
  /// if (stack.IsInstanceType<MyClass>(1))
  /// {
  ///   MyClass *ptr = stack.GetInstanceNoTypeCheck<MyClass>();
  /// }
  /// \endcode
  template<typename T>
  bool IsInstanceType(SQInteger idx) const;

  /// \brief Gets the type name of the object at the specified index.
  /// Will generate the same result as calling typeof(obj) in squirrel.
  const SQChar *GetTypeName(SQInteger idx) const;

  /// \brief Gets a boolean from the stack at the specified index. If the object at that index
  /// is not a boolean then the function returns false.
  bool GetBool(SQInteger idx);
  /// \brief Gets an integer from the stack at the specified index. If the object at that index
  /// is not an integer then 0 is returned.
  SQInteger GetInt(SQInteger idx);
  /// \brief Gets a float from the stack at the specified index. If the object at that index is
  /// not a float then 0.0f is returned.
  SQFloat GetFloat(SQInteger idx);
  /// \brief Gets a string from the stack at the specified index. If the object at that index is
  /// not a string then null is returned.
  const SQChar *GetString(SQInteger idx);
  /// \brief Gets an object handle from the stack at the specified index.
  SquirrelObject GetObject(SQInteger idx);
  /// \brief Gets a user pointer from the stack at the specified index.
  SQUserPointer GetUserPointer(SQInteger idx);
  /// \brief Gets an instance user pointer from the stack at the specified index optionally checking
  /// the typetag.
  SQUserPointer GetInstanceUp(SQInteger idx, SQUserPointer typetag);
  /// \brief Gets a class instance from the stack using ClassType<T>::Type() as the typetag check.
  /// If the object at idx is not an instance or does not have the correct typetag then it will
  /// return null otherwise it will return a pointer to the class instance.
  /// \code
  /// sqp::StackHandler stack(vm);
  /// MyClass* instance = stack.GetInstance<MyClass>(1);
  /// if (instance == 0)
  /// {
  ///   return stack.ThrowParamError(1, sqp::TypeInfo<MyClass>().m_typeName);
  /// }
  /// \endcode
  /// \note MyClass must already be bound using DECLARE_INSTANCE_TYPE(MyClass); and sqp::ClassDef<MyClass>.
  template<typename T>
  T* GetInstance(SQInteger idx);

  /// \brief Gets an instance of a class from the stack without checking it's type. Use in conjunction
  /// with IsInstanceType.
  /// \code
  /// sqp::StackHandler stack(vm);
  /// if (!stack.IsInstanceType<MyClass>(1))
  /// {
  ///   return stack.ThrowParamError(1, sqp::TypeInfo<MyClass>().m_typeName);
  /// }
  /// MyClass *_this = stack.GetInstanceNoTypeCheck<MyClass>();
  /// \endcode
  /// \note MyClass must already be bound using DECLARE_INSTANCE_TYPE(MyClass); and sqp::ClassDef<MyClass>.
  template<typename T>
  T* GetInstanceNoTypeCheck(SQInteger idx);

  /// \brief Gets a user data from the stack at the specified index also returning an optional typetag.
  SQUserPointer GetUserData(SQInteger idx, SQUserPointer *tag = 0);

  /// \brief Throws a squirrel exception. Must be the returned by the native closure.
  /// \code
  /// StackHandler stack(vm);
  /// return stack.ThrowError(_SC("error message"));
  /// \endcode
  SQInteger ThrowError(const SQChar *format, ...);

  /// \brief Throws a squirrel exception with an error message specific for wrong parameter arguments.
  /// \code
  /// // squirrel code
  /// testfunc("blah")
  ///
  /// // c++ code inside testfunc
  /// StackHandler stack(vm);
  /// return stack.ThrowParamError(2, _SC("integer"));
  /// \endcode
  /// Generates the error message "parameter 1 has an invalid type 'string' ; expected: 'integer'".
  /// \note idx is the index on the stack not the parameter argument number so a stack index of 2 would be
  /// the first parameter to the function. Stack index 1 is the hidden parameter 0 passed to all functions.
  SQInteger ThrowParamError(SQInteger idx, const SQChar *expectedType);

  /// \brief Used to returns nothing from a native closure, must be returned by the native closure.
  /// \return The number of arguments the closure returns.
  /// \code
  /// StackHandler sa(vm);
  ///   ...
  /// return sa.Return();
  /// \endcode
  SQInteger Return();
  /// \overload
  /// Makes the native closure return a bool.
  SQInteger Return(bool b);
  /// \overload
  /// Makes the native closure return an integer.
  SQInteger Return(SQInteger i);
  /// \overload
  /// Makes the native closure return a float.
  SQInteger Return(SQFloat f);
  /// \overload
  /// Makes the native closure return a string
  SQInteger Return(const SQChar *s, SQInteger length = -1);
  /// \overload
  /// Makes the native closure return a user pointer
  SQInteger Return(SQUserPointer p);
  /// \overload
  /// Makes the native closure return a SquirrelObject.
  SQInteger Return(SquirrelObject &o);
  /// \overload
  /// Returns a bound class instance from a closure.
  template<typename T>
  SQInteger Return(const T *instance);
  /// \overload
  /// Returns a bound class instance from a closure.
  template<typename T>
  SQInteger Return(const T &instance);

  /// \overload
  /// In some cases can be quicker than the other templated return functions as it removes
  /// the need for an extra copy constructor and also removes some extra function calls.
  /// \code
  /// MyClass func();
  ///
  /// SQInteger sqfunc_standard(HSQUIRRELVM vm)
  /// {
  ///   return stack.Return(func());
  /// }
  ///
  /// SQInteger sqfunc_fast(HSQUIRRELVM vm)
  /// {
  ///   void *memory = sq_malloc(sizeof(MyClass));
  ///   MyClass *copy = new (memory) MyClass(func());
  ///   return stack.Return(copy, &sqp::DefaultReleaseHook<MyClass>);
  /// }
  /// \endcode
  template<typename T>
  SQInteger Return(const T *instance, SQRELEASEHOOK hook);

  /// \brief Suspends the execution of the vm must be returned by a c function.
  SQInteger SuspendVM();

private:
/// \cond NO_DOXYGEN
  SQInteger m_top;
  HSQUIRRELVM m_vm;

  // disabled constructors and assignment operators
  StackHandler();
  StackHandler(const StackHandler &rhs);
  const StackHandler &operator = (const StackHandler &rhs);
/// \endcond

  bool IsExactType(SQInteger idx, ClassTypeBase *classType) const;
};
//----------------------------------------------------------------------------------------------------------------------
}

/// \cond NO_DOXYGEN
#include <sqplus/StackHandler.inl>
/// \endcond

#endif //_SQPLUS_STACKHANDLER_H_
