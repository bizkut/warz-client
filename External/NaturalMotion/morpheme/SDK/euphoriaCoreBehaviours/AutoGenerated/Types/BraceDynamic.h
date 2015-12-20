#pragma once

/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
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

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_MDF_TYPE_BRACEDYNAMIC_H
#define NM_MDF_TYPE_BRACEDYNAMIC_H

// include definition file to create project dependency
#include "./Definition/Types/BraceDynamic.types"

// external types
#include "NMPlatform/NMVector3.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'BraceDynamic.types'
// Data Payload: 128 Bytes
// Alignment: 16

/// Class for generating the dynamics of bracing. 
/// Note that rather than descriptive variable names, we use short hand in order to match closely the mathematics in
/// nm\codebaseLowLevel\morphoria\sandpit\Behaviours\BraceDynamics.docx

struct BraceDynamic
{

  float maxReachDistance;                    ///< (Length)

  float minReachDistance;                    ///< (Length)


  // functions

  void init(float baseMass, float braceObjectMass, float hazardMass);
  void setPositions(const NMP::Vector3& basePosition, const NMP::Vector3& braceObjectPosition, const NMP::Vector3& hazardPosition);
  void setVelocities(const NMP::Vector3& baseVelocity, const NMP::Vector3& braceObjectVelocity, const NMP::Vector3& hazardVelocity);
  float findInterceptTime(const NMP::Vector3& braceDirection, const NMP::Vector3& interceptVel);
  NMP::Vector3 getWorldVel(const NMP::Vector3& localVel) const;
  NMP::Vector3 getLocalVel(const NMP::Vector3& worldvel) const;
  float getBufferStiffnessScale() const;
  float getControlMass() const;
  float getImpactMass() const;

private:


  unsigned char pad_0[8]; // automatic padding => 16 (av 16)
  NMP::Vector3 V0;  // These positions and velocities are stored relative to the centre of mass and centre of mass velocity, for easier calculation  (Velocity)

  NMP::Vector3 P0;                           ///< (Position)

  NMP::Vector3 Ph;                           ///< (Position)

  NMP::Vector3 Vh;                           ///< (Velocity)

  NMP::Vector3 centreOfMass;                 ///< (Position)

  NMP::Vector3 centreOfMassVelocity;         ///< (Velocity)

  float m;                                   ///< (Mass)

  float mh;                                  ///< (Mass)

  float mb;                                  ///< (Mass)


  // functions

  float findBestInterceptTime(const NMP::Vector3& braceDir, float mc, float Vsqr, float dir, const NMP::Vector3& Va);
  float distanceScale();

public:


  NM_INLINE void operator *= (const float fVal)
  {
    maxReachDistance *= fVal;
    minReachDistance *= fVal;
    V0 *= fVal;
    P0 *= fVal;
    Ph *= fVal;
    Vh *= fVal;
    centreOfMass *= fVal;
    centreOfMassVelocity *= fVal;
    m *= fVal;
    mh *= fVal;
    mb *= fVal;
  }

  NM_INLINE BraceDynamic operator * (const float fVal) const
  {
    BraceDynamic result;
    result.maxReachDistance = maxReachDistance * fVal;
    result.minReachDistance = minReachDistance * fVal;
    result.V0 = V0 * fVal;
    result.P0 = P0 * fVal;
    result.Ph = Ph * fVal;
    result.Vh = Vh * fVal;
    result.centreOfMass = centreOfMass * fVal;
    result.centreOfMassVelocity = centreOfMassVelocity * fVal;
    result.m = m * fVal;
    result.mh = mh * fVal;
    result.mb = mb * fVal;
    return result;
  }

  NM_INLINE void operator += (const BraceDynamic& rhs)
  {
    maxReachDistance += rhs.maxReachDistance;
    minReachDistance += rhs.minReachDistance;
    V0 += rhs.V0;
    P0 += rhs.P0;
    Ph += rhs.Ph;
    Vh += rhs.Vh;
    centreOfMass += rhs.centreOfMass;
    centreOfMassVelocity += rhs.centreOfMassVelocity;
    m += rhs.m;
    mh += rhs.mh;
    mb += rhs.mb;
  }

  NM_INLINE BraceDynamic operator + (const BraceDynamic& rhs) const
  {
    BraceDynamic result;
    result.maxReachDistance = maxReachDistance + rhs.maxReachDistance;
    result.minReachDistance = minReachDistance + rhs.minReachDistance;
    result.V0 = V0 + rhs.V0;
    result.P0 = P0 + rhs.P0;
    result.Ph = Ph + rhs.Ph;
    result.Vh = Vh + rhs.Vh;
    result.centreOfMass = centreOfMass + rhs.centreOfMass;
    result.centreOfMassVelocity = centreOfMassVelocity + rhs.centreOfMassVelocity;
    result.m = m + rhs.m;
    result.mh = mh + rhs.mh;
    result.mb = mb + rhs.mb;
    return result;
  }


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(maxReachDistance), "maxReachDistance");
    NM_VALIDATOR(FloatNonNegative(minReachDistance), "minReachDistance");
    NM_VALIDATOR(Vector3Valid(V0), "V0");
    NM_VALIDATOR(Vector3Valid(P0), "P0");
    NM_VALIDATOR(Vector3Valid(Ph), "Ph");
    NM_VALIDATOR(Vector3Valid(Vh), "Vh");
    NM_VALIDATOR(Vector3Valid(centreOfMass), "centreOfMass");
    NM_VALIDATOR(Vector3Valid(centreOfMassVelocity), "centreOfMassVelocity");
    NM_VALIDATOR(FloatNonNegative(m), "m");
    NM_VALIDATOR(FloatNonNegative(mh), "mh");
    NM_VALIDATOR(FloatNonNegative(mb), "mb");
#endif // NM_CALL_VALIDATORS
  }

}; // struct BraceDynamic


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_BRACEDYNAMIC_H

