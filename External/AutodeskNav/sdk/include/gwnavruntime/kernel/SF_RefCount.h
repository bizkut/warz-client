/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_RefCount.h
Content     :   Reference counting implementation headers
Created     :   January 14, 1999
Authors     :   Michael Antonov, Maxim Shemanarev, Sergey Sikorskiy

**************************************************************************/

#ifndef INC_KY_Kernel_RefCount_H
#define INC_KY_Kernel_RefCount_H

#include "gwnavruntime/kernel/SF_Types.h"
#include "gwnavruntime/kernel/SF_Memory.h"

namespace Kaim {

// ***** Reference Counting

// There are three types of reference counting base classes:
//
//  RefCountBase     - Provides thread-safe reference counting (Default).
//  RefCountBaseNTS  - Non Thread Safe version of reference counting.
//  RefCountBaseWeakSupport - Non Thread Safe ref-counting with WeakPtr support.


// ***** Declared classes

template<class C, int StatType>
class   RefCountBase;
template<class C, int StatType>
class   RefCountBaseNTS;
template<class C, int StatType>
class   RefCountBaseWeakSupport;

class   RefCountImpl;
class   RefCountNTSImpl;
class   RefCountWeakSupportImpl;
//class   RefCountImpl;

template<class C>
class   WeakPtr;
class   WeakPtrProxy;


// *** Macros for RefCountBase

// These macros specify whether reference counting is configured to be
// thread safe or not. By default, reference counting is not thread-safe
// for most classes.
#define KY_REFCOUNT_NORMAL     0x00000000
#define KY_REFCOUNT_THREADSAFE 0x00000001


// ***** Implementation For Reference Counting

// RefCountImplCore holds RefCount value and defines a few utility
// functions shared by all implementations.

class RefCountImplCore
{
protected:
    volatile int    RefCount;

public:
    // RefCountImpl constructor always initializes RefCount to 1 by default.
    KY_INLINE RefCountImplCore() : RefCount(1) { }

    // Need virtual destructor
    // This:    1. Makes sure the right destructor's called.
    //          2. Makes us have VTable, necessary if we are going to have format needed by InitNewMem()
    KY_EXPORT virtual ~RefCountImplCore();

    // Debug method only.
    int             GetRefCount() const { return RefCount;  }

    // This logic is used to detect invalid 'delete' calls of reference counted
    // objects. Direct delete calls are not allowed on them unless they come in
    // internally from Release.
#ifdef KY_BUILD_DEBUG    
    static void   KY_CDECL  reportInvalidDelete(void *pmem);
    inline static void checkInvalidDelete(RefCountImplCore *pmem)
    {
        if (pmem->RefCount != 0)
            reportInvalidDelete(pmem);
    }
#else
    inline static void checkInvalidDelete(RefCountImplCore *) { }
#endif

    // Base class ref-count content should not be copied.
	RefCountImplCore(const RefCountImplCore &) : RefCount(1) { }
    void operator = (const RefCountImplCore &) { }  
};

class RefCountNTSImplCore
{
protected:
    mutable int    RefCount;

public:
    // RefCountImpl constructor always initializes RefCount to 1 by default.
    KY_INLINE RefCountNTSImplCore() : RefCount(1) { }

    // Need virtual destructor
    // This:    1. Makes sure the right destructor's called.
    //          2. Makes us have VTable, necessary if we are going to have format needed by InitNewMem()
    KY_EXPORT virtual ~RefCountNTSImplCore();

    // Debug method only.
    int             GetRefCount() const { return RefCount;  }

    // This logic is used to detect invalid 'delete' calls of reference counted
    // objects. Direct delete calls are not allowed on them unless they come in
    // internally from Release.
#ifdef KY_BUILD_DEBUG    
    static void   KY_CDECL  reportInvalidDelete(void *pmem);
    KY_INLINE static void checkInvalidDelete(RefCountNTSImplCore *pmem)
    {
        if (pmem->RefCount != 0)
            reportInvalidDelete(pmem);
    }
#else
    KY_INLINE static void checkInvalidDelete(RefCountNTSImplCore *) { }
#endif

