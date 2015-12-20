#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_CLASSHELPERS_H_
#define _SQPLUS_CLASSHELPERS_H_
//----------------------------------------------------------------------------------------------------------------------
/// \file ClassHelpers.h
//----------------------------------------------------------------------------------------------------------------------
#include <new>

#include "sqplus/ClassType.h"
#include "sqplus/SquirrelVM.h"
#include "sqplus/StackHandler.h"

namespace sqp
{
/// \brief Default release hook for bound class instances that assumes the class was allocated with sq_malloc
/// Calls the destructor and then calls sq_free.
template<typename T>
SQInteger DefaultReleaseHook(SQUserPointer up, SQInteger size);

/// \brief Default function used as _typeof metamethod func of classes bound with #RegisterClassType or
/// #RegisterClassTypeNoConstructor.
template<typename T>
SQInteger TypeOf(HSQUIRRELVM vm);

/// \brief Sets newClass as the instance user pointer and hook as the release hook of the new class instance
/// The new instance of a class will be the first parameter passed to the constructor so will be at index 1
/// in the stack.
template<typename T>
inline SQInteger PostConstruct(HSQUIRRELVM vm, T *newClass, SQRELEASEHOOK hook);

/// \brief Templated default constructor function to provide a script class constructor. Template class must
/// have implemented a default constructor and a copy constructor.
/// \code
/// class MyClass
/// {
/// public:
///   MyClass();
///   MyClass(const MyClass &copy);
/// };
/// ...
/// vm.Push(myClassObject);
/// vm.CreateFunction(&DefaultConstructor<MyClass>, _SC("constructor"));
/// vm.PopTop();
/// \endcode
template<typename T>
SQInteger DefaultConstructor(HSQUIRRELVM vm);

/// \brief Creates an instance of a native class without invoking the constructor leaving it on the stack.
bool CreateNativeClassInstance(HSQUIRRELVM vm, HSQOBJECT sqnamespace, const SQChar *classname, SQUserPointer ud, SQRELEASEHOOK hook);

}

#include "sqplus/ClassHelpers.inl"

#endif
