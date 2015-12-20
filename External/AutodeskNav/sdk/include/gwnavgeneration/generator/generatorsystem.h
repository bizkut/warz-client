/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_GeneratorSystem_H
#define GwNavGen_GeneratorSystem_H


#include "gwnavruntime/basesystem/basesystem.h"
#include "gwnavruntime/basesystem/coordsystem.h"
#include "gwnavruntime/basesystem/intcoordsystem.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/kernel/SF_File.h"
#include "gwnavruntime/navmesh/pixelandcellgrid.h"
#include "gwnavgeneration/common/generatorfile.h"
#include "gwnavgeneration/common/generatorfilesystem.h"
#include "gwnavgeneration/common/generatormemory.h"
#include "gwnavgeneration/generator/iparallelforinterface.h"
#include "gwnavgeneration/generator/navdataelementglobalmap.h"
#include "gwnavruntime/collision/icollisioninterface.h"
#include "gwnavgeneration/generator/genflags.h"
#include "gwnavgeneration/common/densegrid.h"
#include "gwnavgeneration/generator/generatorinputoutput.h"
#include "gwnavruntime/database/databasegenmetrics.h"
#include "gwnavgeneration/generator/generatorintegercoords.h"


namespace Kaim
{

class GeneratorInputOutput;
class BlobCategory;
class BlobAggregate;
class GeneratorProfiling;
class BaseVisualDebugServer;
class BaseVisualDebugManager;
class VisualDebugServer;
class FileOpenerBase;
class GeneratorReport;

/// GeneratorSystem gathers everything that is global across a generation: configuration, options...
class GeneratorSystem
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	GeneratorSystem();
	~GeneratorSystem();

	void Init(GeneratorInputOutput* genIO);

	const GeneratorParameters& Config() const { return m_genIO->m_params; }
	GeneratorParameters& Config()             { return m_genIO->m_params; }

	const GeneratorAdvancedParameters& AdvancedParams() const { return m_genIO->m_params.m_advancedParameters; }
	GeneratorAdvancedParameters& AdvancedParams()             { return m_genIO->m_params.m_advancedParameters; }

	const GeneratorAbstractGraphParameters& AbstractGraphParams() const { return m_genIO->m_abstractGraphParams; }
	GeneratorAbstractGraphParameters& AbstractGraphParams()             { return m_genIO->m_abstractGraphParams; }

	const GeneratorRunOptions& RunOptions() const { return m_genIO->m_runOptions; }
	GeneratorRunOptions& RunOptions()             { return m_genIO->m_runOptions; }

	const GeneratorFileSystem& GetFileSystem() const { return m_fileSystem; }
	GeneratorFileSystem& GetFileSystem()             { return m_fileSystem; }

	const String& GetAbsoluteOutputDirectory() const { return GetFileSystem().GetAbsoluteOutputDirectory(); }
	String GetAbsoluteFileName(const String& relativeFileName) const { return GetFileSystem().GetAbsoluteFileName(relativeFileName); }

	IParallelForInterface* GetParallelForIfUseMulticore();

	const PixelAndCellGrid& GetPixelAndCellGrid() const { return m_pixelAndCellGrid; }
	PixelAndCellGrid& GetPixelAndCellGrid()             { return m_pixelAndCellGrid; }

	KyFloat32 GetNormalizedRasterPrecision() const { return m_genIO->m_params.m_normalizedParameters.m_normalizedRasterPrecision; }
	KyFloat32 GetNormalizedCellSize()        const { return m_genIO->m_params.m_normalizedParameters.m_normalizedCellSize; }

	const IntermediateFilesConfig& GetIntermediates() const { return m_genIO->m_runOptions.m_intermediateFiles; }

	// Load Blob from fileName
	void* LoadBlobBase(const char* fileName, BaseBlobHandler& blobHandler);

	template<class T>
	T* LoadBlob(const char* fileName, BlobHandler<T>& blobHandler) { return (T*)LoadBlobBase(fileName, blobHandler); }

	// Save Blob to fileName
	KyResult SaveBlob(const char* fileName, const BaseBlobHandler& blobHandler, Endianness::Type endianness = Endianness::BigEndian);

	// Load BlobAggregate from fileName
	KyResult LoadAggregate(const char* fileName, BlobAggregate& aggregate);

	/* Save Blob to fileName */
	KyResult SaveAggregate(const char* fileName, BlobAggregate& aggregate, Endianness::Type endianness = Endianness::BigEndian);

	void StartVisualDebug();
	void StopVisualDebug();
	bool IsVisualDebugConnected();

	/// Tiling enabled i.e. IsLowMemoryModeEnabled() or possibly a future fast-local-input-produce-mode
	bool IsInputTilingEnabled() const           { return RunOptions().m_doEnableLimitedMemoryMode; }
	KyUInt32 GetInputTileSizeInNbCells() const  { return RunOptions().m_limitedMemoryModeTileSizeInNbCells; }

private:
	void InitFromParams(GeneratorParameters& params);
	KyResult ComputeGenerationMode();

public:
	GeneratorInputOutput* m_genIO;
	DatabaseGenMetrics m_genMetrics;
	PixelAndCellGrid m_pixelAndCellGrid;
	GeneratorIntegerCoords m_integerCoords;
	KyUInt32 m_timeStamp;
	String m_generatorInputOuputSaveFileName;
	GeneratorFileSystem m_fileSystem;
	DenseGrid<KyUInt8> m_isMultipleInputAtCellPosGrid;
	GeneratorNavDataElementMap m_navDataElementGlobalMap;
	BlobCategory* m_blobRegistry;
	Ptr<IParallelForInterface> m_parallelForInterface;
	Ptr<ITlsAlloc> m_tlsAlloc;
	Ptr<ICollisionInterface> m_collisionInterface;
	GeneratorProfiling* m_generatorProfiling;
	FileOpenerBase* m_fileOpener;
	VisualDebugServer* m_visualDebugServer;
	String m_inputTilesTempDir; // local to Generator::GetAbsoluteOutputDirectory()
	String m_inputTilePartsTempDir; // local to Generator::GetAbsoluteOutputDirectory()
	GenFlags::GenerationMode m_generationMode;
	GeneratorReport* m_generatorReport;
};


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, GeneratorSystem& sys)
{
	os << KY_LOG_SMALL_TITLE_BEGIN("", "GeneratorSystem Info");
	os << "ParallelForInterface status:       "  << (sys.m_parallelForInterface       != KY_NULL ? "SET" : "UNSET") << Endl;
	os << "VisualDebugServer status:           " << (sys.IsVisualDebugConnected()  ? "CONNECTED" : "DECONNECTED")   << Endl;
	os << "GamewareKey : "                       << (Kaim::GamewareKey::GetInstance().GetKey())                     << Endl;
	os << KY_LOG_SMALL_TITLE_END("", "GeneratorSystem Info");
	return os;
}

}


#endif

