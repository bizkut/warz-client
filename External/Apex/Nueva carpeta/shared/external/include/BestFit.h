// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.

#ifndef BEST_FIT_H

#define BEST_FIT_H

// A code snippet to compute the best fit AAB, OBB, plane, capsule and sphere
// Quaternions are assumed a float X,Y,Z,W
// Matrices are assumed 4x4 D3DX style format passed as a float pointer
// The orientation of a capsule is assumed that height is along the Y axis, the same format as the PhysX SDK uses
// The best fit plane routine is derived from code previously published by David Eberly on his Magic Software site.
// The best fit OBB is computed by first approximating the best fit plane, and then brute force rotating the points
// around a single axis to derive the closest fit.  If you set 'bruteforce' to false, it will just use the orientation
// derived from the best fit plane, which is close enough in most cases, but not all.
// Each routine allows you to pass the point stride between position elements in your input vertex stream.
// These routines should all be thread safe as they make no use of any global variables.



namespace SharedTools
{

bool  computeBestFitPlane(size_t vcount,	// number of input data points
                          const float* points,					// starting address of points array.
                          size_t vstride,							// stride between input points.
                          const float* weights,					// *optional point weighting values.
                          size_t wstride,							// weight stride for each vertex.
                          float plane[4]);

float  computeBestFitAABB(size_t vcount, const float* points, size_t pstride, float bmin[3], float bmax[3]); // returns the diagonal distance
float  computeBestFitSphere(size_t vcount, const float* points, size_t pstride, float center[3]);
void   computeBestFitOBB(size_t vcount, const float* points, size_t pstride, float* sides, float matrix[16], bool bruteForce);
void   computeBestFitOBB(size_t vcount, const float* points, size_t pstride, float* sides, float pos[3], float quat[4], bool bruteForce);
void   computeBestFitCapsule(size_t vcount, const float* points, size_t pstride, float& radius, float& height, float matrix[16], bool bruteForce);
void   computeBestFitCapsule(size_t vcount, const float* points, size_t pstride, float& radius, float& height, float pos[3], float quat[4], bool bruteForce);

};

#endif
