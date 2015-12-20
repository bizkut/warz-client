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
#include "defaultPhysicsMgr.h"
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"

#include "comms/attributeHelpers.h"
#include "comms/commsServer.h"
#include "comms/mcomms.h"
#include "comms/packet.h"
#include "comms/physicsCommsServerModule.h"

#include "../../iPhysicsMgr.h"
#include "../../iControllerMgr.h"

#include "mrPhysX2.h"
#include "physics/mrPhysicsRigDef.h"
#include "mrPhysicsRigPhysX2.h"
#include "mrPhysicsScenePhysX2.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsRigDef.h"
#include "../../defaultSceneObjectMgr.h"
#include "../../defaultAssetMgr.h"
#include "../../defaultDataManager.h"
#include "../../networkInstanceRecordManager.h"
#include "../../sceneObjectRecordManager.h"
#include "defaultControllerMgr.h"
#include "NMPlatform/NMvpu.h"
#include "physics/Nodes/mrPhysicsNodes.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrManager.h"
#include "physics/mrPhysics.h"

#include "morpheme/mrDispatcher.h"
#include "morpheme/mrNetworkLogger.h"

#include "mrPhysX2Includes.h"
#include "memoryStream.h"
#if defined(NM_HOST_CELL_PPU)
  #include "NMPlatform/ps3/NMSPUManager.h"
#endif // defined(NM_HOST_CELL_PPU)

#include "../../runtimeTargetLogger.h"

//----------------------------------------------------------------------------------------------------------------------
#define ANIM_GRAVITY
//----------------------------------------------------------------------------------------------------------------------
/// \class MyNxUserOutputStream
/// \brief Implementation of an NxUserOutputStream to allow us to catch and print out any messages or errors from PhysX.
/// \ingroup LiveLinkRuntimeModule
//----------------------------------------------------------------------------------------------------------------------
class MyNxUserOutputStream: public NxUserOutputStream
{
  virtual void reportError(NxErrorCode code, const char * message, const char *file, int line);

  virtual NxAssertResponse reportAssertViolation(const char * message, const char *file, int line);

  virtual void print(const char * message);
};

//----------------------------------------------------------------------------------------------------------------------
void MyNxUserOutputStream::reportError(NxErrorCode code, const char * message,
                                       const char *file, int line)
{
  NMP_MSG("PHYSX ERROR: %s, %i: %s (%i)\n", file, line, message, code);
  return;
}

