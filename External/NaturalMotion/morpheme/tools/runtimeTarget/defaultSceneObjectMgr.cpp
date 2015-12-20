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
#include "defaultSceneObjectMgr.h"

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "comms/attributeHelpers.h"
#include "comms/mcomms.h"
#include "comms/packet.h"

#include "defaultPhysicsMgr.h"
#include "networkInstanceRecordManager.h"
#include "runtimeTargetContext.h"
#include "sceneObjectRecordManager.h"

#include "runtimeTargetLogger.h"
//----------------------------------------------------------------------------------------------------------------------

#define PRINT_SCENEOBJECT_INFO

//----------------------------------------------------------------------------------------------------------------------
DefaultSceneObjectMgr::DefaultSceneObjectMgr(RuntimeTargetContext* context, DefaultPhysicsMgr* physicsManager) :
  m_sceneObjectManager(context->getSceneObjectManager()),
  m_networkInstanceManager(context->getNetworkInstanceManager()),
  m_physicsManager(physicsManager)
{
  NMP_ASSERT(context && physicsManager);
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObjectID DefaultSceneObjectMgr::getUnusedSceneObjectID()
{
  static uint32_t nextObjectID = 0;
  return nextObjectID++;
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObject* DefaultSceneObjectMgr::createSceneObject(
  uint32_t numAttributes,
  const MCOMMS::Attribute* const* attributes,
  MCOMMS::Connection* owner,
  MCOMMS::SceneObject *&createdCharacterController)
{
  createdCharacterController = NULL;

  MCOMMS::SceneObjectID objectID = getUnusedSceneObjectID();
  NMP_ASSERT(objectID != 0xFFFFFFFF);

  // Create a new SceneObject
  SceneObjectRecord* const sceneObjectRecord = SceneObjectRecord::create(objectID, numAttributes, attributes, owner);
  MCOMMS::SceneObject* const sceneObject = sceneObjectRecord->getObject();

#if defined(PRINT_SCENEOBJECT_INFO)
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "------------------\n");
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "Adding scene object with ID %i\n", objectID);
  for (uint32_t i = 0; i < numAttributes; ++i)
  {
    const char* const semanticStr = MCOMMS::Attribute::getSemanticDesc(attributes[i]->getSemantic());
    if (semanticStr != 0)
    {
      NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "      with %s\n", semanticStr);
    }
  }
