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
#include "euphoria/erCharacter.h"
#include "euphoria/erBody.h"
#include "euphoria/erLimb.h"
#include "physics/mrPhysics.h"
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorContactReporter.h"
#include "euphoria/erEuphoriaLogger.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
  //----------------------------------------------------------------------------------------------------------------------
  // Aux for creating the collision filter mask
  static MR::AttribDataCollisionFilterMask* createCollisionFilterMaskData(MR::NodeDef* nodeDef, MR::Network* net)
  {
    // AttribData creation
    //
    MR::NodeID nodeID = nodeDef->getNodeID();
    MR::AttribDataHandle handle = MR::AttribDataCollisionFilterMask::create(net->getPersistentMemoryAllocator());
    MR::AttribDataCollisionFilterMask* stateData = (MR::AttribDataCollisionFilterMask*)(handle.m_attribData);
    NMP_ASSERT(stateData); // failed to create
    MR::AttribAddress stateAddress(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeID, MR::INVALID_NODE_ID, MR::VALID_FOREVER);
    net->addAttribData(stateAddress, handle, MR::LIFESPAN_FOREVER);
    return stateData;
  }

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorContactReporterDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorContactReporterDef::locate(MR::AttribData* target)
{
  AttribDataOperatorContactReporterDef* result = (AttribDataOperatorContactReporterDef*)target;

  MR::AttribData::locate(target);
  NMP::endianSwap(result->m_minImpulseMagnitude);
  NMP::endianSwap(result->m_numCollisionGroups);

  NMP::endianSwap(result->m_collisionGroups);
  REFIX_PTR_RELATIVE(int32_t, result->m_collisionGroups, result);

  // Fixup each of the actual float values.
  for (int32_t i = 0; i < result->m_numCollisionGroups; ++i)
  {
    NMP::endianSwap(result->m_collisionGroups[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorContactReporterDef::dislocate(MR::AttribData* target)
{
  AttribDataOperatorContactReporterDef* result = (AttribDataOperatorContactReporterDef*)target;

  MR::AttribData::dislocate(target);
  NMP::endianSwap(result->m_minImpulseMagnitude);

  for (int32_t i = 0; i < result->m_numCollisionGroups; ++i)
  {
    NMP::endianSwap(result->m_collisionGroups[i]);
  }
  NMP::endianSwap(result->m_numCollisionGroups);
  UNFIX_PTR_RELATIVE(int32_t, result->m_collisionGroups, result);
  NMP::endianSwap(result->m_collisionGroups);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorContactReporterDef* AttribDataOperatorContactReporterDef::create(
  NMP::MemoryAllocator* allocator,
  float minImpulseMagnitude,
  int32_t numCollisionGroups,
  uint16_t refCount)
{
  NMP::Memory::Format memReqs = AttribDataOperatorContactReporterDef::getMemoryRequirements(numCollisionGroups);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);

  AttribDataOperatorContactReporterDef* result = AttribDataOperatorContactReporterDef::init(
    resource,
    minImpulseMagnitude,
    numCollisionGroups,
    refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorContactReporterDef::getMemoryRequirements(int32_t numCollisionGroups)
{
  // Make space for the wrapper
  NMP::Memory::Format result(sizeof(AttribDataOperatorContactReporterDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of int: collision groups.
  result += NMP::Memory::Format(sizeof(int32_t) * numCollisionGroups, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorContactReporterDef* AttribDataOperatorContactReporterDef::init(
  NMP::Memory::Resource& resource,
  float minImpulseMagnitude,
  int32_t numCollisionGroups,
  uint16_t refCount)
{
  // Header/wrapper
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataOperatorContactReporterDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorContactReporterDef* result = (AttribDataOperatorContactReporterDef*) resource.alignAndIncrement(memReqsHdr);
  result->setType(ATTRIB_TYPE_OPERATOR_CONTACTREPORTER_DEF);
  result->setRefCount(refCount);
  result->m_minImpulseMagnitude = minImpulseMagnitude;
  result->m_numCollisionGroups = numCollisionGroups;

  // Collision groups
  NMP::Memory::Format memReqsCollisionGroups(sizeof(int32_t) * numCollisionGroups, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_collisionGroups = (int32_t*) resource.alignAndIncrement(memReqsCollisionGroups);
  for (int32_t i = 0; i < numCollisionGroups; ++i)
  {
    result->m_collisionGroups[i] = -1;
  }

  // Ensure multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// void nodeOperatorContactReporterInitInstance(MR::NodeDef* node, MR::Network* net)
// Creates the output pins that we write to during update

static void createOutputs(MR::NodeDef* node, MR::Network* net)
{
  enum OUTPUT_PIN_ATTRIB_TYPE
  {
    BOOL,
    UINT,
    INT,
    FLOAT,
    V3
  };
  OUTPUT_PIN_ATTRIB_TYPE outputPinAttribTypesMap[] =
  {
    INT, //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_LIMBINDEX,
    INT, //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_PARTINDEX,
    INT, //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_RIGPARTINDEX,
    V3, //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_POINTLOCAL,
    V3, //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_NORMALLOCAL,
    V3, //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_IMPULSEWORLD,
  };
  for (int i = NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_LIMBINDEX; 
    i < NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_COUNT;
    ++i)
  {
    MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
    MR::OutputCPPin* outputCPPin = nodeBin->getOutputCPPin((MR::PinIndex)i);
    NMP_ASSERT(!outputCPPin->m_attribDataHandle.m_attribData); // about to create so should not already exist
    switch (outputPinAttribTypesMap[i])
    {
    case BOOL:
      outputCPPin->m_attribDataHandle = MR::AttribDataBool::create(net->getPersistentMemoryAllocator(), false);
      break;
    case UINT:
      outputCPPin->m_attribDataHandle = MR::AttribDataUInt::create(net->getPersistentMemoryAllocator(), static_cast<uint32_t>(0));
      break;
    case INT:
      outputCPPin->m_attribDataHandle = MR::AttribDataInt::create(net->getPersistentMemoryAllocator(), static_cast<int32_t>(0));
      break;
    case FLOAT:
      outputCPPin->m_attribDataHandle = MR::AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
      break;
    case V3:
      outputCPPin->m_attribDataHandle = 
        MR::AttribDataVector3::create(net->getPersistentMemoryAllocator(), NMP::Vector3Zero());
      break;
    }
    NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData); // failed to create
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// nodeOperatorContactReporterOutputCPUpdate
// Marks all pins as updated

static void flagOutputPins(MR::NodeDef* node, MR::Network* net)
{
  for (int i = 0; i < NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_COUNT; ++i)
  {
    MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
    MR::OutputCPPin* outputCPPin = nodeBin->getOutputCPPin((MR::PinIndex)i);
    NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData);
    outputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
  }
}

// The content of the contact report
struct ContactReport
{
  int32_t limbIndex;
  int32_t partIndex;
  int32_t rigPartIndex;
  int32_t pad;
  NMP::Vector3 impulseWorld;
  NMP::Vector3 pointLocal;
  NMP::Vector3 normalLocal;

  // Set "nothing-to-report" values
  void reset()
  {
    limbIndex = -1;
    partIndex = -1;
    rigPartIndex = -1;
    impulseWorld.setToZero();
    pointLocal.setToZero();
    normalLocal.setToZero();
  }
};

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// retrieveContactInfo
// Calls the physx filter shader callback to determine if the contact is of interest

static bool isCollisionOfInterest(
  MR::PhysicsScenePhysX3* physicsScene,
  physx::PxShape* shapeContacted,
  physx::PxFilterData filterDataForPart)
{
  // Filter attribs for the rig part (no need to query for these - it's a part on the character's physics rig)
  physx::PxFilterObjectAttributes filterAttribsForPart = physx::PxFilterObjectType::eARTICULATION;

  // Query fliter info on the shape we're contacting
  physx::PxFilterObjectAttributes filterAttribsContacted = 0;
  physx::PxFilterData             filterDataContacted;
  MR::getFilterInfo(shapeContacted, filterAttribsContacted, filterDataContacted);

  physx::PxPairFlags             pairFlags;
  const void*                     constantBlock = 0;
  physx::PxU32           constantBlockSize = 32;


  physx::PxScene* pxScene = ((MR::PhysicsScenePhysX3*)physicsScene)->getPhysXScene();

  physx::PxFilterFlags filterFlags = pxScene->getFilterShader()(
    
    // synthesised rig part filter info
    filterAttribsForPart,
    filterDataForPart,
    
    // queried i.e. actual real contacted shape filter info
    filterAttribsContacted,
    filterDataContacted,

    // other args required by the api
    pairFlags,
    constantBlock,
    constantBlockSize);

  bool ignoreCollisionPair = (filterFlags == physx::PxFilterFlag::eKILL || filterFlags == physx::PxFilterFlag::eSUPPRESS);
  return !ignoreCollisionPair;
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// nodeOperatorContactReporterOutputCPUpdate
// Retrieves any contact info of interest and caches it in the ContactReport

static void retrieveContactInfo(
  MR::Network* net,
  const AttribDataOperatorContactReporterDef* defData,
  ContactReport &contactReport, 
  const physx::PxFilterData& contactFilterMask)
{
  // Retrieve the physics rig def where the per-part contact data resides
  ER::Character* character = ER::networkGetCharacter(net);
  // Nothing to do if there is no euphoria character
  if (!character)
  {
    contactReport.reset();
    return;
  }

  // Extract basic node attrib data
  //
  NMP_ASSERT(defData);
  // Collision groups from the node attribs
  int32_t* collisionGroupIndices = defData->m_collisionGroups;
  int32_t numCollisionGroups = defData->m_numCollisionGroups;
  // Minimum magnitude of contact impulse to report on
  float minImpulseMagnitude = defData->m_minImpulseMagnitude;

  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(net);
  NMP_ASSERT(physicsRig);
  MR::PhysicsScene* physicsScene = NULL;
  MR::PhysicsRigDef* physicsRigDef = NULL;
  if (physicsRig)
  {
    physicsScene = physicsRig->getPhysicsScene();
    physicsRigDef = physicsRig->getPhysicsRigDef();
  }

  // Build an array of bool to denote parts of interest which are the parts
  // that are listed in the attrib data collision groups
  //
  NMP_ASSERT(physicsRigDef);
  const int32_t numParts = physicsRigDef->getNumParts();
  bool* includedPartIndices = (bool *)alloca(numParts * sizeof(bool));

  // Assume no parts listed then flag those that are found in any of the specified collision groups
  //
  memset((void*)includedPartIndices, 0, numParts * sizeof(bool));
  if (numCollisionGroups > 0 && physicsRigDef->m_numCollisionGroups > 0)
  {
    // For each collision group
    for (int32_t j = 0; j < numCollisionGroups; ++j)
    {
      // Require that specified collision group index is valid
      if (collisionGroupIndices[j] >= 0 && collisionGroupIndices[j] < physicsRigDef->m_numCollisionGroups)
      {
        MR::PhysicsRigDef::CollisionGroup* cg = &physicsRigDef->m_collisionGroups[collisionGroupIndices[j]];

        // Mark all part indices in the collision group as "included"
        for (int32_t k = 0; k < cg->numIndices; ++k)
        {
          includedPartIndices[cg->indices[k]] = true;
        }
      }
    }
  }

  // Check for contacts of interest
  //
  // (assuming none to be found)
  contactReport.reset();
  // Only impulses greater than the minimum and/or the last max found are considered
  float maxImpulseMag = minImpulseMagnitude;

  // For each limb
  const int32_t numLimbs = character->getBody().getNumLimbs();
  for (int32_t i = 0; i < numLimbs; ++i)
  {
    // For each part
    ER::Limb& limb = character->getBody().getLimb(i);
    // (starting with part 0 on the root limb or part 1 otherwise avoids double counting)
    int32_t jStart = limb.getIsRootLimb() ? 0:1;
    for (int32_t j = jStart; j < (int32_t)limb.getTotalNumParts(); ++j)
    {
      // Skip parts not included (via collision groups above)
      if (!includedPartIndices[limb.getPhysicsRigPartIndex(j)])
      {
        continue;
      }

      // Retrieve the contact data for the part
      ER::EuphoriaRigPartUserData* contactData = ER::EuphoriaRigPartUserData::getFromPart(limb.getPart(j));

      // For each contact listed see if there is one that occurred with a shape that we are interested in
      // if so break immediately and store the (summary) contact data
      uint16_t contactIndex = 0;
      for (contactIndex = 0; contactIndex < contactData->getNumContacts(); ++contactIndex)
      {
        physx::PxShape* shapeContacted = &(contactData->getContactedShape(contactIndex));
        MR::PhysXPerShapeData* perShapeData = MR::PhysXPerShapeData::getFromShape(shapeContacted);
        if (perShapeData && isCollisionOfInterest((MR::PhysicsScenePhysX3 *)physicsScene, shapeContacted, contactFilterMask))
        {
          break;
        }
      }
      // Contact of interest found in the above loop is a candidate for reporting, cache it if it is more significant 
      // than any other so far encountered.
      // Note 1: the following condition is true if and only if we break early from the above loop. If it is false
      // this means that getNumContacts() == 0 i.e. there is no current contact at all or there are current contacts on
      // the part but not with any shape we are interested in.
      // Note 2: the contact data available to us is only a "summary" of the contact info, eg. accumulated impulse avg
      // position/normal etc. this is why the above loop terminates as soon as one is detected.
      //
      if (contactIndex < contactData->getNumContacts())
      {
        // Store only if contact impulse magnitude is greater than the threshold or one we've already come across
        //
        float impulseMag = contactData->getLastTotalImpulseMagnitude();
        if (impulseMag > maxImpulseMag)
        {
          // update the max known
          maxImpulseMag = impulseMag;

          // Cache the corresponding data ready for output
          contactReport.limbIndex = i; 
          contactReport.partIndex = j;
          contactReport.rigPartIndex = limb.getPhysicsRigPartIndex(j);
          contactReport.impulseWorld = contactData->getLastTotalImpulse();
          NMP::Matrix34 partTM = limb.getPart(j)->getTransform();
          partTM.inverseTransformVector(contactData->getLastAveragePosition(), contactReport.pointLocal);
          partTM.inverseRotateVector(contactData->getLastAverageNormal(), contactReport.normalLocal);
        }
      } // ~For each contact..
      
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// nodeOperatorContactReporterOutputCPUpdate
// Updates output pins from from the data in the ContactReport

static void updateOutputs(MR::NodeDef* node, MR::Network* net, const ContactReport& contactReport)
{
  // Pointers to pins and data types we need
  MR::OutputCPPin* outputCPPin;
  MR::AttribDataInt* outputAttribDataInt;
  MR::AttribDataVector3* outputAttribDataVector3;

  // Indexes of the pins we are writing to
  //
  //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_LIMBINDEX = 0,
  //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_PARTINDEX,
  //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_RIGPARTINDEX,
  //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_POINTLOCAL,
  //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_NORMALLOCAL,
  //NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_IMPULSEWORLD,

  // Limb/Part index
  MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  outputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_LIMBINDEX);
  outputAttribDataInt = outputCPPin->getAttribData<MR::AttribDataInt>();
  outputAttribDataInt->m_value = contactReport.limbIndex;
  outputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_PARTINDEX);
  outputAttribDataInt = outputCPPin->getAttribData<MR::AttribDataInt>();
  outputAttribDataInt->m_value = contactReport.partIndex;
  outputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_RIGPARTINDEX);
  outputAttribDataInt = outputCPPin->getAttribData<MR::AttribDataInt>();
  outputAttribDataInt->m_value = contactReport.rigPartIndex;

  // Surface point/normal
  NMP::Vector3 nothingYet(0,0,0);
  outputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_POINTLOCAL);
  outputAttribDataVector3 = outputCPPin->getAttribData<MR::AttribDataVector3>();
  outputAttribDataVector3->m_value = contactReport.pointLocal;
  outputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_NORMALLOCAL);
  outputAttribDataVector3 = outputCPPin->getAttribData<MR::AttribDataVector3>();
  outputAttribDataVector3->m_value = contactReport.normalLocal;
  
  // Impulse
  outputCPPin = nodeBin->getOutputCPPin(NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_IMPULSEWORLD);
  outputAttribDataVector3 = outputCPPin->getAttribData<MR::AttribDataVector3>();
  outputAttribDataVector3->m_value = contactReport.impulseWorld;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* nodeOperatorContactReporterOutputCPUpdate(
  MR::NodeDef* nodeDef,
  MR::PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  MR::Network* net)
{
  NMP_ASSERT(outputCPPinIndex < NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_COUNT);

  // PhysX specific contact filter mask from the morpheme attrib data if we have it.
  //
  physx::PxFilterData contactFilterMask(0, 0, 0, 0);
  MR::CollisionFilterMask* cfmCurrent = MR::getCollisionFilterMask(nodeDef, net);
  NMP_ASSERT(cfmCurrent);
  if (cfmCurrent)
  {
    contactFilterMask.word0 = cfmCurrent->m_word0;
    contactFilterMask.word1 = cfmCurrent->m_word1;
    contactFilterMask.word2 = cfmCurrent->m_word2;
    contactFilterMask.word3 = cfmCurrent->m_word3;
  }

  // Perform the actual function of this node - to retrieve some contact data of interest
  AttribDataOperatorContactReporterDef* defData = nodeDef->getAttribData<AttribDataOperatorContactReporterDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  NMP_ASSERT(defData);
  ContactReport contactData;
  retrieveContactInfo(net, defData, contactData, contactFilterMask);

  // Update all the pins in one go..
  updateOutputs(nodeDef, net, contactData);
  //..and increment their frame counts to avoid further update requests this frame
  flagOutputPins(nodeDef, net);

  // Return the value of the requested output cp pin.
  MR::NodeBin* nodeBin = net->getNodeBin(nodeDef->getNodeID());
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}


//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorContactReporterInitInstance(MR::NodeDef* nodeDef, MR::Network* net)
{
  createCollisionFilterMaskData(nodeDef, net);
  createOutputs(nodeDef, net);
  if(nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(nodeDef, net);
  }
}


} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