    // Base class ref-count content should not be copied.
	RefCountNTSImplCore(const RefCountNTSImplCore &) : RefCount(1) { }
    void operator = (const RefCountNTSImplCore &) { }  
};



// RefCountImpl provides Thread-Safe implementation of reference counting, so
// it should be used by default in most places.

class RefCountImpl : public RefCountImplCore
{
public:
    // Thread-Safe Ref-Count Implementation.
    KY_EXPORT void    AddRef();
    KY_EXPORT void    Release();   
};

// RefCountVImpl provides Thread-Safe implementation of reference counting, plus,
// virtual AddRef and Release.

class RefCountVImpl : public RefCountImplCore
{
public:
    // Thread-Safe Ref-Count Implementation.
    virtual void      AddRef();
    virtual void      Release();   
};


// RefCountImplNTS provides Non-Thread-Safe implementation of reference counting,
// which is slightly more efficient since it doesn't use atomics.

class RefCountNTSImpl : public RefCountNTSImplCore
{
public:
    KY_INLINE void    AddRef() const { RefCount++; }
    KY_EXPORT void    Release() const;   
};

// RefCountWeakSupportImpl provides reference counting with WeakProxy support,
// allowing for WeakPtr<> to be used on objects derived from this class. Weak pointer
// support is non-thread safe. Weak pointers are used for some ActionScript objects,
// especially when GC is disabled.

class RefCountWeakSupportImpl : public RefCountNTSImpl
{
protected:    
    mutable WeakPtrProxy*   pWeakProxy;
public:
    RefCountWeakSupportImpl() { pWeakProxy = 0; }
    KY_EXPORT virtual ~RefCountWeakSupportImpl();

    // Create/return create proxy, users must release proxy when no longer needed.
    WeakPtrProxy*  CreateWeakProxy() const;
};


// RefCountBaseStatImpl<> is a common class that adds new/delete override with Stat tracking
// to the reference counting implemenation. Base must be one of the RefCountImpl classes.

template<class Base, int StatType>
class RefCountBaseStatImpl : public Base
{
public:
    RefCountBaseStatImpl() { }
     
    // *** Override New and Delete

    // DOM-IGNORE-BEGIN
    // Undef new temporarily if it is being redefined
#ifdef KY_BUILD_DEFINE_NEW
#ifdef KY_DEFINE_NEW
#undef new
#endif
#endif

#ifdef KY_BUILD_DEBUG
    // Custom check used to detect incorrect calls of 'delete' on ref-counted objects.
    #define KY_REFCOUNTALLOC_CHECK_DELETE(class_name, p)   \
        do {if (p) Base::checkInvalidDelete((class_name*)p); } while(0)
#else
    #define KY_REFCOUNTALLOC_CHECK_DELETE(class_name, p)
#endif

