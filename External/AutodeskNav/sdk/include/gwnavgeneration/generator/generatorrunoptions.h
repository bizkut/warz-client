/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_GeneratorRunOptions_H
#define GwNavGen_GeneratorRunOptions_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavgeneration/generator/generatorvisualdebugoptions.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/blob/blobfieldarray.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavgeneration/generator/intermediatefilesconfig.h"


namespace Kaim
{

class BlobAggregate;


class GeneratorRunOptionsBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
		KY_ROOT_BLOB_CLASS(Generator, GeneratorRunOptionsBlob, 0)
		KY_CLASS_WITHOUT_COPY(GeneratorRunOptionsBlob)
public:
	GeneratorRunOptionsBlob() {}
	BlobFieldArray m_fields;
};
inline void SwapEndianness(Endianness::Target e, GeneratorRunOptionsBlob& self)
{
	SwapEndianness(e, self.m_fields);
}


/// The GeneratorRunOptions class contains configuration parameters that control the way in which the Generator
/// carries out the process of generating NavData for its sectors. You can create an instance of this class,
/// set up its data members as desired, and pass it in a call to Generator::SetRunOptions().
class GeneratorRunOptions
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	enum { Version = 0 };

public:
	GeneratorRunOptions();
	void Clear(); ///< Reset members as if the object was freshly constructed
	void SetDefaultValues() { Clear(); } ///< alias for Clear()

	/// Read accessor for #m_intermediateFiles. 
	const IntermediateFilesConfig& Intermediates() const { return m_intermediateFiles; }

	/// Const read accessor for #m_intermediateFiles. 
	IntermediateFilesConfig& Intermediates() { return m_intermediateFiles; }

	/// Restricts NavData generation to the specified cell, and generates intermediate data for that cell. 
	/// Used to troubleshoot localized problems with NavData generation. 
	void AddCellToBuild(const CellPos& pos) { m_intermediateFiles.Add(pos); }

	/// Indicates whether or not the Generator will actually use parallel processing. When the Generator is configured 
	/// to write intermediate data, multi-core generation is deactivated. 
	bool DoReallyUseMultiCore() const { return m_doMultiCore && !m_intermediateFiles.IsEnabled(); }

	/// For internal use. 
	bool DoReallyUseVisualDebug() const { return !DoReallyUseMultiCore() && m_visualDebugOptions.IsActive(); }

	void BuildBlob(BlobHandler<GeneratorRunOptionsBlob>& handler) const;
	KyResult ReadBlob(const GeneratorRunOptionsBlob& blob);
	KyResult ReadFromAggregate(BlobAggregate& aggregate);

	void InitBlobFieldsMapping(BlobFieldsMapping& mapping);
	void InitBlobFieldsMapping(BlobFieldsMapping& mapping) const;

