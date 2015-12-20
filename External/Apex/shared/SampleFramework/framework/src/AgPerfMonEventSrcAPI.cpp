/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

// suppress LNK4221
#include "PxPreprocessor.h"
PX_DUMMY_SYMBOL

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#ifdef AGPERFMON
#include "AgPerfMonEventSrcAPI.h"

static HMODULE AgPmDllHandle=0;

static AgPmCreateSourceConnection_FUNC  *createFunc;
static AgPmDestroySourceConnection_FUNC *destroyFunc;
static AgPmRegisterEvent_FUNC           *registerEventFunc;
static AgPmSubmitEvent_FUNC             *submitEventFunc;
static AgPmEventEnabled_FUNC            *eventEnabledFunc;
static AgPmEventLoggingEnabled_FUNC     *eventLoggingEnabledFunc;


/** macros */
#define AgPmLibLoaded() (0 != AgPmDllHandle)


AgPmHANDLE AgPmCreateSourceConnection()
{
    /** load the DLL */
    AgPmDllHandle = LoadLibrary(AG_PERFMON_LIB_NAME);

    if(!AgPmDllHandle)
    {
        /* printf("Could not load AgPerfMon Library\n"); */
        return 0;
    }

    /** get the function pointers to the event src api */
    createFunc              = (AgPmCreateSourceConnection_FUNC*)GetProcAddress(AgPmDllHandle, "AgPmCreateSourceConnection");
    destroyFunc             = (AgPmDestroySourceConnection_FUNC*)GetProcAddress(AgPmDllHandle, "AgPmDestroySourceConnection");
    registerEventFunc       = (AgPmRegisterEvent_FUNC*)GetProcAddress(AgPmDllHandle, "AgPmRegisterEvent");
    submitEventFunc         = (AgPmSubmitEvent_FUNC*)GetProcAddress(AgPmDllHandle, "AgPmSubmitEvent");
    eventEnabledFunc        = (AgPmEventEnabled_FUNC*)GetProcAddress(AgPmDllHandle, "AgPmEventEnabled");
    eventLoggingEnabledFunc = (AgPmEventLoggingEnabled_FUNC*)GetProcAddress(AgPmDllHandle, "AgPmEventLoggingEnabled");

    if(!createFunc          ||
       !destroyFunc         ||
       !registerEventFunc   ||
       !submitEventFunc     ||
       !eventEnabledFunc    ||
       !eventLoggingEnabledFunc)
    {
#ifdef UNICODE
        MessageBox(NULL, L"The AGEIA PerfMON DLL does not contain all required entrypoints, PerfMON is disabled.  Install the latest AgPerfMon.dll.", L"AGEIA PerfMON ERROR", MB_OK|MB_ICONSTOP);
#else
        MessageBox(NULL, "The AGEIA PerfMON DLL does not contain all required entrypoints, PerfMON is disabled.  Install the latest AgPerfMon.dll.", "AGEIA PerfMON ERROR", MB_OK|MB_ICONSTOP);
#endif
        FreeLibrary(AgPmDllHandle);
        AgPmDllHandle = 0;
        return 0;
    }

    return createFunc(AG_PERFMON_VERSION);
}

bool AgPmDestroySourceConnection(AgPmHANDLE hconn)
{
    if(!AgPmLibLoaded())
        return false;

    bool retVal = destroyFunc(hconn);
    FreeLibrary(AgPmDllHandle);
    AgPmDllHandle = 0;
    
    return retVal;
}

AgEventID AgPmRegisterEvent(AgPmHANDLE hconn, const char *name)
{
    if(!AgPmLibLoaded())
        return AG_INVALID_EVENT_ID;

    return registerEventFunc(hconn, name);
}

bool AgPmSubmitEvent(AgPmHANDLE hconn, AgEventID id, unsigned int data0, unsigned int data1, unsigned char data2)
{
    if(!AgPmLibLoaded())
        return false;

    return submitEventFunc(hconn, id, data0, data1, data2);
}

bool AgPmEventEnabled(AgPmHANDLE hconn, AgEventID id)
{
    if(!AgPmLibLoaded())
        return false;

    return eventEnabledFunc(hconn, id);
}

bool AgPmEventLoggingEnabled(AgPmHANDLE hconn)
{
    if(!AgPmLibLoaded())
        return false;

    return eventLoggingEnabledFunc(hconn);
}

//** AgPerfUtils */

#ifdef TBD
#define DEFINE_EVENT(name) #name,
const char *AgAppPerfUtils::mEventNames[] = {
#include "AgPerfMonEventDefs.h"
	""
};
#undef DEFINE_EVENT
#endif

