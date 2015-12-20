// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erLimbTransforms.h"
#include "NMGeomUtils/NMGeomUtils.h"

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
void LimbTransform::createFromForwardAlignedByUp(
  const NMP::Vector3& frontDirection,
  const NMP::Vector3& guidingUpDirection,
  const NMP::Vector3& position)
{
  NMP_ASSERT(fabsf(frontDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  this->frontDirection() = frontDirection;
  NMRU::GeomUtils::calculateOrthonormalVectors(
    rightDirection(), upDirection(), this->frontDirection(), guidingUpDirection);

  translation() = position;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbTransform::createFromRightAlignedByUp(
  const NMP::Vector3& rightDirection,
  const NMP::Vector3& guidingUpDirection,
  const NMP::Vector3& position)
{
  NMP_ASSERT(fabsf(rightDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  this->rightDirection() = rightDirection;
  NMRU::GeomUtils::calculateOrthonormalVectors(
    frontDirection(), upDirection(), this->rightDirection(), guidingUpDirection);
  frontDirection() *= -1.0f;

  translation() = position;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbTransform::createFromUpAlignedByForward(
  const NMP::Vector3& upDirection,
  const NMP::Vector3& guidingFrontDirection,
  const NMP::Vector3& position)
{
  NMP_ASSERT(fabsf(upDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  this->upDirection() = upDirection;
  NMRU::GeomUtils::calculateOrthonormalVectors(
    rightDirection(), frontDirection(), this->upDirection(), guidingFrontDirection);
  rightDirection() *= -1.0f;

  translation() = position;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbTransform::createFromUpAlignedByRight(
  const NMP::Vector3& upDirection,
  const NMP::Vector3& guidingRightDirection,
  const NMP::Vector3& position)
{
  NMP_ASSERT(fabsf(upDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  this->upDirection() = upDirection;
  NMRU::GeomUtils::calculateOrthonormalVectors(
    frontDirection(), rightDirection(), this->upDirection(), guidingRightDirection);

  translation() = position;
}

//----------------------------------------------------------------------------------------------------------------------
void HandFootTransform::createFromNormalAlignedByPoint(
  const NMP::Vector3& normalDirection,
  const NMP::Vector3& guidingPointDirection,
  const NMP::Vector3& position)
{
  NMP_ASSERT(fabsf(normalDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  this->normalDirection() = normalDirection;
  NMRU::GeomUtils::calculateOrthonormalVectors(
    leftDirection(), pointDirection(), this->normalDirection(), guidingPointDirection);

  translation() = position;
}

void HandFootTransform::createFromPointingAlignedByNormal(
  const NMP::Vector3& pointDirection,
  const NMP::Vector3& guidingNormalDirection,
  const NMP::Vector3& position)
{
  NMP_ASSERT(fabsf(pointDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  this->pointDirection() = pointDirection;
  NMRU::GeomUtils::calculateOrthonormalVectors(
    leftDirection(), normalDirection(), this->pointDirection(), guidingNormalDirection);
  leftDirection() *= -1.0f;

  translation() = position;
}

void HandFootTransform::createFromPointingAlignedByLeft(
  const NMP::Vector3& pointDirection,
  const NMP::Vector3& guidingThumbDirection,
  const NMP::Vector3& position)
{
  NMP_ASSERT(fabsf(pointDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  this->pointDirection() = pointDirection;
  NMRU::GeomUtils::calculateOrthonormalVectors(
    normalDirection(), leftDirection(), this->pointDirection(), guidingThumbDirection);

  translation() = position;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbTransform::alignFrontDirectionWith(const NMP::Vector3& frontDirection)
{
  NMP_ASSERT(fabsf(frontDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  NMP::Quat q;
  q.forRotation(this->frontDirection(), frontDirection);
  this->multiply3x3(NMP::Matrix34(q));
}

//----------------------------------------------------------------------------------------------------------------------
void LimbTransform::alignUpDirectionWith(const NMP::Vector3& upDirection)
{
  NMP_ASSERT(fabsf(upDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  NMP::Quat q;
  q.forRotation(this->upDirection(), upDirection);
  this->multiply3x3(NMP::Matrix34(q));
}

//----------------------------------------------------------------------------------------------------------------------
void LimbTransform::alignRightDirectionWith(const NMP::Vector3& rightDirection)
{
  NMP_ASSERT(fabsf(rightDirection.magnitudeSquared() - 1.0f) < 0.01f); // primary direction vector needs to be normalised
  NMP::Quat q;
  q.forRotation(this->rightDirection(), rightDirection);
  this->multiply3x3(NMP::Matrix34(q));
}

}