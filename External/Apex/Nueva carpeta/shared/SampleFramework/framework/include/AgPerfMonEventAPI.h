/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 * Copyright 2007-2008 Ageia Technologies.  All rights reserved.
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

#ifndef __AG_PERFMON_EVENT_API___
#define __AG_PERFMON_EVENT_API___

/** version info */
#define AG_PERFMON_VERSION                  0x06121900

/** Required for Event Sink/Src */
#ifdef UNICODE
#   define AG_PERFMON_LIB_NAME                 L"AgPerfMon.dll"
#else
#   define AG_PERFMON_LIB_NAME                 "AgPerfMon.dll"
#endif

#define AG_PERFMON_API_INIT_EVENT_NAME      "AgPerfMonApiInitEvent"
#define AG_EVENT_NAME_LEN                   129
#define AG_INVALID_EVENT_ID                 ((physx::PxU16)0xffff)
#define AG_INVALID_PCM_ID                   ((physx::PxU8)0xff)

typedef unsigned short AgEventID;
typedef unsigned int AgPCMID;
typedef void* AgPmHANDLE;

#include "PsShare.h"

typedef enum AgPcmEventGroup
{
	_agMipsEvent,
	_agVcuEvent,
	_agVpuEvent,
} AgPcmEventGroup;

typedef struct AgPerfMonEvent
{
    physx::PxU16 eventID;
    physx::PxU8  destMask;
    physx::PxU8  vpeID;
    physx::PxU32 timeStamp;
    physx::PxU8  taskListID;
    physx::PxU8  pcmID;
    physx::PxU8  taskSeq;
    physx::PxU8  vpuID;
    physx::PxU32 vpeTime;
} AgPerfMonRemoteEvent;

typedef enum AgPerfMonAppType
{
    _agEventSrc,
    _agEventSink,
    _agEventCollector,
} AgPerfMonAppType;

#if defined(WIN32)
#define AG_PERFMON_DLL_API      extern "C" __declspec(dllexport)
#else
#define AG_PERFMON_DLL_API
#endif

typedef bool (*AgPassiveEventCallback)(void *context, AgEventID id);

/** Event Source API */
AG_PERFMON_DLL_API AgPmHANDLE   AgPmCreateSourceConnection(physx::PxU32 version);
AG_PERFMON_DLL_API bool         AgPmDestroySourceConnection(AgPmHANDLE hconn);
AG_PERFMON_DLL_API AgEventID    AgPmRegisterEvent(AgPmHANDLE hconn, char *name);
AG_PERFMON_DLL_API bool         AgPmSubmitEvent(AgPmHANDLE hconn, AgEventID id, unsigned int data0, unsigned int data1, unsigned char data2);
AG_PERFMON_DLL_API bool         AgPmRegisterEventAsPassive(AgPmHANDLE hconn, AgEventID id, void *context, AgPassiveEventCallback funcPtr);
AG_PERFMON_DLL_API bool         AgPmEventEnabled(AgPmHANDLE hconn, AgEventID id);
AG_PERFMON_DLL_API bool         AgPmEventLoggingEnabled(AgPmHANDLE hconn);

/** Event Sink API */
AG_PERFMON_DLL_API AgPmHANDLE       AgPmCreateSinkConnection(physx::PxU32 version);
AG_PERFMON_DLL_API bool             AgPmDestroySinkConnection(AgPmHANDLE hconn);
AG_PERFMON_DLL_API bool				AgPmStartLogging(AgPmHANDLE hconn);
AG_PERFMON_DLL_API bool				AgPmStopLogging(AgPmHANDLE hconn);
AG_PERFMON_DLL_API bool             AgPmEventAvailable(AgPmHANDLE hconn);
AG_PERFMON_DLL_API AgPerfMonEvent*  AgPmGetCurrentEventPtr(AgPmHANDLE hconn);
AG_PERFMON_DLL_API bool				AgPmPopEventPtr(AgPmHANDLE hconn);
AG_PERFMON_DLL_API AgEventID		AgPmGetEventID(AgPmHANDLE hconn, char *name);
AG_PERFMON_DLL_API char*			AgPmGetEventName(AgPmHANDLE hconn, AgEventID id);

/** Event Filtering API */
AG_PERFMON_DLL_API bool				AgPmEnableEvent(AgPmHANDLE hconn, AgEventID id);			
AG_PERFMON_DLL_API bool				AgPmDisableEvent(AgPmHANDLE hconn, AgEventID id);
AG_PERFMON_DLL_API bool				AgPmEnableAllEvents(AgPmHANDLE hconn);
AG_PERFMON_DLL_API bool				AgPmDisableAllEvents(AgPmHANDLE hconn, AgEventID id);
AG_PERFMON_DLL_API bool				AgPmEnablePcmEvents(AgPmHANDLE hconn, AgPCMID pcmID, AgPcmEventGroup procID);
AG_PERFMON_DLL_API bool				AgPmDisablePcmEvents(AgPmHANDLE hconn, AgPCMID pcmID, AgPcmEventGroup procID);

/** Event Chaining API */
AG_PERFMON_DLL_API bool				AgPmChainEvent(AgPmHANDLE hconn, AgEventID leadID, AgEventID followerID);
AG_PERFMON_DLL_API bool				AgPmRemoveEventChain(AgPmHANDLE hconn, AgEventID leadID);


#endif /* __AG_PERFMON_EVENT_API___ */
