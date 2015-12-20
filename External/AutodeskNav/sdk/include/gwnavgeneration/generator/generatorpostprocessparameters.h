/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef GwNavGen_GeneratorAbstractGraphParameters_H
#define GwNavGen_GeneratorAbstractGraphParameters_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/blob/blobfieldarray.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavruntime/basesystem/logger.h"

namespace Kaim
{

class BlobAggregate;

class GeneratorAbstractGraphParametersBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Generator, GeneratorAbstractGraphParametersBlob, 0)
	KY_CLASS_WITHOUT_COPY(GeneratorAbstractGraphParametersBlob)
public:
	GeneratorAbstractGraphParametersBlob() {}
	BlobFieldArray m_fields;
};
inline void SwapEndianness(Endianness::Target e, GeneratorAbstractGraphParametersBlob& self)
{
	SwapEndianness(e, self.m_fields);
}

enum GenerateAbstractGraphs
{
	GENERATE_ABSTRACTGRAPHS_DISABLE      = 0, // no abstract graph generated
	GENERATE_ABSTRACTGRAPHS_ENABLE       = 1, // abstract graph generated only for sectors which did change (cf. SECTOR_CHANGED)
	GENERATE_ABSTRACTGRAPHS_FORCE_ENABLE = 2, // abstract graph generated for all sectors even if they didn't change (cf. SECTOR_NOCHANGE)
};

class GeneratorAbstractGraphParameters
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	GeneratorAbstractGraphParameters();
	void Clear(); ///< Reset members as if the object was freshly constructed

	/// Build a GeneratorParametersBlob from these parameters.
	void BuildBlob(BlobHandler<GeneratorAbstractGraphParametersBlob>& handler) const;

	/// Read parameters values from a BlobAggregate.
	/// \returns #KY_SUCCESS if one and only one GeneratorParametersBlob was found in the aggregate, #KY_ERROR otherwise.
	KyResult ReadFromAggregate(BlobAggregate& aggregate);

	KyResult ReadBlob(const GeneratorAbstractGraphParametersBlob& blob);

	/// For internal use. 
	void InitBlobFieldsMapping(BlobFieldsMapping& mapping);
	void InitBlobFieldsMapping(BlobFieldsMapping& mapping) const;

	/// \return true if all data values in this object match the values in the specified instance of GeneratorAbstractGraphParameters. 
	bool operator==(const GeneratorAbstractGraphParameters& other) const
	{
		return  m_doGenerateAbstractGraphs == other.m_doGenerateAbstractGraphs
			&& m_extentsInNumberOfCells == other.m_extentsInNumberOfCells;
	}

	/// \return true if at least one data value in this object does not match the value in the specified instance of GeneratorAbstractGraphParameters. 
	bool operator!=(const GeneratorAbstractGraphParameters& other) const { return !operator==(other); }

public:
	/// Indicates if AbstractGraphs must be generated, 
	/// note that AbstractGraphs are only generated for Regular NavData with a specified NavDataCellBox
	KyUInt32 m_doGenerateAbstractGraphs;
	/// Gives the maximum extents of an AbstractGraph to be generated into a NavData
	/// By default, it is set to 0 which means that one AbstractGraph is generated for the whole NavData
	KyUInt32 m_extentsInNumberOfCells;
	/// The size limit in Bytes set to the WorkingMemory when generating AbstractGraph
	KyUInt32 m_workingMemorySizeLimit;
};


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, GeneratorAbstractGraphParameters& params)
{
	BlobFieldsMapping mapping;
	params.InitBlobFieldsMapping(mapping);

	os << KY_LOG_SMALL_TITLE_BEGIN("", "GeneratorAbstractGraphParameters Info");
	os << mapping;
	os << KY_LOG_SMALL_TITLE_END("", "GeneratorAbstractGraphParameters Info");
	return os;
}


class GeneratorPostProcessParametersBlobBuilder : public BaseBlobBuilder<GeneratorAbstractGraphParametersBlob>
{
public:
	GeneratorPostProcessParametersBlobBuilder(const GeneratorAbstractGraphParameters* params)
		: m_params(params)
	{}

private:
	virtual void DoBuild();

	const GeneratorAbstractGraphParameters* m_params;
};

} // namespace Kaim


#endif

