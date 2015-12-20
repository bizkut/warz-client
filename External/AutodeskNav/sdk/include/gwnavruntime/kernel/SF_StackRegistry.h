/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef GwNav_StackWalker_H
#define GwNav_StackWalker_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_Alg.h"
#include "gwnavruntime/kernel/SF_Atomic.h"

#if defined (KY_ENABLE_STACK_REGISTRY)

namespace Kaim
{

enum StackRegistryInitializationStatus
{
	StackRegistryNotInitialized = 0,
	StackRegistryInitialized = 1,
};

enum StackRegistryEnabledStatus
{
	StackRegistryNotEnabled = 0,
	StackRegistryEnabled = 1,
};

#define MAX_STACK_DEPTH 32
#define MAX_NAME_LENGTH 256

class StackRef
{
public:
	StackRef(void* stack, KyUInt32 frameCount) : m_frameCount(frameCount)
	{
		Alg::MemUtil::Copy(m_stack, stack, sizeof(void*) * frameCount);
	}

	~StackRef() { }

	KyUInt32 GetFrameCount() const { return m_frameCount; }
	void* GetFrame(KyUInt32 frameIndex) const { return m_stack[frameIndex]; }

private:
	void*    m_stack[MAX_STACK_DEPTH];
	KyUInt32 m_frameCount;
};

class StackRegistryImpl;

class StackRegistry
{
public:
	StackRegistry() : m_initializationStatus(StackRegistryNotInitialized), m_enabledStatus(StackRegistryNotEnabled), m_impl(KY_NULL) { }
	~StackRegistry() { Clear(); }

	static void Clear();

	static void Enable();
	static void Disable();

	static void MapCurrentStackToUid(UPInt uid); // uid is typically a void*
	static void UnMapStackFromUid(UPInt uid);    

	static KyUInt32 GetCount();
	static void Report(); // Outputs all the stack that have not been removed.

private:
	static StackRegistry& Instance() { static StackRegistry s_instance; return s_instance; }
	static void Initialize();

	StackRegistryInitializationStatus m_initializationStatus;
	StackRegistryEnabledStatus        m_enabledStatus;
	StackRegistryImpl*                m_impl;
	Lock                              m_lock;
};

}

#endif // KY_ENABLE_STACK_REGISTRY

#endif // GwNav_StackWalker_H
