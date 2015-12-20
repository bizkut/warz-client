/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: BRGR - secondary contact: NOBODY

#ifndef Navigation_SocketDispatcher_H
#define Navigation_SocketDispatcher_H

#include "gwnavruntime/kernel/SF_Types.h"

#ifdef KY_ENABLE_SOCKETS
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/visualdebug/amp/Amp_Socket.h"

namespace Kaim
{

class File;

enum SocketDispatchMode 
{
	SocketDispatch_NetworkOnly = 0, //< data are sent to and received from the network exclusively
	SocketDispatch_SendToFile = 1, //< data are sent to file and receive calls always give a Heartbeat message
	SocketDispatch_ReceiveFromFile = 2, //< data are received from file exclusively, and sent to nowhere.
};

class SocketDispatcherFactory : public Net::AMP::SocketImplFactory
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// @param file must be ready for write and read
	SocketDispatcherFactory(SocketDispatchMode mode, File* file, KyInt32 fileBytesLimit)
		: m_mode(mode)
		, m_file(file)
		, m_fileBytesLimit(fileBytesLimit)
	{
		// check that a file is passed if the given mode requires one!
		KY_ASSERT( m_mode == SocketDispatch_NetworkOnly || m_file != KY_NULL );
		
		// cap the file size in MB to prevent KyInt32 overflow when it will be converted to Bytes
		// minus 10MB since writing ends when the file size is above the limit,
		// and we don't expect that a buffer of 10MB be given to Net::AMP::Socket::Send()
		KyInt32 capLimit = (2047-10) * 1024 * 1024; // cap to 2037 MBytes
		if (m_fileBytesLimit <= 0 || m_fileBytesLimit >= capLimit)
			m_fileBytesLimit = capLimit;
	}

	virtual ~SocketDispatcherFactory() {}

	virtual Net::AMP::SocketInterface* Create();
	virtual void Destroy(Net::AMP::SocketInterface* socketImpl);

private:
	SocketDispatchMode m_mode;
	File* m_file;
	KyInt32 m_fileBytesLimit;
};

}

#endif

#endif // Navigation_SocketDispatcher_H
