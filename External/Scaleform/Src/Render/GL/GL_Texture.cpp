/**************************************************************************

Filename    :   GL_Texture.cpp
Content     :   GL Texture and TextureManager implementation
Created     :   
Authors     :   

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "GL_Texture.h"
#include "Render/Render_TextureUtil.h"
#include "Kernel/SF_Debug.h"

#if defined(SF_USE_GLES)
#include "Render/GL/GLES11_ExtensionMacros.h"
#elif defined(GL_ES_VERSION_2_0)
#include "Render/GL/GLES_ExtensionMacros.h"
#else
#include "Render/GL/GL_ExtensionMacros.h"
#endif

namespace Scaleform { namespace Render { namespace GL {


extern TextureFormat GLTextureFormatMapping[];

Texture::Texture(TextureManagerLocks* pmanagerLocks, const TextureFormat* pformat,
                 unsigned mipLevels, const ImageSize& size, unsigned use,
                 ImageBase* pimage) :   
    Render::Texture(pmanagerLocks, size, (UByte)mipLevels, (UInt16)use, pimage), 
    pFormat(pformat), pMap(0)
{
    TextureCount = (UByte) pformat->GetPlaneCount();
    if (TextureCount > 1)
    {
        pTextures = (HWTextureDesc*)
            SF_HEAP_AUTO_ALLOC(this, sizeof(HWTextureDesc) * TextureCount);
    }
    else
    {
        pTextures = &Texture0;
    }
    memset(pTextures, 0, sizeof(HWTextureDesc) * TextureCount);
}

Texture::Texture(TextureManagerLocks* pmanagerLocks, GLuint texID, 
                 bool deleteTexture, const ImageSize& size, ImageBase* pimage) : 
    Render::Texture(pmanagerLocks, size, 0, 0, pimage), 
    pFormat(0), pMap(0)
{
    TextureFlags |= TF_UserAlloc;
    if ( !deleteTexture )
        TextureFlags |= TF_DoNotDelete;

    pTextures = &Texture0;
    pTextures[0].Size = size;
    pTextures[0].TexId = texID;
}

Texture::~Texture()
{
    //  pImage must be null, since ImageLost had to be called externally.
    SF_ASSERT(pImage == 0);

    Mutex::Locker  lock(&pManagerLocks->TextureMutex);

    if ((State == State_Valid) || (State == State_Lost))
    {
        // pManagerLocks->pManager should still be valid for these states.
        SF_ASSERT(pManagerLocks->pManager);
        RemoveNode();
        pNext = pPrev = 0;
        // If not on Render thread, add HW textures to queue.
        ReleaseHWTextures();
    }

    if ((pTextures != &Texture0) && pTextures)
        SF_FREE(pTextures);
}

// LoseManager is called from ~TextureManager, cleaning out most of the
// texture internal state. Clean-out needs to be done during a Lock,
// in case ~Texture is called due to release in another thread.
void    Texture::LoseManager()
{        
    Ptr<Texture> thisTexture = this;    // Guards against the texture being deleted inside Texture::LoseManager()
    SF_ASSERT(pMap == 0);
    Render::Texture::LoseManager();
    LastMinFilter = LastAddress = 0;
    pFormat = 0; // Users can't access Format any more ?
}

void Texture::LoseTextureData()
{
    Render::Texture::LoseTextureData();

    SF_ASSERT(pMap == 0);

    Lock::Locker lock(&pManagerLocks->ImageLock);
    for (unsigned itex = 0; itex < TextureCount; itex++)
        pTextures[itex].TexId = 0;
    State   = State_Lost;
    
    if (pImage)
    {
        // TextureLost may release 'this' Texture, so do it last.
        SF_ASSERT(pImage->GetImageType() != Image::Type_ImageBase);
        Image* pimage = (Image*)pImage;
        pimage->TextureLost(Image::TLR_DeviceLost);
    }
}


bool Texture::Initialize()
{    
    if ( TextureFlags & TF_UserAlloc )
    {
        return Initialize(pTextures[0].TexId);
    }

    bool            resize  = false;
    ImageFormat     format  = GetImageFormat();
    TextureManager* pmanager= GetManager();
    unsigned        itex;

    if (State != State_Lost)
    {
        // Determine sizes of textures.
        for (itex = 0; itex < TextureCount; itex++)
        {
            HWTextureDesc& tdesc = pTextures[itex];
            tdesc.Size = ImageData::GetFormatPlaneSize(format, ImgSize, itex);
            if (!pmanager->IsNonPow2Supported(format, Use))
            {
                ImageSize roundedSize = ImageSize_RoundUpPow2(tdesc.Size);
                if (roundedSize != tdesc.Size)
                {
                    tdesc.Size = roundedSize;
                    resize = true;
                }
            }        
        }

        // XXX padding to power of 2 not supported in core yet
        if (resize /*&& (Use & ImageUse_Wrap)*/)
        {        
            if (ImageData::IsFormatCompressed(format))
            {        
                SF_DEBUG_ERROR(1, 
                               "CreateTexture failed - Can't rescale compressed Wrappable image to Pow2");
                if (State != State_Lost)
                    State = State_InitFailed;
                return false;
            }
            TextureFlags |= TF_Rescale;
        }
    }

    // Determine how many mipLevels we should have and whether we can
    // auto-generate them or not.
    unsigned allocMipLevels = MipLevels;
    bool genMipmaps = 0, allocMapBuffer = 0;

    if (Use & ImageUse_MapSimThread ||
        Use & ImageUse_MapRenderThread )
        allocMapBuffer = 1;

    if (Use & ImageUse_GenMipmaps)
    {
        SF_ASSERT(MipLevels == 1);
        if (!(pmanager->GetTextureUseCaps(format) & ImageUse_GenMipmaps))
        {            
            TextureFlags |= TF_SWMipGen;
            // If using SW MipGen, determine how many mip-levels we should have.
            allocMipLevels = 31;
            for (itex = 0; itex < TextureCount; itex++)
                allocMipLevels = Alg::Min(allocMipLevels, ImageSize_MipLevelCount(pTextures[itex].Size));
            MipLevels = (UByte)allocMipLevels;
        }
        else
        {
            genMipmaps = 1;
            allocMipLevels = 1;
        }
    }

    if (allocMapBuffer)
    {
        pBackingImage = *RawImage::Create(format, MipLevels, pTextures[0].Size, 0);
    }

    LastMinFilter = LastAddress = 0;

    // Create textures
    for (itex = 0; itex < TextureCount; itex++)
    {
        HWTextureDesc& tdesc = pTextures[itex];

        glGenTextures(1, &tdesc.TexId);
        glBindTexture(GL_TEXTURE_2D, tdesc.TexId);

#if defined(GL_TEXTURE_MAX_LEVEL)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, MipLevels-1);
#elif defined(GL_APPLE_texture_max_level)
        if (MipLevels > 1 )
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL_APPLE, MipLevels-1);        
#endif

        if (!ImageData::IsFormatCompressed(format))
        {
            glTexImage2D(GL_TEXTURE_2D, 0, pFormat->GLColors, tdesc.Size.Width, tdesc.Size.Height, 0, pFormat->GLFormat, pFormat->GLData, 0);

            unsigned width = tdesc.Size.Width;
            unsigned height = tdesc.Size.Height;
            for (unsigned i = 1; i < allocMipLevels; i++)
            {
                width >>= 1;
                height >>= 1;
                if (width < 1)
                    width = 1;
                if (height < 1)
                    height = 1;
                glTexImage2D(GL_TEXTURE_2D, i, pFormat->GLColors, width, height, 0, pFormat->GLFormat, pFormat->GLData, 0);
            }
        }
    }

    if (genMipmaps)
    {
        glGenerateMipmapEXT(GL_TEXTURE_2D);
    }

    // Upload image content to texture, if any.
    if (pImage && !Update())
    {
        SF_DEBUG_ERROR(1, "CreateTexture failed - couldn't initialize texture contents");

        ReleaseHWTextures();
        State = State_InitFailed;
        return false;
    }

    State = State_Valid;
    return true;
}

