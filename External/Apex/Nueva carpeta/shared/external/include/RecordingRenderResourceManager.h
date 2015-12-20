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

#ifndef RECORDING_RENDER_RESOURCE_MANAGER_H
#define RECORDING_RENDER_RESOURCE_MANAGER_H

#include "NxUserRenderResourceManager.h"
#include "NxUserRenderer.h"

#include <string>

namespace physx
{
namespace apex
{
class NxApexRenderVertexBufferData;
//class NxApexRenderBoneBufferData; // not possible, d'oh
}
}

#include "NxUserRenderBoneBuffer.h"


class RecordingRenderResourceManager : public physx::apex::NxUserRenderResourceManager
{
public:

	class RecorderInterface
	{
	public:
		virtual ~RecorderInterface() {}

		virtual void createVertexBuffer(unsigned int id, const physx::apex::NxUserRenderVertexBufferDesc& desc) = 0;
		virtual void writeVertexBuffer(unsigned int id, const physx::apex::NxApexRenderVertexBufferData& data, unsigned int firstVertex, unsigned int numVertices) = 0;
		virtual void releaseVertexBuffer(unsigned int id) = 0;

		virtual void createIndexBuffer(unsigned int id, const physx::apex::NxUserRenderIndexBufferDesc& desc) = 0;
		virtual void writeIndexBuffer(unsigned int id, const void* srcData, physx::PxU32 srcStride, unsigned int firstDestElement, unsigned int numElements, physx::apex::NxRenderDataFormat::Enum format) = 0;
		virtual void releaseIndexBuffer(unsigned int id) = 0;

		virtual void createBoneBuffer(unsigned int id, const physx::apex::NxUserRenderBoneBufferDesc& desc) = 0;
		virtual void writeBoneBuffer(unsigned int id, const physx::apex::NxApexRenderBoneBufferData& data, unsigned int firstBone, unsigned int numBones) = 0;
		virtual void releaseBoneBuffer(unsigned int id) = 0;

		virtual void createResource(unsigned int id, const physx::apex::NxUserRenderResourceDesc& desc) = 0;
		virtual void renderResource(unsigned int id, const physx::apex::NxUserRenderResourceDesc& desc) = 0;
		virtual void releaseResource(unsigned int id) = 0;

		virtual void setMaxBonesForMaterial(void* material, unsigned int maxBones) = 0;

	};

	RecordingRenderResourceManager(physx::apex::NxUserRenderResourceManager* child, bool ownsChild, RecorderInterface* recorder);
	~RecordingRenderResourceManager();


	virtual physx::apex::NxUserRenderVertexBuffer*   createVertexBuffer(const physx::apex::NxUserRenderVertexBufferDesc& desc);
	virtual void                                     releaseVertexBuffer(physx::apex::NxUserRenderVertexBuffer& buffer);

	virtual physx::apex::NxUserRenderIndexBuffer*    createIndexBuffer(const physx::apex::NxUserRenderIndexBufferDesc& desc);
	virtual void                                     releaseIndexBuffer(physx::apex::NxUserRenderIndexBuffer& buffer);

	virtual physx::apex::NxUserRenderBoneBuffer*     createBoneBuffer(const physx::apex::NxUserRenderBoneBufferDesc& desc);
	virtual void                                     releaseBoneBuffer(physx::apex::NxUserRenderBoneBuffer& buffer);

	virtual physx::apex::NxUserRenderInstanceBuffer* createInstanceBuffer(const physx::apex::NxUserRenderInstanceBufferDesc& desc);
	virtual void                                     releaseInstanceBuffer(physx::apex::NxUserRenderInstanceBuffer& buffer);

	virtual physx::apex::NxUserRenderSpriteBuffer*   createSpriteBuffer(const physx::apex::NxUserRenderSpriteBufferDesc& desc);
	virtual void                                     releaseSpriteBuffer(physx::apex::NxUserRenderSpriteBuffer& buffer);

	virtual physx::apex::NxUserRenderResource*       createResource(const physx::apex::NxUserRenderResourceDesc& desc);

	virtual void                                     releaseResource(physx::apex::NxUserRenderResource& resource);

	virtual physx::PxU32                             getMaxBonesForMaterial(void* material);
protected:

	physx::apex::NxUserRenderResourceManager* mChild;
	bool mOwnsChild;

	RecorderInterface* mRecorder;
};


class RecordingRenderer : public physx::apex::NxUserRenderer
{
public:
	RecordingRenderer(physx::apex::NxUserRenderer* child, RecordingRenderResourceManager::RecorderInterface* recorder);
	virtual ~RecordingRenderer();

	virtual void renderResource(const physx::apex::NxApexRenderContext& context);

protected:
	physx::apex::NxUserRenderer* mChild;
	RecordingRenderResourceManager::RecorderInterface* mRecorder;
};


class FileRecorder : public RecordingRenderResourceManager::RecorderInterface
{
public:
	FileRecorder(const char* filename);
	~FileRecorder();

	virtual void createVertexBuffer(unsigned int id, const physx::apex::NxUserRenderVertexBufferDesc& desc);
	virtual void writeVertexBuffer(unsigned int id, const physx::apex::NxApexRenderVertexBufferData& data, unsigned int firstVertex, unsigned int numVertices);
	virtual void releaseVertexBuffer(unsigned int id);

	virtual void createIndexBuffer(unsigned int id, const physx::apex::NxUserRenderIndexBufferDesc& desc);
	virtual void writeIndexBuffer(unsigned int id, const void* srcData, physx::PxU32 srcStride, unsigned int firstDestElement, unsigned int numElements, physx::apex::NxRenderDataFormat::Enum format);
	virtual void releaseIndexBuffer(unsigned int id);

	virtual void createBoneBuffer(unsigned int id, const physx::apex::NxUserRenderBoneBufferDesc& desc);
	virtual void writeBoneBuffer(unsigned int id, const physx::apex::NxApexRenderBoneBufferData& data, unsigned int firstBone, unsigned int numBones);
	virtual void releaseBoneBuffer(unsigned int id);

	virtual void createResource(unsigned int id, const physx::apex::NxUserRenderResourceDesc& desc);
	virtual void renderResource(unsigned int id, const physx::apex::NxUserRenderResourceDesc& desc);
	virtual void releaseResource(unsigned int id);

	virtual void setMaxBonesForMaterial(void* material, unsigned int maxBones);

protected:
	void writeElem(const char* name, unsigned int value);

	void writeBufferData(const void* data, unsigned int stride, unsigned int numElements, physx::apex::NxRenderDataFormat::Enum format);
	void writeBufferDataFloat(const void* data, unsigned int stride, unsigned int numElements, unsigned int numFloatsPerDataSet);
	void writeBufferDataShort(const void* data, unsigned int stride, unsigned int numElements, unsigned int numFloatsPerDataSet);
	void writeBufferDataLong(const void* data, unsigned int stride, unsigned int numElements, unsigned int numFloatsPerDataSet);

	FILE* mOutputFile;
};


#endif // RECORDING_RENDER_RESOURCE_MANAGER_H
