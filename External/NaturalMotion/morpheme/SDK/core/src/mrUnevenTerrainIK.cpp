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
#include "morpheme/mrUnevenTerrainIK.h"
#include "NMIK/NMTwoBoneIK.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// UnevenTerrainHipsIK
//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainHipsIK::init(
  const AnimRigDef* rig,
  NMP::DataBuffer* outputBuffer,
  uint32_t         hipsChannelID)
{
  NMP_ASSERT(rig);
  m_rig = rig;
  NMP_ASSERT(outputBuffer);
  m_outputBuffer = outputBuffer;
  m_outputBufferPos = m_outputBuffer->getChannelPos(0);
  NMP_ASSERT(m_outputBufferPos);
  m_outputBufferQuat = m_outputBuffer->getChannelQuat(0);
  NMP_ASSERT(m_outputBufferQuat);

  // Hips joint
  m_hipsChannelID = hipsChannelID;
  NMP_ASSERT(m_hipsChannelID < rig->getNumBones());
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainHipsIK::fkHipsParentJointTM(const NMP::Matrix34& globalRootTM, int32_t globalRootID)
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  const NMP::Quat* bindPoseBufferQuat = m_rig->getBindPoseBoneQuat(0);
  NMP_ASSERT(bindPoseBufferQuat);

  int32_t j = m_rig->getParentBoneIndex(m_hipsChannelID);

  if(j == globalRootID)
  {
    // Use the global root transform
    m_hipsParentJointTM = globalRootTM;
  }
  else
  {
    UnevenTerrainGetTransform(
      m_outputBuffer->getUsedFlags(),
      m_outputBufferPos,
      m_outputBufferQuat,
      bindPoseBufferPos,
      bindPoseBufferQuat,
      j,
      m_hipsParentJointTM);

    while ((j = m_rig->getParentBoneIndex(j)) != globalRootID)
    {
      NMP::Matrix34 T;
      UnevenTerrainGetTransform(
        m_outputBuffer->getUsedFlags(),
        m_outputBufferPos,
        m_outputBufferQuat,
        bindPoseBufferPos,
        bindPoseBufferQuat,
        j,
        T);
      m_hipsParentJointTM.multiply(T);
    }

    // Apply the global root transform
    m_hipsParentJointTM.multiply(globalRootTM);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainHipsIK::fkHipsJointTM()
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  const NMP::Quat* bindPoseBufferQuat = m_rig->getBindPoseBoneQuat(0);
  NMP_ASSERT(bindPoseBufferQuat);

  // Retrieve the current hips joint worldspace TM
  NMP::Matrix34 localHipsJointTM;
  UnevenTerrainGetTransform(
    m_outputBuffer->getUsedFlags(),
    m_outputBufferPos,
    m_outputBufferQuat,
    bindPoseBufferPos,
    bindPoseBufferQuat,
    m_hipsChannelID,
    localHipsJointTM);

  m_hipsJointTM = localHipsJointTM * m_hipsParentJointTM;
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainHipsIK::solve()
{
  // Set the output local-space hip position
  NMP::Vector3 localHipsPos = m_hipsJointTM.translation();
  m_hipsParentJointTM.inverseTransformVector(localHipsPos);

  if (m_outputBuffer->hasChannel(m_hipsChannelID))
  {
    m_outputBufferPos[m_hipsChannelID] = localHipsPos;
  }
  else
  {
    const NMP::Quat* bindPoseBufferQuat = m_rig->getBindPoseBoneQuat(0);
    NMP_ASSERT(bindPoseBufferQuat);
    m_outputBufferPos[m_hipsChannelID] = localHipsPos;
    m_outputBufferQuat[m_hipsChannelID] = bindPoseBufferQuat[m_hipsChannelID];
    m_outputBuffer->setChannelUsed(m_hipsChannelID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainHipsIK::blendWithBuffer(const NMP::DataBuffer* bufferFrom, float weight)
{
  NMP_ASSERT(bufferFrom);
  const NMP::Vector3* bufferFromPos = bufferFrom->getChannelPos(0);
  NMP_ASSERT(bufferFromPos);
  const NMP::Quat* bufferFromQuat = bufferFrom->getChannelQuat(0);
  NMP_ASSERT(bufferFromQuat);
  NMP::Quat qa, qb;
  NMP::Vector3 pa, pb;
  float fromDotTo;
  NMP_ASSERT(weight >= 0.0f && weight <= 1.0f);

  // Quat channel
  NMP_ASSERT(m_outputBuffer->hasChannel(m_hipsChannelID));
  qa = bufferFromQuat[m_hipsChannelID];
  qb = m_outputBufferQuat[m_hipsChannelID];
  fromDotTo = qa.dot(qb);
  if (fromDotTo < 0.0f)
  {
    qb *= -1.0f;
    fromDotTo *= -1.0f;
  }
  m_outputBufferQuat[m_hipsChannelID].fastSlerp(qa, qb, weight, fromDotTo);

  // Pos channel
  pa = bufferFromPos[m_hipsChannelID];
  pb = m_outputBufferPos[m_hipsChannelID];
  m_outputBufferPos[m_hipsChannelID].lerp(pa, pb, weight);
}

//----------------------------------------------------------------------------------------------------------------------
// UnevenTerrainLegIK
//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainLegIK::init(
  const AnimRigDef* rig,
  NMP::DataBuffer* outputBuffer,
  uint32_t         rootChannelID,
  uint32_t         midChannelID,
  uint32_t         endChannelID)
{
  NMP_ASSERT(rig);
  m_rig = rig;
  NMP_ASSERT(outputBuffer);
  m_outputBuffer = outputBuffer;
  m_outputBufferPos = m_outputBuffer->getChannelPos(0);
  NMP_ASSERT(m_outputBufferPos);
  m_outputBufferQuat = m_outputBuffer->getChannelQuat(0);
  NMP_ASSERT(m_outputBufferQuat);

  // End joint
  m_endChannelID = endChannelID;
  NMP_ASSERT(m_endChannelID < rig->getNumBones());

  // Mid joint
  m_midChannelID = midChannelID;
  NMP_ASSERT(m_midChannelID < rig->getNumBones());

  // Root joint
  m_rootChannelID = rootChannelID;
  NMP_ASSERT(m_rootChannelID < rig->getNumBones());

  // Root parent joint
  m_rootParentChannelID = (uint32_t)rig->getParentBoneIndex(m_rootChannelID);
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainLegIK::init(
  const AnimRigDef*   rig,
  NMP::DataBuffer*    outputBuffer,
  uint32_t            rootChannelID,
  uint32_t            midChannelID,
  uint32_t            endChannelID,
  const NMP::Vector3& midJointRotationAxis,
  float               straightestLegFactor)
{
  NMP_ASSERT(rig);
  m_rig = rig;
  NMP_ASSERT(outputBuffer);
  m_outputBuffer = outputBuffer;
  m_outputBufferPos = m_outputBuffer->getChannelPos(0);
  NMP_ASSERT(m_outputBufferPos);
  m_outputBufferQuat = m_outputBuffer->getChannelQuat(0);
  NMP_ASSERT(m_outputBufferQuat);

  m_midJointRotationAxis = midJointRotationAxis;
  NMP_ASSERT(straightestLegFactor > 0.0f && straightestLegFactor <= 1.0f);
  m_straightestLegFactor = straightestLegFactor;

  // End joint
  m_endChannelID = endChannelID;
  NMP_ASSERT(m_endChannelID < rig->getNumBones());

  // Mid joint
  m_midChannelID = midChannelID;
  NMP_ASSERT(m_midChannelID < rig->getNumBones());

  // Root joint
  m_rootChannelID = rootChannelID;
  NMP_ASSERT(m_rootChannelID < rig->getNumBones());

  // Root parent joint
  m_rootParentChannelID = (uint32_t)rig->getParentBoneIndex(m_rootChannelID);
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainLegIK::fkRootParentJointTM(const NMP::Matrix34& globalRootTM, int32_t globalRootID)
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  const NMP::Quat* bindPoseBufferQuat = m_rig->getBindPoseBoneQuat(0);
  NMP_ASSERT(bindPoseBufferQuat);

  // FK the chain root joint out to the rig's root node, thus getting chain root frame in body-local space
  int32_t j = m_rig->getParentBoneIndex(m_rootChannelID);

  if(j == globalRootID)
  {
    m_rootParentJointTM = globalRootTM;
  }
  else
  {
    UnevenTerrainGetTransform(
      m_outputBuffer->getUsedFlags(),
      m_outputBufferPos,
      m_outputBufferQuat,
      bindPoseBufferPos,
      bindPoseBufferQuat,
      j,
      m_rootParentJointTM);

    while ((j = m_rig->getParentBoneIndex(j)) != globalRootID)
    {
      NMP::Matrix34 T;
      UnevenTerrainGetTransform(
        m_outputBuffer->getUsedFlags(),
        m_outputBufferPos,
        m_outputBufferQuat,
        bindPoseBufferPos,
        bindPoseBufferQuat,
        j,
        T);
      m_rootParentJointTM.multiply(T);
    }

    // Apply the global root transform
    m_rootParentJointTM.multiply(globalRootTM);
  }

}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainLegIK::fkRootJointTM()
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  const NMP::Quat* bindPoseBufferQuat = m_rig->getBindPoseBoneQuat(0);
  NMP_ASSERT(bindPoseBufferQuat);

  // Retrieve the current two bone IK chain end joint worldspace TM
  NMP::Matrix34 localRootJointTM;
  UnevenTerrainGetTransform(
    m_outputBuffer->getUsedFlags(),
    m_outputBufferPos,
    m_outputBufferQuat,
    bindPoseBufferPos,
    bindPoseBufferQuat,
    m_rootChannelID,
    localRootJointTM);

  m_rootJointTM = localRootJointTM * m_rootParentJointTM;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 UnevenTerrainLegIK::fkRootJointPos() const
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);

  // Position of the IK root in the world frame
  NMP::Vector3 localRootPos;
  if (m_outputBuffer->hasChannel(m_rootChannelID))
    localRootPos = m_outputBufferPos[m_rootChannelID];
  else
    localRootPos = bindPoseBufferPos[m_rootChannelID];
    
  NMP::Vector3 worldRootPos;
  m_rootParentJointTM.transformVector(localRootPos, worldRootPos);
  
  return worldRootPos;
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainLegIK::fkEndJointTM()
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  const NMP::Quat* bindPoseBufferQuat = m_rig->getBindPoseBoneQuat(0);
  NMP_ASSERT(bindPoseBufferQuat);

  // Root Joint
  NMP::Matrix34 localRootJointTM;
  UnevenTerrainGetTransform(
    m_outputBuffer->getUsedFlags(),
    m_outputBufferPos,
    m_outputBufferQuat,
    bindPoseBufferPos,
    bindPoseBufferQuat,
    m_rootChannelID,
    localRootJointTM);
  m_rootJointTM = localRootJointTM * m_rootParentJointTM;

  // Mid Joint
  NMP::Matrix34 localMidJointTM;
  UnevenTerrainGetTransform(
    m_outputBuffer->getUsedFlags(),
    m_outputBufferPos,
    m_outputBufferQuat,
    bindPoseBufferPos,
    bindPoseBufferQuat,
    m_midChannelID,
    localMidJointTM);
  m_midJointTM = localMidJointTM * m_rootJointTM;

  // End Joint
  NMP::Matrix34 localEndJointTM;
  UnevenTerrainGetTransform(
    m_outputBuffer->getUsedFlags(),
    m_outputBufferPos,
    m_outputBufferQuat,
    bindPoseBufferPos,
    bindPoseBufferQuat,
    m_endChannelID,
    localEndJointTM);
  m_endJointTM = localEndJointTM * m_midJointTM;
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainLegIK::fkStraightestLegLength()
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);

  float distSourceRootToMid;
  if (m_outputBuffer->hasChannel(m_midChannelID))
    distSourceRootToMid = m_outputBufferPos[m_midChannelID].magnitude();
  else
    distSourceRootToMid = bindPoseBufferPos[m_midChannelID].magnitude();

  float distSourceMidToEnd;
  if (m_outputBuffer->hasChannel(m_endChannelID))
    distSourceMidToEnd = m_outputBufferPos[m_endChannelID].magnitude();
  else
    distSourceMidToEnd = bindPoseBufferPos[m_endChannelID].magnitude();

  m_straightestLegLength = distSourceRootToMid + distSourceMidToEnd;
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainLegIK::blendWithBuffer(const NMP::DataBuffer* bufferFrom, float weight)
{
  NMP_ASSERT(bufferFrom);
  const NMP::Vector3* bufferFromPos = bufferFrom->getChannelPos(0);
  NMP_ASSERT(bufferFromPos);
  const NMP::Quat* bufferFromQuat = bufferFrom->getChannelQuat(0);
  NMP_ASSERT(bufferFromQuat);
  NMP::Quat qa, qb;
  NMP::Vector3 pa, pb;
  float fromDotTo;
  NMP_ASSERT(weight >= 0.0f && weight <= 1.0f);

  //------------------------
  // Root joint
  NMP_ASSERT(m_outputBuffer->hasChannel(m_rootChannelID));
  qa = bufferFromQuat[m_rootChannelID];
  qb = m_outputBufferQuat[m_rootChannelID];
  fromDotTo = qa.dot(qb);
  if (fromDotTo < 0.0f)
  {
    qb *= -1.0f;
    fromDotTo *= -1.0f;
  }
  m_outputBufferQuat[m_rootChannelID].fastSlerp(qa, qb, weight, fromDotTo);

  pa = bufferFromPos[m_rootChannelID];
  pb = m_outputBufferPos[m_rootChannelID];
  m_outputBufferPos[m_rootChannelID].lerp(pa, pb, weight);

  //------------------------
  // Mid joint
  NMP_ASSERT(m_outputBuffer->hasChannel(m_midChannelID));
  qa = bufferFromQuat[m_midChannelID];
  qb = m_outputBufferQuat[m_midChannelID];
  fromDotTo = qa.dot(qb);
  if (fromDotTo < 0.0f)
  {
    qb *= -1.0f;
    fromDotTo *= -1.0f;
  }
  m_outputBufferQuat[m_midChannelID].fastSlerp(qa, qb, weight, fromDotTo);

  pa = bufferFromPos[m_midChannelID];
  pb = m_outputBufferPos[m_midChannelID];
  m_outputBufferPos[m_midChannelID].lerp(pa, pb, weight);

  //------------------------
  // End joint
  NMP_ASSERT(m_outputBuffer->hasChannel(m_endChannelID));
  qa = bufferFromQuat[m_endChannelID];
  qb = m_outputBufferQuat[m_endChannelID];
  fromDotTo = qa.dot(qb);
  if (fromDotTo < 0.0f)
  {
    qb *= -1.0f;
    fromDotTo *= -1.0f;
  }
  m_outputBufferQuat[m_endChannelID].fastSlerp(qa, qb, weight, fromDotTo);

  pa = bufferFromPos[m_endChannelID];
  pb = m_outputBufferPos[m_endChannelID];
  m_outputBufferPos[m_endChannelID].lerp(pa, pb, weight);
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainLegIK::solve()
{
  NMP_ASSERT(fabs(1.0f - m_midJointRotationAxis.magnitude()) < 1e-3f);

  // Retrieve the bind pose transform arrays
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  const NMP::Quat* bindPoseBufferQuat = m_rig->getBindPoseBoneQuat(0);
  NMP_ASSERT(bindPoseBufferQuat);

  //------------------------
  // Prepare NMIK geometry

  // Set up the IK joint index array
  uint32_t jointIndex[3] = {m_rootChannelID, m_midChannelID, m_endChannelID};

  // Set up the IK geometry, adding missing channels from the bind pose and activating them in the
  // transform buffer, since we are modifying them.
  NMRU::GeomUtils::PosQuat ikGeometry[4];
  for (uint32_t i = 0; i < 3; ++i)
  {
    int32_t j = jointIndex[i];
    if (!m_outputBuffer->hasChannel(j))
    {
      ikGeometry[i].t = bindPoseBufferPos[j];
      ikGeometry[i].q = bindPoseBufferQuat[j];
      m_outputBuffer->setChannelUsed(j);
    }
    else
    {
      ikGeometry[i].t = m_outputBufferPos[j];
      ikGeometry[i].q = m_outputBufferQuat[j];
    }
  }
  // The fourth 'joint' is the end effector, which is co-located with the end joint in this case.
  ikGeometry[3] = NMRU::GeomUtils::identityPosQuat();

  // End effector target, in the coordinate frame of the parent of the root
  NMP::Matrix34 inverseRootParentTM = m_rootParentJointTM;
  inverseRootParentTM.invertFast();
  NMP::Matrix34 targetMatrix = m_endJointTM * inverseRootParentTM;
  NMRU::GeomUtils::PosQuat target;
  NMRU::GeomUtils::fromMatrix34PosQuat(target, targetMatrix);

  //--------------------------------------------
  // Get the TwoBoneIK parameters
  NMRU::TwoBoneIK::Params ikParams;
  ikParams.hingeAxis = m_midJointRotationAxis;
  ikParams.endJointOrientationWeight = 1.0f;
  ikParams.preventBendingBackwards = true;
  ikParams.maximumReachFraction = m_straightestLegFactor;

  //---------------------------------
  // SOLVE
  NMRU::TwoBoneIK::solveWithEndJoint(&ikParams, ikGeometry, target);

  //------------------------
  // Set results in output buffer
  m_outputBufferQuat[m_rootChannelID] = ikGeometry[0].q;
  m_outputBufferQuat[m_midChannelID] = ikGeometry[1].q;
  m_outputBufferQuat[m_endChannelID] = ikGeometry[2].q;
}

//----------------------------------------------------------------------------------------------------------------------
// UnevenTerrainFootIK
//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::init(
  const AnimRigDef* rig,
  NMP::DataBuffer* outputBuffer,
  uint32_t         numFootJoints,
  const uint32_t*  footChannelIDs)
{
  NMP_ASSERT(rig);
  m_rig = rig;
  NMP_ASSERT(outputBuffer);
  m_outputBuffer = outputBuffer;
  m_outputBufferPos = m_outputBuffer->getChannelPos(0);
  NMP_ASSERT(m_outputBufferPos);
  m_outputBufferQuat = m_outputBuffer->getChannelQuat(0);
  NMP_ASSERT(m_outputBufferQuat);

  // Foot joints
  m_numFootJoints = numFootJoints;
  NMP_ASSERT(m_numFootJoints > 0 && m_numFootJoints <= 3);
  for (uint32_t i = 0; i < m_numFootJoints; ++i)
  {
    m_footChannelIDs[i] = footChannelIDs[i];
    NMP_ASSERT(m_footChannelIDs[i] < rig->getNumBones());
  }

  // Ankle parent joint
  m_ankleParentChannelID = (uint32_t)rig->getParentBoneIndex(m_footChannelIDs[0]);
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::solve()
{
  // Initialise any invalid channels with the bind pose
  uint32_t ankleChannelID = m_footChannelIDs[0];
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  if (!m_outputBuffer->hasChannel(ankleChannelID))
  {
    m_outputBufferPos[ankleChannelID] = bindPoseBufferPos[ankleChannelID];
    m_outputBuffer->setChannelUsed(ankleChannelID);
  }

  // Apply the inverse ankle parent joint transform to find the local channel rotation
  NMP::Matrix34 invAnkleParentTM;
  invAnkleParentTM.invertFast3x3(m_ankleParentJointTM);
  NMP::Matrix34 localAnkleTM;
  localAnkleTM.multiply3x3(m_footJointTM[0], invAnkleParentTM);

  // Set the channel transform
  m_outputBufferQuat[ankleChannelID] = localAnkleTM.toQuat();
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::terrainSurfaceAlignmentTransform(
  const NMP::Vector3& upAxisWS,
  const NMP::Quat& footPreAlignOffsetQuat,
  const NMP::Vector3& targetWorldFootbaseLiftingPos,
  const NMP::Vector3& targetWorldFootbaseLiftingNormal,
  const NMP::Vector3& initWorldFootbasePos,
  float footAlignToSurfaceAngleLimit,
  float footAlignToSurfaceMaxSlopeAngle)
{
  // FK the world foot pivot position
  m_footJointTM[0].transformVector(m_localFootPivotPos, m_worldFootPivotPos);

  NMP_ASSERT(footAlignToSurfaceAngleLimit >= 0.0f && footAlignToSurfaceAngleLimit <= 1.0f); // Cosine of the angle limit
  NMP_ASSERT(footAlignToSurfaceMaxSlopeAngle >= 0.0f && footAlignToSurfaceMaxSlopeAngle <= 1.0f); // Cosine of the angle limit

  // Surface normal rotation. Computes the rotation whose action aligns the
  // up axis vector with the terrain surface normal.
  NMP::Quat footAlignToSurfaceQuat;
  footAlignToSurfaceQuat.forRotation(upAxisWS, targetWorldFootbaseLiftingNormal);

  // Check if the surface slope angle is within the slope limit. Use the half angle formula
  // for this: cos(theta/2) = sqrt(0.5*(1 + cos(theta)))
  float cosTheta = 2.0f * footAlignToSurfaceQuat.w * footAlignToSurfaceQuat.w - 1.0f;
  NMP::Quat footbaseTMQuat = footAlignToSurfaceQuat;

  // Check if the terrain slope angle is greater than the maximum slope gradient limit
  if (cosTheta < footAlignToSurfaceMaxSlopeAngle)
  {
    // The terrain surface is too steep, align the foot back to the canonical ground plane
    footbaseTMQuat.identity();
    cosTheta = 1.0f;
  }

  // Check if the terrain slope angle is greater than the foot surface angle limit
  if (cosTheta < footAlignToSurfaceAngleLimit)
  {
    // The terrain surface is steeper than the maximum foot alignment angle
    NMP::Vector3 v((const NMP::Vector3&)footAlignToSurfaceQuat);
    float mag2 = v.magnitudeSquared();
    float cosHalfTheta = sqrtf(0.5f * (1.0f + footAlignToSurfaceAngleLimit));
    float k = sqrtf((1.0f - cosHalfTheta * cosHalfTheta) / mag2);

    footbaseTMQuat.setXYZW(
      footAlignToSurfaceQuat.x * k,
      footAlignToSurfaceQuat.y * k,
      footAlignToSurfaceQuat.z * k,
      cosHalfTheta);

    cosTheta = footAlignToSurfaceAngleLimit;
  }
  NMP_ASSERT(cosTheta > 0.0f);

  // Apply the pre-rotation offset. i.e. this is essentially the rotation
  // that aligns the tilted foot in its animation pose (animation trajectory
  // ground plane is inclined) back to the canonical ground plane.
  footbaseTMQuat = footbaseTMQuat * footPreAlignOffsetQuat;

  // Compute the transform that rotates the foot about the worldspace pivot
  // point within the foot (i.e. the foot centroid) in order to align the
  // footbase with the terrain surface. The composition of transforms required to
  // do this can be seen as: move the world foot pivot position to the origin,
  // apply the terrain surface rotation, then move it back to its world position.
  NMP::Matrix34 liftingTM;
  liftingTM.fromQuat(footbaseTMQuat);
  NMP::Vector3 v;
  liftingTM.rotateVector(m_worldFootPivotPos, v);
  liftingTM.translation() = m_worldFootPivotPos - v;

  // Having applied a rotation about the foot pivot point, the initial projected
  // point on the footbase that we want to place on the terrain surface has moved.
  // We need to find the vertical displacement that moves the rotated footbase
  // plane onto the terrain surface so that the two are coplanar.

  // Compute the foot lifting translation delta in the vertical direction that moves
  // the initial projected footbase point onto the terrain surface lifting target.
  // Note that this vector should have a component in the up axis direction only.
  NMP::Vector3 footliftingDelta = targetWorldFootbaseLiftingPos - initWorldFootbasePos;

  // Compute the footbase translation delta in the vertical direction between
  // the foot pivot point and this point projected vertically onto the footbase plane.
  // Note that this vector should have a component in the up axis direction only.
  NMP::Vector3 footbaseDelta = initWorldFootbasePos - m_worldFootPivotPos;

  // Apply the foot lifting translation that moves the footbase plane onto the terrain surface
  liftingTM.translation() += (footliftingDelta + footbaseDelta * (1.0f - 1.0f / cosTheta));

  // Compute the worldspace transform of the end joint after applying the foot lifting transform.
  m_footJointTM[0].multiply(liftingTM);
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::computeFootbaseLiftingPos(
  const NMP::Vector3& upAxisWS,
  const NMP::Matrix34& initAnkleJointTM,
  float footbaseDistFromPivotPos,
  NMP::Vector3& drivenWorldFootbaseLiftingPos,
  NMP::Vector3& drivenWorldFootbaseLiftingNormal) const
{
  // Compute the rotation between the initial animation source and the lifted end joint poses.
  NMP::Matrix34 invEndJointTMInit;
  invEndJointTMInit.transpose3x3(initAnkleJointTM);
  NMP::Matrix34 endJointDeltaTM;
  endJointDeltaTM.multiply3x3(invEndJointTMInit, m_footJointTM[0]);

  // Compute the new footbase terrain surface normal
  endJointDeltaTM.rotateVector(upAxisWS, drivenWorldFootbaseLiftingNormal);

  // Compute the driven footbase lifting point by projecting the foot pivot point
  // vertically onto the footbase plane
  float upDotN = upAxisWS.dot(drivenWorldFootbaseLiftingNormal);
  NMP_ASSERT(fabs(upDotN) > 1e-4f);
  float lambda = footbaseDistFromPivotPos / upDotN;
  drivenWorldFootbaseLiftingPos = m_worldFootPivotPos - upAxisWS * lambda;
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::fkAnkleParentJointTM(
  const NMP::Matrix34& globalRootTM,
  int32_t globalRootID)
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  const NMP::Quat* bindPoseBufferQuat = m_rig->getBindPoseBoneQuat(0);
  NMP_ASSERT(bindPoseBufferQuat);

  // FK the chain root joint out to the rig's root node, thus getting chain root frame in body-local space
  int32_t j = m_footChannelIDs[0];
  m_ankleParentJointTM.identity();
  while ((j = m_rig->getParentBoneIndex(j)) != globalRootID)
  {
    NMP::Matrix34 T;
    UnevenTerrainGetTransform(
      m_outputBuffer->getUsedFlags(),
      m_outputBufferPos,
      m_outputBufferQuat,
      bindPoseBufferPos,
      bindPoseBufferQuat,
      j,
      T);
    m_ankleParentJointTM.multiply(T);
  }

  // Apply the global root transform
  m_ankleParentJointTM.multiply(globalRootTM);
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::fkFootJointTMs(bool updateAnkleJoint)
{
  const NMP::Vector3* bindPoseBufferPos = m_rig->getBindPoseBonePos(0);
  NMP_ASSERT(bindPoseBufferPos);
  const NMP::Quat* bindPoseBufferQuat =m_rig->getBindPoseBoneQuat(0);
  NMP_ASSERT(bindPoseBufferQuat);

  // Ankle joint
  if (updateAnkleJoint)
  {
    uint32_t channelID = m_footChannelIDs[0];
    UnevenTerrainGetTransform(
      m_outputBuffer->getUsedFlags(),
      m_outputBufferPos,
      m_outputBufferQuat,
      bindPoseBufferPos,
      bindPoseBufferQuat,
      channelID,
      m_footJointTM[0]);

    m_footJointTM[0].multiply(m_ankleParentJointTM);
  }

  // Foot joints
  for (uint32_t i = 1; i < m_numFootJoints; ++i)
  {
    uint32_t channelID = m_footChannelIDs[i];
    UnevenTerrainGetTransform(
      m_outputBuffer->getUsedFlags(),
      m_outputBufferPos,
      m_outputBufferQuat,
      bindPoseBufferPos,
      bindPoseBufferQuat,
      channelID,
      m_footJointTM[i]);

    channelID = m_rig->getParentBoneIndex(channelID);

    while (channelID != m_footChannelIDs[i-1])
    {
      NMP_ASSERT(channelID != AnimRigDef::INVALID_BONE_INDEX);
      NMP::Matrix34 T;
      UnevenTerrainGetTransform(
        m_outputBuffer->getUsedFlags(),
        m_outputBufferPos,
        m_outputBufferQuat,
        bindPoseBufferPos,
        bindPoseBufferQuat,
        channelID,
        T);

      m_footJointTM[i].multiply(T);
      channelID = m_rig->getParentBoneIndex(channelID);
    }

    m_footJointTM[i].multiply(m_footJointTM[i-1]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::computeWorldFootCentroid(NMP::Vector3& worldFootPos) const
{
  worldFootPos = m_footJointTM[0].translation();
  for (uint32_t i = 1; i < m_numFootJoints; ++i)
    worldFootPos += m_footJointTM[i].translation();
  worldFootPos /= (float)m_numFootJoints;
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::initFootPivotPos(const NMP::Vector3& worldFootPivotPos)
{
  m_worldFootPivotPos = worldFootPivotPos;
  m_footJointTM[0].inverseTransformVector(worldFootPivotPos, m_localFootPivotPos);
}

//----------------------------------------------------------------------------------------------------------------------
void UnevenTerrainFootIK::fkWorldFootPivotPos()
{
  m_footJointTM[0].transformVector(m_localFootPivotPos, m_worldFootPivotPos);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
