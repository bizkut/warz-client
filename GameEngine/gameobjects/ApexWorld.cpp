//=========================================================================
//	Module: ApexWorld.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if APEX_ENABLED

#include "PhysXWorld.h"
#include "ApexWorld.h"
#include "ApexRenderer.h"
#include "ApexCommon.h"
#include "obj_Apex.hpp"
#include "ApexFileStream.h"

#ifdef _DEBUG
#pragma comment(lib, "ApexFrameworkCHECKED_x86.lib")
#elif defined(FINAL_BUILD)
#pragma comment(lib, "ApexFramework_x86.lib")
#else // RELEASE
#pragma comment(lib, "ApexFrameworkPROFILE_x86.lib")
#endif

//////////////////////////////////////////////////////////////////////////

extern int DisablePhysXSimulation;
extern UserRenderer apexUserRenderer;
UserRenderResourceManager userRenderMgr;

//////////////////////////////////////////////////////////////////////////

using namespace physx;
using namespace physx::apex;

namespace
{
	class MyOutputStream : public PxErrorCallback
	{
	public:
		virtual void reportError (PxErrorCode::Enum code, const char *message, const char* file, int line)
		{
			if (code == PxErrorCode::eDEBUG_WARNING)
				r3dOutToLog("Apex Warning: '%s' at file %s, line %d\n", message, file, line);
			else
				r3dError("Apex Error: '%s' at file %s, line %d\n", message, file, line);
		}
	};
	MyOutputStream apexOutputStream;

/////////////////////////////////////////////////////////////////////////

	class UserResourceCallback: public NxResourceCallback
	{
		void * RequestMaterial(const char *name)
		{
			char buf[256] = {0};
			strcpy_s(buf, _countof(buf), name);
			const char * matName = strtok(buf, ";");
			const char * depot = strtok(NULL, ";");
			return r3dMaterialLibrary::RequestMaterialByName(matName, "ApexObjects", false,true);// he tocado aki
		}

		//////////////////////////////////////////////////////////////////////////
		
		void * RequestDestructibleAsset(const char* name)
		{
//			physx::PxFileBuf *stream = g_pApexWorld->apexSDK->createStream(name, physx::apex::NxApexStreamFlags::OPEN_FOR_READ);
			physx::PxFileBuf *stream = game_new ApexFileStream(name, physx::general_PxIOStream2::PxFileBuf::OPEN_READ_ONLY);
			r3d_assert(stream);
			if (!stream) return 0;

			NxParameterized::Serializer::SerializeType serType = g_pApexWorld->apexSDK->getSerializeType(*stream);
			r3d_assert(serType != NxParameterized::Serializer::NST_LAST);
			if (serType == NxParameterized::Serializer::NST_LAST)
				return 0;

			NxParameterized::Serializer * ser = g_pApexWorld->apexSDK->createSerializer(serType);

			NxParameterized::Traits* t = g_pApexWorld->apexSDK->getParameterizedTraits();
			physx::PxU32 len = stream->getFileLength();

			NxApexAsset *asset = 0;
			NxParameterized::Serializer::DeserializedData data;

			if (ser)
			{
				void* p = t->alloc(len);
				stream->read(p, len);
				physx::PxFileBuf* memStream = g_pApexWorld->apexSDK->createMemoryReadStream(p, len);

				NxParameterized::Serializer::ErrorType serError = ser->deserialize(*memStream, data);

				g_pApexWorld->apexSDK->releaseMemoryReadStream(*memStream);
				t->free(p);

				if (serError == NxParameterized::Serializer::ERROR_NONE && data.size() == 1)
				{
					NxParameterized::Interface *params = data[0];
					asset = g_pApexWorld->apexSDK->createAsset(params, name);
				}
			}

			stream->release();
			ser->release();

			return asset;
		}

		//////////////////////////////////////////////////////////////////////////
		
		virtual void* requestResource(const char* nameSpace, const char* name)
		{
			if (strcmp(nameSpace, "ApexMaterials") == 0)
			{
				return RequestMaterial(name);
			}
			return RequestDestructibleAsset(name);

		}

		//////////////////////////////////////////////////////////////////////////
		
		virtual void releaseResource(const char* nameSpace, const char* name, void* resource)
		{
			if (strcmp(nameSpace, "ApexObject") == 0)
			{
				g_pApexWorld->apexSDK->releaseAsset(*reinterpret_cast<NxApexAsset*>(resource));
			}
		}
	};
	UserResourceCallback userResourceCallback;

} // unnamed namespace

ApexWorld *g_pApexWorld;

//////////////////////////////////////////////////////////////////////////

