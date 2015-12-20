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
# pragma once
#endif
#ifndef MR_DEBUG_MACROS_H
#define MR_DEBUG_MACROS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrDebugManager.h"
#include "morpheme/mrInstanceDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
/// morpheme targeted version of scratch pad communication.
/// \var _coreDebugInterface The debug interface that this data is to be sent via.
/// \var _nodeID The ID of the Node that is originating this data. If the data is not specific to a node use INVALID_NODE_ID.
/// \var _tag An arbitrary string tag that will be displayed in connect.
/// \var _value The value to display.
//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
#define MR_LOG_SCRATCH_PAD_INTERNAL(_instanceDebugInterface, _logFunction, _nodeID, _tag, _value) \
  if (_instanceDebugInterface) \
  { \
    if ((_instanceDebugInterface)->debugOutputsAreOn(MR::DEBUG_OUTPUT_SCRATCH_PAD)) \
    { \
      (_instanceDebugInterface)->_logFunction(_nodeID, _tag, _value, #_value, (uint16_t)__LINE__); \
    } \
  }

#else // defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
#define MR_LOG_SCRATCH_PAD_INTERNAL(...)

#endif // defined(MR_OUTPUT_DEBUGGING)


//----------------------------------------------------------------------------------------------------------------------
#define MR_LOG_SCRATCH_PAD_INT(_instanceDebugInterface, _nodeID, _tag, _value) \
  MR_LOG_SCRATCH_PAD_INTERNAL(_instanceDebugInterface, logScratchPadInt, _nodeID, _tag, _value)

//----------------------------------------------------------------------------------------------------------------------
#define MR_LOG_SCRATCH_PAD_FLOAT(_instanceDebugInterface, _nodeID, _tag, _value) \
  MR_LOG_SCRATCH_PAD_INTERNAL(_instanceDebugInterface, logScratchPadFloat, _nodeID, _tag, _value)

//----------------------------------------------------------------------------------------------------------------------
#define MR_LOG_SCRATCH_PAD_BOOL(_instanceDebugInterface, _nodeID, _tag, _value) \
  MR_LOG_SCRATCH_PAD_INTERNAL(_instanceDebugInterface, logScratchPadBool, _nodeID, _tag, _value)

//----------------------------------------------------------------------------------------------------------------------
#define MR_LOG_SCRATCH_PAD_VECTOR3(_instanceDebugInterface, _nodeID, _tag, _value) \
  MR_LOG_SCRATCH_PAD_INTERNAL(_instanceDebugInterface, logScratchPadVector3, _nodeID, _tag, _value)

//----------------------------------------------------------------------------------------------------------------------
/// Global debug draw macros
//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_GLOBAL_INTERNAL(_drawFunction, ...) \
  if (MR::DebugManager* _debugManager = MR::DebugManager::getInstance()) \
  _debugManager->_drawFunction(0xFFFFFFFF, MR::INVALID_NODE_ID, NULL, MR::UNKNOWN_FRAME, MR::INVALID_LIMB_INDEX, __VA_ARGS__)

#else // defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_GLOBAL_INTERNAL(...)

#endif // defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_LINE_GLOBAL(start, end, colour) \
  MR_DEBUG_DRAW_GLOBAL_INTERNAL(drawLine, start, end, colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_POINT_GLOBAL(point, radius, colour) \
  MR_DEBUG_DRAW_GLOBAL_INTERNAL(drawPoint, point, radius, colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_SPHERE_GLOBAL(point, radius, colour) \
  MR_DEBUG_DRAW_GLOBAL_INTERNAL(drawSphere, tm, radius, colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_MATRIX_GLOBAL(matrix, size) \
  MR_DEBUG_DRAW_GLOBAL_INTERNAL(drawMatrix, matrix, size, 255)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_VECTOR_GLOBAL(type, position, _offset, colour) \
  MR_DEBUG_DRAW_GLOBAL_INTERNAL(drawVector, type, position, _offset, colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_TEXT_GLOBAL(_position, _text, _colour) \
  MR_DEBUG_DRAW_GLOBAL_INTERNAL(drawText, _position, _text, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_TRIANGLE_GLOBAL(_vertex1, _vertex2, _vertex3, _colour) \
  MR_DEBUG_DRAW_GLOBAL_INTERNAL(drawTriangle, _vertex1, _vertex2, _vertex3, _colour)

//----------------------------------------------------------------------------------------------------------------------
/// Instance debug draw macros
//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, _drawFunction, ...) \
  if (_instanceDebugInterface) \
  { \
    if ((_instanceDebugInterface)->debugOutputsAreOn(MR::DEBUG_OUTPUT_DEBUG_DRAW)) \
    { \
      (_instanceDebugInterface)->_drawFunction(__VA_ARGS__); \
    } \
  }

#else // defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_INTERNAL(...)

#endif // defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_POINT(_instanceDebugInterface, _position, _radius, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawPoint, _position, _radius, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_SPHERE(_instanceDebugInterface, _tm, _radius, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawSphere, _tm, _radius, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_LINE(_instanceDebugInterface, _start, _end, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawLine, _start, _end, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_POLYLINE(_instanceDebugInterface, _numVertices, _vertices, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawPolyLine, _numVertices, _vertices, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_PLANE(_instanceDebugInterface, _position, _normal, _radius, _colour, _normalScale) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawPlane, _position, _normal, _radius, _colour, _normalScale)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_TRIANGLE(_instanceDebugInterface, _vertex1, _vertex2, _vertex3, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawTriangle, _vertex1, _vertex2, _vertex3, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_MATRIX(_instanceDebugInterface, _matrix, _scale) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawMatrix, _matrix, _scale, 255)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_BOX(_instanceDebugInterface, _position, _halfSizes, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawBox, _position, _halfSizes, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_BBOX(_instanceDebugInterface, _tm, _halfSizes, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawBBox, _tm, _halfSizes, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_SPHERESWEEP(_instanceDebugInterface, _position, _motion, _radius, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawSphereSweep, _position, _motion, _radius, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_TEXT(_instanceDebugInterface, _position, _text, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawText, _position, _text, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_VECTOR(_instanceDebugInterface, _type, _position, _offset, _colour) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawVector, _type, _position, _offset, _colour)

//----------------------------------------------------------------------------------------------------------------------
#define MR_DEBUG_DRAW_ENVIRONMENT_PATCH(_instanceDebugInterface, _position, _normal, _radius, _colour ) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawEnvironmentPatch, _position, _normal, _radius, _colour)

#define MR_DEBUG_DRAW_CONE_AND_DIAL(_instanceDebugInterface, _position, _direction, theta, size, dialDirection, _colour ) \
  MR_DEBUG_DRAW_INTERNAL(_instanceDebugInterface, drawConeAndDial, _position, _direction, theta, size, dialDirection, _colour)


//----------------------------------------------------------------------------------------------------------------------
/// Instance debug draw macros
//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)

/// Macro that establishes a debug draw scope for modules.
#define MR_DEBUG_MODULE_ENTER(_instanceDebugInterface, _name) \
  MR::ScopeEntryExit entryExit ## __LINE__; \
  entryExit ## __LINE__.enterScope(_instanceDebugInterface, MR::InstanceDebugInterface::kModule, _name)

/// Macro that establishes a debug draw scope for controls.
#define MR_DEBUG_CONTROL_ENTER(_instanceDebugInterface, _controlID) \
  MR::ScopeEntryExit entryExit ## __LINE__; \
  entryExit ## __LINE__.enterScope(_instanceDebugInterface, MR::InstanceDebugInterface::kControl, (uint32_t)_controlID)

/// Macro that establishes a debug draw scope for controls that for a particular limb.
#define MR_DEBUG_CONTROL_WITH_PART_ENTER(_instanceDebugInterface, _controlID, _limbIndex) \
  MR::ScopeEntryExit entryExit ## __LINE__; \
  entryExit ## __LINE__.enterScope(_instanceDebugInterface, MR::InstanceDebugInterface::kControl, (uint32_t)_controlID, (MR::LimbIndex)_limbIndex)

#else

#define MR_DEBUG_MODULE_ENTER(_instanceDebugInterface, _name)
#define MR_DEBUG_CONTROL_ENTER(_instanceDebugInterface, _controlID)
#define MR_DEBUG_SCOPE_FOR_PART_ENTER(_instanceDebugInterface, _controlID, _part)

#endif


//----------------------------------------------------------------------------------------------------------------------
#endif // MR_DEBUG_MACROS_H
//----------------------------------------------------------------------------------------------------------------------