AgPmHANDLE	AgAppPerfUtils::mhAgPm = 0;

AgAppPerfUtils::AgAppPerfUtils():
   mNtQueryThreadInfo(0),
#ifdef UNICODE
   mhNTDLL(LoadLibrary(L"NTDLL.DLL"))
#else
   mhNTDLL(LoadLibrary("NTDLL.DLL"))
#endif
{
    if(mhNTDLL)
        mNtQueryThreadInfo = (tinfo_FUNC *)GetProcAddress((HMODULE)mhNTDLL, "NtQueryInformationThread");

	if (!mhAgPm)
	{
		mhAgPm = AgPmCreateSourceConnection();
		
		if (mhAgPm)
		{
			for (int i = 0; i < AgPerfEventNumEvents; i++)
				mEventIds[i] = AgPmRegisterEvent(mhAgPm, mEventNames[i]);
		}
	}
}

AgAppPerfUtils::~AgAppPerfUtils()
{
    if(mhNTDLL)
        FreeLibrary((HMODULE)mhNTDLL);

	if (mhAgPm)
	{
		AgPmDestroySourceConnection(mhAgPm);
		mhAgPm = 0;
	}
}

physx::PxU32 AgAppPerfUtils::GetThreadPriority()
{
    physx::PxU32 retVal = 0;
    
    if(mNtQueryThreadInfo)
    {
        /** call into the Native API to get the dynamic thread priority */
        physx::PxU32 len;
        THREAD_BASIC_INFORMATION tbi;
        physx::PxU32 status = mNtQueryThreadInfo(GetCurrentThread(), 0, &tbi, sizeof(tbi), &len); //740ns
        if(!status)
            retVal = tbi.Priority;
    }
    
    return retVal;
}

#include <intrin.h>
unsigned char AgAppPerfUtils::GetProcID()
{
   int CPUInfo[4];
   int InfoType = 1;
	__cpuid(CPUInfo, InfoType);

    return unsigned char(CPUInfo[1] >> 24); // APIC Physical ID
}

AgEventID AgAppPerfUtils::registerEvent(const char *name)
{
	return AgPmRegisterEvent(mhAgPm, name);
}

bool AgAppPerfUtils::isEnabled()
{
	return AgPmEventLoggingEnabled(mhAgPm);
}

void AgAppPerfUtils::startEvent(AgEventID id, physx::PxU16 data)
{
	physx::PxU32 threadCpuData = 0;
	((physx::PxU8*)(&threadCpuData))[0] = (physx::PxU8)GetThreadPriority();
	((physx::PxU8*)(&threadCpuData))[1] = GetProcID();
	((physx::PxU16*)(&threadCpuData))[1] = data;
	AgPmSubmitEvent(mhAgPm, id, GetCurrentThreadId(), threadCpuData, AG_PERFMON_EV_START);
}

void AgAppPerfUtils::stopEvent(AgEventID id, physx::PxU16 data)
{
	physx::PxU32 threadCpuData = 0;
	((physx::PxU8*)(&threadCpuData))[0] = (physx::PxU8)GetThreadPriority();
	((physx::PxU8*)(&threadCpuData))[1] = GetProcID();
	((physx::PxU16*)(&threadCpuData))[1] = data;
	AgPmSubmitEvent(mhAgPm, id, GetCurrentThreadId(), threadCpuData, AG_PERFMON_EV_STOP);
}

void AgAppPerfUtils::statEvent(AgEventID id, physx::PxU32 stat)
{
	AgPmSubmitEvent(mhAgPm, id, stat, GetCurrentThreadId(), AG_PERFMON_EV_STAT);
}

void AgAppPerfUtils::statEvent(AgEventID id, physx::PxU32 stat, physx::PxU32 ident)
{
	AgPmSubmitEvent(mhAgPm, id, stat, ident, AG_PERFMON_EV_STAT);
}

void AgAppPerfUtils::debugEvent(AgEventID id, physx::PxU32 data0, physx::PxU32 data1)
{
	AgPmSubmitEvent(mhAgPm, id, data0, data1, AG_PERFMON_EV_DEBUG);
}

bool AgAppPerfUtils::isEventEnabled(AgEventID id)
{
	return AgPmEventEnabled(mhAgPm, id);
}

bool AgAppPerfUtils::isLibraryLoaded()
{
	if(!AgPmLibLoaded())
        return false;
	else
		return true;
}
#endif
#endif
