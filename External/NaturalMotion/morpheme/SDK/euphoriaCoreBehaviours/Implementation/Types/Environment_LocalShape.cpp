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

#include "Types/Environment_LocalShape.h"
#include "Types/Environment_Patch.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

int cNumberOfFacesOrCapsuleEdges[Environment::Patch::EO_Max] = { -1, 3, 2, 1, 1, 0 };

void Environment::LocalShape::fromWorldSpace(const Patch& patch, const NMP::Matrix34& matrix)
{
  type = patch.type;
  matrix.inverseTransformVector(patch.knownContactPoint, knownContactPoint);
  matrix.inverseTransformVector(patch.corner, corner);
  for (int i = 0; i < cNumberOfFacesOrCapsuleEdges[patch.type]; i++)
    matrix.inverseRotateVector(patch.faceNormals[i], faceNormals[i]);
}

void Environment::LocalShape::toWorldSpace(Patch& patch, const NMP::Matrix34& matrix)
{
  patch.type = type;
  matrix.transformVector(knownContactPoint, patch.knownContactPoint);
  matrix.transformVector(corner, patch.corner);
  for (int i = 0; i < cNumberOfFacesOrCapsuleEdges[patch.type]; i++)
    matrix.rotateVector(faceNormals[i], patch.faceNormals[i]);
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

