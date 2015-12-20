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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_UNEVEN_TERRAIN_IK_H
#define MR_UNEVEN_TERRAIN_IK_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMBuffer.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMatrix34.h"
#include "morpheme/mrRig.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Utility function
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE
void UnevenTerrainGetTransform(
  const NMP::BitArray* outputBufferUsedFlags,
  const NMP::Vector3* outputBufferPos,
  const NMP::Quat* outputBufferQuat,
  const NMP::Vector3* bindPoseBufferPos,
  const NMP::Quat* bindPoseBufferQuat,
  int32_t channelIndex,
  NMP::Matrix34& T)
{
  NMP::Quat q;
  NMP::Vector3 v;
  if (outputBufferUsedFlags->isBitSet(channelIndex))
  {
    q = outputBufferQuat[channelIndex];
    //q.fastNormalise();
    v = outputBufferPos[channelIndex];
  }
  else
  {
    q = bindPoseBufferQuat[channelIndex]; // Should already be normalised
    v = bindPoseBufferPos[channelIndex];
  }
  T.initialise(q, v);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::UnevenTerrainHipsIK
/// \brief A simple chain inverse kinematics solver for the Hips component of the uneven terrain node.
/// The 'Hips' bone that is used for character height control is the root-most joint that is allowed to translate.
/// Height control increases the range of motion of the leg IK, allowing it to adapt better to uneven surfaces.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class UnevenTerrainHipsIK
{
public:
  //---------------------------------------------------------------------
  /// \name   Initialisation
  /// \brief  Functions to initialise the hip height solver.
  //---------------------------------------------------------------------
  //@{
  UnevenTerrainHipsIK() {}
  ~UnevenTerrainHipsIK() {}

  // Initialisation
  void init(
    const AnimRigDef* rig,
    NMP::DataBuffer* outputBuffer,
    uint32_t         hipsChannelID);

  // Channel IDs
  uint32_t getHipsChannelID() const { return m_hipsChannelID; }
  //@}

  //---------------------------------------------------------------------
  /// \name   IK solve
  /// \brief  Functions to perform the hip height solve
  //---------------------------------------------------------------------
  //@{
  void solve();
  //@}

  //---------------------------------------------------------------------
  /// \name   FK compute
  /// \brief  Functions to compute various world space transforms by performing
  ///         an FK step on the local space transforms in the data buffer.
  //---------------------------------------------------------------------
  //@{
  void fkHipsParentJointTM(
    const NMP::Matrix34& globalRootTM,
    int32_t globalRootID = 0);

  void fkHipsJointTM();

  // Worldspace joint TMs
  const NMP::Matrix34& getHipsParentJointTM() const { return m_hipsParentJointTM; }
  void setHipsParentJointTM(const NMP::Matrix34& hipsParentJointTM) { m_hipsParentJointTM = hipsParentJointTM; }

  const NMP::Matrix34& getHipsJointTM() const { return m_hipsJointTM; }
  NMP::Matrix34& getHipsJointTM() { return m_hipsJointTM; }
  void setHipsJointTM(const NMP::Matrix34& targetTM) { m_hipsJointTM = targetTM; }
  //@}

  //---------------------------------------------------------------------
  /// \name   Data buffer functions
  /// \brief  Functions that operate on the local space transform buffer
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to blend the local space orientation of the hips joint in the data
  /// buffer with a specified input buffer. A weight of 0 sets the joint orientation from
  /// bufferFrom while a weight of 1 preserves the joint orientation from the data buffer.
  void blendWithBuffer(const NMP::DataBuffer* bufferFrom, float weight);
  //@}

protected:
  // Transforms
  NMP::Matrix34                   m_hipsParentJointTM;
  NMP::Matrix34                   m_hipsJointTM;

  // Rig joint indices
  uint32_t                        m_hipsChannelID;

  // Data buffers
  const AnimRigDef*               m_rig;
  NMP::DataBuffer*                m_outputBuffer;
  NMP::Vector3*                   m_outputBufferPos;
  NMP::Quat*                      m_outputBufferQuat;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::UnevenTerrainLegIK
/// \brief A simple two bone inverse kinematics solver for the leg component of the uneven terrain node.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class UnevenTerrainLegIK
{
public:
  //---------------------------------------------------------------------
  /// \name   Initialisation
  /// \brief  Functions to initialise the 2-bone IK solver.
  //---------------------------------------------------------------------
  //@{
  UnevenTerrainLegIK() {}
  ~UnevenTerrainLegIK() {}

  /// \brief Minimal initialisation of the IK chain and data buffers. Use this method
  /// of initialisation if you only intend to perform an FK step to compute the worldspace
  /// configuration of the 2-bone system joints.
  void init(
    const AnimRigDef* rig,
    NMP::DataBuffer* outputBuffer,
    uint32_t         rootChannelID,
    uint32_t         midChannelID,
    uint32_t         endChannelID);

  /// \brief Initialisation of the IK chain, data buffers and 2-bone system properties.
  /// This method of initialisation also invalidates the angular velocity and acceleration
  /// clamping limits.
  void init(
    const AnimRigDef*   rig,
    NMP::DataBuffer*    outputBuffer,
    uint32_t            rootChannelID,
    uint32_t            midChannelID,
    uint32_t            endChannelID,
    const NMP::Vector3& midJointRotationAxis,
    float               straightestLegFactor);

  // Channel IDs
  uint32_t getRootParentChannelID() const { return m_rootParentChannelID; }
  uint32_t getRootChannelID() const { return m_rootChannelID; }
  uint32_t getMidChannelID() const { return m_midChannelID; }
  uint32_t getEndChannelID() const { return m_endChannelID; }
  //@}

  //---------------------------------------------------------------------
  /// \name   IK solve
  /// \brief  Functions to perform the 2-bone Leg IK solve
  //---------------------------------------------------------------------
  //@{
  void solve();
  //@}

  //---------------------------------------------------------------------
  /// \name   FK compute
  /// \brief  Functions to compute various world space transforms by performing
  ///         an FK step on the local space transforms in the data buffer.
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to accumulate the local space transforms in the data buffer
  /// from the parent joint of the IK system root joint until (not including) the
  /// specified global root joint index. The globalRootTM is then applied on top
  /// of the accumulated transforms. i.e. this may be the world transform of the
  /// character controller in the scene.
  void fkRootParentJointTM(
    const NMP::Matrix34& globalRootTM,
    int32_t globalRootID = 0);

  /// \brief Function to accumulate the local space transform in the data buffer
  /// between the IK system root joint and its parent joint.
  void fkRootJointTM();
  NMP::Vector3 fkRootJointPos() const;

  /// \brief Function to accumulate the local space transforms in the data buffer
  /// from the IK system end joint until the IK system root joint.
  void fkEndJointTM();

  /// \brief Function to accumulate the length of the joints between the IK system
  /// end joint and the IK system root joint.
  void fkStraightestLegLength();

  /// \brief Function to recover the accumulated world space IK system root parent joint transform.
  const NMP::Matrix34& getRootParentJointTM() const { return m_rootParentJointTM; }
  void setRootParentJointTM(const NMP::Matrix34& rootParentJointTM) { m_rootParentJointTM = rootParentJointTM; }

  /// \brief Function to recover the accumulated world space IK system root joint transform.
  const NMP::Matrix34& getRootJointTM() const { return m_rootJointTM; }
  void setRootJointTM(const NMP::Matrix34& rootJointTM) { m_rootJointTM = rootJointTM; }

  /// \brief Function to recover the accumulated world space IK system mid joint transform.
  const NMP::Matrix34& getMidJointTM() const { return m_midJointTM; }
  void setMidJointTM(const NMP::Matrix34& midJointTM) { m_midJointTM = midJointTM; }

  /// \brief Function to recover the accumulated world space IK system end joint transform.
  const NMP::Matrix34& getEndJointTM() const { return m_endJointTM; }

  /// \brief Function to recover the accumulated world space IK system end joint transform.
  NMP::Matrix34& getEndJointTM() { return m_endJointTM; }

  /// \brief Function to set the desired world space IK system end joint transform.
  void setEndJointTM(const NMP::Matrix34& targetTM) { m_endJointTM = targetTM; }

  /// \brief Function to recover the actual accumulated length of the joints in the IK chain
  float getStraightestLegLength() const { return m_straightestLegLength; }
  void setStraightestLegLength(float straightestLegLength) { m_straightestLegLength = straightestLegLength; }

  /// \brief Function to compute the straightest allowable length of the joint chain that the
  /// IK solver will try to achieve. The IK solves for the straightest allowable leg length
  /// in order to prevent bad jitter in nearly straight leg cases. i.e. a small change in the
  /// end joint target position can cause a fairly large change in the knee angle, which can
  /// be quite noticeable in idle type animations.
  float getStraightestAllowableLegLength() const { return m_straightestLegFactor * m_straightestLegLength; }
  //@}

  //---------------------------------------------------------------------
  /// \name   Data buffer functions
  /// \brief  Functions that operate on the local space transform buffer
  //---------------------------------------------------------------------
  //@{
  const NMP::Quat& getRootChannelQuat() const { return m_outputBufferQuat[m_rootChannelID]; }
  const NMP::Quat& getMidChannelQuat() const { return m_outputBufferQuat[m_midChannelID]; }
  const NMP::Quat& getEndChannelQuat() const { return m_outputBufferQuat[m_endChannelID]; }

  void setRootChannelQuat(const NMP::Quat& rootChannelQuat) { m_outputBufferQuat[m_rootChannelID] = rootChannelQuat; }
  void setMidChannelQuat(const NMP::Quat& midChannelQuat) { m_outputBufferQuat[m_midChannelID] = midChannelQuat; }
  void setEndChannelQuat(const NMP::Quat& endChannelQuat) { m_outputBufferQuat[m_endChannelID] = endChannelQuat; }

  /// \brief Function to blend the local space joint orientations in the data buffer with
  /// a specified input buffer. A weight of 0 sets the joint orientations from bufferFrom
  // while a weight of 1 preserves the joint orientations from the data buffer.
  void blendWithBuffer(const NMP::DataBuffer* bufferFrom, float weight);
  //@}

protected:
  // Hinge axis of 2-bone system in the local co-ordinate frame of the Root joint
  NMP::Vector3                    m_midJointRotationAxis;

  // Current frames world space FK transforms (maintained by user)
  NMP::Matrix34                   m_rootParentJointTM;
  NMP::Matrix34                   m_rootJointTM;
  NMP::Matrix34                   m_midJointTM;
  NMP::Matrix34                   m_endJointTM;

  // IK parameters
  float                           m_straightestLegLength; // Actual straightest leg length
  float                           m_straightestLegFactor; // Factor for straightest allowable leg length

  // Rig joint indices
  uint32_t                        m_rootParentChannelID;
  uint32_t                        m_rootChannelID;
  uint32_t                        m_midChannelID;
  uint32_t                        m_endChannelID;

  // Data buffers
  const AnimRigDef*               m_rig;
  NMP::DataBuffer*                m_outputBuffer;
  NMP::Vector3*                   m_outputBufferPos;
  NMP::Quat*                      m_outputBufferQuat;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::UnevenTerrainFootIK
/// \brief A simple chain inverse kinematics solver for the foot component of the uneven terrain node.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class UnevenTerrainFootIK
{
public:
  //---------------------------------------------------------------------
  /// \name   Initialisation
  /// \brief  Functions to initialise the foot IK solver.
  //---------------------------------------------------------------------
  //@{
  UnevenTerrainFootIK() {}
  ~UnevenTerrainFootIK() {}

  // Initialisation
  void init(
    const AnimRigDef* rig,
    NMP::DataBuffer* outputBuffer,
    uint32_t         numFootJoints,
    const uint32_t*  footChannelIDs);

  // Channel IDs
  uint32_t getAnkleParentChannelID() const { return m_ankleParentChannelID; }
  uint32_t getNumFootJoints() const { return m_numFootJoints; }
  const uint32_t* getFootChannelIDs() const { return m_footChannelIDs; }
  //@}

  //---------------------------------------------------------------------
  /// \name   IK solve
  /// \brief  Functions to perform the Foot IK solve
  //
  //---------------------------------------------------------------------
  //@{

  /// \brief Function that sets the output buffer ankle joint quat channel with the
  /// specified transform
  void solve();

  /// \brief Function to apply foot lifting and surface alignment to the ankle joint
  void terrainSurfaceAlignmentTransform(
    const NMP::Vector3& upAxisWS,                           ///< IN: The world up axis direction vector
    const NMP::Quat& footPreAlignOffsetQuat,                ///< IN: The foot pre-alignment rotation (removes offset of inclined trajectory plane)
    const NMP::Vector3& targetWorldFootbaseLiftingPos,      ///< IN: The target footbase lifting position
    const NMP::Vector3& targetWorldFootbaseLiftingNormal,   ///< IN: The target footbase terrain normal
    const NMP::Vector3& initWorldFootbasePos,               ///< IN: The initial projected footbase point on the canonical ground plane
    float footAlignToSurfaceAngleLimit,                     ///< IN: The maximum terrain surface angle limit (cosine of angle) that the foot can be aligned with. Foot alignment is clamped to this limit.
    float footAlignToSurfaceMaxSlopeAngle                   ///< IN: The maximum terrain slope gradient limit (cosine of angle) that is considered for foot alignment. The foot is aligned back to the ground plane if beyond this limit.
    );

  /// \brief Function to compute the driven foot base lifting pos. Consider the footbase as
  /// being the plane a distance d below the foot pivot point. The foot is rotated about the
  /// pivot for terrain surface alignment, however the footbase lifting pos can be seen as
  /// the intersection point derived by projecting downwards from the pivot point onto the
  /// rotated footbase plane.
  void computeFootbaseLiftingPos(
    const NMP::Vector3& upAxisWS,                           ///< IN:  The world up axis direction vector
    const NMP::Matrix34& initAnkleJointTM,                  ///< IN:  The initial animation pose ankle joint TM
    float footbaseDistFromPivotPos,                         ///< IN:  The distance of the footbase from the pivot point
    NMP::Vector3& drivenWorldFootbaseLiftingPos,            ///< OUT: The driven footbase lifting position.
    NMP::Vector3& drivenWorldFootbaseLiftingNormal          ///< OUT: The driven footbase lifting normal.
    ) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   FK compute
  /// \brief  Functions to compute various world space transforms by performing
  ///         an FK step on the local space transforms in the data buffer.
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to accumulate the local space transforms in the data buffer
  /// from the ankle parent joint until (not including) the specified global root
  /// joint index. The globalRootTM is then applied on top of the accumulated transforms.
  void fkAnkleParentJointTM(
    const NMP::Matrix34& globalRootTM,
    int32_t globalRootID = 0);

  /// \brief Function to accumulate the local space transforms in the data buffer
  /// from the foot end joint until the ankle joint.
  void fkFootJointTMs(bool updateAnkleJoint);

  // Ankle parent joint
  const NMP::Matrix34& getAnkleParentJointTM() const { return m_ankleParentJointTM; }
  NMP::Matrix34& getAnkleParentJointTM() { return m_ankleParentJointTM; }
  void setAnkleParentJointTM(const NMP::Matrix34& ankleParentJointTM) { m_ankleParentJointTM = ankleParentJointTM; }

  // Joint transforms
  const NMP::Matrix34& getAnkleJointTM() const { return m_footJointTM[0]; }
  NMP::Matrix34& getAnkleJointTM() { return m_footJointTM[0]; }
  void setAnkleJointTM(const NMP::Matrix34& ankleJointTM) { m_footJointTM[0] = ankleJointTM; }

  /// \brief Function to compute the centroid of the foot joints
  void computeWorldFootCentroid(NMP::Vector3& worldFootPos) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Foot pivot position
  /// \brief  Functions to manipulate the additional foot pivot joint that is
  ///         parented to the ankle joint.
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to set the target world foot pivot position and compute the
  /// corresponding local foot pivot position in the frame of the ankle joint
  void initFootPivotPos(const NMP::Vector3& worldFootPivotPos);

  // FK the world foot pivot position from the ankle joint
  void fkWorldFootPivotPos();

  // Target world foot pivot position
  const NMP::Vector3& getWorldFootPivotPos() const { return m_worldFootPivotPos; }
  void setWorldFootPivotPos(const NMP::Vector3& worldFootPivotPos) { m_worldFootPivotPos = worldFootPivotPos; }

  // Local foot pivot position (in the frame of the ankle joint)
  const NMP::Vector3& getLocalFootPivotPos() const { return m_localFootPivotPos; }
  void setLocalFootPivotPos(const NMP::Vector3& localFootPivotPos) { m_localFootPivotPos = localFootPivotPos; }
  //@}

protected:
  // Foot joint Transforms
  NMP::Matrix34                   m_ankleParentJointTM;
  NMP::Matrix34                   m_footJointTM[3];           ///< World transforms for the foot joints.

  // Foot pivot point. Point within the foot about which the foot pivots for foot lifting and surface alignment.
  NMP::Vector3                    m_worldFootPivotPos;        ///< World foot pivot position
  NMP::Vector3                    m_localFootPivotPos;        ///< Foot pivot position in the frame of the ankle joint

  // Rig joint indices
  uint32_t                        m_numFootJoints;
  uint32_t                        m_ankleParentChannelID;
  uint32_t                        m_footChannelIDs[3];

  // Data buffers
  const AnimRigDef*               m_rig;
  NMP::DataBuffer*                m_outputBuffer;
  NMP::Vector3*                   m_outputBufferPos;
  NMP::Quat*                      m_outputBufferQuat;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_UNEVEN_TERRAIN_IK_H
//----------------------------------------------------------------------------------------------------------------------
