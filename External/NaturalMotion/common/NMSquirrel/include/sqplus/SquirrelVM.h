//----------------------------------------------------------------------------------------------------------------------
/// \file SquirrelVM.h
/// Based on the SquirrelVM class written by Alberto Demichelis in his DXSquirrel example bindings
/// and the further work of John Schultz in sqp.
/// \author Alberto Demichelis
//----------------------------------------------------------------------------------------------------------------------
#ifndef _SQPLUS_SQUIRRELVM_H_
#define _SQPLUS_SQUIRRELVM_H_

#include <squirrel/squirrel.h>

#include <sqplus/Interface.h>
#include <sqplus/SquirrelObject.h>

/*lint -sem(sqp::SquirrelVM::Close,cleanup)*/
/*lint -esym(534, sqp::SquirrelVM::CreateFunction, sqp::SquirrelVM::RunScript)*/

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief Wraps a HSQUIRRELVM providing some simple functionality.
//----------------------------------------------------------------------------------------------------------------------
class SQPLUS_API SquirrelVM
{
public:
  /// \brief 
  SquirrelVM();
  /// \brief Initialises the wrapper with an existing vm
  SquirrelVM(HSQUIRRELVM vm);
  /// \brief Copy constructor.
  SquirrelVM(const SquirrelVM& rhs);

  /// \brief Destructor closes the \link #HSQUIRRELVM vm\endlink.
  ~SquirrelVM();

  /// \brief
  bool Initialise(SQInteger size = 1024);

  /// \brief
  bool IsInitialised() const;

  /// \brief Closes the vm.
  bool Shutdown();

  /// \brief Gets the \link #HSQUIRRELVM vm\endlink this class wraps.
  HSQUIRRELVM GetVMPtr() const;

  /// \brief Sets the root table to null.
  SQRESULT ClearRootTable();
  /// \brief Pushes the root table onto the top of the stack.
  void PushRootTable();
  /// \brief Gets the root table for this object.
  SquirrelObject& GetRootTable() const;

  /// \brief Pushes the const table onto the top of the stack.
  void PushConstTable();
  /// \brief Gets the const table for this object.
  SquirrelObject& GetConstTable() const;
  /// \brief Sets the const table.
  SQRESULT SetConstTable(const SquirrelObject& table);

  /// \brief Registers the standard io, math, string and blob lives to the table on top of the stack.
  SQRESULT RegisterStdLibs();
  /// \brief Sets print function for this vm, defaults to the static SquirrelVM::PrintFunc
  void SetPrintFunc(SQPRINTFUNCTION func = &PrintFunc);

  /// \brief Compiles a script into a SquirrelObject with any errors optionally printed using the
  /// currently set \link #SQPRINTFUNCTION print function\endlink.  
	SquirrelObject CompileScript(const SQChar *filename, bool printerror = true, SQRESULT *pResult = 0);
  /// \brief Compiles a buffer into a SquirrelObject with any errors optionally printed using the
  /// currently set \link #SQPRINTFUNCTION print function\endlink. The parameter debugInfo is the name
  /// displayed as the script name from within a debug hook.
	SquirrelObject CompileBuffer(
    const SQChar *buffer,
    bool printerror = true,
    const SQChar *debugInfo = _SC("buffer"),
    SQRESULT *pResult = 0);
  /// \brief Runs a SquirrelObject script using an optional _this parameter as the first argument
  /// to the script, if no _this object is specified then the root table is passed. Any parameter
  /// returned by the script is returned by this function as a SquirrelObject.
	SquirrelObject RunScript(
    const SquirrelObject& script,
    const SquirrelObject *_this = 0,
    bool raiseError = true,
    SQRESULT *pResult = 0);

  /// \brief Creates a squirrel string and returns it as a SquirrelObject.
	SquirrelObject CreateString(const SQChar *s);
  /// \brief Creates a squirrel table and returns it as a SquirrelObject.
	SquirrelObject CreateTable();
  /// \brief Creates a squirrel array and returns it as a SquirrelObject.
	SquirrelObject CreateArray(SQInteger size);
  /// \brief Creates squirrel userdata and returns it as a SquirrelObject.
  /// Optionally the user can get the pointer to the new user data as well if needed.
  SquirrelObject CreateUserData(SQUnsignedInteger size, SQUserPointer *newUserData = 0);
  /// \brief Creates a class and returns it as a SquirrelObject. If hasBase is true
  /// an additional base class at the top of the stack is popped and used as the base
  /// of the new derived class.
  SquirrelObject CreateClass(
    const SQChar *name,
    const SQChar *baseName,
    SQUserPointer typetag,
    SQRESULT *result);

