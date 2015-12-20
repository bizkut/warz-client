//----------------------------------------------------------------------------------------------------------------------
/// \file   TrimData.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the clases required for trimmed Nurbs surfaces.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector4.h"
#include "XM2/Vector3Array.h"
#include "XM2/RealArray.h"
#include "XMD/XFn.h"
#include "XMD/FileIO.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::Curve2D
/// \brief A simple structure to hold a 2D trim curve
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT Curve2D
{
  /// u coords of curve CV's
  XRealArray uCoords;

  /// v coords of curve CV's
  XRealArray vCoords;

  /// The curves knot vector
  XRealArray Knots;

  /// The degree of the curve
  XS32 Degree;

#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XTrimData;
  XU32 GetDataSize() const;
  bool DoData(XFileIO&);
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::Curve3D
/// \brief A simple structure to hold a 3D trim curve
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT Curve3D 
{
  /// the control vertices
  XVector3Array CVs;

  /// the knot vector for the curve
  XRealArray Knots;

  /// the curve degree
  XS32 Degree;

#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XTrimData;
  bool DoData(XFileIO&);
  XU32 GetDataSize() const;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XTrimData
/// \brief This class is used to hold the TrimCurve information for a parametric surface. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XTrimData 
{
#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XParametricSurface;
  friend std::istream &operator>>(std::istream& ifs, XTrimData& td);
  friend std::ostream &operator<<(std::ostream& ofs, const XTrimData& td);
protected:

  /// ctor
  XTrimData();

  /// ctor
  XTrimData(const XTrimData& td);

  /// dtor
  ~XTrimData();

#endif
public:

  /// The type of the trim boundary
  enum BoundaryType 
  {
    kInner,
    kOuter,
    kSegment,
    kClosedSegment,
    kLast
  };

  /// \brief  returns the number of regions in the surface
  XS32 GetNumRegions() const;

  /// \brief  returns the number of boundaries in a given region
  /// \param  region  -  the region to query
  XS32 GetNumBoundaries(XS32 region) const;

  /// \brief  returns the number of curves within a given boundary
  /// \param  region    -  the region to query
  /// \param  boundary  -  the boundary to query
  XS32 GetNumCurves(XS32 region, XS32 boundary) const;

  /// \brief  returns the boundary type 
  /// \param  region    -  the region to query
  /// \param  boundary  -  the boundary to query
  BoundaryType GetBoundaryType(XS32 region, XS32 boundary) const;

  /// \brief  returns a pointer to the requested 2D parametric curve
  const Curve2D* Get2DCurve(XS32 region, XS32 boundary, XS32 curve) const;

  /// \brief  returns a pointer to the requested 3D parametric curve
  const Curve3D* Get3DCurve(XS32 region, XS32 boundary, XS32 curve) const;

  /// \brief  sets the boundary type 
  /// \param  region    -  the region to query
  /// \param  boundary  -  the boundary to query
  /// \param  type  - the boundary type for the specified boundary
  /// \return  true if OK
  bool SetBoundaryType(XS32 region, XS32 boundary, BoundaryType type);

  /// \brief  returns the number of regions in the surface
  /// \param  nregions  - the new number of regions
  /// \return  true if OK
  bool SetNumRegions(XU32 nregions);

  /// \brief  returns the number of boundaries in a given region
  /// \param  region  -  the region to query
  /// \param  nboundaries - the new number of boundaries
  /// \return  true if OK
  bool SetNumBoundaries(XS32 region, XU32 nboundaries);

  /// \brief  returns the number of boundaries in a given region
  /// \param  region    -  the region to query
  /// \param  boundary  -  the boundary to query
  /// \param  num      -  the number of curves to set on the boundary
  /// \return  true if OK
  bool SetNumCurves(XS32 region, XS32 boundary, XU32 num);

  /// \brief  returns a pointer to the requested 2D parametric curve
  /// \param  region    -  the region to query
  /// \param  boundary  -  the boundary to query
  /// \param  curve    -  the curve ID to query
  /// \param  data - the curve data to set
  /// \return  true if OK
  bool Set2DCurve(XS32 region, XS32 boundary, XS32 curve, const Curve2D& data);

  /// \brief  returns a pointer to the requested 3D parametric curve
  /// \param  region    -  the region to query
  /// \param  boundary  -  the boundary to query
  /// \param  curve    -  the curve ID to query
  /// \param  data - the curve data to set
  /// \return  true if OK
  bool Set3DCurve(XS32 region, XS32 boundary, XS32 curve, const Curve3D& data);

protected:
#ifndef DOXYGEN_PROCESSING

  bool DoData(XFileIO&);
  XU32 GetDataSize() const;

  /// \brief  A structure to hold a trim curve
  struct TrimCurve 
  {
    /// the 2D trim representation
    Curve2D m_2D;
    /// the 3D trim representation
    Curve3D m_3D;
    /// \brief  ctor
    TrimCurve():m_2D(),m_3D(){}
    bool DoData(XFileIO&);
    XU32 GetDataSize() const;
  };

  /// \brief  A structure to handle Surface boundaries. Basically contains a single type constant and a set of curves
  /// 
  struct Boundary
  {
    /// a vector of trim curves
    XM2::pod_vector<TrimCurve*> m_Curves;
    /// the type of the boundary
    BoundaryType m_Type;
    /// \brief  ctor
    Boundary();
    /// \brief  dtor
    ~Boundary();
    bool DoData(XFileIO&);
    XU32 GetDataSize() const;
  };

  /// \brief The regions basically define seperate sections of Nurbs
  ///        surface. For example, if you trimmed a surface to cut 
  ///        a hole in it, there would be one region (ie, one continuous
  ///        surface, allbeit with a hole). With a case of one hole, 
  ///        there would be two boundaries, the first an outer boundary
  ///        defining the edge of the surface, and the second would
  ///        be an inner boundary that defined the trimmed hole.
  struct Region
  {
    /// an array of surface regions
    XM2::pod_vector<Boundary*> m_Boundaries;
    /// \brief  ctor
    Region();
    /// \brief  dtor
    ~Region();
    bool DoData(XFileIO&);
    XU32 GetDataSize() const;
  };

  /// an array of surface regions
  XM2::pod_vector<Region*> m_Regions;
#endif
};
}