ApexWorld::ApexWorld()
: apexSDK(0)
, apexScene(0)
, r3dIResource(r3dIntegrityGuardian())
{
	
}

//////////////////////////////////////////////////////////////////////////

ApexWorld::~ApexWorld()
{
	r3d_assert(actors.Count() == 0);
}

//////////////////////////////////////////////////////////////////////////

physx::apex::NxModule * ApexWorld::InitModule(const char *moduleName)
{
	physx::apex::NxModule * module = 0;
	if (apexSDK)
	{
		module = apexSDK->createModule(moduleName);
		r3d_assert(module);
		if (module)
		{
			module->init(*module->getDefaultModuleDesc());
			modules.PushBack(module);
		}
	}

	return module;
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::Init()
{
	NxApexSDKDesc apexDesc;
	apexDesc.physXSDK = g_pPhysicsWorld->PhysXSDK;
	apexDesc.cooking = g_pPhysicsWorld->Cooking;
	apexDesc.outputStream = &apexOutputStream;

	apexDesc.resourceCallback = &userResourceCallback;
	apexDesc.renderResourceManager = &userRenderMgr;
	NxApexCreateError errorCode;
	apexSDK = NxCreateApexSDK(apexDesc, &errorCode);

	//r3dOutToLog("errorCode = %d\n",errorCode);
	r3d_assert(apexSDK);

	//	Create scene
	NxApexSceneDesc sceneDesc;
	sceneDesc.scene = g_pPhysicsWorld->PhysXScene;

	apexScene = apexSDK->createScene(sceneDesc);

	//	Create apex destruction module
	physx::apex::NxModuleDestructible * destrMod = static_cast<physx::apex::NxModuleDestructible*>(InitModule("Destructible"));
	if (destrMod)
	{
		destrMod->setLODEnabled(false);
	}

	InitModule("Common_Legacy");
	InitModule("Framework_Legacy");
	InitModule("Destructible_Legacy");

	apexScene->allocViewMatrix(physx::ViewMatrixType::LOOK_AT_LH);
	apexScene->allocProjMatrix(physx::ProjMatrixType::USER_CUSTOMIZED);
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::Destroy()
{
	for (uint32_t i = 0; i < modules.Count(); ++i)
	{
		physx::apex::NxModule *mod = modules[i];
		if (mod)
		{
			mod->release();
		}
	}
	modules.Clear();

	if (apexScene)
	{
		apexScene->setPhysXScene(0);
		apexScene->release();
		apexScene = 0;
	}

	if (apexSDK)
	{
		apexSDK->release();
		apexSDK = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::FetchResults(bool block)
{
	apexScene->fetchResults(block, 0);
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::Simulate(float timeStep, bool final)
{
	apexScene->simulate(timeStep, final);
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::ApplyAreaDamage(float damage, float momentum, const r3dPoint3D& position, float radius, bool falloff)
{
	for (uint32_t i = 0; i < actors.Count(); ++i)
	{
		ApexActorBase *aab = actors[i];
		if (aab)
			aab->ApplyAreaDamage(damage, momentum, position, radius, falloff);
	}
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::ApplyDamage(float damage, float momentum, const r3dPoint3D &pos, const r3dPoint3D &direction)
{
	for (uint32_t i = 0; i < actors.Count(); ++i)
	{
		ApexActorBase *aab = actors[i];
		if (aab)
			aab->ApplyDamage(damage, momentum, pos, direction);
	}
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::D3DCreateResource()
{
	for (uint32_t i = 0; i < actors.Count(); ++i)
	{
		ApexActorBase *aab = actors[i];
		if (aab)
			aab->RewriteBuffers();
	}
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::ConvertAllToDestructibles()
{
	//	Copy actors because convert routines will modify actors array.
	ActorsArray actorsCopy(actors);
	for (uint32_t i = 0; i < actorsCopy.Count(); ++i)
	{
		ApexActorBase *aab = actorsCopy[i];
		if (!aab || !aab->GetCreationParameters().userData) continue;

		obj_ApexDestructible *owner = reinterpret_cast<obj_ApexDestructible*>(aab->GetCreationParameters().userData);
		owner->ConvertToDestructible();
	}
}

//////////////////////////////////////////////////////////////////////////

void ApexWorld::ConvertAllToPreview()
{
	ActorsArray actorsCopy(actors);
	for (uint32_t i = 0; i < actorsCopy.Count(); ++i)
	{
		ApexActorBase *aab = actorsCopy[i];
		if (!aab || !aab->GetCreationParameters().userData) continue;

		obj_ApexDestructible *owner = reinterpret_cast<obj_ApexDestructible*>(aab->GetCreationParameters().userData);
		owner->ConvertToPreview();
	}
}

#endif