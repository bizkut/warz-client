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
#ifndef AG_PERFMON_EVENT_SRC_API_H
#define AG_PERFMON_EVENT_SRC_API_H

#ifdef AGPERFMON
#ifdef WIN32
#include "AgPerfMonEventAPI.h"
#endif
#include "PxSimpleTypes.h"

#define AG_PERFMON_EV_START				0x00
#define AG_PERFMON_EV_STOP				0x01
#define AG_PERFMON_EV_STAT  			0x02
#define AG_PERFMON_EV_DEBUG  			0x03


#define DEFINE_EVENT(name) name,
enum AgPerfEventEnum
{
#include "AgPerfMonEventDefs.h"
	AgPerfEventNumEvents
};
#undef DEFINE_EVENT



#ifdef WIN32
typedef AgPmHANDLE  (AgPmCreateSourceConnection_FUNC)(physx::PxU32 version);
typedef bool        (AgPmDestroySourceConnection_FUNC)(AgPmHANDLE hconn);
typedef AgEventID   (AgPmRegisterEvent_FUNC)(AgPmHANDLE hconn, const char *name);
typedef bool        (AgPmSubmitEvent_FUNC)(AgPmHANDLE hconn, AgEventID id, unsigned int data0, unsigned int data1, unsigned char data2);
typedef bool        (AgPmEventEnabled_FUNC)(AgPmHANDLE hconn, AgEventID id);
typedef bool        (AgPmEventLoggingEnabled_FUNC)(AgPmHANDLE hconn);

/** Event Source API */
AgPmHANDLE  AgPmCreateSourceConnection();
bool        AgPmDestroySourceConnection(AgPmHANDLE hconn);   
AgEventID   AgPmRegisterEvent(AgPmHANDLE hconn, const char *name);
bool        AgPmSubmitEvent(AgPmHANDLE hconn, AgEventID id, unsigned int data0, unsigned int data1, unsigned char data2);
bool        AgPmRegisterEventAsPassive(AgPmHANDLE hconn, AgEventID id, void *context, AgPassiveEventCallback funcPtr);
bool        AgPmEventEnabled(AgPmHANDLE hconn, AgEventID id);
bool        AgPmEventLoggingEnabled(AgPmHANDLE hconn);

class AgAppPerfUtils
{
public:

	AgAppPerfUtils();
    ~AgAppPerfUtils();

	AgEventID registerEvent(const char *);
	bool isEnabled();
	bool isEventEnabled(AgEventID id);
	bool isLibraryLoaded();
	void startEvent(AgEventID id, physx::PxU16 data = 0);
	void stopEvent(AgEventID id, physx::PxU16 data = 0);
	void statEvent(AgEventID id, physx::PxU32 stat);
	void statEvent(AgEventID id, physx::PxU32 stat, physx::PxU32 ident);
	void debugEvent(AgEventID id, physx::PxU32 data0, physx::PxU32 data1);

	inline AgEventID getEventId(int index) { return mEventIds[index]; };

	physx::PxU32 GetThreadPriority();
    unsigned char GetProcID (void);
	inline AgPmHANDLE getHAgPm() {return mhAgPm; };

private:
    typedef struct _THREAD_BASIC_INFORMATION {
        physx::PxU32               ExitStatus;
        void               *TebBaseAddress;
        physx::PxU32               ClientId;
        physx::PxU32               reserved;
        physx::PxU32               AffinityMask;
        physx::PxU32               Priority;
        physx::PxU32               BasePriority;
    } THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

    typedef physx::PxU32 ( __stdcall tinfo_FUNC)(void *ThreadHandle,
        physx::PxU32 ThreadInformationClass,
        PTHREAD_BASIC_INFORMATION ThreadInformation,
        physx::PxU32 ThreadInformationLength,
        physx::PxU32 *ReturnLength);

    tinfo_FUNC			*mNtQueryThreadInfo;
    void				*mhNTDLL;
	static AgPmHANDLE	mhAgPm;

	AgEventID			mEventIds[AgPerfEventNumEvents+1];
	static const char *	mEventNames[AgPerfEventNumEvents+1];
};

#else

#define AG_INVALID_EVENT_ID             ((AgU16)0xffff)

