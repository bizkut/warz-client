/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: MAMU
#ifndef Navigation_VisualBoxVertices_H
#define Navigation_VisualBoxVertices_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{

class Box3f;
class Transform;
class OrientedBox2d;

//      H----------G
//     /|         /|
//    / |        / |
//   E----------F  |
//   |  |       |  |
//   |  D-------|--C
//   | /        | /
//   |/         |/
//   A----------B

/*
VisualSystem module internal class.
Allows to store oriented box vertices.
There are many ways to initialize the oriented box vertices positions : InitFrom...(...).
*/
class VisualBoxVertices
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)

public:
	void InitFromCenter(const Vec3f& center, KyFloat32 halfWidth);
	void InitFromBox(const Box3f& box);
	void InitFromSegment(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth);
	void InitFromWall(const Vec3f& P, const Vec3f& Q, KyFloat32 upHeight, KyFloat32 downHeight, KyFloat32 halfWidth);
	void InitFromTransformExtent(const Transform& transform, const Box3f& extents);
	void InitFromOrientedBox2d(const OrientedBox2d& orientedBox2d);
public:
	Vec3f A, B, C, D, E, F, G, H;
};


}


#endif
