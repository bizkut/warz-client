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
#ifndef NM_DEBUG_DRAW_MANAGER_H
#define NM_DEBUG_DRAW_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
typedef enum LineType
{
  LT_Force,               // kg m / s^2
  LT_Torque,              // kg m^2 rad / s^2
  LT_Acceleration,        // m / s^2
  LT_AngularAcceleration, // rad / s^2
  LT_Impulse,             // kg m / s        (also use for momentum)
  LT_TorqueImpulse,       // kg m^2 rad / s  (also use for angular momentum)
  LT_Velocity,            // m / s
  LT_AngularVelocity,     // rad / s
  LT_Push,                // kg m
  LT_AnglePush,           // kg m^2 rad
  LT_Delta,               // m                (a change in a position)
  LT_AngleDelta,          // rad
  LT_Normal,              // dimensionless, should be unit length really
  LT_size,
} LineType;

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::DebugDrawClient
/// \brief This class provides an interface which must be implemented in order to register a rendering system with the
///  NMP::DebugDrawManager.  It allows applications to recieve debug draw commands from internal NM libraries in an
///  abstracted fashion.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class DebugDrawClient
{
public:
  DebugDrawClient() {}
  virtual ~DebugDrawClient() {}

  virtual void drawLine(const NMP::Vector3& start, const NMP::Vector3& end, float red, float green, float blue, float alpha) = 0;
  virtual void drawTriangle(const NMP::Vector3& v1, const NMP::Vector3& v2, const NMP::Vector3& v3, float red, float green, float blue, float alpha) = 0;
  virtual void drawMatrix(const NMP::Matrix34& matrix, float size = 1.0f) = 0;
  virtual void drawNonUniformMatrix(const NMP::Matrix34& matrix, float scale, float alpha) = 0;
  virtual void drawPoint(const NMP::Vector3& point, float radius, float red, float green, float blue, float alpha) = 0;
  virtual void drawSphere(const NMP::Matrix34& tm, float radius, float red, float green, float blue, float alpha) = 0;
  virtual void drawPlane(const NMP::Vector3& point, const NMP::Vector3& normal, float radius, const NMP::Vector3& colour, float normalScale = 1.0f) = 0;
  virtual void clear() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::DebugDrawManager
/// \brief A class to provide an abstracted set of simple debug draw functionality.  The aim of this class is to decouple
///  NM code from a specific renderer that is consuming debug draw commands.  This class allows an application
///  to register NMP::DebugDrawClient implementations. Debug draw commands sent in will then be forwarded to each of the
///  clients for actual rendering.
///  This class also provides complex line types to represent values such as acceleration or angular velocity.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class DebugDrawManager
{
public:
  static const uint32_t MAX_DEBUG_DRAW_CLIENTS = 8;

  DebugDrawManager();
  ~DebugDrawManager();

  void drawLine(NMP::LineType type, const NMP::Vector3& pos, const NMP::Vector3& vec, const NMP::Vector3& color);
  void drawLine(const NMP::Vector3& start, const NMP::Vector3& end, float red, float green, float blue, float alpha);
  void drawTriangle(const NMP::Vector3& v1, const NMP::Vector3& v2, const NMP::Vector3& v3, float red, float green, float blue, float alpha);
  void drawMatrix(const NMP::Matrix34& matrix, float size = 1.0f);
  void drawNonUniformMatrix(const NMP::Matrix34& matrix, float scale, float alpha);
  void drawPoint(const NMP::Vector3& point, float radius, float red, float green, float blue, float alpha);
  void drawSphere(const NMP::Matrix34& tm, float radius, float red, float green, float blue, float alpha);
  void drawPlane(const NMP::Vector3& point, const NMP::Vector3& normal, float radius, const NMP::Vector3& colour, float normalScale = 1.0f);

  void clear();

  void addClient(DebugDrawClient* ddClient);
  bool removeClient(DebugDrawClient* ddClient);

  // allow global enable/disable
  void enableDebugDraw(bool enable) { m_enabled = enable; }

protected:

  void addArrowHead(const NMP::Vector3& pos, const NMP::Vector3& dir, const NMP::Vector3& dir2, const NMP::Vector3& col, bool hasMass, bool isDelta);
  void addTwist(const NMP::Vector3& pos, const NMP::Vector3& primaryDir, const NMP::Vector3& dir, const NMP::Vector3& dir2, const NMP::Vector3& col, bool isDouble, bool hasMass, bool isDelta);

  DebugDrawClient* m_debugDrawClients[MAX_DEBUG_DRAW_CLIENTS];
  uint32_t         m_numDebugDrawClients;
  bool             m_enabled;

  static NMP::Vector3 sm_up;
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_DEBUG_DRAW_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
