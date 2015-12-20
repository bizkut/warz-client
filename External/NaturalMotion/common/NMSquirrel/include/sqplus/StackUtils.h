#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_STACKUTILS_H_
#define _SQPLUS_STACKUTILS_H_
//----------------------------------------------------------------------------------------------------------------------
/// \file sqplus/StackUtils.h
/// \author James Whitworth
//----------------------------------------------------------------------------------------------------------------------

#include "squirrel/squirrel.h"

#include "sqplus/BaseHeader.h"
#include "sqplus/SquirrelObject.h"

namespace sqp
{
/// \brief Pushes a value onto the stack, there is no default implementation of the template versions
/// of this function. Specialisations are declared when the \link INSTANCE_DECL instance binding macros \endlink
/// are used.
template<typename T>
bool Push(HSQUIRRELVM vm, T *value);

/// \overload
template<typename T>
bool Push(HSQUIRRELVM vm, T &value);

/// \overload
template<typename T>
bool Push(HSQUIRRELVM vm, const T *value);

/// \overload
template<typename T>
bool Push(HSQUIRRELVM vm, const T &value);

/// \brief Checks that an object on the stack is of a particular type. 
template<typename T>
bool Match(TypeWrapper<T *>, HSQUIRRELVM vm, SQInteger index);

/// \overload
template<typename T>
bool Match(TypeWrapper<T &>, HSQUIRRELVM vm, SQInteger index);

/// \overload
template<typename T>
bool Match(TypeWrapper<const T *>, HSQUIRRELVM vm, SQInteger index);

/// \overload
template<typename T>
bool Match(TypeWrapper<const T &>, HSQUIRRELVM vm, SQInteger index);

/// \brief Gets a typed value from the stack at the index specified. If the value is
/// a reference then it will be invalid when it is removed from the stack. To maintain
/// the value either copy the result or use the overload that returns a SquirrelObject
/// which maintains a strong reference on the stack.
/// Get should be used in conjunction with Match as no type checking is done.
template<typename T>
T *Get(TypeWrapper<T*>, HSQUIRRELVM vm, SQInteger index);

/// \overload
template<typename T>
T &Get(TypeWrapper<T&>, HSQUIRRELVM vm, SQInteger index);

/// \overload
template<typename T>
const T *Get(TypeWrapper<const T *>, HSQUIRRELVM vm, SQInteger index);

/// \overload
template<typename T>
const T &Get(TypeWrapper<const T &>, HSQUIRRELVM vm, SQInteger index);

/// \overload
bool Push(HSQUIRRELVM vm, bool value);
/// \overload
bool Push(HSQUIRRELVM vm, char value);
/// \overload
bool Push(HSQUIRRELVM vm, unsigned char value);
/// \overload
bool Push(HSQUIRRELVM vm, short value);
/// \overload
bool Push(HSQUIRRELVM vm, unsigned short value);
/// \overload
bool Push(HSQUIRRELVM vm, int value);
/// \overload
bool Push(HSQUIRRELVM vm, unsigned int value);
/// \overload
bool Push(HSQUIRRELVM vm, long value);
/// \overload
bool Push(HSQUIRRELVM vm, unsigned long value);
/// \overload
bool Push(HSQUIRRELVM vm, float value);
/// \overload
bool Push(HSQUIRRELVM vm, double value);
/// \overload
bool Push(HSQUIRRELVM vm, const SQChar *value);
/// \overload
bool Push(HSQUIRRELVM vm, SQFUNCTION value);
/// \overload
bool Push(HSQUIRRELVM vm, SquirrelObject &value);

/// \overload
bool Match(TypeWrapper<bool>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<char>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<unsigned char>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<short>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<unsigned short>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<int>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<unsigned int>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<long>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<unsigned long>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<float>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<double>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<const SQChar*>, HSQUIRRELVM vm, SQInteger index);
/// \overload
bool Match(TypeWrapper<SquirrelObject>, HSQUIRRELVM, SQInteger);

/// \overload
bool            Get(TypeWrapper<bool>, HSQUIRRELVM vm, SQInteger index);
/// \overload
char            Get(TypeWrapper<char>, HSQUIRRELVM vm, SQInteger index);
/// \overload
unsigned char   Get(TypeWrapper<unsigned char>, HSQUIRRELVM vm, SQInteger index);
/// \overload
short           Get(TypeWrapper<short>, HSQUIRRELVM vm, SQInteger index);
/// \overload
unsigned short  Get(TypeWrapper<unsigned short>, HSQUIRRELVM vm, SQInteger index);
/// \overload
int             Get(TypeWrapper<int>, HSQUIRRELVM vm, SQInteger index);
/// \overload
unsigned int    Get(TypeWrapper<unsigned int>, HSQUIRRELVM vm, SQInteger index);
/// \overload
long            Get(TypeWrapper<long>, HSQUIRRELVM vm, SQInteger index);
/// \overload
unsigned long   Get(TypeWrapper<unsigned long>, HSQUIRRELVM vm, SQInteger index);
/// \overload
float           Get(TypeWrapper<float>, HSQUIRRELVM vm, SQInteger index);
/// \overload
double          Get(TypeWrapper<double>, HSQUIRRELVM vm, SQInteger index);
/// \overload
const SQChar   *Get(TypeWrapper<const SQChar*>, HSQUIRRELVM vm, SQInteger index);
/// \overload
SquirrelObject  Get(TypeWrapper<SquirrelObject>, HSQUIRRELVM vm, SQInteger index);

/// \brief Gets the return type of a function and pops the parameters.
/// Assumes the call was made with retval as SQTrue otherwise the return value
/// will be garbage. To ensure the return value is not lost when the call to sq_pop
/// is made the SquirrelObject ref is used to add a strong reference to the object.
/// The return value will be valid for the lifetime of the SquirrelObject ref.
template<typename RT>
RT GetRet(TypeWrapper<RT>, HSQUIRRELVM vm, SQInteger index, SquirrelObject *ref);

}

#include "sqplus/StackUtils.inl"

#endif