bool Texture::Initialize(GLuint texID)
{
    pFormat = 0;
    TextureManager* pmanager = GetManager();

    if ( pTextures[0].TexId != texID )
    {
        ReleaseHWTextures();
    }
    pTextures = &Texture0;
    Texture0.TexId = texID;

    int format;
#if defined(SF_USE_GLES) || defined(GL_ES_VERSION_2_0)
    // There is no way to query texture information in GLES. 
    format = GL_RGBA;
    SF_ASSERT(ImgSize != ImageSize(0));
    Texture0.Size.SetSize(ImgSize);
#else
    int width, height, level0, levelN;
    glBindTexture(GL_TEXTURE_2D, texID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, &level0);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &levelN);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPONENTS, &format);

    Texture0.Size.SetSize(width, height);

    // Override the image size if it was not provided.
    if ( ImgSize == ImageSize(0) )
        ImgSize = pTextures[0].Size;

    MipLevels = Alg::Max<UByte>(1, (UByte)(levelN - level0));
#endif // GL_ES_VERSION_2_0


    // If an image is provided, try to obtain the texture format from the image.
    pFormat = 0;
    if ( pImage )
        pFormat = (TextureFormat*)pmanager->getTextureFormat(pImage->GetFormatNoConv());

    // Otherwise, figure out the texture format, based on the mapping table. Assume all levels have the same format.
    TextureFormat* pmapping;
    for (pmapping = GLTextureFormatMapping; pmapping->Format != Image_None; pmapping++)
    {
        if ( pmapping->GLFormat == TextureManager::getBaseTextureFormatFromInternal((GLenum)format) )
        {
            pFormat = (TextureFormat*)pmanager->getTextureFormat(pmapping->Format);
            break;
        }
    }
    if ( !pFormat )
    {
        State = State_InitFailed;
        return false;
    }

    State = State_Valid;
    return true;
}

