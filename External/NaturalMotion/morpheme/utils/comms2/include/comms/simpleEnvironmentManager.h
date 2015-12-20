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
#ifndef MCOMMS_SIMPLEENVIRONMENTMANAGER_H
#define MCOMMS_SIMPLEENVIRONMENTMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMVectorContainer.h"

#include "comms/runtimeTargetInterface.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
/// \class SimpleEnvironmentManager
/// \brief Implements EnvironmentManagementInterface.
/// \ingroup MorphemeComms
///
/// A simple implementation of the environment management interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
///
/// This implementation simply keeps a list of environment attributes up to the maximum specified in the constructor,
/// how these attributes are used by the runtime is up to the specific implementation. Morpheme:connect only writes to
/// but never reads from these attributes as such they inform the runtime of preferences set in connect - the gravity
/// setting for example - but does not mandate their use.
//----------------------------------------------------------------------------------------------------------------------
class SimpleEnvironmentManager :
  public EnvironmentManagementInterface
{
  // Construction/destruction
public:

  SimpleEnvironmentManager(uint32_t maxEnvironmentAttributes);
  virtual ~SimpleEnvironmentManager();

  // [EnvironmentManagementInterface]
public:

  virtual bool canEditEnvironmentAttributes() const NM_OVERRIDE { return true; }
  virtual bool setEnvironmentAttribute(const Attribute* attribute) NM_OVERRIDE;

public:

  virtual uint32_t getEnvironmentNumAttributes() NM_OVERRIDE;

  virtual Attribute* getEnvironmentAttribute(uint32_t index) NM_OVERRIDE;
  virtual const Attribute* getEnvironmentAttribute(uint32_t index) const NM_OVERRIDE;

  virtual Attribute* getEnvironmentAttribute(Attribute::Semantic semantic) NM_OVERRIDE;
  virtual const Attribute* getEnvironmentAttribute(Attribute::Semantic semantic) const NM_OVERRIDE;

private:

  NMP::HeapAllocator                m_heapAllocator;
  NMP::VectorContainer<Attribute*>* m_environmentAttributes;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_SIMPLEENVIRONMENTMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
