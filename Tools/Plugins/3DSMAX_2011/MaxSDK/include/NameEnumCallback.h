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

#include "MaxHeap.h"
#include "BuildWarnings.h"
#include "strbasic.h"

//! \deprecated see : class AssetEnumCallback: public MaxHeapOperators {
class NameEnumCallback: public MaxHeapOperators {
	public:
		/*! \remarks Destructor. */
		virtual ~NameEnumCallback() {}
		/*! \remarks This method is used to record the name passed.
		\par Parameters:
		<b>MCHAR *name</b>\n\n
		The name to store. */
		MAX_DEPRECATED void RecordName(MCHAR *name);
	};

