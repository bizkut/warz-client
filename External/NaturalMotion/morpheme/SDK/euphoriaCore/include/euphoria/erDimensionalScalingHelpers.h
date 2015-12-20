// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_DIMENSIONAL_SCALING_HELPERS_H
#define NM_DIMENSIONAL_SCALING_HELPERS_H

#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMMathUtils.h"

namespace ER
{
  // These are just helper functions - normally use the macros below in Euphoria/behaviour code
  template<typename T> T scaleFrequency(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleFrequency(val);}
  template<typename T> T scaleTime(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleTime(val);}
  template<typename T> T scaleDist(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleDist(val);}
  template<typename T> T scaleArea(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleArea(val);}
  template<typename T> T scaleVolume(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleVolume(val);}
  template<typename T> T scaleAng(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleAng(val);}
  template<typename T> T scaleMass(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleMass(val);}
  template<typename T> T scaleVel(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleVel(val);}
  template<typename T> T scaleAccel(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleAccel(val);}
  template<typename T> T scaleAngVel(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleAngVel(val);}
  template<typename T> T scaleAngAccel(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleAngAccel(val);}
  template<typename T> T scaleInertia(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleInertia(val);}
  template<typename T> T scaleForce(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleForce(val);}
  template<typename T> T scaleTorque(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleTorque(val);}
  template<typename T> T scaleImpulse(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleImpulse(val);}
  template<typename T> T scaleAngImpulse(const T &val, const DimensionalScaling &dimensionalScaling) {return dimensionalScaling.scaleAngImpulse(val);}

  template<typename T> T scaleStiffness(const T &val, const DimensionalScaling &dimensionalScaling) {return val / dimensionalScaling.m_timeScale;}
  template<typename T> T scaleStrength(const T &val, const DimensionalScaling &dimensionalScaling) {return val / (dimensionalScaling.m_timeScale * dimensionalScaling.m_timeScale);}
  template<typename T> T scaleDamping(const T &val, const DimensionalScaling &dimensionalScaling) {return val / dimensionalScaling.m_timeScale;}
}

#endif

// The following macros let you call the scaling function from (normally) different places in the
// Euphoria behaviour code. You need to define SCALING_SOURCE to indicate where the dimensional
// scaling comes from.

#ifndef SCALING_SOURCE
#error "#define SCALING_SOURCE before including DimensionalScalingHelpers.h"
// example #defines for SCALING_SOURCE, depending where you want the info to come from 
#define SCALING_SOURCE data->dimensionalScaling
#define SCALING_SOURCE owner->data->dimensionalScaling
#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#define SCALING_SOURCE owner->owner->owner->data->dimensionalScaling
#endif

#ifdef SCALE_TIME
#undef SCALE_TIME
#undef SCALE_DIST
#undef SCALE_ANG
#undef SCALE_MASS
#undef SCALE_VEL
#undef SCALE_ANGVEL
#undef SCALE_ACCEL
#undef SCALE_ANGACCEL
#undef SCALE_FORCE
#undef SCALE_TORQUE
#undef SCALE_IMPULSE
#undef SCALE_ANGIMPULSE

#undef SCALE_STIFFNESS
#undef SCALE_STRENGTH
#undef SCALE_DAMPING
#endif

#define       SCALE_TIME(v) ER::scaleTime(v,       SCALING_SOURCE)
#define       SCALE_DIST(v) ER::scaleDist(v,       SCALING_SOURCE)
#define       SCALE_AREA(v) ER::scaleArea(v,       SCALING_SOURCE)
#define     SCALE_VOLUME(v) ER::scaleVolume(v,     SCALING_SOURCE)
#define        SCALE_ANG(v) v
#define       SCALE_MASS(v) ER::scaleMass(v,       SCALING_SOURCE)
#define        SCALE_VEL(v) ER::scaleVel(v,        SCALING_SOURCE)
#define     SCALE_ANGVEL(v) ER::scaleAngVel(v,     SCALING_SOURCE)
#define      SCALE_ACCEL(v) ER::scaleAccel(v,      SCALING_SOURCE)
#define   SCALE_ANGACCEL(v) ER::scaleAngAccel(v,   SCALING_SOURCE)
#define    SCALE_INERTIA(v) ER::scaleInertia(v,    SCALING_SOURCE)
#define      SCALE_FORCE(v) ER::scaleForce(v,      SCALING_SOURCE)
#define     SCALE_TORQUE(v) ER::scaleTorque(v,     SCALING_SOURCE)
#define    SCALE_IMPULSE(v) ER::scaleImpulse(v,    SCALING_SOURCE)
#define SCALE_ANGIMPULSE(v) ER::scaleAngImpulse(v, SCALING_SOURCE)
#define  SCALE_FREQUENCY(v) ER::scaleFrequency(v,  SCALING_SOURCE)
#define  SCALE_STIFFNESS(v) ER::scaleStiffness(v,  SCALING_SOURCE)
#define   SCALE_STRENGTH(v) ER::scaleStrength(v,   SCALING_SOURCE)
#define    SCALE_DAMPING(v) ER::scaleDamping(v,    SCALING_SOURCE)

// imminence is defined as 1/time
#define SCALE_IMMINENCE SCALE_FREQUENCY

