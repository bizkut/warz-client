//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/

#pragma once
#include "coreexp.h"
#include "BuildWarnings.h"

/** \deprecated This method has been deprecated in 3ds Max 2011. 
 * Starting with 3ds Max 2010, this method always returns 0. 
 * Plug-in developers will have to implement their own licensing mechanism.
 */
MAX_DEPRECATED CoreExport unsigned int HardwareLockID();
/*! \remarks Returns TRUE if the application is running under a trial license, as
opposed to a full, authorized license; otherwise FALSE. */
extern CoreExport bool IsTrialLicense();
//! Returns true if the application is running under a network license.
extern CoreExport bool IsNetworkLicense();
//! \deprecated  As of R4 this method is no longer supported and always returns false.
MAX_DEPRECATED extern CoreExport bool IsEmergencyLicense();


