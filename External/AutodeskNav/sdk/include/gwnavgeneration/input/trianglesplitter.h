/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: NONE
#ifndef GwNavGen_TriangleSplitter_H
#define GwNavGen_TriangleSplitter_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavgeneration/generator/generatorsystem.h"

namespace Kaim
{

// This class essentially split a triangle into multiple sub triangles using the "IsTriangleTooBig" criteria from the coordSystem. 
// Children will have the same winding (CW or CCW) as their parent.
// If the input triangles already satisfies the criteria, children list will be empty
class TriangleSplitter
{
public:
	static KyResult SplitTriangleAccordingToGeneratorSystem(
		const Vec3f& A, const Vec3f& B, const Vec3f& C, const  GeneratorSystem& generatorSystem, KyArray<Vec3f>& childrenLegitTriangles);

private: // internal usage
	static KyResult StepSubDivide(const Vec3f& A, const Vec3f& B, const Vec3f& C, bool isCCW, KyArray<Vec3f>& children);
};


}


#endif

