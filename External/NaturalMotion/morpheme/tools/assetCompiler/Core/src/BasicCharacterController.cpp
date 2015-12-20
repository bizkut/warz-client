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
#include "BasicCharacterController.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BasicCharacterController::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BasicCharacterController), NMP_VECTOR_ALIGNMENT);
  
  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
BasicCharacterController* BasicCharacterController::init(
  NMP::Memory::Resource& resource,
  const NMP::Vector3&    characterStartPosition,
  const NMP::Quat&       characterStartRotation)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(BasicCharacterController), NMP_VECTOR_ALIGNMENT);
  BasicCharacterController* result = (BasicCharacterController*)resource.alignAndIncrement(memReqsHdr);
  
  // Call the constructor in-place to setup the vtable.
  new(result) BasicCharacterController(characterStartPosition, characterStartRotation);
  
  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
BasicCharacterController* BasicCharacterController::createAndInit(
  const NMP::Vector3& characterStartPosition,
  const NMP::Quat&    characterStartRotation)
{
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(getMemoryRequirements());
  NMP_ASSERT(resource.ptr);
  return init(resource, characterStartPosition, characterStartRotation);
}

//----------------------------------------------------------------------------------------------------------------------
void BasicCharacterController::init(
  const NMP::Vector3& characterStartPosition,
  const NMP::Quat&    characterStartRotation)
{
  m_requestedMovement.setToZero();
  m_characterPosition = characterStartPosition;
  m_characterPositionOld = characterStartPosition;
  m_characterOrientation = characterStartRotation;
  m_characterOrientationOld = characterStartRotation;
  m_deltaTranslation.setToZero();
  m_deltaOrientation.identity();
}

//----------------------------------------------------------------------------------------------------------------------
void BasicCharacterController::updateController(
  const NMP::Vector3& deltaTranslation,
  const NMP::Quat&    deltaOrientation)
{
  //----------------------------
  // Orientate the movement and update the orientation
  m_requestedMovement = m_characterOrientation.rotateVector(deltaTranslation);
  m_characterOrientationOld = m_characterOrientation;
  m_characterOrientation *= deltaOrientation;
  m_characterOrientation.normalise();

  //----------------------------
  // Update the character position
  m_characterPositionOld = m_characterPosition;
  m_characterPosition += m_requestedMovement;  
}

//----------------------------------------------------------------------------------------------------------------------
bool BasicCharacterController::setRequestedPropertyOverride(
  MR::CCPropertyType     NMP_UNUSED(propertyType),
  MR::AttribData*        NMP_UNUSED(property),
  MR::CCOverridePriority NMP_UNUSED(priority),
  MR::FrameCount         NMP_UNUSED(frameIndex))
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* BasicCharacterController::getRequestedPropertyOverride(
  MR::CCPropertyType NMP_UNUSED(propertyType),
  MR::FrameCount     NMP_UNUSED(frameIndex)) const
{
  return NULL;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
