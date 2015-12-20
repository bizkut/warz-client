// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_LIMBTRANSFORMS_H
#define NM_LIMBTRANSFORMS_H

#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
/// This class can be used to represent any matrix where the three rotation axes are well defined as below
/// Currently it represents the matrices for hands and feet
//----------------------------------------------------------------------------------------------------------------------
class HandFootTransform : public NMP::Matrix34
{
public:
  HandFootTransform() { *this = (const HandFootTransform&)NMP::Matrix34Identity(); }
  HandFootTransform(const NMP::Matrix34& mat) { *this = (const HandFootTransform&)mat; }
  NMP::Matrix34& matrix() { return (NMP::Matrix34&) * this; }
  const NMP::Matrix34& matrix() const { return (NMP::Matrix34&) * this; }
  const NMP::Vector3& normalDirection() const { return r[0]; } // out back of hands and feet, match this to surface normal
  const NMP::Vector3& pointDirection() const { return r[1]; } // toe and finger direction
  const NMP::Vector3& leftDirection() const { return r[2]; } // points left if palms facing forwards on a desk, or for flat feet
  NMP::Vector3& normalDirection() { return r[0]; }
  NMP::Vector3& pointDirection() { return r[1]; }
  NMP::Vector3& leftDirection() { return r[2]; }

  NM_INLINE NMP::Vector3& xAxis() { NMP_ASSERT_FAIL(); return r[0]; } // please use above direction accessors
  NM_INLINE NMP::Vector3& yAxis() { NMP_ASSERT_FAIL(); return r[1]; } // please use above direction accessors
  NM_INLINE NMP::Vector3& zAxis() { NMP_ASSERT_FAIL(); return r[2]; } // please use above direction accessors
  NM_INLINE const NMP::Vector3 xAxis() const { NMP_ASSERT_FAIL(); return r[0]; } // please use above direction accessors
  NM_INLINE const NMP::Vector3 yAxis() const { NMP_ASSERT_FAIL(); return r[1]; } // please use above direction accessors
  NM_INLINE const NMP::Vector3 zAxis() const { NMP_ASSERT_FAIL(); return r[2]; } // please use above direction accessors

  /*
  // here, matrices are constructed with the 2nd argument roughly guiding the direction
  void createPointingAlignedNormal(const NMP::Vector3 &pointDirection, const NMP::Vector3 &roughPalmOutDirection) {}
  void createPointingAlignedLeft(const NMP::Vector3 &pointDirection, const NMP::Vector3 &roughThumbDirection) {}

  void createPlacingAlignedLeft(const NMP::Vector3 &backOfHandDirection, const NMP::Vector3 &roughThumbDirection) {}
  */
  void createFromPointingAlignedByNormal(const NMP::Vector3& pointDirection, const NMP::Vector3& roughPalmOutDirection, const NMP::Vector3& position = NMP::Vector3(0, 0, 0));
  void createFromNormalAlignedByPoint(const NMP::Vector3& normalDirection, const NMP::Vector3& guidingNormalDirection, const NMP::Vector3& position = NMP::Vector3(0, 0, 0));
  void createFromPointingAlignedByLeft(const NMP::Vector3& pointDirection, const NMP::Vector3& guidingThumbDirection, const NMP::Vector3& position = NMP::Vector3(0, 0, 0));
};

//----------------------------------------------------------------------------------------------------------------------
/// This class can be used to represent any matrix where the three rotation axes are well defined as below
/// Currently it represents the matrices for all limb roots, bases and all limb ends apart from hands and feet
//----------------------------------------------------------------------------------------------------------------------
class LimbTransform : public NMP::Matrix34
{
public:
  LimbTransform() { *this = (const HandFootTransform&)NMP::Matrix34Identity(); }
  LimbTransform(const NMP::Matrix34& mat) { *this = (const LimbTransform&)mat; }
  NMP::Matrix34& matrix() { return (NMP::Matrix34&) * this; }
  const NMP::Matrix34& matrix() const { return (NMP::Matrix34&) * this; }
  const NMP::Vector3& frontDirection() const { return r[0]; } // In the general forwards direction of the character
  const NMP::Vector3& upDirection() const { return r[1]; } // In the general upwards direction of the character
  const NMP::Vector3& rightDirection() const { return r[2]; } // In the general rightward direction of the character
  NMP::Vector3& frontDirection() { return r[0]; }
  NMP::Vector3& upDirection() { return r[1]; }
  NMP::Vector3& rightDirection() { return r[2]; }

  NM_INLINE NMP::Vector3& xAxis() { NMP_ASSERT_FAIL(); return r[0]; } // please use above direction accessors
  NM_INLINE NMP::Vector3& yAxis() { NMP_ASSERT_FAIL(); return r[1]; } // please use above direction accessors
  NM_INLINE NMP::Vector3& zAxis() { NMP_ASSERT_FAIL(); return r[2]; } // please use above direction accessors
  NM_INLINE const NMP::Vector3 xAxis() const { NMP_ASSERT_FAIL(); return r[0]; } // please use above direction accessors
  NM_INLINE const NMP::Vector3 yAxis() const { NMP_ASSERT_FAIL(); return r[1]; } // please use above direction accessors
  NM_INLINE const NMP::Vector3 zAxis() const { NMP_ASSERT_FAIL(); return r[2]; } // please use above direction accessors

  // here, matrices are constructed with the 2nd argument roughly guiding the direction
  void createFromForwardAlignedByUp(const NMP::Vector3& frontDirection, const NMP::Vector3& guidingUpDirection, const NMP::Vector3& position = NMP::Vector3(0, 0, 0));
  void createFromRightAlignedByUp(const NMP::Vector3& rightDirection, const NMP::Vector3& guidingUpDirection, const NMP::Vector3& position = NMP::Vector3(0, 0, 0));
  void createFromUpAlignedByForward(const NMP::Vector3& upDirection, const NMP::Vector3& guidingFrontDirection, const NMP::Vector3& position = NMP::Vector3(0, 0, 0));
  void createFromUpAlignedByRight(const NMP::Vector3& upDirection, const NMP::Vector3& guidingRightDirection, const NMP::Vector3& position = NMP::Vector3(0, 0, 0));

  //here, matrices are rotated to align the axis with the argument
  void alignFrontDirectionWith(const NMP::Vector3& frontDirection);
  void alignUpDirectionWith(const NMP::Vector3& upDirection);
  void alignRightDirectionWith(const NMP::Vector3& rightDirection);
};
}

#endif // NM_LIMBTRANSFORMS_H