bool Texture::Upload(unsigned itex, unsigned level, const ImagePlane& plane)
{
    glBindTexture(GL_TEXTURE_2D, pTextures[itex].TexId);

    if (ImageData::IsFormatCompressed(pFormat->Format))
    {
        // The plane's DataSize member contains the size of the buffer remaining, not the actual size of
        // the texture's mip level. Compute that, and pass it in.
        UPInt levelSize = ImageData::GetMipLevelSize(pFormat->ConvFormat, plane.GetSize(), itex);
        glCompressedTexImage2D(GL_TEXTURE_2D, level, pFormat->GLFormat, plane.Width, plane.Height, 0, (GLsizei)levelSize, plane.pData);
        
        // For certain formats (PVRTC), mip levels that are too small generate errors, and do not function correctly.
        // In this case, artibrarily reduce the miplevel count.
        GLenum mipError = glGetError();
        if (mipError != 0 && level > 0)
        {
            MipLevels = (UByte)level;
            SF_DEBUG_WARNING4(mipError != 0, "Uploading mip-level failed (level=%d, size=(%d x %d). Reducing number of mips to %d.", 
                              level, plane.Width, plane.Height, MipLevels);
#if defined(GL_APPLE_texture_max_level)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL_APPLE, MipLevels-1);
#endif            
            return true;
        }
        else if (mipError != 0)
            return false;
        return true;
    }

    if (plane.Width * pFormat->BytesPerPixel == plane.Pitch)
    {
        if (plane.Width == pTextures[itex].Size.Width && plane.Height == pTextures[itex].Size.Height)
            glTexImage2D(GL_TEXTURE_2D, level, pFormat->GLColors, plane.Width, plane.Height, 0, pFormat->GLFormat, pFormat->GLData, plane.pData);
        else
            glTexSubImage2D(GL_TEXTURE_2D, level, 0,0, plane.Width, plane.Height, pFormat->GLFormat, pFormat->GLData, plane.pData);
    }
#if (defined(GL_UNPACK_ROW_LENGTH) && defined(GL_UNPACK_ALIGNMENT))
    else if (plane.Pitch == ((3 + plane.Width * pFormat->BytesPerPixel) & ~3))
    {
        glPixelStorei(GL_UNPACK_ROW_LENGTH, plane.Width);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        if (plane.Width == pTextures[itex].Size.Width && plane.Height == pTextures[itex].Size.Height)
            glTexImage2D(GL_TEXTURE_2D, level, pFormat->GLColors, plane.Width, plane.Height, 0, pFormat->GLFormat, pFormat->GLData, plane.pData);
        else
            glTexSubImage2D(GL_TEXTURE_2D, level, 0,0, plane.Width, plane.Height, pFormat->GLFormat, pFormat->GLData, plane.pData);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
#endif
    else
    {
        if (!pBackingImage)
        {
            SF_DEBUG_WARNING(1, "GL::Texture - irregular size, cannot use upload, making texture mappable");
            MakeMappable();
        }

        unsigned width = pTextures[itex].Size.Width;
        unsigned height = pTextures[itex].Size.Height;

        for (unsigned i = 1; i < level; i++)
        {
            width >>= 1;
            height >>= 1;
            if (width < 1)
                width = 1;
            if (height < 1)
                height = 1;
        }

        UpdateDesc upd;
        upd.DestRect = ImageRect(0,0, width, height);
        upd.PlaneIndex = itex;
        upd.SourcePlane = plane;

        Update(&upd, 1, level);
    }

    return true;
}

bool Texture::Update()
{
    ImageFormat     format   = GetImageFormat();
    TextureManager* pmanager = GetManager();
    bool            rescale  = (TextureFlags & TF_Rescale) ? true : false;
    bool            swMipGen = (TextureFlags & TF_SWMipGen) ? true : false;
    bool            convert  = false;
    ImageData       *psource;
    ImageData       *pdecodeTarget = 0, *prescaleTarget = 0, *pconvertTarget = 0;
    ImageData       imageData1, imageData2;
    Ptr<RawImage>   pimage1, pimage2;
    unsigned        sourceMipLevels = GetMipmapCount(); // May be different from MipLevels    

    // Texture update proceeds in four (optional) steps:
    //   1. Image::Decode - Done unless rescaling directly from RawImage.
    //   2. Rescale       - Done if non-pow2 textures are not supported as necessary.
    //   3. Convert       - Needed if conversion wasn't applied in Decode.
    //   4. SW Mipmap Gen - Loop to generate SW mip-maps, may also have a convert step.

    // Although Decode can do scan-line conversion, Convert step is necessary
    // because our Rescale and GenerateMipLevel functions don't support all GL
    // destination formats. If non-supported format is encountered, conversion
    // is delayed till after rescale (which, of course, requires an extra image buffer).

    ImageFormat      rescaleBuffFromat = pFormat->ConvFormat;
    ImageRescaleType rescaleType = ResizeNone;

    if (rescale)
    {
        if (pFormat->BytesPerPixel == 4)
            rescaleType = ResizeRgbaToRgba;
        else if (pFormat->BytesPerPixel == 1)
            rescaleType = ResizeGray;
        else
        {
            rescaleBuffFromat = Image_R8G8B8A8;
            convert = true;
        }
    }
    if (swMipGen && !(pFormat->BytesPerPixel == 4 || pFormat->BytesPerPixel == 1))
        convert = true;

    // *** 1. Decode from source pImage to Image1/MappedTexture

    Lock::Locker  imageLock(&pManagerLocks->ImageLock);

    if (!pImage || (TextureFlags & TF_UserAlloc))
        return false;

    // Decode is not needed if RawImage is used directly as a source for rescale.
    if (! ((pImage->GetImageType() == Image::Type_RawImage) && rescale) )
    {
        // Determine decodeTarget -> Image1 if rescale / convert will take place
        if (rescale || convert || !pmanager->mapTexture(this))
        {
            pimage1 = *RawImage::Create(rescaleBuffFromat, sourceMipLevels, ImgSize, 0);
            if (!pimage1) return false;
            pimage1->GetImageData(&imageData1);
            imageData1.Format = (ImageFormat)(format | ImageFormat_Convertible);
            pdecodeTarget = &imageData1;
        }
        else
        {
            pdecodeTarget = &pMap->Data;
        }

        // Decode to decode_Target (Image1 or MappedTexture)
        if ( !pImage->Decode(pdecodeTarget, convert ? &Image::CopyScanlineDefault : pFormat->CopyFunc) )
        {
            // Image decoding can fail, for example for video textures which are not ready yet. 
            if ( pMap && (pdecodeTarget == &pMap->Data) )
                pmanager->unmapTexture(this);

            // Note: still return true, even when decoding fails. Otherwise texture initialization may (erroneously) fail.
            return true;
        }

        psource = pdecodeTarget;
    }
    else
    {
        ((RawImage*)pImage->GetAsImage())->GetImageData(&imageData1);
        psource = &imageData1;
    }

    // *** 2. Rescale - from source to Image2/MappedTexture

    if (rescale)
    {
        if (convert || !pmanager->mapTexture(this))
        {
            pimage2 = *RawImage::Create(pFormat->ConvFormat, sourceMipLevels, pTextures[0].Size, 0);
            if (!pimage2) return false;
            pimage2->GetImageData(&imageData2);
            prescaleTarget = &imageData2;
        }
        else
        {
            prescaleTarget = &pMap->Data;
        }

        if (rescaleType == ResizeNone)
        {
            rescaleType = GetImageFormatRescaleType(format);
            SF_ASSERT(rescaleType != ResizeNone);
        }        
        RescaleImageData(*prescaleTarget, *psource, rescaleType);
        psource = prescaleTarget;
    }

    // *** 3. Convert - from temp source to MappedTexture
    if (convert)
    {
        if (pmanager->mapTexture(this))
            pconvertTarget = &pMap->Data;
        else if (prescaleTarget)
            pconvertTarget = prescaleTarget;
        else
        {
            pimage2 = *RawImage::Create(format, sourceMipLevels, pTextures[0].Size, 0);
            if (!pimage2) return false;
            pimage2->GetImageData(&imageData2);
            pconvertTarget = &imageData2;
        }

        ConvertImageData(*pconvertTarget, *psource, pFormat->CopyFunc);
        psource = pconvertTarget;
    }

    // *** 4. Generate Mip-Levels

    if (swMipGen)
    {        
        unsigned formatPlaneCount = ImageData::GetFormatPlaneCount(format);
        SF_ASSERT(sourceMipLevels == 1);

        // For each level, generate next mip-map from source to target.
        // Source may be either RawImage, Image1/2, or even MappedTexture itself.
        // Target will be Image1/2 if conversion is needed, MappedTexture otherwise.

        for (unsigned iplane=0; iplane < formatPlaneCount; iplane++)
        {
            ImagePlane splane, tplane;
            psource->GetMipLevelPlane(0, iplane, &splane);

            for (unsigned level = 1; level < MipLevels; level++)
            {
                pMap->Data.GetMipLevelPlane(level, iplane, &tplane);

                if (!convert)
                {
                    GenerateMipLevel(tplane, splane, format, iplane);
                    // If we generated directly into MappedTexture,
                    // texture will be used as source for next iteration.
                    splane = tplane;
                }
                else
                {
                    // Extra conversion step means, source has only one level.
                    // We reuse it through GenerateMipLevel, which allows source
                    // and destination to be the same.
                    ImagePlane dplane(splane);
                    dplane.SetNextMipSize();
                    GenerateMipLevel(dplane, splane, format, iplane);
                    ConvertImagePlane(tplane, dplane, format, iplane,
                        pFormat->CopyFunc, psource->GetColorMap());
                    splane.Width  = dplane.Width;
                    splane.Height = dplane.Height;
                }
            }
        }
    }

    if (psource == &pMap->Data)
        pmanager->unmapTexture(this);
    else
    {
        for (unsigned itex = 0; itex < TextureCount; itex++)
            for (unsigned level = 0; level < MipLevels; level++)
            {
                ImagePlane plane;
                psource->GetPlane(level * TextureCount + itex, &plane);
                Upload(itex, level, plane);
            }
    }

    return true;
}


void Texture::ReleaseHWTextures(bool)
{
    if ( TextureFlags & TF_DoNotDelete )
        return;

    TextureManager* pmanager = GetManager();
    bool useKillList = !(pmanager->IsMultiThreaded() || GetCurrentThreadId() == pmanager->RenderThreadId);

    for (unsigned itex = 0; itex < TextureCount; itex++)
    {
        GLuint TexId = pTextures[itex].TexId;
        if (TexId)
        {
            if (useKillList)
                pmanager->GLTextureKillList.PushBack(TexId);
            else
                glDeleteTextures(1, &TexId);
        }        
        pTextures[itex].TexId = 0;
    }
}

bool    Texture::Map(ImageData* pdata, unsigned mipLevel, unsigned levelCount)
{
    SF_ASSERT((Use & ImageUse_Map_Mask) != 0);
    SF_ASSERT(!pMap);

    if (levelCount == 0)
        levelCount = MipLevels - mipLevel;

    if (!GetManager()->mapTexture(this, mipLevel, levelCount))
    {
        SF_DEBUG_WARNING(1, "Texture::Map failed - couldn't map texture");
        return false;
    }

    pdata->Initialize(GetImageFormat(), levelCount,
        pMap->Data.pPlanes, pMap->Data.RawPlaneCount, true);
    pdata->Use = Use;
    return true;
}

bool    Texture::Unmap()
{
    if (!pMap) return false;
    GetManager()->unmapTexture(this);
    return true;
}

bool Texture::UpdateRenderTargetData(Render::RenderTargetData*, Render::HAL*)
{
#if !defined(SF_USE_GLES)
    if (!pBackingImage)
        return false;

    ImageData BackingData;
    if (!pBackingImage->GetImageData(&BackingData))
        return false;

    SF_DEBUG_ASSERT(TextureCount == 1, "Can only update RTs with one texture.");
    glBindTexture(GL_TEXTURE_2D, pTextures[0].TexId );
    for (unsigned mip = 0; mip < Alg::Min(BackingData.GetMipLevelCount(), GetMipmapCount()); ++mip)
    {
        ImagePlane plane;
        BackingData.GetPlane(mip, &plane);
        glTexImage2D(GL_TEXTURE_2D, mip, pFormat->GLFormat, pTextures[0].Size.Width,
            pTextures[0].Size.Height, 0, pFormat->GLFormat, GL_UNSIGNED_BYTE, plane.pData );
    }
    return true;
#else
    // GLES 1.1 doesn't have FBOs.
    return false;
#endif
}

bool Texture::UpdateStagingData(Render::RenderTargetData* prtData)
{
#if !defined(SF_USE_GLES)
    if (!pBackingImage)
        return false;

    ImageData BackingData;
    if (!pBackingImage->GetImageData(&BackingData))
        return false;

    RenderTargetData* glPRT = (RenderTargetData*)prtData;
    SF_DEBUG_ASSERT(TextureCount == 1, "Can only update RTs with one texture.");
    SF_DEBUG_ASSERT(MipLevels== 1, "Can only update RTs with one mip-level.");
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, glPRT->FBOID);
    //glPixelStorei() ?
    for (unsigned mip = 0; mip < Alg::Min(BackingData.GetMipLevelCount(), GetMipmapCount()); ++mip)
    {
        ImagePlane plane;
        BackingData.GetPlane(mip, &plane);
        glReadPixels(0, 0, GetSize().Width, GetSize().Height, pFormat->GLFormat, GL_UNSIGNED_BYTE, plane.pData );
    }

    // Set back to the default framebuffer
    RenderTarget* lastRT = GetHAL()->RenderTargetStack.Back().pRenderTarget;
    GL::RenderTargetData* defaultData = (GL::RenderTargetData*)lastRT->GetRenderTargetData();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, defaultData->FBOID);
    return true;
