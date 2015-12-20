/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#include "bulletblobs.h"
#include "btBulletCollisionCommon.h"

void btStridingMeshInterfaceBlobBuilder::DoBuild()
{
	const KyInt32 subPartCount = m_meshInterface->getNumSubParts();
	if (subPartCount)
	{
		Kaim::IndexedMeshBlob* partArray = BLOB_ARRAY(m_blob->m_meshParts, subPartCount);

		for (KyInt32 part = 0; part < subPartCount; part++)
		{
			const unsigned char * vertexbase;
			const unsigned char * indexbase;
			int indexstride, stride, numverts, numtriangles;
			PHY_ScalarType type, gfxindextype;
			m_meshInterface->getLockedReadOnlyVertexIndexBase(&vertexbase, numverts, type, stride, &indexbase, indexstride, numtriangles, gfxindextype, part);

			BLOB_ARRAY_COPY(partArray[part].m_indices, (KyUInt32*)indexbase, numtriangles*3);
			Kaim::Vec3f* vertices = BLOB_ARRAY(partArray[part].m_vertices, numverts);
			
			if(IsWriteMode())
			{
				for (KyInt32 i=0; i<numverts; i++)
				{
					KyFloat32* graphicsbase = (KyFloat32*)(vertexbase+i*stride);
					vertices[i][0] = graphicsbase[0];
					vertices[i][1] = graphicsbase[1];
					vertices[i][2] = graphicsbase[2];
				}
			}
		}
	}
	//m_scaling.serializeFloat(trimeshData->m_scaling);
}

#if 0
const char*	btQuantizedBvh::serialize(void* dataBuffer, btSerializer* serializer) const
{
	btQuantizedBvhData* quantizedData = (btQuantizedBvhData*)dataBuffer;

	m_bvhAabbMax.serialize(quantizedData->m_bvhAabbMax);
	m_bvhAabbMin.serialize(quantizedData->m_bvhAabbMin);
	m_bvhQuantization.serialize(quantizedData->m_bvhQuantization);

	quantizedData->m_curNodeIndex = m_curNodeIndex;
	quantizedData->m_useQuantization = m_useQuantization;

	quantizedData->m_numContiguousLeafNodes = m_contiguousNodes.size();
	quantizedData->m_contiguousNodesPtr = (btOptimizedBvhNodeData*) (m_contiguousNodes.size() ? serializer->getUniquePointer((void*)&m_contiguousNodes[0]) : 0);
	if (quantizedData->m_contiguousNodesPtr)
	{
		int sz = sizeof(btOptimizedBvhNodeData);
		int numElem = m_contiguousNodes.size();
		btChunk* chunk = serializer->allocate(sz,numElem);
		btOptimizedBvhNodeData* memPtr = (btOptimizedBvhNodeData*)chunk->m_oldPtr;
		for (int i=0;i<numElem;i++,memPtr++)
		{
			m_contiguousNodes[i].m_aabbMaxOrg.serialize(memPtr->m_aabbMaxOrg);
			m_contiguousNodes[i].m_aabbMinOrg.serialize(memPtr->m_aabbMinOrg);
			memPtr->m_escapeIndex = m_contiguousNodes[i].m_escapeIndex;
			memPtr->m_subPart = m_contiguousNodes[i].m_subPart;
			memPtr->m_triangleIndex = m_contiguousNodes[i].m_triangleIndex;
		}
		serializer->finalizeChunk(chunk,"btOptimizedBvhNodeData",BT_ARRAY_CODE,(void*)&m_contiguousNodes[0]);
	}

	quantizedData->m_numQuantizedContiguousNodes = m_quantizedContiguousNodes.size();
	//	printf("quantizedData->m_numQuantizedContiguousNodes=%d\n",quantizedData->m_numQuantizedContiguousNodes);
	quantizedData->m_quantizedContiguousNodesPtr =(btQuantizedBvhNodeData*) (m_quantizedContiguousNodes.size() ? serializer->getUniquePointer((void*)&m_quantizedContiguousNodes[0]) : 0);
	if (quantizedData->m_quantizedContiguousNodesPtr)
	{
		int sz = sizeof(btQuantizedBvhNodeData);
		int numElem = m_quantizedContiguousNodes.size();
		btChunk* chunk = serializer->allocate(sz,numElem);
		btQuantizedBvhNodeData* memPtr = (btQuantizedBvhNodeData*)chunk->m_oldPtr;
		for (int i=0;i<numElem;i++,memPtr++)
		{
			memPtr->m_escapeIndexOrTriangleIndex = m_quantizedContiguousNodes[i].m_escapeIndexOrTriangleIndex;
			memPtr->m_quantizedAabbMax[0] = m_quantizedContiguousNodes[i].m_quantizedAabbMax[0];
			memPtr->m_quantizedAabbMax[1] = m_quantizedContiguousNodes[i].m_quantizedAabbMax[1];
			memPtr->m_quantizedAabbMax[2] = m_quantizedContiguousNodes[i].m_quantizedAabbMax[2];
			memPtr->m_quantizedAabbMin[0] = m_quantizedContiguousNodes[i].m_quantizedAabbMin[0];
			memPtr->m_quantizedAabbMin[1] = m_quantizedContiguousNodes[i].m_quantizedAabbMin[1];
			memPtr->m_quantizedAabbMin[2] = m_quantizedContiguousNodes[i].m_quantizedAabbMin[2];
		}
		serializer->finalizeChunk(chunk,"btQuantizedBvhNodeData",BT_ARRAY_CODE,(void*)&m_quantizedContiguousNodes[0]);
	}

	quantizedData->m_traversalMode = int(m_traversalMode);
	quantizedData->m_numSubtreeHeaders = m_SubtreeHeaders.size();

	quantizedData->m_subTreeInfoPtr = (btBvhSubtreeInfoData*) (m_SubtreeHeaders.size() ? serializer->getUniquePointer((void*)&m_SubtreeHeaders[0]) : 0);
	if (quantizedData->m_subTreeInfoPtr)
	{
		int sz = sizeof(btBvhSubtreeInfoData);
		int numElem = m_SubtreeHeaders.size();
		btChunk* chunk = serializer->allocate(sz,numElem);
		btBvhSubtreeInfoData* memPtr = (btBvhSubtreeInfoData*)chunk->m_oldPtr;
		for (int i=0;i<numElem;i++,memPtr++)
		{
			memPtr->m_quantizedAabbMax[0] = m_SubtreeHeaders[i].m_quantizedAabbMax[0];
			memPtr->m_quantizedAabbMax[1] = m_SubtreeHeaders[i].m_quantizedAabbMax[1];
			memPtr->m_quantizedAabbMax[2] = m_SubtreeHeaders[i].m_quantizedAabbMax[2];
			memPtr->m_quantizedAabbMin[0] = m_SubtreeHeaders[i].m_quantizedAabbMin[0];
			memPtr->m_quantizedAabbMin[1] = m_SubtreeHeaders[i].m_quantizedAabbMin[1];
			memPtr->m_quantizedAabbMin[2] = m_SubtreeHeaders[i].m_quantizedAabbMin[2];

			memPtr->m_rootNodeIndex = m_SubtreeHeaders[i].m_rootNodeIndex;
			memPtr->m_subtreeSize = m_SubtreeHeaders[i].m_subtreeSize;
		}
		serializer->finalizeChunk(chunk,"btBvhSubtreeInfoData",BT_ARRAY_CODE,(void*)&m_SubtreeHeaders[0]);
	}
	return btQuantizedBvhDataName;
}
#endif
