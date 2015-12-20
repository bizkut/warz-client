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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "NMPlatform/NMVector3.h"
#include "comms/mcomms.h"
#include "comms/packet.h"
#include "../iPhysicsMgr.h"
#include "../iControllerMgr.h"
#include "defaultControllerMgr.h"
#include "comms/sceneObject.h"
#include "morpheme/mrNetwork.h"
#include "defaultPhysicsMgr.h"
#include "../defaultAssetMgr.h"
#include "../defaultDataManager.h"
#include "../defaultSceneObjectMgr.h"
#include "../networkDefRecordManager.h"
#include "../networkInstanceRecordManager.h"
#include "../runtimeTargetContext.h"
#include "../sceneObjectRecordManager.h"
#if defined(NM_HOST_CELL_PPU)
#include "NMPlatform/ps3/NMSPUManager.h"
#include "morpheme/mrDispatcherPS3.h"
#endif // NM_HOST_CELL_PPU
//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4127)
#endif
//----------------------------------------------------------------------------------------------------------------------
// ControllerRecord
//----------------------------------------------------------------------------------------------------------------------
ControllerRecord::ControllerRecord() :
  m_requestedMovement(NMP::Vector3(NMP::Vector3::InitZero)),
  m_characterPosition(NMP::Vector3(NMP::Vector3::InitZero)),
  m_characterPositionOld(NMP::Vector3(NMP::Vector3::InitZero)),
  m_characterOrientation(NMP::Quat(0.0f, 0.0f, 0.0f, 1.0f)),
  m_controllerEnabled(false),
  m_onGround (false),
  m_onGroundOld (false),
  m_groundContactTime(0.0f),
  m_controllerOffset(0.0f, 0.0f, 0.0f),
  m_connectRepresentation(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
ControllerRecord::~ControllerRecord()
{
}

//----------------------------------------------------------------------------------------------------------------------
// DefaultControllerMgr
//----------------------------------------------------------------------------------------------------------------------
DefaultControllerMgr::DefaultControllerMgr(DefaultPhysicsMgr* physicsManager, RuntimeTargetContext* context) :
  IControllerMgr(),
  m_physicsManager(physicsManager),
  m_context(context)
{
}

//----------------------------------------------------------------------------------------------------------------------
DefaultControllerMgr::~DefaultControllerMgr()
{
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::createControllerRecord(
  MCOMMS::InstanceID  instanceID,
  MR::Network*        const network,
  const NMP::Vector3& characterStartPosition,
  const NMP::Quat&    characterStartRotation)
{
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ControllerRecord), NMP_VECTOR_ALIGNMENT);
  ControllerRecord* ccRecord = new(alignedMemory) ControllerRecord();
  m_records[instanceID] = ccRecord;

  ccRecord->m_characterPosition = characterStartPosition;
  ccRecord->m_characterPositionOld = characterStartPosition;
  ccRecord->m_characterOrientation = characterStartRotation;
  ccRecord->m_characterOrientationOld = characterStartRotation;
  ccRecord->m_controllerEnabled = true;

  ccRecord->m_network = network;
  ccRecord->m_characterVerticalVelDt = 0.0f;
  ccRecord->m_deltaTranslation = NMP::Vector3::InitZero;
  ccRecord->m_deltaOrientation = NMP::Quat::kIdentity;
  ccRecord->m_characterVerticalVel = 0.0f;

  // Get the world up direction.
  NMP::Vector3 worldUpDirection = NMP::Vector3(0.0f, 1.0f, 0.0f);

  // Get the CharacterControllerDef from the Network
  MR::CharacterControllerDef* ccDef = network->getActiveCharacterControllerDef();

  // Set the current character controller values from the def data (i.e. no CC scaling is applied at the moment)
  ccRecord->m_controllerParams = *ccDef;
  ccRecord->m_controllerOffset = worldUpDirection *
    ((ccRecord->m_controllerParams.getHeight() * 0.5f) +
     ccRecord->m_controllerParams.getRadius() +
     ccRecord->m_controllerParams.getSkinWidth());

  ccRecord->m_network->setCharacterController(ccRecord);
  ccRecord->m_network->setCharacterPropertiesWorldRootTransform(
    NMP::Matrix34(ccRecord->m_characterOrientation, ccRecord->m_characterPosition));
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateControllerRecord(MCOMMS::InstanceID instanceID)
{
  ControllerRecord* ccmRecord = m_records[instanceID];

  // Get the CharacterControllerDef from the Network
  MR::CharacterControllerDef* ccDef = ccmRecord->m_network->getActiveCharacterControllerDef();

  // Set the current character controller values from the def data (i.e. no CC scaling is applied at the moment)
  ccmRecord->m_controllerParams = *ccDef;
  ccmRecord->m_network->setCharacterController(ccmRecord);

  // Make the controller recompute offsets etc, as this function is generally only called after an anim set (char controller) change.
  scaleControllerProperties(instanceID, 1.0f, 1.0f);
  
  ccmRecord->m_network->updateCharacterPropertiesWorldRootTransform(
    NMP::Matrix34(ccmRecord->m_characterOrientation, ccmRecord->m_characterPosition), true);
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultControllerMgr::getCollisionEnabled(MCOMMS::InstanceID NMP_UNUSED(instanceID)) const
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultControllerMgr::getOnGround(MCOMMS::InstanceID NMP_UNUSED(instanceID)) const
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::scaleControllerProperties(
  MCOMMS::InstanceID instanceID,
  float              radiusFraction,
  float              heightFraction)
{
  Records::iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    ControllerRecord* record = it->second;
    record->m_controllerParams = *record->m_network->getActiveCharacterControllerDef();

    NMP_ASSERT(m_physicsManager);
    // Get the world up direction.
    NMP::Vector3 worldUpDirection = NMP::Vector3(0.0f, 1.0f, 0.0f);

    // Note: we currently scale the width and depth (for box controllers) using the radius fraction.
    record->m_controllerParams.setRadius(record->m_network->getActiveCharacterControllerDef()->getRadius() * radiusFraction);
    record->m_controllerParams.setSkinWidth(record->m_network->getActiveCharacterControllerDef()->getSkinWidth() * radiusFraction);
    record->m_controllerParams.setHeight(record->m_network->getActiveCharacterControllerDef()->getHeight() * heightFraction);
    record->m_controllerParams.setWidth(record->m_network->getActiveCharacterControllerDef()->getWidth() * radiusFraction);
    record->m_controllerParams.setDepth(record->m_network->getActiveCharacterControllerDef()->getDepth() * radiusFraction);
    record->m_controllerParams.setStepHeight(record->m_network->getActiveCharacterControllerDef()->getStepHeight() * heightFraction);
    record->m_controllerOffset = worldUpDirection * (record->m_controllerParams.getHeight() * 0.5f +
                                 record->m_controllerParams.getRadius() + record->m_controllerParams.getSkinWidth());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::disableCollision(
  MCOMMS::InstanceID NMP_UNUSED(instanceID))
{
  // Nothing needed here
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::enableCollision(
  MCOMMS::InstanceID NMP_UNUSED(instanceID))
{
  // Nothing needed here
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::destroyControllerRecord(
  MCOMMS::InstanceID instanceID)
{
  Records::iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    ControllerRecord* ccRecord = it->second;

    if (ccRecord->m_connectRepresentation)
    {
      const MCOMMS::SceneObjectID sceneObjectID = ccRecord->m_connectRepresentation->getSceneObjectID();

      // The connect representation of the controller is owned by the connection and may therefore have been destroyed
      // as part of a connection close already.
      if (m_context->getSceneObjectManager()->findSceneObject(sceneObjectID))
      {
        SceneObjectRecord* const sceneObject = m_context->getSceneObjectManager()->removeSceneObjectRecord(sceneObjectID);
        sceneObject->releaseAndDestroy();
      }
    }
    NMP::Memory::memFree(ccRecord);
    m_records.erase(it);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::setRequestedMovement(
  MCOMMS::InstanceID  instanceID,
  const NMP::Vector3& delta)
{
  Records::iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    ControllerRecord* record = it->second;
    record->m_requestedMovement = delta;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::setPosition(
  MCOMMS::InstanceID  instanceID,
  const NMP::Vector3& pos)
{
  Records::iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    ControllerRecord* record = it->second;
    record->m_characterPosition = pos;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DefaultControllerMgr::getPosition(
  MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    const ControllerRecord* record = it->second;
    return record->m_characterPosition;
  }
  return NMP::Vector3::InitZero;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::setOrientation(
  MCOMMS::InstanceID instanceID,
  const NMP::Quat&   ori)
{
  // set CC position
  Records::iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    ControllerRecord* record = it->second;
    record->m_characterOrientation = ori;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::teleport(MCOMMS::InstanceID instanceID, const NMP::PosQuat& rootTransform)
{
  Records::iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    ControllerRecord* record = it->second;
    if (record->m_controllerEnabled)
    {
      record->m_characterOrientation = rootTransform.m_quat;
      record->m_characterPosition = rootTransform.m_pos;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat DefaultControllerMgr::getOrientation(
  MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    const ControllerRecord* record = it->second;
    return record->m_characterOrientation;
  }
  return NMP::Quat(0.0f, 0.0f, 0.0f, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DefaultControllerMgr::getPositionDelta(
  MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    const ControllerRecord* record = it->second;
    return record->m_deltaTranslation;
  }
  return NMP::Vector3::InitZero;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat DefaultControllerMgr::getOrientationDelta(
  MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_records.find(instanceID);
  if (it != m_records.end())
  {
    const ControllerRecord* record = it->second;
    return record->m_deltaOrientation;
  }
  return NMP::Quat(0.0f, 0.0f, 0.0f, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateControllers(float dt)
{
  for (Records::iterator it = m_records.begin() ; it != m_records.end() ; ++it)
  {
    ControllerRecord* ccmRecord = it->second;
    MCOMMS::InstanceID instanceID = it->first;
    ccmRecord->m_characterPositionOld = ccmRecord->m_characterPosition;

    if (ccmRecord->m_controllerEnabled)
    {
      enableCollision(instanceID);

      NMP::Vector3 movement(ccmRecord->m_requestedMovement);

      ccmRecord->m_characterPosition += movement;
      ccmRecord->m_onGroundOld = ccmRecord->m_onGround;

      if (ccmRecord->m_onGround && ccmRecord->m_onGroundOld)
        ccmRecord->m_groundContactTime += dt; // on ground
      else if (!ccmRecord->m_onGround && !ccmRecord->m_onGroundOld)
        ccmRecord->m_groundContactTime -= dt; // in air
      else
        ccmRecord->m_groundContactTime = 0.0f; // transition from/to ground to/from air
    }
    else
    {
      disableCollision(instanceID);

      ccmRecord->m_characterPosition += ccmRecord->m_requestedMovement;
      ccmRecord->m_onGroundOld = false;
      ccmRecord->m_onGround = false;
      ccmRecord->m_groundContactTime = 0.0f;
    }
  }

  for (Records::iterator it = m_records.begin() ; it != m_records.end() ; ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    ControllerRecord* ccmRecord = it->second;

    if (ccmRecord->m_controllerEnabled)
    {
      m_physicsManager->setRoot(instanceID, ccmRecord->m_characterPosition);
    }
    ccmRecord->m_requestedMovement.setToZero();
  }
}

//----------------------------------------------------------------------------------------------------------------------
ControllerRecord* DefaultControllerMgr::getControllerRecord(MCOMMS::InstanceID instanceID)
{
  return m_records[instanceID];
}

//----------------------------------------------------------------------------------------------------------------------
// Functions for controlling the Connect scene-object representing the character controller capsule
//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObject* DefaultControllerMgr::addNewControllerRepresentation(MCOMMS::SceneObjectID objectID, MCOMMS::Connection* owner)
{
  // CharacterControllerDef
  // IMPORTANT:
  // 1. There already exists at this point a scene object that describes the character itself; this
  //    is the scene object ID that was passed in.
  // 2. The job of this function is to add a SECOND scene object at the same position as the character
  //    with the parameters of the CCMRecord.
  // 3. The CCMRecord should already have been set up for the character before this function is called.
  MCOMMS::SceneObject* characterSceneObject = m_context->getSceneObjectManager()->findSceneObject(objectID);
  if (characterSceneObject)
  {
    MCOMMS::Attribute* instanceIDAttr = characterSceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_INSTANCEID);
    NMP_ASSERT(instanceIDAttr);
    MCOMMS::InstanceID* instanceID = static_cast<MCOMMS::InstanceID*>(instanceIDAttr->getData());
    MCOMMS::InstanceID networkInstanceID = *instanceID;

    ControllerRecord* ccmRecord = m_records[networkInstanceID];
    if (ccmRecord == NULL)
    {
      // A CCMRecord should have been set up for this controller, but it clearly
      // hasn't been yet.  Why not?
      NMP_ASSERT_FAIL();

      return NULL;
    }

    // if there is already a scene object for this character then don't create a new one.
    if (ccmRecord->m_connectRepresentation)
    {
      return ccmRecord->m_connectRepresentation;
    }

    NMP::Vector3 position = ccmRecord->m_characterPosition;
    MR::CharacterControllerDef::ShapeType shape = ccmRecord->m_network->getActiveCharacterControllerDef()->getShape();
    NMP::Colour colour = ccmRecord->m_network->getActiveCharacterControllerDef()->getColour();
    float height = ccmRecord->m_network->getActiveCharacterControllerDef()->getHeight();
    float width = ccmRecord->m_network->getActiveCharacterControllerDef()->getWidth();
    float depth = ccmRecord->m_network->getActiveCharacterControllerDef()->getDepth();
    float radius = ccmRecord->m_network->getActiveCharacterControllerDef()->getRadius();
    float skinWidth = ccmRecord->m_network->getActiveCharacterControllerDef()->getSkinWidth();

    // Initialise the attributes
    std::vector<MCOMMS::Attribute*> attributes;

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        "CharacterController",
        MCOMMS::Attribute::SEMANTIC_NAME));

    NMP::Matrix34 transform(NMP::Matrix34Identity());
    transform.translation() = position;
    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        transform,
        MCOMMS::Attribute::SEMANTIC_TRANSFORM,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        ccmRecord->m_controllerEnabled ? 1 : 0,
        MCOMMS::Attribute::SEMANTIC_ACTIVE_PHYSICS_OBJECT,
        true));

    switch (shape)
    {
    case MR::CharacterControllerDef::CapsuleShape:
    default:
      attributes.push_back(
        MCOMMS::Attribute::createAttribute(
          MCOMMS::Attribute::CONTROLLER_SHAPE_CAPSULE,
          MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
      break;
    case MR::CharacterControllerDef::CylinderShape:
      attributes.push_back(
        MCOMMS::Attribute::createAttribute(
          MCOMMS::Attribute::CONTROLLER_SHAPE_CYLINDER,
          MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
      break;
    case MR::CharacterControllerDef::BoxShape:
      attributes.push_back(
        MCOMMS::Attribute::createAttribute(
          MCOMMS::Attribute::CONTROLLER_SHAPE_BOX,
          MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
      break;
    case MR::CharacterControllerDef::SphereShape:
      attributes.push_back(
        MCOMMS::Attribute::createAttribute(
          MCOMMS::Attribute::CONTROLLER_SHAPE_SPHERE,
          MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
      break;
    }

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        height,
        MCOMMS::Attribute::SEMANTIC_HEIGHT,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        width,
        MCOMMS::Attribute::SEMANTIC_LENGTH,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        depth,
        MCOMMS::Attribute::SEMANTIC_DEPTH,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        radius,
        MCOMMS::Attribute::SEMANTIC_RADIUS,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        skinWidth,
        MCOMMS::Attribute::SEMANTIC_SKIN_WIDTH));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        colour,
        MCOMMS::Attribute::SEMANTIC_COLOUR));

    // Add object to the list of registered objects.
    DefaultSceneObjectMgr* soMgr = static_cast<DefaultSceneObjectMgr*>(MCOMMS::getRuntimeTarget()->getSceneObjectManager());
    NMP_ASSERT(soMgr);

    MCOMMS::SceneObjectID controllerID = soMgr->getUnusedSceneObjectID();
    SceneObjectRecord* controller =
      SceneObjectRecord::create(
        controllerID,
        (uint32_t)attributes.size(),
        &attributes[0],
        owner);

    for (std::vector<MCOMMS::Attribute*>::iterator it = attributes.begin(); it != attributes.end(); ++it)
    {
      NMP::Memory::memFree((void*)(*it)->getData());
      MCOMMS::Attribute::destroy((*it));
    }

    if (controller)
    {
      ccmRecord->m_connectRepresentation = controller->getObject();
      m_context->getSceneObjectManager()->addSceneObjectRecord(controller);
      return controller->getObject();
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateControllerRepresentations()
{
  // CharacterControllerDef
  // Run through the list of objects used to visualize the character position and volume
  // Update their dynamic attributes from the associated character to reflect the current state

  for (Records::iterator it = m_records.begin(); it != m_records.end(); ++it)
  {
    ControllerRecord* ccmRecord = it->second;
    MCOMMS::SceneObject* object = ccmRecord->m_connectRepresentation;
    if (object == NULL)
      continue;

    NMP::Vector3 position = ccmRecord->m_characterPosition;
    position += ccmRecord->m_controllerOffset;
    float radius = ccmRecord->m_controllerParams.getRadius();
    float height = ccmRecord->m_controllerParams.getHeight();
    float width = ccmRecord->m_controllerParams.getWidth();
    float depth = ccmRecord->m_controllerParams.getDepth();

    MCOMMS::Attribute* transformAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_TRANSFORM);
    if (transformAttr)
    {
      NMP::Matrix34* transform = (NMP::Matrix34*)(transformAttr->getData());
      if (transform)
      {
        NMP::Matrix34 matrix(NMP::Matrix34Identity());
        matrix.translation() = position;
        *transform = matrix;
      }
    }

    MCOMMS::Attribute* activeAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_ACTIVE_PHYSICS_OBJECT);
    if (activeAttr)
    {
      uint32_t* active = (uint32_t*)(activeAttr->getData());
      if (active)
      {
        *active = ccmRecord->m_controllerEnabled ? 1 : 0;
      }
    }

    MCOMMS::Attribute* heightAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_HEIGHT);
    if (heightAttr)
    {
      float* heightValue = (float*)(heightAttr->getData());
      if (heightValue)
      {
        *heightValue = height;
      }
    }

    MCOMMS::Attribute* depthAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_DEPTH);
    if (depthAttr)
    {
      float* depthValue = (float*)(depthAttr->getData());
      if (depthValue)
      {
        *depthValue = depth;
      }
    }

    MCOMMS::Attribute* lengthAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_LENGTH);
    if (lengthAttr)
    {
      float* lengthValue = (float*)(lengthAttr->getData());
      if (lengthValue)
      {
        *lengthValue = width;
      }
    }

    MCOMMS::Attribute* radiusAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_RADIUS);
    if (radiusAttr)
    {
      float* radiusValue = (float*)(radiusAttr->getData());
      if (radiusValue)
      {
        *radiusValue = radius;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstanceNetworksBegin(float dt)
{
  for (Records::iterator it = m_records.begin(); it != m_records.end(); ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    getControllerRecord(instanceID)->m_network->startUpdate(dt, false, true);
  }

  m_numCompleted = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::continueNetworkUpdate(MR::Network* network, MR::TaskID NMP_USED_FOR_ASSERTS(taskID))
{
  MR::Task* task = NULL;
  network->update(task);

  if (network->m_exeResult == MR::EXECUTE_RESULT_COMPLETE)
  {
    ++m_numCompleted;

    // reset the network
    network->getTempMemoryAllocator()->reset();
  }
  else if (network->m_exeResult == MR::EXECUTE_RESULT_BLOCKED)
  {
    NMP_ASSERT(task->m_taskid == taskID);
    ++m_numBlocked;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstanceNetworksContinue(MR::TaskID taskID)
{
  // Continue updating networks until all are completed.
  uint32_t numNetworks = (uint32_t)m_records.size();
  m_numBlocked = 0;

  while (m_numCompleted < numNetworks - m_numBlocked)
  {
    for (Records::iterator it = m_records.begin(); it != m_records.end(); ++it)
    {
      MR::Network* network = getControllerRecord(it->first)->m_network;

      switch (network->m_exeResult)
      {
      case MR::EXECUTE_RESULT_INVALID:
      case MR::EXECUTE_RESULT_BLOCKED:
      case MR::EXECUTE_RESULT_COMPLETE:
        break;

      case MR::EXECUTE_RESULT_STARTED:
        continueNetworkUpdate(network, taskID);
        break;

      case MR::EXECUTE_RESULT_IN_PROGRESS:
#if defined(NM_USE_SPU_DISPATCHER_ON_PS3)
        // We finish an update when the SPU has finished, otherwise spin waiting by moving on to another
        if (((MR::DispatcherPS3*)network->getDispatcher())->checkIfSPUHasFinished())
        {
          continueNetworkUpdate(network, taskID);
        }
#else
        continueNetworkUpdate(network, taskID);
#endif
        break;

      default:
        NMP_ASSERT(false);
      }
    }
  }

  if (m_numBlocked)
  {
    for (Records::iterator it = m_records.begin(); it != m_records.end(); ++it)
    {
      MR::Network* network = getControllerRecord(it->first)->m_network;

      if (network->m_exeResult == MR::EXECUTE_RESULT_BLOCKED)
      {
        network->m_exeResult = MR::EXECUTE_RESULT_IN_PROGRESS;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstanceNetworksEnd()
{
  for (Records::iterator it = m_records.begin(); it != m_records.end(); ++it)
  {
    MR::Network* network = getControllerRecord(it->first)->m_network;
    // finalize post network update
    network->endUpdate();

    // clean up the allocator
    NMP::TempMemoryAllocator* tempAllocator = network->getTempMemoryAllocator();
    tempAllocator->reset();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesConnectivity(
  float dt)
{
  for (Records::iterator it = m_records.begin() ; it != m_records.end() ; ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updateConnectivity(instanceID, dt);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesPreController(
  float delta)
{
  for (Records::iterator it = m_records.begin() ; it != m_records.end() ; ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updatePreController(instanceID, delta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesPrePhysics(
  float delta)
{
  for (Records::iterator it = m_records.begin() ; it != m_records.end() ; ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updatePrePhysics(instanceID, delta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesPostPhysics(
  float delta)
{
  for (Records::iterator it = m_records.begin() ; it != m_records.end() ; ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updatePostPhysics(instanceID, delta);
  }
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::sendInstanceProfileTimingDebugOutput()
{
  for (Records::iterator it = m_records.begin(); it != m_records.end(); ++it)
  {
    ControllerRecord* cr = it->second;
    MR::Network* network = cr->m_network;

    network->sendInstanceProfileTimingDebugOutput();
  }
}
#endif // MR_OUTPUT_DEBUGGING