#else
    // GLES 1.1 doesn't have FBOs.
    return false;
#endif
}

bool    Texture::Update(const UpdateDesc* updates, unsigned count, unsigned mipLevel)
{
    // use map and update entire texture if necessary for performance
    if (pBackingImage)
    {
domap:
        bool inUnmap = pMap != 0;

        if (!GetManager()->mapTexture(this, mipLevel, 1))
        {
            SF_DEBUG_WARNING(1, "Texture::Update failed - couldn't map texture");
            return false;
        }

        ImageFormat format = GetImageFormat(); 
        ImagePlane  dplane;

        for (unsigned i = 0; i < count; i++)
        {
            const UpdateDesc &desc = updates[i];
            ImagePlane        splane(desc.SourcePlane);

            pMap->Data.GetPlane(desc.PlaneIndex, &dplane);
            dplane.pData += desc.DestRect.y1 * dplane.Pitch +
                desc.DestRect.x1 * pFormat->BytesPerPixel;

            splane.SetSize(desc.DestRect.GetSize());
            dplane.SetSize(desc.DestRect.GetSize());
            ConvertImagePlane(dplane, splane, format, desc.PlaneIndex,
                pFormat->CopyFunc, 0);
        }

        if (!inUnmap)
            GetManager()->unmapTexture(this);
        return true;
    }

    // partial update
    bool convert = pFormat->CopyFunc != 0;

    for (unsigned i = 0; i < count; i++)
    {
        const UpdateDesc &desc = updates[i];
        ImagePlane        splane(desc.SourcePlane);

#if (defined(GL_UNPACK_ROW_LENGTH) && defined(GL_UNPACK_ALIGNMENT))
        if (!convert && splane.Pitch == splane.Width * pFormat->BytesPerPixel)
        {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, splane.Width);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexSubImage2D(GL_TEXTURE_2D, mipLevel,
                desc.DestRect.x1, desc.DestRect.y1, desc.DestRect.Width(), desc.DestRect.Height(),
                pFormat->GLFormat, pFormat->GLData, splane.pData);

            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        }
        else if (!convert && splane.Pitch == ((3 + splane.Width * pFormat->BytesPerPixel) & ~3))
        {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, splane.Width);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

            glTexSubImage2D(GL_TEXTURE_2D, mipLevel,
                desc.DestRect.x1, desc.DestRect.y1, desc.DestRect.Width(), desc.DestRect.Height(),
                pFormat->GLFormat, pFormat->GLData, splane.pData);

            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        else
#endif
        if (!convert && splane.Pitch == desc.DestRect.Width() * pFormat->BytesPerPixel)
        {
            glTexSubImage2D(GL_TEXTURE_2D, mipLevel,
                desc.DestRect.x1, desc.DestRect.y1, desc.DestRect.Width(), desc.DestRect.Height(),
                pFormat->GLFormat, pFormat->GLData, splane.pData);
        }
        else
        {
            SF_DEBUG_WARNING(1, "GL::Texture - irregular size, cannot use update, making texture mappable");

            MakeMappable();
            goto domap;
        }
    }

    return true;
}

void Texture::MakeMappable()
{
    pBackingImage = *RawImage::Create(pFormat->Format, MipLevels, pTextures[0].Size, 0);
}

// Copies the image data from the hardware.
#ifdef SF_AMP_SERVER
bool Texture::Copy(ImageData* pdata)
{
#if !defined(SF_USE_GLES_ANY)
    Image::CopyScanlineFunc puncopyFunc = pFormat->UncopyFunc;
    if ( !GetManager() || pFormat->Format != pdata->Format || !puncopyFunc)
    {
        // - No texture manager, OR 
        // - Output format is different from the source input format of this texture (unexpected, because
        //   we should be copying back into the image's original source format) OR
        // - We don't know how to uncopy this format.
        return false;
    }


    // Try to map the texture. Only textures with backing images are actually mappable, which is not the usual case.
    bool alreadyMapped = (pMap != 0);
    unsigned mipCount = GetMipmapCount();
    bool mapped = alreadyMapped;
    if (!alreadyMapped )
        mapped = GetManager()->mapTexture(this, 0, mipCount) != 0;


    // Copy the planes into pdata, using the reverse copy function.
    SF_ASSERT( !mapped || pdata->GetPlaneCount() == pMap->Data.GetPlaneCount());
    Ptr<RawImage> imageDest = 0;
    int ip;
    for ( ip = 0; ip < pdata->RawPlaneCount; ip++ )
    {
        ImagePlane splane, dplane;
        pdata->GetPlane(ip, &dplane);

        if ( mapped )
        {
            pMap->Data.GetPlane(ip, &splane);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, pTextures[ip].TexId );
            imageDest = *RawImage::Create(GetImageFormat(), mipCount, pTextures[ip].Size, Use);
            ImageData pdata;
            imageDest->GetImageData(&pdata);

            for ( unsigned mip = 0; mip < mipCount; ++ mip)
            {
                pdata.GetMipLevelPlane(mip, ip, &splane);

                // For safety.
                GLint w,h;
                glGetTexLevelParameteriv(GL_TEXTURE_2D, mip, GL_TEXTURE_WIDTH, &w );
                glGetTexLevelParameteriv(GL_TEXTURE_2D, mip, GL_TEXTURE_HEIGHT, &h);
                SF_ASSERT((unsigned)w == splane.Width && (unsigned)h == splane.Height);

                // Get the image data from GL, to a temporary buffer.
                glGetTexImage(GL_TEXTURE_2D, mip, pFormat->GLColors, pFormat->GLData, splane.pData );
            }
        }
        ConvertImagePlane(dplane, splane, GetFormat(), ip, puncopyFunc, 0);
    }

    // Unmap the texture, if we mapped it.
    if ( mapped && !alreadyMapped )
        GetManager()->unmapTexture(this);

    return true;
#else
    // Image read-back not supported on GLES2.
    SF_UNUSED(pdata);
    return false;
#endif
}
#endif // SF_AMP_SERVER

