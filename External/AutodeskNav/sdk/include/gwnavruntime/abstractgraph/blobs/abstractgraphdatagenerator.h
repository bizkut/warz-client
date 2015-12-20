/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphDataGenerator_H
#define Navigation_AbstractGraphDataGenerator_H

#include "gwnavruntime/database/database.h"
#include "gwnavruntime/navdata/navdata.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphgenerator.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphblob.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphdatageneratorreport.h"
#include "gwnavruntime/queries/utils/basemultidestinationpathfinderquery.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/kernel/SF_RefCount.h"



namespace Kaim
{

class FileOpenerBase;

/// Usage:
/// AbstractGraphDataGenerator gen;
/// gen.InitFromXxxx()
/// gen.Generate()
/// gen.SaveAbstractGraphData()
///
class AbstractGraphDataGenerator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	static const char* GetFileExtension() { return "AbstractData"; }
public:
	AbstractGraphDataGenerator()
		: m_boxExtentInCellSize(10)
		, m_navMeshBlob(KY_NULL)
		, m_abstractGraphCount(0)
		, m_result(KY_SUCCESS)
	{
		m_tmpCellFilter = *KY_NEW CellFilter;
	}

	void SetOutputDirectory(const char* absoluteOutputBaseDir, const char* relativeOutputDir);
	KyResult InitFromFilename(Ptr<Database> database, const char* navDataFileName, FileOpenerBase* fileOpener = KY_NULL);
	KyResult InitFromFile(Ptr<Database> database, File* navDataFile);
	KyResult InitFromMemory(Ptr<Database> database, void* memory);
	KyResult InitFromNavData(Ptr<NavData> navData);

	void SetExtentInNumberOfCells(KyInt32 abstractGraphBoxExtentInCellSize) { m_boxExtentInCellSize = abstractGraphBoxExtentInCellSize; }

	KyResult Generate();

	KyResult SaveAbstractGraphData(const char* sectorName, FileOpenerBase* fileOpener = KY_NULL, Endianness::Type endianness = Endianness::BigEndian);

	void ClearAndRelease()
	{
		m_abstractGraphBlobHandlers.ClearAndRelease();
		m_tmpAbstractGraphBorderAsCellPos.ClearAndRelease();
		m_tmpBoundaryFlagsPerCellOnBorder.ClearAndRelease();
		m_abstractGraphCount = 0;
		m_abstractGraphGenerator.ClearAndRelease();
	}

public: // internal
	
	// Fulfill m_tmpAbstractGraphBorderAsCellPos with CellPos defining the border of the AbstractGraph (different from the boundary of the cellBox) 
	// Fulfill m_tmpBoundaryFlagsPerCellOnBorder with a mask of Kaim::CardinalDir defining the border on the cell
	// Load the cell from NavData that are in the given cellBox
	// Call DoGenerateAbstractGraph
	KyResult GenerateOneAbstractGraphFromCellBox(const CellBox& cellBox);
	KyResult DoGenerateAbstractGraph();

public:
	// ---------------- Input ------------------
	
	Ptr<Database> m_database;
	Ptr<NavData> m_navData;

	String m_absoluteOutputBaseDir;
	String m_relativeOutputDir;

	KyInt32 m_boxExtentInCellSize;

	// ---------------- Used over several calls to DoGenerateAbstractGraph ------------------

	NavMeshElementBlob* m_navMeshBlob; //< from m_navdata
	CellBox m_navDataCellBox;          //< from m_navdata
		
	AbstractGraphGenerator m_abstractGraphGenerator;
	KyUInt32 m_abstractGraphCount;
	KyArray< Ptr< BlobHandler<AbstractGraphBlob> > > m_abstractGraphBlobHandlers;

	AbstractGraphGenerationReport m_report;

	// ---------------- Used into DoGenerateAbstractGraph, setup by calling functions ------------------
	// m_tmpAbstractGraphBorderAsCellPos and m_tmpBoundaryFlagsPerCellOnBorder are expected to contain the same number of AbstractGraph

	Ptr<CellFilter> m_tmpCellFilter;
	KyArrayPOD<CellPos> m_tmpAbstractGraphBorderAsCellPos;        //< defines the CellPos of the border of the AbstractGraph, hence the CellPos expected to be used to create nodes of the AbstractGraph
	KyArrayPOD<KyUInt8> m_tmpBoundaryFlagsPerCellOnBorder;  //< defines the boundaries to test for the cell at CellPos given (same index in m_tmpAbstractGraphBorderAsCellPos)

	// ---------------- Output result ------------------
	KyResult m_result;
};

}

#endif
