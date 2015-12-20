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
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMathUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
bool Quat::compare(const NMP::Quat& a, const NMP::Quat& b, const float tolerance)
{
  if (NMP::nmfabs(a.x - b.x) > tolerance)
    return false;

  if (NMP::nmfabs(a.y - b.y) > tolerance)
    return false;

  if (NMP::nmfabs(a.z - b.z) > tolerance)
    return false;

  if (NMP::nmfabs(a.w - b.w) > tolerance)
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NMP::Quat::isValid() const
{
  return ((x * 0 == 0) && (y * 0 == 0) && (z * 0 == 0) && (w * 0 == 0));
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------

