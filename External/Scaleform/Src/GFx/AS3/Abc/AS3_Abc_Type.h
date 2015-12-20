/**************************************************************************

PublicHeader:   GFx
Filename    :   AS3_Abc_Type.h
Content     :   
Created     :   Jan, 2010
Authors     :   Sergey Sikorskiy

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_AS3_Abc_Type_H
#define INC_AS3_Abc_Type_H

#include "Kernel/SF_Array.h"
#include "Kernel/SF_ArrayPaged.h"
#include "Kernel/SF_StringHash.h"
#include "Kernel/SF_Alg.h"
#include "../AS3_StringManager.h"
#include "GFx/GFx_PlayerStats.h"

//#define VERSION_AIR

#ifdef SF_BUILD_DEBUG
#define GFX_AS3_TRACE
#endif

///////////////////////////////////////////////////////////////////////////////
#if 1
    #define REPORT_NOT_IMPLEMENTED
#else
/*
    // Minimalistic version of VM.
    #define AS3_NO_FLASH
    #ifdef GFX_AS3_VERBOSE
        #undef GFX_AS3_VERBOSE
    #endif
*/
#endif

///////////////////////////////////////////////////////////////////////////////
#if defined(GFX_AS3_VERBOSE)

#define SF_DEBUG_ARG(x) , x
#define SF_DEBUG_CODE(x) x

#else

#define SF_DEBUG_ARG(x)
#define SF_DEBUG_CODE(x)

#endif

#define WARN_NOT_IMPLEMENTED(x) GetVM().GetUI().Output(FlashUI::Output_Warning, "The method " x " is not implemented\n");
#define WARN_NOT_IMPLEMENTED_STATIC(x) vm.GetUI().Output(FlashUI::Output_Warning, "The method " x " is not implemented\n");

#define WARN_NOT_SUPPORTED_MISSING_DEPENDENCY(x) GetVM().GetUI().Output(FlashUI::Output_Warning, "The method " x " is not supported because core dependencies have been removed\n");

#ifdef REPORT_NOT_IMPLEMENTED
#define NOT_IMPLEMENTED(x) GetVM().ThrowError(VM::Error(VM::eNotImplementedError, GetVM() SF_DEBUG_ARG(x)));
    #define NOT_IMPLEMENTED_STATIC(x) vm.ThrowError(VM::Error(VM::eNotImplementedError, vm SF_DEBUG_ARG(x)));
#else
    #define NOT_IMPLEMENTED(x) WARN_NOT_IMPLEMENTED(x)
    #define NOT_IMPLEMENTED_STATIC(x) WARN_NOT_IMPLEMENTED_STATIC(x)
#endif


namespace Scaleform
{

///////////////////////////////////////////////////////////////////////////
enum { Mem_Stat = GFx::StatMV_VM };


///////////////////////////////////////////////////////////////////////////
// String utilities.

inline
String operator +(const char* l, const String& r)
{
    return String(l, r);
}

///////////////////////////////////////////////////////////////////////////
inline
String operator +(const char* l, const GFx::ASString& r)
{
    return String(l) + String(r.ToCStr(), r.GetSize());
}

inline
String operator +(const String& l, const GFx::ASString& r)
{
    return l + String(r.ToCStr(), r.GetSize());
}

} // namespace Scaleform


///////////////////////////////////////////////////////////////////////////
namespace Scaleform { namespace GFx { namespace AS3 
{

///////////////////////////////////////////////////////////////////////////
// Forward declaration.
class Object;
class Class;
class Value;
class VM;

/////////////////////////////////////////////////////////////////////////// 

// ***** SPtr<>

// SPtr is a smart pointer used for garbage-collectible classes within the VM.
// A special aspect of this class is that it uses lower bit of the pointer
// value as a marker to disable the next Release() call as needed for
// garbage collection of cycles. This bit is only applied by the garbage
// collector, allowing object destructor to be used instead of having an
// explicit finalize method, which would be error-prone.
//
// All member SPtr<> references to child objects must be referenced from
// the ForEachChild_GC() virtual function of that class for collector to
// see it properly.

template <typename T>
class SPtr
{
public:
    friend class Value;
    template <typename AT> friend class APtr;

    typedef T       ObjType;
    typedef SPtr<T> SelfType;

public:
    // Constructors.

    SPtr(ObjType* v = NULL)
        : pObject(v)
    {
        SF_ASSERT(v == NULL || (Scaleform::UPInt)v > 4);
        AddRef();
    }
    // Copy constructor.
    SPtr(const SPtr<ObjType>& other)
        : pObject(other.pObject)
    {
        AddRef();
    }
    ~SPtr()
    {
        Release();
    }

public:
    // Pick constructors.
    // We save on AddRef().

    SPtr(SelfType& other, PickType)
        : pObject(other.pObject)
    {
        other.pObject = NULL;
        // No AddRef() on purpose.
    }
    SPtr(Pickable<ObjType> v)
        : pObject(v.GetPtr())
    {
        SF_ASSERT(v.GetPtr() == NULL || (Scaleform::UPInt)v.GetPtr() > 4);
        // No AddRef() on purpose.
    }

public:
    // Assignment operator.
    SelfType& operator =(const SelfType& other)
    {
        return Set(other);
    }
    SelfType& operator =(ObjType* p)
    {
        return SetPtr(p);
    }
    SelfType& operator =(Pickable<ObjType> v)
    {
        return Pick(v);
    }

public:
    // Pickers.

