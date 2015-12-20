//----------------------------------------------------------------------------------------------------------------------
/// \file   FFD.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the interface for a Free Form Deformation type. 
///         The node, XFFD, will always have a base lattice, XBaseLattice, (defining the 
///         original undeformed space to deform), and a Deformed lattice, XLattice which
///         defines the new deformed area that any geometry should be transformed into.  
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
// forward declarations for the base and deformed lattices 
class XMD_EXPORT XLattice;
class XMD_EXPORT XBaseLattice;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief  defines the interface for a Free Form surface Deformation,
///         a.k.a. Lattice deformation.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFFD 
  : public XDeformer
{
  XMD_CHUNK(XFFD);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kBaseLattice   = XDeformer::kLast+1; // XBase*
  static const XU32 kDeformLattice = XDeformer::kLast+2; // XBase*
  static const XU32 kLast          = kDeformLattice;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   FFD Functions
  /// \brief  Allows you to set or get the deformed and base lattices
  //@{

  /// \brief  returns a pointer to the base lattice used by this FFD
  /// \return the base lattice
  XBaseLattice* GetBaseLattice() const;

  /// \brief  returns a pointer to the deformed lattice used by this FFD
  /// \return the deformed lattice
  XLattice* GetDeformedLattice() const;

  /// \brief  sets the base lattice to use for this FFD
  /// \return the base lattice
  bool SetBaseLattice(const XBase*);

  /// \brief  sets the deformed lattice to use for this FFD
  /// \return the deformed lattice
  bool SetDeformedLattice(const XBase*);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING
  /// the base lattice for the free-form deformation
  XId m_BaseLattice;
  /// the deformed version of the lattice
  XId m_DeformedLattice;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::FFd, XFFD, "ffd", "FFD");
#endif
}
