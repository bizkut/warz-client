//----------------------------------------------------------------------------------------------------------------------
/// \file   TexPlacement2D.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  All textures can be rotated, translated and scaled. This transformation
///         that can be applied to an object is handled by its texture placement.
///         This texture placement can be mimicked via the texture stack in
///         openGL or DirectX.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/ShadingNode.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XImage;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XTexPlacement2D
/// \brief a 2D texture placement 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XTexPlacement2D
  : public XShadingNode
{
  XMD_CHUNK(XTexPlacement2D);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kTranslateFrameU = XBase::kLast+1; // XReal
  static const XU32 kTranslateFrameV = XBase::kLast+2; // XReal
  static const XU32 kRotateFrame     = XBase::kLast+3; // XReal
  static const XU32 kRepeatU         = XBase::kLast+4; // XReal
  static const XU32 kRepeatV         = XBase::kLast+5; // XReal
  static const XU32 kOffsetU         = XBase::kLast+6; // XReal
  static const XU32 kOffsetV         = XBase::kLast+7; // XReal
  static const XU32 kRotateUV        = XBase::kLast+8; // XReal
  static const XU32 kNoiseU          = XBase::kLast+9; // XReal
  static const XU32 kNoiseV          = XBase::kLast+10;// XReal

  static const XU32 kFileName        = XBase::kLast+11;// XString
  static const XU32 kFilterType      = XBase::kLast+12;// XU32
  static const XU32 kFilterAmount    = XBase::kLast+13;// XReal
  static const XU32 kPreFilter       = XBase::kLast+14;// bool
  static const XU32 kPreFilterRadius = XBase::kLast+15;// XReal
  static const XU32 kIsMirroredU     = XBase::kLast+16;// bool
  static const XU32 kIsMirroredV     = XBase::kLast+17;// bool
  static const XU32 kIsStaggered     = XBase::kLast+18;// bool
  static const XU32 kIsWrappedU      = XBase::kLast+19;// bool
  static const XU32 kIsWrappedV      = XBase::kLast+20;// bool
  static const XU32 kLast            = kIsWrappedV;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Assigned Image Access
  /// \brief  Sets/Gets the Image used for this texture
  //@{

  /// \brief  returns the image data used by this texture placement node.
  /// \return a pointer to the image data
  XImage* GetImage() const;

  /// \brief  sets the image ID to be used with this texture placement 
  /// \param  image_id - the image index
  /// \return true if OK, false if an invalid image. 
  bool SetImage(XId image_id);

  /// \brief  returns the filename of the texture used
  /// \return the file texture name
  const XChar* GetFileTextureName() const;

  /// \brief  sets the filename of the texture used
  /// \param  the file texture name
  void SetFileTextureName(const XChar* filename);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Texture Filtering Access
  /// \brief  Sets/Gets the Image used for this texture
  //@{

  enum FilterType 
  {
    kNone=0,
    kBiLinear=1,
    kMipMap=2,
    kBox=3,
    kQuadratic=4,
    kQuartic=5,
    kGuassian=6
  };

  /// \brief  returns the type of filtering used
  /// \return the filter type
  FilterType GetFilterType() const;

  /// \brief  sets the filtering type
  /// \param  type - the type of filtering. 
  void SetFilterType(FilterType type);

  /// \brief  returns the filtering amount. 1.0 is default
  /// \return the filter amount
  XReal GetFilterAmount() const;

  /// \brief  sets the filtering amount. 1.0 is default
  /// \param  amount - the filter amount
  void SetFilterAmount(XReal amount);

  /// \brief  returns whether pre filtering is enabled
  /// \return true if pre-filtering is enabled
  bool GetPreFilter() const;

  /// \brief  sets whether pre filtering is enabled
  /// \param  value - true to enable pre-filtering
  void SetPreFilter(bool value);

  /// \brief  returns the pre-filter radius
  /// \return the pre-filter radius
  XReal GetPreFilterRadius() const;

  /// \brief  sets the pre-filter radius
  /// \param  radius - the pre-filter radius
  void SetPreFilterRadius(XReal radius);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Texturing Parameters
  /// \brief  Sets/Gets the parameters used to map this texture
  //@{

  /// \brief  retrieves the texture translation amount in u and v
  /// \param  u - the returned u translation
  /// \param  v - the returned v translation
  void GetTranslateFrame(XReal& u, XReal& v) const;

  /// \brief  retrieves the texture translation amount in u 
  /// \return the translation in u amount
  XReal GetTranslateFrameU() const;

  /// \brief  retrieves the texture translation amount in v
  /// \return the translation in v amount
  XReal GetTranslateFrameV() const;

  /// \brief  returns the rotation of applied to the texture
  /// \return the rotation angle
  XReal GetRotateFrame() const;

  /// \brief  queries if the texture mirrored in U?
  /// \return true if the texture is mirrored in the U direction
  bool GetIsMirroredU() const;

  /// \brief  queries if the texture mirrored in V?
  /// \return true if the texture is mirrored in the V direction
  bool GetIsMirroredV() const;

  /// \brief  queries if the texture staggered?
  /// \return true if the texture is staggered
  bool GetIsStaggered() const;

  /// \brief  queries if the texture wrapped in U
  /// \return true if the texture is wrapped in the U direction
  bool GetIsWrappedU() const;

  /// \brief  queries if the texture wrapped in V
  /// \return true if the texture is wrapped in the V direction
  bool GetIsWrappedV() const;

  /// \brief  returns the texture scale amount in the U direction
  /// \return the texture scale amount in the U direction
  XReal GetRepeatU() const;

  /// \brief  returns the texture scale amount in the V direction
  /// \return the texture scale amount in the V direction
  XReal GetRepeatV() const;

  /// \brief  returns the texture offset amount in the V direction
  /// \return the texture offset amount in the V direction
  XReal GetOffsetU() const;

  /// \brief  returns the texture offset amount in the V direction
  /// \return the texture offset amount in the V direction
  XReal GetOffsetV() const;

  /// \brief  returns the UV rotation
  /// \return the UV rotation value
  XReal GetRotateUV() const;

  /// \brief  returns the noise in U texture direction
  /// \return the noise in the U direction 
  XReal GetNoiseU() const;

  /// \brief  returns the noise in V texture direction
  /// \return the noise in the V direction 
  XReal GetNoiseV() const;

  /// \brief  sets the texture scale in the U direction
  /// \param  value  - the new U texture scale
  /// \todo   comments
  void SetTranslateFrameU(XReal value);

  /// \brief  sets the texture scale in the V direction
  /// \param  value  - the new V texture scale
  /// \todo   comments
  void SetTranslateFrameV(XReal value);

  /// \brief  sets the texture offset in the U direction
  /// \param  value  - the new U texture offset
  /// \todo   comments
  void SetRotateFrame(XReal value);

  /// \brief  sets the texture scale in the U direction
  /// \param  value  - the new U texture scale
  void SetRepeatU(XReal value);

  /// \brief  sets the texture scale in the V direction
  /// \param  value  - the new V texture scale
  void SetRepeatV(XReal value);

  /// \brief  sets the texture offset in the U direction
  /// \param  value  - the new U texture offset
  void SetOffsetU(XReal value);

  /// \brief  sets the texture offset in the V direction
  /// \param  value  - the new V texture offset
  void SetOffsetV(XReal value);

  /// \brief  sets the UV rotation angle
  /// \param  value - the new rotation angle
  void SetRotateUV(XReal value);

  /// \brief  sets the noise in U direction
  /// \param  value  - the new U texture noise value
  void SetNoiseU(XReal value);

  /// \brief  sets the noise in V direction
  /// \param  value  - the new V texture noise value
  void SetNoiseV(XReal value);

  /// \brief  sets whether the texture mirrored in U
  /// \param  v - true if the texture is mirrored in the U direction
  void SetIsMirroredU(bool v);

  /// \brief  sets whether the texture mirrored in V
  /// \param  v - true if the texture is mirrored in the V direction
  void SetIsMirroredV(bool v);

  /// \brief  sets whether the texture staggered
  /// \param  v - true to make the texture staggered
  void SetIsStaggered(bool v);

  /// \brief  sets whether the texture wrapped in U
  /// \param  v - true if the texture is wrapped in the U direction
  void SetIsWrappedU(bool v);

  /// \brief  sets whether the texture wrapped in V
  /// \param  v - true if the texture is wrapped in the V direction
  void SetIsWrappedV(bool v);

  /// \brief  \b DEPRECATED queries if the texture mirrored in U?
  /// \return true if the texture is mirrored in the U direction
  /// \deprecated Use XTexPlacement2D::GetIsMirroredU() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XTexPlacement2D::GetIsMirroredU() instead")
  #endif
  inline bool IsMirroredU() const { return GetIsMirroredU(); }

  /// \brief  \b DEPRECATED queries if the texture mirrored in V?
  /// \return true if the texture is mirrored in the V direction
  /// \deprecated Use XTexPlacement2D::GetIsMirroredV() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XTexPlacement2D::GetIsMirroredV() instead")
  #endif
  inline bool IsMirroredV() const { return GetIsMirroredV(); }

  /// \brief  \b DEPRECATED queries if the texture staggered?
  /// \return true if the texture is staggered
  /// \deprecated Use XTexPlacement2D::GetIsStaggered() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XTexPlacement2D::GetIsStaggered() instead")
  #endif
  inline bool IsStaggered() const { return GetIsStaggered(); }

  /// \brief  \b DEPRECATED queries if the texture wrapped in U
  /// \return true if the texture is wrapped in the U direction
  /// \deprecated Use XTexPlacement2D::GetIsWrappedU() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XTexPlacement2D::GetIsWrappedU() instead")
  #endif
  inline bool IsWrappedU() const { return GetIsWrappedU(); }

  /// \brief  \b DEPRECATED queries if the texture wrapped in V
  /// \return true if the texture is wrapped in the V direction
  /// \deprecated Use XTexPlacement2D::GetIsWrappedV() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XTexPlacement2D::GetIsWrappedV() instead")
  #endif
  inline bool IsWrappedV() const { return GetIsWrappedV(); }

  /// \brief  \b DEPRECATED sets if the texture is wrapped in U direction
  /// \param  wrapped - true if wrapped in U direction
  /// \deprecated Use XTexPlacement2D::SetIsWrappedU() instead
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XTexPlacement2D::SetIsWrappedU() instead")
  #endif
  inline void SetWrapU(const bool wrapped) { SetIsWrappedU(wrapped); }

  /// \brief  \b DEPRECATED sets if the texture is wrapped in V direction
  /// \param  wrapped - true if wrapped in V direction
  /// \deprecated Use XTexPlacement2D::SetIsWrappedV() instead 
  #ifndef DOXYGEN_PROCESSING
  XMD_DEPRECATED("Use XTexPlacement2D::SetIsWrappedV() instead")
  #endif
  inline void SetWrapV(const bool wrapped) { SetIsWrappedV(wrapped); }

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the index of the image in the image array
  XId m_ImageIndex;
  XReal m_TranslateFrame[2];
  XReal m_RotateFrame;
  /// is the texture mirrored in U?
  bool m_MirrorU;
  /// is the texture mirrored in V?
  bool m_MirrorV;
  /// is the texture staggered?
  bool m_Stagger;
  /// is the texture wrapped in U
  bool m_WrapU;
  /// is the texture wrapped in V
  bool m_WrapV;
  /// essentially the scale in U&V for the texture
  XReal m_RepeatUV[2];
  /// the UV offset
  XReal m_OffsetUV[2];
  /// the UV rotation
  XReal m_RotateUV;
  /// the noise in U & V
  XReal m_NoiseUV[2];
  /// name of the texture file used
  XString m_FileTextureName;
  /// the type of filter
  FilterType m_FilterType;
  /// the amount of filtering
  XReal m_FilterAmount;
  /// true if pre filtering is enabled
  bool m_PreFilter;
  /// the pre filter radius
  XReal m_PreFilterRadius;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::TexturePlacement2D, XTexPlacement2D, "texturePlacement2D", "TEXTURE_2D");
#endif
}
