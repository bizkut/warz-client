/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
All rights reserved.


 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#ifndef HACD_MANIFOLD_MESH_H
#define HACD_MANIFOLD_MESH_H

#include <hacdVersion.h>
#include <hacdCircularList.h>
#include <hacdVector.h>
#include <hacdContainer.h>

namespace HACD
{
	class TMMTriangle;
	class TMMEdge;
    class TMMesh;
    class ICHull;
	class HACD;

	//! IntersectRayTriangle(): intersect a ray with a 3D triangle
	//!    Input:  a ray R, and a triangle T
	//!    Output: *I = intersection point (when it exists)
	//!             0 = disjoint (no intersect)
	//!             1 = intersect in unique point I1
	hacd::HaI32														IntersectRayTriangle( const Vec3<hacd::HaF64> & P0, const Vec3<hacd::HaF64> & dir, 
																					  const Vec3<hacd::HaF64> & V0, const Vec3<hacd::HaF64> & V1, 
																					  const Vec3<hacd::HaF64> & V2, hacd::HaF64 &t);

	// intersect_RayTriangle(): intersect a ray with a 3D triangle
	//    Input:  a ray R, and a triangle T
	//    Output: *I = intersection point (when it exists)
	//    Return: -1 = triangle is degenerate (a segment or point)
	//             0 = disjoint (no intersect)
	//             1 = intersect in unique point I1
	//             2 = are in the same plane
	hacd::HaI32														IntersectRayTriangle2(const Vec3<hacd::HaF64> & P0, const Vec3<hacd::HaF64> & dir, 
																					   const Vec3<hacd::HaF64> & V0, const Vec3<hacd::HaF64> & V1, 
																					   const Vec3<hacd::HaF64> & V2, hacd::HaF64 &r);

    /*
     Calculate the line segment PaPb that is the shortest route between
     two lines P1P2 and P3P4. Calculate also the values of mua and mub where
     Pa = P1 + mua (P2 - P1)
     Pb = P3 + mub (P4 - P3)
     Return FALSE if no solution exists.
     */
    bool                                                        IntersectLineLine(const Vec3<hacd::HaF64> & p1, const Vec3<hacd::HaF64> & p2, 
                                                                                  const Vec3<hacd::HaF64> & p3, const Vec3<hacd::HaF64> & p4,
                                                                                  Vec3<hacd::HaF64> & pa, Vec3<hacd::HaF64> & pb, 
                                                                                  hacd::HaF64 & mua, hacd::HaF64 &mub);
}
#endif