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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MCOMMS_RUNTIMETARGETNULL_H
#define MCOMMS_RUNTIMETARGETNULL_H
//----------------------------------------------------------------------------------------------------------------------
#include "runtimeTargetInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
/// \class ConnectionManagementNull
/// \brief Implements ConnectionManagementInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the connection manager interface. Documentation for individual methods has been omitted for
/// brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class ConnectionManagementNull :
  public ConnectionManagementInterface
{
  // Construction/destruction
public:

  ConnectionManagementNull() : ConnectionManagementInterface() {}
  virtual ~ConnectionManagementNull() {}

  // [ConnectionManagementInterface]
public:

  /// Nothing to be done.
  virtual void onNewConnection(Connection* NMP_UNUSED(connection)) NM_OVERRIDE {}

  /// Nothing to be done.
  virtual void onConnectionClosed(Connection* NMP_UNUSED(connection)) NM_OVERRIDE {}

  /// Nothing to be done.
  virtual void startDebuggingNetworkInstance(Connection* NMP_UNUSED(connection), InstanceID NMP_UNUSED(id)) NM_OVERRIDE {}

  /// Nothing to be done.
  virtual void stopDebuggingNetworkInstance(Connection* NMP_UNUSED(connection), InstanceID NMP_UNUSED(id)) NM_OVERRIDE {}
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NetworkManagementNull
/// \brief Implements NetworkManagementInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the network manager interface. Documentation for individual methods has been omitted for
/// brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class NetworkManagementNull :
  public NetworkManagementInterface
{
  // Construction/destruction
public:

  NetworkManagementNull() : NetworkManagementInterface() {}
  virtual ~NetworkManagementNull() {}

  // [NetworkManagementInterface]
public:

  /// No network definitions loaded, return zero.
  virtual uint32_t getNetworkDefCount() const NM_OVERRIDE
  {
    return 0;
  }

  /// No network definitions loaded, return zero.
  virtual uint32_t getNetworkDefGUIDs(GUID* NMP_UNUSED(guids), uint32_t NMP_UNUSED(maxGuids)) const NM_OVERRIDE
  {
    return 0;
  }

  /// No network definitions loaded, return false.
  virtual bool isNetworkDefinitionLoaded(const GUID& NMP_UNUSED(guid)) const NM_OVERRIDE
  {
    return false;
  }

  /// No network instances available, return zero.
  virtual uint32_t getNetworkInstanceCount() const NM_OVERRIDE
  {
    return 0;
  }

  /// No network instances available, return zero.
  virtual uint32_t getNetworkInstanceIDs(InstanceID* NMP_UNUSED(instanceIDs), uint32_t NMP_UNUSED(maxIntanceIDs)) const NM_OVERRIDE
  {
    return 0;
  }

  /// Instance ID is guaranteed to be invalid, assert and return empty string.
  virtual const char* getNetworkInstanceName(InstanceID NMP_UNUSED(id)) const NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return "";
  }

  /// Instance ID is guaranteed to be invalid, assert and return false.
  virtual bool getNetworkInstanceDefinitionGUID(InstanceID NMP_UNUSED(id), GUID& NMP_UNUSED(guid)) const NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return false;
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NetworkDefLifecycleNull
/// \brief Implements NetworkDefLifecycleInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the network definition life cycle interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class NetworkDefLifecycleNull :
  public NetworkDefLifecycleInterface
{
  // Construction/destruction
public:

  NetworkDefLifecycleNull() : NetworkDefLifecycleInterface() {}
  virtual ~NetworkDefLifecycleNull() {}

  // [NetworkDefLifecycleInterface]
public:

  /// Interface is not implemented, return false.
  virtual bool canLoadNetworkDefinition() const NM_OVERRIDE
  {
    return false;
  }

  /// Loading of network definitions is not supported, return false.
  virtual bool loadNetworkDefinition(
    const GUID& NMP_UNUSED(guid),
    const char* NMP_UNUSED(networkDefName),
    Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// GUID is guaranteed to be invalid, assert and return false.
  virtual bool destroyNetworkDefinition(const GUID& NMP_UNUSED(guid)) NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return false;
  }

  /// Nothing to clear, report success.
  virtual bool clearCachedData() NM_OVERRIDE
  {
    return true;
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NetworkLifecycleNull
/// \brief Implements NetworkLifecycleInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the network life cycle interface. Documentation for individual methods has been omitted for
/// brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class NetworkLifecycleNull :
  public NetworkLifecycleInterface
{
  // Construction/destruction
public:

  NetworkLifecycleNull() : NetworkLifecycleInterface() {}
  virtual ~NetworkLifecycleNull() {}

  // [NetworkLifecycleInterface]
public:

  /// Interface is not implemented, return false.
  virtual bool canCreateNetworkInstance() const NM_OVERRIDE
  {
    return false;
  }

  /// Creating new instances is not supported, return error.
  virtual InstanceID createNetworkInstance(
    const GUID& NMP_UNUSED(guid),
    const char* NMP_UNUSED(instanceName),
    uint32_t NMP_UNUSED(animSetIndex),
    Connection* NMP_UNUSED(connection),
    const NMP::Vector3& NMP_UNUSED(characterStartPosition),
    const NMP::Quat& NMP_UNUSED(characterStartRotation)) NM_OVERRIDE
  {
    return MCOMMS::INVALID_INSTANCE_ID;;
  }

  /// Instance ID is guaranteed to be invalid, assert and return false.
  virtual bool destroyNetworkInstance(InstanceID NMP_UNUSED(instanceId)) NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return false;
  }

  /// Nothing to be done.
  virtual void onConnectionClosed(Connection* NMP_UNUSED(connection)) NM_OVERRIDE {}
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnimationBrowserNull
/// \brief Implements AnimationBrowserInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the animation browser management interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class AnimationBrowserNull :
  public AnimationBrowserInterface
{
  // Construction/destruction
public:

  AnimationBrowserNull() : AnimationBrowserInterface() {}
  virtual ~AnimationBrowserNull() {}

  // [AnimationBrowserInterface]
public:

  /// Interface is not implemented, return false.
  virtual bool canLoadAnimSource() const NM_OVERRIDE { return false; }

  /// Interface is not implemented, return false.
  virtual bool canProvideAnimationBrowserDataSource(AnimationDataSources NMP_UNUSED(dataSource)) const NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool setAnimationBrowserAnim(const char* NMP_UNUSED(animBrowserAnim), Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool setAnimationBrowserNetwork(
    const char* NMP_UNUSED(animBrowserNetwork),
    NetworkDescriptor NMP_UNUSED(animBrowserNetworkDesc),
    Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool loadAnimationBrowserData(const char* NMP_UNUSED(compressionType), Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return zero.
  virtual float setAnimationBrowserTime(float NMP_UNUSED(time), Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return 0.0f;
  }

  /// Interface is not implemented, return zero.
  virtual float getAnimationBrowserTime(Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return 0.0f;
  }

  /// Interface is not implemented, return zero.
  virtual float getAnimationBrowserDuration(Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return 0.0f;
  }

  /// Interface is not implemented, return false.
  virtual uint32_t getAnimationBrowserTransformDataSourceSize(
    AnimationDataSources NMP_UNUSED(dataSource),
    Connection* NMP_UNUSED(connection)) NM_OVERRIDE   
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool serializeAnimationBrowserDataSource(
    AnimationDataSources NMP_UNUSED(dataSource),
    MR::AttribTransformBufferOutputData* NMP_UNUSED(data),
    Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool broadcastAnimationBrowserMessage(
    const MR::Message& NMP_UNUSED(message), 
    Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool sendAnimationBrowserMessage(
    commsNodeID NMP_UNUSED(nodeID), 
    const MR::Message& NMP_UNUSED(message),
    Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool setAnimationBrowserControlParameter(
    commsNodeID            NMP_UNUSED(nodeID),
    MR::NodeOutputDataType NMP_UNUSED(type),
    const void*            NMP_UNUSED(cparamData),
    Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool setAnimationBrowserActiveAnimationSet(
    MR::AnimSetIndex NMP_UNUSED(setIndex),
    Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  /// Nothing to be done.
  virtual void onConnectionClosed(Connection* NMP_UNUSED(connection)) NM_OVERRIDE {}
};

//----------------------------------------------------------------------------------------------------------------------
/// \class EnvironmentManagementNull
/// \brief Implements EnvironmentManagementInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the environment management interface. Documentation for individual methods has been omitted
/// for brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class EnvironmentManagementNull :
  public EnvironmentManagementInterface
{
  // Construction/destruction
public:

  EnvironmentManagementNull() : EnvironmentManagementInterface() {}
  virtual ~EnvironmentManagementNull() {}

  // [EnvironmentManagementInterface]
public:

  /// Interface is not implemented, return false.
  virtual bool canEditEnvironmentAttributes() const NM_OVERRIDE { return false; }

  /// Editing environment attributes is not supported, return false.
  virtual bool setEnvironmentAttribute(const Attribute* NMP_UNUSED(attribute)) NM_OVERRIDE
  {
    return false;
  }

public:

  /// No environment attributes available, return zero.
  virtual uint32_t getEnvironmentNumAttributes() NM_OVERRIDE
  {
    return 0;
  }

  /// Index is guaranteed to be invalid, assert and return NULL.
  virtual Attribute* getEnvironmentAttribute(uint32_t NMP_UNUSED(index)) NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return NULL;
  }

  /// Index is guaranteed to be invalid, assert and return NULL.
  virtual const Attribute* getEnvironmentAttribute(uint32_t NMP_UNUSED(index)) const NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return NULL;
  }

  /// Semantic is guaranteed to be invalid, assert and return NULL.
  virtual Attribute* getEnvironmentAttribute(Attribute::Semantic NMP_UNUSED(semantic)) NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return NULL;
  }

  /// Semantic is guaranteed to be invalid, assert and return NULL.
  virtual const Attribute* getEnvironmentAttribute(Attribute::Semantic NMP_UNUSED(semantic)) const NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return NULL;
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// \class SceneObjectManagementNull
/// \brief Implements SceneObjectManagementInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the scene object management interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class SceneObjectManagementNull :
  public SceneObjectManagementInterface
{
  // Construction/destruction
public:

  SceneObjectManagementNull() : SceneObjectManagementInterface() {}
  virtual ~SceneObjectManagementNull() {}

  // [SceneObjectManagementInterface]
public:

  /// Interface is not implemented, return false.
  virtual bool canCreateSceneObjects() const NM_OVERRIDE
  {
    return false;
  }

  /// Interface is not implemented, return false.
  virtual bool canEditSceneObjects() const NM_OVERRIDE
  {
    return false;
  }

  /// No scene objects available, return zero.
  virtual uint32_t getNumSceneObjects() const NM_OVERRIDE
  {
    return 0;
  }

  /// Adding scene objects is not supported, return NULL.
  virtual SceneObject* createSceneObject(
    uint32_t NMP_UNUSED(numAttributes),
    const Attribute* const* NMP_UNUSED(attributes),
    Connection* NMP_UNUSED(owner),
    SceneObject *&NMP_UNUSED(createdCharacterController)) NM_OVERRIDE
  {
    return NULL;
  }

  /// ID is guaranteed to be invalid, assert and return false.
  virtual bool destroySceneObject(
    SceneObjectID NMP_UNUSED(objectID),
    SceneObjectID& NMP_UNUSED(destroyedCharacterController)) NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return false;
  }

  /// No scene objects available, return zero.
  virtual uint32_t getSceneObjects(SceneObject** NMP_UNUSED(objects), uint32_t NMP_UNUSED(maxObjects)) const NM_OVERRIDE
  {
    return 0;
  }

  /// ID is guaranteed to be invalid, assert and return NULL.
  virtual SceneObject* getSceneObject(SceneObjectID NMP_UNUSED(objectID)) const NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return NULL;
  }

  /// Nothing to be done.
  virtual void onConnectionClosed(Connection* NMP_UNUSED(connection)) NM_OVERRIDE {}
};

//----------------------------------------------------------------------------------------------------------------------
/// \class PhysicsManipulationtNull
/// \brief Implements PhysicsManipulationInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the physics manipulation interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsManipulationtNull :
  public PhysicsManipulationInterface
{
   // Construction/destruction
public:
  PhysicsManipulationtNull() {}
  virtual ~PhysicsManipulationtNull() {}
public: 
  // [PhysicsManipulationInterface]

 /// Interface is not implemented, return false.
  virtual bool canCreateAndEditConstraints() const NM_OVERRIDE
  {
    return false; 
  }
  
  /// Interface is not implemented, return false.
  virtual bool canApplyForces() const NM_OVERRIDE
  {
    return false; 
  }

  /// Interface is not implemented, assert and return false.
  virtual bool createConstraint(uint64_t NMP_UNUSED(constraintGUID),
    uint32_t NMP_UNUSED(physicsEngineObjectID),
    const NMP::Vector3& NMP_UNUSED(localSpaceConstraintPosition),
    const NMP::Vector3& NMP_UNUSED(worldSpaceConstraintPosition),
    bool NMP_UNUSED(constrainOrientation),
    bool NMP_UNUSED(constrainAtCentreOfMass),
    const NMP::Vector3& NMP_UNUSED(sourceRayWSPositionHint),
    const NMP::Vector3& NMP_UNUSED(sourceRayWSDirectionHint)
    ) NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return false; 
  }

  /// Interface is not implemented, assert and return false.
  virtual bool moveConstraint(uint64_t NMP_UNUSED(constraintGUID), 
    const NMP::Vector3& NMP_UNUSED(worldSpaceConstraintPosition)) NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return false; 
  }

  /// Interface is not implemented, assert and return false.
  virtual bool removeConstraint(uint64_t NMP_UNUSED(constraintGUID)) NM_OVERRIDE
  {
    NMP_ASSERT_FAIL();
    return false; 
  }

  virtual bool applyForce(uint32_t NMP_UNUSED(physicsEngineObjectID), 
    ForceMode NMP_UNUSED(mode),
    const NMP::Vector3& NMP_UNUSED(force),
    bool NMP_UNUSED(applyAtCOM),
    const NMP::Vector3& NMP_UNUSED(localSpacePosition),
    const NMP::Vector3& NMP_UNUSED(worldSpacePosition)
    )
  {
    NMP_ASSERT_FAIL();
    return false; 
  }
};


//----------------------------------------------------------------------------------------------------------------------
/// \class PhysicsDataManagementNull
/// \brief Implements PhysicsDataManagementInterface.
/// \ingroup MorphemeComms
///
/// A null implementation of the physics data management interface. Documentation for individual methods has been omitted
/// for brevity but is available on the method declaration of the interface this class is derived from.
///
/// This implementation simply ignores all requests to serialize and transmit physics rig data.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsDataManagementNull : public PhysicsDataManagementInterface
{
  // Construction/destruction
public:

  PhysicsDataManagementNull() : PhysicsDataManagementInterface() {}
  /// this constructor is used when this class is instantiated in the no physics runtime target versions of morpheme.
  PhysicsDataManagementNull(void*, void*) : PhysicsDataManagementInterface() {}
  virtual ~PhysicsDataManagementNull() {}

  // [PhysicsDataManagementInterface]
public:
  virtual void getPhysicsEngineID(char* buffer, uint32_t bufferLength) const NM_OVERRIDE
  {
    NMP_STRNCPY_S(buffer, bufferLength, "NoPhysics");
  }

  virtual MR::AnimSetIndex getPhysicsRigDefCount(const GUID& NMP_UNUSED(guid)) NM_OVERRIDE
  {
    return 0;
  }

  virtual bool serializeTxPhysicsRigDefData(const GUID& NMP_UNUSED(guid), MR::AnimSetIndex NMP_UNUSED(index), Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  virtual bool serializeTxPhysicsRigPersistentData(InstanceID NMP_UNUSED(id), uint32_t NMP_UNUSED(index),Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  virtual bool serializeTxPhysicsRigFrameData(InstanceID NMP_UNUSED(id), Connection* NMP_UNUSED(connection)) NM_OVERRIDE
  {
    return false;
  }

  virtual bool serializeTxPresetGroupTagsPersistentData(InstanceID NMP_UNUSED(id), Connection* NMP_UNUSED(connection))
  {
    return false;
  }

  virtual void onEnvironmentVariableChange(const Attribute* NMP_UNUSED(attribute)) NM_OVERRIDE
  {
  }
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_RUNTIMETARGETNULL_H
//----------------------------------------------------------------------------------------------------------------------