// ***** DepthStencilSurface
int DepthStencilSurface::GLFormatIndex = -1;
#if !defined(SF_USE_GLES)
unsigned DepthStencilSurface::GLStencilFormats[] =
{
#if defined(SF_USE_GLES2)
    GL_DEPTH24_STENCIL8_OES,    // iOS requires this, because it's first, it will always succeed.
    GL_UNSIGNED_INT_24_8_OES,
    GL_STENCIL_INDEX8,
    GL_DEPTH_STENCIL_OES,
    GL_STENCIL_INDEX,
#endif
#if defined(GL_OES_stencil4)
    GL_STENCIL_INDEX4_OES,
#endif
#if defined(GL_ARB_framebuffer_object)
    GL_STENCIL_INDEX8,
    GL_STENCIL_INDEX4,
    GL_STENCIL_INDEX16,
    GL_DEPTH24_STENCIL8,
#endif
    0
};
#else
unsigned DepthStencilSurface::GLStencilFormats[] =
{
    0
};
#endif

DepthStencilSurface::DepthStencilSurface(TextureManagerLocks* pmanagerLocks, const ImageSize& size) :
    Render::DepthStencilSurface(pmanagerLocks), Size(size), RenderBufferID(0), State(Texture::State_InitPending)
{

}

DepthStencilSurface::~DepthStencilSurface()
{
    TextureManager* pmanager = (TextureManager*)GetTextureManager();
    bool useKillList = !(pmanager->IsMultiThreaded() || GetCurrentThreadId() == pmanager->RenderThreadId);
    pmanager->GLDepthStencilKillList.PushBack(RenderBufferID);
    if ( !useKillList )
    {
        pmanager->processTextureKillList();
    }
}

bool DepthStencilSurface::Initialize()
{
#if !defined(SF_USE_GLES)
    glGenRenderbuffersEXT(1, &RenderBufferID);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, RenderBufferID);

    // Clear glError
    GLenum glError = glGetError();
    if ( GLFormatIndex < 0 )
        GLFormatIndex = 0;

    do 
    {
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GLStencilFormats[GLFormatIndex], Size.Width, Size.Height);
        glError = glGetError();
    } while ( glError != GL_NO_ERROR && SetNextGLFormatIndex());

    if ( glGetError() != 0 )
        State = Texture::State_InitFailed;
    else
        State = Texture::State_Valid;
#else
    // No renderbuffer extension on GLES1.1
    State = Texture::State_InitFailed;
#endif // !SF_USE_GLES
    return State == Texture::State_Valid;
}

bool DepthStencilSurface::SetNextGLFormatIndex()
{
    if ( GLFormatIndex >= 0 && GLStencilFormats[GLFormatIndex] == 0 )
        return false;
    GLFormatIndex++;
    SF_ASSERT(GLFormatIndex >= 0);
    SF_DEBUG_MESSAGE2(1, "format index = %d, enum = 0x%x", GLFormatIndex, GLStencilFormats[GLFormatIndex] );
    return GLStencilFormats[GLFormatIndex] > 0;
}

bool DepthStencilSurface::CurrentFormatHasDepth()
{
    switch(GLStencilFormats[GLFormatIndex])
    {
#if defined(SF_USE_GLES2)
        case GL_DEPTH24_STENCIL8_OES:
        case GL_UNSIGNED_INT_24_8_OES:
        case GL_DEPTH_STENCIL_OES:
            return true;
#endif
#if defined(GL_ARB_framebuffer_object)
        case GL_DEPTH24_STENCIL8:
            return true;
#endif

        default: 
            return false;
    }
}

// ***** MappedTexture

bool MappedTexture::Map(Texture* ptexture, unsigned mipLevel, unsigned levelCount)
{
    SF_ASSERT(!IsMapped());
    SF_ASSERT((mipLevel + levelCount) <= ptexture->MipLevels);

    // Initialize Data as efficiently as possible.
    if (levelCount <= PlaneReserveSize)
        Data.Initialize(ptexture->GetImageFormat(), levelCount, Planes, ptexture->GetPlaneCount(), true);
    else if (!Data.Initialize(ptexture->GetImageFormat(), levelCount, true))
        return false;
    if (!ptexture->pBackingImage)
        return false;

    pTexture      = ptexture;
    StartMipLevel = mipLevel;
    LevelCount    = levelCount;

    unsigned textureCount = ptexture->TextureCount;
    ImageData BackingData;
    ptexture->pBackingImage->GetImageData(&BackingData);

    for (unsigned itex = 0; itex < textureCount; itex++)
    {
        ImagePlane              plane;

        for(unsigned i = 0; i < StartMipLevel; i++)
            plane.SetNextMipSize();

        for (unsigned level = 0; level < levelCount; level++)
        {
            BackingData.GetMipLevelPlane(level + StartMipLevel, itex, &plane);
            Data.SetPlane(level * textureCount + itex, plane);
        }
    }

    pTexture->pMap = this;
    return true;
}

void MappedTexture::Unmap()
{
    unsigned textureCount = pTexture->TextureCount;

    for (unsigned itex = 0; itex < textureCount; itex++)
    {
        ImagePlane plane;

        for (int level = 0; level < LevelCount; level++)
        {
            Data.GetPlane(level * textureCount + itex, &plane);

            if (plane.pData)
                pTexture->Upload(itex, level + StartMipLevel, plane);
        }
    }

    pTexture->pMap = 0;
    pTexture       = 0;
    StartMipLevel  = 0;
    LevelCount     = 0;    
}

// ***** TextureManager

TextureManager::TextureManager(ThreadId renderThreadId, ThreadCommandQueue* commandQueue) :
    Render::TextureManager(),
    pHal(0),
    Caps(0),
    RenderThreadId(renderThreadId),
    pRTCommandQueue(commandQueue)
{
}

TextureManager::~TextureManager()
{   
    Mutex::Locker lock(&pLocks->TextureMutex);

    // InitTextureQueue MUST be empty, or there was a thread
    // service problem.
    SF_ASSERT(TextureInitQueue.IsEmpty());
    processTextureKillList();    

    // Notify all textures
    while (!Textures.IsEmpty())
        Textures.GetFirst()->LoseManager();

    pLocks->pManager = 0;
}

void TextureManager::NotifyLostContext()
{
    Mutex::Locker lock(&pLocks->TextureMutex);

    // InitTextureQueue MUST be empty, or there was a thread
    // service problem.
    SF_ASSERT(TextureInitQueue.IsEmpty());
    //processTextureKillList();
    GLTextureKillList.Clear();

    // Notify all textures
    Texture* p= Textures.GetFirst();
    while (!Textures.IsNull(p))
    {
        p->LoseTextureData();
	    p = Textures.GetNext(p);
    }
}

void TextureManager::RestoreAfterLoss()
{
    Mutex::Locker lock(&pLocks->TextureMutex);

    Texture* p= Textures.GetFirst();
    while (!Textures.IsNull(p))
    {
        // Try to restore textures.
        if (p->State == Texture::State_Lost)
            p->Initialize();
        p = Textures.GetNext(p);
    }
}

void TextureManager::ApplyTexture(unsigned stageIndex, GLint texture)
{
    // Detect redundant texture changes.
    if ( CurrentTextures[stageIndex] == texture )
        return;
    glActiveTexture(GL_TEXTURE0 + stageIndex);
    glBindTexture(GL_TEXTURE_2D, texture );
}

void TextureManager::DestroyFBO(GLuint fboid)
{
#if !defined(SF_USE_GLES)
    if (CanCreateTextureCurrentThread())
        glDeleteFramebuffersEXT(1, &fboid);
    else
        GLFrameBufferKillList.PushBack(fboid);
#endif
}

// ***** GL Format mapping and conversion functions

