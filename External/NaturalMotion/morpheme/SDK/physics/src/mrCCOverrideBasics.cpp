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
#include "physics/mrCCOverrideBasics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void CCOverrideBasics::init()
{
  m_horizontalScaleOverride.m_value = 1.0f;
  m_horizontalScaleOverrideStatus.m_lastFrameSet = MR::NOT_FRAME_UPDATED;
  m_defaultHorizontalScale = 1.0f;
  m_currentHorizontalScale = 1.0f;

  m_verticalScaleOverride.m_value = 1.0f;
  m_verticalScaleOverrideStatus.m_lastFrameSet = MR::NOT_FRAME_UPDATED;
  m_defaultVerticalScale = 1.0f;
  m_currentVerticalScale = 1.0f;

  m_verticalMoveStateOverride.m_value = MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY;
  m_verticalMoveStateOverrideStatus.m_lastFrameSet = MR::NOT_FRAME_UPDATED;
  m_defaultVerticalMoveState = MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY;
  m_currentVerticalMoveState = MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY;

  m_positionOffsetOverride.m_value.set(0.0f);
  m_positionOffsetOverrideStatus.m_lastFrameSet = MR::NOT_FRAME_UPDATED;
  m_defaultPositionOffset.set(0.0f);
  m_currentPositionOffset.set(0.0f);

  m_positionAbsOverride.m_value.set(0.0f);
  m_positionAbsOverrideStatus.m_lastFrameSet = MR::NOT_FRAME_UPDATED;
  m_defaultPositionAbs.set(0.0f);
  m_currentPositionAbs.set(0.0f);
  m_overrideAbsPosition = false;
}

//----------------------------------------------------------------------------------------------------------------------
bool CCOverrideBasics::setRequestedPropertyOverride(
  CCPropertyType     propertyType,
  AttribData*        property,
  CCOverridePriority priority,
  FrameCount         frameIndex)
{
  switch (propertyType)
  {
    case CC_PROPERTY_TYPE_HORIZONTAL_SCALE:
    {
      if (m_horizontalScaleOverrideStatus.canBeUpdated(frameIndex, priority))
      {
        m_horizontalScaleOverrideStatus.update(frameIndex, priority);
        NMP_ASSERT(property->getType() == ATTRIB_TYPE_FLOAT);
        AttribDataFloat* propertyAttrib = static_cast<AttribDataFloat*>(property);
        m_horizontalScaleOverride.m_value = propertyAttrib->m_value;
        return true;
      }
      break;
    }
    case CC_PROPERTY_TYPE_VERTICAL_SCALE:
    {
      if (m_verticalScaleOverrideStatus.canBeUpdated(frameIndex, priority))
      {
        m_verticalScaleOverrideStatus.update(frameIndex, priority);
        NMP_ASSERT(property->getType() == ATTRIB_TYPE_FLOAT);
        AttribDataFloat* propertyAttrib = static_cast<AttribDataFloat*>(property);
        m_verticalScaleOverride.m_value = propertyAttrib->m_value;
        return true;
      }
      break;
    }
    case CC_PROPERTY_TYPE_VERTICAL_MOVEMENT_STATE:
    {
      if (m_verticalMoveStateOverrideStatus.canBeUpdated(frameIndex, priority))
      {
        m_verticalMoveStateOverrideStatus.update(frameIndex, priority);
        NMP_ASSERT(property->getType() == ATTRIB_TYPE_UINT);
        AttribDataUInt* propertyAttrib = static_cast<AttribDataUInt*>(property);
        m_verticalMoveStateOverride.m_value = propertyAttrib->m_value;
        return true;
      }
      break;
    }
    case CC_PROPERTY_TYPE_POSITION_OFFSET:
    {
      if (m_positionOffsetOverrideStatus.canBeUpdated(frameIndex, priority))
      {
        m_positionOffsetOverrideStatus.update(frameIndex, priority);
        NMP_ASSERT(property->getType() == ATTRIB_TYPE_VECTOR3);
        AttribDataVector3* propertyAttrib = static_cast<AttribDataVector3*>(property);
        m_positionOffsetOverride.m_value = propertyAttrib->m_value;
        return true;
      }
      break;
    }
    case CC_PROPERTY_TYPE_POSITION_ABSOLUTE:
    {
      if (m_positionAbsOverrideStatus.canBeUpdated(frameIndex, priority))
      {
        m_positionAbsOverrideStatus.update(frameIndex, priority);
        NMP_ASSERT(property->getType() == ATTRIB_TYPE_VECTOR3);
        AttribDataVector3* propertyAttrib = static_cast<AttribDataVector3*>(property);
        m_positionAbsOverride.m_value = propertyAttrib->m_value;
        return true;
      }
      break;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const AttribData* CCOverrideBasics::getRequestedPropertyOverride(
  MR::CCPropertyType propertyType,
  FrameCount         frameIndex) const
{
  switch (propertyType)
  {
    case CC_PROPERTY_TYPE_HORIZONTAL_SCALE:
    {
      if (frameIndex == m_horizontalScaleOverrideStatus.m_lastFrameSet)
        return &m_horizontalScaleOverride;
      break;
    }
    case CC_PROPERTY_TYPE_VERTICAL_SCALE:
    {
      if (frameIndex == m_verticalScaleOverrideStatus.m_lastFrameSet)
        return &m_verticalScaleOverride;
      break;
    }
    case CC_PROPERTY_TYPE_VERTICAL_MOVEMENT_STATE:
    {
      if (frameIndex == m_verticalMoveStateOverrideStatus.m_lastFrameSet)
        return &m_verticalMoveStateOverride;
      break;
    }
    case CC_PROPERTY_TYPE_POSITION_OFFSET:
    {
      if (frameIndex == m_positionOffsetOverrideStatus.m_lastFrameSet)
        return &m_positionOffsetOverride;
      break;
    }
    case CC_PROPERTY_TYPE_POSITION_ABSOLUTE:
    {
      if (frameIndex == m_positionAbsOverrideStatus.m_lastFrameSet)
        return &m_positionAbsOverride;
      break;
    }
  }

  return NULL;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
