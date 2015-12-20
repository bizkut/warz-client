#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_BIND_H_
#define _SQPLUS_BIND_H_
//----------------------------------------------------------------------------------------------------------------------
/// \file Bind.h
/// \author John Schultz
//----------------------------------------------------------------------------------------------------------------------

#include "sqplus/ClassHelpers.h"
#include "sqplus/DirectCall.h"
#include "sqplus/SquirrelVM.h"
#include "sqplus/TypeInfo.h"
#include "sqplus/VarRef.h"

namespace sqp
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Bind a global or static member variable to a table.
template<typename VarType>
inline bool BindVariable(
  SquirrelVM &vm,
  SquirrelObject &object,
  VarType *variable,
  const SQChar *name,
  int flags = 0);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Register a class instance member variable to a class.
template<typename InstanceType, typename VarType>
inline bool RegisterInstanceVariable(
  SquirrelVM &vm,
  SquirrelObject &object,
  VarType InstanceType:: *variable,
  const SQChar *name,
  int flags = 0);

/// \brief Register a class with an optional base class to the root table.
/// \note Binds sqp#DefaultConstructor as "constructor" for class type and sqp#TypeOf as "_typeof" metamethod.
template<typename T>
inline SquirrelObject RegisterClassType(SquirrelVM &vm, const SQChar *className, const SQChar *baseName = 0);

/// \brief Registers a class with an optional bass class to the root table. The _typeof metamethod will be
/// overridden so typeof will return className.
template<typename T>
inline SquirrelObject RegisterClassTypeNoConstructor(SquirrelVM &vm, const SQChar *className, const SQChar *baseName = 0);

}

#include "sqplus/Bind.inl"

#endif
