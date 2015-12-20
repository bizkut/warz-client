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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_ERLIVELINKDATA_H
#define NM_ERLIVELINKDATA_H
//----------------------------------------------------------------------------------------------------------------------
#include "networkDefRecordManager.h"
#include "networkInstanceRecordManager.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erCharacterDef.h"
#include "euphoria/erNetworkInterface.h"
#include "euphoria/erInteractionProxy.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
class Limb;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class ERNetworkDefRecord
/// \brief Extends NetworkDefRecord class to encompass character definition information
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class ERNetworkDefRecord : public NetworkDefRecord
{
  friend class ERNetworkInstanceRecord;
public:
  ERNetworkDefRecord(
    const MCOMMS::GUID&       guid,
    const char*               name,
    MR::NetworkDef*           networkDef,
    MR::NetworkPredictionDef* netPredictionDef);

  virtual ~ERNetworkDefRecord();

protected:
  ER::CharacterDef m_characterDef;
  ER::NetworkInterface* m_behaviourLibrary;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ERNetworkInstanceRecord
/// \brief Extends NetworkInstanceRecord class to encompass character definition information
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class ERNetworkInstanceRecord : public NetworkInstanceRecord
{
public:
  ERNetworkInstanceRecord(
    MCOMMS::InstanceID id,
    NetworkDefRecord* networkDefRecord,
    MCOMMS::Connection* owner,
    const char* instanceName);

  virtual ~ERNetworkInstanceRecord();

  ER::Character* getCharacter() { return m_character; }
  const ER::Character* getCharacter()const { return m_character; }

  virtual bool init(
    uint32_t            animSetIndex,
    const NMP::Vector3& characterStartPosition = NMP::Vector3::InitZero,
    const NMP::Quat&    characterStartRotation = NMP::Quat::kIdentity,
    bool                stepNetwork = false) NM_OVERRIDE;

  void term();

  virtual void updatePrePhysics(float deltaTime);

  virtual void updatePostPhysics(float deltaTime);

  virtual void updateInteractionProxy(float deltaTime);

  virtual void destroyCharacter();

protected:
  /// \brief Add all known modules to the debug interface with initial debug draw status set to false.
  void initDebugInterfaceModuleNames();

  /// \brief Add all known controls to the debug interface with initial debug draw status set to false.
  void initDebugInterfaceControlNames();

  ER::Character *m_character;
  ER::InteractionProxy m_interactionProxy;
  bool m_useProxyCharacter;
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_ERLIVELINKDATA_H
//----------------------------------------------------------------------------------------------------------------------
