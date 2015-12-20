//=========================================================================
//	Module: AutodeskNavMesh.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "shellapi.h"

#if ENABLE_AUTODESK_NAVIGATION

#ifdef WO_SERVER
  #ifdef USE_R3D_MEMORY_ALLOCATOR
    #error("somehow USE_R3D_MEMORY_ALLOCATOR is enabled")
  #endif
#endif

#include "AutodeskNavMesh.h"
#include "AutodeskNavAgent.h"
#include "AutodeskNavProfile.h"
#include "GameLevel.h"
#include "../../EclipseStudio/Sources/XMLHelpers.h"
#include "r3dBackgroundTaskDispatcher.h"
#include "../NavGenerationHelpers.h"

#ifndef WO_SERVER
#include "gwnavgenerationglue/defaultguidgeneratorinterface/defaultguidgeneratorinterface.h"
#include "gwnavgenerationglue/defaultgeneratorglue.h"
#endif

#define NAMESPACE_NAME Kaim
#define MEMORY_TYPE ALLOC_TYPE_NAVIGATION
#include "../../AutodeskMemoryAllocator.h"
#undef MEMORY_TYPE
#undef NAMESPACE_NAME

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
  // client
  #ifdef FINAL_BUILD
    #error "FINAL_BUILD must not have AutodeskNav library"
  #endif

  #ifdef KY_BUILD_DEBUG
    #pragma comment(lib, "../external/AutodeskNav/lib/win32_vc_9.debug-mt/gwnavgeneration.lib")
    #pragma comment(lib, "../external/AutodeskNav/lib/win32_vc_9.debug-mt/gwnavruntime.lib")
  #elif defined KY_BUILD_RELEASE
    #pragma comment(lib, "../external/AutodeskNav/lib/win32_vc_9.release-mt/gwnavgeneration.lib")
    #pragma comment(lib, "../external/AutodeskNav/lib/win32_vc_9.release-mt/gwnavruntime.lib")
  #elif defined KY_BUILD_SHIPPING
    #pragma comment(lib, "../external/AutodeskNav/lib/win32_vc_9.shipping-mt/gwnavgeneration.lib")
    #pragma comment(lib, "../external/AutodeskNav/lib/win32_vc_9.shipping-mt/gwnavruntime.lib")
  #elif
    #error("navigation build version is not specified")
  #endif

#else
  // server
  #ifdef KY_BUILD_DEBUG
    #pragma comment(lib, "../../../src/external/AutodeskNav/lib/win32_vc_9.debug-mt/gwnavgeneration.lib")
    #pragma comment(lib, "../../../src/external/AutodeskNav/lib/win32_vc_9.debug-mt/gwnavruntime.lib")
  #elif defined KY_BUILD_RELEASE
    #pragma comment(lib, "../../../src/external/AutodeskNav/lib/win32_vc_9.release-mt/gwnavgeneration.lib")
    #pragma comment(lib, "../../../src/external/AutodeskNav/lib/win32_vc_9.release-mt/gwnavruntime.lib")
  #elif defined KY_BUILD_SHIPPING
    #pragma comment(lib, "../../../src/external/AutodeskNav/lib/win32_vc_9.shipping-mt/gwnavgeneration.lib")
    #pragma comment(lib, "../../../src/external/AutodeskNav/lib/win32_vc_9.shipping-mt/gwnavruntime.lib")
  #elif
    #error("navigation build version is not specified")
  #endif

#endif


//////////////////////////////////////////////////////////////////////////

AutodeskNavMesh gAutodeskNavMesh;
extern r3dCamera gCam;

//////////////////////////////////////////////////////////////////////////

namespace
{
	const char * const BUILD_CFG_FILE_NAME = "\\build_config.xml";

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
	struct BuildParams: public r3dTaskParams
	{
		Kaim::GeneratorParameters params;
		BuildParams() {}
	};

