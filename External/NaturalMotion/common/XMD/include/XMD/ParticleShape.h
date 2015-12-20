//----------------------------------------------------------------------------------------------------------------------
/// \file   ParticleShape.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a container for all particles within a particle system.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Geometry.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XEmitter;
class XMD_EXPORT XField;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \param XMD::XParticleShape
/// \brief Defines a shape that contains particles
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XParticleShape :
  public XGeometry
{
  friend class XEmitter;
  friend class XField;
  XMD_CHUNK(XParticleShape);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kEmitters       = XGeometry::kLast+1; // XList
  static const XU32 kFields         = XGeometry::kLast+2; // XList
  static const XU32 kDepthSort      = XGeometry::kLast+3; // bool
  static const XU32 kLifespanType   = XGeometry::kLast+4; // XU32
  static const XU32 kLifespan       = XGeometry::kLast+5; // XReal
  static const XU32 kLifespanRandom = XGeometry::kLast+6; // XReal
  static const XU32 kParticleType   = XGeometry::kLast+7; // XU32
  static const XU32 kMaterial       = XGeometry::kLast+8; // XU32
  static const XU32 kLast           = kParticleType;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Particle Emitter Information
  /// \brief  allows you to query the assigned particle emitters
  //@{

  /// \brief  returns the number of emitters that input into this particle shape node
  /// \return the number of emitters
  XU32 GetNumEmitters() const;

  /// \brief  returns a pointer to the requested emitter
  /// \param  idx - the index of the emitter to return
  /// \return a pointer to the emitter
  XEmitter* GetEmitter(XU32 idx) const;

  /// \brief  adds the specified emitter into the particle system
  /// \param  ptr  -  the emitter node
  /// \return true if OK
  bool AddEmitter(const XBase* ptr);

  /// \brief  removes the specified emitter into the particle system
  /// \param  ptr  -  the emitter node to remove
  /// \return true if removed, false if not currently a set emitter.
  bool RemoveEmitter(const XBase* ptr);

  /// \brief  returns a list of all the emitters in this shape
  /// \param  emitters  -  the returned list of emitters
  void GetEmitters(XList& emitters) const;

  /// \brief  sets the emitters that input into this particle shape
  /// \param  emitters - the emitters to input particles into this 
  ///         particle shape
  void SetEmitters(const XList& emitters);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Dynamics Field Information
  /// \brief  allows you to query the dynamics fields that influence
  ///         the particle simulation
  //@{

  /// \brief  returns the number of dynamics fields that affect this 
  ///         particle shape node
  /// \return the number of fields
  XU32 GetNumFields() const;

  /// \brief  returns a pointer to the requested field
  /// \param  idx - the index of the field to return
  /// \return a pointer to the field
  XField* GetField(XU32 idx) const;

  /// \brief  adds the field node into the particle system.
  /// \param  ptr  -  the field effect node
  /// \return true if OK
  bool AddField(const XBase* ptr);

  /// \brief  removes the specified field node from the particle system.
  /// \param  ptr  -  the dynamics field to remove from this shape
  /// \return true if OK, false if the field does not affect these 
  ///         particles
  bool RemoveField(const XBase* ptr);

  /// \brief  returns a list of all the dynamics fields that affect the 
  ///         particles in this shape.
  /// \param  fields  -  the returned list of emitters
  void GetFields(XList& fields) const;

  /// \brief  sets the dynamics fields that manipulate the particles.
  /// \param  fields - the fields to manipulate the particles in this shape
  void SetFields(const XList& fields);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Particle Rendering
  //@{

  /// the primitive used to render the particles
  enum XParticleType
  {
    kPoint,       ///< a single point per particle
    kPointSprites,///< a point sprite with texture applied
    kStreak,      ///< a single streak per particle
    kSphere,      ///< spheres
    kMultiPoint,  ///< multiple points
    kMultiStreak, ///< multiple streaks
    kSprites,     ///< bill-boarded sprites
    kNumeric      ///< for debugging purposes
  };

  /// \brief  returns true if the particles are to be depth sorted prior to rendering
  /// \return true if the particles are to be depth sorted
  bool GetDepthSort() const;

  /// \brief  enables or disables depth sorting
  /// \param  f - true to enable depth sorting
  void SetDepthSort(bool f);

  /// \brief  returns the render type of the particles in this system. 
  /// \return the render type
  XParticleType GetParticleType() const;

  /// \brief  sets the particle render type within this particle system
  /// \param  f - the particle render type
  void SetParticleType(XParticleType f);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name Particle life time
  //@{

  /// \brief  defines how long the particles will remain in the system
  enum XLifespanType
  {
    kLiveForever, ///< lifespan is infinite
    kConstant,    ///< lifespan is GetLifetime()
    kRandom       ///< lifespan is GetLifetime() + random(0,1)*GetLifetimeRandom()
  };

  /// \brief  returns the lifespan of the particles in this system. This 
  ///         is only uses if the lifespan type is kConstant or kRandom. 
  /// \return the amount of time in seconds the particles will exist for.
  XReal GetLifespan() const;

  /// \brief  sets the lifespan for the particles in this particle system
  /// \param  f - the lifespan amount of the newly emitted particles
  void SetLifespan(XReal f);

  /// \brief  returns the random variation in lifespan amount of the 
  ///         particles in this system. This is only use if the lifespan 
  ///         type is kRandom. 
  /// \return the amount of time in seconds of random variation to 
  ///         apply to the particles
  XReal GetLifespanRandom() const;

  /// \brief  sets the random variation in lifespan for the particles in this particle system
  /// \param  f - the random lifespan variation of the newly emitted particles
  void SetLifespanRandom(XReal f);

  /// \brief  returns the lifespan type of the particles in this system. 
  /// \return the lifespan type
  XLifespanType GetLifespanType() const;

  /// \brief  sets the lifespan mode of the particle system
  /// \param  f - the lifespan mode
  void SetLifespanType(XLifespanType f);

  //@}

  /// \name Particle Material
  //@{

  //-------------------------------------------------------------------
  /// \brief  returns the currently assigned material
  /// \return the assigned material
  ///
  XMaterial* GetMaterial();

  //-------------------------------------------------------------------
  /// \brief  sets the assigned material
  /// \param  material - the new material to assign to the particles 
  ///         in this shape
  /// 
  void SetMaterial(XMaterial* material);

  //@}
private:
  void PostLoad();

  #ifndef DOXYGEN_PROCESSING
  /// true if depth sorting of particles is enabled
  bool mDepthSort;
  /// the lifetime of the particles generated
  XReal mLifespan;
  /// the amount of variation in the lifetime
  XReal mLifespanRandom;
  /// the type of lifespan of each particle
  XLifespanType mLifespanType;
  /// the type of rendering used for the particles
  XParticleType mParticleType;
  /// a list of the emitter's used in this particle system
  XIndexList m_Emitters;
  /// a list of ID's for the various field nodes applied to the particle
  XIndexList m_Fields;
  /// the material applied to the particles
  XId m_Material;
  #endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::ParticleShape, XParticleShape, "particleShape", "PARTICLE");
#endif
}
