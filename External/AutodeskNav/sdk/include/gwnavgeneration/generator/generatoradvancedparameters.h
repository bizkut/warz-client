/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_GeneratorAdvancedParameters_H
#define GwNavGen_GeneratorAdvancedParameters_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/blob/blobfieldarray.h"
#include "gwnavruntime/visualsystem/visualcolor.h"

namespace Kaim
{

/// Enumerates the different possible settings for GeneratorAdvancedParameters::m_navRasterDistanceMapMetric.
enum NavRasterDistanceMapMetric
{
	/// Indicates the Manhattan metric, which is faster and more accurate.
	/// This metric tends to create bevels in the NavMesh border near corners,which produces more triangles. 
	DISTANCE_MAP_MANHATTAN_METRIC = 0,

	/// Indicates the Chessboard metric, which is slightly slower and can place the NavMesh border a little
	/// farther from the geometry. This metric tends to creates a cleaner NavMesh with fewer triangles. 
	DISTANCE_MAP_CHESSBOARD_METRIC = 1
};

/// The GeneratorAdvancedParameters class is used by the GeneratorParameters class to maintain a set of configuration parameters
/// that control detailed technical aspects of the NavData generation process.
class GeneratorAdvancedParameters
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	GeneratorAdvancedParameters() { SetDefaultValues(); }

	/// Clears all information maintained by this object. 
	void SetDefaultValues();

	/// For internal use. 
	void InitBlobFieldMapping(BlobFieldsMapping& mapping);

	/// \return true if all data values in this object match the values in the specified instance of GeneratorAdvancedParameters. 
	bool operator==(const GeneratorAdvancedParameters& other) const;

	/// \return true if at least one data value in this object does not match the value in the specified instance of GeneratorAdvancedParameters. 
	bool operator!=(const GeneratorAdvancedParameters& other) const { return !operator==(other); }

public:
	/// The distance used to move the navMesh borders from the the holes, so that characters do not "fall" in the hole at runtime.
	/// A negative value means that we used the half of #m_entityRadius for that (default behavior). If a non-negative value is set,
	/// it will be used.
	/// \units           meters
	/// \defaultvalue    -1.f
	KyFloat32 m_distanceFromHoles;

	/// Determines the tolerance used when simplifying the outlines of the navMesh. This simplification reduces the number of triangles
	/// in the NavMesh (and thereby, reduces its memory size) by trimming off or smoothing over small irregularities along the border.
	/// This parameter defines the 2d distance that the NavMesh border may be shifted toward the interior or exterior from the original border
	/// got from rasterization.
	/// \units           meters
	/// \defaultvalue    0.4f 
	KyFloat32 m_inwardBorderTolerance;

	/// This paramter can be used to get very clean (but also less precise) NavMesh. This value is added to the radius during the erosion of the pixels
	/// and also to m_inwardBorderTolerance during the boundary simplification process.
	/// \units           meters
	/// \defaultvalue    0.0f 
	KyFloat32 m_superSmoothExtraTolerance;

	/// This parameter is used only when the Generator is configured to generate vertically accurate NavData: i.e. when
	/// GeneratorParameters::m_doUseAltitudeTolerance is set to GeneratorParameters::VERTICAL_ACCURACY_ENABLED.
	/// It determines the resolution used to sample the NavMesh and compare it to the original raster.
	/// Lower values typically result in NavData with more triangles that matches the altitude of the original terrain mesh
	/// more closely, but may increase generation time.
	/// Note that the sampling step is internally snapped to an integer value to make sure that #m_altitudeToleranceSamplingStep
	/// is in fact a multiple of the original rasterization pixel size (see GeneratorParameters::m_rasterPrecision). Therefore, any
	/// value less than twice the pixel size will be clamped to the pixel size, values between 2.0f and 3.0f times the pixel size
	/// will be clamped to twice the pixel size, etc.
	/// \units 			meters
	/// \defaultvalue    0.30f 
	KyFloat32 m_altitudeToleranceSamplingStep; //in m

	/// Sets the minimum surface area that any isolated area of NavMesh may occupy to no be considered as noise and discarded during the generation process.
	/// Seedpoints will be ignored when considering such surfaces.
	/// If you want to explicitely keep some small isolated area by placing seedpoints, consider using m_minNavigableSurface instead. 
	//// \units			square meters
	/// \defaultvalue    1.0f
	KyFloat32 m_noiseReductionSurface;  //in m2

	/// Sets the minimum surface area that any isolated area of NavMesh may occupy. 
	/// Any NavMesh with a total surface area smaller than this value is discarded
	/// Unless a SeedPoint was placed in this region. 
	/// If you want to ignore seedpoints when performing surface filtering, consider using m_noiseReductionSurface. 
	/// \units 			square meters
	/// \defaultvalue    10.0f 
	KyFloat32 m_minNavigableSurface; //in m2

	/// Sets the vertical tolerance used to select the NavTag to be applied to a raster pixel. When multiple triangles
	/// with different NavTags are merged into the same block in the raster, this parameter controls the vertical
	/// distance that is checked to determine the best NavTag to apply. If set to0.0f, the NavTag of the
	/// uppermost triangle is always selected. For any other positive value, the NavTag::operator< is used to determine the 
	/// "Winning" NavTag among all the triangles that contribute to a pixel within the tolerance range. 
	/// This parameter is used mostly to avoid Z-fighting where triangle with different NavTags overlap. 
	/// \units 			meters
	/// \defaultvalue    0.1f 
	KyFloat32 m_navTagRasterMergeTolerance; //in meters

	/// This parameter is used for filtering noise that might appear when using NavTags. In certain cases, 
	/// small areas with heterogeneous NavTags can result in very small NavFloors of a few isolated pixels.
	/// In order to avoid this side effect, a filtering is performed and areas smaller than #m_navTagMinPixelArea
	/// that are isolated in the middle of different NavTag will be merged to that region.	
	/// If 0, no filtering will be performed.
	/// \units 			pixels
	/// \defaultvalue    6 pixels 
	KyUInt32 m_navTagMinPixelArea; //in pixels

	/// Determines the distance map metric that will be used to compute the navRaster from the rasterized geometry. 
	/// The metric is used to estimate the distance of every pixel from an obstacle or a hole. This metric has an
	/// influence on the overall quality and accuracy of the NavMesh, particularly at corners.
	/// -#DISTANCE_MAP_MANHATTAN_METRIC uses standard Manhattan distance: http://en.wikipedia.org/wiki/Manhattan_distance
	/// which is a more accurate estimate than theDISTANCE_MAP_CHESSBOARD_METRIC but which tends to create some "Diamond" 
	/// patterns around square obstacles. 
	/// -#DISTANCE_MAP_CHESSBOARD_METRIC (aka Chebychev) uses http://en.wikipedia.org/wiki/Chessboard_distance to estimate
	/// the distance. It is less accurate but creates more "regular" features (i.e. less triangles in the final NavMesh).
	/// \acceptedvalues	An element from the #NavRasterDistanceMapMetric enumeration.
	/// \defaultvalue    #DISTANCE_MAP_MANHATTAN_METRIC 
	KyUInt32 m_navRasterDistanceMapMetric;

	/// Determines whether or not back-face triangles (i.e. triangles whose points go in counter-clockwise order when seen
	/// from above) are flagged as non walkable
	/// \defaultvalue      true
	bool m_backFaceTrianglesWalkable;

	VisualColor m_emptyDefaultNavTagColor;
	VisualColor m_nonEmptyDefaultNavTagColor;
	VisualColor m_nonDefaultNavTagColor;
};


} // namespace Kaim


#endif

