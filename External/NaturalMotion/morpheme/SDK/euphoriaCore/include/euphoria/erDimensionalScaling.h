// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
#pragma once

#ifndef DIMENSIONALSCALING_H
#define DIMENSIONALSCALING_H

// external types
#include "NMPlatform/NMVector3.h"

namespace MR
{
  class PhysicsRig;
}

namespace ER
{

class DimensionalScaling
{
public:
  // Each of these takes an unscaled value (e.g. one appropriate for use on the male human
  // "reference" character) and converts it to a scaled value - i.e. one suitable (to a reasonable
  // approximation) for the actual character.  
  // In each case all they do is multiply the input value by the scale values of its units.
  float scaleTime(float val) const {return val * m_timeScale;}
  float scaleFrequency(float val) const {return val / m_timeScale;}
  float scaleImminence(float val) const {return val / m_timeScale;}
  template<typename T> T scaleDist(const T& val) const {return val * m_distScale;}
  float scaleArea(float val) const {return val * m_distScale * m_distScale;}
  float scaleVolume(float val) const {return val * m_distScale * m_distScale * m_distScale;}
  float scaleAng(float val) const {return val;}
  float scaleMass(float val) const {return val * m_massScale;}
  template<typename T> T scaleVel(const T& val) const {return val * (m_distScale / m_timeScale);}
  template<typename T> T scaleAccel(const T& val) const {return val * (m_distScale / (m_timeScale * m_timeScale));}
  template<typename T> T scaleAngVel(const T& val) const {return val / m_timeScale;}
  template<typename T> T scaleAngAccel(const T& val) const {return val / (m_timeScale * m_timeScale);}
  template<typename T> T scaleInertia(const T& val) const {return val * (m_massScale * m_distScale * m_distScale);}
  template<typename T> T scaleForce(const T& val) const {return val * (m_massScale * m_distScale / (m_timeScale * m_timeScale));}
  template<typename T> T scaleTorque(const T& val) const {return val * (m_massScale * m_distScale * m_distScale / (m_timeScale * m_timeScale));}
  template<typename T> T scaleImpulse(const T& val) const {return val * (m_massScale * m_distScale / m_timeScale);}
  template<typename T> T scaleAngImpulse(const T& val) const {return val * (m_massScale * m_distScale * m_distScale / m_timeScale);}
  template<typename T> T scaleStiffness(const T& val) const {return val / m_timeScale;}
  template<typename T> T scaleStrength(const T& val) const {return val / (m_timeScale * m_timeScale);}
  template<typename T> T scaleDamping(const T& val) const {return val / m_timeScale;}

  void setToDefault() {m_timeScale = m_distScale = m_massScale = 1.0f;}
  void setFromPhysicsRig(const MR::PhysicsRig& physicsRig);

  // Note that in principle there is an angScale... but it's always 1
  float m_timeScale;
  float m_distScale;
  float m_massScale;
};

}

#endif // DIMENSIONALSCALING_H

