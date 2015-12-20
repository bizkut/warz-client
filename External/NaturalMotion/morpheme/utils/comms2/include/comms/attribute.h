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
#ifndef MC_ATTRIBUTE_H
#define MC_ATTRIBUTE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMColour.h"
#include "comms/mcomms.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

struct DebugLine;
struct DebugTriangle;

//----------------------------------------------------------------------------------------------------------------------
// Attribute class
//----------------------------------------------------------------------------------------------------------------------
class NMP_ALIGN_PREFIX(16) Attribute
{
public:

  ///
  enum DataType
  {
    DATATYPE_FLOAT,
    DATATYPE_INT,
    DATATYPE_UINT,
    DATATYPE_CHAR,
    DATATYPE_MATRIX,
    DATATYPE_VECTOR,
    DATATYPE_COLOUR,
    DATATYPE_LINE,
    DATATYPE_TRIANGLE,

    DATATYPE_INVALID = 0xffffffffu,
    DATATYPE_FORCE_UINT = 0xffffffffu
  };

  ///
  enum Semantic
  {
    SEMANTIC_VERTEX_POSITIONS,
    SEMANTIC_VERTEX_INDICES,
    SEMANTIC_VERTEX_NORMALS,
    SEMANTIC_VERTICES_WINDING,
    SEMANTIC_PATH_HINT, ///< A macroised string to the mcskin for the rigs skin
    SEMANTIC_BONE_MATRICES,
    SEMANTIC_TRANSFORM,
    SEMANTIC_COLOUR,
    SEMANTIC_NAME,
    SEMANTIC_PARENT_OBJECT_ID,
    SEMANTIC_PARENT_BONE_ID,
    SEMANTIC_PARENT_OFFSET,
    SEMANTIC_STRING_HINT,
    SEMANTIC_INSTANCEID,
    SEMANTIC_ANIMSET_INDEX,

    SEMANTIC_PHYSICS_SHAPE_TYPE,
    SEMANTIC_PHYSICS_OBJECT_ID, // used to identify physics objects when creating constraints and applying forces.
    SEMANTIC_LENGTH,
    SEMANTIC_DEPTH,
    SEMANTIC_HEIGHT,
    SEMANTIC_DENSITY,
    SEMANTIC_RADIUS,
    SEMANTIC_STATIC_FRICTION,
    SEMANTIC_DYNAMIC_FRICTION,
    SEMANTIC_RESTITUTION,
    SEMANTIC_SKIN_WIDTH,
    SEMANTIC_STEP_HEIGHT,
    SEMANTIC_MAX_PUSH_FORCE,
    SEMANTIC_MAX_SLOPE_ANGLE,
    SEMANTIC_MAX_ANGULAR_VELOCITY,
    SEMANTIC_LINEAR_DAMPING,
    SEMANTIC_ANGULAR_DAMPING,
    SEMANTIC_ACTIVE_PHYSICS_OBJECT,
    SEMANTIC_DYNAMIC_PHYSICS_OBJECT,  // if set to true the object is dynamic, if set to false it is static
    SEMANTIC_CONSTRAINED_PHYSICS_OBJECT,
    SEMANTIC_CONSTRAINT_GLOBAL_TRANSFORM,
    SEMANTIC_CONSTRAINT_LOCAL_TRANSFORM,
    SEMANTIC_CONSTRAINT_STIFFNESS,
    SEMANTIC_CONSTRAINT_DAMPING,
    SEMANTIC_POSITION_SOLVER_ITERATION_COUNT,
    SEMANTIC_VELOCITY_SOLVER_ITERATION_COUNT,
    SEMANTIC_SLEEP_THRESHOLD,

    SEMANTIC_GRAVITY_ENABLED,
    SEMANTIC_GRAVITY_VECTOR,
    SEMANTIC_ASSET_SCALE,
    SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE,

    SEMANTIC_DEBUGLINES,
    SEMANTIC_DEBUGTRIANGLES,

