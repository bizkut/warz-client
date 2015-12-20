// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_LIMBDEF_H
#define NM_LIMBDEF_H

#include "euphoria/erReachLimit.h"
#include "euphoria/erSharedEnums.h"

namespace ER
{

class BodyDef;
class Body;

//----------------------------------------------------------------------------------------------------------------------
/// Definition of the limb, i.e. read-only data.
//----------------------------------------------------------------------------------------------------------------------
class LimbDef
{
public:
  void locate();
  void dislocate();

  ///// Get the index into the physics rig list of parts from 0 < limbPartIndex < m_totalNumParts
  NM_INLINE int getPhysicsRigPartIndex(int limbPartIndex) const;

  ///// Get the index into the physics rig list of joints from 0 < limbJointIndex < m_numJointsInChain
  NM_INLINE int getPhysicsRigJointIndex(int limbJointIndex) const;

  /// Returns the transform of the end effector relative to the root, given the local space
  /// transforms of all the parts (for the whole body).
  NMP::Matrix34 getRootToEndTransform(const NMP::Matrix34* TMs) const;

  //----------------------------------------------------------------------------------------------------------------------
  /// \struct ER::LimbDef::Hamstring
  /// \brief A description of a hamstring
  //----------------------------------------------------------------------------------------------------------------------
  struct Hamstring
  {
    /// a twist, swing1, swing2 weight for each joint in the limb
    NMP::Vector3* m_bendScaleWeights;
    /// stiffness of the hamstring
    float m_stiffness;
    /// total rest length (angle) of the hamstring
    float m_thresholdAngle;
    bool m_enabled;
  };

  // Guideline cone relative to the root that we can access
  ReachLimit m_reachLimit;

  //typedef enum { L_arm, L_leg, L_spine, L_head, L_unknown } LimbType;
  typedef ER::LimbTypeEnum::Type LimbType;
  LimbType m_type;

  /// The limb consists of a collection of parts. A subset of these parts will form a contiguous
  /// chain going from m_rootIndex to m_endIndex, and m_baseIndex will be somewhere along that
  /// chain. There may be additional parts, and these will be used to calculate the mass properties
  /// (etc) of the limb.
  /// The physics rig part index for the root of the limb.
  int m_rootIndex;
  /// The physics rig part index for the end of the limb.
  int m_endIndex;
  /// The physics rig part index for the base of the limb.
  int m_baseIndex;
  /// The physics rig part index for the mid of the limb (elbows and knees, remains invalid for other limbtypes).
  int m_midIndex;
  /// The number of parts before the base (including the root)
  int m_numPartsBeforeBase;
  /// for purposes of mass, momentum etc, this indicates if the root part is considered part of the
  /// limb
  bool m_isRootLimb;

  /// The transformation from the effector body to the point/orientation on that body that is
  /// used as the effector target in the rest of the API. It should be orientated so that the x-axis
  /// lies along the effector "normal" (so rotation about this is null-space rotation). The y/z
  /// directions need to be set only according to the conventions of the behaviours.
  NMP::Matrix34 m_endOffset, m_rootOffset;

  /// The zero and default pose info (internal format: end wrt root offset)
  NMP::Matrix34 m_zeroPoseEndRelativeToRoot;
  NMP::Matrix34 m_defaultPoseEndRelativeToRoot;
  NMP::Quat m_defaultPoseEndQuatRelativeToRoot;
  float m_defaultPoseLimbLength;

  /// The limb will consist of parts in the chain from the root to the end effector (used for IK),
  /// plus some additional parts. The additional parts have indices from getNumPartsInChain() to
  /// getTotalNumParts()-1
  uint32_t m_totalNumParts;
  uint32_t m_numPartsInChain;
  int* m_physicsRigPartIndices;

  uint32_t m_numJointsInChain;
  int* m_physicsRigJointIndices;

  // IK Attributes
  float m_guidePoseWeight;
  float m_neutralPoseWeight;
  bool* m_activeGuidePoseJoints;
  float* m_positionWeights;
  float* m_orientationWeights;

  /// Guide pose info (internal format: joint angle quats)
  NMP::Quat* m_zeroSwivelPoseQuats;
  NMP::Quat* m_negativeSwivelPoseQuats;
  NMP::Quat* m_positiveSwivelPoseQuats;

  Hamstring m_hamstring;

  /// string indicating the limb name
  char* m_name;

  BodyDef* m_bodyDef;

};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int LimbDef::getPhysicsRigPartIndex(int limbPartIndex) const
{
  NMP_ASSERT((uint32_t)limbPartIndex < m_totalNumParts);
  return m_physicsRigPartIndices[limbPartIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int LimbDef::getPhysicsRigJointIndex(int limbJointIndex) const
{
  NMP_ASSERT((uint32_t)limbJointIndex < m_numJointsInChain);
  return m_physicsRigJointIndices[limbJointIndex];
}

}
#endif // NM_LIMB_H
