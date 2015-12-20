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
#ifndef MCOMMS_SIMPLEANIMBROWSERMANAGER_H
#define MCOMMS_SIMPLEANIMBROWSERMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"
#include "simpleBundleUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class PrioritiesLogger;
}

namespace MR
{
class AnimationSourceHandle;
}

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

#if defined(MR_ATTRIB_DEBUG_BUFFERING)
//----------------------------------------------------------------------------------------------------------------------
/// \class SimpleAnimBrowserManager
/// \brief Implements AnimationBrowserInterface.
/// \ingroup MorphemeComms
///
/// A simple implementation of the animation browser management interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
//----------------------------------------------------------------------------------------------------------------------
class SimpleAnimBrowserManager :
  public AnimationBrowserInterface
{
  // Construction/destruction
public:

  SimpleAnimBrowserManager(
    uint32_t                              maxSimulataneousConncetions, 
    NMP::PrioritiesLogger&                logger,
    int32_t                               loggerPriority,
    IPluginValidator*                     validatePluginList);

  virtual ~SimpleAnimBrowserManager();

  // [AnimationBrowserInterface]
public:

  virtual bool canLoadAnimSource() const NM_OVERRIDE { return true; }

  virtual bool canProvideAnimationBrowserDataSource(AnimationDataSources NMP_UNUSED(dataSource)) const NM_OVERRIDE { return true; }

  virtual bool setAnimationBrowserAnim(const char* animBrowserAnim, Connection* connection) NM_OVERRIDE;
  virtual bool setAnimationBrowserNetwork(
    const char* animBrowserNetwork,
    NetworkDescriptor animBrowserNetworkDesc,
    Connection* connection) NM_OVERRIDE;

  virtual bool loadAnimationBrowserData(const char* compressionType, Connection* connection) NM_OVERRIDE;

  virtual float setAnimationBrowserTime(float time, Connection* connection) NM_OVERRIDE;
  virtual float getAnimationBrowserTime(Connection* connection) NM_OVERRIDE;
  virtual float getAnimationBrowserDuration(Connection* connection) NM_OVERRIDE;

  virtual uint32_t getAnimationBrowserTransformDataSourceSize(
    AnimationDataSources dataSource,
    Connection* connection) NM_OVERRIDE;

  virtual bool serializeAnimationBrowserDataSource(
    AnimationDataSources dataSource,
    MR::AttribTransformBufferOutputData* transformData,
    Connection* connection) NM_OVERRIDE;

  virtual bool broadcastAnimationBrowserMessage(const MR::Message& message, Connection* connection) NM_OVERRIDE;

  virtual bool sendAnimationBrowserMessage(commsNodeID nodeID, const MR::Message& message, Connection* connection) NM_OVERRIDE;
  virtual bool setAnimationBrowserActiveAnimationSet(MR::AnimSetIndex setIndex, Connection* connection) NM_OVERRIDE;

  virtual bool setAnimationBrowserControlParameter(
    commsNodeID            nodeID,
    MR::NodeOutputDataType type,
    const void*            cparamData,
    Connection* connection) NM_OVERRIDE;

  virtual void onConnectionClosed(Connection* connection) NM_OVERRIDE;

private:

  class ConnectionData;

  ConnectionData**        m_connectionData;
  const uint32_t          m_maxConnections;
  uint32_t                m_numConnections;

  NMP::PrioritiesLogger&  m_logger;
  const int32_t           m_loggerPriority;
  IPluginValidator*       m_validatePluginList;

  SimpleAnimBrowserManager(const SimpleAnimBrowserManager&);
  SimpleAnimBrowserManager& operator=(const SimpleAnimBrowserManager&);

  ConnectionData* findConnectionData(Connection* connection);
  bool addConnection(ConnectionData *& connectionData, Connection* connection);
  void removeConnection(Connection* connection);
};

#endif // defined(MR_ATTRIB_DEBUG_BUFFERING)

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_SIMPLEANIMBROWSERMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
