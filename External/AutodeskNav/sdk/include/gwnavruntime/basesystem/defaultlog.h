/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JAPA - secondary contact: BRGR

#ifndef Navigation_DefaultLog_H
#define Navigation_DefaultLog_H

#include "gwnavruntime/base/fileopener.h"
#include "gwnavruntime/kernel/SF_File.h"
#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/basesystem/baselog.h"


namespace Kaim
{

class VisualDebugServer;

/// Default implementation of the Kaim::Log interface
class DefaultLog : public BaseLog
{
public:
	typedef UInt32 LogToModeMask;
	enum LogToMode { LogToNothing = 0, LogToStdout = 1, LogToIde = 2, LogToFile = 4, LogToVisualDebug = 8, LogToAll = 15 };

public:
	static bool IsInitialized;
	static DefaultLog& Instance() { static DefaultLog s_instance; IsInitialized = true; return s_instance; }
	static void Destroy();

	virtual void LogMessageVarg(LogMessageId messageId, const char* fmt, va_list argList);
	virtual void ReleaseMemorySystemResources();

	/// Sets the output locations in which debug messages will be printed.
	/// logToMode is a bitmask composed of elements from the LogToMode enumeration.
	void SetLogToMode(LogToModeMask logToMode) { m_logToMode = logToMode; }

	/// .GwNavLog will be appended to absoluteLogFileName
	void SetupLogFile(const char* absoluteLogFileName, FileOpenerBase* fileOpener = KY_NULL);
	
	/// Beware, all logs will be sent through this VisualDebugServer.
	/// Hence if you use the VisualDebugServer of one world, you'll also receive the log of a secondary world
	virtual void SetVisualDebugServer(VisualDebugServer* visualDebugServer) { m_visualDebugServer = visualDebugServer; }
	virtual VisualDebugServer* GetVisualDebugServer() { return m_visualDebugServer; }

	virtual void LogBuffer(LogMessageId messageId, const char* buffer);

protected:
	DefaultLog() 
		: m_logToMode(LogToNothing)
		, m_fileOpener(KY_NULL)
		, m_file(KY_NULL)
		, m_visualDebugServer(KY_NULL)
	{
		SFstrcpy(m_logFilename, Kaim::ArraySize(m_logFilename), "NavigationLog.GwNavLog");
	}

	virtual ~DefaultLog() {} // don't do anything here please

private:
	void DoDestroy();

protected:
	UInt32 m_logToMode;
	FileOpenerBase* m_fileOpener; // if set to KY_NULL, Kaim::DefaultFileOpener will be used;
	char m_logFilename[260];
	Ptr<File> m_file;
	VisualDebugServer* m_visualDebugServer;
};



}

#endif // Navigation_DefaultLog_H