#endif

  // Add to record list
  m_sceneObjectManager->addSceneObjectRecord(sceneObjectRecord);

  // CharacterController
  // If the scene object just added was a character
  // and we have a CharacterControllerDef asset available
  // add a second scene object to visualize the character position and volume
  MCOMMS::Attribute* instanceIDAttribute = sceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_INSTANCEID);
  if (instanceIDAttribute)
  {
    DefaultControllerMgr* const controllerManager = static_cast<DefaultControllerMgr*>(m_physicsManager->getControllerManager());

    // Update the object right away, so that the character is displayed correctly at the first frame.
    sceneObjectRecord->update(0, m_networkInstanceManager, controllerManager);

    // Get the CharacterController for this network instance.
#ifdef NMP_ENABLE_ASSERTS
    MCOMMS::InstanceID instanceID = *(MCOMMS::InstanceID*)instanceIDAttribute->getData();
    ControllerRecord* ccmRecord = 
      controllerManager->getControllerRecord(instanceID);
#endif
    NMP_ASSERT(ccmRecord);
    // There cannot (well, should not) be a controller scene object at this point.
    // NMP_ASSERT(!ccmRecord->m_connectRepresentation)
    createdCharacterController = controllerManager->addNewControllerRepresentation(objectID, owner);
  }

  // Create the physical representation
  const MCOMMS::Attribute* physObjAttr = 0;
  for (uint32_t i = 0; i < numAttributes; ++i)
  {
    if (attributes[i]->getSemantic() == MCOMMS::Attribute::SEMANTIC_PHYSICS_SHAPE_TYPE)
    {
      physObjAttr = attributes[i];
      break;
    }
  }

  if (physObjAttr)
  {
    bool isDynamic = false, isConstrained = false;
    bool hasVertices = false, hasIndices = false;
    MCOMMS::Attribute::VerticesWindingType windingType = MCOMMS::Attribute::VERTICES_WINDING_UNDEFINED;

    NMP::Vector3* points = 0;
    size_t numPoints = 0;
    int* indices = 0;
    size_t numIndices = 0;
    float density = 1, length = 1, depth = 1, height = 1, radius = 1, skinWidth = 0.0f;
    float staticFriction = 0.1f, dynamicFriction = 0.1f, restitution = 0.1f;
    float constraintStiffness = 1, constraintDamping = 1;
    float sleepThreshold = 0.00005f;
    float maxAngularVelocity = 7.0f;;
    float angularDamping = 0.05f;
    float linearDamping = 0.0f;
    uint32_t positionSolverIterationCount = 4;
    uint32_t velocitySolverIterationCount = 1;
    NMP::Matrix34 transform, constraintGlobalTransform, constraintLocalTransform;
    transform.identity();
    constraintGlobalTransform.identity();
    constraintLocalTransform.identity();

    MCOMMS::Attribute::PhysicsShapeType shapeType = *(MCOMMS::Attribute::PhysicsShapeType*)physObjAttr->getData();
    for (uint32_t i = 0; i < numAttributes; ++i)
    {
      MCOMMS::Attribute* attr = const_cast<MCOMMS::Attribute*>(attributes[i]);
      switch (attr->getSemantic())
      {
      case MCOMMS::Attribute::SEMANTIC_TRANSFORM:
        transform = *(NMP::Matrix34*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_NAME:
        // TODO: Can this be removed?
        break;
      case MCOMMS::Attribute::SEMANTIC_LENGTH:
        length = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_DEPTH:
        depth = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_HEIGHT:
        height = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_DENSITY:
        density = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_RADIUS:
        radius = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_STATIC_FRICTION:
        staticFriction = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_DYNAMIC_FRICTION:
        dynamicFriction = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_RESTITUTION:
        restitution = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_SKIN_WIDTH:
        skinWidth = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_MAX_ANGULAR_VELOCITY:
        maxAngularVelocity = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_LINEAR_DAMPING:
        linearDamping = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_ANGULAR_DAMPING:
        angularDamping = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_DYNAMIC_PHYSICS_OBJECT:
        isDynamic = (*(uint32_t*)attr->getData() == 0 ? false : true);
        break;
      case MCOMMS::Attribute::SEMANTIC_CONSTRAINT_GLOBAL_TRANSFORM:
        constraintGlobalTransform = *(NMP::Matrix34*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_CONSTRAINT_LOCAL_TRANSFORM:
        constraintLocalTransform = *(NMP::Matrix34*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_CONSTRAINT_STIFFNESS:
        constraintStiffness = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_CONSTRAINT_DAMPING:
        constraintDamping = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_POSITION_SOLVER_ITERATION_COUNT:
        positionSolverIterationCount = *(uint32_t*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_VELOCITY_SOLVER_ITERATION_COUNT:
        velocitySolverIterationCount = *(uint32_t*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_SLEEP_THRESHOLD:
        sleepThreshold = *(float*)attr->getData();
        break;
      case MCOMMS::Attribute::SEMANTIC_CONSTRAINED_PHYSICS_OBJECT:
        isConstrained = (*(uint32_t*)attr->getData() == 0 ? false : true);
        break;
      case MCOMMS::Attribute::SEMANTIC_VERTEX_POSITIONS:
        hasVertices = true; // TODO: redundant?
        points = (NMP::Vector3*)attr->getData();
        numPoints = attr->getDataCount();
        break;
      case MCOMMS::Attribute::SEMANTIC_VERTEX_INDICES:
        hasIndices = true; // TODO: redundant?
        indices = (int*)attr->getData();
        numIndices = attr->getDataCount();
        break;
      case MCOMMS::Attribute::SEMANTIC_VERTICES_WINDING:
        windingType = *(MCOMMS::Attribute::VerticesWindingType*)attr->getData();
        break;
      default:
        break;
      }
    }

    MR::PhysicsObjectID physicsObjectID = m_physicsManager->createNewPhysicsBody(
      dynamicFriction, staticFriction, restitution,
      isDynamic,
      shapeType, depth, height, length, radius, skinWidth,
      hasIndices, hasVertices,
      numPoints, numIndices,
      points, indices,
      windingType,
      transform,
      density,
      isConstrained,
      objectID,
      constraintGlobalTransform, constraintDamping, constraintStiffness, constraintLocalTransform,
      positionSolverIterationCount,
      velocitySolverIterationCount,
      sleepThreshold,
      maxAngularVelocity,
      linearDamping,
      angularDamping);

    MCOMMS::Attribute* physicsObjectIDAttr = sceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_PHYSICS_OBJECT_ID);
    if (physicsObjectIDAttr != 0)
    {
      MCOMMS::setAttributeData(physicsObjectIDAttr, physicsObjectID);
    }
  }

  return sceneObject;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultSceneObjectMgr::getNumSceneObjects() const
{
  return (uint32_t)m_sceneObjectManager->getSceneObjectMap().size();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t DefaultSceneObjectMgr::getSceneObjects(MCOMMS::SceneObject** objects, uint32_t maxObjects) const
{
  const SceneObjectMap& objectMap = m_sceneObjectManager->getSceneObjectMap();

  const SceneObjectMap::const_iterator end = objectMap.end();
  SceneObjectMap::const_iterator it = objectMap.begin();

  for (uint32_t i = 0; (it != end) && (i < maxObjects); ++i, ++it)
  {
    objects[i] = (*it).second->getObject();
  }

  return (uint32_t)objectMap.size();
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObject* DefaultSceneObjectMgr::getSceneObject(MCOMMS::SceneObjectID objectID) const
{
  return m_sceneObjectManager->findSceneObject(objectID);
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultSceneObjectMgr::destroySceneObject(
  MCOMMS::SceneObjectID  objectID,
  MCOMMS::SceneObjectID& destroyedCharacterController)
{
  // This feature is not used at the moment, as the character controller scene object is destroyed when the
  // network instance is.
  destroyedCharacterController = 0xFFFFFFFF;

  // Remove from the map
  SceneObjectRecord* objectRecord = m_sceneObjectManager->removeSceneObjectRecord(objectID);
#ifdef PRINT_SCENEOBJECT_INFO
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "Destroying scene object with ID %i \n", objectID);
#endif
  // Destroy object itself
  if (objectRecord == 0)
  {
    return false;
  }
  objectRecord->releaseAndDestroy();
  objectRecord = NULL;

  m_physicsManager->destroyPhysicsBody(objectID);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultSceneObjectMgr::onConnectionClosed(MCOMMS::Connection* connection)
{
  // Remove all scene object records associated with network instances owned by the closed connection.
  const NetworkInstanceMap& networkInstances = m_networkInstanceManager->getNetworkInstanceMap();
  const NetworkInstanceMap::const_iterator end = networkInstances.end();

  for (NetworkInstanceMap::const_iterator it = networkInstances.begin(); it != end; ++it)
  {
    if (it->second->getOwner() == connection)
    {
      const MCOMMS::InstanceID instanceID = it->second->getInstanceID();
      m_sceneObjectManager->destorySceneObjectRecords(instanceID);
    }
  }

  // Remove all scene object records associated owned by the closed connection.
  m_sceneObjectManager->destorySceneObjectRecords(connection, *m_physicsManager);
}
