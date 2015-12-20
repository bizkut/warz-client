// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma once
#endif
#ifndef  M_VERSION_H
#define  M_VERSION_H
//----------------------------------------------------------------------------------------------------------------------

#define MR_VERSION_MAJOR                 5
#define MR_VERSION_MINOR                 2
#define MR_VERSION_RELEASE               0
#define MR_VERSION_REVISION              0
#define MR_VERSION_SPECIAL_STRING        ""

// Returns true if the version passed in is less than or equal the morpheme revision
#define MR_VERSION_CHECK(Major, Minor, Release, Revision) \
!(((Major < MR_VERSION_MAJOR) || \
((Major == MR_VERSION_MAJOR) && (Minor < MR_VERSION_MINOR)) || \
((Major == MR_VERSION_MAJOR) && (Minor == MR_VERSION_MINOR) && (Release < MR_VERSION_RELEASE)) || \
((Major == MR_VERSION_MAJOR) && (Minor == MR_VERSION_MINOR) && (Release == MR_VERSION_RELEASE) && (Revision <= MR_VERSION_REVISION))))

// Creates an error at compile time if the version passed in is less than or equal to the morpheme revision
#define MR_VERSION_CHECK_ERROR(Major, Minor, Release, Revision) \
  NM_ASSERT_COMPILE_TIME(MR_VERSION_CHECK(Major, Minor, Release, Revision));

// If the version is greater than the product version then a message is printed.  Once the version is less than or 
// equal to the morpheme version then a compile time error is produced
#define MR_DEPRECATED_MESSAGE(Major, Minor, Release, Revision, Message) \
  MR_VERSION_CHECK_ERROR(Major, Minor, Release, Revision) \
  NMP_MSG(Message);

// This number should be bumped every time you make a change to the binary
// format of any serializable asset.
//
#define MR_VERSION_RUNTIME_BINARY        24

//----------------------------------------------------------------------------------------------------------------------
#endif // M_VERSION_H
//----------------------------------------------------------------------------------------------------------------------


