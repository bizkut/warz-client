/**************************************************************************

Filename    :   AS3_Value.h
Content     :   
Created     :   Jan, 2010
Authors     :   Sergey Sikorskiy

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_AS3_Value_H
#define INC_AS3_Value_H

#include "Abc/AS3_Abc_Type.h"
#include "GFx/GFx_ASUtils.h" // For NumberUtil

// We can save memory by not saving classes as slots of Global Objects.
// Saving Classes as Slots of Global Objects is default behavior of AVM2.
#define SF_AS3_CLASS_AS_SLOT

// #define SF_AS3_AOTC

#if defined(SF_AS3_AOTC)
    #define SF_AOTC_ARG(x) , x
    #define SF_AOTC_CODE(x) x
#else
    #define SF_AOTC_ARG(x)
    #define SF_AOTC_CODE(x)
#endif

///////////////////////////////////////////////////////////////////////////////
namespace Scaleform { namespace GFx { namespace AS3 
{

///////////////////////////////////////////////////////////////////////////
// Forward declaration ...
class Object;
class Class;
class VM;
class Traits;
class Value;
class GASRefCountBase;
class WeakProxy;
class SNode;

namespace Instances
{
    class Function;
    class FunctionBase;
    class ThunkFunction;
    class Namespace;
}

namespace Instances { 
    namespace fl
    {
        class GlobalObject;
    }
}

namespace InstanceTraits
{
    class Traits;
}

namespace ClassTraits
{
    class Traits;
}

///////////////////////////////////////////////////////////////////////////
// CheckResult is a helper class that represents a boolean value, which MUST be 
// checked. CheckResult is used primarily to to ensure that exceptions
// are checked for after an operation that might've thrown an exception.

class CheckResult
{
public:
    CheckResult(bool r)
        : Result(r)

#ifdef SF_BUILD_DEBUG
        , Checked(false)
#endif
    {
    }
    CheckResult(const CheckResult& other)
        : Result(other.Result)

#ifdef SF_BUILD_DEBUG
        , Checked(other.Checked)
#endif
    {
#ifdef SF_BUILD_DEBUG
        // Pick semantic.
        other.Checked = true;
#endif
    }
    ~CheckResult()
    {
        SF_ASSERT(Checked);
    }

public:
    void DoNotCheck() const
    {
#ifdef SF_BUILD_DEBUG
        Checked = true;
#endif
    }
    operator bool() const
    {
#ifdef SF_BUILD_DEBUG
        Checked = true;
#endif

        return Result;
    }
    bool operator ==(bool v) const
    {
#ifdef SF_BUILD_DEBUG
        Checked = true;
#endif

        return Result == v;
    }
    bool operator !() const
    {
#ifdef SF_BUILD_DEBUG
        Checked = true;
#endif

        return !Result;
    }

    CheckResult& operator =(bool v)
    {
        Result = v;
        return *this;
    }

private:
    CheckResult& operator =(const CheckResult& other);

private:
    bool Result;

#ifdef SF_BUILD_DEBUG
    mutable bool Checked;
#endif
};

///////////////////////////////////////////////////////////////////////////
// Type of code used with virtual table.
enum TCodeType 
{
    CT_Method = 0U,
    CT_Get = 1U,
    CT_Set = 2U
};

struct ThunkInfo 
{
    typedef void (*TThunkFunc)(const ThunkInfo& ti, VM& vm, const Value& _this, Value& result, unsigned argc, const Value* argv);

    TThunkFunc Method;

    const TypeInfo* ResultType;

    const char* Name;

    //Abc::NamespaceKind NsKind;
    const char* NamespaceName;

    TCodeType CodeType_:3; // One bit can be saved.

    unsigned MinArgNum_:3;
    unsigned MaxArgNum_:10;

    TCodeType GetCodeType() const
    {
        return CodeType_;
    }
    unsigned GetMinArgNum() const
    {
        return MinArgNum_;
    }
    unsigned GetMaxArgNum() const
    {
        return MaxArgNum_;
    }

    static void EmptyFunc(const ThunkInfo& ti, VM& vm, const Value& _this, Value& result, unsigned argc, const Value* argv);
};

///////////////////////////////////////////////////////////////////////////
enum StrongRefType { StrongRefValue };
class ValueStack;

class Value
{
public:
    typedef Double Number;

    // DO NOT reorder values.
    enum KindType
    {
        kUndefined = 0,
        kBoolean,
        kInt,
        kUInt,
        kNumber,

        kThunk,
        kMethodInd, // kMethodInd should be removed.
        kVTableInd, // Index in a virtual table.

        // Value kinds used by the Tracer.
        kInstanceTraits,
        kClassTraits,

        // Below are ref-counted tags.
#ifdef SF_AS3_AOTC
        kSNodeIT, // SNode + InstanceTraits
        kSNodeCT, // SNode + kClassTraits
#endif
        kString,
        kNamespace, // Namespace is a primitive type.

        kObject,
        kClass,
        kFunction,
        // ThunkFunction is a Function object, which stores a Thunk.
        kThunkFunction,
        kThunkClosure,
        kVTableIndClosure,
    };

    enum Hint {hintNone, hintNumber, hintString};
    enum TraceNullType {NotNull, Null, NullOrNot};
    enum NotRefCountedType { NotRefCounted };

    // Do not use bit 0 with ObjectTag. This bit is reserved for GC.
    enum ObjectTag {otInvalid = -1, otObject = 0, otClass = 2, otFunction = 4, otNamespace = 6, ObjectTagMask = 0x06};

public:
    // Generic copy constructor.
    SF_INLINE
    Value(const Value& other)
    : Flags(other.Flags)
    , Bonus(other.Bonus)
    , value(other.value)
    {
        other.AddRef();
    }

    // Copy constructor for not reference counted values.
    SF_INLINE
    Value(const Value& other, NotRefCountedType)
    : Flags(other.Flags)
    , Bonus(other.Bonus)
    , value(other.value)
    {
        // No AddRef() is necessary.
    }

    // Pick value from the stack and pop it.
    Value(ValueStack& v);
    Value(const Value& other, StrongRefType);
    
    SF_INLINE
    Value() : Flags(kUndefined)
    {
    }
    SF_INLINE
    Value(SInt32 v, const Traits& tr, KindType k)
    : Flags(k)
    , value(v, tr)
    {
        SF_ASSERT(k == kMethodInd || k == kVTableInd);
    }
    SF_INLINE
    explicit Value(bool v)
    : Flags(kBoolean)
    , value(v)
    {
    }
    SF_INLINE
    explicit Value(SInt32 v)
    : Flags(kInt)
    , value(v)
    {
    }
#if defined(SF_OS_MAC) || defined(SF_OS_IPHONE) || defined(SF_64BIT_POINTERS) || defined(SF_CC_MSVC)
    SF_INLINE
    explicit Value(UPInt v)
    : Flags(kUInt)
    , value(static_cast<UInt32>(v))
    {
		SF_ASSERT(v <= SF_MAX_UINT32);
    }
#endif
    SF_INLINE
    explicit Value(UInt32 v)
    : Flags(kUInt)
    , value(v)
    {
    }
    SF_INLINE
    explicit Value(Number v)
    : Flags(kNumber)
    , value(v)
    {
    }
    SF_INLINE
    Value(const ThunkInfo& v)
    : Flags(kThunk)
    , value(v)
    {
    }
    Value(const ASString& v);
    Value(ASStringNode* v);

    // This constructor is declared as explicit to prevent unexpected type convention.
    template <typename OT>
    explicit Value(const SPtr<OT>& v) : Flags(kUndefined) { AssignUnsafe(v); }
    // Pick semantic.
    template <typename OT>
    Value(SPtr<OT>& v, PickType) : Flags(kUndefined) { PickUnsafe(v); }


    explicit Value(Object* v);
    explicit Value(Class* v);
    explicit Value(Instances::Function* v);
    explicit Value(Instances::ThunkFunction* v);
    explicit Value(Instances::Namespace* v);

    SF_INLINE
    Value(Object* p, const ThunkInfo& f) 
        : Flags(kThunkClosure)
        , value(p, f)
    { 
        SF_ASSERT(p);
        AddRefClosure();
    }
    SF_INLINE
    Value(Object* p, SInt32 ind, bool super) 
        : Flags(kVTableIndClosure)
        , value(p, ind)
    { 
        SF_ASSERT(p);
        AddRefClosure();
        SetSuperCall(super);
    }

    ~Value()
    {
        Release();
    }

public:
    // Tracer related.
    SF_INLINE
    Value(InstanceTraits::Traits& tr, TraceNullType isNull)
        : Flags(kInstanceTraits)
        , value(tr)
    {
        SetTraceNullType(isNull);
        SetWith(false);
    }
    SF_INLINE
    Value(ClassTraits::Traits& tr, TraceNullType isNull)
        : Flags(kClassTraits)
        , value(tr)
    {
        SetTraceNullType(isNull);
        SetWith(false);
    }

#ifdef SF_AS3_AOTC
    Value(InstanceTraits::Traits& tr, TraceNullType isNull, SNode& n);
    Value(ClassTraits::Traits& tr, TraceNullType isNull, SNode& n);

    void UpdateTraits(InstanceTraits::Traits& tr);
    void UpdateTraits(ClassTraits::Traits& tr);

    void UpdateSNode(SNode& n);
    void UpdateSNode(SNode& n, TraceNullType isNull);
#endif

public:
    // Pick constructors.

    // They save one AddRef and one Release (which is expensive) during 
    // destruction of the "other" value.

    SF_INLINE
    Value(Value& other, PickType)
        : Flags(other.Flags)
        , Bonus(other.Bonus)
        , value(other.value)
    {
        other.SetFlags(0); // Clean up all flags.
    }
    SF_INLINE
    Value(Object* p, const ThunkInfo& f, PickType) 
        : Flags(kThunkClosure)
        , value(p, f)
    { 
        SF_ASSERT(p);
        // No AddRef() is necessary.
    }

    template <typename OT>
    Value(Pickable<OT> v) : Flags(0) { PickUnsafe(v); }

public:
    ///
    void Assign(const Value& other);
    void AssignUnsafe(const Value& other);

    ///
    SF_INLINE
    void Assign(bool v)
    {
        Release();
        SetKind(kBoolean);
        value = v;
    }
    SF_INLINE
    void AssignUnsafe(bool v)
    {
        SF_ASSERT(!IsRefCounted());

        SetKind(kBoolean);
        value = v;
    }

    ///
    SF_INLINE
    void Assign(SInt32 v)
    {
        Release();
        SetKind(kInt);
        value = v;
    }
    SF_INLINE
    void AssignUnsafe(SInt32 v)
    {
        SF_ASSERT(!IsRefCounted());

        SetKind(kInt);
        value = v;
    }

    ///
    SF_INLINE
    void Assign(UInt32 v)
    {
        Release();
        SetKind(kUInt);
        value = v;
    }
    SF_INLINE
    void AssignUnsafe(UInt32 v)
    {
        SF_ASSERT(!IsRefCounted());

        SetKind(kUInt);
        value = v;
    }

    ///
    SF_INLINE
    void Assign(Number v)
    {
        Release();
        SetKind(kNumber);
        value = v;
    }
    SF_INLINE
    void AssignUnsafe(Number v)
    {
        SF_ASSERT(!IsRefCounted());

        SetKind(kNumber);
        value = v;
    }

    ///
    SF_INLINE
    void Assign(const ThunkInfo& v)
    {
        Release();
        SetKind(kThunk);
        value = v;
    }
    SF_INLINE
    void AssignUnsafe(const ThunkInfo& v)
    {
        SF_ASSERT(!IsRefCounted());

        SetKind(kThunk);
        value = v;
    }

    ///
    void Assign(const ASString& v);
    void AssignUnsafe(const ASString& v);

    ///
    void Assign(ASStringNode* v);
    void AssignUnsafe(ASStringNode* v);

    ///
    template <typename OT>
    void Assign(const SPtr<OT>& v) { Assign(v.GetPtr()); }
    template <typename OT>
    void AssignUnsafe(const SPtr<OT>& v) { AssignUnsafe(v.GetPtr()); }


    ///
    void Assign(Object* v);
    void AssignUnsafe(Object* v);

    ///
    void Assign(Class* v);
    void AssignUnsafe(Class* v);

    ///
    void Assign(Instances::Function* v);
    void AssignUnsafe(Instances::Function* v);

    ///
    void Assign(Instances::ThunkFunction* v);
    void AssignUnsafe(Instances::ThunkFunction* v);

    ///
    void Assign(Instances::Namespace* v);
    void AssignUnsafe(Instances::Namespace* v);

public:
    // Assignment operators below eliminate a temporary Value object
    // and an expensive Release() call.
    Value& operator =(const Value& other) { Assign(other); return *this; }
    Value& operator =(const ThunkInfo& v) { Assign(v); return *this; }
    Value& operator =(const ASString& v) { Assign(v); return *this; }
    Value& operator =(ASStringNode* v) { Assign(v); return *this; }
    Value& operator =(Object* v) { Assign(v); return *this; }
    Value& operator =(Class* v) { Assign(v); return *this; }
    Value& operator =(Instances::Function* v) { Assign(v); return *this; }
    Value& operator =(Instances::ThunkFunction* v) { Assign(v); return *this; }
    Value& operator =(Instances::Namespace* v) { Assign(v); return *this; }
    template <typename OT>
    Value& operator =(const SPtr<OT>& v) { Assign(v); return *this; }

public:
    // Pick value from another value.
    // It saves one AddRef/Release.

    ///
    template <typename OT>
    void Pick(SPtr<OT>& v)
    {
        Pick(v.Get());
        v.pObject = NULL;
    }
    template <typename OT>
    void PickUnsafe(SPtr<OT>& v)
    {
        PickUnsafe(v.GetPtr());
        v.pObject = NULL;
    }
    template <typename OT>
    void Pick(Pickable<OT> v) { Pick(v.GetPtr()); }
    template <typename OT>
    void PickUnsafe(Pickable<OT> v) { PickUnsafe(v.GetPtr()); }

    ///
    void Pick(Value& other);
    void PickUnsafe(Value& other);

    ///
    void Pick(Object* v);
    void PickUnsafe(Object* v);

    ///
    void Pick(Class* v);
    void PickUnsafe(Class* v);

    ///
    void Pick(Instances::Function* v);
    void PickUnsafe(Instances::Function* v);
    // Special case.
    void Pick(Instances::FunctionBase* v);
    void PickUnsafe(Instances::FunctionBase* v);

    ///
    void Pick(Instances::ThunkFunction* v);
    void PickUnsafe(Instances::ThunkFunction* v);

    ///
    void Pick(Instances::Namespace* v);
    void PickUnsafe(Instances::Namespace* v);

    /// Pick value from the stack and pop it.
    void Pick(ValueStack& v);
    void PickUnsafe(ValueStack& v);

public:
    template <typename OT>
    Value& operator =(Pickable<OT> v) { Pick(v); return *this; }

public:
    // A bunch of "Set" functions, which are developed to avoid implicit type conversion.
    // (They correspond to explicit constructors.)
    // You can call "Unsafe" version if you know for sure that previous value is not
    // reference countable. (This is done for optimization)
    void SetUndefined()
    {
        Release();
        SetKind(kUndefined);
    }
    SF_INLINE
    void SetUndefinedUnsafe()
    {
        SF_ASSERT(!IsRefCounted());
        SetKind(kUndefined);
    }
    void SetNull()
    {
        Release();
        SetKind(kObject);
        value = (Object*)NULL;
    }
    SF_INLINE
    void SetNullUnsafe()
    {
        SF_ASSERT(!IsRefCounted());
        SetKind(kObject);
        value = (Object*)NULL;
    }
    void SetBool(bool v)
    {
        Release();
        SetKind(kBoolean);
        value = v;
    }
    SF_INLINE
    void SetBoolUnsafe(bool v)
    {
        SF_ASSERT(!IsRefCounted());
        SetKind(kBoolean);
        value = v;
    }
    void SetSInt32(SInt32 v)
    {
        Release();
        SetKind(kInt);
        value = v;
    }
    SF_INLINE
    void SetSInt32Unsafe(SInt32 v)
    {
        SF_ASSERT(!IsRefCounted());
        SetKind(kInt);
        value = v;
    }
    void SetUPInt(UPInt v)
    {
        SF_ASSERT(v <= SF_MAX_UINT32);
        Release();
        SetKind(kUInt);
        value = static_cast<UInt32>(v);
    }
    SF_INLINE
    void SetUPIntUnsafe(UPInt v)
    {
        SF_ASSERT(v <= SF_MAX_UINT32);
        SF_ASSERT(!IsRefCounted());
        SetKind(kUInt);
        value = static_cast<UInt32>(v);
    }
    void SetUInt32(UInt32 v)
    {
        Release();
        SetKind(kUInt);
        value = v;
    }
    SF_INLINE
    void SetUInt32Unsafe(UInt32 v)
    {
        SF_ASSERT(!IsRefCounted());
        SetKind(kUInt);
        value = v;
    }
    void SetNumber(Number v)
    {
        Release();
        SetKind(kNumber);
        value = v;
    }
    SF_INLINE
    void SetNumberUnsafe(Number v)
    {
        SF_ASSERT(!IsRefCounted());
        SetKind(kNumber);
        value = v;
    }    

    SF_INLINE
    void SetWith(bool v = true)
    {
        // Conditionally set or clear bits without branching.
        Flags = (Flags & ~withMask) | (-static_cast<SInt32>(v) & withMask);
    }

    SF_INLINE
    void SetWeakRef(bool v = true)
    {
        // Conditionally set or clear bits without branching.
        Flags = (Flags & ~weakRefMask) | (-static_cast<SInt32>(v) & weakRefMask);
    }

    SF_INLINE
    void SetArtificialClass(bool v = true)
    {
        // Conditionally set or clear bits without branching.
        Flags = (Flags & ~artClassMask) | (-static_cast<SInt32>(v) & artClassMask);
    }

    // This is a temporary method.
    SF_INLINE
    void SetSuperCall(bool v = true)
    {
        // Conditionally set or clear bits without branching.
        Flags = (Flags & ~superCallMask) | (-static_cast<SInt32>(v) & superCallMask);
    }

    // This method is optimized for performance.
    // No AddRef/Release will be called.
    void Swap(Value& other);

    SF_INLINE
    void SetTraceNullType(TraceNullType v)
    {
        // Two bits.
        Flags = (Flags & ~valueTypeMask) | (static_cast<UInt32>(v) << valueTypeOffset);
    }

public:
    ///
    operator ASStringNode*() const
    {
        SF_ASSERT(GetKind() == kString);
        return value;
    }
    operator ASString() const
    {
        SF_ASSERT(GetKind() == kString);
        return ASString(value);
    }

    ///
    operator bool&()
    {
        SF_ASSERT(GetKind() == kBoolean);
        return value.operator bool&();
    }
    operator bool() const
    {
        SF_ASSERT(GetKind() == kBoolean);
        return value.operator bool();
    }

    ///
    operator SInt32&()
    {
        SF_ASSERT(GetKind() == kInt || GetKind() == kUInt);
        return value;
    }
    operator SInt32() const
    {
        SF_ASSERT(GetKind() == kInt || GetKind() == kUInt);
        return value;
    }

    ///
    operator UInt32&()
    {
        SF_ASSERT(GetKind() == kInt || GetKind() == kUInt);
        return value;
    }
    operator UInt32() const
    {
        SF_ASSERT(GetKind() == kInt || GetKind() == kUInt);
        return value.operator UInt32();
    }

    ///
    operator Number&()
    {
        SF_ASSERT(GetKind() == kNumber);
        return value;
    }
    operator Number() const
    {
        SF_ASSERT(GetKind() == kNumber);
        return value;
    }

    ///
    operator SPtr<Object>() const;

    ///
    operator Object&() const
    {
        SF_ASSERT(IsObject());
        SF_ASSERT(static_cast<Object*>(value));
        return *static_cast<Object*>(value);
    }

    ///
    operator Class&() const
    {
        SF_ASSERT(IsClass());
        SF_ASSERT(static_cast<Class*>(value));
        return *static_cast<Class*>(value);
    }

    ///
    operator Instances::Function&() const
    {
        SF_ASSERT(IsFunction());
        SF_ASSERT(static_cast<Instances::Function*>(value));
        return *static_cast<Instances::Function*>(value);
    }

    ///
    operator const ThunkInfo&() const
    {
        SF_ASSERT(IsThunk());
        return value;
    }

    ///
    operator Instances::ThunkFunction&() const
    {
        SF_ASSERT(IsThunkFunction());
        SF_ASSERT(static_cast<Instances::ThunkFunction*>(value));
        return *static_cast<Instances::ThunkFunction*>(value);
    }

    ///
    operator Instances::Namespace&() const
    {
        SF_ASSERT(IsNamespace());
        SF_ASSERT(static_cast<Instances::Namespace*>(value));
        return *static_cast<Instances::Namespace*>(value);
    }
    
public:
    // Convert to not Value data types.

    //
    bool& AsBool()
    {
        SF_ASSERT(GetKind() == kBoolean);
        return value;
    }
    bool AsBool() const
    {
        SF_ASSERT(GetKind() == kBoolean);
        return value;
    }

    //
    SInt32& AsInt()
    {
        SF_ASSERT(GetKind() == kInt || GetKind() == kUInt);
        return value;
    }
    SInt32 AsInt() const
    {
        SF_ASSERT(GetKind() == kInt || GetKind() == kUInt);
        return value;
    }

    //
    UInt32& AsUInt()
    {
        SF_ASSERT(GetKind() == kInt || GetKind() == kUInt);
        return value;
    }
    UInt32 AsUInt() const
    {
        SF_ASSERT(GetKind() == kInt || GetKind() == kUInt);
        return value.operator UInt32();
    }

    //
    Number& AsNumber()
    {
        SF_ASSERT(GetKind() == kNumber);
        return value;
    }
    Number AsNumber() const
    {
        SF_ASSERT(GetKind() == kNumber);
        return value;
    }

	//
    ASStringNode* AsStringNode() const
    {
        SF_ASSERT(GetKind() == kString);
        return value;
    }
    ASString AsString() const
    {
        SF_ASSERT(GetKind() == kString);
        SF_ASSERT(GetStringNode());
        return ASString(value);
    }

    // Although value itself is const it may contain a non-const Object.
    Object* GetObject() const
    {
        SF_ASSERT(IsObject());
        return value.VS._1.VObj;
    }
    GASRefCountBase* GetGASRefCountBase() const
    {
        SF_ASSERT(IsObject() || IsNamespace());
        return (GASRefCountBase*)value.VS._1.VObj;
    }
    Instances::Namespace* GetNamespace() const
    {
        SF_ASSERT(IsNamespace());
        return value.VS._1.VNs;
    }
    Object* GetClosure() const
    {
        SF_ASSERT(IsClosure());
        return value.VS._2.VObj;
    }
    GASRefCountBase** AsGASRefCountBasePtrPtr() const;
    GASRefCountBase** AsClosurePtrPtr() const;
    
	//
    Class& AsClass()
    {
        SF_ASSERT(IsClass());
        SF_ASSERT(value.VS._1.VClass);
        return *value.VS._1.VClass;
    }
    const Class& AsClass() const
    {
        SF_ASSERT(IsClass());
        SF_ASSERT(value.VS._1.VClass);
        return *value.VS._1.VClass;
    }
    
	//
    Instances::Function& AsFunction()
    {
        SF_ASSERT(IsFunction());
        SF_ASSERT(static_cast<Instances::Function*>(value));
        return *static_cast<Instances::Function*>(value);
    }
    const Instances::Function& AsFunction() const
    {
        SF_ASSERT(IsFunction());
        SF_ASSERT(static_cast<const Instances::Function*>(value));
        return *static_cast<const Instances::Function*>(value);
    }

	//
    const ThunkInfo& AsThunk() const
    {
        SF_ASSERT(HasThunk());
        return value;
    }

	//
    const Instances::ThunkFunction& AsThunkFunction() const
    {
        SF_ASSERT(IsThunkFunction());
        SF_ASSERT(static_cast<const Instances::ThunkFunction*>(value));
        return *static_cast<const Instances::ThunkFunction*>(value);
    }

	//
    const Instances::Namespace& AsNamespace() const
    {
        SF_ASSERT(IsNamespace());
        SF_ASSERT(static_cast<const Instances::Namespace*>(value));
        return *static_cast<const Instances::Namespace*>(value);
    }

    //
    SF_INLINE
    const Traits& GetTraits() const
    {
        SF_ASSERT(IsMethodInd() || IsVTableInd());
        return value.GetTraits();
    }

#ifdef SF_AS3_AOTC
    //
    SF_INLINE
    SNode& GetSNode() const
    {
        SF_ASSERT(IsSNode());
        return value.GetSNode();
    }
#endif

public:
    static const Value& GetUndefined();
    static const Value& GetNull();

public:
    bool IsWeakRef() const { return (Flags & weakRefMask) != 0; }
    bool IsValidWeakRef() const;
    // Return true if state has changed.
    bool MakeWeakRef();
    // Return true if state has changed.
    bool MakeStrongRef();

public:
    // Masks without Traits.
    //enum { kindMask = 0x0F, valueTypeMask = 0x30, valueTypeOffset = 4, withMask = 0x80 };
    // Masks with Traits.
    enum { kindMask = 0x1F, valueTypeMask = 0x60, valueTypeOffset = 5, withMask = 0x100, weakRefMask = 0x200, artClassMask = 0x400, superCallMask = 0x800 };

    SF_INLINE
    KindType GetKind() const
    {
        return static_cast<KindType>(Flags & kindMask);
    }

    ObjectTag GetObjectTag() const;

    ASStringNode* GetStringNode() const
    {
        SF_ASSERT(GetKind() == kString || (GetKind() == kObject && value.VS._1.VObj == NULL));
        return value;
    }
    SInt32 GetMethodInd() const
    {
        SF_ASSERT(GetKind() == kMethodInd);
        return value;
    }
    SInt32 GetVTableInd() const
    {
        SF_ASSERT(GetKind() == kVTableInd || GetKind() == kVTableIndClosure);
        return value;
    }

    // Methods related to the Tracer.
    TraceNullType GetTraceNullType() const { return static_cast<TraceNullType>((Flags & valueTypeMask) >> valueTypeOffset); }
    bool GetWith() const { return (Flags & withMask) != 0; }
    bool IsArtificialClass() const { return (Flags & artClassMask) != 0; }
    // This is a temporary method.
    bool IsSuperCall() const { return (Flags & superCallMask) != 0; }
    bool IsTraceNull() const { return GetTraceNullType() == Null; }

    SF_INLINE
    InstanceTraits::Traits& GetInstanceTraits() const
    {
        SF_ASSERT(GetKind() == kInstanceTraits SF_AOTC_CODE( || GetKind() == kSNodeIT ));
        SF_ASSERT(value.VS._1.ITr);
        return *value.VS._1.ITr;
    }
    SF_INLINE
    ClassTraits::Traits& GetClassTraits() const
    {
        SF_ASSERT(GetKind() == kClassTraits SF_AOTC_CODE( || GetKind() == kSNodeCT ));
        SF_ASSERT(value.VS._1.CTr);
        return *value.VS._1.CTr;
    }
    bool IsTraceNullOrUndefined() const { return IsTraceNull() || IsUndefined(); }

public:
    // (ECMA-262 section 9.1)
    // Can throw exceptions.
    // Return false in case of exception.
    // result will not be modified in case of exception.
    CheckResult Convert2PrimitiveValueUnsafe(Value& result, Hint hint = hintNone) const;
    // Can throw exceptions.
    // Return false in case of exception.
    // Value will not be modified in case of exception.
    CheckResult ToPrimitiveValue()
    {
        Value v = GetUndefined();
        bool result = false;
        if (Convert2PrimitiveValueUnsafe(v))
        {
            Swap(v);
            result = true;
        }

        return result;
    }

    // (ECMA-262 section 9.2)
    // Doesn't throw exceptions.
    bool Convert2Boolean() const;
    // Doesn't throw exceptions.
    void ToBooleanValue()
    {
        SetBool(Convert2Boolean());
    }
    
    // (ECMA-262 section 9.3)
    // Can throw exceptions.
    // Return false in case of exception.
    SF_INLINE
    CheckResult Convert2NumberInline(Number& result) const
    {
        KindType kind = GetKind();

        switch(kind)
        {
        case kNumber:
            result = value;
            break;
        case kUndefined:
            result = NumberUtil::NaN();
            break;
        case kBoolean:
            result = value.VS._1.VBool ? 1.0 : NumberUtil::POSITIVE_ZERO();
            break;
        case kInt:
            result = static_cast<Number>(value.VS._1.VInt);
            break;
        case kUInt:
            result = static_cast<Number>(value.VS._1.VUInt);
            break;
        default:
            return Convert2NumberInternal(result, kind);
        }

        return true;
    }
    CheckResult Convert2Number(Number& result) const;
    // Can throw exceptions.
    // value will not be modified in case of exception.
    SF_INLINE
    CheckResult ToNumberValueInline()
    {
        CheckResult rc = true;
        Number result;

        if (Convert2NumberInline(result))
            SetNumber(result);
        else
            rc = false;

        return rc;
    }
    CheckResult ToNumberValue();
    
    // (ECMA-262 section 9.5)
    // Can throw exceptions.
    // Return false in case of exception.
    CheckResult Convert2Int32(SInt32& result) const;
    // Can throw exceptions.
    CheckResult ToInt32Value();
    
    // (ECMA-262 section 9.6)
    CheckResult Convert2UInt32(UInt32& result) const;
    // Can throw exceptions.
    CheckResult ToUInt32Value();
    
    // (ECMA-262 section 9.8)
    // Can throw exceptions.
    // Return false in case of exception.
    CheckResult Convert2String(ASString& result) const;
    CheckResult Convert2String(StringBuffer& result) const;
    // Can throw exceptions.
    CheckResult ToStringValue(StringManager& sm);
    
    SF_INLINE
    void Clean()
    {
        Release();
        SetFlags(0);
    }
    
public:
    SF_INLINE
    bool IsNull() const 
    { 
        bool result;

        if (IsObject())
            result = value.VS._1.VObj == NULL;
        else
            result = false;

        return result;
    }

    static bool IsInt(const KindType k) { return k == kInt || k == kUInt; }
    bool IsInt() const { return IsInt(GetKind()); }

    static bool IsIntStrict(const KindType k) { return k == kInt; }
    bool IsIntStrict() const { return IsIntStrict(GetKind()); }

    static bool IsBool(const KindType k) { return k == kBoolean; }
    bool IsBool() const { return IsBool(GetKind()); }

    static bool IsUInt(const KindType k) { return k == kInt || k == kUInt; }
    bool IsUInt() const { return IsUInt(GetKind()); }

    static bool IsUIntStrict(const KindType k) { return k == kUInt; }
    bool IsUIntStrict() const { return IsUIntStrict(GetKind()); }

    static bool IsNumber(const KindType k) { return k == kNumber; }
    bool IsNumber() const { return IsNumber(GetKind()); }

    static bool IsString(const KindType k) { return k == kString; }
    bool IsString() const { return IsString(GetKind()); }

    static bool IsUndefined(const KindType k) { return k == kUndefined; }
    bool IsUndefined() const { return IsUndefined(GetKind()); }

    bool IsNullOrUndefined() const { return IsUndefined(GetKind()) || IsNull(); }

    static bool IsFunction(const KindType k) { return k == kFunction; }
    bool IsFunction() const { return IsFunction(GetKind()); }

    static bool IsObjectStrict(const KindType k) { return k == kObject; }
    bool IsObjectStrict() const { return IsObjectStrict(GetKind()); }

    static bool IsClass(const KindType k) { return k == kClass; }
    bool IsClass() const { return IsClass(GetKind()); }

    // Objects have only one part.
    static bool IsObject(const KindType k) { return k > kNamespace && k < kThunkClosure; }
    bool IsObject() const { return IsObject(GetKind()); }

    static bool IsThunk(const KindType k) { return k == kThunk; }
    bool IsThunk() const { return IsThunk(GetKind()); }

    static bool HasThunk(const KindType k) { return k == kThunk || k == kThunkClosure; }
    bool HasThunk() const { return HasThunk(GetKind()); }

    static bool IsThunkFunction(const KindType k) { return k == kThunkFunction; }
    bool IsThunkFunction() const { return IsThunkFunction(GetKind()); }

    static bool IsNamespace(const KindType k) { return k == kNamespace; }
    bool IsNamespace() const { return IsNamespace(GetKind()); }

    static bool IsThunkClosure(const KindType k) { return k == kThunkClosure; }
    bool IsThunkClosure() const { return IsThunkClosure(GetKind()); }

    static bool IsMethodInd(const KindType k) { return k == kMethodInd; }
    bool IsMethodInd() const { return IsMethodInd(GetKind()); }

    static bool IsVTableInd(const KindType k) { return k == kVTableInd; }
    bool IsVTableInd() const { return IsVTableInd(GetKind()); }

    static bool IsVTableIndClosure(const KindType k) { return k == kVTableIndClosure; }
    bool IsVTableIndClosure() const { return IsVTableIndClosure(GetKind()); }

    // Closures have two parts.
    static bool IsClosure(const KindType k) { return k > kThunkFunction; }
    bool IsClosure() const { return IsClosure(GetKind()); }

    static bool IsCallable(const KindType k) { return IsClosure(k) || IsFunction(k) || IsThunk(k) || IsThunkFunction(k) || IsMethodInd(k) || IsVTableInd(k) || k == kObject || k == kClass; }
    bool IsCallable() const { return IsCallable(GetKind()); }

    static bool IsNumeric(const KindType k) { return k > kBoolean && k < kThunk; }
    bool IsNumeric() const { return IsNumeric(GetKind()); }

    static bool IsPrimitive(const KindType k) { return (k < kThunk || k == kString); }
    bool IsPrimitive() const { return IsPrimitive(GetKind()); }

    static bool IsRefCounted(const KindType k) { return k > kClassTraits; }
    bool IsRefCounted() const { return IsRefCounted(GetKind()); }

#if 0
    // We shouldn't use methods below because they will cause crash with PSVITA (Mon 03/26/2012)
    static bool IsGarbageCollectable(const KindType k) { return k > kString; }
    bool IsGarbageCollectable() const { return IsGarbageCollectable(GetKind()); }
#else
    bool IsGarbageCollectable() const { return GetKind() > kString; }
#endif

#ifdef SF_AS3_AOTC
    static bool IsTraits(const KindType k) { return k >= kInstanceTraits && k <= kSNodeCT; }
    bool IsTraits() const { return IsTraits(GetKind()); }

    static bool IsTraitsStrict(const KindType k) { return k == kInstanceTraits || k == kClassTraits; }
    bool IsTraitsStrict() const { return IsTraitsStrict(GetKind()); }

    static bool IsClassTraits(const KindType k) { return k == kClassTraits || k == kSNodeCT; }
    bool IsClassTraits() const { return IsClassTraits(GetKind()); }

    static bool IsInstanceTraits(const KindType k) { return k == kInstanceTraits || k == kSNodeIT; }
    bool IsInstanceTraits() const { return IsInstanceTraits(GetKind()); }

    static bool IsSNode(const KindType k) { return k == kSNodeIT || k == kSNodeCT; }
    bool IsSNode() const { return IsSNode(GetKind()); }
#else
    static bool IsTraits(const KindType k) { return k == kInstanceTraits || k == kClassTraits; }
    bool IsTraits() const { return IsTraits(GetKind()); }

    static bool IsClassTraits(const KindType k) { return k == kClassTraits; }
    bool IsClassTraits() const { return IsClassTraits(GetKind()); }

    static bool IsInstanceTraits(const KindType k) { return k == kInstanceTraits; }
    bool IsInstanceTraits() const { return IsInstanceTraits(GetKind()); }
#endif
            
    bool IsNaN() const;
    bool IsPOSITIVE_INFINITY() const;
    bool IsNEGATIVE_INFINITY() const;
    bool IsNaNOrInfinity() const;
    bool IsPOSITIVE_ZERO() const;
    bool IsNEGATIVE_ZERO() const;
    bool IsNaN_OR_NP_ZERO() const;

public:
    bool operator ==(const Value& other) const;

    struct HashFunctor
    {
        // Alternative: "sdbm" hash function, suggested at same web page
        // above, http::/www.cs.yorku.ca/~oz/hash.html
        // This is somewhat slower then Bernstein, but it works way better than the above
        // hash function for hashing large numbers of 32-bit ints.
        static UPInt SDBM_Hash(const void* data_in, UPInt size, UPInt seed = 5381);

        UPInt operator()(const Value& key) const;
    };

    // Workaround for Hash.
    template <typename U, typename HashF>
    Value(const HashNode<Value, U, HashF>& node)
        : Flags(node.First.Flags)
        , Bonus(node.First.Bonus)
        , value(node.First.value)
    {
        node.First.AddRef();
    }

public:
    // Function call. It will use undefined for *this* value.
    Value operator()() const;

    template <typename T1>
    Value operator()(const T1& a1) const;

    template <typename T1, typename T2>
    Value operator()(const T1& a1, const T2& a2) const;

    template <typename T1, typename T2, typename T3>
    Value operator()(const T1& a1, const T2& a2, const T3& a3) const;

private:
    SF_INLINE
    void AddRefString() const
    {
        // We should check value.VS._1.VStr for NULL outside of this function.
        value.VS._1.VStr->AddRef();
    }
#ifdef SF_AS3_AOTC
    void AddRefSNode() const;
    void ReleaseSNode() const;
#endif
    void AddRefObject() const;
    void AddRefClosure() const;
    SF_INLINE
    void AddRefMethodClosure() const
    {
        AddRefClosure();
        AddRefObject();
    }
    void AddRefInternal() const;
    void ReleaseInternal();

    void AddRefWeakRef() const;
    void ReleaseWeakRef();

    SF_INLINE
    void AddRef() const
    {
        if (IsRefCounted())
        {
            if (IsWeakRef())
                AddRefWeakRef();
            else
                AddRefInternal();
        }
    }
    SF_INLINE
    void Release()
    {
        if (IsRefCounted())
        {
            if (IsWeakRef())
                ReleaseWeakRef();
            else
                ReleaseInternal();
        }
    }

    GASRefCountBase* GetWeakBase() const;

private:
    //
    static UInt32 GetFlags(KindType v)
    {
        return static_cast<UInt32>(v);
    }

    //
    void SetFlags(UInt32 flags)
    {
        Flags = flags;
    }
    SF_INLINE
    void SetKind(KindType v)
    {
        // Four bits.
        Flags = (Flags & ~kindMask) | (static_cast<UInt32>(v));
    }

private:
    CheckResult Convert2NumberInternal(Number& result, KindType kind) const;

private:
    // Size: one word.
    union Extra
    {
        Extra() : pWeakProxy(NULL) {}
        Extra(WeakProxy* v) : pWeakProxy(v) {}

        WeakProxy* pWeakProxy; // Weak reference support.
    };

    // Size: one word.
    union V1U
    {
        bool VBool;
        SInt32 VInt;
        UInt32 VUInt;
        ASStringNode* VStr;
        Object* VObj;
        Class* VClass;
        Instances::Function* VFunct;
        const ThunkInfo* VThunk;
        Instances::ThunkFunction* VThunkFunct;
        Instances::Namespace* VNs;
        InstanceTraits::Traits* ITr;
        ClassTraits::Traits* CTr;
    };

    // Size: one word.
    union V2U
    {
        Object* VObj;
        const Traits* pTraits;
#ifdef SF_AS3_AOTC
        SNode* VSNode;
#endif
    };

    // Size: two words.
    struct VStruct
    {
        V1U _1;
        V2U _2;
    };

    // Size: two words.
    // Both parts have to be initialized because they will be used to calculate a hash key.
    union VU
    {
    public:
        VU() {} // No initialization for performance reason.
#ifdef SF_OS_XBOX360
        // Msvc90 has a bug with "/O2 /Os" and "/Ox /Os" options. (/Ox implies /O2)
        SF_NOINLINE VU(bool v) { VS._1.VBool = v; }
#else
        VU(bool v) { VS._1.VBool = v; }
#endif
        VU(SInt32 v) { VS._1.VInt = v; }
        VU(UInt32 v) { VS._1.VUInt = v; }
        VU(Number v) { VNumber = v; }
        VU(ASStringNode* v) { VS._1.VStr = v; }
        VU(Object* v) { VS._1.VObj = v; }
        VU(Class* v) { VS._1.VClass = v; }
        VU(Instances::Function* v) { VS._1.VFunct = v; }
        VU(const ThunkInfo& v) { VS._1.VThunk = &v; }
        VU(Instances::ThunkFunction* v) { VS._1.VThunkFunct = v; }
        VU(Instances::Namespace* v) { VS._1.VNs = v; }
        VU(InstanceTraits::Traits& tr) { VS._1.ITr = &tr; }
        VU(ClassTraits::Traits& tr) { VS._1.CTr = &tr; }

#ifdef SF_AS3_AOTC
        VU(InstanceTraits::Traits& tr, SNode& n)
        {
            VS._1.ITr = &tr;
            VS._2.VSNode = &n;
        }
        VU(ClassTraits::Traits& tr, SNode& n)
        {
            VS._1.CTr = &tr;
            VS._2.VSNode = &n;
        }
#endif

        VU(Object* p, const ThunkInfo& f) 
        { 
            VS._2.VObj = p; 
            VS._1.VThunk = &f; 
        }
        VU(Object* p, SInt32 v) 
        { 
            VS._2.VObj = p; 
            VS._1.VInt = v; 
        }
        VU(SInt32 v, const Traits& tr)
        {
            VS._2.pTraits = &tr;
            VS._1.VInt = v;
        }

    public:
        ///
        operator bool&() { return VS._1.VBool; }
        operator bool() const { return VS._1.VBool; }

        ///
        operator SInt32&() { return VS._1.VInt; }
        operator SInt32() const { return VS._1.VInt; }

        ///
        operator UInt32&() { return VS._1.VUInt; }
        operator UInt32() const { return VS._1.VUInt; }

#if !defined(SF_CC_SNC) && !defined(SF_CC_MWERKS) && !defined(SF_CC_ARM) && !defined(SF_CC_GHS)

        ///
        operator UPInt() const { return VS._1.VUInt; }
#endif

        ///
        operator Number&() { return VNumber; }
        operator Number() const { return VNumber; }

        ///
        operator ASStringNode*() const { return VS._1.VStr; }

        ///
        operator Object*() const { return VS._1.VObj; }

        ///
        operator Class*() const { return VS._1.VClass; }

        ///
        operator Instances::Function*() const { return VS._1.VFunct; }

        ///
        operator const ThunkInfo&() const { return *VS._1.VThunk; }

        ///
        operator Instances::ThunkFunction*() const { return VS._1.VThunkFunct; }

        ///
        operator Instances::Namespace*() const { return VS._1.VNs; }

        ///
        Instances::Function* GetFunct() const { return VS._1.VFunct; }
        const Traits& GetTraits() const { return *VS._2.pTraits; }

        ///
#ifdef SF_AS3_AOTC
        SNode& GetSNode() const { return *VS._2.VSNode; }
#endif

    public:
        Number VNumber;
        VStruct VS;
    };

private:
    UInt32  Flags;
    Extra   Bonus;
    VU      value;
};

///////////////////////////////////////////////////////////////////////////
inline
Value::ObjectTag GetObjectTag(UPInt addr)
{
    return (Value::ObjectTag)(addr & Value::ObjectTagMask);
}

///////////////////////////////////////////////////////////////////////////
// It is basically the same thing as SPtr except it uses bits 2 and 3 to
// store object tag.

template <int Stat> class RefCountBaseGC;
template <int Stat> class RefCountCollector;

class STPtr
{
public:
    friend class Value;

    typedef GASRefCountBase ObjType;
    typedef STPtr           SelfType;

public:
    // Constructors.

    STPtr()
        : pObject(NULL)
    {
    }
    STPtr(const Value& v)
        : pObject(MakeAddr(v))
    {
        AddRef();
    }
    // Copy constructor.
    STPtr(const SelfType& other)
        : pObject(other.pObject)
    {
        AddRef();
    }
    ~STPtr()
    {
        Release();
    }

public:
    // Assignment operator.
    SelfType& operator =(const SelfType& other)
    {
        return Set(other);
    }
    SelfType& operator =(const Value& v)
    {
        return SetValue(v);
    }

public:
    // Setters.

    SelfType& SetValue(const Value& v)
    {
        ObjType* addr = MakeAddr(v);

        if (addr != pObject)
        {
            Release();
            pObject = addr;
            AddRef();
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

    void GetValue(Value& v);
    void GetValueUnsafe(Value& v);

    // Obtain pointer reference directly
    ObjType* GetPtr() const
    {
        return pObject;
    }

    // Conversion
    operator ObjType*() const
    {
        return pObject;
    }

    template <int Stat>   
    void ForEachChild_GC(RefCountCollector<Stat>* prcc, typename RefCountBaseGC<Stat>::GcOp op
                         SF_DEBUG_ARG(const RefCountBaseGC<Stat>& owner)) const
    {
        if (pObject)
        {
            Value::ObjectTag tag = GetObjectTag((Scaleform::UPInt)pObject);
            // Strip tag.
            const ObjType* addr = GetAddrNoTag();

            (*op)(prcc, (const RefCountBaseGC<Stat>**)&addr SF_DEBUG_ARG(owner));

            // Add tag back.
            pObject = (ObjType*)((UPInt)addr | tag);
        }
    }

private:
    ObjType* GetAddrNoTag() const
    {
        return (ObjType*)((Scaleform::UPInt)pObject & ~Value::ObjectTagMask);
    }
    ObjType* GetRawAddr() const
    {
        return (ObjType*)((UPInt)GetAddrNoTag() & ~1);
    }

    static ObjType* MakeAddr(const Value& v)
    {
        ObjType* addr = NULL;

        if (v.IsObject() || v.IsNamespace())
        {
            GASRefCountBase* vv = v.GetGASRefCountBase();

            if (vv)
                addr = (ObjType*)((UPInt)vv | v.GetObjectTag());
        }

        return addr;
    }

    void AddRef() const;
    void Release() const;

private:
    mutable ObjType* pObject;
}; // STPtr

///////////////////////////////////////////////////////////////////////////
typedef ArrayLH<Value>  ValueArray;
typedef ArrayDH<Value>  ValueArrayDH;
// ScopeStack has to store Value because we need to store "with" flag
// with scope stack values somehow.
typedef ValueArrayDH    ScopeStackType;

//typedef ScopeStackType TGlobalObjects;
typedef ArrayLH<Instances::fl::GlobalObject*> TGlobalObjects;

SF_INLINE
bool Value::IsNaN() const
{
    SF_ASSERT(GetKind() == kNumber);

    return NumberUtil::IsNaN(value.VNumber);
}

SF_INLINE
bool Value::IsPOSITIVE_INFINITY() const
{
    SF_ASSERT(GetKind() == kNumber);

    return NumberUtil::IsPOSITIVE_INFINITY(value.VNumber);
}

SF_INLINE
bool Value::IsNEGATIVE_INFINITY() const
{
    SF_ASSERT(GetKind() == kNumber);

    return NumberUtil::IsNEGATIVE_INFINITY(value.VNumber);
}

SF_INLINE
bool Value::IsNaNOrInfinity() const
{
    SF_ASSERT(GetKind() == kNumber);

    return NumberUtil::IsNaNOrInfinity(value.VNumber);
}

SF_INLINE
bool Value::IsPOSITIVE_ZERO() const
{
    SF_ASSERT(GetKind() == kNumber);
    
    return NumberUtil::IsPOSITIVE_ZERO(value.VNumber);
}

SF_INLINE
bool Value::IsNEGATIVE_ZERO() const
{
    SF_ASSERT(GetKind() == kNumber);
    
    return NumberUtil::IsNEGATIVE_ZERO(value.VNumber);
}

SF_INLINE
bool Value::IsNaN_OR_NP_ZERO() const
{
    SF_ASSERT(GetKind() == kNumber);
    
    return IsNaN() || IsPOSITIVE_ZERO() || IsNEGATIVE_ZERO();
}

SF_INLINE
Value::Number MakeValueNumber(double v)
{
#ifdef SF_NO_DOUBLE
    return NumberUtil::ConvertDouble2Float(v);
#else
    return v;
#endif
}

inline
bool IsNamespaceObject(const Value& v) { return v.IsNamespace(); }
bool IsQNameObject(const Value& v);

bool IsXMLObject(Object* obj);
inline
bool IsXMLObject(const Value& v) { return v.IsObject() && IsXMLObject(v.GetObject()); }

bool IsXMLListObject(Object* obj);
inline
bool IsXMLListObject(const Value& v) { return v.IsObject() && IsXMLListObject(v.GetObject()); }

// (ECMA-262 section 11.9.6)
// No exceptions.
bool StrictEqual(const Value& x, const Value& y);

// (ECMA-262 section 11.9.3)
// Can throw exceptions.
// Return false in case of exception.
CheckResult AbstractEqual(bool& result, const Value& l, const Value& r);

CheckResult Add(StringManager& sm, Value& result, const Value& l, const Value& r);

///////////////////////////////////////////////////////////////////////////
enum Boolean3 { undefined3, true3, false3 };

// (ECMA-262 section 11.8.5)
// Can throw exceptions.
// Return false in case of exception.
CheckResult AbstractLessThan(Boolean3& result, const Value& l, const Value& r);


// Retrieve an object stored at absolute address "addr".
// "addr" stores an ObjectTag in the lowest two bits.
Value GetAbsObject(UPInt addr);

inline
bool Value::operator ==(const Value& other) const
{
#if 0
    if (IsValidWeakRef() && other.IsValidWeakRef())
        return StrictEqual(*this, other);

    if (GetKind() == other.GetKind())
    {
        if (IsObject() && other.IsObject() && GetObject() == other.GetObject())
            return true;

        if (IsClosure() && other.IsClosure() && GetClosure() == other.GetClosure())
            return true;
    }

    return false;
#else
    // Theoretically, StrictEqual() should work correctly even in case of
    // invalid weak references.
    return StrictEqual(*this, other);
#endif
}

///////////////////////////////////////////////////////////////////////////////
inline
Value::TraceNullType CanBeNull(const Value& _1, const Value& _2)
{
    const Value::TraceNullType tn1 = _1.GetTraceNullType();
    const Value::TraceNullType tn2 = _2.GetTraceNullType();

    if (tn1 == tn2)
        return tn1;

    return Value::NullOrNot;
}

}}} // namespace Scaleform { namespace GFx { namespace AS3 {

namespace Scaleform { namespace Alg {

// Specialization for Swap()
template <>
SF_INLINE
void Swap<GFx::AS3::Value>(GFx::AS3::Value& l, GFx::AS3::Value& r)
{
    l.Swap(r);
}

}} //namespace Scaleform { namespace Alg {


namespace Scaleform { namespace GFx { 

template<>
inline
void ASString::operator = <AS3::Value>(const AS3::Value& v)
{
    if (v.IsNull())
        SetNull();
    else
        AssignNode(v.AsStringNode());
}

}} // namespace Scaleform { namespace GFx { 

#endif // INC_AS3_Value_H
