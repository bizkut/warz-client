// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/Euphoria/GameEuphoriaManager.h"

#if defined(NM_HOST_CELL_PPU)
  #include "NMPlatform/ps3/NMSPUManager.h"
#endif // defined(NM_HOST_CELL_PPU)
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

// Disable this define when not using Euphoria in order to save a small amount of memory when creating physics objects.
#define USING_EUPHORIA

/// Some default initialisation values for PhysX.
const float g_physXSkinWidth = 0.01f;
const float g_physXDefaultMaterialStaticFriction = 1.0f;
const float g_physXDefaultMaterialDynamicFriction = 1.0f;
const float g_physXDefaultMaterialRestitution = 0.0f;
const physx::PxCombineMode::Enum g_physXDefaultMaterialFrictionCombineMode    = physx::PxCombineMode::eMULTIPLY; ///< How to combine the friction of two contacting objects.
const physx::PxCombineMode::Enum g_physXDefaultMaterialRestitutionCombineMode = physx::PxCombineMode::eMULTIPLY; ///< How to combine the restitution of two impacting objects.
const float g_physXMaxSimulationTimeStep = 1.0f / 15.0f; ///< Maximum allowed interval for one PhysX time step.
const float g_minContactOffset = 0.0001f; ///< Sets a skin contact distance around objects that helps prevent jitter between objects that are in contact.

#ifdef NM_HOST_CELL_PPU
  // Maximum number of SPUs to use for the PhysX simulation spurs Taskset.
  uint32_t g_numSimulationTasksetSpus = 5;
 
  // Maximum number of SPUs to use for the PhysX query spurs Taskset.
  uint32_t g_numQueryTasksetSpus = 5;
#endif // NM_HOST_CELL_PPU

/// Specifies which type of events a PhysX3 actor will broadcast to clients.
// MORPH-20317: Upgrade the handling of behaviour bits.
const physx::PxU32 g_clientBehaviourBits = 
                                  physx::PxActorClientBehaviorBit::eREPORT_TO_FOREIGN_CLIENTS_CONTACT_NOTIFY | 
                                  physx::PxActorClientBehaviorBit::eREPORT_TO_FOREIGN_CLIENTS_SCENE_QUERY;

/// Which client to register with any PhysX3 objects we create.
physx::PxClientID g_defaultClientID = physx::PX_DEFAULT_CLIENT;

//----------------------------------------------------------------------------------------------------------------------
// EuphoriaManager functions.
//----------------------------------------------------------------------------------------------------------------------
void EuphoriaManager::init(
#ifdef NM_HOST_CELL_PPU
  CellSpurs2* spursObject,
#endif // NM_HOST_CELL_PPU
  float physicsToleranceScale)
{
  // Make sure we were not already initialised.
  NMP_ASSERT(!m_physXFoundation && !m_physXSDK && !m_physXScene && !m_physXCharacterControllerManager);

  // Define the scale at which simulation runs.
  physx::PxTolerancesScale scale;
  scale.length *= physicsToleranceScale;
  scale.mass   *= physicsToleranceScale * physicsToleranceScale * physicsToleranceScale;
  scale.speed  *= physicsToleranceScale;
  m_physicsToleranceScale = physicsToleranceScale;

  // Create an instance of the PhysX foundation class.
  m_physXFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_physXAllocator, m_physXErrorStream);
  NMP_ASSERT(m_physXFoundation);

  // Create a new PhysX profile zone manager.
  m_physXProfileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(m_physXFoundation);
  NMP_ASSERT(m_physXProfileZoneManager);

  // Creates an instance of the PhysX SDK.
  m_physXSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_physXFoundation, scale, true, m_physXProfileZoneManager);
  NMP_ASSERT_MSG(m_physXSDK, "Unable to create the NVIDIA PhysX SDK. The NVIDIA PhysX System software is required to run this app.");

  // Create an instance of the PhysX cooking interface.
  physx::PxCookingParams cookingParams;
  cookingParams.skinWidth = g_physXSkinWidth;
  m_physXCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_physXFoundation, cookingParams);
  NMP_ASSERT(m_physXCooking);

  // Initialize the PhysXExtensions library.
  PxInitExtensions(*m_physXSDK);

