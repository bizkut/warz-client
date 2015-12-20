/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: MUBI
#ifndef GwNavGen_GeneratorBaseSystem_H
#define GwNavGen_GeneratorBaseSystem_H

#include "gwnavruntime/basesystem/basesystem.h"
#include "gwnavgeneration/common/generatormemory.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavruntime/blob/blobregistry.h"

namespace Kaim
{

/// The GeneratorBaseSystem class is a helper tool that enable the user to correctly initialize the BaseSystem for 
/// generating the NavData.
/// There are two ways to use GeneratorBaseSystem, either using static function Init() and Destroy() or by creating a BaseSystem instance.
/// When creating an instance, Init() is called in the constructor and Destroy() is called in the destructor.
class GeneratorBaseSystem
{
public:
	typedef BaseSystemConfig Config; ///< Defines a type for the configuration object used to initialize the GeneratorBaseSystem.

public:
	/// Creates and sets up the new BaseSystem with a default configuration. 
	GeneratorBaseSystem()
	{
		BaseSystemConfig config;
		Init(config);
	}

	/// Creates and sets up the BaseSystem according to the specified configuration. 
	GeneratorBaseSystem(BaseSystemConfig& config) { Init(config); }

	/// Calls static function BaseSystem::Destroy()
	~GeneratorBaseSystem() { BaseSystem::Destroy(); }

	/// Initialize
	static void Init(BaseSystemConfig& config)
	{
		if (config.m_sysAlloc == KY_NULL)
			config.m_sysAlloc = GeneratorSysAllocMalloc::InitSystemSingleton();

		// Init the BaseSystem
		BaseSystem::Init(config);
		if (BaseSystem::IsInitialized() == false)
			return;

		// Register gwnavgeneration.lib specific Blobs.
		// This is not done in BaseSystem::Init() because BaseSystem::Init() is part of gwnavruntime.lib,
		// and gwnavgeneration.lib depend on gwnavruntime.lib.
		GeneratorCategoryBuilder generatorCategoryBuilder;
		BlobRegistry::AddCategory(&generatorCategoryBuilder);
	}

	/// Indicates whether or not the BaseSystem has been successfully initialized. 
	static bool IsInitialized() { return BaseSystem::IsInitialized(); }

	/// Indicates whether or not any memory allocated by the GeneratorBaseSystem remains to be freed.
	/// Call this method only after you destroy the BaseSystem. 
	static bool HasMemoryLeaks() { return BaseSystem::IsInitialized(); }


};

} // namespace Kaim


#endif
