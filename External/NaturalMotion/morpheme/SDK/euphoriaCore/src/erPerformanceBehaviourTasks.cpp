// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrNetworkLogger.h"
#include "physics/mrPhysicsAttribData.h"

#include "physics/mrPhysics.h"
#include "physics/mrPhysicsScene.h"
#include "physics/mrPhysicsTasks.h"

#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "euphoria/erAttribData.h"
#include "euphoria/erBehaviour.h"
#include "euphoria/Nodes/erNodes.h"
#include "euphoria/erBody.h"

#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace MR;

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
static void updateControlParameters(NodeDef* nodeDef, Network* net)
{
  // Update the list of input control parameters specified within the AttribDataBehaviourParameters.
  for (MR::PinIndex i = 0 ; i < nodeDef->getNumInputCPConnections() ; ++i)
  {
    const CPConnection* inputCPConnection =  nodeDef->getInputCPConnection(i);
    if (inputCPConnection->m_sourceNodeID != INVALID_NODE_ID)
    {
      MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

      net->updateInputCPConnection(inputCPConnection, animSet);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPerformanceBehaviourUpdateTransformsPrePhysics(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  uint32_t taskIndex = 0;
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(taskIndex++, ATTRIB_SEMANTIC_RIG);
  AttribDataPhysicsRig* physicsRigAttrib = 
    parameters->getInputAttrib<AttribDataPhysicsRig>(taskIndex++, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataCharacterProperties* characterController = 
    parameters->getInputAttrib<AttribDataCharacterProperties>(taskIndex++, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  AttribDataBool* outputUpdated = 
    parameters->createOutputAttrib<AttribDataBool>(taskIndex++, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS);
  (void)outputUpdated; // not actually interested in it
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataPhysicsStateCreateDesc desc(numRigJoints);
  AttribDataPhysicsState* physicsState = 
    parameters->createOutputAttribReplace<AttribDataPhysicsState>(taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, &desc);
  NMP_ASSERT(physicsState->m_previousChildTransforms->m_transformBuffer->getLength() == numRigJoints);
  AttribDataPhysicalEffectData* performanceBehaviourData = 
    parameters->getInputAttrib<AttribDataPhysicalEffectData>(taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2);
  AttribDataPhysicsInitialisation* physicsInitialisation = 
    parameters->getInputAttrib<AttribDataPhysicsInitialisation>(taskIndex++, ATTRIB_SEMANTIC_PHYSICS_INITIALISATION);
  AttribDataBoolArray* outputMask = 
    parameters->getInputAttrib<AttribDataBoolArray>(taskIndex++, ATTRIB_SEMANTIC_OUTPUT_MASK);
  AttribDataAnimToPhysicsMap* animToPhysics = 
    parameters->getInputAttrib<AttribDataAnimToPhysicsMap>(taskIndex++, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);
  AttribDataCharacterControllerDef* characterControllerDef = 
    parameters->getInputAttrib<AttribDataCharacterControllerDef>(taskIndex++, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF);
  AttribDataCharacter* characterAttrib = parameters->getInputAttrib<AttribDataCharacter>(taskIndex++, ATTRIB_SEMANTIC_CHARACTER);
  Character* character = characterAttrib ? characterAttrib->m_character : 0;

  ++physicsState->m_updateCounter;

  PhysicsRig* physicsRig = physicsRigAttrib->m_physicsRig;
  if (!physicsRig)
  {
    return;
  }
  NMP_ASSERT(physicsRig->getPhysicsRigDef());

  if (!physicsState->m_haveAddedPhysicsRigRef)
  {
    // reference added in here
    PhysicsInitialisation pi;
    pi.m_transforms = physicsInitialisation->m_transforms->m_transformBuffer;
    pi.m_prevTransforms = physicsInitialisation->m_prevTransforms->m_transformBuffer;
    pi.m_worldRoot = physicsInitialisation->m_worldRoot;
    pi.m_prevWorldRoot = physicsInitialisation->m_prevWorldRoot;
    pi.m_deltaTime = physicsInitialisation->m_deltaTime;

    bool originallyReferenced = physicsRig->isReferenced();
    initialisePhysics(pi, physicsRig, physicsState, parameters->m_dispatcher);

    if (character && !originallyReferenced)
    {
      character->getBody().postPhysicsStep(physicsInitialisation->m_deltaTime);
    }
  }

  if (physicsState->m_needToInitialiseRoot)
  {
    physicsState->m_needToInitialiseRoot = false;
    NMP::Matrix34 originalRoot = characterController->m_worldRootTransform;
    // Re-initialise the blend between the current and the physics roots transforms
    (void) physicsRig->getRoot(&originalRoot, 0.0f, characterControllerDef->m_characterControllerDef);
  }

  for (uint32_t i = 0; i < physicsRig->getNumParts(); ++i)
  {
    // Enable dynamics on all parts enabled by the partial body/output mask
    if (outputMask->m_values[animToPhysics->m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(i)] == true)
    {
      physicsRig->getPart(i)->makeKinematic(false, 1.0f, false);
    }
  }

  // Now call the performance functions
  updateControlParameters(performanceBehaviourData->m_nodeDef, performanceBehaviourData->m_network);
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------