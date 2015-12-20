/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef Navigation_TimeProfiler_H
#define Navigation_TimeProfiler_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_Timer.h"


namespace Kaim
{

//-----------------------------------------------------------------
// macro used to internally to implement ScopedProfilerXXX classes
#define KY_IMPLEMENT_SCOPED_PROFILER(class_name, time_unit, init_zero) \
KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem) \
public: \
	class_name(KyFloat32* time) : m_time(time), m_start(0) { if (m_time) { m_start = Timer::GetProfileTicks(); init_zero(); } } \
	~class_name()                              { if (m_time) { *m_time += (KyFloat32)((KyFloat64)(Timer::GetProfileTicks() - m_start) * time_unit); } } \
	void DoInitZero() { *m_time = 0.0f; } \
	void DoNotInitZero() {} \
private: \
	KyFloat32* m_time; \
	UInt64 m_start;

//-----------------------------------------------------------------
// ScopedProfilerXXX classes
// Usage:
// MyFunc() {
//     ScopedProfilerXXX ScopedProfilerXXX(&someKyFloat32);
//     ...
// }
// The difference between ScopedProfilerXXX and ScopedProfilerSumXXX is that 
// ScopedProfilerXXX    does someKyFloat32  = timeSpent
// ScopedProfilerSumXXX does someKyFloat32 += timeSpent
class ScopedProfilerSeconds    { KY_IMPLEMENT_SCOPED_PROFILER(ScopedProfilerSeconds   , 0.000001, DoInitZero   ) };
class ScopedProfilerMs         { KY_IMPLEMENT_SCOPED_PROFILER(ScopedProfilerMs        , 0.001   , DoInitZero   ) };
class ScopedProfilerMks        { KY_IMPLEMENT_SCOPED_PROFILER(ScopedProfilerMks       , 1.0     , DoInitZero   ) };
class ScopedProfilerSumSeconds { KY_IMPLEMENT_SCOPED_PROFILER(ScopedProfilerSumSeconds, 0.000001, DoNotInitZero) };
class ScopedProfilerSumMs      { KY_IMPLEMENT_SCOPED_PROFILER(ScopedProfilerSumMs     , 0.001   , DoNotInitZero) };
class ScopedProfilerSumMks     { KY_IMPLEMENT_SCOPED_PROFILER(ScopedProfilerSumMks    , 1.0     , DoNotInitZero) };

//-----------------------------------------------------------------
// Small macros atop ScopedProfilerXXX.
// Not very interesting, allows to avoid the use of brackets {} to limit the lifetime of ScopedProfilerXXX
#define KY_BEGIN_PROFILE_SECONDS(s) { Kaim::ScopedProfilerSeconds scopedProfiler(s);
#define KY_BEGIN_PROFILE_MS(ms)     { Kaim::ScopedProfilerMs      scopedProfiler(ms);
#define KY_BEGIN_PROFILE_MKS(mks)   { Kaim::ScopedProfilerMks     scopedProfiler(mks);

#define KY_BEGIN_PROFILE_SECONDS_CUMULATE(s) { Kaim::ScopedProfilerSumSeconds scopedProfiler(s);
#define KY_BEGIN_PROFILE_MS_CUMULATE(ms)     { Kaim::ScopedProfilerSumMs      scopedProfiler(ms);
#define KY_BEGIN_PROFILE_MKS_CUMULATE(mks)   { Kaim::ScopedProfilerSumMks     scopedProfiler(mks);

#define KY_END_PROFILE }


/// Small utility class atop Kaim::Timer to profile easily.
/// For profiling, use ScopedProfiler or Profiler rather than Timer.
class Profiler
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	Profiler() { m_start = Timer::GetProfileTicks(); }

	// ------------------- Get time since start and reset start -------------------
	KyFloat64 GetSecondsAndRestart()      { UInt64 end = Timer::GetProfileTicks(); KyFloat64 t = (KyFloat64)(end - m_start) * 0.000001; m_start = end; return t; }
	KyFloat64 GetMillisecondsAndRestart() { UInt64 end = Timer::GetProfileTicks(); KyFloat64 t = (KyFloat64)(end - m_start) * 0.001   ; m_start = end; return t; }
	KyFloat64 GetMicrosecondsAndRestart() { UInt64 end = Timer::GetProfileTicks(); KyFloat64 t = (KyFloat64)(end - m_start)           ; m_start = end; return t; }

	KyFloat32 GetSecondsAndRestartF()      { return (KyFloat32)GetSecondsAndRestart();      }
	KyFloat32 GetMillisecondsAndRestartF() { return (KyFloat32)GetMillisecondsAndRestart(); }
	KyFloat32 GetMicrosecondsAndRestartF() { return (KyFloat32)GetMicrosecondsAndRestart(); }

	void GetSecondsAndRestart(KyFloat64* s)        { if (s)   { *s   = GetSecondsAndRestart();      } }
	void GetMillisecondsAndRestart(KyFloat64* ms)  { if (ms)  { *ms  = GetMillisecondsAndRestart(); } }
	void GetMicrosecondsAndRestart(KyFloat64* mks) { if (mks) { *mks = GetMicrosecondsAndRestart(); } }

	void GetSecondsAndRestartF(KyFloat32* s)        { if (s)   { *s   = GetSecondsAndRestartF();      } }
	void GetMillisecondsAndRestartF(KyFloat32* ms)  { if (ms)  { *ms  = GetMillisecondsAndRestartF(); } }
	void GetMicrosecondsAndRestartF(KyFloat32* mks) { if (mks) { *mks = GetMicrosecondsAndRestartF(); } }

	// ------------------- Get time since start, does not change start -------------------
	KyFloat64 GetSeconds()      const { UInt64 end = Timer::GetProfileTicks(); KyFloat64 t = (KyFloat64)(end - m_start) * 0.000001; return t; }
	KyFloat64 GetMilliseconds() const { UInt64 end = Timer::GetProfileTicks(); KyFloat64 t = (KyFloat64)(end - m_start) * 0.001   ; return t; }
	KyFloat64 GetMicroseconds() const { UInt64 end = Timer::GetProfileTicks(); KyFloat64 t = (KyFloat64)(end - m_start)           ; return t; }

	KyFloat32 GetSecondsF()      const { return (KyFloat32)GetSeconds();      }
	KyFloat32 GetMillisecondsF() const { return (KyFloat32)GetMilliseconds(); }
	KyFloat32 GetMicrosecondsF() const { return (KyFloat32)GetMicroseconds(); }

	void GetSeconds(KyFloat64* s)        const { if (s)   { *s   = GetSeconds();      } }
	void GetMilliseconds(KyFloat64* ms)  const { if (ms)  { *ms  = GetMilliseconds(); } }
	void GetMicroseconds(KyFloat64* mks) const { if (mks) { *mks = GetMicroseconds(); } }

	void GetSecondsF(KyFloat32* s)        const { if (s)   { *s   = GetSecondsF();      } }
	void GetMillisecondsF(KyFloat32* ms)  const { if (ms)  { *ms  = GetMillisecondsF(); } }
	void GetMicrosecondsF(KyFloat32* mks) const { if (mks) { *mks = GetMicrosecondsF(); } }

private:
	UInt64 m_start; // time in microseconds
};

} // namespace Kaim


#endif
