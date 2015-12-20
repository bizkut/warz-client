//----------------------------------------------------------------------------------------------------------------------
/// \file   Lattice.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a 3D grid of points that are used to control an FFD deformation
/// \note   I'm adopting the maya solution for FFd's because it 
///         seems fairly reasonable. Basically we have three main parts
///
///           1. XBaseLattice - this is simply a cuboid shape that
///               defines a volume in space, in which the points must
///               be, before they are deformed.
/// 
///            2. XLattice - this is a geometry object that is a cubic
///               grid of points defining a deformed version of the lattice.
///               The number of points on the surface is controlled by 
///               the actual ffd deformer
/// 
///            3. XFFD - This is the actual deformer class responsible
///               for manipulating the geometry data.
/// \todo   Handle EFFD's from 3DS max.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
# pragma once
#endif
#ifndef XMDAPI__Lattice__H__
#define XMDAPI__Lattice__H__
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Geometry.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
// forward declaration
class XMD_EXPORT XBaseLattice;
class XMD_EXPORT XFFD;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief The lattice is used to calculate the deformed points in
///        the geometry that is being deformed. The curve uses the 
///        following basis functions for calculation of the different
///        number of CV's
/// 
///           2  - simple linear interpolation
///           3  - quadratic interpolation
///           4  - bezier interpolation
///           5+ - Cubic Nurbs curve. 
/// 
///        If a NURBS curve is used for the evaluation, then the 
///        knot vector is calculated as follows :
/// 
///           * The first 4 knots are always 0
///           * The last 4 knots are always (numCvs-3)
///           * In between knots go 1,2,3,4 etc etc...
/// 
///        Although the original FFD paper states the use of bezier
///        curves, generally it's better to use a NURBS basis instead.
class XMD_EXPORT XLattice :
  public XGeometry
{
  friend class XMD_EXPORT XFFD;
  XMD_CHUNK(XLattice);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kDivisionsS  = XGeometry::kLast+1; // XU32
  static const XU32 kDivisionsT  = XGeometry::kLast+2; // XU32
  static const XU32 kDivisionsU  = XGeometry::kLast+3; // XU32
  static const XU32 kFFD         = XGeometry::kLast+2; // XBase*
  static const XU32 kBaseLattice = XGeometry::kLast+3; // XBase*
  static const XU32 kLast        = kBaseLattice;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Lattice Shape Params
  /// \brief  Sets/Gets the params for the XLattice class
  //@{

  /// \brief  This function retrieves the lattice divisions
  /// \param  i - the num divisions in i
  /// \param  j - the num divisions in j
  /// \param  k - the num divisions in k
  void GetNumDivisions(XU32& i, XU32& j, XU32& k) const;

  /// \brief  This function sets the lattice divisions
  /// \param  i - the num divisions in i
  /// \param  j - the num divisions in j
  /// \param  k - the num divisions in k
  void SetNumDivisions(XU32 i, XU32 j, XU32 k);

  /// \brief  returns a pointer to the base lattice
  /// \return a pointer to the base lattice shape
  XBaseLattice* GetBaseLattice();

  /// \brief  This function returns a pointer to the FFD deformer
  /// \return a pointer to the associated deformer
  XFFD* GetFFD();

  /// \brief  returns the requested point
  /// \param  i  -  the i coord
  /// \param  j  -  the j coord
  /// \param  k  -  the k coord
  /// \return the actual point
  const XVector3& GetPoint(XU32 i, XU32 j, XU32 k) const;

  /// \brief  sets the requested point
  /// \param  i  -  the i coord
  /// \param  j  -  the j coord
  /// \param  k  -  the k coord
  /// \param  point - the new point value
  void SetPoint(XU32 i, XU32 j, XU32 k, const XVector3& point);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  void PostLoad();
  XId m_BaseLattice;
  XId m_FFD;
  XU32 m_nDivisionsI;
  XU32 m_nDivisionsJ;
  XU32 m_nDivisionsK;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Lattice, XLattice, "lattice", "LATTICE");
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#endif // XMDAPI__Lattice__H__
//----------------------------------------------------------------------------------------------------------------------