#ifdef NM_HOST_CELL_PPU
  // Initialise the PhysX SDK to use Spurs.
  physx::PxPS3Ext& physXps3APIextension = physx::PxGetPS3Extension();
  
  // Create a Cell Taskset instance for PhysX simulation tasks.
#ifdef NMP_ENABLE_ASSERTS
  uint32_t numSPUThreads = 0;
  spursObject->getNumSpuThread(&numSPUThreads);
  NMP_ASSERT(numSPUThreads >= g_numSimulationTasksetSpus);
#endif // NMP_ENABLE_ASSERTS
  uint8_t spuPriorities[8] = {2, 2, 2, 2, 2, 2, 2, 2};
  m_spursSimulationTaskset = physXps3APIextension.createTaskset(*spursObject, g_numSimulationTasksetSpus, spuPriorities);

  // Create a Cell Taskset instance for PhysX query tasks.
  NMP_ASSERT(numSPUThreads >= g_numQueryTasksetSpus);
  uint8_t queryPriorities[8] = {1, 1, 1, 1, 1, 1, 1, 1};
  m_spursQueryTaskset = physXps3APIextension.createTaskset(*spursObject, g_numQueryTasksetSpus, queryPriorities);
 
  // Initialize the PhysX SDK with two Spurs Tasksets: for simulation and scene query tasks.
  bool physXInitResult = physx::PxPS3Control::initializePhysXWithTaskset(
                                                      m_spursSimulationTaskset,
                                                      m_spursQueryTaskset,
                                                      g_numSimulationTasksetSpus,
                                                      g_numQueryTasksetSpus);
  NMP_ASSERT(physXInitResult);
