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

#ifndef NX_CLOTHING_USER_RECOMPUTE_H
#define NX_CLOTHING_USER_RECOMPUTE_H

#include "NxApexUsingNamespace.h"
#include "NxRenderMeshAsset.h"
#include "NxAbstractMeshDescription.h"
#include "NxClothingPhysicalMesh.h"

namespace physx
{
namespace apex
{

class NxApexRenderDebug;

PX_PUSH_PACK_DEFAULT

/**
\brief Callback class for normal and tangent space update

Whenever normals and/or tangents need updating, the three recompute* methods are used. As long as a Clothing Actor does not have
a user defined subclaass of NxClothingUserRecompute, it will just take the default implementation.

\note These callback classes can be called from APEX threads, simultaneously on various Actors, during simulation or fetchResults.
In order to prevent from race conditions or crashes, no additional user's data should be referenced, or only be read!
*/
class NxClothingUserRecompute
{

public:

	NxClothingUserRecompute() {}
	virtual ~NxClothingUserRecompute() {}

	/**
	\brief A fast inverse square root. Mainly used to normalize vectors more quickly (and less accurately)
	*/
	inline static physx::PxF32 invSqrt(physx::PxF32 input)
	{
		const float halfInput = 0.5f * input;
		int         i     = *(int*)&input;

		i = 0x5f375a86 - (i >> 1);
		input = *(float*) & i;
		input = input * (1.5f - halfInput * input * input);
		return input;
	}

	/**
	\brief Called whenever only the normals of a mesh need updating
	*/
	virtual void recomputeRenderingNormals(NxAbstractMeshDescription& destMesh, const NxRenderMeshAsset* rendermesh, physx::PxU32 submeshIndex)
	{
		const physx::PxU32 numGraphicalVertices =			rendermesh->getSubmesh(submeshIndex).getVertexBuffer().getVertexCount();
		const physx::PxU32 numGraphicalVertexIndices =		rendermesh->getSubmesh(submeshIndex).getIndexCount(0);
		const physx::PxU32* indices =					rendermesh->getSubmesh(submeshIndex).getIndexBuffer(0);
		PX_ASSERT(numGraphicalVertices == destMesh.numVertices);
		PX_ASSERT(destMesh.pPosition != NULL);
		PX_ASSERT(destMesh.pNormal != NULL);
		PX_ASSERT(destMesh.pTangent == NULL);
		PX_ASSERT(destMesh.pBitangent == NULL);

		memset(destMesh.pNormal, 0, sizeof(physx::PxVec3) * destMesh.numVertices);

		for (physx::PxU32 i = 0; i < numGraphicalVertexIndices;)
		{
			const physx::PxU32 i0 = indices[i++];
			const physx::PxU32 i1 = indices[i++];
			const physx::PxU32 i2 = indices[i++];

			const physx::PxVec3& p0 = destMesh.pPosition[i0];
			const physx::PxVec3& p1 = destMesh.pPosition[i1];
			const physx::PxVec3& p2 = destMesh.pPosition[i2];

			physx::PxVec3 faceNormal = (p1 - p0).cross(p2 - p0);
			faceNormal *= invSqrt(faceNormal.magnitudeSquared());

			destMesh.pNormal[i0] += faceNormal;
			destMesh.pNormal[i1] += faceNormal;
			destMesh.pNormal[i2] += faceNormal;
		}

		for (physx::PxU32 i = 0; i < numGraphicalVertices; i++)
		{
			destMesh.pNormal[i] *= invSqrt(destMesh.pNormal[i].magnitudeSquared());
		}
	}

