// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef NMP_SYNC_H
#define NMP_SYNC_H

#include "NMPlatform/NMPlatform.h"
// NOTE: If this include fails to find NMSync.inl, the platform-specific NMPlatform include is missing from your
//  include path.  It may also be the case that there's no NMSync support on the platform being built.
#include "NMSync.inl"

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Mutex
/// \brief
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class Mutex
{
public:
  bool init();
  bool term();

  bool tryLock();
  bool waitLock();

  bool unlock();
protected:
  PSMutex m_psMutex;
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_SYNC_H
//----------------------------------------------------------------------------------------------------------------------
