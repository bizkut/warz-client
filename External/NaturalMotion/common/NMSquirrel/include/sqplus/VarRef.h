#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_VARREF_H_
#define _SQPLUS_VARREF_H_
//----------------------------------------------------------------------------------------------------------------------
/// \file VarRef.h
/// Based on the VarRef class written by John Schultz and others in sqp.
/// \author John Schultz
//----------------------------------------------------------------------------------------------------------------------
#include "squirrel/squirrel.h"

#include "sqplus/ClassType.h"
#include "sqplus/BaseHeader.h"
#include "sqplus/SquirrelVM.h"

namespace sqp
{
class ClassTypeBase;
class StackHandler;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Reference to a variable that stores information about a global/static variable's location
/// or a class instance variable's location relative to it's class instance.
class SQPLUS_API VarRef
{
public:
  enum Flags
  {
    kStaticVar = 1 << 1
  };

  static void CreateTableSetGetHandlers(SquirrelVM& vm, SquirrelObject& object);
  static void CreateInstanceSetGetHandlers(SquirrelVM& vm, SquirrelObject& object);

  /// \brief Creates userdata slot for a VarRef in the object with the key name.
  static SQUserPointer Create(SquirrelObject& object, const SQChar *name);

  VarRef(
    void *offsetOrAddress,
    ScriptVarType type,
    ClassTypeBase *instanceType,
    ClassTypeBase *varType = 0,
    int flags = 0);

private:
  void          *m_offsetOrAddress; ///< the address of a global variable or the offset of an instance variable.
  ScriptVarType  m_type; ///< The type of the variable
  ClassTypeBase *m_instanceType; ///< The class type of the instance variable's instance if valid.
  ClassTypeBase *m_varType; ///< The class type of the variable if it is an instance type.
  int32_t        m_flags; ///< Is the instance variable static or not.

  VarRef();

  /// \brief Gets the bound tag name of the variable in the variable table name from the script name it
  /// was bound with.
  static void GetNameTag(SQChar *buffer, size_t size, const SQChar *scriptName);

  static SQRESULT GetVarInfo(StackHandler& sa, VarRef *&ref);
  static SQRESULT GetInstanceVarInfo(StackHandler& sa, VarRef *&ref, SQUserPointer& data);

  static SQInteger SetVar(StackHandler& sa, const VarRef *ref, void *data);
  static SQInteger GetVar(StackHandler& sa, const VarRef *ref, void *data);

  static SQInteger SetVarFunc(HSQUIRRELVM vm);
  static SQInteger GetVarFunc(HSQUIRRELVM vm);

  static SQInteger SetInstanceVarFunc(HSQUIRRELVM vm);
  static SQInteger GetInstanceVarFunc(HSQUIRRELVM vm);
};

}

#include "sqplus/VarRef.inl"

#endif
