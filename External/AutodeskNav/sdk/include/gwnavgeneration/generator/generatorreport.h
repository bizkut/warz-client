/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorReport_H
#define GwNavGen_GeneratorReport_H

#include "gwnavruntime/containers/stringstream.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/basesystem/logger.h"
#include "gwnavgeneration/generator/generatorsectorreport.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphdatageneratorreport.h"
#include "gwnavruntime/basesystem/floatformatter.h"

namespace Kaim
{

inline String GeneratorReportGetNoneIfEmpty(const String& str) { return str  == "" ? "--NONE--" : str; }

template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const GeneratorSectorReport& sectorReport)
{
	FloatFormatter fmt2f("%.2f");
	const char* prefix = sectorReport.m_logPrefix.ToCStr();

	const char* isPatchString = sectorReport.m_isPatch ? "PATCH" : "REGULAR";
	os << KY_LOG_SMALL_TITLE_BEGIN(prefix, isPatchString << " sector " << sectorReport.m_sectorName << " " << sectorReport.m_guid);

	if (sectorReport.m_clientInputLoaded)
	{
		os << prefix << "INPUT: LOADED FROM CLIENTINPUT" << Endl;
		os << prefix << prefix << "Loaded: " << sectorReport.m_loadedClientInputAbsoluteFileName << Endl;
		os << prefix << prefix << "Input Consume time : " << fmt2f(sectorReport.m_sectorInputConsumptionDuration) << " seconds" << Endl;
		os << prefix << prefix << "InputTriangle Count: " << sectorReport.m_consumedTriangleCount << " triangles" << Endl;
	}

	if (sectorReport.m_sectorInputProduced)
	{
		os << prefix << "INPUT: GENERATED FROM INPUTPRODUCER"<< Endl;
		os << prefix << prefix << "Input Consume time:  " << fmt2f(sectorReport.m_sectorInputConsumptionDuration) << " seconds" << Endl;
		os << prefix << prefix << "InputTriangle Count: " << sectorReport.m_consumedTriangleCount << Endl;
		os << prefix << prefix << "Saved: " << GeneratorReportGetNoneIfEmpty(sectorReport.m_savedClientInputAbsoluteFileName) << Endl;
	}
	

	if (sectorReport.m_navDataGenerated)
	{
		os << prefix << "GENERATED NAVDATA: REGULAR" << Endl;
		os << prefix << prefix << "Generation Time: " << fmt2f(sectorReport.m_sectorRawNavMeshTime) << " seconds" << Endl;
		os << prefix << prefix << "NavData Size   : " << fmt2f(sectorReport.m_navDataSizeInBytes / 1024.0f) << " kilobytes" << Endl;
		os << prefix << prefix << "NavData File: " << GeneratorReportGetNoneIfEmpty(sectorReport.m_savedNavDataAbsoluteFileName) << Endl;
	}

	if (sectorReport.m_navDataPatchGenerated)
	{
		os << prefix << "GENERATED NAVDATA: PATCH" << Endl;
		os << prefix << prefix << "Generation Time  : " << fmt2f(sectorReport.m_sectorRawNavMeshTime) << " seconds" << Endl;
		os << prefix << prefix << "NavDataPatch Size: " << fmt2f(sectorReport.m_navDataPatchSizeInBytes / 1024.0f) << " kilobytes" << Endl;
		os << prefix << prefix << "NavDataPatch File: " << GeneratorReportGetNoneIfEmpty(sectorReport.m_savedNavDataPatchAbsoluteFileName) << Endl;

	}

	if (sectorReport.SomethingHappenedForThisSector() == false)
	{
		os << prefix << "Nothing loaded or generated" << Endl;
	}

	// ---------------------------------------- Details ----------------------------------------
	if (sectorReport.m_doLogDetails)
	{
		const Box3f& box = sectorReport.m_inputBoundingBox;

		KyFloat32 extentX = box.m_max.x - box.m_min.x;
		KyFloat32 extentY = box.m_max.y - box.m_min.y;
		if (extentX > 0.0f && extentY > 0.0f)
		{
			os << prefix << "Input geometry BBOX Min: ("   << fmt2f(box.m_min.x) << "," << fmt2f(box.m_min.y) << "," << fmt2f(box.m_min.z) << ")" << Endl;
			os << prefix << "Input geometry BBOX Max: ("   << fmt2f(box.m_max.x) << "," << fmt2f(box.m_max.y) << "," << fmt2f(box.m_max.z) << ")" << Endl;
			os << prefix << "Input geometry BBOX Extent: " << fmt2f(extentX * 0.0001f) << " Km by " << fmt2f(extentY * 0.0001f) << " Km" << Endl;
		}

		os << prefix << "Nb Input cells:       " << sectorReport.m_inputCellsCount                     << Endl; 
		os << prefix << "Nb NavData cells:     " << sectorReport.m_generatedTotalNavCellsCount         << Endl;
		os << prefix << "Nb Main cells:        " << sectorReport.m_generatedMainNavCellsCount          << Endl;
		os << prefix << "Nb Overlap cells:     " << sectorReport.m_generatedOverlapNavCellsCount       << Endl;
		os << prefix << "Nb NavData Element:   " << sectorReport.m_navdataElementsCount                << Endl;
		os << prefix << "NavMesh size:         " << sectorReport.m_totalNavmeshSizeInBytes << " bytes" << Endl;
		os << prefix << "Overlap size:         " << sectorReport.m_overlapDataSizeInBytes  << " bytes" << Endl;
	}

	os << KY_LOG_SMALL_TITLE_END(prefix, "sector " << sectorReport.m_sectorName);
	
	return os;
}


