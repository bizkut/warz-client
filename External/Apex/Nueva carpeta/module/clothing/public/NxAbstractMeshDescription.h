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

#ifndef NX_ABSTRACT_MESH_DESCRIPTION_H
#define NX_ABSTRACT_MESH_DESCRIPTION_H

#include <NxApexUsingNamespace.h>
#include "foundation/PxVec3.h"

class NxClothConstrainCoefficients;


namespace physx
{
namespace apex
{

class NxApexRenderDebug;
/**
\brief a simplified, temporal container for a mesh with non-interleaved vertex buffers
*/

struct NxAbstractMeshDescription
{
	NxAbstractMeshDescription() : numVertices(0), numIndices(0), numBonesPerVertex(0),
		pPosition(NULL), pNormal(NULL), pTangent(NULL), pTangent4(NULL), pBitangent(NULL),
		pBoneIndices(NULL), pBoneWeights(NULL), pConstraints(NULL), pVertexFlags(NULL), pIndices(NULL),
		avgEdgeLength(0.0f), avgTriangleArea(0.0f), pMin(0.0f), pMax(0.0f), centroid(0.0f), radius(0.0f) {}

	/// the number of vertices in the mesh
	physx::PxU32	numVertices;
	/// the number of indices in the mesh
	physx::PxU32	numIndices;
	/// the number of bones per vertex in the boneIndex and boneWeights buffer. Can be 0
	physx::PxU32	numBonesPerVertex;

	/// pointer to the positions array
	physx::PxVec3* PX_RESTRICT pPosition;
	/// pointer to the normals array
	physx::PxVec3* PX_RESTRICT pNormal;
	/// pointer to the tangents array
	physx::PxVec3* PX_RESTRICT pTangent;
	/// alternative pointer to the tangents array, with float4
	physx::PxVec4* PX_RESTRICT pTangent4;
	/// pointer to the bitangents/binormal array
	physx::PxVec3* PX_RESTRICT pBitangent;
	/// pointer to the bone indices array
	physx::PxU16* PX_RESTRICT pBoneIndices;
	/// pointer to the bone weights array
	physx::PxF32* PX_RESTRICT pBoneWeights;
	/// pointer to the cloth constraints array
	NxClothConstrainCoefficients* PX_RESTRICT pConstraints;
	/// pointer to per-vertex flags
	physx::PxU32* PX_RESTRICT pVertexFlags;
	/// pointer to the indices array
	physx::PxU32*	PX_RESTRICT pIndices;

	/// updates the derived data
	void UpdateDerivedInformation(NxApexRenderDebug* renderDebug);

	/// Derived Data, average Edge Length
	physx::PxF32 avgEdgeLength;
	/// Derived Data, average Triangle Area
	physx::PxF32 avgTriangleArea;
	/// Derived Data, Bounding Box min value
	physx::PxVec3 pMin;
	/// Derived Data, Bounding Box max value
	physx::PxVec3 pMax;
	/// Derived Data, Average of pMin and pMax
	physx::PxVec3 centroid;
	/// Derived Data, Half the distance between pMin and pMax
	physx::PxF32 radius;
};

}
}


#endif //_NX_ABSTRACT_MESH_DESCRIPTION_H