  /// \brief Creates an instance of the class passed into the function and returns it
  /// as a SquirrelObject.
  SquirrelObject CreateInstance(const SquirrelObject& classdef);

  /// \brief creates a function and returns it as a SquirrelObject.
  SquirrelObject CreateFunction(SQFUNCTION func);

  /// \brief Creates a function func with the parameter check specified by nParams and
  /// typeMask. This function is then added to a slot named funcName in the table or class
  /// currently on the stack. If the object on the stack is a class the parameter staticMethod
  /// is used to specify whether the new function is a static member.
  SquirrelObject CreateFunction(
    SQFUNCTION func,
    const SQChar *funcName,
    SQInteger nParams = 0,
    const SQChar *typeMask = 0,
    bool isStatic = false);

  /// \overload
  /// Instead of creating the the function in a slot on the table on the stack the slot is
  /// in the table or class specified by object.
  SquirrelObject CreateFunction(
    SquirrelObject& object,
    SQFUNCTION func,
    const SQChar *funcName,
    SQInteger nParams = 0,
    const SQChar *typeMask = 0,
    bool isStatic = false);

  /// \code
  /// float example(int p1, float p2);
  /// SquirrelVM vm;
  /// ...
  /// vm.CreateFunction(&example, _SC("example");
  /// \endcode
  template<typename Func>
  SquirrelObject CreateFunction(
    Func func,
    const SQChar *name,
    bool isstatic = false);

  /// \overload
  /// Instead of creating the the function in a slot on the table on the stack the slot is
  /// in the root table effectively creating a global function.
  SquirrelObject CreateGlobalFunction(
    SQFUNCTION func,
    const SQChar *funcName,
    SQInteger nParams = 0,
    const SQChar *typeMask = 0);

  /// \brief Creates a new closure popping nfreevars setting them as free variables of the closure.
  /// The resulting closure is left on the stack.
  void NewClosure(SQFUNCTION func, SQUnsignedInteger nfreevars);

  /// \brief Creates a new user data and pushes it in the stack.
  SQUserPointer NewUserData(SQUnsignedInteger size);

  void NewSlot(SQInteger idx, bool isstatic);

  /// \brief Pushes a bool onto the top of the stack.
  void Push(bool value);
  /// \overload
  /// Pushes an integer onto the stack.
  void Push(SQInteger value);
  /// \overload
  /// Pushes a float onto the stack.
  void Push(SQFloat value);
  /// \overload
  /// Pushes a string onto the stack.
  void Push(const SQChar *value);
  /// \overload
  /// Pushes a user pointer onto the stack.
  void Push(SQUserPointer value);
  /// \overload
  /// Pushes a SquirrelObject onto the stack.
  void Push(const SquirrelObject& value);

  /// \brief Gets the number of elements on the stack
  SQInteger GetTop();
  /// \brief Sets the top of the stack.
  void SetTop(SQInteger newtop);
  /// \brief pops the top value off the stack.
  void PopTop();
  /// \brief pops a value from the \link #HSQUIRRELVM vm\endlink stack
  void Pop(SQInteger nelemstopop);

  /// \brief pops a key from the stack and performs a get operation on the
  /// object at the position idx in the stack, and pushes the result in the stack
  SQRESULT Get(SQInteger idx);

  /// \brief sets the name of the native closure at the position idx in the stack.
  /// The name of a native closure is purely for debug purposes.
  /// The name is retrieved through the function sq_stackinfos() while the closure is in the call stack.
  void SetNativeClosureName(SQInteger idx, const SQChar *name);
  /// \brief Sets the typemask check for a closure.
  void SetParamsCheck(SQInteger nparamscheck, const SQChar *typemask);

  /// \brief Static print function to stdout.
  static void PrintFunc(HSQUIRRELVM v, const SQChar *s, ...);

private:
/// \cond NO_DOXYGEN
	HSQUIRRELVM m_vm;
	mutable SquirrelObject *m_rootTable;
  mutable SquirrelObject *m_constTable;
/// \endcond
};
//----------------------------------------------------------------------------------------------------------------------
}

#include "sqplus/SquirrelVM.inl"

#endif
