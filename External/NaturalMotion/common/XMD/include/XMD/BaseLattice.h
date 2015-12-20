//----------------------------------------------------------------------------------------------------------------------
/// \file   BaseLattice.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Basically the base lattice is in control of which
///         points are going to be affected by the lattice. ie, it
///         defines a cuboid volume in which the lattice points will
///         be offset with respect to a deformed lattice (XLattice).
/// \note   Most of these lattice operations have to occur within
///         world space. This poses a small problem. If we were to
///         transform all points into world space we would have a 
///         wee bit of an expensive operation on our hands! To avoid
///         this, i transform the base and deformed lattices into 
///         the meshes local space and perform the comparison there.
/// 
///         The XFFD node will directly access both the XBaseLattice
///         and XLattice classes to setup some temporary data. 
///
///         (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
///
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Geometry.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBaseLattice
/// \brief Defines the base lattice shape that determines the main
///        area within which to deform any related points.
///        This class on its own really does very very little in terms
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBaseLattice :
  public XShape
{
  XMD_CHUNK(XBaseLattice);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kXSize = XShape::kLast+1;
  static const XU32 kYSize = XShape::kLast+2;
  static const XU32 kZSize = XShape::kLast+3;
  static const XU32 kLast  = kZSize;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Base Lattice Params
  /// \brief  Sets/Gets the params for the XBaseLattice
  //@{

  /// \brief  returns the size of the base lattice in the X axis
  /// \return the size of the base lattice in X
  XReal GetXSize() const;

  /// \brief  returns the size of the base lattice in the Y axis
  /// \return the size of the base lattice in Y
  XReal GetYSize() const;

  /// \brief  returns the size of the base lattice in the Z axis
  /// \return the size of the base lattice in Z
  XReal GetZSize() const;

  /// \brief  sets the size of the base lattice in X
  /// \param  x - the size in the x axis
  void SetXSize(XReal x);

  /// \brief  sets the size of the base lattice in Y
  /// \param  y - the size in the y axis
  void SetYSize(XReal y);

  /// \brief  sets the size of the base lattice in Z
  /// \param  z - the size in the z axis
  void SetZSize(XReal z);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XReal mXSize;
  XReal mYSize;
  XReal mZSize;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::BaseLattice,XBaseLattice,"baseLattice","BASE_LATTICE");
#endif
}
