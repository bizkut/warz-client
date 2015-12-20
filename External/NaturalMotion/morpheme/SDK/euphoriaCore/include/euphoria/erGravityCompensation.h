// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_GRAVITYCOMPENSATION_H
#define NM_GRAVITYCOMPENSATION_H

#include "NMPlatform/NMVector3.h"
#include "erDebugDraw.h"

namespace MR
{
  class PhysicsRig;
}

namespace ER
{
class Limb;

/// Class devoted to compensating for the sagging effects of gravity
class GravityCompensation
{
public:
  void init(int numLimbs, class Limb* limbRef);
  void deinit();
  void apply(MR::PhysicsRig* physicsRig, MR::InstanceDebugInterface* pDebugDrawInst);
  void setLimbLength(int limb, int length);
  void setLimbBodyIndex(int limb, int count, int bodyIndex);
  NM_INLINE void setGravity(const NMP::Vector3& gravity) { m_gravity = gravity; }
  /// This scales the torques that are applied
  NM_INLINE void setLimbGravityCompensationMultiplier(uint32_t limb, float compensation)
  {
    NMP_ASSERT(limb < m_numLimbs);
    NMP_ASSERT(compensation >= 0.0f);
    m_limb[limb].gravityCompensationMultiplier = compensation;
  }
  NM_INLINE void setLimbRootSupported(uint32_t limb, float rootSupported)
  {
    NMP_ASSERT(limb < m_numLimbs);
    m_limb[limb].rootSupported = rootSupported;
  }
  NM_INLINE float getLimbRootSupported(uint32_t limb) const { return m_limb[limb].rootSupported; }
  NM_INLINE bool getLimbEndSupported(uint32_t limb)
  {
    NMP_ASSERT(limb < m_numLimbs);
    return m_limb[limb].endSupported;
  }
  NM_INLINE void setLimbEndSupported(uint32_t limb, bool endSupported)
  {
    NMP_ASSERT(limb < m_numLimbs);
    m_limb[limb].endSupported = endSupported;
  }
  /// This function below scales up joint force to account for the resistance of the ground in
  /// achieving an acceleration it is an approximation based on the masses on either side of each
  /// joint 1 is default, which gives no force multiplication at all. Higher values will increase
  /// the maximum multiplication factor... a factor which only gets really high if the feet have
  /// very low mass and are touching the ground
  NM_INLINE void setSupportForceMultiplier(uint32_t limb, float supportForceMultipler)
  {
    NMP_ASSERT(limb < m_numLimbs);
    m_limb[limb].supportForceMultipler = supportForceMultipler;
  }
  NM_INLINE void setTotalMass(float totalMass) { m_totalMass = totalMass; }

  static bool getDebugDrawTorquesFlag();
  static void setDebugDrawTorquesFlag(bool b);
  static bool getDebugDrawGravityFlag();
  static void setDebugDrawGravityFlag(bool b);
  NM_INLINE void setOrderedLimbIndices(int order, int limbIndex) { m_orderedLimbIndices[order] = limbIndex; }
  NM_INLINE void setZMPOffset(const NMP::Vector3& ZMPOffset) { m_ZMPOffset = ZMPOffset; }
  NM_INLINE void setBridgeSupportLimb(int limbIndex) { m_bridgeSupportLimb = limbIndex; } // set away from -1 for spine when front and back limbs are supporting
  NM_INLINE void setLimbIsLeg(int limbIndex) { m_limb[limbIndex].isLeg = true; }

private:
  struct PartData
  {
    NMP::Vector3 subtreeCOM;
    NMP::Vector3 partTorque;
    float subtreeMass;
    float subtreePartMass; // mass of the part only - not including extra held objects
    bool isSupported;
    bool hasIterated;
  };

  typedef struct
  {
    int* bodyIndex;
    int length;
    float rootSupported;
    float gravityCompensationMultiplier;
    float supportForceMultipler;
    bool endSupported;
    bool isLeg;
  } LimbData;

  int m_orderedLimbIndices[32];
  NMP::Vector3 m_gravity;
  NMP::Vector3 m_ZMPOffset;
  NMP::Vector3 m_centreOfMass;
  LimbData* m_limb;
  uint32_t m_numLimbs;

  /// Total mass of the parts plus held objects
  float m_totalMass;
  /// Total mass of the parts
  float m_totalPartMass;
  Limb* m_limbRef;
  int m_bridgeSupportLimb;

  void applyGravityCompensation(
    MR::InstanceDebugInterface* pDebugDrawInst,
    int limbIndex,
    MR::PhysicsRig* physicsRig,
    PartData* partData,
    const NMP::Vector3* grav);
};

}
#endif // NM_GRAVITYCOMPENSATION_H
