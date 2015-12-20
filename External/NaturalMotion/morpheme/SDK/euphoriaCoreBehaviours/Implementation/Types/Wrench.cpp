/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

#include "Types/Wrench.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

Wrench::Wrench()
{
  setToZero();
}
//----------------------------------------------------------------------------------------------------------------------
void Wrench::setToZero()
{
  force.setToZero();
  torque.setToZero();
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

