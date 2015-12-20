/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LASI - secondary contact: NOBODY
#ifndef GwNavGen_ClientInput_H
#define GwNavGen_ClientInput_H


#include "gwnavgeneration/generator/generatorparameters.h"
#include "gwnavgeneration/input/clientinputchunk.h"
#include "gwnavgeneration/input/clientinputdescriptor.h"
#include "gwnavgeneration/input/clientinputsectordescriptor.h"
#include "gwnavruntime/base/kyguid.h"


namespace Kaim
{

class GeneratorSystem;
class File;
class FileOpenerBase;

/// The ClientInput is a representation of the input that was supplied to the generator for a particular Kaim::GeneratorSector
/// It contains all the input triangles and tagVolumes as provided by the the Kaim::GeneratorInputProducer to the Kaim::ClientInputConsumer
/// and can be saved or loaded to a .ClientInput file
class ClientInput
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(ClientInput)

public:
	// ---------------------------------- Constructor and destructor  ----------------------------------
	ClientInput() : m_sectorGuid(KyGuid::GetInvalidGuid()), m_sys(KY_NULL) {}
	~ClientInput() { Clear(); }
	void Clear();

	// ---------------------------------- Accessors  ----------------------------------
	const KyGuid* GetGuid() const;
	KyUInt32 GetDatabaseIndex() const { return m_sectorDescriptor.m_databaseIndex; }

	// ---------------------------------- Save and Load from file ----------------------------------
	KyResult Save(const char* fullFileName);
	KyResult Load(const char* fullFileName, FileOpenerBase* fileOpener);

	// ---------------------------------- Internals ----------------------------------
	void Init(GeneratorSystem* sys);
	void InitFromDynamicClientInputChunk(GeneratorSystem* sys, const DynamicClientInputChunk& dynamicClientInputChunk);

	Kaim::Box3f ComputeTrianglesBoundingBox();

private:
	KyResult Load(File* file);

	/// Called by Load() once the m_clientInputParts strings have been retrieved from root client Input
	KyResult LoadParts(const char* fullFileName, FileOpenerBase* fileOpener);

public:
	/// The global config used for this Kaim::ClientInput
	GeneratorParameters m_globalConfig;

	/// The Kaim::KyGuid of the Kaim::GeneratorSector associated with this Kaim::ClientInput
	KyGuid m_sectorGuid;

	// Information about the sector (databaseIndex)
	ClientInputSectorDescriptor m_sectorDescriptor;

	KyArray< Ptr<BlobHandler<ClientInputChunk> > > m_clientInputChunks; //< Internal data containing the inputs
	KyArray<ClientInputDescriptor> m_clientInputParts; //< Internal usage for buffering
	GeneratorSystem* m_sys; //< Internal usage from within the generation
};



}


#endif

