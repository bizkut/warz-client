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
#include "morpheme/Nodes/mrNodeOperatorRayCast.h"
#include "morpheme/mrCharacterControllerInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorRayCastOutputCPUpdate(
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net)
{
  // Must be pin 0 as this function should only be called in response to a pin 0 query.
  NMP_ASSERT(outputCPPinIndex <  NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_COUNT);

  // Get the ray cast definition data.
  const AttribDataRayCastDef* const rayCastDef = node->getAttribData<AttribDataRayCastDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  // Get the output control param AttribData
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *distanceOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_DISTANCE);
  OutputCPPin *positionOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_POSITION);
  OutputCPPin *hitNormalOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_NORMAL);
  OutputCPPin *hitPitchDownAngleOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_PITCH_DOWN_ANGLE);
  OutputCPPin *hitRollRightAngleOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_ROLL_RIGHT_ANGLE);
  
  AttribDataFloat* distanceOutCPAttrib = distanceOutputCPPin->getAttribData<AttribDataFloat>();
  AttribDataVector3* positionOutCPAttrib = positionOutputCPPin->getAttribData<AttribDataVector3>();
  AttribDataVector3* hitNormalOutCPAttrib =  hitNormalOutputCPPin->getAttribData<AttribDataVector3>();
  AttribDataFloat* hitPitchDownAngleOutCPAttrib = hitPitchDownAngleOutputCPPin->getAttribData<AttribDataFloat>();
  AttribDataFloat* hitRollRightAngleOutCPAttrib = hitRollRightAngleOutputCPPin->getAttribData<AttribDataFloat>();

  // Perform the actual task
  positionOutCPAttrib->m_value.setToZero();
  hitNormalOutCPAttrib->m_value.setToZero();
  distanceOutCPAttrib->m_value = -1.0f;
  hitPitchDownAngleOutCPAttrib->m_value = 0.0f;
  hitRollRightAngleOutCPAttrib->m_value = 0.0f;

  CharacterControllerInterface* cc = net->getCharacterController();
  if (cc)
  {
    NMP::Matrix34 t = net->getCharacterPropertiesWorldRootTransform();
    NMP::Vector3 start;
    t.transformVector(rayCastDef->m_rayStart, start);
    NMP::Vector3 delta = rayCastDef->m_rayDelta;
    if (rayCastDef->m_useLocalOrientation)
      t.rotateVector(rayCastDef->m_rayDelta, delta);

    NMP::Vector3 raycastHitVelocity;
    bool hit = cc->castRayIntoCollisionWorld(start,
               delta,
               distanceOutCPAttrib->m_value,
               positionOutCPAttrib->m_value,
               hitNormalOutCPAttrib->m_value,
               raycastHitVelocity,
               net);
    if (hit)
    {
      t.inverseRotateVector(hitNormalOutCPAttrib->m_value);
      NMP_ASSERT_MSG((rayCastDef->m_upAxisIndex == 1 || rayCastDef->m_upAxisIndex == 2), "Invalid up axis in RayCastDef!");
      
      if(rayCastDef->m_upAxisIndex == 1)
      {
        // Y-Up; roll and pitch are in the local frame, and z is forwards
        hitPitchDownAngleOutCPAttrib->m_value = NMP::radiansToDegrees(atan2(hitNormalOutCPAttrib->m_value.z, hitNormalOutCPAttrib->m_value.y));
        hitRollRightAngleOutCPAttrib->m_value = NMP::radiansToDegrees(atan2(-hitNormalOutCPAttrib->m_value.x, hitNormalOutCPAttrib->m_value.y));
      }
      else
      {
        // Z-Up; roll and pitch are in the local frame, and y is forwards
        hitPitchDownAngleOutCPAttrib->m_value = NMP::radiansToDegrees(atan2(hitNormalOutCPAttrib->m_value.y, hitNormalOutCPAttrib->m_value.z));
        hitRollRightAngleOutCPAttrib->m_value = NMP::radiansToDegrees(atan2(-hitNormalOutCPAttrib->m_value.x, hitNormalOutCPAttrib->m_value.z));
      }
    }
  }

  // The requested output pin will automatically be flagged as having been updated this frame,
  // but we need to make sure that all pins we have updated are correctly flagged.
  distanceOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
  positionOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
  hitNormalOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
  hitPitchDownAngleOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
  hitRollRightAngleOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
 
  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorRayCastInitInstance(NodeDef* node, Network* net)
{
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *distanceOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_DISTANCE);
  OutputCPPin *positionOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_POSITION);
  OutputCPPin *hitNormalOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_NORMAL);
  OutputCPPin *hitPitchDownAngleOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_PITCH_DOWN_ANGLE);
  OutputCPPin *hitRollRightAngleOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_ROLL_RIGHT_ANGLE);

  NMP::Vector3 vZero(NMP::Vector3::InitZero);
  distanceOutputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  positionOutputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator(), vZero);
  hitNormalOutputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator(), vZero);
  hitPitchDownAngleOutputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  hitRollRightAngleOutputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