/// The GeneratorReport class encapsulates a detailed report of all activities carried out during a
/// call to the Generator::Generate(GeneratorInputOutput&) method. This report may be useful to you for evaluating the data
/// generated in your level design tool; it may also be useful to help Autodesk middleware Support
/// diagnose problems that occur in your NavData generation.
/// You can retrieve an instance of this class by calling Generator::GetGeneratorReport() at any time
/// after a successful NavData generation. You can interact with the retrieved instance programmatically
/// in your application, or write the report to a text string by calling ToString().
class GeneratorReport
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	/// For internal use: constructs a new GeneratorReport.
	GeneratorReport() :
		m_doLogTitle(true),
		m_doLogDetails(true),
		m_doLogPerSectorReport(true),
		m_totalGenerationSeconds(0.0f),
		m_inputConsumptionSeconds(0.0f),
		m_loadInputTilesSeconds(0.0f),
		m_rawNavMeshGenerationSeconds(0.0f),
		m_tagVolumeIntegrationSeconds(0.0f),
		m_navMeshFilteringSeconds(0.0f),
		m_aggregateNavDataSeconds(0.0f),
		m_regularSectorCount(0),
		m_patchSectorCount(0),
		m_sectorInputProduceCount(0),
		m_clientInputLoadedCount(0),
		m_clientInputSavedCount(0),
		m_navDataGeneratedCount(0),
		m_navDataPatchGeneratedCount(0)
	{
		m_logPrefix = "  ";
	}

	void AddSectorStats(const GeneratorSectorReport& sectorStats)
	{
		m_sectorStatistics.PushBack(sectorStats);

		if (sectorStats.m_isPatch)
			++m_patchSectorCount;
		else
			++m_regularSectorCount;

		if (sectorStats.m_sectorInputProduced)
			++m_sectorInputProduceCount;

		if (sectorStats.m_clientInputLoaded)
			++m_clientInputLoadedCount;

		if (sectorStats.m_savedClientInputAbsoluteFileName != "")
			++m_clientInputSavedCount;

		if (sectorStats.m_navDataGenerated)
			++m_navDataGeneratedCount;

		if (sectorStats.m_navDataPatchGenerated)
			++m_navDataPatchGeneratedCount;

		// m_rawNavMeshGenerationSeconds was timed globally, we have to subtract the time taken in each sector LoadInputTile
		m_loadInputTilesSeconds       += sectorStats.m_sectorLoadInputTileTime;
		m_rawNavMeshGenerationSeconds -= sectorStats.m_sectorLoadInputTileTime;
	}

public:
	bool m_doLogTitle;
	bool m_doLogDetails;
	bool m_doLogPerSectorReport;
	String m_logPrefix;

	KyFloat32 m_totalGenerationSeconds;      ///< The total time taken by the NavData generation process, in seconds. 
	KyFloat32 m_inputConsumptionSeconds;     ///< The time taken to consume the input geometry, in seconds. 
	KyFloat32 m_loadInputTilesSeconds;       ///< The time taken to load of InputTile files, in seconds. Relevant when runOptions.m_doEnableLimitedMemoryMode is true.
	KyFloat32 m_rawNavMeshGenerationSeconds; ///< The time taken to generate the RawNavMesh, in seconds. Does not include m_loadInputTilesSeconds.
	KyFloat32 m_tagVolumeIntegrationSeconds; ///< The time taken to integrate the generation TagVolumes, in seconds.
	KyFloat32 m_navMeshFilteringSeconds;     ///< The time taken to remove not reachable parts of the NavMesh, in seconds. 
	KyFloat32 m_aggregateNavDataSeconds;     ///< The time taken to aggregate the generated NavCells into NavData buffer (and optionnaly .NavData files)

	KyUInt32 m_regularSectorCount;           ///< The number of sector.
	KyUInt32 m_patchSectorCount;             ///< The number of patch sector.
	KyUInt32 m_sectorInputProduceCount;      ///< The number of sectors for which ProduceInputs() was called.
	KyUInt32 m_clientInputLoadedCount;       ///< The number of sectors for which LoadClientInput() was called.
	KyUInt32 m_clientInputSavedCount;        ///< The number of sectors for which SaveClientInput() was called.
	KyUInt32 m_navDataGeneratedCount;        ///< The number of sectors for generated Regular NavData.
	KyUInt32 m_navDataPatchGeneratedCount;   ///< The number of generated Patch NavData.

	KyArray<GeneratorSectorReport> m_sectorStatistics; ///< Maintains an array of objects that contain detailed statistics about the sectors for which NavData was generated. 
	AbstractDataGenerationReport m_abstractDataReport;

	String m_absoluteGeneratorInputOuputSaveFileName;
};


