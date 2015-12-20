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
#include "comms/simpleEnvironmentManager.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
// SimpleEnvironmentManager
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
SimpleEnvironmentManager::SimpleEnvironmentManager(uint32_t maxEnvironmentAttributes) :
  EnvironmentManagementInterface(),
  m_heapAllocator(),
  m_environmentAttributes(NULL)
{
  m_environmentAttributes = NMP::VectorContainer<Attribute*>::create(maxEnvironmentAttributes, &m_heapAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
SimpleEnvironmentManager::~SimpleEnvironmentManager()
{
  // Free attributes individually (we own the attribute and the data, see setEnvironmentAttribute())
  const NMP::VectorContainer<Attribute*>::iterator end = m_environmentAttributes->end();
  NMP::VectorContainer<Attribute*>::iterator attrib = m_environmentAttributes->begin();

  for (; attrib != end; ++attrib)
  {
    Attribute* const attribute = (*attrib);

    NMP::Memory::memFree(attribute->getData());
    Attribute::destroy(attribute);
  }

  // Free the attribute list
  m_heapAllocator.memFree(m_environmentAttributes);
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleEnvironmentManager::setEnvironmentAttribute(const Attribute* attribute)
{
  const Attribute::Semantic semantic = attribute->getSemantic();
  Attribute* const existingAttribute = getEnvironmentAttribute(semantic);

  // Create a new attribute for the given semantic if it does not already exist
  if (!existingAttribute)
  {
    // Check if space is available in the list first ...
    if (!m_environmentAttributes->push_back(NULL))
    {
      return false;
    }

    // ... then create local copy of the attribute and store in the list.
    Attribute* newAttribute = Attribute::create(attribute->getDescriptor(), attribute->getData());
    m_environmentAttributes->back() = newAttribute;
  }
  else
  {
    existingAttribute->copyDataFrom(attribute);
  }

  getRuntimeTarget()->getPhysicsDataManager()->onEnvironmentVariableChange(attribute);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SimpleEnvironmentManager::getEnvironmentNumAttributes()
{
  return m_environmentAttributes->size();
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* SimpleEnvironmentManager::getEnvironmentAttribute(uint32_t index)
{
  const NMP::VectorContainer<Attribute*>::iterator end = m_environmentAttributes->end();
  NMP::VectorContainer<Attribute*>::iterator attrib = m_environmentAttributes->begin();

  for (uint32_t i = 0; attrib != end; ++i, ++attrib)
  {
    if (i == index)
    {
      return (*attrib);
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const Attribute* SimpleEnvironmentManager::getEnvironmentAttribute(uint32_t index) const
{
  const NMP::VectorContainer<Attribute*>::iterator end = m_environmentAttributes->end();
  NMP::VectorContainer<Attribute*>::iterator attrib = m_environmentAttributes->begin();

  for (uint32_t i = 0; attrib != end; ++i, ++attrib)
  {
    if (i == index)
    {
      return (*attrib);
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* SimpleEnvironmentManager::getEnvironmentAttribute(Attribute::Semantic semantic)
{
  const NMP::VectorContainer<Attribute*>::iterator end = m_environmentAttributes->end();
  NMP::VectorContainer<Attribute*>::iterator attrib = m_environmentAttributes->begin();

  for (uint32_t i = 0; attrib != end; ++i, ++attrib)
  {
    Attribute* const attribute = (*attrib);

    if (attribute->getSemantic() == semantic)
    {
      return attribute;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const Attribute* SimpleEnvironmentManager::getEnvironmentAttribute(Attribute::Semantic semantic) const
{
  const NMP::VectorContainer<Attribute*>::iterator end = m_environmentAttributes->end();
  NMP::VectorContainer<Attribute*>::iterator attrib = m_environmentAttributes->begin();

  for (uint32_t i = 0; attrib != end; ++i, ++attrib)
  {
    Attribute* const attribute = (*attrib);

    if (attribute->getSemantic() == semantic)
    {
      return attribute;
    }
  }

  return NULL;
}

} // namespace COMMS