    // Redefine all new & delete operators.
    KY_MEMORY_REDEFINE_NEW_IMPL(Base, KY_REFCOUNTALLOC_CHECK_DELETE, StatType)

#ifdef KY_DEFINE_NEW
#define new KY_DEFINE_NEW
#endif
        // KY_BUILD_DEFINE_NEW
        // DOM-IGNORE-END
};




// *** End user RefCountBase<> classes


// RefCountBase is a base class for classes that require thread-safe reference
// counting; it also overrides the new and delete operators to use MemoryHeap.
//
// Reference counted objects start out with RefCount value of 1. Further lifetame
// management is done through the AddRef() and Release() methods, typically
// hidden by Ptr<>.

template<class C, int Stat>
class RefCountBase : public RefCountBaseStatImpl<RefCountImpl, Stat>
{
public:
    enum { StatType = Stat };   
    // Constructor.
    KY_INLINE RefCountBase() : RefCountBaseStatImpl<RefCountImpl, Stat>() { }    
};

// RefCountBaseV is the same as RefCountBase but with virtual AddRef/Release

template<class C, int Stat>
class RefCountBaseV : public RefCountBaseStatImpl<RefCountVImpl, Stat>
{
public:
    enum { StatType = Stat };   
    // Constructor.
    KY_INLINE RefCountBaseV() : RefCountBaseStatImpl<RefCountVImpl, Stat>() { }    
};


// RefCountBaseNTS is a base class for classes that require Non-Thread-Safe reference
// counting; it also overrides the new and delete operators to use MemoryHeap.
// This class should only be used if all pointers to it are known to be assigned,
// destroyed and manipulated within one thread.
//
// Reference counted objects start out with RefCount value of 1. Further lifetame
// management is done through the AddRef() and Release() methods, typically
// hidden by Ptr<>.

template<class C, int Stat>
class RefCountBaseNTS : public RefCountBaseStatImpl<RefCountNTSImpl, Stat>
{
public:
    enum { StatType = Stat };   
    // Constructor.
    KY_INLINE RefCountBaseNTS() : RefCountBaseStatImpl<RefCountNTSImpl, Stat>() { }    
};

// RefCountBaseWeakSupport is a base class for classes that require Non-Thread-Safe
// reference counting and WeakPtr<> support; it also overrides the new and delete
// operators to use MemoryHeap. This class should only be used if all pointers to it
// are known to be assigned, destroyed and manipulated within one thread.
//
// Reference counted objects start out with RefCount value of 1. Further lifetame
// management is done through the AddRef() and Release() methods, typically
// hidden by Ptr<>.

template<class C, int Stat>
class RefCountBaseWeakSupport : public RefCountBaseStatImpl<RefCountWeakSupportImpl, Stat>
{
public:
    enum { StatType = Stat };   
    // Constructor.
    KY_INLINE RefCountBaseWeakSupport() : RefCountBaseStatImpl<RefCountWeakSupportImpl, Stat>() { }    
};



// ***** Pickable template pointer
enum PickType { PickValue };

template <typename T>
class Pickable
{
public:
    Pickable() : pV(NULL) {}
    explicit Pickable(T* p) : pV(p) {}
    Pickable(T* p, PickType) : pV(p) 
    {
        KY_ASSERT(pV);
        if (pV)
            pV->AddRef();
    }
    template <typename OT>
    Pickable(const Pickable<OT>& other) : pV(other.GetPtr()) {}

public:
    Pickable& operator =(const Pickable& other)
    {
        KY_ASSERT(pV == NULL);
        pV = other.pV;
        // Extra check.
        //other.pV = NULL;
        return *this;
    }

public:
    T* GetPtr() const { return pV; }
    T* operator->() const
    {
        return pV;
    }
    T& operator*() const
    {
        KY_ASSERT(pV);
        return *pV;
    }

private:
    T* pV;
};

template <typename T>
KY_INLINE
Pickable<T> MakePickable(T* p)
{
    return Pickable<T>(p);
}

// ***** Ref-Counted template pointer

// Automatically AddRefs and Releases interfaces

void* ReturnArg0(void* p);

template<class C>
class Ptr
{
#ifdef KY_CC_ARM
    static C* ReturnArg(void* p) { return (C*)ReturnArg0(p); }
#endif

protected:
    C   *pObject;

public:

    // Constructors
    KY_INLINE Ptr() : pObject(0)
    { }
#ifdef KY_CC_ARM
    KY_INLINE Ptr(C &robj) : pObject(ReturnArg(&robj))
#else
    KY_INLINE Ptr(C &robj) : pObject(&robj)
#endif
    { }
    KY_INLINE Ptr(Pickable<C> v) : pObject(v.GetPtr())
    {
        // No AddRef() on purpose.
    }
    KY_INLINE Ptr(Ptr<C>& other, PickType) : pObject(other.pObject)
    {
        other.pObject = NULL;
        // No AddRef() on purpose.
    }
    KY_INLINE Ptr(C *pobj)
    {
        if (pobj) pobj->AddRef();   
        pObject = pobj;
    }
    KY_INLINE Ptr(const Ptr<C> &src)
    {
        if (src.pObject) src.pObject->AddRef();     
        pObject = src.pObject;
    }

    template<class R>
    KY_INLINE Ptr(Ptr<R> &src)
    {
        if (src) src->AddRef();
        pObject = src;
    }
    template<class R>
    KY_INLINE Ptr(Pickable<R> v) : pObject(v.GetPtr())
    {
        // No AddRef() on purpose.
    }

