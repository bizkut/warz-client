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
#ifndef GAME_ANIM_NETWORK_H
#define GAME_ANIM_NETWORK_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameAnimNetworkDef.h"
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

class GameCharacterControllerBase;

//----------------------------------------------------------------------------------------------------------------------
/// \class GameAnimNetwork
//----------------------------------------------------------------------------------------------------------------------
class GameAnimNetwork
{
public:

  /// \brief Creates a network from the given network definition.
  /// \see runInitializingUpdateStep()
  static GameAnimNetwork* create(
    GameAnimNetworkDef*           networkDef,
    MR::AnimSetIndex              initialAnimSetIndex,
    GameCharacterControllerBase*  characterController,
    NMP::FastHeapAllocator*       tempAllocator = 0);

  virtual ~GameAnimNetwork();

public:

  /// \brief Returns the network definition this instance is derived from.
  GameAnimNetworkDef* getNetworkDef() const { return m_networkDef; }

  /// \brief Returns the morpheme network instance used.
  MR::Network* getNetwork() const { return m_network; }

  /// \brief Returns the pose as evaluated by the most recent network update and the rig it was created for.
  /// \see update()
  const NMP::DataBuffer* getPose(const MR::AnimRigDef *&animRig) const { animRig = m_animRig; return m_pose; }

  /// \see MR::Network::setActiveAnimSetIndex()
  bool setActiveAnimSetIndex(MR::AnimSetIndex animationSetIndex);

  /// \see MR::Network::broadcastMessage()
  bool broadcastRequestMessage(MR::MessageID messageID, bool status);

  /// \see MR::Network::setControlParameter()
  void setControlParameter(MR::NodeID owningNodeID, float value);

  /// \see Updates the world space pose from the local space network output.
  /// \see getPose()
  void updatePose(const NMP::Matrix34& rootTransform);

  /// \brief Returns the translation change as evaluated by the most recent network update.
  /// \see update()
  NMP::Vector3 getTranslationChange() const { return m_network->getTranslationChange(); }

  /// \brief Returns the orientation change as evaluated by the most recent network update.
  /// \see update()
  NMP::Quat getOrientationChange() const { return m_network->getOrientationChange(); }

  // \brief Runs an update step to initialize the network.
  // The initial update with a delta time of zero will initialize active connections etc. It is important that the
  // initial active animation set is set before the first update so the resulting transform output matches the active
  // rig. This should be called exactly once after the network was created.
  void runInitializingUpdateStep();

private:

  GameAnimNetworkDef*       m_networkDef;

  MR::Network*              m_network;
  NMP::DataBuffer*          m_pose;
  MR::AnimRigDef*           m_animRig;

  NMP::FastHeapAllocator*   m_temporaryMemoryAllocator;
  NMP::MemoryAllocator*     m_persistentMemoryAllocator;

  GameAnimNetwork(GameAnimNetworkDef* networkDef);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_ANIM_NETWORK_H
//----------------------------------------------------------------------------------------------------------------------
