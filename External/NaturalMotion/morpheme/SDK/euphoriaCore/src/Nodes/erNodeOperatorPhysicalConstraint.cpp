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
#include "physics/mrPhysics.h"
#include "mrPhysicsRigPhysX3Articulation.h"
#include "mrPhysicsScenePhysX3.h"
#include "mrPhysX3.h"
#include "euphoria/erAttribData.h"
#include "euphoria/erBody.h"
#include "euphoria/erLimb.h"
#include "euphoria/Nodes/erNodeOperatorPhysicalConstraint.h"
#include "euphoria/erEuphoriaLogger.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorPhysicalConstraintDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorPhysicalConstraintDef::locate(MR::AttribData* target)
{
  AttribDataOperatorPhysicalConstraintDef* result = (AttribDataOperatorPhysicalConstraintDef*)target;
  (void) result;
  MR::AttribData::locate(target);

  NMP::endianSwap(result->m_constrainPositionX);
  NMP::endianSwap(result->m_constrainPositionY);
  NMP::endianSwap(result->m_constrainPositionZ);
  NMP::endianSwap(result->m_constrainOrientationTwist);
  NMP::endianSwap(result->m_constrainOrientationSwing1);
  NMP::endianSwap(result->m_constrainOrientationSwing2);
  NMP::endianSwap(result->m_partInertiaMultiplier);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorPhysicalConstraintDef::dislocate(MR::AttribData* target)
{
  AttribDataOperatorPhysicalConstraintDef* result = (AttribDataOperatorPhysicalConstraintDef*)target;
  (void) result;
  MR::AttribData::dislocate(target);

  NMP::endianSwap(result->m_constrainPositionX);
  NMP::endianSwap(result->m_constrainPositionY);
  NMP::endianSwap(result->m_constrainPositionZ);
  NMP::endianSwap(result->m_constrainOrientationTwist);
  NMP::endianSwap(result->m_constrainOrientationSwing1);
  NMP::endianSwap(result->m_constrainOrientationSwing2);
  NMP::endianSwap(result->m_partInertiaMultiplier);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorPhysicalConstraintDef* AttribDataOperatorPhysicalConstraintDef::create(
  NMP::MemoryAllocator* allocator,
  bool                  constrainPositionX,
  bool                  constrainPositionY,
  bool                  constrainPositionZ,
  bool                  constrainOrientationTwist,
  bool                  constrainOrientationSwing1,
  bool                  constrainOrientationSwing2,
  float                 partInertiaMultiplier,
  uint16_t              refCount)
{
  AttribDataOperatorPhysicalConstraintDef* result;

  NMP::Memory::Format memReqs = AttribDataOperatorPhysicalConstraintDef::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = AttribDataOperatorPhysicalConstraintDef::init(
    resource,
    constrainPositionX,
    constrainPositionY,
    constrainPositionZ,
    constrainOrientationTwist,
    constrainOrientationSwing1,
    constrainOrientationSwing2,
    partInertiaMultiplier,
    refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorPhysicalConstraintDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataOperatorPhysicalConstraintDef), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorPhysicalConstraintDef* AttribDataOperatorPhysicalConstraintDef::init(
  NMP::Memory::Resource& resource,
  bool                   constrainPositionX,
  bool                   constrainPositionY,
  bool                   constrainPositionZ,
  bool                   constrainOrientationTwist,
  bool                   constrainOrientationSwing1,
  bool                   constrainOrientationSwing2,
  float                  partInertiaMultiplier,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorPhysicalConstraintDef* result = (AttribDataOperatorPhysicalConstraintDef*)resource.ptr;
  resource.increment(sizeof(AttribDataOperatorPhysicalConstraintDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_DEF);
  result->setRefCount(refCount);

  result->m_constrainPositionX = constrainPositionX;
  result->m_constrainPositionY = constrainPositionY;
  result->m_constrainPositionZ = constrainPositionZ;
  result->m_constrainOrientationTwist = constrainOrientationTwist;
  result->m_constrainOrientationSwing1 = constrainOrientationSwing1;
  result->m_constrainOrientationSwing2 = constrainOrientationSwing2;
  result->m_partInertiaMultiplier = partInertiaMultiplier;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorPhysicalConstraintState functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorPhysicalConstraintState::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataOperatorPhysicalConstraintState), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorPhysicalConstraintState* AttribDataOperatorPhysicalConstraintState::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorPhysicalConstraintState* result = (AttribDataOperatorPhysicalConstraintState*)resource.ptr;
  resource.increment(sizeof(AttribDataOperatorPhysicalConstraintState));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_STATE);
  result->setRefCount(refCount);

  result->m_physxJoint = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataHandle AttribDataOperatorPhysicalConstraintState::create(NMP::MemoryAllocator* allocator)
{
  MR::AttribDataHandle result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, 0);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
static AttribDataOperatorPhysicalConstraintState* getStateData(
  const MR::NodeDef* nodeDef, 
  MR::Network*       net, 
  bool               createIfNeeded)
{
  MR::NodeID nodeID = nodeDef->getNodeID();
  MR::NodeBinEntry* stateEntry = net->getAttribDataNodeBinEntry(
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    nodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME);
  // if it exists, return it
  if (stateEntry)
  {
    return stateEntry->getAttribData<AttribDataOperatorPhysicalConstraintState>();
  }

  if (!createIfNeeded)
  {
    return 0;
  }

  // otherwise create it
  MR::AttribDataHandle handle = AttribDataOperatorPhysicalConstraintState::create(
    net->getPersistentMemoryAllocator());
  AttribDataOperatorPhysicalConstraintState* stateData = 
    (AttribDataOperatorPhysicalConstraintState*) (handle.m_attribData);
  NMP_ASSERT(stateData); // failed to create
  MR::AttribAddress stateAddress(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeID, MR::INVALID_NODE_ID, MR::VALID_FOREVER);
  net->addAttribData(stateAddress, handle, MR::LIFESPAN_FOREVER);

  return stateData;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* nodeOperatorPhysicalConstraintOutputCPUpdate(
  MR::NodeDef* nodeDef,
  MR::PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  MR::Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);

  MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

  // Create the single output attribute if it doesn't already exist.
  MR::NodeBin* nodeBin = net->getNodeBin(nodeDef->getNodeID());
  MR::OutputCPPin* performanceOutputCPPin = nodeBin->getOutputCPPin(outputCPPinIndex);
  if (!performanceOutputCPPin->m_attribDataHandle.m_attribData)
  {
    performanceOutputCPPin->m_attribDataHandle = MR::AttribDataInt::create(net->getPersistentMemoryAllocator(), 0);
  }

  AttribDataOperatorPhysicalConstraintState* stateData = getStateData(nodeDef, net, true); 

  // Catch the case where we're been reinitialised, in which case any existing constraint record is
  // bogus
  if (performanceOutputCPPin->m_lastUpdateFrame != net->getCurrentFrameNo() -1)
  {
    stateData->m_physxJoint = 0;
  }

  // If there's no physics rig then there's very little to do except release any joint we'd created
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(net);
  if (!physicsRig)
  {
    if (stateData->m_physxJoint)
    {
      stateData->m_physxJoint->release();
      stateData->m_physxJoint = 0;
      // We can't deregister it as there's no physics rig.
    }
    // Return the value of the requested output cp pin.
    return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
  }

  // The requested output pin will automatically be flagged as having been updated this frame, but
  // we need to make sure that all pins we have updated are correctly flagged.
  performanceOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();

  // Update input parameters.

  MR::AttribDataBool* inputActivateAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataBool>(nodeDef->getInputCPConnection(0), animSet);
  bool inputActivate = 
    inputActivateAttrib ? inputActivateAttrib->m_value : true;

  // Part data
  MR::AttribDataInt* inputPhysicsRigPartIndexAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataInt>(nodeDef->getInputCPConnection(1), animSet);
  int inputPhysicsRigPartIndex = inputPhysicsRigPartIndexAttrib ? inputPhysicsRigPartIndexAttrib->m_value : 0;

  MR::AttribDataVector3* inputPhysicsRigPartPositionAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataVector3>(nodeDef->getInputCPConnection(2), animSet);
  NMP::Vector3 inputPhysicsRigPartPosition = 
    inputPhysicsRigPartPositionAttrib ? inputPhysicsRigPartPositionAttrib->m_value : NMP::Vector3::InitZero;

  MR::AttribDataVector3* inputPhysicsRigPartOrientationAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataVector3>(nodeDef->getInputCPConnection(3), animSet);
  NMP::Vector3 inputPhysicsRigPartOrientation = 
    inputPhysicsRigPartOrientationAttrib ? inputPhysicsRigPartOrientationAttrib->m_value : NMP::Vector3::InitZero;

  // Object data
  MR::AttribDataPhysicsObjectPointer* inputPhysicsObjectIDAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataPhysicsObjectPointer>(nodeDef->getInputCPConnection(4), animSet);
  physx::PxRigidActor* inputPhysicsObject = 
    (physx::PxRigidActor*) (inputPhysicsObjectIDAttrib ? inputPhysicsObjectIDAttrib->m_value : 0);

  MR::AttribDataVector3* inputPhysicsObjectPositionAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataVector3>(nodeDef->getInputCPConnection(5), animSet);
  NMP::Vector3 inputPhysicsObjectPosition = 
    inputPhysicsObjectPositionAttrib ? inputPhysicsObjectPositionAttrib->m_value : NMP::Vector3::InitZero;

  MR::AttribDataVector3* inputPhysicsObjectOrientationAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataVector3>(nodeDef->getInputCPConnection(6), animSet);
  NMP::Vector3 inputPhysicsObjectOrientation = 
    inputPhysicsObjectOrientationAttrib ? inputPhysicsObjectOrientationAttrib->m_value : NMP::Vector3::InitZero;

  MR::AttribDataFloat* inputPhysicsObjectMassFractionAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataFloat>(nodeDef->getInputCPConnection(7), animSet);
  float inputPhysicsObjectMassFraction = 
    inputPhysicsObjectMassFractionAttrib ? inputPhysicsObjectMassFractionAttrib->m_value : 0.0f;

  // Get the definition data.
  AttribDataOperatorPhysicalConstraintDef* physicalConstraintDef =
    nodeDef->getAttribData<AttribDataOperatorPhysicalConstraintDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Do the work
  MR::PhysicsRigPhysX3* physicsRigPhysX3 = (MR::PhysicsRigPhysX3*) physicsRig;

  // Four possibilities - do nothing, destroy, create or update the joint. However, 
  // if the part index changes the joint needs to be disabled in order to update it.

  // Release joint if we're not supposed to be active
  if (
    inputPhysicsRigPartIndex >= (int) physicsRigPhysX3->getNumParts() ||
    inputPhysicsRigPartIndex < 0)
  {
    // Release if the part index is out of range
    inputActivate = false;
  }

  if (stateData->m_physxJoint)
  {
    bool releaseJoint = false;

    if (!inputActivate)
    {
      releaseJoint = true;
    }
    else if (stateData->m_physxJoint && inputPhysicsRigPartIndex != stateData->m_prevPartIndex)
    {
      // Release joint if the part index has changed
      releaseJoint = true;
    }
    else if (stateData->m_physxJoint)
    {
      // Release joint if the object ID has changed
      physx::PxRigidActor* objectActor = 0;
      physx::PxRigidActor* partActor = 0;
      stateData->m_physxJoint->getActors(objectActor, partActor);
      if (objectActor != inputPhysicsObject)
      {
        releaseJoint = true;
      }
    }

    if (releaseJoint)
    {
      // Actually release the joint and deregister it
      stateData->m_physxJoint->release();
      physicsRigPhysX3->deRegisterJointOnRig(stateData->m_physxJoint);
      stateData->m_physxJoint = 0;
    }
  }

  // Now (re)create the joint and/or update it
  if (inputActivate)
  {
    MR::PhysicsRigPhysX3::PartPhysX3* partPhysX3 = 
      physicsRigPhysX3->getPartPhysX3(inputPhysicsRigPartIndex);

    physx::PxRigidBody* inputPhysicsRigPartObject = partPhysX3->getRigidBody();

    physx::PxTransform physicsObjectFrame(
      MR::nmVector3ToPxVec3(inputPhysicsObjectPosition),
      MR::nmQuatToPxQuat(NMP::Quat(inputPhysicsObjectOrientation, false)));

    physx::PxTransform physicsRigPartFrame(
      MR::nmVector3ToPxVec3(inputPhysicsRigPartPosition),
      MR::nmQuatToPxQuat(NMP::Quat(inputPhysicsRigPartOrientation, false)));

    // Increase inertia to help stability
    MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* partPhysX3Articulation = 
      (MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation*) partPhysX3;

    NMP::Vector3 originalInertia = partPhysX3Articulation->getOriginalMassSpaceInertia();
    partPhysX3Articulation->setMassSpaceInertia(originalInertia * physicalConstraintDef->m_partInertiaMultiplier);

    // Inform the physics rig about the object it's holding
    if (inputPhysicsObjectMassFraction > 0.0f && 
        inputPhysicsObject->isRigidDynamic())
    {
      float mass = inputPhysicsObject->isRigidDynamic()->getMass();
      NMP::Vector3 COMPosition = MR::nmPxVec3ToVector3(inputPhysicsObject->isRigidDynamic()->getGlobalPose().p);
      partPhysX3->setExtraMass(mass * inputPhysicsObjectMassFraction, COMPosition);
    }

    if (!stateData->m_physxJoint)
    {
      // Create the joint
      stateData->m_physxJoint = PxD6JointCreate(
        PxGetPhysics(),
        inputPhysicsObject,
        physicsObjectFrame,
        inputPhysicsRigPartObject,
        physicsRigPartFrame);

      if (!physicalConstraintDef->m_constrainPositionX)
      {
        stateData->m_physxJoint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eFREE);
      }
      if (!physicalConstraintDef->m_constrainPositionY)
      {
        stateData->m_physxJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
      }
      if (!physicalConstraintDef->m_constrainPositionZ)
      {
        stateData->m_physxJoint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eFREE);
      }

      if (!physicalConstraintDef->m_constrainOrientationTwist)
      {
        stateData->m_physxJoint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
      }
      if (!physicalConstraintDef->m_constrainOrientationSwing1)
      {
        stateData->m_physxJoint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
      }
      if (!physicalConstraintDef->m_constrainOrientationSwing2)
      {
        stateData->m_physxJoint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eFREE);
      }

      stateData->m_prevPartIndex = inputPhysicsRigPartIndex;
    }
    else
    {
      // Update the joint properties
      stateData->m_physxJoint->setLocalPose(physx::PxJointActorIndex::eACTOR0, physicsObjectFrame);
      stateData->m_physxJoint->setLocalPose(physx::PxJointActorIndex::eACTOR1, physicsRigPartFrame);
    }
  }


  if (stateData->m_physxJoint)
  {
    // Register the joint in case we're deleted before we free it
    physicsRigPhysX3->registerJointOnRig(stateData->m_physxJoint);

    // set any relevant euphoria limbs as externally supported

    ER::Body* const body = ER::Body::getFromPhysicsRig(physicsRig);
    NMP_ASSERT(body);

    if (body)
    {
      // MORPH-21186 We could optimise this with a part-to-limb-index map.

      // Iterate over all limbs in euphoria body.
      const int32_t numbLimbs = body->getNumLimbs();

      for (int32_t limbIndex(0); limbIndex < numbLimbs; ++limbIndex)
      {
        ER::Limb& limb = body->getLimb(limbIndex);

        // don't overwrite externally supported flag if it has already been set (by game code for example).
        if (!limb.getEndIsExternallySupported())
        {
          bool foundMatchingPartInLimb = false;

          // Iterate over all parts in limb and determine if any of them are constrained by this node.
          const int32_t numLimbParts = limb.getTotalNumParts();

          for (int32_t limbPartIndex(0); (limbPartIndex < numLimbParts) && !foundMatchingPartInLimb; ++limbPartIndex)
          {
            foundMatchingPartInLimb |= (limb.getPhysicsRigPartIndex(limbPartIndex) == inputPhysicsRigPartIndex);
          }

          // Flag limb as externally supported if it is constrained by this node.
          limb.setIsEndExternallySupported(foundMatchingPartInLimb);
        }
      }
    }
  }

  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

//----------------------------------------------------------------------------------------------------------------------
// Note that the delete function doesn't get called - see MORPH-12428
void nodeOperatorPhysicalConstraintOutputDeleteInstance(
  const MR::NodeDef* nodeDef,
  MR::Network*       net)
{
  AttribDataOperatorPhysicalConstraintState* stateData = getStateData(nodeDef, net, false); 

  if (stateData && stateData->m_physxJoint)
  {
    stateData->m_physxJoint->release();
    stateData->m_physxJoint = 0;
  }

  nodeShareDeleteInstanceNoChildren(nodeDef, net);
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
