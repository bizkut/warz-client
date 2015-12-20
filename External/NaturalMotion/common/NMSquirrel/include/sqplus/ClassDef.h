#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_CLASSDEF_H_
#define _SQPLUS_CLASSDEF_H_

#include "sqplus/Bind.h"
#include "sqplus/ClassHelpers.h"
#include "sqplus/BaseHeader.h"
#include "sqplus/DirectCall.h"
#include "sqplus/SquirrelVM.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief Wrapper class for binding squirrel class. This class cannot be used directly, use
/// sqp#ClassDef or sqp#ClassDefNoConstructor
template<typename TypeClass, typename BaseClass>
class ClassDefBase
{
public:
  /// \brief typedef for class method with variable arguments.
  typedef SQInteger (TypeClass:: *HSQMETHOD)(HSQUIRRELVM);

  /// \brief Bind a class method.
  /// \code
  /// class MyClass
  /// {
  /// public:
  ///   int Func(int p1, bool p2, float p3);
  /// };
  ///
  /// sqp::ClassDef<MyClass>(vm, _SC("MyClass")).Func(&MyClass::Func, _SC("func"));
  /// \endcode
  template<typename FuncType>
  ClassDefBase& Func(FuncType func, const SQChar *name);

  /// \brief
  /// \code
  /// class MyClass
  /// {
  /// public:
  ///   void Func(HSQUIRRELVM vm);
  /// };
  ///
  /// sqp::ClassDef<MyClass>(vm, _SC("MyClass")).FuncVarArgs(&MyClass::Func, _SC("func"));
  /// \endcode
  ClassDefBase& FuncVarArgs(
    HSQMETHOD func,
    const SQChar *name,
    SQInteger paramCount = 0,
    const SQChar *typemask = 0);

  /// \brief Binds a static class function as a static squirrel function.
  /// \code
  /// class MyClass
  /// {
  /// public:
  ///   static void Func(int p1, float p2);
  /// }
  ///
  /// sqp::ClassDef<MyClass>(vm, _SC("MyClass")).StaticFunc(&MyClass::Func, _SC("func"));
  /// \endcode
  template<typename FuncType>
  ClassDefBase& StaticFunc(FuncType func, const SQChar *name);

  /// \brief Registers a standard SQFUNCTION as a non-static member function of this class.
  /// \note the Static in the name refers to the c++/c function being static not the squirrel
  /// class function.
  /// \code
  /// class MyClass
  /// {
  /// public:
  ///   static int example1(HSQUIRREL vm);
  /// }
  /// int example2(HSQUIRREL vm);
  ///
  /// sqp::ClassDef<MyClass>(vm, _SC("MyClass")).SquirrelFunc(&MyClass::example1, _SC("example1"));
  /// sqp::ClassDef<MyClass>(vm, _SC("MyClass")).SquirrelFunc(&example2, _SC("example2"));
  /// \endcode
  ClassDefBase& SquirrelFunc(
    SQFUNCTION func,
    const SQChar *name,
    bool isstatic = false);

  /// \brief Allows the additional setting of a typemask.
  /// \overload
  ClassDefBase& SquirrelFunc(
    SQFUNCTION func,
    const SQChar *name,
    SQInteger nparamscheck,
    const SQChar *typemask,
    bool isstatic = false);

  /// \brief Bind a normal member function of a singleton class as a static function in squirrel.
  /// \code
  /// class Singleton
  /// {
  /// public:
  ///   static Singleton *GetInstance();
  ///   void Func();
  /// }
  ///
  /// sqp::ClassDefNoConstructor(vm, "Singleton")
  ///   .SingletonFunc(Singleton::GetInstance, &Singleton::Func, _SC("Func")
  ///   ;
  /// \endcode
  template<typename FuncType>
  ClassDefBase& SingletonFunc(TypeClass *instance, FuncType func, const SQChar *name);

  /// \brief Bind a variable argument member function of a singleton class as a static function in squirrel.
  /// \code
  /// class Singleton
  /// {
  /// public:
  ///   static Singleton *GetInstance();
  ///   SQInteger Func(HSQUIRRELVM vm);
  /// }
  ///
  /// sqp::ClassDefNoConstructor(vm, "Singleton")
  ///   .SingletonFuncVarArgs(Singleton::GetInstance, &Singleton::Func, _SC("Func")
  ///   ;
  /// \endcode
  ClassDefBase& SingletonFuncVarArgs(
    TypeClass *instance,
    HSQMETHOD func,
    const SQChar *name);

  template<typename VarType>
  ClassDefBase& Var(VarType TypeClass:: *var, const SQChar *name);

  template<typename VarType>
  ClassDefBase& VarAsUserPointer(VarType TypeClass:: *var, const SQChar *name);

  template<typename VarType>
  ClassDefBase& StaticVar(VarType TypeClass:: *var, const SQChar *name);

  ClassDefBase& EnumVar(SQInteger value, const SQChar *name);

protected:
  SquirrelVM      m_vm;
  SquirrelObject  m_class;
  const SQChar   *m_name;
  const SQChar   *m_base;

  /// \brief 
  ClassDefBase(SquirrelVM& vm, const SQChar *name, const SQChar *base, SQRELEASEHOOK hook);

  /// \brief Disable copy constructor
  ClassDefBase(const ClassDefBase& rhs);

  /// \brief Disable assignment operator
  const ClassDefBase& operator = (const ClassDefBase& rhs);

  /// \brief Sets the base type of ClassType<TypeClass> class
  template<typename Base>
  void InitBase(TypeWrapper<Base>);
  /// \brief InitBase overload for classes with no base class.
  void InitBase(TypeWrapper<NoBaseClass>);
  /// \brief Set the release hook for the class.
  void InitReleaseHook(SQRELEASEHOOK hook);
  /// \brief Sets the table that the class belongs to
  void InitNamespace(const SquirrelObject &namespaceTable);
  /// \brief Check m_name is set and set it using TypeInfo if it isn't.
  void CheckInitDefaultNames();
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Binds a class with a default constructor and typeof function.
template<typename TypeClass, typename BaseClass = NoBaseClass>
class ClassDef : public ClassDefBase<TypeClass, BaseClass>
{
  // helps resolve unqualified names to members of the dependent base class.
  using ClassDefBase<TypeClass, BaseClass>::m_class;
public:
  ClassDef(
    SquirrelVM& vm,
    const SQChar *name = 0,
    const SQChar *base = 0,
    SQRELEASEHOOK hook = DefaultReleaseHook<TypeClass>);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Binds a class with the typeof function overloaded.
template<typename TypeClass, typename BaseClass = NoBaseClass>
class ClassDefNoConstructor : public ClassDefBase<TypeClass, BaseClass>
{
  // helps resolve unqualified names to members of the dependent base class.
  using ClassDefBase<TypeClass, BaseClass>::m_class;
public:
  ClassDefNoConstructor(SquirrelVM& vm, const SQChar *name = 0, const SQChar *base = 0);
};

/*Initializing the implicit object parameter 'Type' (a non-const reference) with a non-lvalue*/
/*lint -esym(1793,sqp::ClassDefBase<*,*>)*/
/*Ignoring return value of function*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::Func)*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::FuncVarArgs)*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::GlobalMemberFunc)*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::GlobalMemberFuncVarArgs)*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::StaticFunc)*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::StaticFuncVarArgs)*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::Var)*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::VarAsUserPointer)*/
/*lint -esym(534,sqp::ClassDefBase<*,*>::StaticVar)*/

}

#include "sqplus/ClassDef.inl"

#endif