//----------------------------------------------------------------------------------------------------------------------
NxAssertResponse MyNxUserOutputStream::reportAssertViolation(const char * message,
                                                             const char *file, int line)
{
  NMP_MSG("PHYSX ASSERT: %s, %i: %s\n", file, line, message);
  return NX_AR_CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------
void MyNxUserOutputStream::print(const char * message)
{
  NMP_MSG("PHYSX MSG: %s\n", message);
  return;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class PhysicsSDK
/// \brief A singleton class to keep track of the instance of the physics sdk object
/// \ingroup LiveLinkRuntimeModule
//----------------------------------------------------------------------------------------------------------------------
class PhysicsSDK
{
public:
  static bool init();
  static bool term();
  static PhysXSDK* SDKinstance();
  static bool connectToDebugger();
  static bool disconnectFromDebugger();

protected:

  PhysicsSDK() {}
  ~PhysicsSDK() {}

  static bool      sm_initCooking;
  static int32_t   sm_debuggerConnectionRefCount;
  static PhysXSDK* sm_physicsSDK;
  static MyNxUserOutputStream* m_errorStream;
};

//----------------------------------------------------------------------------------------------------------------------
// Statics
bool PhysicsSDK::sm_initCooking = false;
int32_t PhysicsSDK::sm_debuggerConnectionRefCount = 0;
PhysXSDK* PhysicsSDK::sm_physicsSDK = NULL;
MyNxUserOutputStream* PhysicsSDK::m_errorStream = NULL;

//----------------------------------------------------------------------------------------------------------------------
/// \class PhysicsUserData
/// \brief Physics Engine user-data object for connecting a physics actor to the scene object that represents it
/// \ingroup LiveLinkRuntimeModule
//----------------------------------------------------------------------------------------------------------------------
class PhysicsUserData
{
public:
  PhysicsUserData(MCOMMS::SceneObjectID objID)
  {
    objectID = objID;
    dummyActor = 0;
    constraint = 0;
    magicString[0] = 'N';
    magicString[1] = 'M';
    magicString[2] = 'S';
    magicString[3] = 'O';
  }

  char magicString[4]; // initialize ad NMSO (NM-scene object)

  bool isValid()
  {
    return magicString[0] == 'N' && magicString[1] == 'M' && magicString[2] == 'S' && magicString[3] == 'O';
  }

  MCOMMS::SceneObjectID objectID;
  PhysXActor* dummyActor;
  NxJoint* constraint;
};

//----------------------------------------------------------------------------------------------------------------------
// Physics SDK singleton class
//----------------------------------------------------------------------------------------------------------------------
bool PhysicsSDK::init()
{
  if (sm_physicsSDK != NULL) // PhysicsSDK has already been initialised with init()
  {
    NMP_ASSERT_FAIL();
    return false;
  }
  sm_initCooking = true;
  sm_physicsSDK = NULL;
  sm_debuggerConnectionRefCount = 0;

  NxPhysicsSDKDesc sdkDesc;
  sdkDesc.flags |= NX_SDKF_NO_HARDWARE;
  sm_physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, 0, sdkDesc);

  // need to warn in release as well as debug
  if (!sm_physicsSDK)
  {
    printf(
      "Unable to create the NVIDIA PhysX SDK. "
      "The NVIDIA PhysX System software is required to run this app.");
    NMP_ASSERT_FAIL();
  }

  sm_physicsSDK->setParameter(NX_SKIN_WIDTH, 0.01f);
  sm_physicsSDK->setParameter(NX_VISUALIZATION_SCALE, 9.0f);
  sm_physicsSDK->setParameter(NX_ADAPTIVE_FORCE, 0);
  sm_physicsSDK->setParameter(NX_IMPROVED_SPRING_SOLVER, 1);

  if (sm_initCooking)
  {
    bool res = NxInitCooking(0, 0);
    if (res)
      sm_initCooking = false;
  }

  // Attach an error stream so we can see if anything is reported as amiss by PhysX
  m_errorStream = new MyNxUserOutputStream();
  sm_physicsSDK->getFoundationSDK().setErrorStream(m_errorStream);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsSDK::term()
{
  sm_physicsSDK->getFoundationSDK().setErrorStream(NULL);
  delete m_errorStream;

  // If the following fires, either PhysicsSDK has not yet been initialised with init() or term() has already been
  // called
  if (sm_physicsSDK == NULL)
  {
    NMP_ASSERT_FAIL();
    return false;
  }

  // force disconnection, zero ref count
  sm_debuggerConnectionRefCount = 1;
  disconnectFromDebugger();

  sm_physicsSDK = NULL;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
PhysXSDK* PhysicsSDK::SDKinstance()
{
  // If the following fires, either PhysicsSDK has not yet been initialised with init() or term() has already been
  // called
  if (sm_physicsSDK == NULL)
  {
    NMP_ASSERT_FAIL();
    return NULL;
  }
  return sm_physicsSDK;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsSDK::connectToDebugger()
{
#if (defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
  // if this is our first time through, try connecting to the PhysX debugger;
  // we don't want to call this too often, as it stalls for a second or so each time, meaning adding N instances
  // stalls for N seconds in connect
  if (sm_debuggerConnectionRefCount == 0)
  {
    if (!sm_physicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
    {
      sm_physicsSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
    }
  }
#endif // (defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))

  ++ sm_debuggerConnectionRefCount;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsSDK::disconnectFromDebugger()
{
  if (sm_debuggerConnectionRefCount > 0)
    --sm_debuggerConnectionRefCount;

#if (defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
  // at the matched dereference, disconnect (assuming we had a connection in the first place)
  if (sm_debuggerConnectionRefCount == 0)
  {
    if (sm_physicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
    {
      sm_physicsSDK->getFoundationSDK().getRemoteDebugger()->disconnect();
    }
  }
#endif // (defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Default physics manager
//----------------------------------------------------------------------------------------------------------------------

namespace
{

void incPhysicsAssetRefCounts(MR::NetworkDef* networkDef);
void decPhysicsAssetRefCounts(MR::NetworkDef* networkDef);
void deleteOrphanedPhysicsAsset(MR::NetworkDef* networkDef);

#ifdef NM_PROFILING
  float g_totalTiming = 0.0f;
  float g_maxTime = 0.0f;
  uint32_t g_totalSamples = 0;
#endif

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsMgr::DefaultPhysicsMgr(
  RuntimeTargetContext* context, 
  DefaultAssetMgr* assetMgr, 
  const NMP::CommandLineProcessor& commandLineArguments) :
  IPhysicsMgr(),
  m_assetMgr(assetMgr),
  m_context(context),
  m_physicsFreq(240),
  m_frameIndex(0),
  m_nextPhysicsObjectID(0)
{
  PhysicsSDK::init();

  uint32_t physicsFreq = 0;
  commandLineArguments.getOptionValue("-fixedPhysicsUpdate", &physicsFreq);
  if (physicsFreq)
  {
    m_physicsFreq = physicsFreq < 240 ? physicsFreq : 240;
  }

  m_maxTimeStep = m_physicsFreq ? 1.0f/m_physicsFreq : 1.0f / 60.0f;
  m_maxNumSubSteps = 100;

  NxVec3 gravity(0, -9.81f, 0);
  // create the scene
  NxSceneDesc sceneDesc;
  sceneDesc.gravity        = gravity;
  sceneDesc.timeStepMethod = NX_TIMESTEP_FIXED;
  sceneDesc.maxTimestep    = m_maxTimeStep;
  sceneDesc.maxIter        = m_maxNumSubSteps;

  NxScene* scene = PhysicsSDK::SDKinstance()->createScene(sceneDesc);
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(MR::PhysicsScenePhysX2), NMP_VECTOR_ALIGNMENT);
  m_physicsScene = new(alignedMemory) MR::PhysicsScenePhysX2(scene);
  if (scene)
  {
    NxMaterial* material = scene->getMaterialFromIndex(0);
    material->setDynamicFriction(1);
    material->setDynamicFriction(1);
    material->setRestitution(1);
  }
  // create CCM and SOM
  m_characterControllerManager = new DefaultControllerMgr(this, context);

  resetScene();

#ifdef NM_HOST_CELL_PPU
  // Init Spurs if the app hasn't already done it
  if (!NMP::SPUManager::initialised())
  {
    NMP::SPUManager::init(4);
  }

  uint8_t spuPriorities[8] = { 2, 2, 2, 2, 2, 2, 2, 2 };
  NxCellSpursControl::initWithSpurs(NMP::SPUManager::getSpursObject(), NX_CELL_SPURS_DEFAULT_SPU_COUNT, spuPriorities, false);

  bool SPURSInitialised = NxCellSpursControl::isSpursInitialized();
  (void) SPURSInitialised; // prevent warning in debug
  NMP_ASSERT_MSG(SPURSInitialised, "Spurs intialisation test failed");
#endif

  assetMgr->setPhysicsManager(this);
  assetMgr->registerRefCountIncCallback(&incPhysicsAssetRefCounts);
  assetMgr->registerRefCountDecCallback(&decPhysicsAssetRefCounts);
  assetMgr->registerDeleteOrphanedAssetCallback(&deleteOrphanedPhysicsAsset);
}

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsMgr::~DefaultPhysicsMgr()
{
  m_assetMgr->setPhysicsManager(NULL);

  clearScene();
  PhysicsSDK::SDKinstance()->releaseScene((*m_physicsScene->getPhysXScene()));
  NMP::Memory::memFree(m_physicsScene);

  delete m_characterControllerManager;

  NxCloseCooking();
  PhysicsSDK::term();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::getPhysicsEngineID(char* buffer, uint32_t bufferLength) const
{
  NMP_STRNCPY_S(buffer, bufferLength, "PhysX2");
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::initializePhysicsCore(uint32_t numDispatchers, MR::Dispatcher** dispatchers)
{
  // Engine specific assets
  MR::Manager::getInstance().registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX2);

  MR::initMorphemePhysics(numDispatchers, dispatchers);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::finaliseInitPhysicsCore()
{
  MR::finaliseInitMorphemePhysics();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateEnvironment()
{
  NMP::Vector3 worldUpVector(0.0f, 1.0f, 0.0f);

  // Get the gravity settings from the environment.
  MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
  NMP_ASSERT(environmentManager);

  bool gravityEnabled = false;
  NMP::Vector3 gravityVector(0.0f, 0.0f, 0.0f);

  // Configure gravity and up axis
  {
    MCOMMS::Attribute* gravityAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_VECTOR);
    NMP_ASSERT(gravityAttr); // Must exist for us to initialise the gravity correctly.

    gravityVector = *((NMP::Vector3*)gravityAttr->getData());

    // TODO: For now just set the up vector to be the inverse of the gravity direction.
    // Not accurate should be independent values.
    worldUpVector = gravityVector * -1.0f;
    worldUpVector.normalise();
  }

  // Get whether gravity is enabled
  {
    MCOMMS::Attribute* gravityAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);
    NMP_ASSERT(gravityAttr); // Must exist for us to initialise the gravity correctly.

    gravityEnabled = (*(int*)gravityAttr->getData() != 0);
    if (!gravityEnabled)
    {
      // gravity is not enabled, so zero the vector.
      gravityVector.setToZero();
      // Note that the up axis must remain configured!
    }
  }

  NMP_ASSERT(m_physicsScene);
  m_physicsScene->setGravity(gravityVector);
  m_physicsScene->setWorldUpDirection(worldUpVector);
}

//----------------------------------------------------------------------------------------------------------------------
float DefaultPhysicsMgr::getActualTimestep(float deltaTime) const
{
  return deltaTime;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::simulate(float deltaTime)
{
  if (m_physicsFreq)
  {
    m_physicsScene->setLastPhysicsTimeStep(deltaTime);
    // Substep at the command line frequency
    m_physicsScene->getPhysXScene()->setTiming(m_maxTimeStep, m_maxNumSubSteps, NX_TIMESTEP_FIXED);
    m_physicsScene->getPhysXScene()->simulate(deltaTime);
    m_physicsScene->getPhysXScene()->flushStream();
    // run game processing here
    m_physicsScene->getPhysXScene()->fetchResults(NX_RIGID_BODY_FINISHED, true);
  }
  else
  {
    // We _could_ use NX_TIMESTEP_VARIABLE but then when we simulate we would
    // need to flush and fetch results when we wanted to do multiple steps. Instead
    // get PhysX to do this internally. Should make kinematic movement better too.
    m_physicsScene->setLastPhysicsTimeStep(deltaTime);
    // Run the physics at the game fps
    m_physicsScene->getPhysXScene()->setTiming(deltaTime, 1, NX_TIMESTEP_FIXED);
    m_physicsScene->getPhysXScene()->simulate(deltaTime);
    m_physicsScene->getPhysXScene()->flushStream();
    // run game processing here
    m_physicsScene->getPhysXScene()->fetchResults(NX_RIGID_BODY_FINISHED, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::createNewPhysicsBody(
  float                 dynamicFriction,
  float                 staticFriction,
  float                 restitution,
  bool                  isDynamic,
  MCOMMS::Attribute::PhysicsShapeType shapeType,
  float                 depth,
  float                 height,
  float                 length,
  float                 radius,
  float                 skinWidth,
  bool                  hasIndices,
  bool                  hasVertices,
  size_t                numPoints,
  size_t                numIndices,
  NMP::Vector3*         points,
  int32_t*              indices,
  MCOMMS::Attribute::VerticesWindingType windingType,
  NMP::Matrix34&        transform,
  float                 density,
  bool                  isConstrained,
  MCOMMS::SceneObjectID objectID,
  NMP::Matrix34&        constraintGlobalTransform,
  float                 constraintDamping,
  float                 constraintStiffness,
  NMP::Matrix34&        constraintLocalTransform,
  uint32_t              positionSolverIterationCount,
  uint32_t              NMP_UNUSED(velocitySolverIterationCount),
  float                 NMP_UNUSED(sleepThreshold),
  float                 maxAngularVelocity,
  float                 linearDamping,
  float                 angularDamping)
{
  NxActorDesc actorDesc;
  NxBodyDesc bodyDesc;
  actorDesc.body = &bodyDesc;

  NxBoxShapeDesc boxDesc;
  NxSphereShapeDesc sphereDesc;
  NxCapsuleShapeDesc capsuleDesc;
  NxTriangleMeshShapeDesc triangleMeshShapeDesc;

  NxMaterialDesc materialDesc;
  materialDesc.dynamicFriction = dynamicFriction;
  materialDesc.staticFriction = staticFriction;
  materialDesc.restitution = restitution;
  materialDesc.frictionCombineMode = NX_CM_MULTIPLY;
  materialDesc.restitutionCombineMode = NX_CM_MULTIPLY;

  bodyDesc.maxAngularVelocity = maxAngularVelocity;
  bodyDesc.angularDamping = angularDamping;
  bodyDesc.linearDamping = linearDamping;

  NxMaterial* material = m_physicsScene->getPhysXScene()->createMaterial(materialDesc);

  NxCollisionGroup group =
    (NxCollisionGroup)(isDynamic ? MR::GROUP_COLLIDABLE_PUSHABLE : MR::GROUP_COLLIDABLE_NON_PUSHABLE);

  switch (shapeType)
  {
  case MCOMMS::Attribute::PHYSICS_SHAPE_BOX:
    boxDesc.materialIndex = material->getMaterialIndex();
    boxDesc.group = group;
    boxDesc.dimensions.x = depth * 0.5f + skinWidth;
    boxDesc.dimensions.y = height * 0.5f + skinWidth;
    boxDesc.dimensions.z = length * 0.5f + skinWidth;
    boxDesc.skinWidth = skinWidth;
    actorDesc.shapes.push_back(&boxDesc);
    break;

  case MCOMMS::Attribute::PHYSICS_SHAPE_CYLINDER:
  case MCOMMS::Attribute::PHYSICS_SHAPE_CAPSULE:
    {
      capsuleDesc.materialIndex = material->getMaterialIndex();
      capsuleDesc.group = group;
      capsuleDesc.radius = radius + skinWidth;
      capsuleDesc.height = height;
      capsuleDesc.skinWidth = skinWidth;
      NxQuat q;
      q.fromAngleAxis(90.0f, NxVec3(1, 0, 0));
      capsuleDesc.localPose.M.fromQuat(q);
      actorDesc.shapes.push_back(&capsuleDesc);
    }
    break;

  case MCOMMS::Attribute::PHYSICS_SHAPE_SPHERE:
    sphereDesc.materialIndex = material->getMaterialIndex();
    sphereDesc.group = group;
    sphereDesc.radius = radius + skinWidth;
    sphereDesc.skinWidth = skinWidth;
    actorDesc.shapes.push_back(&sphereDesc);
    break;

  case MCOMMS::Attribute::PHYSICS_SHAPE_MESH:
    if (hasIndices && hasVertices)
    {
      NxTriangleMeshDesc triangleMeshDesc;
      NxTriangleMeshShapeDesc triangleMeshShapeDesc;

      triangleMeshDesc.numVertices = (NxU32)numPoints;
      triangleMeshDesc.numTriangles = (NxU32)numIndices / 3;
      triangleMeshDesc.pointStrideBytes = sizeof(NMP::Vector3);
      triangleMeshDesc.triangleStrideBytes = 3 * sizeof(int);
      triangleMeshDesc.points = points;
      triangleMeshDesc.triangles = indices;
      triangleMeshDesc.flags = 0;

      if (windingType != MCOMMS::Attribute::VERTICES_WINDING_UNDEFINED)
      {
        NMP_MSG("PhysicsManager: mesh vertices winding undefined - SceneObjectID: %d.", objectID);
      }
      if (windingType == MCOMMS::Attribute::VERTICES_WINDING_COUNTERCLOCKWISE)
        triangleMeshDesc.flags = NX_MF_FLIPNORMALS;

      bool res = initializeTriangleMeshShapeDesc(triangleMeshDesc, triangleMeshShapeDesc);

      if (res)
      {
        triangleMeshShapeDesc.materialIndex = material->getMaterialIndex();
        triangleMeshShapeDesc.group = group;
        triangleMeshShapeDesc.skinWidth = skinWidth;
        actorDesc.shapes.push_back(&triangleMeshShapeDesc);
      }
      else
      {
        NMP_MSG("PhysicsManager: Error initializing the convex shape - SceneObjectID: %d", objectID);
      }
    }
    break;

  case MCOMMS::Attribute::PHYSICS_SHAPE_CONVEX_HULL:
    if (hasIndices && hasVertices)
    {
      NxConvexMeshDesc convexDesc;
      NxConvexShapeDesc triangleMeshShapeDesc;

      convexDesc.numVertices = (NxU32)numPoints;
      convexDesc.numTriangles = (NxU32)numIndices / 3;
      convexDesc.pointStrideBytes = sizeof(NMP::Vector3);
      convexDesc.triangleStrideBytes = 3 * sizeof(int);
      convexDesc.points = points;
      convexDesc.triangles = indices;
      convexDesc.flags = NX_CF_COMPUTE_CONVEX; // Setting this flag tells PhysX to compute the convex hull from the 
                                               // mesh that it is supplied

      if (windingType != MCOMMS::Attribute::VERTICES_WINDING_UNDEFINED)
      {
        NMP_MSG("PhysicsManager: mesh vertices winding undefined - SceneObjectID: %d.", objectID);
      }

      if (windingType == MCOMMS::Attribute::VERTICES_WINDING_COUNTERCLOCKWISE)
      {
        convexDesc.flags |= NX_MF_FLIPNORMALS;
      }

      bool res = initializeConvexShapeDesc(convexDesc, triangleMeshShapeDesc);

      if (res)
      {
        triangleMeshShapeDesc.materialIndex = material->getMaterialIndex();
        triangleMeshShapeDesc.group = group;
        triangleMeshShapeDesc.skinWidth = skinWidth;
        actorDesc.shapes.push_back(&triangleMeshShapeDesc);
      }
      else
      {
        NMP_MSG("PhysicsManager: Error initializing the convex shape - SceneObjectID: %d", objectID);
      }
    }
    break;

  default:
    NMP_ASSERT_FAIL();
    break;
  }

  // make sure that we don't send to physx the scale part
  NMP::Quat rot = transform.toQuat();
  rot.normalise();
  NMP::Vector3 pos = transform.translation();
  NMP::Matrix34 mat(rot, pos);
  actorDesc.globalPose = MR::nmMatrix34ToNxMat34(mat);

  // It seems that the iteration count is still used by statics to influence contacting dynamic
  // bodies - so use a low value for statics to stop this masking attempts to reduce the solver
  // iteration count for the rig.
  bodyDesc.solverIterationCount = isDynamic ? positionSolverIterationCount : 1;
  bodyDesc.mass = density;

  if (!isDynamic && !isConstrained) // safety check
  {
    bodyDesc.flags |= NX_BF_KINEMATIC;
  }

  NxActor* actor = m_physicsScene->getPhysXScene()->createActor(actorDesc);
  if (!actor)
  {
    NMP_MSG("PhysicsManager: could not create the physics body - SceneObjectID: %d", objectID);
  }
  else
  {
    MR::PhysicsObjectID actorID = assignPhysicsIDToActor(actor);

    PhysicsUserData* userData = new PhysicsUserData(objectID);
    actor->userData = (void*)userData;
    getSceneActors().push_back(actor);
    getMaterials().push_back(material);

    if (isConstrained && isDynamic)
    {
      // create a kinematic actor to attach the new actor to
      NxBoxShapeDesc dummyBoxDesc;
      dummyBoxDesc.dimensions.x = 0.1f;
      dummyBoxDesc.dimensions.y = 0.1f;
      dummyBoxDesc.dimensions.z = 0.1f;
      dummyBoxDesc.shapeFlags |= NX_SF_DISABLE_COLLISION;

      NxActorDesc dummyActorDesc;
      dummyActorDesc.shapes.push_back(&dummyBoxDesc);

      NxBodyDesc dummyBodyDesc;
      dummyBodyDesc.flags |= NX_BF_KINEMATIC;
      dummyBodyDesc.solverIterationCount = 1;
      dummyBodyDesc.mass = 1;

      dummyActorDesc.body = &dummyBodyDesc;

      NxActor* dummyActor = m_physicsScene->getPhysXScene()->createActor(dummyActorDesc);
      dummyActor->setGlobalPose(MR::nmMatrix34ToNxMat34(constraintGlobalTransform));
      userData->dummyActor = dummyActor;

      NxD6JointDesc constraintDesc;
      constraintDesc.jointFlags |= NX_JF_COLLISION_ENABLED; // TODO: make sense?
      constraintDesc.flags |= NX_D6JOINT_SLERP_DRIVE;

#define NX_VERSION_WITH_ACCEL_SPRING ((2<<24)+(8<<16)+(1<<8) + 0)
#if NX_PHYSICS_SDK_VERSION >= NX_VERSION_WITH_ACCEL_SPRING
      constraintDesc.useAccelerationSpring = true;
#endif

      constraintDesc.xMotion = NX_D6JOINT_MOTION_LOCKED;
      constraintDesc.yMotion = NX_D6JOINT_MOTION_LOCKED;
      constraintDesc.zMotion = NX_D6JOINT_MOTION_LOCKED;

      constraintDesc.swing1Motion = NX_D6JOINT_MOTION_FREE; //NX_D6JOINT_MOTION_LIMITED;
      constraintDesc.swing2Motion = NX_D6JOINT_MOTION_FREE; //NX_D6JOINT_MOTION_LIMITED;
      constraintDesc.twistMotion  = NX_D6JOINT_MOTION_FREE; //NX_D6JOINT_MOTION_LIMITED;

      constraintDesc.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
      constraintDesc.slerpDrive.damping = constraintDamping;
      constraintDesc.slerpDrive.spring = constraintStiffness;

      constraintLocalTransform.invert();

      NMP::Vector3 localAnchor0 = NMP::Vector3Zero();
      NMP::Vector3 localAnchor1 = constraintLocalTransform.translation();

      NMP::Vector3 localAxis0 = NMP::Matrix34Identity().xAxis();
      NMP::Vector3 localAxis1 = constraintLocalTransform.xAxis();

      NMP::Vector3 localNormal0 = NMP::Matrix34Identity().yAxis();
      NMP::Vector3 localNormal1 = constraintLocalTransform.yAxis();

      constraintDesc.localAnchor[0] = MR::nmVector3ToNxVec3(localAnchor0);
      constraintDesc.localAnchor[1] = MR::nmVector3ToNxVec3(localAnchor1);

      constraintDesc.localAxis[0] = MR::nmVector3ToNxVec3(localAxis0);
      constraintDesc.localAxis[1] = MR::nmVector3ToNxVec3(localAxis1);

      constraintDesc.localNormal[0] = MR::nmVector3ToNxVec3(localNormal0);
      constraintDesc.localNormal[1] = MR::nmVector3ToNxVec3(localNormal1);

      constraintDesc.actor[0] = dummyActor;
      constraintDesc.actor[1] = actor;

      NxJoint* constraint = m_physicsScene->getPhysXScene()->createJoint(constraintDesc);
      userData->constraint = constraint;

      // no material!
      // NB the dummy actor and the constraint are not added to the container.. so they can be destroyed easily
    }

    return actorID;
  }

  return MR_INVALID_PHYSICS_OBJECT_ID;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::destroyPhysicsBody(MCOMMS::SceneObjectID objectID)
{
  // destroy the physics part if there is one
  PhysXActor* foundActor = 0;
  size_t foundActorIndex = 0;
  for (size_t i = 0; i < m_sceneActors.size(); ++i)
  {
    if (!m_sceneActors[i])
      continue;

    PhysicsUserData* userData = (PhysicsUserData*)m_sceneActors[i]->userData;
    if (userData && userData->isValid())
    {
      MCOMMS::SceneObjectID foundObjectID = userData->objectID;
      if (foundObjectID == objectID)
      {
        foundActor = m_sceneActors[i];
        foundActorIndex = i;
        break;
      }
    }
  }

  if (foundActor)
  {
    std::vector<NxTriangleMesh*> meshData;

    // destroy actor's materials
    int numShapes = foundActor->getNbShapes();
    for (int iShape = 0; iShape < numShapes; ++iShape)
    {
      NxShape* shape = foundActor->getShapes()[iShape];

      if (shape->getType() == NX_SHAPE_MESH)
      {
        NxTriangleMeshShape* trimeshShape = shape->isTriangleMesh();
        if (trimeshShape)
        {
          meshData.push_back(&trimeshShape->getTriangleMesh());
        }
      }

      NxMaterialIndex mat = shape->getMaterial();
      const NxMaterialIndex defaultIndex = 0;
      if (mat != defaultIndex)
      {
        NxMaterial* matPointer = m_physicsScene->getPhysXScene()->getMaterialFromIndex(mat);
        std::vector<NxMaterial*>& mats = getMaterials();
        bool matFound = false;
        size_t matIndex = 0;
        for (size_t iMat = 0; iMat < mats.size(); ++iMat)
        {
          if (mats[iMat] == matPointer)
          {
            matFound = true;
            matIndex = iMat;
            break;
          }
        }
        if (matFound)
        {
          mats[matIndex] = mats.back();
          mats.pop_back();
        }
        m_physicsScene->getPhysXScene()->releaseMaterial(*matPointer);
      }
    }

    // if the object is constrained, release the dummy actor and the physical joint
    PhysicsUserData* userData = (PhysicsUserData*)foundActor->userData;
    if (userData->constraint && userData->dummyActor)
    {
      m_physicsScene->getPhysXScene()->releaseActor(*userData->dummyActor);
      m_physicsScene->getPhysXScene()->releaseJoint(*userData->constraint);
      userData->constraint = 0;
      userData->dummyActor = 0;
    }

    delete (PhysicsUserData*)foundActor->userData;
    foundActor->userData = 0;

    unassignPhysicsID(foundActor);

    // finally release the actor
    m_physicsScene->getPhysXScene()->releaseActor(*foundActor);
    m_sceneActors[foundActorIndex] = m_sceneActors.back();
    m_sceneActors.pop_back();

    // check if there is a mesh data to destroy
    for (size_t i = 0; i < meshData.size(); ++i)
    {
      if (meshData[i]->getReferenceCount() == 0)
      {
        m_physicsScene->getPhysXScene()->getPhysicsSDK().releaseTriangleMesh(*meshData[i]);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::createConstraint(
  uint64_t            constraintGUID,
  MR::PhysicsObjectID physicsObjectID, 
  const NMP::Vector3& wsConstraintPosition, 
  bool                NMP_UNUSED(lockOrientation),
  bool                constrainAtCOM)
{
  NMP_ASSERT_MSG(!getConstraint(constraintGUID), "createConstraint called for a GUID that already exists!");

  PhysXActor *actorToConstrain = getActorByPhysicsID(physicsObjectID);
  NMP_ASSERT_MSG(actorToConstrain, "Could not find physics object of ID %i!", physicsObjectID);

  // This constraint object is persisted in order to make it possible to modify it later.
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(Constraint), NMP_VECTOR_ALIGNMENT);
  Constraint *newConstraint = new(alignedMemory) Constraint();

  // create a dummy kinematic actor to attach the constrained body
  NxBoxShapeDesc dummyBoxDesc;
  dummyBoxDesc.dimensions.x = dummyBoxDesc.dimensions.y = dummyBoxDesc.dimensions.z = 0.1f;
  dummyBoxDesc.shapeFlags |= NX_SF_DISABLE_COLLISION;

  NxBodyDesc dummyBodyDesc;
  dummyBodyDesc.flags |= NX_BF_KINEMATIC;
  dummyBodyDesc.solverIterationCount = 1;
  dummyBodyDesc.mass = 1;

  NxActorDesc dummyActorDesc;
  dummyActorDesc.shapes.push_back(&dummyBoxDesc);
  dummyActorDesc.body = &dummyBodyDesc;

  NxActor* dummyActor = m_physicsScene->getPhysXScene()->createActor(dummyActorDesc);
  NMP::Matrix34 constraintMatrix = NMP::Matrix34Identity();
  constraintMatrix.setTranslation(wsConstraintPosition);
  dummyActor->setGlobalPose(MR::nmMatrix34ToNxMat34(constraintMatrix));

  NxD6JointDesc constraintDesc;
  constraintDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
  constraintDesc.flags |= NX_D6JOINT_SLERP_DRIVE;

  // TODO: Constrain correctly based on the flags supplied.
  constraintDesc.xMotion = NX_D6JOINT_MOTION_LOCKED;
  constraintDesc.yMotion = NX_D6JOINT_MOTION_LOCKED;
  constraintDesc.zMotion = NX_D6JOINT_MOTION_LOCKED;

  constraintDesc.swing1Motion = NX_D6JOINT_MOTION_FREE;
  constraintDesc.swing2Motion = NX_D6JOINT_MOTION_FREE;
  constraintDesc.twistMotion  = NX_D6JOINT_MOTION_FREE;

  constraintDesc.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
  constraintDesc.slerpDrive.damping = 1.0f; // TODO: Check what these values should be
  constraintDesc.slerpDrive.spring = 1.0f; // TODO: Check what these values should be


  NMP::Vector3 localAnchor0 = NMP::Vector3Zero();
  NxVec3 localOffsetOfGrab = MR::nmVector3ToNxVec3(wsConstraintPosition) - actorToConstrain->getGlobalPosition();
  actorToConstrain->getGlobalOrientationQuat().inverseRotate(localOffsetOfGrab);

  NMP::Vector3 localAxis0 = NMP::Matrix34Identity().xAxis();
  NMP::Vector3 localAxis1 = NMP::Matrix34Identity().xAxis(); // TODO: Anything to do if we're constraining orient?

  NMP::Vector3 localNormal0 = NMP::Matrix34Identity().yAxis();
  NMP::Vector3 localNormal1 = NMP::Matrix34Identity().yAxis(); // TODO: Anything to do if we're constraining orient?

  constraintDesc.localAnchor[0] = MR::nmVector3ToNxVec3(localAnchor0);
  constraintDesc.localAnchor[1] = constrainAtCOM ? MR::nmVector3ToNxVec3(NMP::Vector3Zero()) : localOffsetOfGrab;

  constraintDesc.localAxis[0] = MR::nmVector3ToNxVec3(localAxis0);
  constraintDesc.localAxis[1] = MR::nmVector3ToNxVec3(localAxis1);

  constraintDesc.localNormal[0] = MR::nmVector3ToNxVec3(localNormal0);
  constraintDesc.localNormal[1] = MR::nmVector3ToNxVec3(localNormal1);

  constraintDesc.actor[0] = dummyActor;
  constraintDesc.actor[1] = actorToConstrain;

  NxJoint* constraintJoint = m_physicsScene->getPhysXScene()->createJoint(constraintDesc);
  
  newConstraint->m_constrainedActor = actorToConstrain;
  newConstraint->m_dummyActor = dummyActor;
  newConstraint->m_jointConstraint = constraintJoint;
  newConstraint->m_isCOMConstraint = constrainAtCOM;
  newConstraint->m_grabOffsetFromCOM = MR::nmNxVec3ToVector3(localOffsetOfGrab);

  m_constraintMap.insert(constraintGUID, newConstraint);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::destroyConstraint(uint64_t constraintGUID)
{
  Constraint* constraint = NULL;
  bool found = m_constraintMap.find(constraintGUID, &constraint);

  if (!found)
  {
    return false;
  }

  // Release the dummy actor and the joint itself
  m_physicsScene->getPhysXScene()->releaseActor(*constraint->m_dummyActor);
  m_physicsScene->getPhysXScene()->releaseJoint(*constraint->m_jointConstraint);

  // Remove the constraint from the map
  m_constraintMap.erase(constraintGUID);

  // Delete the constraint record itself
  NMP::Memory::memFree(constraint);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::moveConstraint(uint64_t constraintGUID, const NMP::Vector3 &newGrabPosition)
{
  Constraint* constraint = NULL;
  bool found = m_constraintMap.find(constraintGUID, &constraint);

  if (!found)
  {
    return false;
  }

  NMP::Matrix34 constraintMatrix = NMP::Matrix34Identity();
  constraintMatrix.setTranslation(newGrabPosition);
  constraint->m_dummyActor->setGlobalPose(MR::nmMatrix34ToNxMat34(constraintMatrix));

  // We don't need to do anything here with COM constraints, they should just work.

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::applyForce(
  uint32_t            physicsEngineObjectID,
  ForceMode           mode,
  const NMP::Vector3& force,
  bool                applyAtCOM,
  const NMP::Vector3& NMP_UNUSED(localSpacePosition),
  const NMP::Vector3& worldSpacePosition)
{
  PhysXActor* actor = getActorByPhysicsID(physicsEngineObjectID);
  NMP_ASSERT_MSG(actor, "Could not find physics object of ID %i!", physicsEngineObjectID);
  if(!actor)
  {
    return false;
  }

  NMP::Vector3 forcePosition = worldSpacePosition;
  if(applyAtCOM)
  {
    NxMat34 pos;
    pos.multiply(actor->getGlobalPose(), actor->getCMassLocalPose());

    forcePosition = MR::nmNxMat34ToNmMatrix34(pos).translation();
  }

  switch(mode)
  {
  case IPhysicsMgr::kFORCE:
    {
      NMP::Vector3 actorCOM = MR::getActorCOMPos(actor);
      NMP::Vector3 torque = NMP::vCross(forcePosition - actorCOM, force);
      actor->addForce(MR::nmVector3ToNxVec3(force));
      actor->addTorque(MR::nmVector3ToNxVec3(torque));
    break; 
    }
    
  case IPhysicsMgr::kIMPULSE:
    MR::addImpulseToActor(*actor, force, forcePosition, 1.0f);
    break; 

  case IPhysicsMgr::kVELOCITY_CHANGE:
    MR::addVelocityChangeToActor(*actor, force, forcePosition);
    break; 

  default:
    NMP_ASSERT_FAIL_MSG("Unknown force mode in apply force");
  }
  return true; 
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::setPhysicsObjectAttribute(
  uint32_t physicsEngineObjectID,
  const MCOMMS::Attribute* physicsObjAttribute)
{
  NxActor * actor = getActorByPhysicsID(physicsEngineObjectID);
  NMP_ASSERT_MSG(actor, "Could not find physics object of ID %i!", physicsEngineObjectID);
  if(!actor)
  {
    return false;
  }

  if (!actor->isDynamic())
  {
    return false;
  }

  switch(physicsObjAttribute->getSemantic())
  {
    // Instantaneously teleport the object to the world space transform specified in the attribute data
    case MCOMMS::Attribute::SEMANTIC_TRANSFORM:
      {
        NMP::Matrix34* newWorldSpaceTransform = (NMP::Matrix34*)(physicsObjAttribute->getData());
        if (actor->readBodyFlag(NX_BF_KINEMATIC))
        {
          actor->moveGlobalPose(MR::nmMatrix34ToNxMat34(*newWorldSpaceTransform));
        }
        else
        {
          MR::setActorGlobalPose(*actor, *newWorldSpaceTransform);
          MR::setActorLinearVelocity(*actor, NMP::Vector3(NMP::Vector3::InitZero));
          MR::setActorAngularVelocity(*actor, NMP::Vector3(NMP::Vector3::InitZero));
        }
        return true;
      }

    default:
      break;
  }

  // attribute semantic not handled
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::setPhysicsEnvironmentAttribute(const MCOMMS::Attribute* physicsEnvAttribute)
{
  // We must have a physics scene to work with
  NMP_ASSERT(m_physicsScene);
  if(!m_physicsScene)
  {
    return false;
  }
  
  switch(physicsEnvAttribute->getSemantic())
  {
    // Only handling gravity related attibutes at the moment, which we pass on to updateEnvironment()
    // provided the environment has got both attribs already setup
  case MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED:
  case MCOMMS::Attribute::SEMANTIC_GRAVITY_VECTOR:
    {
      MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
      if(environmentManager)
      {
        MCOMMS::Attribute* gravityAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_VECTOR);
        MCOMMS::Attribute* gravityEnabledAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);
        if(gravityAttr && gravityEnabledAttr)
        {
          updateEnvironment();
        }
      }
    break;
    }
  default:
    break;
  }

  // attribute semantic not handled
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::validatePluginList(const NMP::OrderedStringTable& pluginList)
{
  if (pluginList.getNumEntries() == 0)
  {
    return true;
  }

  const char* physicsPlugin = "acPluginPhysX2_target_"NM_PLATFORM_FORMAT_STRING;
  const char* debugPhysicsPlugin = "acPluginPhysX2_target_"NM_PLATFORM_FORMAT_STRING"_debug";

  // Physics is registered first so we expect it to be the first compiled plug-in
  const char* pluginEntry0 = pluginList.getEntryString(0);
  if (NMP_STRCMP(pluginEntry0, physicsPlugin) == 0 || NMP_STRCMP(pluginEntry0, debugPhysicsPlugin) == 0)
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void* DefaultPhysicsMgr::getPhysicsObjectPointerFromPhysicsID(MR::PhysicsObjectID id) const
{
  return getActorByPhysicsID(id);
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::getPhysicsObjectIDFromPhysicsObjectPointer(void* physicsObject) const
{
  return getPhysicsIDForActor((PhysXActor*) physicsObject);
}

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsMgr::Constraint* DefaultPhysicsMgr::getConstraint(uint64_t guid) const
{
  Constraint* constraint = 0;
  m_constraintMap.find(guid);
  return constraint;
}

//----------------------------------------------------------------------------------------------------------------------
PhysXTriangleMesh* DefaultPhysicsMgr::createTriangleMeshData(const PhysXTriangleMeshDesc& meshDesc)
{
  phx::MemoryWriteBuffer buf;
  bool status = NxCookTriangleMesh(meshDesc, buf);
  if (!status)
  {
    return 0;
  }

  phx::MemoryReadBuffer readBuffer(buf.data);
  PhysXTriangleMesh* mesh = PhysicsSDK::SDKinstance()->createTriangleMesh(readBuffer);
  return mesh;
}

//----------------------------------------------------------------------------------------------------------------------
PhysXConvexMesh* DefaultPhysicsMgr::createConvexMeshData(const PhysXConvexMeshDesc& meshDesc)
{
  phx::MemoryWriteBuffer buf;
  bool status = NxCookConvexMesh(meshDesc, buf);
  if (!status)
    return 0;
  phx::MemoryReadBuffer readBuffer(buf.data);
  PhysXConvexMesh* mesh = PhysicsSDK::SDKinstance()->createConvexMesh(readBuffer);
  return mesh;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::initializeTriangleMeshShapeDesc(
  const PhysXTriangleMeshDesc& meshDesc,
  PhysXTriangleMeshShapeDesc&  shapeDesc)
{
  PhysXTriangleMesh* meshData = createTriangleMeshData(meshDesc);
  if (!meshData)
    return false;

  shapeDesc.meshData = meshData;
  shapeDesc.shapeFlags = NX_SF_FEATURE_INDICES;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::initializeConvexShapeDesc(
  const PhysXConvexMeshDesc& meshDesc,
  PhysXConvexShapeDesc&      shapeDesc)
{
  PhysXConvexMesh* meshData = createConvexMeshData(meshDesc);
  if (!meshData)
    return false;

  shapeDesc.meshData = meshData;
  shapeDesc.shapeFlags = NX_SF_FEATURE_INDICES;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::resetScene()
{
  PhysicsSDK::disconnectFromDebugger();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::initializeScene()
{
#ifdef NM_PROFILING
  g_totalTiming = 0.0f;
  g_maxTime = 0.0f;
  g_totalSamples = 0;
#endif
  PhysicsSDK::connectToDebugger();
}

//----------------------------------------------------------------------------------------------------------------------
#if PHX_SWEEP_REPORT
  #define MAXCOLLISIONENABLED 256
static bool origCollisionEnabled[MAXCOLLISIONENABLED];
static size_t numOrigCollisionEnabled;

class MySweepReport : public NxUserEntityReport<NxSweepQueryHit>
{
public:
  MySweepReport(const MR::PhysicsRigPhysX2* skipChar) : m_distance(NX_MAX_F32)
  {
    m_skipChar = const_cast<MR::PhysicsRigPhysX2*>(skipChar);
    numOrigCollisionEnabled = skipChar->getNumParts();
    NMP_ASSERT(numOrigCollisionEnabled <= MAXCOLLISIONENABLED);
    for (uint32_t i = 0; i < (uint32_t)numOrigCollisionEnabled; ++i)
    {
      origCollisionEnabled[i] = ((MR::PhysicsRigPhysX2::PartPhysX*)m_skipChar->getPart(i))->getCollisionEnabled();
      if (origCollisionEnabled[i])
        ((MR::PhysicsRigPhysX2::PartPhysX*)m_skipChar->getPart(i))->enableCollision(false);
    }
  }

  virtual ~MySweepReport()
  {
    for (uint32_t i = 0; i < (uint32_t)numOrigCollisionEnabled; ++i)
      ((MR::PhysicsRigPhysX2::PartPhysX*)m_skipChar->getPart(i))->enableCollision(origCollisionEnabled[i]);
  }

  virtual bool onEvent(NxU32 nbEntities, NxSweepQueryHit* entities)
  {
    for (NxU32 iEntity = 0; iEntity < nbEntities; ++iEntity)
    {
      NxSweepQueryHit& hit = entities[iEntity];

      if (hit.t < m_distance)
        m_distance = hit.t;
    }
    return true;
  }

  MR::PhysicsRigPhysX2* m_skipChar;
  float                 m_distance;
};
#endif // #ifdef PHX_SWEEP_REPORT

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::clearScene()
{
  for (size_t i = 0; i < m_sceneActors.size(); ++i)
  {
    if (m_sceneActors[i])
    {
      m_physicsScene->getPhysXScene()->releaseActor(*m_sceneActors[i]);
    }
  }
  m_sceneActors.clear();

  while (!m_materials.empty())
  {
    m_physicsScene->getPhysXScene()->releaseMaterial(*m_materials.back());
    m_materials.pop_back();
  }

  for (size_t i = 0; i < m_joints.size(); ++i)
  {
    m_physicsScene->getPhysXScene()->releaseJoint(*m_joints[i]);
  }

  m_joints.clear();

  clearAllPhysicsIDs();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::createPhysicsRig(MR::Network *network, NMP::Vector3 *initialPosition)
{
  NMP_ASSERT(network);
  NMP_ASSERT(initialPosition);

  MR::AnimRigDef* rig = network->getActiveRig();
  MR::PhysicsRigDef* physicsRigDef = getActivePhysicsRigDef(network);

  if (physicsRigDef != NULL)
  {
    NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(
      MR::PhysicsRigPhysX2::getMemoryRequirements(physicsRigDef));
    NMP_ASSERT(resource.ptr);
    MR::PhysicsRigPhysX2* physicsRig = MR::PhysicsRigPhysX2::init(
      resource,
      physicsRigDef,
      getPhysicsScene(),
      rig,
      getAnimToPhysicsMap(network->getNetworkDef(), network->getActiveAnimSetIndex()),
      1 << MR::GROUP_CHARACTER_PART,
      (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE));

    physicsRig->setKinematicPos(*initialPosition);
    setPhysicsRig(network, physicsRig);

    uint32_t numParts = physicsRig->getNumParts();
    for (uint32_t i = 0; i != numParts; ++i)
    {
      MR::PhysicsRigPhysX2::PartPhysX* part =  (MR::PhysicsRigPhysX2::PartPhysX*)physicsRig->getPart(i);
      NxActor* actor = part->getActor();
      assignPhysicsIDToActor(actor);
    }
  }
  else
  { // There is no physics rig in this anim set.
    setPhysicsRig(network, NULL);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::destroyPhysicsRig(MR::Network *network)
{
  NMP_ASSERT(network);

  // Check whether this network has a physics rig
  MR::PhysicsRig* physicsRigExists = getPhysicsRig(network);

  if (physicsRigExists)
  {
    MR::PhysicsRigDef* physicsRigDef = getActivePhysicsRigDef(network);
    if (physicsRigDef)
    {
      MR::PhysicsRig* physicsRig = getPhysicsRig(network);

      if (physicsRig)
      {
        // Remove all physics id associations with parts in the rig
        uint32_t numParts = physicsRig->getNumParts();
        for (uint32_t i = 0; i != numParts; ++i)
        {
          MR::PhysicsRigPhysX2::PartPhysX* part =  (MR::PhysicsRigPhysX2::PartPhysX*)physicsRig->getPart(i);
          NxActor* actor = part->getActor();
          unassignPhysicsID(actor);
        }

        physicsRig->term();
        NMP::Memory::memFree(physicsRig);
      }

      setPhysicsRig(network, NULL);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateConnections(
  MCOMMS::InstanceID NMP_UNUSED(instanceID),
  float              NMP_UNUSED(deltaTime))
{
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePreController(
  MCOMMS::InstanceID instanceID,
  float              deltaTime)
{
  NET_LOG_ENTER_FUNC();
  ControllerRecord* charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);

  NMP_ASSERT(m_characterControllerManager);

  if (charControllerRecord->m_network->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
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
    charControllerRecord->m_deltaTranslation = charControllerRecord->m_network->getTranslationChange();
    charControllerRecord->m_deltaOrientation = charControllerRecord->m_network->getOrientationChange();

    NMP::Vector3 charDeltaTranslation = charControllerRecord->m_characterOrientation.rotateVector(charControllerRecord->m_deltaTranslation);
    charControllerRecord->m_characterOrientationOld = charControllerRecord->m_characterOrientation;
    charControllerRecord->m_characterOrientation *= charControllerRecord->m_deltaOrientation;
    charControllerRecord->m_characterOrientation.normalise();

    m_context->getNetworkInstanceManager()->findInstanceRecord(instanceID)->setRootTransform(
      NMP::Matrix34(charControllerRecord->m_characterOrientation, charControllerRecord->m_characterPosition));

    charControllerRecord->m_velocityInGravityDirectionDt = deltaTime;

#ifdef ANIM_GRAVITY
    if (charControllerRecord->m_overrideBasics.m_currentVerticalMoveState != MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY)
    {
      // Don't apply gravity to the CC but set the velocity in gravity direction to the component of the animation
      // delta in the gravity direction so that CC will inherit anim's velocity when switching back to the default
      // (gravity enabled) state.

      // Find the direction of gravity.
      NMP::Vector3 gravityDirection(getPhysicsScene()->getGravity());
      gravityDirection.fastNormalise();

      // Set the velocity in the gravity direction.
      charControllerRecord->m_velocityInGravityDirection =
        gravityDirection * charDeltaTranslation.dot(gravityDirection) * deltaTime;
    }
    else
    {
      // apply gravity in real world units
      MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
      NMP_ASSERT(environmentManager);

      bool gravityEnabled = true;
      MCOMMS::Attribute* gravityEnabledAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);

      if (gravityEnabledAttr != 0)
        gravityEnabled = (*(int*)gravityEnabledAttr->getData() != 0);
      gravityEnabled &= (charControllerRecord->m_overrideBasics.m_currentVerticalMoveState == MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY);

      if (gravityEnabled)
      {
        NMP::Vector3 gravity = getPhysicsScene()->getGravity();
        charControllerRecord->m_velocityInGravityDirection += (gravity * deltaTime);
        charDeltaTranslation += (charControllerRecord->m_velocityInGravityDirection * deltaTime);
      }
    }
#endif

    // this should overwrite our root translation via setRootFromCharacterController when the manager
    // updates anyway
    m_characterControllerManager->setRequestedMovement(instanceID, charDeltaTranslation);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePrePhysics(
  MCOMMS::InstanceID instanceID,
  float deltaTime)
{
  NET_LOG_ENTER_FUNC();
#if defined(NM_DEBUG)
  // For detecting memory leaks
#define NODE_MEMORY_DEBUGGINGx
#ifdef NODE_MEMORY_DEBUGGING
  ControllerRecord* ccmRecord = m_characterControllerManager->getControllerRecord(instanceID);
  ccmRecord->m_network->attribNodeAccounting(10000);
#endif // NODE_MEMORY_DEBUGGING
#endif // NM_DEBUG

  ControllerRecord *charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);

  if (getPhysicsRig(charControllerRecord->m_network))
    getPhysicsRig(charControllerRecord->m_network)->updatePrePhysics(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePostPhysicsInit(
  MCOMMS::InstanceID instanceID,
  float              deltaTime)
{
  ControllerRecord* charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);

  if (getPhysicsRig(charControllerRecord->m_network))
    getPhysicsRig(charControllerRecord->m_network)->updatePostPhysics(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePostPhysics(
  MCOMMS::InstanceID instanceID,
  float              deltaTime)
{
  NET_LOG_ENTER_FUNC();
  ControllerRecord* charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);

  if (charControllerRecord->m_network->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    NET_LOG_MESSAGE(99, "NetworkInstanceRecord::updatePostPhysics: Updating trajectory");
    // Update the root translation and orientation.
    // NOTE: When using physics the root needs to be updated in updatePostPhysics so that the network
    // has the new root location, and can thus convert from world to local space. This required removing
    // setting from the root position from SceneObjectRecord::update (since that would be too late)
    charControllerRecord->m_deltaOrientation = charControllerRecord->m_network->getOrientationChange();
    charControllerRecord->m_deltaTranslation = charControllerRecord->m_network->getTranslationChange();
    NMP::Vector3 charDeltaTranslation = charControllerRecord->m_characterOrientation.rotateVector(charControllerRecord->m_deltaTranslation);
    NMP::Vector3 newPos = charControllerRecord->m_characterPosition + charDeltaTranslation;

    // Make the root follow the ragdoll.
    charControllerRecord->m_characterPosition = newPos;
    charControllerRecord->m_characterOrientationOld = charControllerRecord->m_characterOrientation;
    charControllerRecord->m_characterOrientation *= charControllerRecord->m_deltaOrientation;
    charControllerRecord->m_characterOrientation.normalise();
    charControllerRecord->m_network->updateCharacterPropertiesWorldRootTransform(
      NMP::Matrix34(charControllerRecord->m_characterOrientation, charControllerRecord->m_characterPosition),
      true);

    // Find the component of delta translation in the gravity direction.
    NMP::Vector3 gravity = getPhysicsScene()->getGravity();
    NMP::Vector3 gravityDirectionVector;
    gravityDirectionVector.normaliseDep(gravity);
    float deltaTranslationInGravityDirection = gravityDirectionVector.dot(charDeltaTranslation);

    // Calculate velocity in gravity direction.
    NMP_ASSERT(deltaTime > 0.0f);
    charControllerRecord->m_velocityInGravityDirection = gravityDirectionVector *
        (deltaTranslationInGravityDirection / deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::setRoot(
  MCOMMS::InstanceID  instanceID,
  const NMP::Vector3& NMP_UNUSED(pos))
{
  // Set the position of the network root.  This function is called from updateControllers(), which
  // has already taken the movement requested by the animation system and filtered it through the
  // NxController's fake physics (if the controller is switched on, or just keyframed the animation along
  // if not).  It has also calculated whether the controller is on the ground or in the air, and whether
  // it has just transitioned between the two.  The ccmRecord's m_characterPosition is at this stage set
  // to point to the foot centre position as processed by the above.

  // TODO: convert to local space
  ControllerRecord* charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);
  NMP::Vector3 actualTranslation = (charControllerRecord->m_characterPosition -
                                    charControllerRecord->m_characterPositionOld);

  MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
  NMP_ASSERT(environmentManager);

  if (charControllerRecord->m_network->getRootControlMethod() != MR::Network::ROOT_CONTROL_PHYSICS)
  {
    // record the movement of the entity - but don't add on vertical velocity due to stepping.
    bool gravityEnabled = true;
    MCOMMS::Attribute* gravityEnabledAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);
    
    if (gravityEnabledAttr)
      gravityEnabled = (*(int*)gravityEnabledAttr->getData() != 0);
    gravityEnabled &= (charControllerRecord->m_overrideBasics.m_currentVerticalMoveState == MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY);

    // Find the component of actual translation in the gravity direction.
    NMP::Vector3 gravity = getPhysicsScene()->getGravity();
    NMP::Vector3 gravityDirectionVector;
    gravityDirectionVector.normaliseDep(gravity);
    float actualTranslationInGravityDirection = gravityDirectionVector.dot(actualTranslation);

    // Find the component of delta translation in the gravity direction.
    float deltaTranslationInGravityDirection = gravityDirectionVector.dot(charControllerRecord->m_deltaTranslation);

    if (actualTranslationInGravityDirection > deltaTranslationInGravityDirection && gravityEnabled == true)
    {
      // Find the component of delta translation in the gravity direction.
      charControllerRecord->m_velocityInGravityDirection = actualTranslation - charControllerRecord->m_deltaTranslation;
      deltaTranslationInGravityDirection = gravityDirectionVector.dot(charControllerRecord->m_velocityInGravityDirection);

      // Calculate velocity in gravity direction
      charControllerRecord->m_velocityInGravityDirection =
        gravityDirectionVector * (deltaTranslationInGravityDirection / charControllerRecord->m_velocityInGravityDirectionDt);
    }
    else
    {
      charControllerRecord->m_velocityInGravityDirection.setToZero();
    }

    // if stepping down then zero the velocity so that we run off cliff edges starting horizontal.
    if (m_characterControllerManager->getOnGround(instanceID))
      charControllerRecord->m_velocityInGravityDirection.setToZero();
  }

  charControllerRecord->m_deltaTranslation = actualTranslation;
  NMP::Quat oldOrientInv(charControllerRecord->m_characterOrientationOld);
  oldOrientInv.conjugate();
  charControllerRecord->m_deltaTranslation = oldOrientInv.rotateVector(charControllerRecord->m_deltaTranslation);

  // Set the character controller's position to the adjusted vector
  charControllerRecord->m_network->updateCharacterPropertiesWorldRootTransform(
    NMP::Matrix34(charControllerRecord->m_characterOrientation, charControllerRecord->m_characterPosition),
    true);
}

//----------------------------------------------------------------------------------------------------------------------
IControllerMgr* DefaultPhysicsMgr::getControllerManager()
{
  return m_characterControllerManager;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePhysicalSceneObjects()
{
  const MCOMMS::SceneObjectManagementInterface* const sceneObjectManager =
    MCOMMS::getRuntimeTarget()->getSceneObjectManager();
  NMP_ASSERT(sceneObjectManager);

  if (sceneObjectManager)
  {
    std::vector<PhysXActor*>& sceneActors = getSceneActors();
    size_t nActors = sceneActors.size();

    for (size_t i = 0 ; i < nActors ; ++i)
    {
      PhysXActor* actor = sceneActors[i];
      if (!actor)
        continue;

      if (!actor->userData)
        continue;

      PhysicsUserData* userData = (PhysicsUserData*)actor->userData;
      if (!userData->isValid())
        continue;

      const MCOMMS::SceneObjectID objectID = userData->objectID;

      // Find the scene object.
      MCOMMS::SceneObject* const sceneObect = sceneObjectManager->getSceneObject(objectID);
      if (sceneObect)
      {
        MCOMMS::Attribute* const transformAttribute = sceneObect->getAttribute(MCOMMS::Attribute::SEMANTIC_TRANSFORM);
        if (transformAttribute)
        {
          const NMP::Matrix34 transform = MR::nmNxMat34ToNmMatrix34(actor->getGlobalPose());
          *(NMP::Matrix34*)transformAttribute->getData() = transform;
        }
      }
    }
  }
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
void DefaultPhysicsMgr::updateControllers(float deltaTime)
{
  m_characterControllerManager->updateControllers(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
PhysXActor* DefaultPhysicsMgr::getActorByPhysicsID(MR::PhysicsObjectID id) const
{
  PhysXActor* actor = 0;
  m_physicsIDActorMap.find(id, &actor);
  return actor;
}


//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::getPhysicsIDForPart(const MR::PhysicsRig::Part* part) const
{
  const MR::PhysicsRigPhysX2::PartPhysX* physxPart = static_cast<const MR::PhysicsRigPhysX2::PartPhysX*>(part);
  return getPhysicsIDForActor(physxPart->getActor());
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::getPhysicsIDForActor(const PhysXActor* actor) const
{
  MR::PhysicsObjectID id = MR_INVALID_PHYSICS_OBJECT_ID;
  m_actorPhysicsIDMap.find(const_cast<PhysXActor*>(actor), &id);
  return id;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::assignPhysicsIDToActor(PhysXActor* actor)
{
  NMP_ASSERT(actor != 0);

  bool found = m_actorPhysicsIDMap.find(actor);
  if (found)
  {
    // already added to map
    return false;
  }

  // get a free id for this object
  MR::PhysicsObjectID actorID = m_nextPhysicsObjectID++;

  bool result = m_physicsIDActorMap.insert(actorID, actor);

  if (!result)
  {
    // insertion failed
    return MR_INVALID_PHYSICS_OBJECT_ID;
  }

  result = m_actorPhysicsIDMap.insert(actor, actorID);

  if (!result)
  {
    // insertion failed, remove the entry from m_physicsIdActorMap so it still contains the same
    // objects as m_actorPhysicsIdMap.
    m_physicsIDActorMap.erase(actorID);
    return MR_INVALID_PHYSICS_OBJECT_ID;
  }

  return actorID;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::unassignPhysicsID(PhysXActor* actor)
{
  NMP_ASSERT(actor != 0);

  MR::PhysicsObjectID actorId = MR_INVALID_PHYSICS_OBJECT_ID;

  bool found = m_actorPhysicsIDMap.find(actor, &actorId);
  if (!found)
  {
    // object was never given a physics object id.
    return false;
  }

  bool result = m_physicsIDActorMap.erase(actorId);
  if (!result)
  {
    // the maps got out of sync somehow
    return false;
  }

  m_actorPhysicsIDMap.erase(actor);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::clearAllPhysicsIDs()
{
  m_nextPhysicsObjectID = 0;
  m_physicsIDActorMap.clear();
  m_actorPhysicsIDMap.clear();
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
    LOG_PROFILING_MESSAGE(" | ");
  }
  if (depth)
  {
    LOG_PROFILING_MESSAGE(" |_");
  }
  else
  {
    g_maxTime = (profiler->getRecordTime(record) > g_maxTime) ? profiler->getRecordTime(record): g_maxTime;
    g_totalTiming += profiler->getRecordTime(record);
    LOG_PROFILING_MESSAGE("Average time: %6.3f ms\n", g_totalTiming/++g_totalSamples);
    LOG_PROFILING_MESSAGE("Peak time: %6.3f ms\n", g_maxTime);
  }
  LOG_PROFILING_MESSAGE("%6.3f ms - %s\n", profiler->getRecordTime(record), profiler->getRecordBlock(record)->getTag());
}

// -----------------------------------------------------------------------------
void gatherProfilingData(NMP::Profiler* profiler)
{
  const size_t threadCount = profiler->getNumThreads();
  for (size_t threadIndex = 0; threadIndex != threadCount; ++threadIndex)
  {
    LOG_PROFILING_MESSAGE("=================================================\n");
    LOG_PROFILING_MESSAGE("=================================================\n");

    const NMP::Profiler::ProfilerRecord* rootRecord = profiler->getRootRecord(threadIndex);

    if (rootRecord)
    {
      recursiveTraverseRecords(profiler, rootRecord, 0, &printRecord);
    }
   LOG_PROFILING_MESSAGE("=================================================\n\n");
  }
}
#endif //NM_PROFILING
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::update(float deltaTime)
{
  // A physics scene and character controller manager 
  if (!m_physicsScene || !m_characterControllerManager)
  {
    return;
  }

  NM_BEGIN_PROFILING("------- Update Root -------");

  // Update all instances
  m_context->getNetworkInstanceManager()->applyAnimationSetChanges();

  PHYSICS_LOG_LINE_DIVIDE();
  NM_BEGIN_PROFILING("setNextPhysicsTimeStep");
  m_physicsScene->setNextPhysicsTimeStep(getActualTimestep(deltaTime));
  NM_END_PROFILING(); //"setNextPhysicsTimeStep"
  
  m_characterControllerManager->updateInstanceNetworksBegin(deltaTime);

  PHYSICS_LOG_MESSAGE("updateInstancesConnections");
  NM_BEGIN_PROFILING("updateInstancesConnections");
  m_characterControllerManager->updateInstancesConnections(deltaTime);
  NM_END_PROFILING(); //"updateInstancesConnections"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);

  PHYSICS_LOG_MESSAGE("updateInstancesPreController");
  NM_BEGIN_PROFILING("updateInstancesPreController");
  m_characterControllerManager->updateInstancesPreController(deltaTime);
  NM_END_PROFILING(); //"updateInstancesPreController"

  PHYSICS_LOG_MESSAGE("updating character controllers");
  NM_BEGIN_PROFILING("updateControllers");
  m_characterControllerManager->updateControllers(deltaTime);
  NM_END_PROFILING(); //"updateControllers"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS);

  PHYSICS_LOG_MESSAGE("updateInstancesPrePhysics");
  NM_BEGIN_PROFILING("updateInstancesPrePhysics");
  m_characterControllerManager->updateInstancesPrePhysics(deltaTime);
  NM_END_PROFILING(); //"updateInstancesPrePhysics"

  // update scene object pre-physics
  PHYSICS_LOG_MESSAGE("Stepping Physics");
  NM_BEGIN_PROFILING("simulatePhysics");
  simulate(deltaTime);
  NM_END_PROFILING(); //"simulatePhysics"

  m_characterControllerManager->updateInstancesPostPhysicsInit(deltaTime);
  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

  PHYSICS_LOG_MESSAGE("updateInstancesPostPhysics");
  NM_BEGIN_PROFILING("updateInstancesPostPhysics");
  m_characterControllerManager->updateInstancesPostPhysics(deltaTime);
  NM_END_PROFILING(); //"updateInstancesPostPhysics"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::TASK_ID_UNSPECIFIED);
  // we don't expect any external tasks, or any physics related tasks, to be left here
  m_characterControllerManager->updateInstanceNetworksEnd();

  PHYSICS_LOG_MESSAGE("updateSceneObjects");

  // update the physics objects
  NM_BEGIN_PROFILING("updateSceneObjectsFromPhysics");
  updatePhysicalSceneObjects();
  NM_END_PROFILING(); //"updateSceneObjectsFromPhysics"

  // Update all objects
  NM_BEGIN_PROFILING("updateSceneObjects");
  updateSceneObjects(deltaTime);
  NM_END_PROFILING(); //"updateSceneObjects"

  // Update the Connect-side representations of the character controllers
  m_characterControllerManager->updateControllerRepresentations();

  NM_END_PROFILING(); //"------- Update Root -------"

#ifdef MR_OUTPUT_DEBUGGING
  m_characterControllerManager->sendInstanceProfileTimingDebugOutput();
#endif // MR_OUTPUT_DEBUGGING

#ifdef NM_PROFILING
  NMP::Profiler::getProfiler()->endProfilingFrame();
  gatherProfilingData(NMP::Profiler::getProfiler());
  NMP::Profiler::getProfiler()->reset();
#endif // NM_PROFILING

  ++m_frameIndex;

  return;
}

namespace
{

//----------------------------------------------------------------------------------------------------------------------
void incPhysicsAssetRefCounts(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (MR::AnimSetIndex animSetIdx = 0; animSetIdx < numAnimSets; ++animSetIdx)
  {
    // Physics Rigs.
    MR::PhysicsRigDef* physicsRig = getPhysicsRigDef(networkDef, animSetIdx);
    if (physicsRig)
    {
      uint32_t refCount = MR::Manager::incObjectRefCount(physicsRig);
      NM_LOG_MESSAGE(
        RTT_LOGGER,
        RTT_MESSAGE_PRIORITY,
        "  Increasing RefCount - PhysicsRigDef AssetID: %p, RefCount: %d\n",
        MR::Manager::getInstance().getObjectIDFromObjectPtr(physicsRig),
        refCount);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void decPhysicsAssetRefCounts(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (MR::AnimSetIndex animSetIdx = 0; animSetIdx < numAnimSets; ++animSetIdx)
  {
    // Physics Rigs.
    MR::PhysicsRigDef* physicsRig = getPhysicsRigDef(networkDef, animSetIdx);
    if (physicsRig)
    {
      uint32_t refCount = MR::Manager::decObjectRefCount(physicsRig);
      NM_LOG_MESSAGE(
        RTT_LOGGER,
        RTT_MESSAGE_PRIORITY,
        "    Decreasing RefCount - PhysicsRigDef AssetID: %p, RefCount: %d\n",
        MR::Manager::getInstance().getObjectIDFromObjectPtr(physicsRig),
        refCount);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void deleteOrphanedPhysicsAsset(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (uint16_t animSetIndex = 0 ; animSetIndex < numAnimSets ; ++animSetIndex)
  {
    //----------
    // Physics Rig.
    MR::PhysicsRigDef* physicsRig = getPhysicsRigDef(networkDef, animSetIndex);
    if (physicsRig && (MR::Manager::getInstance().getObjectRefCount(physicsRig) == 0))
    {
      NM_LOG_MESSAGE(
        RTT_LOGGER,
        RTT_MESSAGE_PRIORITY,
        "  Deleting referenced PhysicsRigDef - AssetID: %p.\n",
        MR::Manager::getInstance().getObjectIDFromObjectPtr(physicsRig));
      MR::Manager::getInstance().unregisterObject(physicsRig);
      NMP::Memory::memFree(physicsRig);
    }
  }
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