    SEMANTIC_INVALID = 0xffffffffu,
    SEMANTIC_FORCE_UINT = 0xffffffffu
  };

  typedef enum
  {
    PHYSICS_SHAPE_BOX,
    PHYSICS_SHAPE_SPHERE,
    PHYSICS_SHAPE_CAPSULE,
    PHYSICS_SHAPE_CYLINDER,
    PHYSICS_SHAPE_MESH,
    PHYSICS_SHAPE_CONVEX_HULL,  ///< A mesh that will be converted to a convex hull (convex mesh)
    PHYSICS_SHAPE_INVALID
  } PhysicsShapeType;

  typedef enum
  {
    VERTICES_WINDING_CLOCKWISE,
    VERTICES_WINDING_COUNTERCLOCKWISE,
    VERTICES_WINDING_UNDEFINED,
  } VerticesWindingType;

  typedef enum
  {
    CONTROLLER_SHAPE_BOX,
    CONTROLLER_SHAPE_CYLINDER,
    CONTROLLER_SHAPE_CAPSULE,
    CONTROLLER_SHAPE_SPHERE
  } ControllerShapeType;

  //--------------------------------------------------------
  /// \brief The data type
  //--------------------------------------------------------
  class Descriptor
  {
  public:
    Descriptor(DataType dataType, uint32_t dataCount,
               Semantic sem, bool dynamic = false);
    DataType getDataType() const;
    size_t getDataCount() const;
    void setDataCount(uint32_t dataCount);
    Semantic getSemantic() const;
    bool isDynamic() const;

    bool operator==(const Descriptor& other) const;
    bool operator!=(const Descriptor& other) const
    { return !(operator == (other)); }

    void endianSwap();

  protected:
    DataType     m_dataType;
    uint32_t     m_dataCount;
    Semantic     m_semantic;
    bool         m_isDynamic;
  };

  /// \param data Must point to a block of memory of the appropriate size
  /// \param attrDesc The attribute descriptor to create an attribute from
  void init(const Descriptor& attDesc, void* data);

  DataType getDataType() const;
  size_t getDataCount() const;
  Semantic getSemantic() const;
  bool isDynamic() const;

  size_t getDataSize() const;
  const Descriptor* getDescriptor() const;
  const void* getData() const;
  void* getData();

  inline void setDataCount(uint32_t dataCount);

  bool copyDataFrom(const Attribute* src);
  void endianSwapData();
  static void endianSwapData(Descriptor* desc, void* data);
  static void endianSwapDesc(Descriptor* desc);

  /// \return the size in bytes of the specified datatype
  static uint32_t getDataTypeSize(DataType dataType);

  /// \return a string description of the semantic
  static const char* getSemanticDesc(Semantic semantic);

  static Attribute* create(const Descriptor* desc, const void* data);
  static Attribute* createEmpty();
  static void destroy(Attribute* attribute);