    // Pick a value.
    SelfType& Pick(SelfType& other)
    {
        if (&other != this)
        {
            Release();

            pObject = other.pObject;
            other.pObject = NULL;
        }

        return *this;
    }

    SelfType& Pick(Pickable<ObjType> v)
    {
        if (v.GetPtr() != pObject)
        {
            Release();

            pObject = v.GetPtr();
        }

        return *this;
    }

    SelfType& Pick(ObjType* p)
    {
        if (p != pObject)
        {
            Release();

            pObject = p;
        }

        return *this;
    }

public:
    // Setters.

    SelfType& SetPtr(ObjType* p)
    {
        if (p != pObject)
        {
            SF_ASSERT(p == NULL || (Scaleform::UPInt)p > 4);
            Release();
            if (p)
                p->AddRef();

            pObject = p;
        }

        return *this;
    }
    SelfType& Set(const SelfType& other)
    {
        if (&other != this)
        {
            Release();
            other.AddRef();

            pObject = other.pObject;
        }

        return *this;
    }

public:
    // Getters.

    ObjType* operator->() const
    {
        return pObject;
    }
    ObjType& operator*() const
    {
        SF_ASSERT(pObject);
        return *pObject;
    }

    // Obtain pointer reference directly, for D3D interfaces
    ObjType*& GetRawRef() { return pObject; }
    ObjType** GetRawPtr() const { return &pObject; }
    ObjType*  GetPtr() const
    {
        return pObject;
    }
    // Conversion
    operator ObjType*() const
    {
        return pObject;
    }

private:
    void AddRef() const
    {
        if (pObject)
            pObject->AddRef();
    }
    void Release()
    {
        if (pObject)
        {
            if ((Scaleform::UPInt)pObject & 1)
                pObject = (ObjType*)((Scaleform::UPInt)pObject - 1);
            else
                pObject->Release();
        }
    }

private:
    mutable ObjType* pObject;
}; // SPtr

///////////////////////////////////////////////////////////////////////////
namespace ClassTraits
{
    class Traits;
}

typedef Pickable<ClassTraits::Traits> (*TTraitsFactory)(VM& vm);

struct TypeInfo
{
    enum {
        TimeTypeOffset = 0,
        ObjectTypeOffset = 1,
        ImplTypeOffset = 2,
        FinalTypeOffset = 3,
    };
    enum {
        TimeTypeMask = 1 << TimeTypeOffset,
        ObjectTypeMask = 1 << ObjectTypeOffset,
        ImplTypeMask = 1 << ImplTypeOffset,
        FinalTypeMask = 1 << FinalTypeOffset,
    };

    enum TimeType {RunTime = 0, CompileTime = 1 << TimeTypeOffset};
    enum ObjectType {SealedObject = 0, DynamicObject = 1 << ObjectTypeOffset};
    enum ImplType {Cpp = 0, Abc = 1 << ImplTypeOffset};
    enum FinalType {Alterable = 0, Final = 1 << FinalTypeOffset};

    bool IsCompileTime() const { return (Flags & TimeTypeMask) != 0; }
    bool IsDynamicObject() const { return (Flags & ObjectTypeMask) != 0; }
    bool IsAbcObject() const { return (Flags & ImplTypeMask) != 0; }
    bool IsFinalType() const { return (Flags & FinalTypeMask) != 0; }

    const char* GetName() const { return Name; }
    const char* GetPkgName() const { return PkgName; }
    const TypeInfo* GetParent() const { return Parent; }

    UPInt           Flags;
    const char*     Name;
    const char*     PkgName;
    const TypeInfo* Parent;
};

struct ThunkInfo;
struct MemberInfo;

struct ClassInfo
{
    const TypeInfo*     Type;
    TTraitsFactory      Factory;
    UInt8               ClassMethodNum;
    UInt8               ClassMemberNum;
    UInt8               InstanceMethodNum;
    UInt8               InstanceMemberNum;
    const ThunkInfo*    ClassMethod;
    const MemberInfo*   ClassMember;
    const ThunkInfo*    InstanceMethod;
    const MemberInfo*   InstanceMember;

    bool IsCompileTime() const { return Type->IsCompileTime(); }
    bool IsDynamicObject() const { return Type->IsDynamicObject(); }
    bool IsAbcObject() const { return Type->IsAbcObject(); }
    bool IsFinalType() const { return Type->IsFinalType(); }

    const char* GetName() const { return Type->GetName(); }
    const char* GetPkgName() const { return Type->GetPkgName(); }
    const TypeInfo* GetParent() const { return Type->GetParent(); }
};

namespace Classes
{
#ifdef GFX_AS3_SUPPORT
    extern const ClassInfo* ClassRegistrationTable[];
#endif
}

///////////////////////////////////////////////////////////////////////////
extern const ClassInfo ClassClassCI;

///////////////////////////////////////////////////////////////////////////
namespace Abc
{
    ///////////////////////////////////////////////////////////////////////////
    // Instruction offset within TOpCode (which is an OpCodeBuffer)
    typedef UPInt TCodeOffset;

    ///////////////////////////////////////////////////////////////////////////
    // NamespaceKind describes the type of namespace NamespaceInfo represents.
    enum NamespaceKind
    {
        NS_Undefined = -1,
        NS_Public,
        NS_Protected,
        NS_StaticProtected,
        NS_Private,  // Private NS shouldn't be interned (private namespaces don't match).
        NS_Explicit, // What is this for? Perhaps it identifies NSs with explicit URIs?
        NS_PackageInternal
    };

}

}}} // namespace Scaleform { namespace GFx { namespace AS3 {


#endif // INC_AS3_Abc_Type_H