	/**
	\brief Called whenever only the tangent and binormal needs recomputing. This is the case when the normals are built from skinning already
	*/
	virtual void recomputeRenderingTangents(NxAbstractMeshDescription& destMesh, const NxRenderMeshAsset* rendermesh, physx::PxU32 uvChannel, physx::PxU32 submeshIndex)
	{
		const physx::PxU32 numGraphicalVertices =		rendermesh->getSubmesh(submeshIndex).getVertexBuffer().getVertexCount();
		const physx::PxU32 numGraphicalVertexIndices =	rendermesh->getSubmesh(submeshIndex).getIndexCount(0);
		const physx::PxU32* indices =					rendermesh->getSubmesh(submeshIndex).getIndexBuffer(0);

		const NxVertexBuffer& vertexBuffer = rendermesh->getSubmesh(submeshIndex).getVertexBuffer();
		NxVertexFormat::BufferID bufferID = 0;
		switch (uvChannel)
		{
		case 0:
			bufferID = vertexBuffer.getFormat().getSemanticID(NxRenderVertexSemantic::TEXCOORD0);
			break;
		case 1:
			bufferID = vertexBuffer.getFormat().getSemanticID(NxRenderVertexSemantic::TEXCOORD1);
			break;
		case 2:
			bufferID = vertexBuffer.getFormat().getSemanticID(NxRenderVertexSemantic::TEXCOORD2);
			break;
		case 3:
			bufferID = vertexBuffer.getFormat().getSemanticID(NxRenderVertexSemantic::TEXCOORD3);
			break;
		}
		if (bufferID == 0)
		{
			return;
		}
		PxU32 bufferIndex = vertexBuffer.getFormat().getBufferIndexFromID(bufferID);
		const NxVertexUV* uvs = (const NxVertexUV*)vertexBuffer.getBuffer(bufferIndex);
		NxRenderDataFormat::Enum uvFormat = vertexBuffer.getFormat().getBufferFormat(bufferIndex);
		if (uvFormat != NxRenderDataFormat::FLOAT2)
		{
			return;    // Only handling (float,float) uvs for now
		}

		PX_ASSERT(uvs != NULL);

		PX_ASSERT(numGraphicalVertices == destMesh.numVertices);
		PX_ASSERT(destMesh.pPosition != NULL);
		PX_ASSERT(destMesh.pNormal == NULL);
		PX_ASSERT(destMesh.pTangent != NULL);
		PX_ASSERT(destMesh.pBitangent != NULL);

		memset(destMesh.pTangent, 0, sizeof(physx::PxVec3) * destMesh.numVertices);
		memset(destMesh.pBitangent, 0, sizeof(physx::PxVec3) * destMesh.numVertices);

		for (physx::PxU32 i = 0; i < numGraphicalVertexIndices;)
		{
			const physx::PxU32 i0 = indices[i++];
			const physx::PxU32 i1 = indices[i++];
			const physx::PxU32 i2 = indices[i++];

			const physx::PxVec3& p0 = destMesh.pPosition[i0];
			const physx::PxVec3& p1 = destMesh.pPosition[i1];
			const physx::PxVec3& p2 = destMesh.pPosition[i2];

			const NxVertexUV& w0 = uvs[i0];
			const NxVertexUV& w1 = uvs[i1];
			const NxVertexUV& w2 = uvs[i2];


			const float s1 = w1.u - w0.u;
			const float s2 = w2.u - w0.u;
			const float t1 = w1.v - w0.v;
			const float t2 = w2.v - w0.v;

			const float invR = s1 * t2 - s2 * t1;
			if (invR != 0.0f)
			{
				const float r = 1.0F / invR;

				const float x1 = p1.x - p0.x;
				const float x2 = p2.x - p0.x;
				const float y1 = p1.y - p0.y;
				const float y2 = p2.y - p0.y;
				const float z1 = p1.z - p0.z;
				const float z2 = p2.z - p0.z;

				physx::PxVec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				physx::PxVec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				destMesh.pTangent[i0] += sdir;
				destMesh.pTangent[i1] += sdir;
				destMesh.pTangent[i2] += sdir;

				destMesh.pBitangent[i0] += tdir;
				destMesh.pBitangent[i1] += tdir;
				destMesh.pBitangent[i2] += tdir;
			}
		}

		for (physx::PxU32 i = 0; i < numGraphicalVertices; i++)
		{
			physx::PxVec3& t = destMesh.pTangent[i];
			physx::PxVec3& bt = destMesh.pBitangent[i];

			t *= invSqrt(t.magnitudeSquared());
			bt *= invSqrt(bt.magnitudeSquared());
		}

	}

