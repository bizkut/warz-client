/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorSectorReport_H
#define GwNavGen_GeneratorSectorReport_H

#include "gwnavruntime/kernel/SF_String.h"

namespace Kaim
{


/// The SectorStatistics class encapsulates a detailed set of statistics about a single sector treated by
/// the NavData generation framework. Instances of this class can be retrieved from a GeneratorReport 
/// through its GeneratorReport::m_sectorStatistics array.
class GeneratorSectorReport
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	/// For internal use: constructs a new SectorStatistic.
	GeneratorSectorReport():
		m_doLogDetails(false),
		m_isPatch(false),
		m_clientInputLoaded(false),
		m_sectorInputProduced(false),
		m_navDataGenerated(false),
		m_navDataPatchGenerated(false),
		m_colDataGenerated(false),
		m_inputCellsCount(0),
		m_consumedTriangleCount(0),
		m_generatedTotalNavCellsCount(0),
		m_generatedMainNavCellsCount(0),
		m_generatedOverlapNavCellsCount(0),
		m_navdataElementsCount(0),
		m_sectorInputConsumptionDuration(0.0f),
		m_sectorRawNavMeshTime(0.0f),
		m_sectorLoadInputTileTime(0.0f),
		m_navDataSizeInBytes(0),
		m_navDataPatchSizeInBytes(0),
		m_coldataSizeInBytes(0),
		m_totalNavmeshSizeInBytes(0),
		m_overlapDataSizeInBytes(0)
	{
		m_logPrefix = "    ";
	}

	bool SomethingHappenedForThisSector() const
	{
		return m_clientInputLoaded     == true
			|| m_sectorInputProduced   == true
			|| m_navDataGenerated      == true
			|| m_navDataPatchGenerated == true;
	}

public:
	mutable bool m_doLogDetails;
	String m_logPrefix;

	String m_sectorName;
	KyGuid m_guid;
	bool m_isPatch;

	bool m_clientInputLoaded;     ///< Indicates if the client input loaded for this sector
	bool m_sectorInputProduced;   ///< Indicates if the InputProducer was called for this sector
	bool m_navDataGenerated;      ///< Indicates if the NavData was generated for this sector
	bool m_navDataPatchGenerated; ///< Indicates if the Patch.X.NavData was generated for this sector
	bool m_colDataGenerated;      ///< Indicates if the ColData was generated for this sector

	String m_loadedClientInputAbsoluteFileName; // != "" if .ClientInput was loaded from the hard drive
	String m_savedClientInputAbsoluteFileName;  // != "" if .ClientInput was saved to the hard drive
	String m_savedNavDataAbsoluteFileName;      // != "" if .NavData was saved to the hard drive
	String m_savedNavDataPatchAbsoluteFileName; // != "" if .Patch.X.NavData was saved to the hard drive
	String m_savedColDataAbsoluteFileName;      // != "" if .ColData was saved to the hard drive

	KyUInt32 m_inputCellsCount; ///< Maintains the number of input cells created internally for the input geometry of the sector.
	KyUInt32 m_consumedTriangleCount; ///< Maintains the number of input triangles consumed in the input geometry of the sector.
	Box3f m_inputBoundingBox; ///< Maintains the bounding box around the input geometry of the sector. 

	KyUInt32 m_generatedTotalNavCellsCount; ///< Maintains the total number of cells in the NavMesh generated for the sector. 
	KyUInt32 m_generatedMainNavCellsCount; ///< Maintains the total number of unique cells in the NavMesh generated for the sector: i.e. cells that do not overlap the NavData of other sectors. 
	KyUInt32 m_generatedOverlapNavCellsCount; ///< Maintains the total number of overlap cells in the NavMesh generated for the sector: i.e. cells that overlap the NavData of one or more other sectors. 
	KyUInt32 m_navdataElementsCount; ///< Maintains the total number of NavData elements in the sector: i.e. the number of data chunks that are identified by different GUID compounds. 

	KyFloat32 m_sectorInputConsumptionDuration; ///< Maintains the time taken to consume the input geometry for the sector. 
	KyFloat32 m_sectorRawNavMeshTime; ///< Maintains the time taken to generate the NavData for the sector. This includes m_sectorLoadInputTileTime.
	KyFloat32 m_sectorLoadInputTileTime; ///< Maintains the time taken to load the InputTile for the sector. 

	KyUInt32 m_navDataSizeInBytes; ///< Maintains the total size of the NavData generated for the sector.
	KyUInt32 m_navDataPatchSizeInBytes; ///< Maintains the total size of the NavData generated for the sector. 
	KyUInt32 m_coldataSizeInBytes;

	KyUInt32 m_totalNavmeshSizeInBytes; ///< Maintains the size of the NavMeshElement generated for the sector. 
	KyUInt32 m_overlapDataSizeInBytes; ///< Maintains the size of the overlap data generated for the sector: the NavData that overlaps the NavData of one or more other sectors. 
};

}


#endif

