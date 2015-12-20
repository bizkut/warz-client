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
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
#ifndef SAMPLE_APEX_RESOURCE_CALLBACK_H
#define SAMPLE_APEX_RESOURCE_CALLBACK_H

#include <NxApexDefs.h>
#include <NxApexSDK.h>
#if NX_SDK_VERSION_MAJOR == 2
#include <Nxp.h>
#elif NX_SDK_VERSION_MAJOR == 3
#include <PxFiltering.h>
#endif
#include <NxResourceCallback.h>
#include <PxFileBuf.h>
#include "Find.h"
#include <SampleAsset.h>
#include <vector>

namespace physx
{
namespace apex
{
class NxApexSDK;
}
}

namespace SampleRenderer
{
class Renderer;
}

namespace SampleFramework
{
class SampleAssetManager;
}

// TODO: DISABLE ME!!!
#define WORK_AROUND_BROKEN_ASSET_PATHS 1

enum SampleAssetFileType
{
	XML_ASSET,
	BIN_ASSET,
	ANY_ASSET,
};

class SampleApexResourceCallback : public physx::apex::NxResourceCallback
{
public:
	SampleApexResourceCallback(SampleRenderer::Renderer& renderer, SampleFramework::SampleAssetManager& assetManager);
	virtual				   ~SampleApexResourceCallback(void);

	void					addResourceSearchPath(const char* path);
	void					removeResourceSearchPath(const char* path);
	void					clearResourceSearchPaths();

#if NX_SDK_VERSION_MAJOR == 2
	void					registerNxCollisionGroup(const char* name, NxCollisionGroup group);
	void					registerNxGroupsMask128(const char* name, NxGroupsMask& groupsMask);
	void					registerNxCollisionGroupsMask(const char* name, physx::PxU32 groupsBitMask);
	void					registerPhysicalMaterial(const char* name, NxMaterialIndex physicalMaterial);
#elif NX_SDK_VERSION_MAJOR == 3
	void					registerSimulationFilterData(const char* name, const physx::PxFilterData& simulationFilterData);
	void					registerPhysicalMaterial(const char* name, physx::PxMaterialTableIndex physicalMaterial);
#endif

	void					registerNxGroupsMask64(const char* name, physx::apex::NxGroupsMask64& groupsMask);

	void					setApexSupport(physx::apex::NxApexSDK& apexSDK);

	physx::PxFileBuf*   	findApexAsset(const char* assetName);
	void					findFiles(const char* dir, physx::apex::FileHandler& handler);

	void					setAssetPreference(SampleAssetFileType pref)
	{
		m_assetPreference = pref;
	}

	static bool				xmlFileExtension(const char* assetName);
	static const char*		getFileExtension(const char* assetName);

private:
	SampleFramework::SampleAsset*	findSampleAsset(const char* assetName, SampleFramework::SampleAsset::Type type);

#if WORK_AROUND_BROKEN_ASSET_PATHS
	const char*				mapHackyPath(const char* path);
#endif

public:
	virtual void*			requestResource(const char* nameSpace, const char* name);
	virtual void			releaseResource(const char* nameSpace, const char* name, void* resource);

	bool					doesFileExist(const char* filename, const char* ext);
	bool					doesFileExist(const char* filename);

protected:
	SampleRenderer::Renderer&				m_renderer;
	SampleFramework::SampleAssetManager&	m_assetManager;
	std::vector<char*>						m_searchPaths;
#if NX_SDK_VERSION_MAJOR == 2
	std::vector<NxGroupsMask>				m_nxGroupsMasks;
#elif NX_SDK_VERSION_MAJOR == 3
	std::vector<physx::PxFilterData>		m_FilterDatas;
#endif
	std::vector<physx::apex::NxGroupsMask64>	m_nxGroupsMask64s;

	physx::apex::NxApexSDK*					m_apexSDK;
	physx::PxU32							m_numGets;
	SampleAssetFileType						m_assetPreference;
};

#endif // SAMPLE_APEX_RESOURCE_CALLBACK_H
