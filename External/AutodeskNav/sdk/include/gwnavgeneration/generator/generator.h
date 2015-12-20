/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_Generator_H
#define GwNavGen_Generator_H

#include "gwnavruntime/base/fileopener.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

#include "gwnavgeneration/common/generatormemory.h"
#include "gwnavgeneration/common/generatormemoryheap.h"
#include "gwnavgeneration/generator/generatorprofiling.h"
#include "gwnavgeneration/input/inputtilecontributionatpos.h"
#include "gwnavgeneration/input/inputcellinfogrid.h"
#include "gwnavgeneration/generator/igeneratorglue.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"
#include "gwnavgeneration/generator/generatorsector.h"
#include "gwnavgeneration/generator/generatorsystem.h"
#include "gwnavgeneration/input/clientinputconsumer.h"
#include "gwnavgeneration/input/generatorinputproducer.h"
#include "gwnavgeneration/generator/generatorinputoutput.h"
#include "gwnavgeneration/generator/generatorreport.h"
#include "gwnavgeneration/generator/generatorsectorbuilder.h"


namespace Kaim
{

class IParallelForInterface;
class CollisionInterfaceFactory;
class NavMeshElementBlob;
class GeneratorSectorBuilder;
class GeneratorNavDataElement;


/// The Generator is the principal class in the NavData generation system.
/// \section generatoroutput Output files
/// The Generator is capable of generating multiple kinds of data files:
/// -  .ClientInput files, which are records of the input triangles, NavTags and TagVolumes that describe
///     your terrain, passed as inputs to the NavData generation system.
/// -  .GenIO files, which record the list of sectors and the generation parameters that where used when the Generator::Generate()
///     function was called. See Kaim::GeneratorInputOutput class
/// -  Intermediate data files, which record temporary data calculated during the generation process. Mostly for debug purposes.
/// -  Final .NavData files, which contain the final data that you will load into your game at runtime.
/// You can control which types of data files you want to generate each time you use the Generator.
/// \section generatorsectors Sectors and multi-sectors
/// The Generator can be set up with any number of sectors, each of which is responsible for a block of terrain geometry
/// within a defined area of your game world. Separate input, intermediate and output data are created for each sector,
/// so that the sectors can be streamed into memory independently at runtime. Adjacent sectors whose triangles lie close to
/// each other will be linked automatically at runtime.
/// You can also configure the Generator to re-partition each sector that you provide into smaller sectors automatically.
class Generator : public RefCountBaseNTS<Generator, MemStat_NavDataGen>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

friend class NavDataIndexBlobBuilder_FromGenerator;

public:
	/// Constructs a new Generator initialized with the provided information.
	/// \param producer                A pointer to an object that the Generator will use to retrieve the input data for each
	///                                  sector.This object is ref-counted : It will be destroyed automatically when appropriate.
	/// \param parallelForInterface    A pointer to an object that the Generator will use to parallelize computations over multiple
	///                                 processors, or KY_NULL in order to perform all computations sequentially on a single processor.
	///                                 This object is ref-counted; It will be destroyed automatically when appropriate.
	/// \param tlsAlloc                A pointer to an object that the Generator will use to allocate and free memory in secondary threads,
	///                                  when configured to use parallel generation.  This object is ref-counted; It will be destroyed
	///                                  automatically when appropriate.
	Generator(Ptr<GeneratorInputProducer> producer, 
			  Ptr<IParallelForInterface> parallelForInterface = KY_NULL, 
			  Ptr<ITlsAlloc> tlsAlloc = KY_NULL,
			  Ptr<ICollisionInterface> visibilityInterface = KY_NULL);

	/// Constructs a new Generator initialized with the provided GeneratorInputProducer and IGeneratorGlue.
	/// Requires an instance of a class that derives from IGeneratorGlue, which the Generator invokes to retrieve objects that implement
	/// IParallelForInterface and ITlsAlloc when needed.
	Generator(Ptr<GeneratorInputProducer> producer, Ptr<IGeneratorGlue> generatorGlue);

	~Generator();

public:
	// ------------------------------ Configuration ------------------------------
	
	/// Sets the Kaim::FileOpenerBase derived class that will be used to open any files. 
	/// This is not mandatory to call this function, an instance of \c DefaultFileOpener will be used automatically.
	void SetFileOpener(FileOpenerBase* fileOpener);

	/// Sets the directory used for the generation.
	/// It is passed as an absolute directory and a relative path.
	/// The relative path will be embedded in the generated data, 
	/// so when visual debugging all data can be found by providing absoluteBaseDirectory to the NavigationLab.
	/// \param absoluteOutputBaseDir An absolute path used as a base path for relativeOutputDir
	/// \param relativeOutputDir A relative path to absoluteOutputBaseDir. The absolute path to this directory is : absoluteOutputBaseDir/relativeOutputDir
	void SetOutputDirectory(const char* absoluteOutputBaseDir, const char* relativeOutputDir);

