/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_GeneratorGlobalConfig_H
#define GwNavGen_GeneratorGlobalConfig_H


#include "gwnavruntime/basesystem/coordsystem.h"
#include "gwnavgeneration/generator/generatoradvancedparameters.h"
#include "gwnavgeneration/input/taggedtriangle3i.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavruntime/database/navtag.h"

namespace Kaim
{

class BlobAggregate;
class DatabaseGenMetrics;

class GeneratorNormalizedParameters
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	GeneratorNormalizedParameters();
	void Clear();
	bool IsValid() const;

public:
	KyFloat32 m_normalizedRasterPrecision; // NearestInt(GeneratorParameters::m_entityRadius / GeneratorParameters::m_rasterPrecision)
	KyUInt32 m_cellSizeInPixel;
	KyUInt32 m_entityRadiusInPixel;
	KyUInt32 m_distanceFromWallsInPixel;
	KyUInt32 m_distanceFromHolesInPixel;
	KyFloat32 m_normalizedCellSize;     /// m_cellSizeInPixel     * m_normalizedRasterPrecision
	KyFloat32 m_normalizedEntityRadius; /// m_entityRadiusInPixel * m_normalizedRasterPrecision
};


class GeneratorParametersBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Generator, GeneratorParametersBlob, 0)
	KY_CLASS_WITHOUT_COPY(GeneratorParametersBlob)
public:
	GeneratorParametersBlob() {}
	BlobFieldArray m_fields;
	NavTag m_defaultNavTag;
	VisualColor m_emptyDefaultNavTagColor;
	VisualColor m_nonEmptyDefaultNavTagColor;
	VisualColor m_nonDefaultNavTagColor;
};
inline void SwapEndianness(Endianness::Target e, GeneratorParametersBlob& self)
{
	SwapEndianness(e, self.m_fields);
	SwapEndianness(e, self.m_defaultNavTag);
	SwapEndianness(e, self.m_emptyDefaultNavTagColor);
	SwapEndianness(e, self.m_nonEmptyDefaultNavTagColor);
	SwapEndianness(e, self.m_nonDefaultNavTagColor);
}


/// The GeneratorParameters class contains configuration parameters that control the characteristics of the NavData 
/// created by the Generator. You can create an instance of this class, set up its data members as desired, and 
/// pass it in a call to Generator::SetGeneratorParameters().
class GeneratorParameters
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	// The binary backward compatibility of the GeneratorParameters blob is ensured by its nature (BlobFieldMapping) 
	// However, when a parameter is added or renamed its value will be set to the default value (from Ctor) 
	// when loading older versions of configs (from older clientInput). 
	// This behavior might have some random effects. 
	enum { Version = 1 };

public:
	GeneratorParameters(); /// default constructor
	void Clear(); ///< Reset members as if the object was freshly constructed
	void SetDefaultValues() { Clear(); } ///< alias for Clear()

	void ComputeNormalizedParams();

	// Helper function allowing to transform a genParams to its runtime equivalent 
	void ComputeDatabaseGenMetrics(DatabaseGenMetrics& genMetrics) const;

	/// Build a GeneratorParametersBlob from these parameters.
	void BuildBlob(BlobHandler<GeneratorParametersBlob>& handler) const;

	/// \copydoc ReadFromBlob()
	KyResult ReadFromBlobHandler(BlobHandler<GeneratorParametersBlob>& handler) { return ReadFromBlob(*handler.Blob()); }

	/// Read parameters values from a BlobAggregate.
	/// \returns #KY_SUCCESS if one and only one GeneratorParametersBlob was found in the aggregate, #KY_ERROR otherwise.
	KyResult ReadFromAggregate(BlobAggregate& aggregate);

	/// \return true if the specified GeneratorParameters contains the same values as this object. 
	bool operator==(const GeneratorParameters& other) const;

	/// \return true if the specified GeneratorParameters contains at least one value that is different from this object. 
	bool operator!=(const GeneratorParameters& other) const { return !operator==(other); }

	void InitBlobFieldMapping(BlobFieldsMapping& mapping, KyUInt32& version);
	void InitBlobFieldMapping(BlobFieldsMapping& mapping, KyUInt32& version) const;

	/// Read parameters values from a GeneratorParametersBlob.
	/// \returns #KY_SUCCESS if GeneratorParametersBlob has been successfully read, #KY_ERROR otherwise (namely in case of bad Blob version).
	KyResult ReadFromBlob(const GeneratorParametersBlob& blob);

	void SetDefaultNavTag(const KyArrayPOD<KyUInt32>& blindDataArray) { SetDefaultNavTag(blindDataArray.GetDataPtr(), blindDataArray.GetCount()); }
	void SetDefaultNavTag(const KyUInt32* blindDataBuffer, KyUInt32 blindDataCount);
	void SetEmptyDefaultNavTagColor(VisualColor color);
	void SetNonEmptyDefaultNavTagColor(VisualColor color);
	void SetNonDefaultNavTagColor(VisualColor color);

	const DynamicNavTag& GetDefaultNavTag() const { return m_defaultNavTag; }

