// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_COLOUR_H
#define NM_COLOUR_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Colour
/// \brief A %Colour class, offering the usual set of functionality.
/// \ingroup NaturalMotionPlatform
///
/// RGBA values between 0 and 255.
//----------------------------------------------------------------------------------------------------------------------
class Colour
{
public:

  static const Colour BLACK;
  static const Colour LIGHT_GREY;
  static const Colour DARK_GREY;
  static const Colour WHITE;
  static const Colour DARK_RED;
  static const Colour RED;
  static const Colour LIGHT_RED;
  static const Colour DARK_GREEN;
  static const Colour GREEN;
  static const Colour LIGHT_GREEN;
  static const Colour DARK_BLUE;
  static const Colour BLUE;
  static const Colour LIGHT_BLUE;
  static const Colour DARK_YELLOW;
  static const Colour YELLOW;
  static const Colour LIGHT_YELLOW;
  static const Colour DARK_PURPLE;
  static const Colour PURPLE;
  static const Colour LIGHT_PURPLE;
  static const Colour DARK_TURQUOISE;
  static const Colour TURQUOISE;
  static const Colour LIGHT_TURQUOISE;
  static const Colour DARK_ORANGE;
  static const Colour ORANGE;
  static const Colour LIGHT_ORANGE;

  NM_INLINE Colour();
  NM_INLINE Colour(const Colour& copy);
  NM_INLINE Colour(const Colour& copy, uint8_t inA); // RGB come from copy, and A from inA.
  NM_INLINE Colour(uint8_t inR, uint8_t inG, uint8_t inB);
  NM_INLINE Colour(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA);
  NM_INLINE explicit Colour(const Vector3& c); // Must be values between 0.0 and 1.0.
  NM_INLINE explicit Colour(const Vector3& c, uint8_t inA); // RGB come from c, and A from inA.

  NM_INLINE const Colour& operator = (const Colour& copy);

  NM_INLINE bool  operator == (const Colour& c) const;
  NM_INLINE bool  operator != (const Colour& c) const;

  /// \brief Copy the local values to the vector specified
  NM_INLINE void copyTo(Colour& c) const;

  /// \brief Get the red component of the colour
  NM_INLINE uint8_t getR() const {return m_col[0];}

  /// \brief Get the green component of the colour
  NM_INLINE uint8_t getG() const {return m_col[1];}

  /// \brief Get the blue component of the colour
  NM_INLINE uint8_t getB() const {return m_col[2];}

  /// \brief Get the alpha component of the colour
  NM_INLINE uint8_t getA() const {return m_col[3];}

  /// \brief Set the red component of the colour
  NM_INLINE void setR(uint8_t v) { m_col[0] = v; }

  /// \brief Set the green component of the colour
  NM_INLINE void setG(uint8_t v) { m_col[1] = v; }

  /// \brief Set the blue component of the colour
  NM_INLINE void setB(uint8_t v) { m_col[2] = v; }

  /// \brief Set the alpha component of the colour
  NM_INLINE void setA(uint8_t v) { m_col[3] = v; }

  /// \brief Get the normalised (0.0 - 1.0) red component of the colour.
  NM_INLINE float getRf() const {return (((float)m_col[0]) * (1.0f / 255.0f));}

  /// \brief Get the normalised (0.0 - 1.0) green component of the colour.
  NM_INLINE float getGf() const {return (((float)m_col[1]) * (1.0f / 255.0f));}

  /// \brief Get the normalised (0.0 - 1.0) blue component of the colour.
  NM_INLINE float getBf() const {return (((float)m_col[2]) * (1.0f / 255.0f));}

  /// \brief Get the normalised (0.0 - 1.0) alpha component of the colour.
  NM_INLINE float getAf() const {return (((float)m_col[3]) * (1.0f / 255.0f));}

  /// \brief Set the red component of the colour, clamps the float to the range 0 - 1.
  NM_INLINE void setRf(float v);

  /// \brief Set the green component of the colour, clamps the float to the range 0 - 1.
  NM_INLINE void setGf(float v);

  /// \brief Set the blue component of the colour, clamps the float to the range 0 - 1.
  NM_INLINE void setBf(float v);

  /// \brief Set the alpha component of the colour, clamps the float to the range 0 - 1.
  NM_INLINE void setAf(float v);

  /// \brief Set all components of this colour to the given value including the alpha component
  NM_INLINE void set(uint8_t all);

  /// \brief Set the components of this vector in one go, alpha will be set to 255
  NM_INLINE void set(uint8_t inR, uint8_t inG, uint8_t inB);

  /// \brief Set the components of this vector in one go
  NM_INLINE void set(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA);

  /// \brief Copy the input vector into the local vector, including the 'w' component from 'v'
  NM_INLINE void set(const Colour& c);

  /// \brief Set all components of this colour to the given value including the alpha component
  NM_INLINE void setf(float all);

  /// \brief Set the components of this vector in one go, alpha will be set to 255
  NM_INLINE void setf(float inR, float inG, float inB);

  /// \brief Set the components of this vector in one go
  NM_INLINE void setf(float inR, float inG, float inB, float inA);

  /// \brief Set the red green blue components from hue saturation value components all within
  /// the range 0.0f - 1.0f. Alpha component remains the same.
  NM_INLINE void setFromHSV(float h, float s, float v);

protected:
  uint8_t m_col[4];

};