	/**	Separate thread navmesh build function. */
	void ThreadBuildFunc(r3dTaskParams *params)
	{
		BuildParams *p = static_cast<BuildParams*>(params);

		char curDir[MAX_PATH];
		::GetCurrentDirectory(sizeof(curDir), curDir);
		
		r3dPurgeArtBugs();

		AutodeskGeneratorInputProducer producer;
		Nav::gConvexRegionsManager.FillSeedPoints(producer.seedPoints);

		Kaim::Generator generator(&producer);
		generator.SetOutputDirectory(curDir, Nav::GetNavDataFolderPath().c_str());
		generator.Sys().RunOptions().m_doEnableLimitedMemoryMode = true;
		
		//	Create generator
		Kaim::GeneratorSector levelSector(Kaim::KyGuid("51BDD88A-AD2E-450f-AC82-12281206DF63"), "Level");

		Kaim::GeneratorInputOutput config;
		config.m_params = p->params;
		config.AddSector(levelSector);

		{
			r3dCSHolder block(Nav::gDrawBuildInfo.buildInfoStringGuard);
			sprintf_s(Nav::gDrawBuildInfo.buildInfoString, _countof(Nav::gDrawBuildInfo.buildInfoString), "%s", "Building autodesk navigation mesh");
		}

		if (Kaim::Result::Fail(generator.Generate(config)))
		{
			return;
		}
		
		r3dShowArtBugs();
	}
#endif // #ifndef WO_SERVER
}

//////////////////////////////////////////////////////////////////////////

AutodeskNavMesh::AutodeskNavMesh()
: m_visualDebugPort(48888)
, world(0)
, doDebugDraw(false)
, initialized(false)
{

}

//////////////////////////////////////////////////////////////////////////

AutodeskNavMesh::~AutodeskNavMesh()
{
	Close();
}

//////////////////////////////////////////////////////////////////////////