void SF_STDCALL GL_CopyScanline8_Extend_A_LA(UByte* pd, const UByte* ps, UPInt size, Palette*, void*)
{
    for (UPInt i = 0; i< size; i++, pd+=2, ps++)
    {        
        pd[1] = ps[0];
        pd[0] = 255;
    }
}

// Image to Texture format conversion and mapping table,
// organized by the order of preferred image conversions.

enum TexDrawMode
{
    FixFn = 1,
    Shader = 2,
    Both = 3
};

#if defined(SF_USE_GLES)
#define BGRA_EXT "GL_APPLE_texture_format_BGRA8888"
#elif defined(GL_ES_VERSION_2_0)
#define BGRA_EXT "GL_IMG_texture_format_BGRA8888"
#else
#define BGRA_EXT "EXT_bgra"
#endif

TextureFormat GLTextureFormatMapping[] = 
{
    // Standard formats.
#ifdef SF_OS_PS3
    { Image_R8G8B8A8,   Image_R8G8B8A8,     GL_RGBA,            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 4, true,  Both,  NULL,        &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault },
    { Image_B8G8R8A8,   Image_R8G8B8A8,     GL_RGBA,            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 4, false, Both,  NULL,        &Image_CopyScanline32_SwapBR,           &Image_CopyScanline32_SwapBR },
    { Image_R8G8B8,     Image_R8G8B8A8,     GL_RGBA,            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 4, false, Both,  NULL,        &Image_CopyScanline24_Extend_RGB_RGBA,  &Image_CopyScanline32_Retract_RGBA_RGB },
    { Image_B8G8R8,     Image_R8G8B8A8,     GL_RGBA,            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 4, false, Both,  NULL,        &Image_CopyScanline24_Extend_RGB_RGBA,  &Image_CopyScanline32_Retract_RGBA_RGB },
#else
    { Image_R8G8B8A8,   Image_R8G8B8A8,     GL_RGBA,            GL_RGBA,            GL_UNSIGNED_BYTE, 4, true,  Both,  NULL,        &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault},
    { Image_R8G8B8A8,   Image_R8G8B8A8,     GL_RGBA,            GL_BGRA_EXT,        GL_UNSIGNED_BYTE, 4, false, Both,  BGRA_EXT,    &Image_CopyScanline32_SwapBR,           &Image_CopyScanline32_SwapBR},
    { Image_B8G8R8A8,   Image_R8G8B8A8,     GL_RGBA,            GL_BGRA_EXT,        GL_UNSIGNED_BYTE, 4, true,  Both,  BGRA_EXT,    &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault},
    { Image_B8G8R8A8,   Image_R8G8B8A8,     GL_RGBA,            GL_RGBA,            GL_UNSIGNED_BYTE, 4, false, Both,  NULL,        &Image_CopyScanline32_SwapBR,           &Image_CopyScanline32_SwapBR},

#if !defined(SF_USE_GLES_ANY)
    { Image_R8G8B8,     Image_R8G8B8,       GL_RGB,             GL_RGB,             GL_UNSIGNED_BYTE, 3, true,  Both,  NULL,        &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault },
#endif
    { Image_R8G8B8,     Image_R8G8B8A8,     GL_RGBA,            GL_RGBA,            GL_UNSIGNED_BYTE, 4, false, Both,  NULL,        &Image_CopyScanline24_Extend_RGB_RGBA,  &Image_CopyScanline32_Retract_RGBA_RGB },
    { Image_R8G8B8,     Image_R8G8B8A8,     GL_RGBA,            GL_BGRA_EXT,        GL_UNSIGNED_BYTE, 4, false, Both,  BGRA_EXT,    &Image_CopyScanline24_Extend_RGB_BGRA,  &Image_CopyScanline32_Retract_BGRA_RGB },

#if !defined(SF_USE_GLES_ANY)
    { Image_B8G8R8,     Image_R8G8B8,       GL_RGB,             GL_BGR_EXT,         GL_UNSIGNED_BYTE, 3, true,  Both,  BGRA_EXT,    &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault},
#endif
    { Image_B8G8R8,     Image_R8G8B8A8,     GL_RGBA,            GL_BGRA_EXT,        GL_UNSIGNED_BYTE, 4, false, Both,  BGRA_EXT,    &Image_CopyScanline24_Extend_RGB_RGBA,  &Image_CopyScanline32_Retract_RGBA_RGB },
    { Image_B8G8R8,     Image_R8G8B8A8,     GL_RGBA,            GL_RGBA,            GL_UNSIGNED_BYTE, 4, false, Both,  NULL,        &Image_CopyScanline24_Extend_RGB_BGRA,  &Image_CopyScanline32_Retract_BGRA_RGB },
#endif

    { Image_A8,         Image_A8,           GL_ALPHA,           GL_ALPHA,           GL_UNSIGNED_BYTE, 1, true,  FixFn, NULL,        &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault },
    { Image_A8,         Image_A8,           GL_LUMINANCE,       GL_LUMINANCE,       GL_UNSIGNED_BYTE, 1, true,  Shader,NULL,        &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault },
//    { Image_A8,       Image_A8L8,         GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 2, false, Both,  NULL,        &GL_CopyScanline8_Extend_A_LA },

    // Video formats.
    { Image_Y8_U2_V2,   Image_Y8_U2_V2,     GL_ALPHA,           GL_ALPHA,           GL_UNSIGNED_BYTE, 1, true,  Both,   0,          &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault },
    { Image_Y8_U2_V2_A8,Image_Y8_U2_V2_A8,  GL_ALPHA,           GL_ALPHA,           GL_UNSIGNED_BYTE, 1, true,  Both,   0,          &Image::CopyScanlineDefault,            &Image::CopyScanlineDefault },


    // Compressed formats.
#if defined(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
    { Image_DXT1,     Image_DXT1,     GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, 0, true, Both, "EXT_texture_compression_s3tc",   &Image::CopyScanlineDefault, &Image::CopyScanlineDefault },
    { Image_DXT3,     Image_DXT3,     GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_UNSIGNED_BYTE, 0, true, Both, "EXT_texture_compression_s3tc",   &Image::CopyScanlineDefault, &Image::CopyScanlineDefault },
    { Image_DXT5,     Image_DXT5,     GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, 0, true, Both, "EXT_texture_compression_s3tc",   &Image::CopyScanlineDefault, &Image::CopyScanlineDefault },
#endif

#if defined(GL_IMG_texture_compression_pvrtc)
    { Image_PVRTC_RGB_4BPP,   Image_PVRTC_RGB_4BPP,  GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,  GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, GL_UNSIGNED_BYTE, 0, true, Both, "IMG_texture_compression_pvrtc", &Image::CopyScanlineDefault, 0 },
    { Image_PVRTC_RGB_2BPP,   Image_PVRTC_RGB_2BPP,  GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,  GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, GL_UNSIGNED_BYTE, 0, true, Both, "IMG_texture_compression_pvrtc", &Image::CopyScanlineDefault, 0 },
    { Image_PVRTC_RGBA_4BPP,  Image_PVRTC_RGBA_4BPP, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, GL_UNSIGNED_BYTE, 0, true, Both, "IMG_texture_compression_pvrtc", &Image::CopyScanlineDefault, 0 },
    { Image_PVRTC_RGBA_2BPP,  Image_PVRTC_RGBA_2BPP, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, GL_UNSIGNED_BYTE, 0, true, Both, "IMG_texture_compression_pvrtc", &Image::CopyScanlineDefault, 0 },
#endif // GL_IMG_texture_compression_pvrtc

#if defined(GL_OES_compressed_ETC1_RGB8_texture)
    { Image_ETC1_RGB_4BPP,    Image_ETC1_RGB_4BPP,   GL_ETC1_RGB8_OES,  GL_ETC1_RGB8_OES, GL_UNSIGNED_BYTE, 0, true, Both, "OES_compressed_ETC1_RGB8_texture", &Image::CopyScanlineDefault, 0 },
#endif // GL_OES_compressed_ETC1_RGB8_texture

#if defined(GL_AMD_compressed_ATC_texture)
    { Image_ATCIC,    Image_ATCIC,    GL_ATC_RGB_AMD,                     GL_ATC_RGB_AMD,                     GL_UNSIGNED_BYTE, 0, true, Both, "AMD_compressed_ATC_texture", &Image::CopyScanlineDefault, 0 },
    { Image_ATCICA,   Image_ATCICA,   GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,     GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,     GL_UNSIGNED_BYTE, 0, true, Both, "AMD_compressed_ATC_texture", &Image::CopyScanlineDefault, 0 },
    { Image_ATCICI,   Image_ATCICI,   GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD, GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD, GL_UNSIGNED_BYTE, 0, true, Both, "AMD_compressed_ATC_texture", &Image::CopyScanlineDefault, 0 },
#endif // GL_AMD_compressed_ATC_texture

    { Image_None, Image_None,      0,  0, false, 0, 0 }
};


