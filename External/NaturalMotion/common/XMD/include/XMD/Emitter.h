//----------------------------------------------------------------------------------------------------------------------
/// \file   Emitter.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the loading / saving routines for a particle emitter
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Bone.h"
#include "XM2/Colour.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XParticleShape;
#endif
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XEmitter
/// \brief A particle emitter  
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XEmitter :
  public XBone
{
  friend class XParticleShape;
  XMD_CHUNK(XEmitter);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kRate             = XBone::kLast+1; // XReal
  static const XU32 kMaxDistance      = XBone::kLast+2; // XReal
  static const XU32 kMinDistance      = XBone::kLast+3; // XReal
  static const XU32 kDirection        = XBone::kLast+4; // XVector3
  static const XU32 kSpread           = XBone::kLast+5; // XReal
  static const XU32 kColour           = XBone::kLast+6; // XColour
  static const XU32 kEmitterType      = XBone::kLast+7; // XU32
  static const XU32 kEmitterObject    = XBone::kLast+8; // XBase*
  static const XU32 kEmitterTransform = XBone::kLast+9; // XBase*
  static const XU32 kVolumeType       = XBone::kLast+10;// XU32
  static const XU32 kVolumeOffset     = XBone::kLast+11;// XVector3
  static const XU32 kVolumeSweep      = XBone::kLast+12;// XReal
  static const XU32 kSectionRadius    = XBone::kLast+13;// XReal
  static const XU32 kSpeed            = XBone::kLast+14;// XReal
  static const XU32 kSpeedRandom      = XBone::kLast+15;// XReal
  static const XU32 kSize             = XBone::kLast+16;// XReal
  static const XU32 kSizeRandom       = XBone::kLast+17;// XReal
  static const XU32 kMass             = XBone::kLast+18;// XReal
  static const XU32 kMassRandom       = XBone::kLast+19;// XReal
  static const XU32 kAlpha            = XBone::kLast+20;// XReal
  static const XU32 kAlphaRandom      = XBone::kLast+21;// XReal
  static const XU32 kHueOffset        = XBone::kLast+22;// XReal
  static const XU32 kSaturationOffset = XBone::kLast+23;// XReal
  static const XU32 kLuminanceOffset  = XBone::kLast+24;// XReal
  static const XU32 kLast             = kColour;
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  /// \name  Output Particle Shape
  /// \brief Sets/Gets the output particle shape into which the emitter
  ///        will spawn new particles.
  //@{

  /// \brief  returns a pointer to the shape object that this emitter
  ///         outputs particles into
  /// \return a pointer to the geometry object
  XParticleShape* GetShape() const;

  /// \brief  sets the shape object that this emitter  outputs particles into
  /// \return a pointer to the geometry object
  bool SetShape(const XGeometry*);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name  Particle Emitter Params
  /// \brief Sets/Gets the params that control the emission of particles
  //@{

  /// \brief  returns the emitter rate
  /// \return the particle emitter rate
  /// 
  XReal GetRate() const;

  /// \brief  returns the maximum emitter distance
  /// \return the max distance from the emitter where a particle can be spawned.
  XReal GetMaxDistance() const;

  /// \brief  returns the minimum emitter distance
  /// \return the min distance from the emitter where a particle can be spawned.
  XReal GetMinDistance() const;

  /// \brief  returns the direction of particle emission
  /// \return the emitter direction
  const XVector3& GetDirection() const;

  /// \brief  returns the spread angle away from the emitter
  /// \return the emission spread
  XReal GetSpread() const;

  /// \brief  sets the emission rate of the particle emitter.
  /// \param  value - the new particle rate (per sec)
  void SetRate(XReal value);

  /// \brief  sets the maximum distance away from the emitter position
  ///         that a particle can be spawned at
  /// \param  value - the maximum emission distance 
  void SetMaxDistance(XReal value);

  /// \brief  sets the minimum distance from the emitter at which a
  ///         particle can be emitted
  /// \param  value - the minimum distance to the particle
  void SetMinDistance(XReal value);

  /// \brief  sets the direction of the emission
  /// \param  value - the new direction vector
  void SetDirection(const XVector3& value);

  /// \brief  sets the spread angle that defines an emission cone down
  ///         which the particles get emitted
  /// \param  value - the new spread angle value
  void SetSpread(XReal value);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Emitted Particle Colour
  /// \brief  the emitted particle colours are determined using an 
  ///         RGBA base colour, and a random variation of colour determined
  ///         using Hue, Saturation and Luminance values. 
  //@{

  /// \brief  returns the colour applied to the created particles
  /// \return  the particle colour
  /// 
  const XColour& GetColour() const;

  /// \brief  sets the colour of the emitted particles
  /// \param  value - the new particle colour value
  void SetColour(const XColour& value);

  /// \brief  sets the hue saturation and luminance colour offset values. 
  /// \param  hue - the random variance in the particles hue
  /// \param  saturation - the random variation of the particles saturation
  /// \param  luminance - the random variation of the particles luminance
  void SetColourOffset(XReal hue, XReal saturation, XReal luminance);
  
  /// \brief  sets the hue amount of offset in colour variation of emitted
  ///         particles.
  /// \param  hue - the random variance in the particles hue
  void SetColourOffsetHue(XReal hue);

  /// \brief  sets the saturation amount of offset in colour variation of emitted
  ///         particles.
  /// \param  saturation - the random variation of the particles saturation
  void SetColourOffsetSaturation(XReal saturation);
  
  /// \brief  sets the luminance amount of offset in colour variation of emitted
  ///         particles.
  /// \param  luminance - the random variation of the particles luminance
  void SetColourOffsetLuminance(XReal luminance);

  /// \brief  sets the hue amount of offset in colour variation of emitted
  ///         particles.
  /// \param  hue - the random variance in the particles hue
  XReal GetColourOffsetHue() const;

  /// \brief  sets the saturation amount of offset in colour variation of emitted
  ///         particles.
  /// \param  saturation - the random variation of the particles saturation
  XReal GetColourOffsetSaturation() const;

  /// \brief  sets the luminance amount of offset in colour variation of emitted
  ///         particles.
  /// \param  luminance - the random variation of the particles luminance
  XReal GetColourOffsetLuminance() const;

  /// \brief  returns the min alpha value of the emitted particles. 
  /// \return the min alpha of spawned particles.
  XReal GetAlpha() const;

  /// \brief  returns a random offset amount to the alpha value of the 
  ///         emitted particles. 
  /// \return the alpha variance 
  XReal GetAlphaRandom() const;

  /// \brief  sets the min alpha amount of emitted particles
  /// \param  c - the new min alpha value
  void SetAlpha(XReal c);

  /// \brief  sets the random offset alpha amount that is added to the
  ///         emitted particles. i.e. 
  ///   
  ///         alphaValue = GetAlpha() + GetAlphaRandom()*random(0,1)
  /// 
  /// \param  c - the new alpha offset value
  void SetAlphaRandom(XReal c);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Particle Emitter Type
  //@{

  /// \brief  the type of particle emitter
  enum XEmitterType 
  {
    kOmni,        ///< emits in all directions around a sphere
    kDirectional, ///< emits along a vector (x axis)
    kGeometry,    ///< emits a particle from a surface or curve
    kVolume       ///< emits within a 3D Box volume
  };

  /// \brief  returns the emitter type
  /// \return the emitter type enum
  XEmitterType GetEmitterType() const;

  /// \brief  sets the type of the emitter
  /// \param  et - the emitter type
  void SetEmitterType(XEmitterType et);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Volume Emitter Descriptions
  /// \brief  these functions are used to set a volume primitive shape from 
  ///         which the emitter will spawn particles. 
  //@{

  /// \brief  the type of volume from which the particles are emitted.
  ///         This is only relevant if the particle emitter type is 
  ///         Emitter::kVolume
  enum XEmitterVolumeType
  {
    kCube,      ///< emit from a unit length cube
    kSphere,    ///< emit from a swept sphere 
    kCylinder,  ///< emit from a swept cylinder
    kCone,      ///< emit from a swept cone
    kTorus,     ///< emit from a swept torus
    kDisc,      ///< emit from a circular disc
    kGrid       ///< emit from a grid shape (radius == width, inner radius == height)
  };

  /// \brief  returns the emitter type
  /// \return the emitter type enum
  XEmitterVolumeType GetVolumeType() const;

  /// \brief  sets the type of the emitter
  /// \param  et - the emitter type
  void SetVolumeType(XEmitterVolumeType et);

  /// \brief  returns the radius of the sphere, torus, cylinder and cone volumes
  /// \return the volume radius 
  XReal GetVolumeRadius() const;

  /// \brief  sets the radius of the volume (cone,cylinder,torus,sphere)
  /// \param  r - the new radius
  void SetVolumeRadius(XReal r);

  /// \brief  returns the radius of the inner cross section circle of the 
  ///         torus volume emitter type
  /// \return the inner radius of the torus emitter volume
  XReal GetInnerRadius() const;

  /// \brief  sets the inner radius of the torus volume
  /// \param  r - the new inner radius value
  void SetInnerRadius(XReal r);

  /// \brief  returns the width of the grid primitive
  /// \return the grids width
  XReal GetWidth() const;

  /// \brief  sets the width of the grid primitive
  /// \param  r - the new grid width
  void SetWidth(XReal r);

  /// \brief  returns the height of the grid primitive
  /// \return the grids height
  XReal GetHeight() const;

  /// \brief  sets the height of the grid primitive
  /// \param  r - the new grid height
  void SetHeight(XReal r);

  /// \brief  returns the sweep angle of the volume primitive (in radians)
  /// \return the volume sweep angle (in radians)
  XReal GetVolumeSweep() const;

  /// \brief  sets the volume sweep of the volume emitter
  /// \param  angle - the sweep angle in radians
  void SetVolumeSweep(XReal angle);

  /// \brief  returns the offset of the emitter
  /// \return the emitter offset
  const XVector3& GetOffset() const;

  /// \brief  returns the offset of the emitter
  /// \return the emitter offset
  void SetOffset(const XVector3& offset);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Emitted Particle Size
  /// \brief  emittedParticleSize = getSize() + getSizeRandom()*RAND(0,1)
  //@{

  /// \brief  returns the emitter type
  /// \return the emitter type enum
  XReal GetSize() const;

  /// \brief  sets the max colour of the spawned particles
  /// \param  c - the new particle emitter rate
  void SetSize(XReal c);

  /// \brief  returns the emitter type
  /// \return the emitter type enum
  XReal GetSizeRandom() const;

  /// \brief  sets the max colour of the spawned particles
  /// \param  c - the new particle emitter rate
  void SetSizeRandom(XReal c);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Emitted Particle Mass
  /// \brief  emittedParticleMass = GetMass() + GetMassRandom()*random(0,1)
  //@{

  /// \brief  returns the emitter type
  /// \return the emitter type enum
  XReal GetMass() const;

  /// \brief  sets the max colour of the spawned particles
  /// \param  c - the new particle emitter rate
  void SetMass(XReal c);

  /// \brief  returns the emitter type
  /// \return the emitter type enum
  XReal GetMassRandom() const;

  /// \brief  sets the max colour of the spawned particles
  /// \param  c - the new particle emitter rate
  void SetMassRandom(XReal c);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Emitted Particle Speed
  /// \brief  emittedParticleSpeed = GetSpeed() + GetSpeedRandom()*random(0,1)
  //@{

  /// \brief  returns the emission speed of particles
  /// \return the speed of particles emitted
  XReal GetSpeed() const;

  /// \brief  sets the speed of emitted particles
  /// \param  c - the new particle spread
  void SetSpeed(XReal c);

  /// \brief  returns the random variance in emission speed of particles
  /// \return the random variance in speed of particles emitted
  XReal GetSpeedRandom() const;

  /// \brief  sets the amount of random variation of speed of emitted particles
  /// \param  c - the amount of random variation in the speed
  void SetSpeedRandom(XReal c);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Geometry Emitter Information
  /// \brief  these functions are used to set and get an XGeometry shape
  ///         that is used to emit particles from. 
  //@{

  /// \brief  sets the geometry object to use as the shape to emit particles from. 
  /// \param  geometry - the geometry object
  /// \param  parent - the parent object
  bool SetEmitterGeometry(XGeometry* geometry, XBone* parent);

  /// \brief  sets a geometry object instance to use as the shape to emit 
  ///         particles from. this will fail if the instance does not 
  ///         reference an XGeometry shape
  /// \param  geometry - the geometry instance
  /// \return true if OK, false is the geometry instance is invalid. 
  bool SetEmitterGeometry(XInstance* geometry);

  /// \brief  returns the geometry object from which particles are 
  ///         emitted from, or NULL. 
  /// \return the geometry object from which particles are emitted
  XGeometry* GetEmitterGeometry() const;

  /// \brief  returns the transform node that positions the geometry 
  ///         object in space. 
  /// \return the geometries transform
  XBone* GetEmitterTransform() const;

  /// \brief  returns the XInstance of the geometry object that is used
  ///         to emit particles from. 
  /// \return the geometry object used to emit particles 
  XInstance* GetEmitterInstance() const;

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the ID number of the particle shape it attaches to.
  XId m_Shape;
  /// The rate of emission
  XReal m_Rate;
  /// The max distance that particles can be emitted from the emitter
  XReal m_MaxDistance;
  /// The max distance that particles can be emitted from the emitter
  XReal m_MinDistance;
  /// The direction of emission
  XVector3 m_Direction;
  /// The spread of particles
  XReal m_Spread;
  /// The particle colour when emitted.
  XColour m_Colour;
  /// the min alpha amount for the particles
  XReal m_Alpha;
  /// the random variation applied to the alpha of emitted particles
  XReal m_AlphaRandom;
  /// the hue colour offset for particles
  XReal m_Hue;
  /// the saturation colour offset of emitted particles
  XReal m_Saturation;
  /// the luminance colour offset of emitted particles
  XReal m_Luminance;
  /// the ID of an XGeometry object to spawn particles from
  XId m_EmitterObject;
  /// the ID of a transform that is the parent of the emitter object
  XId m_EmitterTransform;
  /// mass of emitted particles
  XReal m_Mass;
  /// amount of random variation in the particle mass
  XReal m_MassRandom;
  /// size of emitted particles
  XReal m_Size;
  /// amount of random variation in the particle size
  XReal m_SizeRandom;
  /// speed of emitted particles
  XReal m_Speed;
  /// amount of random variation in the particle speed
  XReal m_SpeedRandom;
  /// the emitter type
  XEmitterType m_EmitterType;
  /// the type of volume emitter
  XEmitterVolumeType m_VolumeType;
  /// sweep amount of volume primitive
  XReal m_VolumeSweep;
  /// radius of the sphere/torus/cylinder or cone.
  union 
  {
    XReal m_Radius;
    XReal m_Width;
  };
  /// the radius of the torus cross section
  union 
  {
    XReal m_SectionRadius;
    XReal m_Height;
  };
  /// an offset vector for the particle emitter
  XVector3 m_Offset;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Emitter, XEmitter, "emitter", "EMITTER");
#endif
}