    // Destructor
    KY_INLINE ~Ptr()
    {
        if (pObject) pObject->Release();        
    }

    // Compares
    KY_INLINE bool operator == (const Ptr &other) const       { return pObject == other.pObject; }
    KY_INLINE bool operator != (const Ptr &other) const       { return pObject != other.pObject; }

    KY_INLINE bool operator == (C *pother) const              { return pObject == pother; }
    KY_INLINE bool operator != (C *pother) const              { return pObject != pother; }


    KY_INLINE bool operator < (const Ptr &other) const       { return pObject < other.pObject; }

    // Assignment
    template<class R>
    KY_INLINE const Ptr<C>& operator = (const Ptr<R> &src)
    {
        if (src) src->AddRef();
        if (pObject) pObject->Release();        
        pObject = src;
        return *this;
    }   
    // Specialization
    KY_INLINE const Ptr<C>& operator = (const Ptr<C> &src)
    {
        if (src) src->AddRef();
        if (pObject) pObject->Release();        
        pObject = src;
        return *this;
    }   
    
    KY_INLINE const Ptr<C>& operator = (C *psrc)
    {
        if (psrc) psrc->AddRef();
        if (pObject) pObject->Release();        
        pObject = psrc;
        return *this;
    }   
    KY_INLINE const Ptr<C>& operator = (C &src)
    {       
        if (pObject) pObject->Release();        
        pObject = &src;
        return *this;
    }
    KY_INLINE Ptr<C>& operator = (Pickable<C> src)
    {
        return Pick(src);
    }
    template<class R>
    KY_INLINE Ptr<C>& operator = (Pickable<R> src)
    {
        return Pick(src);
    }
    
    // Set Assignment
    template<class R>
    KY_INLINE Ptr<C>& SetPtr(const Ptr<R> &src)
    {
        if (src) src->AddRef();
        if (pObject) pObject->Release();
        pObject = src;
        return *this;
    }
    // Specialization
    KY_INLINE Ptr<C>& SetPtr(const Ptr<C> &src)
    {
        if (src) src->AddRef();
        if (pObject) pObject->Release();
        pObject = src;
        return *this;
    }   
    
    KY_INLINE Ptr<C>& SetPtr(C *psrc)
    {
        if (psrc) psrc->AddRef();
        if (pObject) pObject->Release();
        pObject = psrc;
        return *this;
    }   
    KY_INLINE Ptr<C>& SetPtr(C &src)
    {       
        if (pObject) pObject->Release();
        pObject = &src;
        return *this;
    }
    KY_INLINE Ptr<C>& SetPtr(Pickable<C> src)
    {       
        return Pick(src);
    }

    // Nulls ref-counted pointer without decrement
    KY_INLINE void    NullWithoutRelease()    
    { 
        pObject = 0;    
    }

    // Clears the pointer to the object
    KY_INLINE void    Clear()
    {
        if (pObject) pObject->Release();
        pObject = 0;
    }

    // Obtain pointer reference directly, for D3D interfaces
    KY_INLINE C*& GetRawRef()                 { return pObject; }

    // Access Operators
    KY_INLINE C* GetPtr() const               { return pObject; }
    KY_INLINE C& operator * () const          { return *pObject; }
    KY_INLINE C* operator -> ()  const        { return pObject; }
    // Conversion                   
    KY_INLINE operator C* () const            { return pObject; }

    // Pickers.

    // Pick a value.
    KY_INLINE Ptr<C>& Pick(Ptr<C>& other)
    {
        if (&other != this)
        {
            if (pObject) pObject->Release();
            pObject = other.pObject;
            other.pObject = 0;
        }

        return *this;
    }

    KY_INLINE Ptr<C>& Pick(Pickable<C> v)
    {
        if (v.GetPtr() != pObject)
        {
            if (pObject) pObject->Release();
            pObject = v.GetPtr();
        }

        return *this;
    }

