//----------------------------------------------------------------------------------------------------------------------
/// \file   Image.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the XImage class for storing image data
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Base.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XImage
/// \brief This class is designed to hold an image loaded from the binary
///        *.tex file. It simple contains a 2D image, which you can 
///        access (and to some extents, edit, though it's not recommended....)
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XImage
{
#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XModel;
protected:

  /// the ctor
  XImage();

  /// the dtor
  ~XImage();

public:

  /// \brief  reads the image data from the texture pack
  /// \param  fp  - the file pointer
  /// \return true if OK
  bool Read(FILE* fp);

  /// \brief  write the image data to the texture pack
  /// \param  fp  - the file pointer
  /// \return true if OK
  bool Write(FILE* fp);

#endif

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Image Data Information
  /// \brief  Sets/Gets the image data
  //@{

  /// \brief  This function sets the file size of the image.
  /// \param  w - the width of the image
  /// \param  h - the height of the image
  /// \param  d - the depth of the image
  /// \return true if OK
  bool SetSize(XU32 w, XU32 h, XU32 d = 3);

  /// \brief  This function returns the filename of the original texture file
  /// \return the texture file name
  const XChar* GetFileName() const;

  /// \brief  This function returns the name of the maya file texture node
  /// \return the texture file node name
  const XChar* GetMayaName() const;

  /// \brief  This function returns a pointer to the pixel data array
  /// \return the pixel data array
  XU8* Pixels() const;

  /// \brief  returns the width of the image
  /// \return the width
  XU32 Width() const;

  /// \brief  returns the height of the image
  /// \return the height
  XU32 Height() const;

  /// \brief  returns the byte depth of the pixels, ie.. 1,2,3,4
  /// \return the depth of the pixels in bytes
  XU8 Depth() const;

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the name of the file texture node in maya
  XString m_MayaName;
  /// the original path to the texture (for later debugging of pipelines)
  XString m_FilePath;
  /// the ID of the texture (corresponds to the texture placement ID)
  XU32 m_ID;
  /// the width of the texture
  XU32 m_Width;
  /// the height of the texture
  XU32 m_Height;
  /// the height of the texture
  XU32 m_Depth;
  /// the array of pixels
  XU8* m_Pixels;
#endif
};
/// \brief  An array of images
typedef XM2::pod_vector<XImage*> XImageList;
}