	/**
	\brief Called whenever all components of the tangent space needs recomputing.
	*/
	virtual void recomputeRenderingTangentSpace(NxAbstractMeshDescription& destMesh, const NxRenderMeshAsset* rendermesh, physx::PxU32 uvChannel, physx::PxU32 submeshIndex)
	{
		const physx::PxU32 numGraphicalVertices =		rendermesh->getSubmesh(submeshIndex).getVertexBuffer().getVertexCount();
		const physx::PxU32 numGraphicalVertexIndices =	rendermesh->getSubmesh(submeshIndex).getIndexCount(0);
		const physx::PxU32* indices =					rendermesh->getSubmesh(submeshIndex).getIndexBuffer(0);

		const NxVertexBuffer& vertexBuffer = rendermesh->getSubmesh(submeshIndex).getVertexBuffer();
		const NxVertexFormat& vf = vertexBuffer.getFormat();
		NxVertexFormat::BufferID bufferID = 0;
		switch (uvChannel)
		{
		case 0:
			bufferID = vf.getSemanticID(NxRenderVertexSemantic::TEXCOORD0);
			break;
		case 1:
			bufferID = vf.getSemanticID(NxRenderVertexSemantic::TEXCOORD1);
			break;
		case 2:
			bufferID = vf.getSemanticID(NxRenderVertexSemantic::TEXCOORD2);
			break;
		case 3:
			bufferID = vf.getSemanticID(NxRenderVertexSemantic::TEXCOORD3);
			break;
		}
		NxRenderDataFormat::Enum uvFormat = NxRenderDataFormat::UNSPECIFIED;
		const NxVertexUV* uvs = NULL;
		if (bufferID != 0)
		{
			uvs = (const NxVertexUV*)vertexBuffer.getBufferAndFormat(uvFormat, vf.getBufferIndexFromID(bufferID));
		}
		if (uvFormat != NxRenderDataFormat::FLOAT2)
		{
			recomputeRenderingNormals(destMesh, rendermesh, submeshIndex);
			return;	// Only handling (float,float) uvs for now
		}

		PX_ASSERT(uvs != NULL);

		PX_ASSERT(numGraphicalVertices == destMesh.numVertices);
		PX_ASSERT(destMesh.pPosition != NULL);
		PX_ASSERT(destMesh.pNormal != NULL);
		PX_ASSERT(destMesh.pTangent != NULL);
		PX_ASSERT(destMesh.pBitangent != NULL);

		memset(destMesh.pNormal, 0, sizeof(physx::PxVec3) * destMesh.numVertices);

		memset(destMesh.pTangent, 0, sizeof(physx::PxVec3) * destMesh.numVertices);
		memset(destMesh.pBitangent, 0, sizeof(physx::PxVec3) * destMesh.numVertices);

		// add all face normals to each of their vertices' normals
		for (physx::PxU32 i = 0; i < numGraphicalVertexIndices;)
		{
			const physx::PxU32 i0 = indices[i++];
			const physx::PxU32 i1 = indices[i++];
			const physx::PxU32 i2 = indices[i++];

			const physx::PxVec3& p0 = destMesh.pPosition[i0];
			const physx::PxVec3& p1 = destMesh.pPosition[i1];
			const physx::PxVec3& p2 = destMesh.pPosition[i2];

			physx::PxVec3 faceNormal = (p1 - p0).cross(p2 - p0);
			faceNormal *= invSqrt(faceNormal.magnitudeSquared());

			destMesh.pNormal[i0] += faceNormal;
			destMesh.pNormal[i1] += faceNormal;
			destMesh.pNormal[i2] += faceNormal;

			const NxVertexUV& w0 = uvs[i0];
			const NxVertexUV& w1 = uvs[i1];
			const NxVertexUV& w2 = uvs[i2];

			const float x1 = p1.x - p0.x;
			const float x2 = p2.x - p0.x;
			const float y1 = p1.y - p0.y;
			const float y2 = p2.y - p0.y;
			const float z1 = p1.z - p0.z;
			const float z2 = p2.z - p0.z;

			const float s1 = w1.u - w0.u;
			const float s2 = w2.u - w0.u;
			const float t1 = w1.v - w0.v;
			const float t2 = w2.v - w0.v;

			const float invH = (s1 * t2 - s2 * t1);

			if (invH != 0.0f)
			{
				const float r = 1.0F / invH;
				physx::PxVec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				physx::PxVec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				destMesh.pTangent[i0] += sdir;
				destMesh.pTangent[i1] += sdir;
				destMesh.pTangent[i2] += sdir;

				destMesh.pBitangent[i0] += tdir;
				destMesh.pBitangent[i1] += tdir;
				destMesh.pBitangent[i2] += tdir;
			}
		}

		// normalize all vertex normals
		for (physx::PxU32 i = 0; i < numGraphicalVertices; i++)
		{
			destMesh.pNormal[i] *= invSqrt(destMesh.pNormal[i].magnitudeSquared());

			physx::PxVec3& t = destMesh.pTangent[i];
			physx::PxVec3& bt = destMesh.pBitangent[i];

			// orthonormalize tangents
			const physx::PxVec3& n = destMesh.pNormal[i];
			t -= n * n.dot(t);
			t *= invSqrt(t.magnitudeSquared());

			const physx::PxVec3 nxt = n.cross(t);
			const physx::PxF32 sign = bt.dot(nxt) < 0.0f ? -1.0f : 1.0f;
			bt = nxt * sign;
		}
	}
};


PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_CLOTHING_USER_RECOMPUTE_H
