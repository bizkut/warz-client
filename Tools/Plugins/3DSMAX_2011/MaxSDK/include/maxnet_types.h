//-----------------------------------------------------------------------------
// -------------------
// File ....: maxnet.h
// -------------------
// Author...: Gus J Grubba
// Date ....: February 2000
//
// Descr....: 3D Studio MAX Network Interface
//
// History .: Feb, 07 2000 - Started
//            
//-----------------------------------------------------------------------------
#pragma once

#include <WTypes.h>
#include "MaxHeap.h"

//! MaxNet Errors
enum maxnet_error_t
{
	MAXNET_ERR_NONE = 0,
	MAXNET_ERR_CANCEL,
	MAXNET_ERR_NOMEMORY,
	MAXNET_ERR_FILEIO,
	MAXNET_ERR_BADARGUMENT,
	MAXNET_ERR_NOTCONNECTED,
	MAXNET_ERR_NOTREADY,
	MAXNET_ERR_IOERROR,
	MAXNET_ERR_CMDERROR,
	MAXNET_ERR_HOSTNOTFOUND,
	MAXNET_ERR_BADSOCKETVERSION,
	MAXNET_ERR_WOULDBLOCK,
	MAXNET_ERR_SOCKETLIMIT,
	MAXNET_ERR_CONNECTIONREFUSED,
	MAXNET_ERR_ACCESSDENIED,
	MAXNET_ERR_TIMEOUT,
	MAXNET_ERR_BADADDRESS,
	MAXNET_ERR_HOSTUNREACH,
	MAXNET_ERR_DUPLICATE_JOB_NAME,
	MAXNET_ERR_UNKNOWN
};

//---------------------------------------------------------
//-- Special Types

/*! \sa  Class MaxNetManager
\remarks This structure is available in release 4.0 and later only. \n\n
This structure is used by the Network Rendering API as a handle to a server.
*/
struct HSERVER : public MaxHeapOperators {
	/*! The handle to a server by its mac (Ethernet) address. */
	BYTE addr[8];
};

#define HBSERVER	(BYTE *)(void *) 
typedef DWORD		HJOB;


/*! \sa Class MaxNetManager, Structure WeekSchedule
\remarks This structure is available in release 4.0 and later only.\n\n
This structure is used by the Network Rendering API to store hourly scheduling information.
*/
struct Schedule: public MaxHeapOperators {
	/*! This bit map represents the hourly schedule where 24 bits represent the hours.
	A bit set to 0 indicates it is allowed to work, a bit set to 1 indicates
	its not allowed to work.
	*/
	DWORD hour;
};

/*! \sa Class MaxNetManager, Structure Schedule
\remarks This structure is available in release 4.0 and later only. \n\n
This structure is used by the Network Rendering API to store weekly scheduling information.
*/
struct WeekSchedule: public MaxHeapOperators {
	/*! The hourly schedule configuration for each day of the week. */
	Schedule	day[7];
	/*! The attended priority value which is one of the following values: HIGHPRIORITY, LOWPRIORITY, or IDLEPRIORITY. */
	int		AttendedPriority;
	/*! The unattended priority value which is one of the following values: HIGHPRIORITY, LOWPRIORITY, or IDLEPRIORITY. */
	int		UnattendedPriority;
};
