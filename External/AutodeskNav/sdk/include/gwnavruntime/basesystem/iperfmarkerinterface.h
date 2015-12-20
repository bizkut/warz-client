/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_IPerfMarkerInterface_H
#define Navigation_IPerfMarkerInterface_H

#include "gwnavruntime/base/types.h"

namespace Kaim
{

/// An abstract interface for an object that can respond to performance tracking markers.
/// You can re-implement this interface to handle the performance markers raised by the Gameware Navigation code, and markers
/// that you raise yourself by calling the KY_PERF_MARKER_BEGIN ,KY_PERF_MARKER_END andKY_SCOPED_PERF_MARKER macros.
/// For example, you could log the time taken for each event, or pass the events to a third-party profiling tool.
class IPerfMarkerInterface
{
public:
	/// Retrieves the static instance of the IPerfMarkerInterface class managed by the BaseSystem. 
	static IPerfMarkerInterface*& Instance() { static IPerfMarkerInterface* s_instance = KY_NULL; return s_instance; }

	virtual ~IPerfMarkerInterface() {}

	/// Called by instrumented code to indicate that a tracked block of code has begun.
	/// You can implement this method in order to respond to the start of each performance marker
	/// instrumented in the Gameware Navigation code, and each marker that you begin using the KY_PERF_MARKER_BEGIN
	/// macro.
	/// \param name   A descriptive name for this marker. 
	virtual void Begin(const char* name) = 0;

	/// Called by instrumented code to indicate that the currently tracked block of code has been completed.
	/// You can implement this method in order to respond to the end of each performance marker
	/// instrumented in the Gameware Navigation code, and each marker that you end using the KY_PERF_MARKER_END
	/// macro. 
	virtual void End() = 0;

	/// Indicates to the IPerfMarkerInterface that a block of code has begun that it should track.
	/// Transparently calls your implementation of IPerfMarkerInterface::Begin().
	/// \param name   A descriptive name for this marker. 
	static void BeginMarker(const char* name) { if (Instance()) Instance()->Begin(name); }

	/// Indicates to the IPerfMarkerInterface that the currently tracked block of code has been completed.
	/// Transparently calls your implementation of IPerfMarkerInterface::End(). 
	static void EndMarker()                   { if (Instance()) Instance()->End();       }
};


class ScopedPerfMarker
{
public:
	ScopedPerfMarker(const char* name) { IPerfMarkerInterface::BeginMarker(name); }
	~ScopedPerfMarker()                { IPerfMarkerInterface::EndMarker();       }
};


} // Kaim

#if defined(KY_PERF_MARKERS_ENABLED)

	/// Indicates to the IPerfMarkerInterface that a block of code has begun that it should track.
	/// Transparently calls IPerfMarkerInterface::BeginMarker(). 
	#define KY_PERF_MARKER_BEGIN(name)  Kaim::IPerfMarkerInterface::BeginMarker(name)

	/// Indicates to the IPerfMarkerInterface that the currently tracked block of code has been completed.
	/// Transparently calls IPerfMarkerInterface::EndMarker(). 
	#define KY_PERF_MARKER_END()        Kaim::IPerfMarkerInterface::EndMarker()

	/// Indicates to the IPerfMarkerInterface that a block of code has begun that it should track.
	/// Transparently calls IPerfMarkerInterface::BeginMarker() immediately, and calls IPerfMarkerInterface::EndMarker()
	/// when the block of code is finished (that is, when an internally created object goes out of scope). 
	/// This macro is useful for tracking the performance of functions that have many possible exit points. 
	#define KY_SCOPED_PERF_MARKER(name) Kaim::ScopedPerfMarker scopedPerfMarker__(name);

#else

	#define KY_PERF_MARKER_BEGIN(name)
	#define KY_PERF_MARKER_END()
	#define KY_SCOPED_PERF_MARKER(name)

#endif	// defined(KY_PERF_MARKERS_ENABLED)


#endif // Navigation_IPerfMarkerInterface_H
