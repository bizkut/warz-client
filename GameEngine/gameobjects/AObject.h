#pragma once

#include "r3dProtect.h"

typedef r3dgameVector(r3dSTLString) stringlist_t;

//
//
//
#define DECLARE_BASE_CLASS(TClass, TParent)                     \
    public:                                                     \
    typedef TParent parent;                                     \
    static AObject  *ClassConstructor()                         \
    {                                                           \
      return game_new TClass;                     \
    }                                                           \
    public:

//
//
//
#define DECLARE_CLASS(TClass, TParent)                  \
        DECLARE_BASE_CLASS(TClass, TParent)             \
        static AClass   ClassData;                      \
    public:

//
//
//
#define IMPLEMENT_CLASS(TClass, name, type)                     \
  AClass TClass::ClassData                                      \
  (                                                             \
    &TClass::parent::ClassData,                                 \
    name,                                                       \
    type,                                                       \
    ( void *(*)(void))TClass::ClassConstructor                  \
  );                                                            \
  extern "C" AClass* classdata##TClass;                         \
  AClass* classdata##TClass = &TClass::ClassData;

//
//
//
#define AUTOREGISTER_CLASS(TClass)                              \
  static int REG_##TClass()                                     \
  {                                                             \
    return AObjectTable_RegisterClass(classdata##TClass);       \
  }                                                             \
  static int REG_##TClass##Init = REG_##TClass();



class AClass
{
  public:
    int         ID;
    r3dString   Name;
    r3dString   Type;
    AClass      *BaseClass;
    void        *(*ClassConstructor)(void);

            AClass(AClass *InBaseClass,
                   const char* InName,
                   const char* InType,
                   void *(*InClassConstructor)(void)
            ) : Name(InName),
                Type(InType),
                BaseClass(InBaseClass),
                ClassConstructor(InClassConstructor)
            {};
};

class AObject
{
  public:
    DECLARE_CLASS(AObject, AObject)
    r3dSec_type<AClass*, 0x44FAADE9> Class;
};

extern	int		AObjectTable_RegisterClass(AClass *cl);
extern	int		AObjectTable_GetClassID(const char* name, const char* type);
extern	AObject*	AObjectTable_CreateObject(int id);
extern stringlist_t AObjectTable_GetRegisteredClassesList ();