//----------------------------------------------------------------------------------------------------------------------
// Colour functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Colour::Colour()
{
  // doing stuff in default ctors can provoke hidden performance hits
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Colour::Colour(const Colour& copy)
{
  set(copy);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Colour::Colour(const Colour& copy, uint8_t inA)
{
  set(copy);
  setA(inA);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Colour::Colour(uint8_t inR, uint8_t inG, uint8_t inB)
{
  set(inR, inG, inB);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Colour::Colour(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA)
{
  set(inR, inG, inB, inA);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Colour::Colour(const Vector3& c)
{
  m_col[0] = (int8_t)(c.x * 255.0f);
  m_col[1] = (int8_t)(c.y * 255.0f);
  m_col[2] = (int8_t)(c.z * 255.0f);
  m_col[3] = 255;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Colour::Colour(const Vector3& c, uint8_t inA)
{
  m_col[0] = (int8_t)(c.x * 255.0f);
  m_col[1] = (int8_t)(c.y * 255.0f);
  m_col[2] = (int8_t)(c.z * 255.0f);
  m_col[3] = inA;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const Colour& Colour::operator = (const Colour& copy)
{
  set(copy);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Colour::operator == (const Colour& c) const
{
  return *((uint32_t*)m_col) == *((uint32_t*)c.m_col);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Colour::operator != (const Colour& c) const
{
  return *((uint32_t*)m_col) != *((uint32_t*)c.m_col);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::copyTo(Colour& c) const
{
  *((uint32_t*)c.m_col) = *((uint32_t*)m_col);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::setRf(float v)
{
  m_col[0] = (int8_t)clampValue(v * 255.0f, 0.0f, 255.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::setGf(float v)
{
  m_col[1] = (int8_t)clampValue(v * 255.0f, 0.0f, 255.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::setBf(float v)
{
  m_col[2] = (int8_t)clampValue(v * 255.0f, 0.0f, 255.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::setAf(float v)
{
  m_col[3] = (int8_t)clampValue(v * 255.0f, 0.0f, 255.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::set(uint8_t all)
{
  m_col[0] = m_col[1] = m_col[2] = m_col[3] = all;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::set(uint8_t inR, uint8_t inG, uint8_t inB)
{
  this->set(inR, inG, inB, 255);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::set(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA)
{
  setR(inR);
  setG(inG);
  setB(inB);
  setA(inA);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::set(const Colour& c)
{
  *((uint32_t*)m_col) = *((uint32_t*)c.m_col);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::setf(float all)
{
  m_col[0] = m_col[1] = m_col[2] = m_col[3] = (int8_t)clampValue(all * 255.0f, 0.0f, 255.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::setf(float inR, float inG, float inB)
{
  setRf(inR);
  setGf(inG);
  setBf(inB);
  setA(255);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::setf(float inR, float inG, float inB, float inA)
{
  setRf(inR);
  setGf(inG);
  setBf(inB);
  setAf(inA);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Colour::setFromHSV(float h, float s, float v)
{
  NMP_ASSERT(h >= 0.0f && h <= 1.0f);
  NMP_ASSERT(s >= 0.0f && s <= 1.0f);
  NMP_ASSERT(v >= 0.0f && v <= 1.0f);

  // if the colour has no saturation
  if (s < 1e-3f)
  {
    uint8_t ns = static_cast<uint8_t>(v * 255.0f);
    m_col[0] = ns;
    m_col[1] = ns;
    m_col[2] = ns;
  }
  else
  {
    h *= 6.0f; // re-parametrise h to the range 0.0f - 6.0f
    int i = (int)h;
    float f = h - i;

    float p = v * (1.0f - s);
    float q = v * (1.0f - (s * f));
    float t = v * (1.0f - (s * (1.0f - f)));

    switch (i)
    {
    case 0:
      m_col[0] = static_cast<uint8_t>(v * 255.0f);
      m_col[1] = static_cast<uint8_t>(t * 255.0f);
      m_col[2] = static_cast<uint8_t>(p * 255.0f);
      break;
    case 1:
      m_col[0] = static_cast<uint8_t>(q * 255.0f);
      m_col[1] = static_cast<uint8_t>(v * 255.0f);
      m_col[2] = static_cast<uint8_t>(p * 255.0f);
      break;
    case 2:
      m_col[0] = static_cast<uint8_t>(p * 255.0f);
      m_col[1] = static_cast<uint8_t>(v * 255.0f);
      m_col[2] = static_cast<uint8_t>(t * 255.0f);
      break;
    case 3:
      m_col[0] = static_cast<uint8_t>(p * 255.0f);
      m_col[1] = static_cast<uint8_t>(q * 255.0f);
      m_col[2] = static_cast<uint8_t>(v * 255.0f);
      break;
    case 4:
      m_col[0] = static_cast<uint8_t>(t * 255.0f);
      m_col[1] = static_cast<uint8_t>(p * 255.0f);
      m_col[2] = static_cast<uint8_t>(v * 255.0f);
      break;
    case 5:
      m_col[0] = static_cast<uint8_t>(v * 255.0f);
      m_col[1] = static_cast<uint8_t>(p * 255.0f);
      m_col[2] = static_cast<uint8_t>(q * 255.0f);
      break;
    default:
      // h was out of range so set to colour to black.
      m_col[0] = 0;
      m_col[1] = 0;
      m_col[2] = 0;
      break;
    }
  }
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_COLOUR_H
//----------------------------------------------------------------------------------------------------------------------
