  #pragma once

/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_VALUEPOSTCOMBINERS_H
#define NM_VALUEPOSTCOMBINERS_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMatrix34.h"

namespace PostCombiners
{
NM_INLINE void Matrix34Orthonormalise(NMP::Matrix34& v)
{
  v.orthonormalise();
}

NM_INLINE void Vector3Normalise(NMP::Vector3& v)
{
  v.normalise();
}

NM_INLINE void QuatNormalise(NMP::Quat& q)
{
  q.normalise();
}

NM_INLINE void ClampWeight(float& w)
{
  w = NMP::clampValue(w, 0.0f, 1.0f);
}
}

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_VALUEPOSTCOMBINERS_H
//----------------------------------------------------------------------------------------------------------------------

