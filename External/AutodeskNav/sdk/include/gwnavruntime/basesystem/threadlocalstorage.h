/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_ThreadLocalStorage_H
#define Navigation_ThreadLocalStorage_H

#include "gwnavruntime/base/memory.h"

#if defined (KY_OS_XBOX360)
#define KY_THREAD_LOCAL __declspec(thread)
#elif defined (KY_OS_WII)
#define KY_THREAD_LOCAL
#elif defined (KY_OS_MAC) || defined (KY_OS_IPHONE)
#define KY_THREAD_LOCAL
#include <pthread.h>
#else
#define KY_THREAD_LOCAL __thread   
#endif


namespace Kaim
{


//-----------------------------------------------------------------------------------
//                                 KY_OS_WIN32
//-----------------------------------------------------------------------------------
#if defined (KY_OS_WIN32)

class WinTls
{
public:
	static unsigned long KyTlsAlloc();
	static void* KyTlsGetValue(unsigned long dwTlsIndex);
	static int KyTlsSetValue(unsigned long dwTlsIndex, void* lpTlsValue);
	static int KyTlsFree(unsigned long dwTlsIndex);					
};


template<typename ptrT> class ThreadLocalStorageWrapper
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	ThreadLocalStorageWrapper()   { m_tlsIdx = (KyUInt32)WinTls::KyTlsAlloc(); }
	~ThreadLocalStorageWrapper()  { WinTls::KyTlsFree(m_tlsIdx); }
	inline ptrT Get()             { return (ptrT)WinTls::KyTlsGetValue(m_tlsIdx); }
	inline void Set(ptrT ptr)     { WinTls::KyTlsSetValue(m_tlsIdx, (void*)ptr);  }
private:
	KyUInt32 m_tlsIdx;
};

//-----------------------------------------------------------------------------------
//                                     KY_OS_MAC
//-----------------------------------------------------------------------------------
#elif defined (KY_OS_MAC) || defined (KY_OS_IPHONE)

template<typename ptrT> class ThreadLocalStorageWrapper
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	ThreadLocalStorageWrapper()   { pthread_key_create(&m_key, KY_NULL); }
	~ThreadLocalStorageWrapper()  { pthread_key_delete(m_key); }
	inline ptrT Get()             { return (ptrT) pthread_getspecific(m_key); }
	inline void Set(ptrT ptr)     { pthread_setspecific(m_key, (void*)ptr ); }
private:
	pthread_key_t m_key;
};


//-----------------------------------------------------------------------------------
//                                     !KY_OS_WIN32 !KY_OS_MAC
//-----------------------------------------------------------------------------------
#else

template<typename ptrT> class ThreadLocalStorageWrapper
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	ThreadLocalStorageWrapper()  {}
	~ThreadLocalStorageWrapper() {}
	inline ptrT Get()            { return TlsInstance(); }
	inline void Set(ptrT ptr)    { TlsInstance() = ptr; }
private:
	inline ptrT& TlsInstance() { static KY_THREAD_LOCAL ptrT s_ptr; return s_ptr; }
};

#endif


} // namespace Kaim


#endif