    template<class R>
    KY_INLINE Ptr<C>& Pick(Pickable<R> v)
    {
        if (v.GetPtr() != pObject)
        {
            if (pObject) pObject->Release();
            pObject = v.GetPtr();
        }

        return *this;
    }

    KY_INLINE Ptr<C>& Pick(C* p)
    {
        if (p != pObject)
        {
            if (pObject) pObject->Release();
            pObject = p;
        }

        return *this;
    }
};



// *** Weak Pointer Support

// Helper for making objects that can have weak_ptr's.
// TBD: Converted WeakPtr logic is not thread-safe, need to work this out

class WeakPtrProxy : public NewOverrideBase<Stat_Default_Mem>
{
public:
    WeakPtrProxy(RefCountWeakSupportImpl* pobject)
      : RefCount(1), pObject(pobject)
    { }

    // WeakPtr calls this to determine if its pointer is valid or not
    KY_INLINE bool  IsAlive() const    { return (pObject != 0); }

    // Objects call this to inform of their death
    KY_INLINE void  NotifyObjectDied() { pObject = 0; }

    RefCountWeakSupportImpl* GetObject() const { return pObject; }

    KY_INLINE void  AddRef()
    {   
        RefCount++;
        // Uncomment this to track potential bad use of Ptr
        //KY_ASSERT(RefCount > -1 && RefCount < 0x00FFFFFF);
    }

    KY_INLINE void  Release()
    {
        // Uncomment this to track potential bad use of Ptr
        //KY_ASSERT(RefCount > -1 && RefCount < 0x00FFFFFF);

        RefCount--;
        if (RefCount == 0)
            delete this;
    }    

private:
    // Hidden
    WeakPtrProxy(const WeakPtrProxy& w) { KY_UNUSED(w); }
    void    operator=(const WeakPtrProxy& w) { KY_UNUSED(w); }

    int                         RefCount;
    RefCountWeakSupportImpl*    pObject;
};


// A weak pointer points at an object, but the object may be deleted
// at any time, in which case the weak pointer automatically becomes
// NULL.  The only way to use a weak pointer is by converting it to a
// strong pointer (i.e. for temporary use).
//
// The class pointed to must have a "WeakPtrProxy* GetWeakPtrProxy()" method.
//
// Usage idiom:
//
// if (Ptr<my_type> ptr = weak_ptr_to_my_type) { ... use ptr->whatever() safely in here ... }


template<class C>
class WeakPtr
{
public:
    WeakPtr()
    { }

    KY_INLINE WeakPtr(C* ptr)
        : pProxy(*(ptr ? ptr->CreateWeakProxy() : (WeakPtrProxy*)0))
    {  }
    KY_INLINE WeakPtr(const Ptr<C>& ptr)
        : pProxy(*(ptr.GetPtr() ? ptr->CreateWeakProxy() : (WeakPtrProxy*)0))
    {  }

    // Default constructor and assignment from WeakPtr<C> are OK
    KY_INLINE void    operator = (C* ptr)
    {
        if (ptr)
        {
            pProxy = *ptr->CreateWeakProxy();
        }
        else
        {
            pProxy.Clear();
        }
    }

    KY_INLINE void    operator = (const Ptr<C>& ptr)
    { operator=(ptr.GetPtr()); }

    // Conversion to Ptr
    inline operator Ptr<C>() const
    {        
        return Ptr<C>(GetObjectThroughProxy());
    }

    KY_INLINE bool    operator == (C* ptr)
    { return GetObjectThroughProxy() == ptr; }
    KY_INLINE bool    operator == (const Ptr<C>& ptr)
    { return GetObjectThroughProxy() == ptr.GetPtr(); }

private:
    
    // Set pObject to NULL if the object died
    KY_INLINE C* GetObjectThroughProxy() const
    {
        C* pobject = 0;

        if (pProxy)
        {
            if (pProxy->IsAlive())
            {
                pobject = (C*)pProxy->GetObject();
            }
            else
            {
                // Release proxy if the underlying object went away.
                pProxy.Clear();
            }
        }
        return pobject;
    }

    mutable Ptr<WeakPtrProxy> pProxy;
};

} // Scaleform

#endif
