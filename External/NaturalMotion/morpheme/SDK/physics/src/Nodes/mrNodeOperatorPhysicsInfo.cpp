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
#include "physics/Nodes/mrNodeOperatorPhysicsInfo.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorPhysicsInfoOutputCPUpdate(
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net)
{
  // Must be pin 0 as this function should only be called in response to a pin 0 query.
  NMP_ASSERT(outputCPPinIndex <  NODE_OPERATOR_PHYSICS_INFO_OUT_CP_PINID_COUNT);

  // Get the animSet
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Get the physics info definition data.
  const AttribDataPhysicsInfoDef* const physicsInfoDef = node->getAttribData<AttribDataPhysicsInfoDef>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, animSet);
  
  // Get the output control param AttribData
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *positionOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_PHYSICS_INFO_OUT_CP_PINID_POSITION);
  OutputCPPin *velocityOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_PHYSICS_INFO_OUT_CP_PINID_VELOCITY);
  
  AttribDataVector3* positionOutCPAttrib = positionOutputCPPin->getAttribData<AttribDataVector3>();
  AttribDataVector3* velocityOutCPAttrib =  velocityOutputCPPin->getAttribData<AttribDataVector3>();

  // Perform the actual task
  positionOutCPAttrib->m_value.setToZero();
  velocityOutCPAttrib->m_value.setToZero();

  PhysicsRig * physicsRig = getPhysicsRig(net);
  if (physicsRig)
  {
    PhysicsRig::Part *part = physicsRig->getPart(physicsInfoDef->m_partIndex);
    if (part)
    {
      positionOutCPAttrib->m_value = part->getPosition();
      velocityOutCPAttrib->m_value = part->getVel();
      
      // Convert data to character controller space if flag is set
      if (!physicsInfoDef->m_outputInWorldSpace)
      {
        CharacterControllerInterface* cc = net->getCharacterController();
        if (cc)
        {
          NMP::Matrix34 t = net->getCharacterPropertiesWorldRootTransform();
          positionOutCPAttrib->m_value -= t.translation();
          t.inverseRotateVector(positionOutCPAttrib->m_value);
          t.inverseRotateVector(velocityOutCPAttrib->m_value);
        }
      }
    }
  }

  // The requested output pin will automatically be flagged as having been updated this frame,
  // but we need to make sure that all pins we have updated are correctly flagged.
  positionOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
  velocityOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
 
  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorPhysicsInfoInitInstance(NodeDef* node, Network* net)
{
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *positionOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_PHYSICS_INFO_OUT_CP_PINID_POSITION);
  OutputCPPin *velocityOutputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_PHYSICS_INFO_OUT_CP_PINID_VELOCITY);

  positionOutputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator());
  velocityOutputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator());
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