bool CheckExtension (const char *exts, const char *name)
{
    const char *p = strstr(exts, name);
    return (p && (p[strlen(name)] == 0 || p[strlen(name)] == ' '));
}

const Render::TextureFormat*  TextureManager::getTextureFormat(ImageFormat format) const
{
    for (unsigned i = 0; i< TextureFormats.GetSize(); i++)    
        if (TextureFormats[i]->Format == format)
            return (Render::TextureFormat*)TextureFormats[i];
    return 0;
}

GLenum TextureManager::getBaseTextureFormatFromInternal(GLenum intfmt)
{
#ifdef SF_USE_GLES_ANY
    return intfmt;
#else
    switch( intfmt )
    {
        case GL_ALPHA4:
        case GL_ALPHA8:
        case GL_ALPHA12:
        case GL_ALPHA16:
            return GL_ALPHA;
        case GL_LUMINANCE4:
        case GL_LUMINANCE8:
        case GL_LUMINANCE12:
        case GL_LUMINANCE16:
        case GL_LUMINANCE4_ALPHA4:
        case GL_LUMINANCE6_ALPHA2:
        case GL_LUMINANCE8_ALPHA8:
        case GL_LUMINANCE12_ALPHA4:
        case GL_LUMINANCE12_ALPHA12:
        case GL_LUMINANCE16_ALPHA16:
        case GL_INTENSITY:
        case GL_INTENSITY4:
        case GL_INTENSITY8:
        case GL_INTENSITY12:
        case GL_INTENSITY16:
            return GL_LUMINANCE;

        case GL_R3_G3_B2:
        case GL_RGB4:
        case GL_RGB5:
        case GL_RGB8:
        case GL_RGB10:
        case GL_RGB12:
        case GL_RGB16:
            return GL_RGB;

        case GL_RGBA2:
        case GL_RGBA4:
        case GL_RGB5_A1:
        case GL_RGBA8:
        case GL_RGB10_A2:
        case GL_RGBA12:
        case GL_RGBA16:
            return GL_RGBA;
        default:
            return intfmt;
    }
#endif
}

void TextureManager::Initialize(HAL* phal)
{
    RenderThreadId = GetCurrentThreadId();
    pHal = phal;
    Caps = 0;

    const char *glexts = (const char *) glGetString(GL_EXTENSIONS);

#if defined(SF_USE_GLES)
    // XXX
    if (CheckExtension(glexts, BGRA_EXT))
        Caps |= TC_UseBgra;
#elif defined(GL_ES_VERSION_2_0)
    Caps |= TC_NonPower2Limited;
    if (CheckExtension(glexts, "GL_OES_texture_npot"))
        Caps |= TC_NonPower2Full;
// PowerVR texture support is different between Android 2.0-2.2 and 2.3
// 2.2 doesn't support RGBA but supports BGRA
// 2.3 supports RGBA, but not BGRA, but still reports that BGRA is supported
if (CheckExtension(glexts, "GL_OES_required_internalformat") &&
    CheckExtension(glexts, "GL_IMG_texture_format_BGRA8888") &&
    !CheckExtension(glexts, "GL_IMG_multisampled_render_to_texture"))
    Caps |= TC_UseBgra;
#else
    // XXX
    Caps |= TC_NonPower2Full|TC_NonPower2Limited;
#endif

    TextureFormats.Clear();
    TextureFormat* pmapping;

    for (pmapping = GLTextureFormatMapping; pmapping->Format != Image_None; pmapping++)
    {
        // See if format is supported.
#if defined(SF_USE_GLES) || defined(GL_ES_VERSION_2_0)
        if (Caps & TC_UseBgra)
        {
            if (pmapping->GLFormat == GL_RGBA)
                continue;
        }
        else if (pmapping->GLFormat == GL_BGRA_EXT)
            continue;
#endif
        if (pmapping->Extension == NULL || CheckExtension(glexts, pmapping->Extension))
        {
            TextureFormats.PushBack(pmapping);

            // If format added, skip additional mappings for it.
            while ((pmapping+1)->Format == pmapping->Format)
                pmapping++;
        }
    }
}


MappedTexture* TextureManager::mapTexture(Texture* ptexture, unsigned mipLevel, unsigned levelCount)
{
    MappedTexture* pmap;

    if (MappedTexture0.Reserve())
        pmap = &MappedTexture0;
    else
    {
        pmap = SF_HEAP_AUTO_NEW(this) MappedTexture;
        if (!pmap) return 0;
    }

    if (pmap->Map(ptexture, mipLevel, levelCount))
        return pmap;
    if (pmap != &MappedTexture0)
        delete pmap;
    return 0;  
}

void           TextureManager::unmapTexture(Texture *ptexture)
{
    MappedTexture *pmap = ptexture->pMap;
    pmap->Unmap();
    if (pmap != &MappedTexture0)
        delete pmap;
}

void    TextureManager::processTextureKillList()
{
    if ( GLTextureKillList.GetSize() > 0 )
    {
        glDeleteTextures((GLsizei)GLTextureKillList.GetSize(), GLTextureKillList.GetDataPtr());
        GLTextureKillList.Clear();
    }

#if !defined(SF_USE_GLES)
    if ( GLDepthStencilKillList.GetSize() > 0 )
    {
        glDeleteRenderbuffersEXT((GLsizei)GLDepthStencilKillList.GetSize(), GLDepthStencilKillList.GetDataPtr());
        GLDepthStencilKillList.Clear();
    }
    if ( GLFrameBufferKillList.GetSize() > 0 )
    {
        glDeleteFramebuffersEXT((GLsizei)GLFrameBufferKillList.GetSize(), GLFrameBufferKillList.GetDataPtr());
        GLFrameBufferKillList.Clear();
    }
#endif
}

void    TextureManager::processInitTextures()
{
    // TextureMutex lock expected externally.
    //Mutex::Locker lock(&TextureMutex);

    if (!TextureInitQueue.IsEmpty())
    {
        do {
            Texture* ptexture = TextureInitQueue.GetFirst();
            ptexture->RemoveNode();
            ptexture->pPrev = ptexture->pNext = 0;
            if (ptexture->Initialize())
                Textures.PushBack(ptexture);

        } while (!TextureInitQueue.IsEmpty());
        pLocks->TextureInitWC.NotifyAll();
    }
}