  // This doxygen grouping crashes doxygen SteveT.
  // @{
  /// Intended only for use in tools.
  /// Allocates a new Attribute and copies the specified data into it.
  static Attribute* createAttribute(bool           value, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(float          value, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(int32_t        value, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(uint32_t       value, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const NMP::Vector3&  value, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const NMP::Matrix34& value, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const NMP::Colour&   value, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const float*         values, uint32_t numElements, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const int32_t*       values, uint32_t numElements, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const uint32_t*      values, uint32_t numElements, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const NMP::Vector3*  values, uint32_t numElements, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const NMP::Matrix34* values, uint32_t numElements, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const char* string, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const MCOMMS::DebugLine* values, uint32_t numElements, Semantic semantic, bool dynamic = false);
  static Attribute* createAttribute(const MCOMMS::DebugTriangle* values, uint32_t numElements, Semantic semantic, bool dynamic = false);
  // @}

protected:
  Attribute();

  Descriptor            m_attrDesc;
  size_t                m_dataSize;
  void*                 m_data;
#ifndef NM_HOST_64_BIT
  uint32_t              m_pad[2];
#endif // NM_HOST_64_BIT

} NMP_ALIGN_SUFFIX(16);

//----------------------------------------------------------------------------------------------------------------------
// Attribute::Descriptor inline functions
//----------------------------------------------------------------------------------------------------------------------
inline Attribute::Descriptor::Descriptor(
  Attribute::DataType dataType,
  uint32_t            dataCount,
  Attribute::Semantic sem, bool dynamic) :
  m_dataType(dataType),
  m_dataCount(dataCount),
  m_semantic(sem),
  m_isDynamic(dynamic)
{ }

//----------------------------------------------------------------------------------------------------------------------
inline void Attribute::Descriptor::endianSwap()
{
  NMP::netEndianSwap(m_dataType);
  NMP::netEndianSwap(m_dataCount);
  NMP::netEndianSwap(m_semantic);
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::DataType Attribute::Descriptor::getDataType() const
{
  return m_dataType;
}

//----------------------------------------------------------------------------------------------------------------------
inline size_t Attribute::Descriptor::getDataCount() const
{
  return m_dataCount;
}

//----------------------------------------------------------------------------------------------------------------------
inline void Attribute::Descriptor::setDataCount(uint32_t dataCount)
{
  m_dataCount = dataCount;
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::Semantic Attribute::Descriptor::getSemantic() const
{
  return m_semantic;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Attribute::Descriptor::isDynamic() const
{
  return m_isDynamic;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Attribute::Descriptor::operator==(const Attribute::Descriptor& other) const
{
  return (m_dataType == other.m_dataType) &&
         (m_dataCount == other.m_dataCount) &&
         (m_semantic == other.m_semantic) &&
         (m_isDynamic == other.m_isDynamic);
}

//----------------------------------------------------------------------------------------------------------------------
// Attribute inline functions
//----------------------------------------------------------------------------------------------------------------------
inline void Attribute::init(const Attribute::Descriptor& attrDesc, void* data)
{
  m_attrDesc = attrDesc;
  m_data = data;
  m_dataSize = getDataTypeSize(m_attrDesc.getDataType()) * m_attrDesc.getDataCount();
  NMP_ASSERT_MSG(data != 0, "data argument must not be nul to Attribute::Attribute.");
}

//----------------------------------------------------------------------------------------------------------------------
inline void Attribute::setDataCount(uint32_t dataCount)
{
  m_attrDesc.setDataCount(dataCount);
  // Amend the data size.
  m_dataSize = getDataTypeSize(m_attrDesc.getDataType()) * m_attrDesc.getDataCount();
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::DataType Attribute::getDataType() const
{
  return m_attrDesc.getDataType();
}

//----------------------------------------------------------------------------------------------------------------------
inline size_t Attribute::getDataCount() const
{
  return m_attrDesc.getDataCount();
}

//----------------------------------------------------------------------------------------------------------------------
inline Attribute::Semantic Attribute::getSemantic() const
{
  return m_attrDesc.getSemantic();
}

//----------------------------------------------------------------------------------------------------------------------
inline bool Attribute::isDynamic() const
{
  return m_attrDesc.isDynamic();
}

//----------------------------------------------------------------------------------------------------------------------
inline size_t Attribute::getDataSize() const
{
  return m_dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
inline const Attribute::Descriptor* Attribute::getDescriptor() const
{
  return &m_attrDesc;
}

//----------------------------------------------------------------------------------------------------------------------
inline const void* Attribute::getData() const
{
  if (m_data)
    return m_data;
  // When data is not set, assuming actual data just beyhond the object.
  return (const void*)(this + 1);
}

//----------------------------------------------------------------------------------------------------------------------
inline void* Attribute::getData()
{
  if (m_data)
    return m_data;
  // When data is not set, assuming actual data just beyhond the object.
  return (void*)(this + 1);
}

} // MCCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MC_ATTRIBUTE_H
//----------------------------------------------------------------------------------------------------------------------
