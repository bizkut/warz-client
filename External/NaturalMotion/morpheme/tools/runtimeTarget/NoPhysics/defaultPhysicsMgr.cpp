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
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "comms/mcomms.h"
#include "comms/packet.h"
#include "../iPhysicsMgr.h"
#include "../iControllerMgr.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrNetwork.h"
#include "defaultPhysicsMgr.h"
#include "../defaultAssetMgr.h"
#include "../defaultSceneObjectMgr.h"
#include "defaultControllerMgr.h"

#ifdef NM_HOST_CELL_PPU
  #include "morpheme/mrDispatcherPS3.h"
#endif // NM_HOST_CELL_PPU

#include "../defaultDataManager.h"
#include "../networkInstanceRecordManager.h"
#include "../sceneObjectRecordManager.h"
#include "../runtimeTargetContext.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Default physics manager
//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsMgr::DefaultPhysicsMgr(
  RuntimeTargetContext* context,
  DefaultAssetMgr* assetMgr,
  const NMP::CommandLineProcessor& NMP_UNUSED(commandLineArguments)) :
  IPhysicsMgr(),
  m_assetMgr(assetMgr),
  m_context(context),
  m_frameIndex(0)
{
  // create CCM and SOM
  m_characterControllerManager = new DefaultControllerMgr(this, context);

  resetScene();

  assetMgr->setPhysicsManager(this);
}

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsMgr::~DefaultPhysicsMgr()
{
  m_assetMgr->setPhysicsManager(NULL);

  clearScene();

  delete m_characterControllerManager;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::getPhysicsEngineID(char* buffer, uint32_t bufferLength) const
{
  NMP_STRNCPY_S(buffer, bufferLength, "NoPhysics");
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::createNewPhysicsBody(
  float NMP_UNUSED(dynamicFriction),
  float NMP_UNUSED(staticFriction),
  float NMP_UNUSED(restitution),
  bool NMP_UNUSED(isDynamic),
  MCOMMS::Attribute::PhysicsShapeType NMP_UNUSED(shapeType),
  float NMP_UNUSED(depth),
  float NMP_UNUSED(height),
  float NMP_UNUSED(length),
  float NMP_UNUSED(radius),
  float NMP_UNUSED(skinWidth),
  bool NMP_UNUSED(hasIndices),
  bool NMP_UNUSED(hasVertices),
  size_t NMP_UNUSED(numPoints),
  size_t NMP_UNUSED(numIndices),
  NMP::Vector3* NMP_UNUSED(points),
  int32_t* NMP_UNUSED(indices),
  MCOMMS::Attribute::VerticesWindingType NMP_UNUSED(windingType),
  NMP::Matrix34& NMP_UNUSED(transform),
  float NMP_UNUSED(mass),
  bool NMP_UNUSED(isConstrained),
  MCOMMS::SceneObjectID NMP_UNUSED(objectID),
  NMP::Matrix34& NMP_UNUSED(constraintGlobalTransform),
  float NMP_UNUSED(constraintDamping),
  float NMP_UNUSED(constraintStiffness),
  NMP::Matrix34& NMP_UNUSED(constraintLocalTransform),
  uint32_t NMP_UNUSED(positionSolverIterationCount),
  uint32_t NMP_UNUSED(velocitySolverIterationCount),
  float NMP_UNUSED(sleepThreshold),
  float NMP_UNUSED(maxAngularVelocity),
  float NMP_UNUSED(linearDamping),
  float NMP_UNUSED(angularDamping))
{
  // Nothing needed here.
  return MR_INVALID_PHYSICS_OBJECT_ID;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::destroyPhysicsBody(MCOMMS::SceneObjectID NMP_UNUSED(objectID))
{
  // Nothing needed here.
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::resetScene()
{
  clearScene();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::initializeScene()
{
  // Nothing needed here.
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::clearScene()
{
  // Nothing needed here.
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateConnectivity(
  MCOMMS::InstanceID NMP_UNUSED(instanceID),
  float              NMP_UNUSED(dt))
{
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePreController(
  MCOMMS::InstanceID instanceID,
  float              delta)
{
  NET_LOG_ENTER_FUNC();
  ControllerRecord* ccmRecord = m_characterControllerManager->getControllerRecord(instanceID);

  NMP_ASSERT(m_characterControllerManager);

  if (ccmRecord->m_network->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    m_characterControllerManager->disableCollision(instanceID);
  }
  else
  {
    NET_LOG_MESSAGE(99, "NetworkInstanceRecord::updatePrePhysics: Updating trajectory\n");
    if (!m_characterControllerManager->getCollisionEnabled(instanceID))
    {
      m_characterControllerManager->enableCollision(instanceID);
    }

    // trajectories should be done now - get them and set in the controller
    ccmRecord->m_deltaTranslation = ccmRecord->m_network->getTranslationChange();
    ccmRecord->m_deltaOrientation = ccmRecord->m_network->getOrientationChange();

    NMP::Vector3 tempVec = ccmRecord->m_characterOrientation.rotateVector(ccmRecord->m_deltaTranslation);
    ccmRecord->m_characterOrientationOld = ccmRecord->m_characterOrientation;
    ccmRecord->m_characterOrientation *= ccmRecord->m_deltaOrientation;
    ccmRecord->m_characterOrientation.normalise();

    m_context->getNetworkInstanceManager()->findInstanceRecord(instanceID)->setRootTransform(
      NMP::Matrix34(ccmRecord->m_characterOrientation, ccmRecord->m_characterPosition));

    ccmRecord->m_characterVerticalVelDt = delta;

    // this should overwrite our root translation via setRootFromCharacterController when the manager
    // updates anyway
    m_characterControllerManager->setRequestedMovement(instanceID, tempVec);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePrePhysics(
  MCOMMS::InstanceID NMP_UNUSED(instanceID),
  float NMP_UNUSED(delta))
{
  NET_LOG_ENTER_FUNC();  
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePostPhysics(
  MCOMMS::InstanceID instanceID,
  float              delta)
{
  NET_LOG_ENTER_FUNC();
  ControllerRecord* ccmRecord = m_characterControllerManager->getControllerRecord(instanceID);

  if (ccmRecord->m_network->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    NET_LOG_MESSAGE(99, "NetworkInstanceRecord::updatePostPhysics: Updating trajectory");
    // Update the root translation and orientation.
    // NOTE: When using physics the root needs to be updated in updatePostPhysics so that the network
    // has the new root location, and can thus convert from world to local space. This required removing
    // setting from the root position from SceneObjectRecord::update (since that would be too late)
    ccmRecord->m_deltaOrientation = ccmRecord->m_network->getOrientationChange();
    ccmRecord->m_deltaTranslation = ccmRecord->m_network->getTranslationChange();
    NMP::Vector3 tempVec = ccmRecord->m_characterOrientation.rotateVector(ccmRecord->m_deltaTranslation);
    NMP::Vector3 newPos = ccmRecord->m_characterPosition + tempVec;

    // make the root follow the ragdoll
    m_characterControllerManager->setPosition(instanceID, newPos);
    ccmRecord->m_characterPosition = newPos;
    ccmRecord->m_characterOrientationOld = ccmRecord->m_characterOrientation;
    ccmRecord->m_characterOrientation *= ccmRecord->m_deltaOrientation;
    ccmRecord->m_characterOrientation.normalise();
    ccmRecord->m_network->updateCharacterPropertiesWorldRootTransform(
      NMP::Matrix34(ccmRecord->m_characterOrientation, ccmRecord->m_characterPosition), true);
    ccmRecord->m_characterVerticalVel = tempVec.y / delta;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::setRoot(
  MCOMMS::InstanceID  instanceID,
  const NMP::Vector3& pos)
{
  // TODO: convert to local space
  ControllerRecord* ccmRecord = m_characterControllerManager->getControllerRecord(instanceID);
  NMP::Vector3 actualTranslation = (ccmRecord->m_characterPosition - ccmRecord->m_characterPositionOld);
  if (ccmRecord->m_network->getRootControlMethod() != MR::Network::ROOT_CONTROL_PHYSICS)
  {
    MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
    NMP_ASSERT(environmentManager);

    // record the movement of the entity - but don't add on vertical velocity due to stepping.
    bool gravityEnabled = false;
    MCOMMS::Attribute* gravityEnabledAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);

    if (gravityEnabledAttr != 0)
    {
      gravityEnabled = (*(int*)gravityEnabledAttr->getData() != 0);
    }

    if (actualTranslation.y < ccmRecord->m_deltaTranslation.y && gravityEnabled)
      ccmRecord->m_characterVerticalVel =
        (actualTranslation.y - ccmRecord->m_deltaTranslation.y) / ccmRecord->m_characterVerticalVelDt;
    else
      ccmRecord->m_characterVerticalVel = 0.0f;

    // if stepping down then zero the velocity so that we run off cliff edges starting horizontal.
    if (m_characterControllerManager->getOnGround(instanceID))
      ccmRecord->m_characterVerticalVel = 0.0f;
  }

  ccmRecord->m_deltaTranslation = actualTranslation;
  NMP::Quat oldOrientInv(ccmRecord->m_characterOrientationOld);
  oldOrientInv.conjugate();
  ccmRecord->m_deltaTranslation = oldOrientInv.rotateVector(ccmRecord->m_deltaTranslation);

  // Set the networks root position to the adjusted vector
  ccmRecord->m_characterPosition = pos;

  // Set the character controller's position to the adjusted vector
  ccmRecord->m_network->updateCharacterPropertiesWorldRootTransform(
    NMP::Matrix34(ccmRecord->m_characterOrientation, ccmRecord->m_characterPosition), true);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsRecords
//----------------------------------------------------------------------------------------------------------------------
IControllerMgr* DefaultPhysicsMgr::getControllerManager()
{
  return m_characterControllerManager;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePhysicalSceneObjects()
{
  // Nothing needed here, as this is the NULL physics build.
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateEnvironment()
{
  // Nothing needed here, as this is the NULL physics build.
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateSceneObjects(float deltaTime)
{
  m_context->getSceneObjectManager()->updateSceneObjectRecords(
    deltaTime,
    m_context->getNetworkInstanceManager(),
    getControllerManager());
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateControllers(float NMP_UNUSED(delta))
{
  // Nothing needed here.
}

namespace
{
#ifdef NM_PROFILING
// -----------------------------------------------------------------------------
void recursiveTraverseRecords(NMP::Profiler* profiler, const NMP::Profiler::ProfilerRecord* record, size_t depth, void (*processRecord)(NMP::Profiler*, const NMP::Profiler::ProfilerRecord*, size_t))
{
  (*processRecord)(profiler, record, depth);
  const NMP::Profiler::ProfilerRecord* end = profiler->getRecordChildrenEnd(record);
  for (const NMP::Profiler::ProfilerRecord* child = profiler->getRecordChildrenBegin(record); child != end; child = profiler->getRecordNextSibling(child))
  {
    recursiveTraverseRecords(profiler, child, depth + 1, processRecord);
  }
}

// -----------------------------------------------------------------------------
void printRecord(NMP::Profiler* profiler, const NMP::Profiler::ProfilerRecord* record, size_t depth)
{
  for (uint32_t tab = 1; tab < depth; ++tab)
  {
    printf(" | ");
  }
  if (depth)
  {
    printf(" |_");
  }
  printf("%6.3f ms - %s\n", profiler->getRecordTime(record), profiler->getRecordBlock(record)->getTag());
}

// -----------------------------------------------------------------------------
void gatherProfilingData(NMP::Profiler* profiler)
{
  const size_t threadCount = profiler->getNumThreads();
  for (size_t threadIndex = 0; threadIndex != threadCount; ++threadIndex)
  {
    printf("=================================================\n");
    printf("=================================================\n");

    const NMP::Profiler::ProfilerRecord* rootRecord = profiler->getRootRecord(threadIndex);

    if (rootRecord)
    {
      recursiveTraverseRecords(profiler, rootRecord, 0, &printRecord);
    }
    printf("=================================================\n\n");
  }
}
#endif // NM_PROFILING
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::update(float delta)
{
  NM_BEGIN_PROFILING("------- Update Root -------");

  // Update all instances
  m_context->getNetworkInstanceManager()->applyAnimationSetChanges();

  PHYSICS_LOG_LINE_DIVIDE();

  m_characterControllerManager->updateInstanceNetworksBegin(delta);

  PHYSICS_LOG_MESSAGE("updateInstancesConnectivity");
  NM_BEGIN_PROFILING("updateInstancesConnectivity");
  m_characterControllerManager->updateInstancesConnectivity(delta);
  NM_END_PROFILING(); //"updateInstancesConnectivity"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);

  PHYSICS_LOG_MESSAGE("updateInstancesPreController");
  NM_BEGIN_PROFILING("updateInstancesPreController");
  m_characterControllerManager->updateInstancesPreController(delta);
  NM_END_PROFILING(); //"updateInstancesPreController"

  PHYSICS_LOG_MESSAGE("updating character controllers");
  NM_BEGIN_PROFILING("updateControllers");
  m_characterControllerManager->updateControllers(delta);
  NM_END_PROFILING(); //"updateControllers"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS);

  PHYSICS_LOG_MESSAGE("updateInstancesPrePhysics");
  NM_BEGIN_PROFILING("updateInstancesPrePhysics");
  m_characterControllerManager->updateInstancesPrePhysics(delta);
  NM_END_PROFILING(); //"updateInstancesPrePhysics"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

  PHYSICS_LOG_MESSAGE("updateInstancesPostPhysics");
  NM_BEGIN_PROFILING("updateInstancesPostPhysics");
  m_characterControllerManager->updateInstancesPostPhysics(delta);
  NM_END_PROFILING(); //"updateInstancesPostPhysics"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::TASK_ID_UNSPECIFIED);
  m_characterControllerManager->updateInstanceNetworksEnd();

  NM_END_PROFILING(); //"updateInstances"

#ifdef NM_PROFILING
  NMP::Profiler::getProfiler()->endProfilingFrame();
  gatherProfilingData(NMP::Profiler::getProfiler());
  NMP::Profiler::getProfiler()->reset();
#endif // NM_PROFILING

  PHYSICS_LOG_MESSAGE("updateSceneObjects");

  // Update all scene objects.
  updateSceneObjects(delta);

  // Update the Connect-side representations of the character controllers
  m_characterControllerManager->updateControllerRepresentations();

#ifdef MR_OUTPUT_DEBUGGING
  m_characterControllerManager->sendInstanceProfileTimingDebugOutput();
#endif // MR_OUTPUT_DEBUGGING

  ++m_frameIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::initializePhysicsCore(
  uint32_t NMP_UNUSED(numDispatchers),
  MR::Dispatcher** NMP_UNUSED(dispatchers))
{
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::finaliseInitPhysicsCore()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::createConstraint(uint64_t NMP_UNUSED(constraintGUID),
                                         MR::PhysicsObjectID NMP_UNUSED(physicsObjectID), 
                                         const NMP::Vector3 &NMP_UNUSED(constraintPosition), 
                                         bool NMP_UNUSED(lockOrientation),
                                         bool NMP_UNUSED(constrainAtCOM))
{
  // This configuration does not currently support constraints
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::destroyConstraint(uint64_t NMP_UNUSED(constraintGUID))
{
  // This configuration does not currently support constraints
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::moveConstraint(uint64_t NMP_UNUSED(constraintGUID), const NMP::Vector3 &NMP_UNUSED(newGrabPosition))
{
  // This configuration does not currently support constraints
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool  DefaultPhysicsMgr::applyForce(uint32_t NMP_UNUSED(physicsEngineObjectID), 
                        ForceMode NMP_UNUSED(mode),
                        const NMP::Vector3& NMP_UNUSED(force),
                        bool NMP_UNUSED(applyAtCOM),
                        const NMP::Vector3& NMP_UNUSED(localSpacePosition),
                        const NMP::Vector3& NMP_UNUSED(worldSpacePosition)
                        )
{
  // This configuration does not currently support constraints
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::setPhysicsObjectAttribute(uint32_t NMP_UNUSED(physicsEngineObjectID), const MCOMMS::Attribute* NMP_UNUSED(physicsObjAttribute))
{
  // This configuration does not currently support physics objects
  return false;
}
  
//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::setPhysicsEnvironmentAttribute(const MCOMMS::Attribute* NMP_UNUSED(physicsEnvAttribute))
{
  // This configuration does not currently support physics objects
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::validatePluginList(const NMP::OrderedStringTable& NMP_UNUSED(pluginList))
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void* DefaultPhysicsMgr::getPhysicsObjectPointerFromPhysicsID(MR::PhysicsObjectID NMP_UNUSED(id)) const
{
  // This configuration does not currently support physics objects
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::getPhysicsObjectIDFromPhysicsObjectPointer(
  void* NMP_UNUSED(physicsObject)) const
{
  // This configuration does not currently support physics objects
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------

