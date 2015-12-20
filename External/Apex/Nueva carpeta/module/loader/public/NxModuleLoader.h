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

#ifndef NX_MODULE_LOADER_H
#define NX_MODULE_LOADER_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

/**
\brief The NxModuleLoader is a utility class for loading APEX modules

\note The most useful methods for rapid integration are "loadAllModules()",
      "loadAllLegacyModules()", and "releaseLoadedModules()".

\note If you need to release APEX modules loaded with NxModuleLoader you should use one of
      the release-methods provided below. Note that you may never need it because SDK automatically
	  releases all modules when program ends.
*/
class NxModuleLoader : public NxModule
{
public:
	/// \brief Load and initialize a specific APEX module
	virtual NxModule* loadModule(const char* name) = 0;

	/**
	\brief Load and initialize a list of specific APEX modules
	\param [in] modules The modules array must be the same size as the names array to
	                    support storage of every loaded NxModule pointer. Use NULL if
	                    you do not need the list of created modules.
	 */
	virtual void loadModules(const char** names, PxU32 size, NxModule** modules = 0) = 0;

	/// \brief Load and initialize all APEX modules
	virtual void loadAllModules() = 0;

	/// \brief Load and initialize all legacy APEX modules (useful for deserializing legacy assets)
	virtual void loadAllLegacyModules() = 0;

	/// \brief Returns the number of loaded APEX modules
	virtual physx::PxU32 getLoadedModuleCount() const = 0;

	/// \brief Returns the APEX module specified by the index if it was loaded by this NxModuleLoader
	virtual NxModule* getLoadedModule(physx::PxU32 idx) const = 0;

	/// \brief Returns the APEX module specified by the name if it was loaded by this NxModuleLoader
	virtual NxModule* getLoadedModule(const char* name) const = 0;

	/// \brief Releases the APEX module specified by the index if it was loaded by this NxModuleLoader
	virtual void releaseModule(physx::PxU32 idx) = 0;

	/// \brief Releases the APEX module specified by the name if it was loaded by this NxModuleLoader
	virtual void releaseModule(const char* name) = 0;

	/// \brief Releases the APEX module specified by the index if it was loaded by this NxModuleLoader
	virtual void releaseModules(NxModule** modules, PxU32 size) = 0;

	/**
	\brief Releases the specified APEX module
	\note If the NxModuleLoader is used to load modules, this method must be used to
	      release individual modules.
	      Do not use the NxModule::release() method.
	*/
	virtual void releaseModule(NxModule* module) = 0;

	/**
	\brief Releases the APEX modules specified in the names list
	\note If the NxModuleLoader is used to load modules, this method must be used to
	      release individual modules.
	      Do not use the NxModule::release() method.
	*/
	virtual void releaseModules(const char** names, PxU32 size) = 0;

	/// \brief Releases all APEX modules loaded by this NxModuleLoader
	virtual void releaseLoadedModules() = 0;
};

#if !defined(_USRDLL)
void instantiateModuleLoader();
#endif

}
} // end namespace physx::apex

#endif // NX_MODULE_LOADER_H
