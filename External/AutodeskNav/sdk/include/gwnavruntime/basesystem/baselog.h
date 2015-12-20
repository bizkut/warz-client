/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BaseLog_H
#define Navigation_BaseLog_H

#include "gwnavruntime/kernel/SF_Log.h"

namespace Kaim
{

class VisualDebugServer;

/// BaseLog must used instead of Kaim::Log due to parti pris for Navigation's VisualDebug
class BaseLog : public Log
{
public:
	virtual void SetVisualDebugServer(VisualDebugServer* server) = 0;
	virtual VisualDebugServer* GetVisualDebugServer() = 0;

	/// This is called in Kaim::BaseSystem::Destroy() before Navigation memory system is destroyed.
	/// So, implementations of BaseLog relying on Kaim::BaseSystem resources (e.g. using KY_MALLOC or KY_NEW or Kaim::SysFile)
	/// should release these resources on this call. Otherwise, expect crashes.
	virtual void ReleaseMemorySystemResources() {}

	/// BaseLog static functions must be used instead of Log::
	static BaseLog* GetGlobalBaseLog() { return (BaseLog*) GetGlobalLog(); }
	static void     SetGlobalBaseLog(BaseLog *log) { SetGlobalLog(log); }
};

}

#endif //Navigation_Logger_H