template <class OSTREAM>
inline void LogGenerationTime(OSTREAM& os, const char* prefix, const char* title, KyFloat32 seconds, KyFloat32 minSecondsToLog)
{
	FloatFormatter fmt2f("%.2f");
	if (seconds >= minSecondsToLog)
		os << prefix << title << fmt2f(seconds) << " seconds" << Endl;
}


template <class OSTREAM>
inline void LogGenerationCount(OSTREAM& os, const char* prefix, const char* title, KyUInt32 count, KyFloat32 minCountToLog)
{
	if (count >= minCountToLog)
		os << prefix << title << count << Endl;
}

template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const GeneratorReport& report)
{
	FloatFormatter fmt2f("%.2f");
	const char* prefix = report.m_logPrefix.ToCStr();

	if (report.m_doLogTitle)
		os << KY_LOG_BIG_TITLE_BEGIN(prefix, "GENERATION REPORT");

	os << prefix << "Saved GenIO File: " << GeneratorReportGetNoneIfEmpty(report.m_absoluteGeneratorInputOuputSaveFileName) << Endl;

	LogGenerationTime(os, prefix, "Input Consumption time:        ", report.m_inputConsumptionSeconds    , 0.05f);
	LogGenerationTime(os, prefix, "Load InputTiles time:          ", report.m_loadInputTilesSeconds      , 0.05f);
	LogGenerationTime(os, prefix, "Raw NavMesh Generation time:   ", report.m_rawNavMeshGenerationSeconds, 0.05f);
	LogGenerationTime(os, prefix, "TagVolume Integration time:    ", report.m_tagVolumeIntegrationSeconds, 0.05f);
	LogGenerationTime(os, prefix, "NavMesh Filtering time:        ", report.m_navMeshFilteringSeconds    , 0.05f);
	LogGenerationTime(os, prefix, "Aggregate NavData time:        ", report.m_aggregateNavDataSeconds    , 0.05f);
	LogGenerationTime(os, prefix, "Total Generation time:         ", report.m_totalGenerationSeconds     , 0.0f);

	LogGenerationCount(os, prefix, "REGULAR Sector count:          ", report.m_regularSectorCount         , 1);
	LogGenerationCount(os, prefix, "PATCH Sector count:            ", report.m_patchSectorCount           , 1);
	LogGenerationCount(os, prefix, "ClientInput Loaded count:      ", report.m_clientInputLoadedCount     , 1);
	LogGenerationCount(os, prefix, "Sector InputProduced count:    ", report.m_sectorInputProduceCount    , 1);
	LogGenerationCount(os, prefix, "ClientInput Saved count:       ", report.m_clientInputSavedCount      , 1);
	LogGenerationCount(os, prefix, "Generated NavData Patch count: ", report.m_navDataPatchGeneratedCount , 1);
	LogGenerationCount(os, prefix, "Generated NavData count:       ", report.m_navDataGeneratedCount      , 1);
	os << Endl;

	if (report.m_doLogPerSectorReport)
	{
		// report sectors for which *something* happened (and count sectorForWhichSomethingHappened)
		KyUInt32 sectorsForWhichSomethingHappened = 0;
		for (KyUInt32 i = 0; i < report.m_sectorStatistics.GetCount(); ++i)
		{
			const GeneratorSectorReport& stats = report.m_sectorStatistics[i];
			if (stats.SomethingHappenedForThisSector() == true)
			{
				stats.m_doLogDetails = report.m_doLogDetails;
				os << stats;
			}
			else
			{
				++sectorsForWhichSomethingHappened ;
			}
		}

		// report sectors for which *nothing* happened
		if (sectorsForWhichSomethingHappened > 0)
		{
			os << KY_LOG_SMALL_TITLE_BEGIN(prefix, sectorsForWhichSomethingHappened << " Sectors for which nothing happened");
			for (KyUInt32 i = 0; i < report.m_sectorStatistics.GetCount(); ++i)
			{
				const GeneratorSectorReport& stats = report.m_sectorStatistics[i];
				if (stats.SomethingHappenedForThisSector() == false)
				{
					const char* isPatchString = stats.m_isPatch ? "PATCH  " : "REGULAR";
					os << prefix << isPatchString << " sector " << stats.m_guid << " " << stats.m_sectorName << Endl;
				}
			}
			os << KY_LOG_SMALL_TITLE_END(prefix, "Sectors for which nothing happened");
		}
	}

	if (report.m_doLogTitle)
		os << KY_LOG_BIG_TITLE_END(prefix, "GENERATION REPORT");


	// log AbstractData generation if any
	os << report.m_abstractDataReport;

	return os;
}

}


#endif