#endif // NM_HOST_CELL_PPU

  // Create the PhysX character controller manager. This manages all characters from all scenes.
  m_physXCharacterControllerManager = PxCreateControllerManager(*m_physXFoundation);
  NMP_ASSERT(m_physXCharacterControllerManager);
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaManager::term()
{ 
  // Destroy the scene, if there is one.
  destroyScene();

  if (m_physXCharacterControllerManager)
  {
    m_physXCharacterControllerManager->release();
    m_physXCharacterControllerManager = NULL;
  }

#ifdef NM_HOST_CELL_PPU
  if (m_spursSimulationTaskset)
  {
    // Check if Tasksets are still in use; release all scenes to ensure they are not.
    NMP_ASSERT(!physx::PxPS3Control::isUsingTaskset());
    
    // Terminate SPU usage.
    physx::PxPS3Control::terminate();

    // Destroy the Tasksets.
    physx::PxPS3Ext& physXps3APIextension = physx::PxGetPS3Extension();
    physXps3APIextension.destroyTaskset(m_spursSimulationTaskset);

    m_spursSimulationTaskset = NULL;
    m_spursQueryTaskset = NULL;
  }
#endif // NM_HOST_CELL_PPU

  PxCloseExtensions();

  // Release the physX cooking
  if (m_physXCooking)
  {
    m_physXCooking->release();
    m_physXCooking = NULL;
  }

  // Destroy our PhysX SDK instance.
  if (m_physXSDK)
  {
    m_physXSDK->release();
    m_physXSDK = NULL;
  }
  
  if (m_physXProfileZoneManager)
  {
    m_physXProfileZoneManager->release();
    m_physXProfileZoneManager = NULL;
  }
  
  if (m_physXFoundation)
  {
    m_physXFoundation->release();
    m_physXFoundation = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool EuphoriaManager::connectToPhysXVisualDebugger()
{
  NMP_ASSERT(m_physXSDK);

  // These calls will return 0 on platforms that don't support the debugger
  if (m_physXSDK->getPvdConnectionManager() != 0)
  {
    if (m_physXVisualDebuggerFilename[0] == '\0')
    {
      m_physXSDK->getPvdConnectionManager()->disconnect();
      // The normal way to connect to PVD. PVD needs to be running at the time this function is called.
      // We don't worry about the return value because we are already registered as a listener for connections
      // and thus our onPvdConnected call will take care of setting up our basic connection state.
      physx::PxVisualDebuggerExt::createConnection(
                                    m_physXSDK->getPvdConnectionManager(), 
                                    "127.0.0.1",
                                    5425,
                                    100, 
                                    physx::PxVisualDebuggerConnectionFlags(
                                        physx::PxVisualDebuggerConnectionFlag::Debug |
                                        physx::PxVisualDebuggerConnectionFlag::Memory |
                                        physx::PxVisualDebuggerConnectionFlag::Profile));
    }   
    else
    {
      // Create a PVD connection that writes data straight to the file system. This is
      // the fastest connection on windows for various reasons.  First, the transport is quite fast as
      // PVD writes data in blocks and file systems work well with that abstraction.
      // Second, you don't have the PVD application parsing data and using CPU and memory bandwidth
      // while your application is running.
      physx::debugger::comm::PvdConnectionManager* mgr = m_physXSDK->getPvdConnectionManager();
      mgr->connect(m_physXAllocator, m_physXVisualDebuggerFilename);
    }

    if (m_physXSDK->getVisualDebugger())
    {
      m_physXSDK->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlags::eTRANSMIT_CONTACTS, true);
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaManager::setPhysXVisualDebuggerFileName(const char* filename)
{
  if (filename)
  {
    NMP_STRNCPY_S(m_physXVisualDebuggerFilename, sm_maxFilenameLength, filename);
  }
  else
  {
    m_physXVisualDebuggerFilename[0] = '\0';
  }
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxScene* EuphoriaManager::createScene(const NMP::Vector3* gravity)
{
  // We must have initialised the PhysX SDK and we can only have one scene per instance of EuphoriaManager.
  NMP_ASSERT(m_physXFoundation && m_physXSDK && m_physXCharacterControllerManager && !m_physXScene);

  //----------------------------------
  // Initialize the scene description.
  physx::PxSceneDesc sceneDesc(m_physXSDK->getTolerancesScale());

  // If no gravity direction is provided assume its acting in the negative Y direction.
  sceneDesc.gravity        = physx::PxVec3(0.0f, -9.81f, 0.0f);
  if (gravity)
    sceneDesc.gravity      = MR::nmVector3ToPxVec3(*gravity);

  // Create a CPU task dispatcher for the scene, this controls processing of the simulation over multiple threads.
  //  Pass in the number of worker threads to use.
  m_physXCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(0);
  sceneDesc.cpuDispatcher = m_physXCpuDispatcher; 

#ifdef NM_HOST_CELL_PPU
  // Initialise an SPU task dispatcher for use with this scene.
  NMP_ASSERT(m_spursSimulationTaskset && m_spursQueryTaskset);
  m_physXSpuDispatcher = PxDefaultSpuDispatcherCreate(m_spursSimulationTaskset, m_spursQueryTaskset);
  sceneDesc.spuDispatcher = m_physXSpuDispatcher; 
#endif // NM_HOST_CELL_PPU

  // It is required to register either the morpheme filter shader, or one that is very similar
  // (perhaps one that has additional functionality).
  sceneDesc.filterShader = MR::morphemePhysX3FilterShader;

  // This is optional, in order to improve character self-collision by reducing the suddenness of penetration resolution.
  // It has a slight performance cost. Initialised here but reset every frame.
  m_contactModifyCallback.setMaxSeparationSpeed(1.0f); // Maximum speed at which objects will be separated (e.g. 1 m/s).
  m_contactModifyCallback.setTimeStep(1.0f / 30.0f);   // Simulation time step. 
  sceneDesc.contactModifyCallback = &m_contactModifyCallback; 
  
  //----------------------------------
  // Create the scene.
  m_physXScene = PxGetPhysics().createScene(sceneDesc);
  NMP_ASSERT(m_physXScene);
  m_physXScene->setClientBehaviorBits(0, 15); // Setting all the basic bits for now, see MORPH-11272

  return m_physXScene;
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaManager::destroyScene()
{
  NMP_ASSERT(m_physXFoundation && m_physXSDK);

  // Make sure we have tidied up scene contents properly before releasing it.
  NMP_ASSERT(!m_physXScene || m_physXScene->getNbActors(physx::PxActorTypeSelectionFlag::eRIGID_STATIC | physx::PxActorTypeSelectionFlag::eRIGID_DYNAMIC) == 0);
  NMP_ASSERT(!m_physXScene || m_physXScene->getNbAggregates() == 0);
  NMP_ASSERT(!m_physXScene || m_physXScene->getNbArticulations() == 0);
  NMP_ASSERT(!m_physXScene || m_physXScene->getNbConstraints() == 0);

#ifdef NM_HOST_CELL_PPU
  // Destroy the SPU task dispatcher we are using with this scene.
  if (m_physXSpuDispatcher)
  {
    m_physXSpuDispatcher->release();
    m_physXSpuDispatcher = NULL;
  }
#endif // NM_HOST_CELL_PPU

  // Destroy the CPU task dispatcher we are using with this scene.
  if (m_physXCpuDispatcher)
  {
    m_physXCpuDispatcher->release();
    m_physXCpuDispatcher = NULL;
  }

  // Destroy all the internally managed materials.
  while (!m_materials.empty())
  {
    m_materials.back()->release();
    m_materials.pop_back();
  }

  // Destroy the PhysX scene.
  if (m_physXScene)
  {
    m_physXScene->release();
    m_physXScene = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxRigidActor* EuphoriaManager::createPhysicsObject(
  physx::PxGeometryType::Enum geomType,
  physx::PxGeometry*          geometry,
  bool                        dynamic,
  const NMP::Vector3&         position,
  const NMP::Quat&            rotation,
  float                       density,
  bool                        hasCollision,
  float                       staticFriction,
  float                       dynamicFriction,
  float                       skinWidth,
  float                       restitution,
  float                       sleepThreshold,
  float                       maxAngularVelocity,
  float                       linearDamping,
  float                       angularDamping)
{
  NMP_ASSERT(m_physXFoundation && m_physXSDK && m_physXScene);

  const physx::PxTolerancesScale& tolerancesScale = m_physXSDK->getTolerancesScale();

  physx::PxTransform globalPose(MR::nmVector3ToPxVec3(position), MR::nmQuatToPxQuat(rotation));

  //-----------------------
  // create the actor
  physx::PxRigidActor *actor = 0;
  if (dynamic)
  {
    // Dynamic object.
    physx::PxRigidDynamic* dynamicActor = m_physXSDK->createRigidDynamic(globalPose);

    dynamicActor->setClientBehaviorBits(g_clientBehaviourBits);
    dynamicActor->setOwnerClient(g_defaultClientID);

    dynamicActor->setLinearDamping(linearDamping);
    dynamicActor->setAngularDamping(angularDamping);
    dynamicActor->setMaxAngularVelocity(maxAngularVelocity);
    dynamicActor->setSleepThreshold(sleepThreshold * tolerancesScale.speed * tolerancesScale.speed);

    actor = dynamicActor;
  }
  else
  {
    // Static object.
    physx::PxRigidStatic* staticActor = m_physXSDK->createRigidStatic(globalPose);

    staticActor->setClientBehaviorBits(g_clientBehaviourBits);
    staticActor->setOwnerClient(g_defaultClientID);

    actor = staticActor;
  }
  NMP_ASSERT(actor);

  //-----------------------
  // adjust local pose for capsules so they are oriented correctly
  physx::PxTransform shapeLocalPose = physx::PxTransform::createIdentity();
  if (geomType == physx::PxGeometryType::eCAPSULE)
  {
    shapeLocalPose.q = physx::PxQuat(NM_PI_OVER_TWO, physx::PxVec3(0.0f, 1.0f, 0.0f));
  }

  physx::PxMaterial* material = m_physXSDK->createMaterial(staticFriction, dynamicFriction, restitution);
  material->setFrictionCombineMode(physx::PxCombineMode::eMULTIPLY);
  material->setRestitutionCombineMode(physx::PxCombineMode::eMULTIPLY);
  NMP_ASSERT(material);
  m_materials.push_back(material);

  //-----------------------
  // create the actor's shape
  physx::PxShape* shape = actor->createShape(*geometry, *material, shapeLocalPose);

  //-----------------------
  // For simplicity, make the contact offset of statics effectively zero, and the offset of dynamics > 0
  shape->setContactOffset(dynamic ? skinWidth : g_minContactOffset);
  shape->setRestOffset(0.0f);

  physx::PxU32 filterWord0 = (dynamic ? 1 << MR::GROUP_COLLIDABLE_PUSHABLE : 1 << MR::GROUP_COLLIDABLE_NON_PUSHABLE);

  //-----------------------
  // set up filter data
  shape->setQueryFilterData(physx::PxFilterData(filterWord0, 0, 0, 0));
  if (!hasCollision)
  {
    shape->setSimulationFilterData(physx::PxFilterData(0, 0xffffffff, 0, 0));
  }
  else
  {
    shape->setSimulationFilterData(physx::PxFilterData(filterWord0, 0, 0, 0));
  }

  if (dynamic)
  {
    NMP_ASSERT(density >= 0.0f);
    physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(actor), density);
  }

  // Add per shape data - needed for the object to be recognised by Euphoria.
#ifdef USING_EUPHORIA
  physx::PxShape *tempShapes[1];
  int32_t numShapes = actor->getShapes(&tempShapes[0], 1);
  for (int32_t i = 0; i < numShapes; i++)
  {
    MR::PhysXPerShapeData::create(tempShapes[i]);
  }
#endif // USING_EUPHORIA

  //-----------------------
  // Add the actor to the scene.
  m_physXScene->addActor(*actor);

  return actor;
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxRigidActor* EuphoriaManager::createPhysicsObjectStaticBox(
  const NMP::Vector3& dimensions,
  const NMP::Vector3& position,
  const NMP::Quat*    rotation)
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXFoundation && m_physXSDK && m_physXScene);

  // Create the description of our box.
  physx::PxBoxGeometry boxGeometryDescription(dimensions.x * 0.5f, dimensions.y* 0.5f, dimensions.z * 0.5f); // Half extents.
  
  // If a rotation has been provided use it otherwise the box will be world axis aligned.
  NMP::Quat rotationApplied(NMP::Quat::kIdentity);
  if (rotation)
    rotationApplied = *rotation;

  return createPhysicsObject(
            physx::PxGeometryType::eBOX,
            &boxGeometryDescription,
            false,              // This a static object.
            position,
            rotationApplied,
            0.0f,               // Density.
            true,               // hasCollision.
            1.0f,               // staticFriction
            1.0f,               // dynamicFriction
            g_minContactOffset, // skinWidth
            0.0f,               // restitution
            0.0f,               // sleepThreshold
            0.0f,               // maxAngularVelocity
            0.0f,               // linearDamping
            0.0f);              // angularDamping
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxRigidActor* EuphoriaManager::createPhysicsObjectGroundPlane(
  float               groundHeight,
  float               planeWidth,
  const NMP::Vector3* inputPlaneNormal)
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXFoundation && m_physXSDK && m_physXScene);

  // Provide dimensions for the ground plane assuming the Y axis as the world up axis.
  const float planeThickness = 2.0f;
  NMP::Vector3 dimensions(planeWidth, planeThickness, planeWidth);

  //----------------------------
  // Determine our plane normal.
  NMP::Vector3 planeNormal(NMP::Vector3::InitTypeOneY);
  if (inputPlaneNormal)
  {
    // Provided normal.
    planeNormal.normaliseDep(*inputPlaneNormal, NMP::Vector3::InitTypeOneY);
  }
  else
  {
    // Inverted gravity direction.
    physx::PxVec3 physXGravity = m_physXScene->getGravity();
    planeNormal.set(-physXGravity.x, -physXGravity.y, -physXGravity.z);
    planeNormal.normalise(NMP::Vector3::InitTypeOneY);
  }

  //----------------------------
  // Rotate the ground plane in order for its normal to be aligned with the true world up a direction.
  NMP::Vector3 yAxis(0.0f, 1.0f, 0.0f);
  NMP::Quat planeAttitude;
  planeAttitude.forRotation(yAxis, planeNormal);

  // Determine the position of the ground plane box.
  NMP::Vector3 position = planeNormal * (groundHeight - (planeThickness * 0.5f));

  //----------------------------
  // Create the box that will act as the ground plane.
  return createPhysicsObjectStaticBox(dimensions, position, &planeAttitude);
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaManager::destroyPhysicsObject(physx::PxRigidActor *actor)
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXSDK && m_physXScene && actor)

  // Remove the actor from the scene.
  m_physXScene->removeActor(*actor);

  // Release the actor.
  actor->release();
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaManager::simulate(float deltaTime)
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXSDK && m_physXScene);

  if (deltaTime < 0.000001f)
    return;

  if (deltaTime > g_physXMaxSimulationTimeStep)
    deltaTime = g_physXMaxSimulationTimeStep;

  // Update the maximum speed at which objects will be separated and the simulation time step
  //  for our contact modify callback.
  float maxSeparationSpeed = 0.5f * m_physicsToleranceScale;
  m_contactModifyCallback.setTimeStep(deltaTime);
  m_contactModifyCallback.setMaxSeparationSpeed(maxSeparationSpeed);
 
  m_physXScene->simulate(deltaTime);
  m_physXScene->fetchResults(true);
}

//----------------------------------------------------------------------------------------------------------------------
bool EuphoriaManager::validatePluginList(const NMP::OrderedStringTable& pluginList)
{
  if (pluginList.getNumEntries() == 0)
  {
    return true;
  }

  const char* physicsPlugin = "acPluginPhysX3_target_"NM_PLATFORM_FORMAT_STRING;
  const char* debugPhysicsPlugin = "acPluginPhysX3_target_"NM_PLATFORM_FORMAT_STRING"_debug";

  // The AnimModule has the core and the physics library registered. If the asset contains the physics plug-in then
  // it must be the first plug-in.
  const char* pluginListEntry = pluginList.getEntryString(0);
  if ((NMP_STRCMP(pluginListEntry, debugPhysicsPlugin) == 0 || NMP_STRCMP(pluginListEntry, physicsPlugin) == 0))
  {
    return true;
  }

  NMP_ASSERT_FAIL_MSG(0, "Physics plug-in must be first in the list");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// EuphoriaManager::ErrorStream functions.
//----------------------------------------------------------------------------------------------------------------------
void EuphoriaManager::ErrorStream::reportError(physx::PxErrorCode::Enum e, const char* message, const char* file, int line)
{
  NMP_ASSERT_FAIL_MSG("Error reported from PhysX");
  NMP_STDOUT("%s (%d) :", file, line);
  switch (e)
  {
    case physx::PxErrorCode::eINVALID_PARAMETER:
      NMP_STDOUT("invalid parameter");
      break;
    case physx::PxErrorCode::eINVALID_OPERATION:
      NMP_STDOUT("invalid operation");
      break;
    case physx::PxErrorCode::eOUT_OF_MEMORY:
      NMP_STDOUT("out of memory");
      break;
    case physx::PxErrorCode::eDEBUG_INFO:
      NMP_STDOUT("info");
      break;
    case physx::PxErrorCode::eDEBUG_WARNING:
      NMP_STDOUT("warning");
      break;
    default:
      printf("unknown ` error");
  }

  NMP_STDOUT(" : %s\n", message);
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaManager::ErrorStream::print(const char* message)
{
  NMP_STDOUT("%s", message);
}

//----------------------------------------------------------------------------------------------------------------------
// EuphoriaManager::PhysXAllocator functions.
//----------------------------------------------------------------------------------------------------------------------
#ifdef WIN32

  //----------------------------------------------------------------------------------------------------------------------
  // on win32 we only have 8-byte alignment guaranteed, but the CRT provides special aligned
  // allocation fns
  void* EuphoriaManager::PhysXAllocator::allocate(size_t size, const char*, const char*, int)
  {
    return _aligned_malloc(size, 16);
  }

  //----------------------------------------------------------------------------------------------------------------------
  void EuphoriaManager::PhysXAllocator::deallocate(void* ptr)
  {
    _aligned_free(ptr);
  }

#else // WIN32

  //----------------------------------------------------------------------------------------------------------------------
  // on PS3, XBox and Win64 we get 16-byte alignment by default
  void* EuphoriaManager::PhysXAllocator::allocate(size_t size, const char*, const char*, int)
  {
    void *ptr = ::malloc(size);
    PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15)==0);
    return ptr;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void EuphoriaManager::PhysXAllocator::deallocate(void* ptr)
  {
    ::free(ptr);
  }

#endif // WIN32

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