typedef unsigned short int				AgU16;
typedef unsigned long int				AgU32;
typedef unsigned short int				AgEventID;
#ifndef AGPERFUTILS_DEFINED
#define AGPERFUTILS_DEFINED
class AgPerfUtils
{
public:
	AgEventID registerEvent(const char *);
	bool isEnabled();
	void connect(const char* host, unsigned int port, physx::PxU32 eventMask);
	void disconnect();
	bool isEventEnabled(AgEventID id);
	void startEvent(AgEventID id, AgU16 data = 0);
	void stopEvent(AgEventID id, AgU16 data = 0);
	void statEvent(AgEventID id, AgU32 stat);
	void statEvent(AgEventID id, AgU32 stat, AgU32 ident);
	void debugEvent(AgEventID id, AgU32 data0, AgU32 data1);
	static AgPerfUtils* Initialize(unsigned int numEvents, unsigned char **eventNames, unsigned short *eventIds);
	static void SetIPAddress(char* hostIPAddress);
};
#endif
class AgAppPerfUtils
{
public:
	AgAppPerfUtils()
	{
		mSdkPerfUtils = AgPerfUtils::Initialize(AgPerfEventNumEvents, (unsigned char **)mEventNames, (unsigned short *)mEventIds);
	};
    ~AgAppPerfUtils();

	AgEventID registerEvent(const char *);
	bool isEventEnabled(AgEventID id);
	void startEvent(AgEventID id, AgU16 data = 0) {mSdkPerfUtils->startEvent(id, data);};
	void stopEvent(AgEventID id, AgU16 data = 0){mSdkPerfUtils->stopEvent(id, data);};
	void statEvent(AgEventID id, AgU32 stat){mSdkPerfUtils->statEvent(id, stat);};
	void statEvent(AgEventID id, AgU32 stat, AgU32 ident){mSdkPerfUtils->statEvent(id, stat, ident);};
	void debugEvent(AgEventID id, AgU32 data0, AgU32 data1){mSdkPerfUtils->debugEvent(id, data0, data1);};
	inline AgEventID getEventId(int index) { return mEventIds[index]; };
	static void SetIPAddress(char* hostIPAddress) {AgPerfUtils::SetIPAddress(hostIPAddress);};

private:
	AgEventID			mEventIds[AgPerfEventNumEvents+1];
	static const char *	mEventNames[AgPerfEventNumEvents+1];
	AgPerfUtils *mSdkPerfUtils;
};
#endif


/*
 * Example wrapper class.  You must create an instance of class
 * AgPerfUtils named gPerfUtils in order to use AgPerfScope.
 * To use AgPerfScope, place event names in AgPerfMonEventDefs.h,
 * for example:
 *
 * ================ AgPerfMonEventDefs.h ====================
 * DEFINE_EVENT(exampleEvent0)
 * DEFINE_EVENT(exampleEvent1)
 * ================ AgPerfMonEventDefs.h ====================
 *
 * In your source code, you can now declare scope profile zones:
 *
 * if (doEvent0)
 * {
 *      AgPerfScope scope(exampleEvent0);
 *      // event0 stuff
 * }
 * else
 * {
 *      AgPerfScope scope(exampleEvent1);
 *      // event1 stuff
 * }
 *
 * Note that you can store a 16bit data value in both the start and stop
 * events and both datums will be shown in the viewer and can be later
 * analyzed via the DataExtractor.  The most common usage for these
 * values is to store the amount of work done by that scope.  For
 * instance:
 *
 * AgU16 workDone = 0;
 * AgPerfScope scope(exampleEvent0, loopCount);
 * for (int i = 0 ; i < loopCount ; i++)
 *      workDone += doEvent0Stuff(i);
 * scope.stop(workDone);
 *
 * In the viewer, this event bar will have the description:
 * "exampleEvent0 (loopCount-workDone) 3ms"
 *
 */

extern AgAppPerfUtils* gAppPerfUtils;

class AgPerfScope
{
public:
    AgPerfScope(AgPerfEventEnum e, physx::PxU16 data = 0)
    {
		stopped = false;
		if(gAppPerfUtils)
		{
			mEventID = gAppPerfUtils->getEventId(e);
			gAppPerfUtils->startEvent(mEventID, data);
		}
    }
    void stop(physx::PxU16 data = 0)
    {
        if(gAppPerfUtils)
			gAppPerfUtils->stopEvent(mEventID, data);
        stopped = true;
    }
    ~AgPerfScope()
    {
        if (gAppPerfUtils && !stopped)
            gAppPerfUtils->stopEvent(mEventID);
    }
private:
    AgEventID mEventID;
    bool stopped;
};

#define SAMPLE_PERF_SCOPE(name)							   AgPerfScope __scope(name)
#define SAMPLE_PERF_SCOPE_PERF_DSCOPE(name, data)          AgPerfScope __scope(name, data)
#define SAMPLE_PERF_SCOPE_PERF_SCOPE_STOP(data)            __scope.stop(data)

#else
#define SAMPLE_PERF_SCOPE(name)
#define SAMPLE_PERF_SCOPE_PERF_DSCOPE(name, data)
#define SAMPLE_PERF_SCOPE_PERF_SCOPE_STOP(data)
#endif
#endif /** _AG_PERFMON_EVENT_SRC_API___ */
