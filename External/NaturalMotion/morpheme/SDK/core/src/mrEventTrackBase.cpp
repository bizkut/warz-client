// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrEventTrackBase.h"
#include "morpheme/mrEventTrackDiscrete.h"
#include "morpheme/mrEventTrackCurve.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackDefBase::locate()
{
  NMP::endianSwap(m_numEvents);
  NMP::endianSwap(m_type);
  NMP::endianSwap(m_userData);
  NMP::endianSwap(m_trackID);

  NMP::endianSwap(m_name);
  REFIX_PTR_RELATIVE(char, m_name, this);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackDefBase::dislocate()
{
  NMP::endianSwap(m_numEvents);
  NMP::endianSwap(m_type);
  NMP::endianSwap(m_userData);
  NMP::endianSwap(m_trackID);

  UNFIX_PTR_RELATIVE(char, m_name, this);
  NMP::endianSwap(m_name);

  return true;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