public:
	/// Incremented each time a Generator operates on the GeneratorInputOutput.
	/// Allows to give a unique fileName to Patches when generating iteratively many of them.
	/// \defaultvalue 0
	KyUInt32 m_generationRevision;

	/// Determines whether or not the Generator saves the GeneratorInputOutput content.
	/// Check the class GeneratorInputOutput.
	/// \defaultvalue \c true
	bool m_doSaveGeneratorInputOutput;

	/// FileName of the saved GeneratorInputOutput
	/// \defaultvalue \c "Generator"
	String m_generatorInputOuputSaveFileName;

	/// Determines whether or not the NavData generation system spreads computations across all available CPUs on your computer.
	/// \defaultvalue \c true
	bool m_doMultiCore;

	/// Determines whether or not .NavData files are written.
	/// \defaultvalue \c true 
	bool m_doWriteNavDataFiles;

	/// Determines whether or not .ColData files are written.
	/// \defaultvalue \c true 
	bool m_doWriteColDataFiles;

	/// Determines whether or not the input geometry consumption is buffered to avoid exceeding the memory limitations on 32 bit.
	/// If activated, sectors added with GenFlags::SECTOR_CLIENT_INPUT_SAVE_ENABLED could generate several .ClientInput files,
	/// i.e. one main .ClientInput which refers to other files ending with .part.X.ClientInput where X is a number.
	/// Activating this mode might slightly increase the generation time.
	/// \defaultvalue	\c false 
	bool m_doEnableLimitedMemoryMode;

	/// Size of the internally used tiles for large scale generations and/or running generation with low memory.
	/// Used when m_doEnableLimitedMemoryMode is true.
	/// \defaultvalue 25
	KyUInt32 m_limitedMemoryModeTileSizeInNbCells;

	/// Determines the endianness of the final output files. This allows you to avoid swapping the byte order of the data when loading
	/// NavData on a machine with a different endianness.
	/// \acceptedvalues Any element from the Endianness::Type enumeration.
	/// \defaultvalue \c Endianness::BigEndian 
	KyUInt32 m_outputFilesEndianness;

	/// Determines whether or not the Generator uses thread local storage when an ITlsAlloc object is provided in its constructor.
	/// \defaultvalue \c true
	bool m_doUseTlsIfAvailable;

	/// Determines whether or not the Generator will profile its CPU performance.
	/// \defaultvalue \c false
	bool m_doProfile;

	/// Determines whether or not the Generator will log its configuration parameters in its report for each
	/// run. See also #m_doLogReport.
	/// \defaultvalue \c false
	bool m_doLogConfiguration;

	/// Determines whether or not the Generator will log a report the results of each run. When this parameter is set to \c true,
	/// you can access the report by calling Generator::GetGeneratorReport() after the generation has completed.
	/// \defaultvalue \c true
	bool m_doLogReport;

	/// Determines whether or not the Generator will log verbose details in its report for each run. See also #m_doLogReport.
	/// \defaultvalue \c false
	bool m_doLogReportDetails;

	/// Determines whether or not the Generator will log per Sector information in its report for each run. See also #m_doLogReport.
	/// \defaultvalue \c true
	bool m_doLogPerSectorReport;

	/// Determines whether or not the Generator will log the progress of the Generation, useful from very long generation.
	/// \defaultvalue \c false
	bool m_doLogProgress;

	/// For internal use. Do not modify.
	GeneratorVisualDebugOptions m_visualDebugOptions;

	/// Contains configuration parameters that relate to the generation of intermediate data files. 
	IntermediateFilesConfig m_intermediateFiles;
};

template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, GeneratorRunOptions& runOptions)
{
	os << KY_LOG_SMALL_TITLE_BEGIN("", "GeneratorRunOptions");

	os << "generationRevision                  : " << runOptions.m_generationRevision                 << Endl;
	os << "doSaveGeneratorInputOutput          : " << runOptions.m_doSaveGeneratorInputOutput         << Endl;
	os << "generatorInputOuputSaveFileName     : " << runOptions.m_generatorInputOuputSaveFileName    << Endl;
	os << "doMultiCore                         : " << runOptions.m_doMultiCore                        << Endl;
	os << "doWriteNavDataFiles                 : " << runOptions.m_doWriteNavDataFiles                << Endl;
	os << "doWriteColDataFiles                 : " << runOptions.m_doWriteColDataFiles                << Endl;
	os << "doEnableLimitedMemoryMode           : " << runOptions.m_doEnableLimitedMemoryMode          << Endl;
	os << "limitedMemoryModeTileSizeInNbCells  : " << runOptions.m_limitedMemoryModeTileSizeInNbCells << Endl;
	os << "outputFilesEndianness               : " << runOptions.m_outputFilesEndianness              << Endl;
	os << "doUseTlsIfAvailable                 : " << runOptions.m_doUseTlsIfAvailable                << Endl;
	os << "doProfile                           : " << runOptions.m_doProfile                          << Endl;
	os << "doLogConfiguration                  : " << runOptions.m_doLogConfiguration                 << Endl;
	os << "doLogReport                         : " << runOptions.m_doLogReport                        << Endl;
	os << "doLogReportDetails                  : " << runOptions.m_doLogReportDetails                 << Endl;
	os << "doLogPerSectorReport                : " << runOptions.m_doLogPerSectorReport               << Endl;
	
	os << KY_LOG_SMALL_TITLE_END("", "GeneratorRunOptions");

	return os;
}


class GeneratorRunOptionsBlobBuilder : public BaseBlobBuilder<GeneratorRunOptionsBlob>
{
public:
	GeneratorRunOptionsBlobBuilder(const GeneratorRunOptions* runOptions) : m_runOptions(runOptions) {}
private:
	virtual void DoBuild();
	const GeneratorRunOptions* m_runOptions;
};


} // namespace Kaim

#endif // GwNavGen_GeneratorRunOptions_H

