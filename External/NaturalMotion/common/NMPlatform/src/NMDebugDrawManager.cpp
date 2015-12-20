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
#include "NMPlatform/NMDebugDrawManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
Vector3 DebugDrawManager::sm_up(0.0f, 1.0f, 0.0f);

//----------------------------------------------------------------------------------------------------------------------
DebugDrawManager::DebugDrawManager()
{
  m_enabled = true;
  m_numDebugDrawClients = 0;
}

//----------------------------------------------------------------------------------------------------------------------
DebugDrawManager::~DebugDrawManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::addClient(DebugDrawClient* ddClient)
{
  m_debugDrawClients[m_numDebugDrawClients] = ddClient;
  m_numDebugDrawClients++;
}

//----------------------------------------------------------------------------------------------------------------------
bool DebugDrawManager::removeClient(DebugDrawClient* ddClient)
{
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    if (m_debugDrawClients[i] == ddClient)
    {
      m_numDebugDrawClients--;

      // Shuffle up the rest
      for (; i < m_numDebugDrawClients; i++)
      {
        m_debugDrawClients[i] = m_debugDrawClients[i+1];
      }
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::drawLine(const NMP::Vector3& start, const NMP::Vector3& end, float red, float green, float blue, float alpha)
{
  if (!m_enabled)
    return;
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->drawLine(start, end, red, green, blue, alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::drawTriangle(const NMP::Vector3& v1, const NMP::Vector3& v2, const NMP::Vector3& v3, float red, float green, float blue, float alpha)
{
  if (!m_enabled)
    return;
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->drawTriangle(v1, v2, v3, red, green, blue, alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::drawMatrix(const NMP::Matrix34& matrix, float size)
{
  if (!m_enabled)
    return;
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->drawMatrix(matrix, size);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::drawNonUniformMatrix(const NMP::Matrix34& matrix, float scale, float alpha)
{
  if (!m_enabled)
    return;
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->drawNonUniformMatrix(matrix, scale, alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::drawPoint(const NMP::Vector3& point, float radius, float red, float green, float blue, float alpha)
{
  if (!m_enabled)
    return;
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->drawPoint(point, radius, red, green, blue, alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::drawSphere(const NMP::Matrix34& tm, float radius, float red, float green, float blue, float alpha)
{
  if (!m_enabled)
    return;
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->drawSphere(tm, radius, red, green, blue, alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::drawPlane(
  const NMP::Vector3& point, const NMP::Vector3& normal, float radius, const NMP::Vector3& colour, float normalScale)
{
  if (!m_enabled)
    return;
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->drawPlane(point, normal, radius, colour, normalScale);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::clear()
{
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->clear();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::drawLine(NMP::LineType type, const NMP::Vector3& pos, const NMP::Vector3& vec, const NMP::Vector3& color)
{
  if (!m_enabled)
    return;

  // Draw the main line
  for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
  {
    DebugDrawClient* ddClient = m_debugDrawClients[i];
    ddClient->drawLine(pos, pos + vec, color.x, color.y, color.z, 1.0f);
  }

  if (type == NMP::LT_Normal) // no head
  {
    return;
  }

  bool isDelta = type >= NMP::LT_Push && type <= NMP::LT_AngleDelta;
  bool hasMass = (type / 2) % 2 == 0;
  float arrowHeadScale = 0.03f;
  float twistScale = 0.06f;
  NMP::Vector3 perp;
  perp.cross(vec, sm_up);
  NMP::Vector3 dir2;
  dir2.cross(vec, perp);
  perp.normalise();
  dir2.normalise();
  NMP::Vector3 dir = vec;
  float length = sqrtf(dir.normaliseGetLength());

  if (type % 2 == 1) // if its a rotation
  {
    addTwist(pos + vec, dir* length* twistScale, dir2* length* twistScale, perp* length* twistScale, color, type >= NMP::LT_Force && type <= NMP::LT_AngularAcceleration, hasMass, isDelta);
  }
  else
  {
    NMP::Vector3 tangent = type == NMP::LT_Delta ? NMP::Vector3(0, 0, 0) : dir * length * arrowHeadScale;
    addArrowHead(pos + vec, tangent, dir2 * length * arrowHeadScale, color, hasMass, isDelta);
    if (type >= NMP::LT_Force && type <= NMP::LT_AngularAcceleration) // if its a second derivative
    {
      addArrowHead(pos + vec - tangent, tangent, dir2 * length * arrowHeadScale, color, hasMass, isDelta);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::addTwist(const NMP::Vector3& pos, const NMP::Vector3& primaryDir, const NMP::Vector3& dir, const NMP::Vector3& dir2, const NMP::Vector3& col, bool isDouble, bool hasMass, bool isDelta)
{
  if (!m_enabled)
    return;

  int numSegments = 8;
  NMP::Vector3 oldPos = pos + dir;
  NMP::Vector3 newDir = -dir2 * cosf(0.3f) + dir * sinf(0.3f);
  addArrowHead(oldPos, newDir * 0.4f, primaryDir * 0.4f, col, hasMass, isDelta);
  if (isDouble)
  {
    float t = 0.4f;
    NMP::Vector3 point = pos + dir * cosf(t) + dir2 * sinf(t);
    addArrowHead(point, newDir * 0.4f, primaryDir * 0.4f, col, hasMass, isDelta);
  }
  for (int i = 1; i < numSegments; i++)
  {
    float t = 4.0f * (float)i / ((float)numSegments - 1.0f);
    NMP::Vector3 point = pos + dir * cosf(t) + dir2 * sinf(t);

    for (uint32_t j = 0; j < m_numDebugDrawClients; ++j)
    {
      DebugDrawClient* ddClient = m_debugDrawClients[j];
      ddClient->drawLine(oldPos, point, col.x, col.y, col.z, 1.0f);
    }

    oldPos = point;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDrawManager::addArrowHead(const NMP::Vector3& pos, const NMP::Vector3& dir, const NMP::Vector3& dir2, const NMP::Vector3& col, bool hasMass, bool isDelta)
{
  if (!m_enabled)
    return;

  // the two lines below just rearrange the arrow shape for deltas (ie shifts in space)
  NMP::Vector3 tangent = isDelta ? (hasMass ? -dir : NMP::Vector3(0, 0, 0)) : dir;
  NMP::Vector3 start = isDelta && hasMass ? pos + tangent : pos;
  NMP::Vector3 corner1 = start;
  NMP::Vector3 corner2 = start - tangent + dir2;
  NMP::Vector3 corner3 = start - tangent - dir2;
  if (hasMass)
  {
    for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
    {
      DebugDrawClient* ddClient = m_debugDrawClients[i];
      ddClient->drawTriangle(corner1, corner2, corner3, col.x, col.y, col.z, 1.0f);
    }
  }
  else
  {

    for (uint32_t i = 0; i < m_numDebugDrawClients; ++i)
    {
      DebugDrawClient* ddClient = m_debugDrawClients[i];
      ddClient->drawLine(corner1, corner2, col.x, col.y, col.z, 1.0f);
      ddClient->drawLine(corner1, corner3, col.x, col.y, col.z, 1.0f);
    }
  }
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
