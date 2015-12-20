// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
#pragma once

#ifndef REACHLIMIT_H
#define REACHLIMIT_H

// external types
#include "NMPlatform/NMMatrix34.h"
#include "morpheme/mrInstanceDebugInterface.h"

namespace ER
{

struct ReachLimit
{
  /// Reaching is around the Z axis, with limits determined by rotations around the X/Y axes
  NMP::Matrix34 m_transform; // offset from the root part transform in the def (before the locator offset is applied) OR world space transform in the limb
  float m_angle;
  float m_distance;

  NMP::Vector3 getReachConeDir() const { return m_transform.zAxis(); }
  void draw(MR::InstanceDebugInterface* pDebugDrawInst, float coneAngle = 0.0f, const NMP::Colour& colour = NMP::Colour::RED) const;
  void locate();
  void dislocate();
};

}

#endif