bool             TextureManager::CanCreateTextureCurrentThread()
{
    return (GetCurrentThreadId() == RenderThreadId);
}

// Should be called before each frame on RenderThread.
void TextureManager::BeginFrame( )   
{ 
    Render::TextureManager::BeginFrame();
    memset(CurrentTextures, -1, sizeof(CurrentTextures) );
}

Render::Texture* TextureManager::CreateTexture(ImageFormat format, unsigned mipLevels,
                                               const ImageSize& size, unsigned use, 
                                               ImageBase* pimage, Render::MemoryManager* allocManager)    
{
    SF_UNUSED(allocManager);

    TextureFormat* ptformat = (TextureFormat*)precreateTexture(format, use, pimage);
    if ( !ptformat )
        return 0;

    Texture* ptexture = 
        SF_HEAP_AUTO_NEW(this) Texture(pLocks, ptformat, mipLevels, size, use, pimage);
    if (!ptexture)
        return 0;
    if (!ptexture->IsValid())
    {
        ptexture->Release();
        return 0;
    }

    Mutex::Locker lock(&pLocks->TextureMutex);

    if (!IsMultiThreaded() || (GetCurrentThreadId() == RenderThreadId))
    {
        // Before initializing texture, process previous requests, if any.
        if (!IsMultiThreaded())
        {
            processTextureKillList();
            processInitTextures();
        }
        if (ptexture->Initialize())
            Textures.PushBack(ptexture);
    }
    else
    {
        TextureInitQueue.PushBack(ptexture);
        if (pRTCommandQueue)
        {
            pLocks->TextureMutex.Unlock();
            pRTCommandQueue->PushThreadCommand(&ServiceCommandInstance);
            pLocks->TextureMutex.DoLock();
        }
        while(ptexture->State == Texture::State_InitPending)
            pLocks->TextureInitWC.Wait(&pLocks->TextureMutex);
    }

    // Clear out 'pImage' reference if it's not supposed to be kept. It is safe to do this
    // without ImageLock because texture hasn't been returned yet, so this is the only
    // thread which has access to it. Also free the data if it is a RawImage.
    if (use & ImageUse_NoDataLoss)
    {
        if ( ptexture->pImage && ptexture->pImage->GetImageType() == Image::Type_RawImage )
            ((Render::RawImage*)ptexture->pImage)->freeData();
        ptexture->pImage = 0;
    }

    // If texture was properly initialized, it would've been added to list.
    if (ptexture->State == Texture::State_InitFailed)
    {
        ptexture->Release();
        return 0;
    }
    return ptexture;
}

Render::Texture* TextureManager::CreateTexture(GLuint texID, bool deleteTexture, 
                                               ImageSize imgSize, ImageBase* pimage )
{
    Texture* ptexture = 
        SF_HEAP_AUTO_NEW(this) Texture(pLocks, texID, deleteTexture, imgSize, pimage);

    if (!ptexture)
        return 0;

    if (!ptexture->IsValid())
    {
        ptexture->Release();
        return 0;
    }
    Mutex::Locker lock(&pLocks->TextureMutex);    
    if (!IsMultiThreaded() || (GetCurrentThreadId() == RenderThreadId))
    {
        // Before initializing texture, process previous requests, if any.
        if (!IsMultiThreaded())
        {
            processTextureKillList();
            processInitTextures();
        }
        if (ptexture->Initialize())
            Textures.PushBack(ptexture);
    }
    else
    {
        TextureInitQueue.PushBack(ptexture);
        if (pRTCommandQueue)
        {
            pLocks->TextureMutex.Unlock();
            pRTCommandQueue->PushThreadCommand(&ServiceCommandInstance);
            pLocks->TextureMutex.DoLock();
        }
        while(ptexture->State == Texture::State_InitPending)
            pLocks->TextureInitWC.Wait(&pLocks->TextureMutex);
    }

    // If texture was properly initialized, it would've been added to list.
    if (ptexture->State == Texture::State_InitFailed)
    {
        ptexture->Release();
        return 0;
    }
    return ptexture;
}

Render::DepthStencilSurface* TextureManager::CreateDepthStencilSurface(const ImageSize& size, MemoryManager* manager )
{
    SF_UNUSED(manager);
    DepthStencilSurface* pdss = SF_HEAP_AUTO_NEW(this) DepthStencilSurface(pLocks, size);
    if (!pdss)
        return 0;

    Mutex::Locker lock(&pLocks->TextureMutex);
    if (!IsMultiThreaded() || (GetCurrentThreadId() == RenderThreadId))
    {
        // Before initializing texture, process previous requests, if any.
        if (!IsMultiThreaded())
        {
            processTextureKillList();
            processInitTextures();
        }
        pdss->Initialize();
    }
    else
    {
        DepthStencilInitQueue.PushBack(pdss);
        if (pRTCommandQueue)
        {
            pLocks->TextureMutex.Unlock();
            pRTCommandQueue->PushThreadCommand(&ServiceCommandInstance);
            pLocks->TextureMutex.DoLock();
        }
        while(pdss->State == Texture::State_InitPending)
            pLocks->TextureInitWC.Wait(&pLocks->TextureMutex);
    }
    return pdss;

}

Render::DepthStencilSurface* TextureManager::CreateDepthStencilSurface(GLuint id)
{
#if !defined(SF_USE_GLES)
    if ( !glIsRenderbufferEXT(id) )
        return 0;

    GLint width, height;
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, id);
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &width);
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &height);
    DepthStencilSurface* pdss = SF_HEAP_AUTO_NEW(this) DepthStencilSurface(pLocks, ImageSize(width, height) );
    pdss->RenderBufferID = id;
    pdss->State = Texture::State_Valid;
    return pdss;
#else
    // Not supported on GLES 1.1
    SF_UNUSED(id);
    return 0;
#endif
}

unsigned TextureManager::GetTextureFormatSupport() const
{
    unsigned formats = ImageFormats_Standard;

    for (unsigned i = 0; i< TextureFormats.GetSize(); i++)    
    {
        if (TextureFormats[i]->Format == Image_DXT5)
            formats |= ImageFormats_DXT;
        else if (TextureFormats[i]->Format == Image_PVRTC_RGBA_4BPP)
            formats |= ImageFormats_PVR;
        else if (TextureFormats[i]->Format == Image_ATCICA)
            formats |= ImageFormats_ATITC;
        else if (TextureFormats[i]->Format == Image_ETC1_RGBA_8BPP)
            formats |= ImageFormats_ETC;
    }

    return formats;
}

unsigned TextureManager::GetTextureUseCaps(ImageFormat format)
{
    unsigned use = ImageUse_Update;
#if !defined(SF_OS_MAC) && !defined(SF_OS_ANDROID)
    use |= ImageUse_NoDataLoss;
#endif
    if (!ImageData::IsFormatCompressed(format))
        use |= ImageUse_PartialUpdate | ImageUse_GenMipmaps;

    const Render::TextureFormat* ptformat = getTextureFormat(format);
    if (!ptformat)
        return 0;
    if (isMappable(ptformat))
        use |= ImageUse_MapRenderThread;
    return use;   
}

bool TextureManager::IsNonPow2Supported(ImageFormat, UInt16 use)
{
    unsigned need = TC_NonPower2Limited;
    if (use & (ImageUse_Wrap|ImageUse_GenMipmaps))
        need |= TC_NonPower2Full;

    return ((Caps & need) == need) != 0;
}

}}};  // namespace Scaleform::Render::GL