bool AutodeskNavMesh::Init(USHORT visualDebugPort/*=48888*/)
{
	Close();

	m_visualDebugPort = visualDebugPort;

	m_coordSystem.Setup(1.0f, Kaim::CLIENT_X, Kaim::CLIENT_Z, Kaim::CLIENT_Y);

	Kaim::BaseSystem::Config config;
	config.m_gamewareKeyConfig.m_gamewareKey = "OZ5RFEPBLCC46S2WUS7G0WD7A4QWECM0EQSP4GYT";

	config.m_log = KY_NULL;	
	config.m_defaultLogToMode = (Kaim::DefaultLog::LogToMode) (Kaim::DefaultLog::LogToStdout | Kaim::DefaultLog::LogToIde);
#ifdef WO_SERVER	
	config.m_sysAlloc = KY_NULL; // use default allocator for server runtime!
#else
	#ifdef USE_R3D_MEMORY_ALLOCATOR
		static Kaim::r3dAutodeskAlloc gNavMeshAlloc;
		config.m_sysAlloc = &gNavMeshAlloc;
	#else
		config.m_sysAlloc = Kaim::GeneratorSysAllocMalloc::InitSystemSingleton();
	#endif
 	config.m_perfMarkerInterface = &perfBridge;
#endif	

	Kaim::BaseSystem::Init(config);
	if (Kaim::BaseSystem::IsInitialized() == false)
	{
		Kaim::BaseSystem::Destroy();
		r3dError("Failed to initialize AutodeskNavigation");
		return false;
	}

	LoadPathData();

	initialized = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::SetupVisualDebug()
{
	ShutdownVisualDebug();

	Kaim::VisualDebugServerConfig visualDebugServerConfig;
	visualDebugServerConfig.UseNavigationLab(m_visualDebugPort);
	visualDebugServerConfig.SetWaitOnStart(Kaim::VisualDebugServerConfig::DoNotWaitOnStart);
	KyResult startResult = world->StartVisualDebug(visualDebugServerConfig);
	if (KY_FAILED(startResult))
	{
		r3dOutToLog("AutodeskNav: Failed to start visual debug\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::ShutdownVisualDebug()
{
	if (world)
		world->StopVisualDebug();
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::LoadBuildConfig()
{
	r3dString filePath = Nav::GetNavDataFolderPath() + BUILD_CFG_FILE_NAME;

	r3dFile* f = r3d_open(filePath.c_str(), "r");
	if (!f) return;

	r3dTL::TArray<char> fileBuffer(f->size + 1);

	fread(&fileBuffer[0], f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlDoc;
	pugi::xml_parse_result parseResult = xmlDoc.load_buffer_inplace(&fileBuffer[0], f->size);
	fclose(f);
	if (!parseResult) return;

	pugi::xml_node bp = xmlDoc.child("build_config");
	GetXMLVal("entity_height", bp, &buildGlobalConfig.m_entityHeight);
	GetXMLVal("entity_radius", bp, &buildGlobalConfig.m_entityRadius);
	GetXMLVal("step_max", bp, &buildGlobalConfig.m_stepMax);
	GetXMLVal("slope_max", bp, &buildGlobalConfig.m_slopeMax);
	GetXMLVal("raster_precision", bp, &buildGlobalConfig.m_rasterPrecision);
	GetXMLVal("cell_size", bp, &buildGlobalConfig.m_cellSize);
	GetXMLVal("altitude_tolerance", bp, &buildGlobalConfig.m_altitudeTolerance);
	GetXMLVal("verticel_sampling_step", bp, &buildGlobalConfig.m_advancedParameters.m_altitudeToleranceSamplingStep);

	Nav::gConvexRegionsManager.Load(xmlDoc);
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::SaveBuildConfig()
{
	char curDir[MAX_PATH];
	::GetCurrentDirectory(sizeof(curDir), curDir);
	r3dString path = r3dString(curDir) + "\\" + Nav::GetNavDataFolderPath();

	SHCreateDirectoryEx(0, path.c_str(), 0);

	r3dString filePath = Nav::GetNavDataFolderPath() + BUILD_CFG_FILE_NAME;

	pugi::xml_document xmlDoc;

	pugi::xml_node bp = xmlDoc.append_child();
	bp.set_name("build_config");

	SetXMLVal("entity_height", bp, &buildGlobalConfig.m_entityHeight);
	SetXMLVal("entity_radius", bp, &buildGlobalConfig.m_entityRadius);
	SetXMLVal("step_max", bp, &buildGlobalConfig.m_stepMax);
	SetXMLVal("slope_max", bp, &buildGlobalConfig.m_slopeMax);
	SetXMLVal("raster_precision", bp, &buildGlobalConfig.m_rasterPrecision);
	SetXMLVal("cell_size", bp, &buildGlobalConfig.m_cellSize);
	SetXMLVal("altitude_tolerance", bp, &buildGlobalConfig.m_altitudeTolerance);
	SetXMLVal("verticel_sampling_step", bp, &buildGlobalConfig.m_advancedParameters.m_altitudeToleranceSamplingStep);

	Nav::gConvexRegionsManager.Save(xmlDoc);

	xmlDoc.save_file(filePath.c_str());
	
	SaveNavGenProj();
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::SaveNavGenProj()
{
	mkdir(Nav::GetNavDataFolderPath().c_str());

	char baseDir[MAX_PATH];
	::GetCurrentDirectory(sizeof(baseDir), baseDir);

	r3dTL::TArray<r3dVector> seedPoints;
	Nav::gConvexRegionsManager.FillSeedPoints(seedPoints);

	r3dString filePath = Nav::GetNavDataFolderPath() + "/Level.NavGenProj";
	FILE* f = fopen_for_write(filePath.c_str(), "wt");
	fprintf(f, "<?xml version=\"1.0\" ?>\n");
	fprintf(f, "<outputDir>\n");
	fprintf(f, "    <baseDir>%s</baseDir>\n", baseDir);
	fprintf(f, "    <relativeDir>%s</relativeDir>\n", Nav::GetNavDataFolderPath().c_str());
	fprintf(f, "</outputDir>\n");
	fprintf(f, "<config>\n");
	fprintf(f, "    <entityHeight>%f</entityHeight>\n", buildGlobalConfig.m_entityHeight);
	fprintf(f, "    <entityRadius>%f</entityRadius>\n", buildGlobalConfig.m_entityRadius);
	fprintf(f, "    <stepMax>%f</stepMax>\n", buildGlobalConfig.m_stepMax);
	fprintf(f, "    <slopeMax>%f</slopeMax>\n", buildGlobalConfig.m_slopeMax);
	fprintf(f, "    <rasterPrecision>%f</rasterPrecision>\n", buildGlobalConfig.m_rasterPrecision);
	fprintf(f, "    <cellSize>%f</cellSize>\n", buildGlobalConfig.m_cellSize);
	fprintf(f, "    <altitudeTolerance>%f</altitudeTolerance>\n", buildGlobalConfig.m_altitudeTolerance);
	fprintf(f, "    <doUseAltitudeTolerance>1</doUseAltitudeTolerance>\n");
	fprintf(f, "    <backFaceTrianglesWalkable>1</backFaceTrianglesWalkable>\n");
	fprintf(f, "    <altitudeToleranceSamplingStep>%f</altitudeToleranceSamplingStep>\n", buildGlobalConfig.m_advancedParameters.m_altitudeToleranceSamplingStep);
	fprintf(f, "    <noiseReductionSurface>1.000000</noiseReductionSurface>\n");
	fprintf(f, "    <minNavigableSurface>10.000000</minNavigableSurface>\n");
	fprintf(f, "    <navTagMinPixelArea>6</navTagMinPixelArea>\n");
	fprintf(f, "    <navTagRasterMergeTolerance>0.100000</navTagRasterMergeTolerance>\n");
	fprintf(f, "    <navmeshSimplificationTolerance>0.000000</navmeshSimplificationTolerance>\n");
	fprintf(f, "    <navRasterDistanceMapMetric>Manhattan</navRasterDistanceMapMetric>\n");
	fprintf(f, "    <useMultiCore>TRUE</useMultiCore>\n");
	fprintf(f, "    <doEnableLimitedMemoryMode>FALSE</doEnableLimitedMemoryMode>\n");
	fprintf(f, "    <outputFilesAreLittleEndian>FALSE</outputFilesAreLittleEndian>\n");
	fprintf(f, "    <visualDebug-use>FALSE</visualDebug-use>\n");
	fprintf(f, "    <visualDebug-file></visualDebug-file>\n");
	fprintf(f, "    <IntermediateData>\n");
	fprintf(f, "        <Write>FALSE</Write>\n");
	fprintf(f, "    </IntermediateData>\n");
	fprintf(f, "    <CoordSystem>\n");
	fprintf(f, "        <OneMeter>1.000000</OneMeter>\n");
	fprintf(f, "        <X>CLIENT_X</X>\n");
	fprintf(f, "        <Y>CLIENT_Z</Y>\n");
	fprintf(f, "        <Z>CLIENT_Y</Z>\n");
	fprintf(f, "    </CoordSystem>\n");
	fprintf(f, "</config>\n");
	fprintf(f, "<sector>\n");
	fprintf(f, "    <guid>2E489F85-E573-7D4A-BAB3-1F1254E34D17</guid>\n");
	fprintf(f, "    <doBuild>TRUE</doBuild>\n");
	fprintf(f, "    <geometries>\n");
	fprintf(f, "        <geometry>\n");
	fprintf(f, "            <filepath>%s\\%s\\level.obj</filepath>\n", baseDir, Nav::GetNavDataFolderPath().c_str());
	fprintf(f, "        </geometry>\n");
	fprintf(f, "    </geometries>\n");
	fprintf(f, "</sector>\n");

	for (uint32_t i = 0; i < seedPoints.Count(); ++i)
	{
		fprintf(f, "<seedpoint>\n");
		fprintf(f, "    <X>%f</X>\n", seedPoints[i].x);
		fprintf(f, "    <Y>%f</Y>\n", seedPoints[i].y);
		fprintf(f, "    <Z>%f</Z>\n", seedPoints[i].z);
		fprintf(f, "</seedpoint>\n");
	}
	fclose(f);
}

//////////////////////////////////////////////////////////////////////////

bool AutodeskNavMesh::LoadPathData()
{
#ifndef WO_SERVER
	LoadBuildConfig();
#endif

	RemovePathData();

	r3dTL::TArray<r3dString> paths;

	WIN32_FIND_DATA ffblk;
	HANDLE h = FindFirstFile((Nav::GetNavDataFolderPath() + "/*.NavData").c_str(), &ffblk);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			paths.PushBack(Nav::GetNavDataFolderPath() + "/" + ffblk.cFileName);
		}
		while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}
	if (paths.Count() == 0)
		return false;

	world = *KY_NEW Kaim::World(1);

	// Add Navigation Profiles
	Kaim::Ptr<AutodeskNavProfileSuperZombie> navProfile = *KY_NEW AutodeskNavProfileSuperZombie;
	m_NavProfileIdSuperZombie = world->AddNavigationProfile(navProfile);

	// set new budget for all navmesh queries - default budget for single world QueryQueue is 1ms, which is way too low
	for(KyUInt32 i = 0; i < world->GetQueryQueueArraysCount(); i++)
	{
		Kaim::QueryQueueArray* qqa = world->GetQueryQueueArray(i);
		for(KyUInt32 j = 0; j < qqa->GetQueueCount(); j++)
		{
			Kaim::QueryQueue* qq = qqa->GetQueue(j);
			qq->SetBudgetMs(BUDGET_NAVQUERIES_MS);
		}
	}

	navData = *KY_NEW Kaim::NavData(world->GetDatabase(0));

	for (uint32_t i = 0; i < paths.Count(); ++i)
	{
		Kaim::Ptr<Kaim::File> f = *KY_HEAP_NEW_ID(Kaim::Memory::GetGlobalHeap(), Kaim::MemStat_LabEngine) Kaim::SysFile(paths[i].c_str());
		KyResult loadingResult = navData->Load(f);
		if (KY_FAILED(loadingResult))
		{
			r3dError("failed to load navdata %s : %d\n", paths[i].c_str(), loadingResult);
			return false;
		}
	}

	KyResult rv = navData->AddToDatabaseImmediate();
	r3d_assert(KY_SUCCEEDED(rv));

#ifndef WO_SERVER
	SetupVisualDebug();
#endif // WO_SERVER

	return true;
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::RemovePathData()
{
	navDraw.Clear();

	if (navData)
	{
		navData->RemoveFromDatabaseImmediate();
	}
	navData = KY_NULL;
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::Close()
{
	if (!initialized)
		return;

 	for (uint32_t i = 0; i < obstacles.Count(); ++i)
 	{
 		RemoveObstacle(static_cast<int>(i));
 	}
 	obstacles.Clear();

	for (uint32_t i = 0; i < agents.Count(); ++i)
	{
		delete agents[i];
	}
	agents.Clear();

	RemovePathData();

	ShutdownVisualDebug();
	world = KY_NULL;

	Kaim::BaseSystem::Destroy();

	initialized = false;
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::Update()
{
	if (!world)
		return;

#ifndef FINAL_BUILD
	doDebugDraw = d_navmesh_debug->GetBool();
#endif

	if(world->GetVisualDebugServer())
		world->GetVisualDebugServer()->NewFrame();

	float timeStep = r3dGetFrameTime();

	// in their samples they first update bots, then world.
	for (uint32_t i = 0; i < agents.Count(); ++i)
	{
		if (agents[i])
			agents[i]->Update(timeStep);
	}

	world->Update(timeStep);
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::ClearNavDataDir()
{
	WIN32_FIND_DATA  ffblk;
	r3dTL::TArray<r3dString> files;
	HANDLE h = FindFirstFile((Nav::GetNavDataFolderPath() + "/*").c_str(), &ffblk);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			files.PushBack(Nav::GetNavDataFolderPath() + "/" + ffblk.cFileName);
		}
		while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}
	
	for (uint32_t i = 0; i < files.Count(); ++i)
	{
		DeleteFile(files[i].c_str());
	}
}

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
void AutodeskNavMesh::BuildForCurrentLevel()
{
	ClearNavDataDir();

	SaveBuildConfig();

	r3dBackgroundTaskDispatcher::TaskDescriptor td;

	td.Fn = &ThreadBuildFunc;
	Nav::gNavMeshBuildComplete = 0;
	td.CompletionFlag = &Nav::gNavMeshBuildComplete;

	static BuildParams bp;
	bp.params = buildGlobalConfig;
	td.Params = &bp;

	g_pBackgroundTaskDispatcher->AddTask(td);
}
#endif

//////////////////////////////////////////////////////////////////////////

int AutodeskNavMesh::AddObstacle(GameObject* obstacle, const r3dBoundBox &bb, float rotX)
{
	if (!world)
		return -1;

	Kaim::Ptr<Kaim::BoxObstacle> boxObstacle = *KY_NEW Kaim::BoxObstacle;
	Kaim::BoxObstacleInitConfig boxObstacleInitConfig;
	boxObstacleInitConfig.m_world = world;

	r3dPoint3D c = bb.Center();
	boxObstacleInitConfig.m_startPosition = Kaim::Vec3f(c.x, c.z, c.y);
	boxObstacleInitConfig.m_rotationMode = Kaim::BoxObstacleRotation_Yaw;
	boxObstacleInitConfig.m_localHalfExtents = Kaim::Vec3f(bb.Size.x, bb.Size.z, bb.Size.y) * 0.5f;
	boxObstacleInitConfig.m_navTag.SetAsExclusive();
	boxObstacleInitConfig.m_userData = (void*)obstacle;
	boxObstacle->Init(boxObstacleInitConfig);

	Kaim::BoxObstacleUpdateConfig updateCfg;
	updateCfg.m_angularVelocity = Kaim::Vec3f::Zero();
	updateCfg.m_linearVelocity = Kaim::Vec3f::Zero();
	updateCfg.m_transform.m_rotationMatrix.SetRotation(Kaim::Vec3f(0, 0, 1), R3D_DEG2RAD(-rotX)); // sign flip is needed here by some reason
	updateCfg.m_transform.m_translation = boxObstacleInitConfig.m_startPosition;
	boxObstacle->SetNextUpdateConfig(updateCfg);

	boxObstacle->AddToWorld();

	int idx = GetFreeObstacleIdx();
	obstacles[idx] = boxObstacle;
	
	return idx;
}

//////////////////////////////////////////////////////////////////////////

int AutodeskNavMesh::AddObstacle(GameObject* obstacle, const r3dCylinder &c)
{
	if (!world)
		return -1;

	Kaim::Ptr<Kaim::CylinderObstacle> cylinderObstacle = *KY_NEW Kaim::CylinderObstacle;
	Kaim::CylinderObstacleInitConfig cylinderObstacleInitConfig;
	cylinderObstacleInitConfig.m_world = world;
	cylinderObstacleInitConfig.m_startPosition = Kaim::Vec3f(c.vBase.x, c.vBase.z, c.vBase.y);
	cylinderObstacleInitConfig.m_height = c.vAltitude.y;
	cylinderObstacleInitConfig.m_radius = c.fRadius;
	cylinderObstacleInitConfig.m_userData = (void*)obstacle;
	cylinderObstacle->Init(cylinderObstacleInitConfig);

	cylinderObstacle->AddToWorld();

	Kaim::CylinderObstacleUpdateConfig uc;
	uc.m_position = cylinderObstacleInitConfig.m_startPosition;
	uc.m_velocity = Kaim::Vec3f::Zero();

	cylinderObstacle->SetNextUpdateConfig(uc);

	int idx = GetFreeObstacleIdx();
	obstacles[idx] = cylinderObstacle;

	return idx;

}

//////////////////////////////////////////////////////////////////////////

int AutodeskNavMesh::GetFreeObstacleIdx()
{
	int k = static_cast<int>(obstacles.Count());
	for (int i = 0; i < k; ++i)
	{
		if (!obstacles[i])
		{
			return i;
		}
	}
	obstacles.Resize(obstacles.Count() + 1);
	obstacles.GetLast() = 0;
	return k;
}

//////////////////////////////////////////////////////////////////////////

bool AutodeskNavMesh::RemoveObstacle(int idx)
{
	if (idx == -1 || idx >= static_cast<int>(obstacles.Count()) || !obstacles[idx])
		return false;

	Kaim::WorldElement *e = obstacles[idx];
	if (e->GetType() == Kaim::TypeBoxObstacle)
	{
		Kaim::BoxObstacle *o = static_cast<Kaim::BoxObstacle*>(e);
		o->RemoveFromWorld();
	}
	else if (e->GetType() == Kaim::TypeCylinderObstacle)
	{
		Kaim::CylinderObstacle *o = static_cast<Kaim::CylinderObstacle*>(e);
		o->RemoveFromWorld();
	}

	obstacles[idx] = KY_NULL;
	return true;
}

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
void AutodeskNavMesh::DebugDraw()
{
#ifndef FINAL_BUILD
	if (!doDebugDraw)
		return;

	if (!navDraw.IsFilled() && world)
	{
		Kaim::Database *db = world->GetDatabase(0);
		db->SetVisualGeometry(&navDraw);
		db->BuildVisualGeometry();
	}
	navDraw.Render();

	//	Agents
	for (uint32_t i = 0; i < agents.Count(); ++i)
	{
		AutodeskNavAgent * a = agents[i];
		if (a)
		{
			a->DebugDraw();
		}
	}

	DebugDrawObstacles();
#endif
}
#endif

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
void AutodeskNavMesh::DebugDrawObstacles()
{
	//	Obstacles
	for (uint32_t i = 0; i < obstacles.Count(); ++i)
	{
		Kaim::WorldElement *e = obstacles[i];
		if (e)
		{
			Kaim::WorldElementType t = e->GetType();
			if (t == Kaim::TypeBoxObstacle)
			{
				Kaim::BoxObstacle *o = static_cast<Kaim::BoxObstacle*>(e);
				const Kaim::Vec3f &c = o->GetTransform().m_translation;
				const Kaim::Vec3f &s = o->GetLocalHalfExtents();
				const Kaim::Vec3f &l = o->GetLocalCenter();

				r3dVector cp(c.x, c.z, c.y);
				r3dVector sp(s.x, s.z, s.y);
				r3dVector lp(l.x, l.z, l.y);

				cp += lp;
				cp -= sp;

				r3dBoundBox bb;
				bb.Org = cp;
				bb.Size = sp * 2;

				r3dDrawBoundBox(bb, gCam, r3dColor::red, 0.05f);
			}
			else if (t == Kaim::TypeCylinderObstacle)
			{
				Kaim::CylinderObstacle *o = static_cast<Kaim::CylinderObstacle*>(e);
				const Kaim::Vec3f &p = o->GetPosition();

				r3dVector pp(p.x, p.z, p.y);

				r3dCylinder c;
				c.fRadius = o->GetRadius();
				c.vAltitude.Assign(0, o->GetHeight(), 0);
				c.vBase = pp;

				r3dDrawCylinder(c, gCam, r3dColor::red);
			}
		}
	}
}
#endif

bool AutodeskNavMesh::IsNavPointValid(const r3dPoint3D &inOut)
{
	r3dPoint3D pos = inOut;
	return AdjustNavPointHeight(pos, 1.0f);
}

//////////////////////////////////////////////////////////////////////////

bool AutodeskNavMesh::AdjustNavPointHeight(r3dPoint3D &inOut, float searchHeightRange)
{
	bool rv = false;
	if (!world)
		return rv;

	Kaim::TriangleFromPosQuery q;
	q.BindToDatabase(world->GetDatabase(0));
	q.Initialize(R3D_KY(inOut));
	q.SetPositionSpatializationRange(Kaim::PositionSpatializationRange(searchHeightRange, searchHeightRange));
	q.PerformQuery();

	if (q.GetResult() == Kaim::TRIANGLEFROMPOS_DONE_TRIANGLE_FOUND)
	{
		inOut.y = q.GetAltitudeOfProjectionInTriangle();
		rv = true;
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////

AutodeskNavAgent** AutodeskNavMesh::GetNavAgentsInAABB(const r3dPoint3D& boxCenter, const r3dPoint3D& boxHalfExtents, std::tr1::unordered_set<uint32_t>& PoiTypesSet, uint32_t& foundNumNavAgents, bool skipListCreation /*= false*/)
{
	// Keep the extents rather small, to keep searching fast.
	// NOTE: Added .1f to the upper limits to handle floating point
	//		 rounding error.  Make sure any values given to the
	//		 function Max out at 20.0f and Min out at 0.01f.
	//r3d_assert(20.1f >= boxHalfExtents.x && 0.0f < boxHalfExtents.x &&
	//		   10.1f >= boxHalfExtents.y && 0.0f < boxHalfExtents.y &&
	//		   20.1f >= boxHalfExtents.z && 0.0f < boxHalfExtents.z);
	if(	20.1f < boxHalfExtents.x || 0.0f > boxHalfExtents.x ||
		10.1f < boxHalfExtents.y || 0.0f > boxHalfExtents.y ||
		20.1f < boxHalfExtents.z || 0.0f > boxHalfExtents.z)
	{
		r3dOutToLog("!!! ERROR %s(%d): Extents are outside the limits intended to keep the search area small.\n\tPosition<%.2f, %.2f, %.2f>\n\tExtents<%.2f, %.2f, %.2f>\n\tLimits<0.0, 0.0, 0.0> - <20.1, 10.1, 20.1>\n", __FILE__, __LINE__, boxCenter.x, boxCenter.y, boxCenter.z, boxHalfExtents.x, boxHalfExtents.y, boxHalfExtents.z );
	}


	// Set up a filter
	Kaim::SpatializedPointCollectorFilter_SelectiveCollect selectiveCollect;
	selectiveCollect.Select(Kaim::SpatializedPointObjectType_PointOfInterest);

	// Instantiate the query
	Kaim::SpatializedPointCollectorInAABBQuery<Kaim::SpatializedPointCollectorFilter_SelectiveCollect> query(selectiveCollect);
	query.BindToDatabase(world->GetDatabase(0));

	// Set up the query parameters
	const Kaim::Vec3f collectorBoxCenter(boxCenter.x, boxCenter.z, boxCenter.y);
	const Kaim::Vec3f collectorBoxHalfExtentVec3f(boxHalfExtents.x, boxHalfExtents.z, boxHalfExtents.y);
	const Kaim::Box3f collectorBoxExtent(collectorBoxHalfExtentVec3f, collectorBoxHalfExtentVec3f);
	query.Initialize(collectorBoxCenter, collectorBoxExtent);

	// Perform the query
	query.PerformQueryBlocking();

	// Collect the results
	if( query.GetResult() == Kaim::SPATIALIZEDPOINTCOLLECTOR_DONE )
	{
		Kaim::QueryDynamicOutput* queryDynamicOutput = query.GetQueryDynamicOutput();
		if( queryDynamicOutput )
		{
			// Determine how many AutodeskNavAgents were found.
			for( uint32_t i = 0; i < queryDynamicOutput->GetSpatializedPointCount(); ++i )
			{
				Kaim::SpatializedPoint* spatializedPoint = queryDynamicOutput->GetSpatializedPoint( i );
				if( spatializedPoint && 
					spatializedPoint->GetObjectType() == Kaim::SpatializedPointObjectType_PointOfInterest )
				{
					Kaim::PointOfInterest* pointOfInterest = (Kaim::PointOfInterest*)spatializedPoint->GetObject();

					// Filter by PointOfInterest types defined in AutodeskNavAgent::EPoiType.
					if( PoiTypesSet.find( pointOfInterest->GetPointOfInterestType() ) != PoiTypesSet.end() )
						++foundNumNavAgents;
				}
			}

			if( skipListCreation )
				return NULL;

			// Allocate the memory for the list.
			int* pInts = new int[5];
			AutodeskNavAgent** pNavAgents = game_new AutodeskNavAgent*[foundNumNavAgents];

			// Collect the AutodeskNavAgents.
			for( uint32_t i = 0, j = 0; i < queryDynamicOutput->GetSpatializedPointCount(); ++i )
			{
				Kaim::SpatializedPoint* spatializedPoint = queryDynamicOutput->GetSpatializedPoint( i );
				if( spatializedPoint && 
					spatializedPoint->GetObjectType() == Kaim::SpatializedPointObjectType_PointOfInterest )
				{
					Kaim::PointOfInterest* pointOfInterest = (Kaim::PointOfInterest*)spatializedPoint->GetObject();

					// Filter by PointOfInterest types defined in AutodeskNavAgent::EPoiType.
					if( PoiTypesSet.find( pointOfInterest->GetPointOfInterestType() ) != PoiTypesSet.end() )
					{
						r3d_assert( j < foundNumNavAgents );
						pNavAgents[ j++ ] = (AutodeskNavAgent*)pointOfInterest->GetUserData();
					}
				}
			}

			return pNavAgents;
		}
	}


	return NULL;
}

//////////////////////////////////////////////////////////////////////////

bool AutodeskNavMesh::GetClosestNavMeshPoint(r3dPoint3D &inOut, float searchHeightRange, float searchWidthRadius)
{
	bool rv = false;
	if (!world)
		return rv;

	Kaim::InsidePosFromOutsidePosQuery<Kaim::DefaultTraverseLogic> q;
	q.BindToDatabase(world->GetDatabase(0));
	q.Initialize(R3D_KY(inOut));
	q.SetPositionSpatializationRange(Kaim::PositionSpatializationRange(searchHeightRange, searchHeightRange));
	q.SetHorizontalTolerance(searchWidthRadius);
	q.PerformQuery();

	if (q.GetResult() == Kaim::INSIDEPOSFROMOUTSIDE_DONE_POS_FOUND)
	{
		inOut = KY_R3D(q.GetInsidePos());
		rv = true;
	}

	return rv;
}

//////////////////////////////////////////////////////////////////////////

AutodeskNavAgent * AutodeskNavMesh::CreateNavAgent(const r3dVector &pos)
{
	r3dPoint3D pos2 = pos;
	AdjustNavPointHeight(pos2, 20.0f);

	AutodeskNavAgent *a = game_new AutodeskNavAgent;
	a->Init(world, pos2);
	AddNavAgent(a);
	return a;
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::AddNavAgent(AutodeskNavAgent *a)
{
	agents.PushBack(a);
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMesh::DeleteNavAgent(AutodeskNavAgent *a)
{
	for (uint32_t i = 0; i < agents.Count(); ++i)
	{
		if (agents[i] == a)
		{
			delete a;
			agents.Erase(i);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
void AutodeskNavMesh::ExportToObj()
{
	ClearNavDataDir();
	SaveBuildConfig();
	PopulateLevelMesh(Nav::gLevelMesh);
	ExportGeometryToObjFormat(*Nav::gLevelMesh, Nav::GetLevelObjFilePath());
	
	// and run NavBuilder
	char curDir[MAX_PATH];
	::GetCurrentDirectory(sizeof(curDir), curDir);
	
	char prg[MAX_PATH];
	char param[MAX_PATH];
	sprintf(prg, "%s\\NavGenLauncher\\NavGenLauncher.exe", curDir);
	sprintf(param, "%s\\%s\\Level.NavGenProj", curDir, Nav::GetNavDataFolderPath().c_str());
	
	ShellExecute(NULL, "open", prg, param, NULL, SW_SHOW);
}
#endif

//////////////////////////////////////////////////////////////////////////

#endif // ENABLE_AUTODESK_NAVIGATION