	/// returns absoluteOutputBaseDir/relativeOutputDir set by SetOutputDirectory()
	const String& GetAbsoluteOutputDirectory() const { return m_sys.GetAbsoluteOutputDirectory(); }

	// ---------------------------------- Generation ----------------------------------

	/// Launch the generation of the sectors stored in \c config, using the config parameters and runOptions
	/// also defined in this structure. See GeneratorInputOutput for more information
	/// \return A #KyResult that indicates the success or failure of the request.
	KyResult Generate(GeneratorInputOutput& config);

	// ---------------------------------- Reporting ----------------------------------

	/// Creates a detailed record of all configuration parameters and sector information, and writes the record to the specified stream.
	template <class OSTREAM> void LogConfiguration(OSTREAM& os);

	/// Retrieves a detailed record of the results of the last call to Generate(). 
	const GeneratorReport& GetGeneratorReport() const { return m_report; }

	/// Get some profiling information about the time taken by the various steps of the generation
	const GeneratorProfiling& GetGeneratorProfiling() const { return m_generatorProfiling; }
	
	// ---------------------------------- Internal Access Only ----------------------------------

	GeneratorSystem& Sys() { return m_sys; }
	const KyArray<Ptr<GeneratorSectorBuilder> >& GetSectorBuilders() const { return m_sectorBuilders; }

private:
	void ClearGeneratedData();
	void ClearSectors();

	KyResult AddSector(Ptr<GeneratorSector> sector);

	void Init(Ptr<IParallelForInterface> parallelFor, Ptr<ITlsAlloc> tlsAlloc, Ptr<ICollisionInterface> visibilityInterface);
	
	KyResult GenerateInit(GeneratorInputOutput& generatorInputOutput);
	bool CheckConfig() const;

	KyResult GenerateNavData();
	KyResult ProduceSectorInputs();
	void MakeGlobalGrid();
	void MakeNavDataElements();
	KyResult GenerateRawNavCells();
	KyResult PerformSeedOrSurfaceFiltering();
	KyResult PerformIntegrationOfTagvolumes();
	KyResult MakeOutputBlobAggregates();
	void ComputeStatistics();
	KyResult BuildOutputDataIndex();
	KyResult UpdateAndSaveGeneratorInputOutput();

	void AddInputCellInfosContributingToSingleGuid(
		KyUInt32 mainIdx, const InputCellInfoArrayAtPos* inputCellInfoArrayAtPos, InputCellInfoArrayAtPos* contributingInputCellInfoArrayAtPos);

	bool IsGuidCompoundExclusive(const GeneratorGuidCompound& guidCompound) const;
	void UpdateGeneratorInputOutput();
	KyResult SaveGeneratorInputOutput();
	KyResult SaveDatabaseDescriptorIndex();

	KyResult ProduceSectorInputs_FromChangedPatchs();
	KyResult ProduceSectorInputs_FromSectorsOverlappingWithChangedPatches();
	KyResult ProduceSectorInputs_FromChangedSectors();
	KyResult ProduceSectorInputs_FromSectorsOverlappingWithChangedRegularSectors();

	void MakeNavDataElementsAtCellPos(const InputCellInfoArrayAtPos* cellsAtPos);
	void RemoveInputTilesTempDir();

	KyResult MakeAbstractGraphs();

public: //internal
	void RemoveInputTilesPartTempDir();
	
private:
	Ptr<GeneratorInputProducer> m_producer;
	Ptr<IGeneratorGlue> m_generatorGlue; // Ptr stored so that the passed glue is not deleted while being used
	GeneratorSystem m_sys;
	DefaultFileOpener m_defaultFileOpener;
	KyArray<Ptr<GeneratorSectorBuilder> > m_sectorBuilders;
	GeneratorSectorList m_sectorList;
	InputCellInfoGrid m_inputCellGrid;
	KyArray<GeneratorGuidCompound> m_exclusiveGuids;
	GeneratorReport m_report;
	GeneratorProfiling m_generatorProfiling;
	KyArray<CellPos> m_patchedCellPositions;
};


template <class OSTREAM>
inline void Generator::LogConfiguration(OSTREAM& os)
{
	os << KY_LOG_BIG_TITLE_BEGIN("", "Generator Configuration Info");

	os << m_sys;
	os << m_sys.Config();
	os << m_sys.RunOptions();

	for (KyUInt32 sectorIdx = 0; sectorIdx < m_sectorBuilders.GetCount(); ++sectorIdx)
	{
		os << KY_LOG_SMALL_TITLE_BEGIN("", "Sector " << sectorIdx <<" Info");
		m_sectorBuilders[sectorIdx]->LogConfiguration(os);
		os << KY_LOG_SMALL_TITLE_END("", "Sector " << sectorIdx <<" Info");
	}

	os << KY_LOG_BIG_TITLE_END("", "Generator Configuration Info");
}


}


#endif