public:
	/// The height of the character that will use the NavData at runtime.
	/// \units           meters
	/// \defaultvalue    2.0f
	KyFloat32 m_entityHeight;

	/// The radius (or half-width) of the character that will use the NavData at runtime. Particularly this radius is used to keep the
	/// navMesh borders at a distance of #m_entityRadius from the walls, so that characters do not collide with the at runtime.
	/// \units           meters
	/// \defaultvalue    0.4f
	KyFloat32 m_entityRadius;

	/// The maximum difference in altitude that the character that will use the NavData at runtime can traverse in its normal movement. 
	/// \units           meters
	/// \defaultvalue    0.6f
	KyFloat32 m_stepMax;

	/// The maximum slope that the character that will use the NavData at runtime can traverse in its normal movement. Any input
	///  triangle with a slope greater than this value is automatically tagged with the exclusive NavTag, and no  NavData will be
	///  generated for that triangle.
	/// \units           degrees
	/// \defaultvalue    50.0f
	KyFloat32 m_slopeMax;

	/// Determines the approximate width and length of each pixel used to rasterize the input triangles.
	/// This value is rounded to a "Normalized" value so that a pixel is exactly m_entityRadius / N
	/// where N is an integer value of at least 1.
	/// Ex: m_entityRadius = 0.4, m_rasterPrecision = 0.25 => m_normalizedRasterPrecision = 0.2
	/// \units           meters
	/// \defaultvalue    0.2f
	KyFloat32 m_rasterPrecision;

	/// Determines the approximate width and length of each cell in the grid used to partition the NavMesh internally.
	/// This value will be rounded so that a cell contains exactly "n" pixels where n is an integer
	/// The "Normalized" value is accessible using ComputeNormalizedCellSize()
	/// \units           meters
	/// \defaultvalue    20.0f
	KyFloat32 m_cellSize;

	/// Determines the maximum difference in altitude that may exist between the NavMesh and the original terrain mesh.
	/// The way this parameter is interpreted depends on whether or not the Generator is configured to
	/// generate vertically accurate NavData: i.e. on the value of #m_doUseAltitudeTolerance.
	/// -	When vertically accurate NavMesh generation is enabled (the default), this parameter determines the maximum vertical
	/// 	distance that the NavMesh triangles may be from the original raster at any point.
	/// -	When vertically accurate NavMesh generation is disabled, this parameter is only taken into account at the edges
	/// 	of NavMesh triangles that compose the borders between NavCells and NavFloors. It has no effect on the NavData
	/// 	generated in the interior of the cells themselves.
	/// Lower values typically result in a NavMesh with smaller triangles that is more faithful to the original 
	/// rasterization. Larger values simplify the NavMesh more aggressively, and typically result in a NavMesh with fewer,
	/// larger triangles. In most cases, the default value should be sufficient unless you need an accurate NavMesh for your
	/// own purposes outside of Gameware Navigation.
	/// \units           meters
	/// \defaultvalue    0.5f
	KyFloat32 m_altitudeTolerance;

	/// Determines whether or not the Generator performs a post-processing step to ensure that altitude of the NavData accurately
	/// reflects the altitude of the terrain mesh.
	/// -	Without this post-processing step, the altitude of the NavData is guaranteed to match the altitude of the terrain mesh
	/// 	only at the boundaries between adjacent cells. Within each cell, the altitude of the NavData is not guaranteed to match
	/// 	the terrain mesh with any degree of precision. 
	/// -	Carrying out the post-processing step increases the vertical accuracy of the final NavMesh, but may increase the size of 
	/// 	the final NavData and the time required to generate it.
	/// \defaultvalue   true
	bool m_doUseAltitudeTolerance;

	/// Contains more advanced configuration parameters. The default values of the parameters offered by GeneratorAdvancedParameters
	/// should suffice for most projects. 
	GeneratorAdvancedParameters m_advancedParameters;

	// computed in ComputeNormalizedParams(), do not set directly
	GeneratorNormalizedParameters m_normalizedParameters;

private:
	/// Specifies the default NavTag that will be applied to all input triangles that do not otherwise have a NavTag set explicitly.
	DynamicNavTag m_defaultNavTag;
};


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, GeneratorParameters& params)
{
	BlobFieldsMapping mapping;
	KyUInt32 version = 0;
	params.InitBlobFieldMapping(mapping, version);

	os << KY_LOG_SMALL_TITLE_BEGIN("", "GeneratorParameters Info");
	os << mapping;
	// os << m_defaultNavTag;
	os << KY_LOG_SMALL_TITLE_END("", "GeneratorParameters Info");
	return os;
}


class GeneratorParametersBlobBuilder : public BaseBlobBuilder<GeneratorParametersBlob>
{
public:
	GeneratorParametersBlobBuilder(const GeneratorParameters* params) : m_params(params) {}

private:
	virtual void DoBuild();

	const GeneratorParameters* m_params;
};

} // namespace Kaim

#endif
