/**************************************************************************

Filename    :   D3D1x_Texture.cpp
Content     :   
Created     :   Mar 2011
Authors     :   Bart Muzzin

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "D3D1x_Texture.h"
#include "Render\Render_TextureUtil.h"
#include "Kernel\SF_Debug.h"

namespace Scaleform { namespace Render { namespace D3D1x {

extern TextureFormat::Mapping D3D1xTextureFormatMapping[];

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

Texture::Texture(TextureManagerLocks* pmanagerLocks, ID3D1x(Texture2D)* ptexture, ImageSize size, ImageBase* image) :
    Render::Texture(pmanagerLocks, size, 0, 0, image ), 
    pFormat(0), pMap(0)
{
    TextureFlags |= TF_UserAlloc;
    ptexture->AddRef();
    pTextures = &Texture0;
    pTextures[0].pTexture = ptexture;
    pTextures[0].pView    = 0;
    pTextures[0].Size     = size;
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

void Texture::GetUVGenMatrix(Matrix2F* mat) const
{
    // UV Scaling rules are as follows:
    //  - If image got scaled, divide by original size.
    //  - If image is padded, divide by new texture size.
    const ImageSize& sz = (TextureFlags & TF_Rescale) ? ImgSize : pTextures[0].Size;
    *mat = Matrix2F::Scaling(1.0f / (float)sz.Width, 1.0f / (float)sz.Height);
}


void    Texture::LoseManager()
{        
    Ptr<Texture> thisTexture = this;    // Guards against the texture being deleted inside Texture::LoseManager()
    SF_ASSERT(pMap == 0);
    Render::Texture::LoseManager();
    pFormat = 0; // Users can't access Format any more ?
}

void Texture::LoseTextureData()
{
    Render::Texture::LoseTextureData();
    SF_ASSERT(pMap == 0);

    Lock::Locker lock(&pManagerLocks->ImageLock);
    ReleaseHWTextures();
    State   = State_Lost;
    
    if (pImage)
    {
        // TextureLost may release 'this' Texture, so do it last.
        SF_ASSERT(pImage->GetImageType() != Image::Type_ImageBase);
        Image* pimage = (Image*)pImage;
        pimage->TextureLost(Image::TLR_DeviceLost);
    }
}


bool IsD3DFormatRescaleCompatible(DXGI_FORMAT format,
                                  ImageFormat *ptargetImageFormat,
                                  ResizeImageType* presizeType)
{
    switch(format)
    {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            *ptargetImageFormat = Image_R8G8B8A8;
            *presizeType        = ResizeRgbaToRgba;
            return true;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            *ptargetImageFormat = Image_R8G8B8A8;
            *presizeType        = ResizeRgbaToRgba;
            return true;
        case DXGI_FORMAT_A8_UNORM:
            *ptargetImageFormat = Image_A8;
            *presizeType        = ResizeGray;
            return true;
        default:
            break;
    }
    return false;
}


bool IsD3DFormatMipGenCompatible(DXGI_FORMAT format)
{
    switch(format)
    {
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return true;
        default:
            return false;
    }
}


bool Texture::Initialize()
{    
    SF_AMP_SCOPE_TIMER(GetManager()->RenderThreadId == GetCurrentThreadId() ? AmpServer::GetInstance().GetDisplayStats() : NULL, __FUNCTION__, Amp_Profile_Level_Medium);

    if ( TextureFlags & TF_UserAlloc )
    {
        return Initialize( pTextures[0].pTexture );
    }

    bool            resize  = false;
    ImageFormat     format  = GetImageFormat();
    TextureManager* pmanager= GetManager();
    unsigned        itex;

    // Determine sizes of textures.
    if (State != State_Lost)
    {        
        for (itex = 0; itex < TextureCount; itex++)
        {
            HWTextureDesc& tdesc = pTextures[itex];
            tdesc.Size = ImageData::GetFormatPlaneSize(format, ImgSize, itex);
        }

        if (resize && (Use & ImageUse_Wrap))
        {
            if (ImageData::IsFormatCompressed(format))
            {
                SF_DEBUG_ERROR(1, 
                    "CreateTexture failed - Can't rescale compressed Wrappable image to Pow2");
                State = State_InitFailed;
                return false;
            }
            TextureFlags |= TF_Rescale;
        }
    }
        
    // Determine how many mipLevels we should have and whether we can
    // auto-generate them or not.
    unsigned allocMipLevels = MipLevels;

    if (Use & ImageUse_GenMipmaps)
    {
        SF_ASSERT(MipLevels == 1);
        if (IsD3DFormatMipGenCompatible(pFormat->GetD3DFormat()))
        {            
            TextureFlags |= TF_SWMipGen;
            // If using SW MipGen, determine how many mip-levels we should have.
            allocMipLevels = 31;
            for (itex = 0; itex < TextureCount; itex++)
                allocMipLevels = Alg::Min(allocMipLevels,
                                          ImageSize_MipLevelCount(pTextures[itex].Size));
            MipLevels = (UByte)allocMipLevels;
        }
        else
        {
            allocMipLevels = 0;
        }
    }

    // Only use Dynamic textures for updatable/mappable textures.
    // Also, since Dynamic textures can be lost, don't allow them if ImageUse_InitOnly
    // is not specified.
    bool    allowDynamicTexture = ((Use & ImageUse_InitOnly) != 0) &&
                                  ((Use & (ImageUse_PartialUpdate | ImageUse_Map_Mask)) != 0);
    bool    renderTarget = (Use & ImageUse_RenderTarget) != 0;

    D3D1x(USAGE) usage  = D3D1x(USAGE_DEFAULT);
    UINT cpu            = 0;
    UINT bindFlags      = D3D1x(BIND_SHADER_RESOURCE);
    if (allowDynamicTexture)
    {
        usage = D3D1x(USAGE_DYNAMIC);
        cpu   |= D3D1x(CPU_ACCESS_WRITE);
    }

    if (renderTarget)
        bindFlags |= D3D1x(BIND_RENDER_TARGET);

    // Create textures
    for (itex = 0; itex < TextureCount; itex++)
    {
        HWTextureDesc& tdesc = pTextures[itex];

        D3D1x(TEXTURE2D_DESC) desc;
        memset(&desc, 0, sizeof desc);
        desc.Width              = tdesc.Size.Width;
        desc.Height             = tdesc.Size.Height;
        desc.MipLevels          = MipLevels;
        desc.ArraySize          = 1;
        desc.Format             = pFormat->GetD3DFormat();
        desc.Usage              = usage;
        desc.BindFlags          = bindFlags;
        desc.CPUAccessFlags     = cpu;
        desc.SampleDesc.Count   = 1;

        if (FAILED(pmanager->pDevice->CreateTexture2D( &desc, 0, &tdesc.pTexture) ) ||
            FAILED(pmanager->pDevice->CreateShaderResourceView( tdesc.pTexture, 0, &tdesc.pView)))
        {
            SF_DEBUG_ERROR(1, "ID3D1x(Device)::CreateTexture2D failed");
            // Texture creation failed, release all textures and fail.
initialize_texture_fail_after_create:
            ReleaseHWTextures();
            if (State != State_Lost)
                State = State_InitFailed;
            return false;
        }
    }

    // Upload image content to texture, if any.
    if (pImage && !Update())
    {
        SF_DEBUG_ERROR(1, "CreateTexture failed - couldn't initialize texture");
        goto initialize_texture_fail_after_create;
    }

    State = State_Valid;
    return true;
}

bool Texture::Initialize(ID3D1x(Texture2D)* ptexture)
{
    if ( !ptexture )
        return false;

    // If this is called explicitly (ie. not after the constructor), and the texture is being replaced,
    // A reference needs to be added to the new texture.
    if ( pTextures[0].pTexture != ptexture )
    {
        ReleaseHWTextures();
        pTextures[0].pTexture = ptexture;
        pTextures[0].pTexture->AddRef();
    }

    D3D1x(TEXTURE2D_DESC) texDesc;
    ptexture->GetDesc(&texDesc);
    MipLevels = (UByte)texDesc.MipLevels;

    pFormat = 0;
    TextureManager* pmanager = GetManager();

    // If an image is provided, try to obtain the texture format from the image.
    if ( pImage )
        pFormat = (TextureFormat*)pmanager->getTextureFormat(pImage->GetFormatNoConv());

    // Otherwise, figure out the texture format, based on the mapping table.    
    if ( pFormat == 0 )
    {
        TextureFormat::Mapping* pmapping;
        for (pmapping = D3D1xTextureFormatMapping; pmapping->Format != Image_None; pmapping++)
        {
            if ( pmapping->D3DFormat == texDesc.Format )
            {
                pFormat = (TextureFormat*)pmanager->getTextureFormat(pmapping->Format);
                break;
            }
        }
    }

    // Could not determine the format.
    if ( !pFormat)
    {
        SF_DEBUG_ERROR(1, "Texture::Initialize failed - couldn't determine ImageFormat of user supplied texture.");
        State = State_InitFailed;
        return false;
    }

    // Fill out the HW description.
    pTextures[0].Size.SetSize(texDesc.Width, texDesc.Height);

    // Override the image size if it was not provided.
    if ( ImgSize == ImageSize(0) )
        ImgSize = pTextures[0].Size;

    // Create the view.
    if ( FAILED(GetManager()->pDevice->CreateShaderResourceView( pTextures[0].pTexture, 0, &pTextures[0].pView )))
    {
        State = State_InitFailed;
        return false;
    }

    State = State_Valid;
    return true;
}

bool Texture::Update()
{
    SF_AMP_SCOPE_TIMER(GetManager()->RenderThreadId == GetCurrentThreadId() ? AmpServer::GetInstance().GetDisplayStats() : NULL, __FUNCTION__, Amp_Profile_Level_Medium);

    ImageFormat     format   = GetImageFormat();
    TextureManager* pmanager = GetManager();
    bool            rescale  = (TextureFlags & TF_Rescale) ? true : false;
    bool            swMipGen = (TextureFlags & TF_SWMipGen) ? true : false;
    bool            convert  = false;
    ImageData       *psource;
    ImageData       *pdecodeTarget = 0, *prescaleTarget = 0;
    ImageData       imageData1, imageData2;
    Ptr<RawImage>   pimage1, pimage2;
    unsigned        sourceMipLevels = GetMipmapCount(); // May be different from MipLevels    

    // Texture update proceeds in four (optional) steps:
    //   1. Image::Decode - Done unless rescaling directly from RawImage.
    //   2. Rescale       - Done if non-pow2 textures are not supported as necessary.
    //   3. Convert       - Needed if conversion wasn't applied in Decode.
    //   4. SW Mipmap Gen - Loop to generate SW mip-maps, may also have a convert step.

    // Although Decode can do scan-line conversion, Convert step is necessary
    // because our Rescale and GenerateMipLevel functions don't support all D3D1x
    // destination formats. If non-supported format is encountered, conversion
    // is delayed till after rescale (which, of course, requires an extra image buffer).

    ImageFormat      rescaleBuffFromat = format;
    ImageRescaleType rescaleType = ResizeNone;
    
    if (rescale && !IsD3DFormatRescaleCompatible(pFormat->pMapping->D3DFormat,
                                                 &rescaleBuffFromat, &rescaleType))
        convert = true;
    if (swMipGen && !IsD3DFormatMipGenCompatible(pFormat->pMapping->D3DFormat))
        convert = true;
   
    
    // *** 1. Decode from source pImage to Image1/MappedTexture

    Lock::Locker  imageLock(&pManagerLocks->ImageLock);

    if (!pImage)
        return false;

    // Decode is not needed if RawImage is used directly as a source for rescale.
    if (! ((pImage->GetImageType() == Image::Type_RawImage) && rescale) )
    {
        // Determine decodeTarget -> Image1 if rescale / convert will take place
        if (rescale || convert)
        {
            pimage1 = *RawImage::Create(rescaleBuffFromat, sourceMipLevels, ImgSize, 0);
            if (!pimage1) return false;
            pimage1->GetImageData(&imageData1);
            imageData1.Format = (ImageFormat)(format | ImageFormat_Convertible);
            pdecodeTarget = &imageData1;
        }
        else
        {
            if (!pmanager->mapTexture(this))
                return false;
            pdecodeTarget = &pMap->Data;
        }

        // Decode to decode_Target (Image1 or MappedTexture)
        pImage->Decode(pdecodeTarget, 
            convert ? &Image::CopyScanlineDefault : pFormat->pMapping->CopyFunc);
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
        if (convert)
        {
            pimage2 = *RawImage::Create(format, sourceMipLevels, pTextures[0].Size, 0);
            if (!pimage2) return false;
            pimage2->GetImageData(&imageData2);
            prescaleTarget = &imageData2;
        }
        else
        {
            if (!pmanager->mapTexture(this))
                return false;
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
        if (!pmanager->mapTexture(this))
            return false;
        ConvertImageData(pMap->Data, *psource, pFormat->pMapping->CopyFunc);
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
                                      pFormat->pMapping->CopyFunc, psource->GetColorMap());
                    splane.Width  = dplane.Width;
                    splane.Height = dplane.Height;
                }
            }
        }
    }

    pmanager->unmapTexture(this);
    return true;
}


void Texture::ReleaseHWTextures(bool)
{
    TextureManager* pmanager = GetManager();
    bool useKillList = (pmanager->RenderThreadId == 0 || 
        GetCurrentThreadId() != pmanager->RenderThreadId);

    for (unsigned itex = 0; itex < TextureCount; itex++)
    {
        ID3D1x(Texture2D)* ptexture = pTextures[itex].pTexture;
        ID3D1x(ShaderResourceView)* pview = pTextures[itex].pView;

        if (ptexture)
        {
            if (useKillList)
            {
                pmanager->D3DTextureKillList.PushBack(ptexture);
                pmanager->D3DTexViewKillList.PushBack(pview);
            }
            else
            {
                ptexture->Release();
                pview->Release();
            }
        }
        pTextures[itex].pTexture = 0;
        pTextures[itex].pView    = 0;
    }
}

// Applies a texture to device starting at pstageIndex, advances index
void    Texture::ApplyTexture(DWORD stageIndex, ImageFillMode fm)
{
    TextureManager* pmanager = GetManager();
    ID3D1x(SamplerState)* sampler = pmanager->SamplerStates[fm.Fill];
    ID3D1x(ShaderResourceView) * views[MaxTextureCount];
    for ( unsigned view = 0; view < TextureCount; ++view )
        views[view] = pTextures[view].pView;

    // Assumption: all textures in a stage use the same sampler.
    pmanager->SetSamplerState(stageIndex, TextureCount, views, sampler);
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


bool    Texture::Update(const UpdateDesc* updates, unsigned count, unsigned mipLevel)
{
    SF_AMP_SCOPE_RENDER_TIMER(__FUNCTION__, Amp_Profile_Level_Medium);
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
                        desc.DestRect.x1 * pFormat->pMapping->BytesPerPixel;

        splane.SetSize(desc.DestRect.GetSize());
        dplane.SetSize(desc.DestRect.GetSize());
        ConvertImagePlane(dplane, splane, format, desc.PlaneIndex,
                          pFormat->pMapping->CopyFunc, 0);
    }

    GetManager()->unmapTexture(this);
    return true;
}

#ifdef SF_AMP_SERVER
bool Texture::Copy(ImageData* pdata)
{
    Image::CopyScanlineFunc puncopyFunc = pFormat->pMapping->UncopyFunc;
    if ( !GetManager() || pFormat->pMapping->Format != pdata->Format || !puncopyFunc)
    {
        // - No texture manager, OR 
        // - Output format is different from the source input format of this texture (unexpected, because
        //   we should be copying back into the image's original source format) OR
        // - We don't know how to uncopy this format.
        return false;
    }

    // Map the texture.
    bool alreadyMapped = (pMap != 0);
    unsigned mipCount = GetMipmapCount();
    if (!alreadyMapped && !GetManager()->mapTexture(this, 0, mipCount))
    {
        SF_DEBUG_WARNING(1, "Texture::Copy failed - couldn't map texture");
        return false;
    }
    SF_ASSERT(pMap);

    // Copy the planes into pdata, using the reverse copy function.
    SF_ASSERT( pdata->GetPlaneCount() == pMap->Data.GetPlaneCount() );
    int ip;
    for ( ip = 0; ip < pdata->RawPlaneCount; ip++ )
    {
        ImagePlane splane, dplane;
        pdata->GetPlane(ip, &dplane);
        pMap->Data.GetPlane(ip, &splane);

        ConvertImagePlane(dplane, splane, GetFormat(), ip, puncopyFunc, 0);
    }

    // Unmap the texture, if we mapped it.
    if ( !alreadyMapped )
        GetManager()->unmapTexture(this);

    return true;
}
#endif

// ***** DepthStencilSurface

DepthStencilSurface::DepthStencilSurface(TextureManagerLocks* pmanagerLocks, const ImageSize& size) :
    Render::DepthStencilSurface(pmanagerLocks), Size(size), pDepthStencilSurface(0), pDepthStencilSurfaceView(0),
    State(Texture::State_InitPending)
{
}

DepthStencilSurface::~DepthStencilSurface()
{
    if (pDepthStencilSurface)
        pDepthStencilSurface->Release();
    if (pDepthStencilSurfaceView)
        pDepthStencilSurfaceView->Release();
}

bool DepthStencilSurface::Initialize()
{
    TextureManager* pmanager= (TextureManager*)GetTextureManager();

    D3D1x(TEXTURE2D_DESC) desc;
    memset(&desc, 0, sizeof desc);
    desc.Width              = Size.Width;
    desc.Height             = Size.Height;
    desc.MipLevels          = 1;
    desc.ArraySize          = 1;
    desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.Usage              = D3D1x(USAGE_DEFAULT);
    desc.BindFlags          = D3D1x(BIND_DEPTH_STENCIL);
    desc.CPUAccessFlags     = 0;
    desc.SampleDesc.Count   = 1;

    if ( FAILED(pmanager->pDevice->CreateTexture2D(&desc, 0, (ID3D1x(Texture2D)**)&pDepthStencilSurface)) ||
         FAILED(pmanager->pDevice->CreateDepthStencilView(pDepthStencilSurface, 0,&pDepthStencilSurfaceView )))
        State = Texture::State_InitFailed;
    else
        State = Texture::State_Valid;
    return State == Texture::State_Valid;
}

// ***** MappedTexture

bool MappedTexture::Map(Texture* ptexture, ID3D1x(Device)* pdevice, ID3D1x(DeviceContext)* pcontext, unsigned mipLevel, unsigned levelCount)
{
    SF_ASSERT(!IsMapped());
    SF_ASSERT((mipLevel + levelCount) <= ptexture->MipLevels);

    // Initialize Data as efficiently as possible.
    if (levelCount <= PlaneReserveSize)
        Data.Initialize(ptexture->GetImageFormat(), levelCount, Planes, ptexture->GetPlaneCount(), true);
    else if (!Data.Initialize(ptexture->GetImageFormat(), levelCount, true))
        return false;

    pTexture        = ptexture;
    pDeviceContext  = pcontext;
    StartMipLevel   = mipLevel;
    LevelCount      = levelCount;

    unsigned textureCount = ptexture->TextureCount;

    for (unsigned itex = 0; itex < textureCount; itex++)
    {
        Texture::HWTextureDesc &tdesc = pTexture->pTextures[itex];
        ImagePlane              plane(tdesc.Size, 0);

        for(unsigned i = 0; i < StartMipLevel; i++)
            plane.SetNextMipSize();

        D3D1x(TEXTURE2D_DESC) inputDesc;
        tdesc.pTexture->GetDesc(&inputDesc);
        D3D1x(TEXTURE2D_DESC) stagingDesc;
        memcpy(&stagingDesc, &inputDesc, sizeof inputDesc);
        stagingDesc.CPUAccessFlags  = D3D1x(CPU_ACCESS_WRITE);
        stagingDesc.Usage           = D3D1x(USAGE_STAGING);
        stagingDesc.BindFlags       = 0;

        if ( FAILED(pdevice->CreateTexture2D(&stagingDesc, 0, &pStagingTextures[itex] )))
            return false;

        pDeviceContext->CopyResource(pStagingTextures[itex], tdesc.pTexture);
        for (unsigned level = 0; level < levelCount; level++)
        {
            D3D1x(MAPPED_TEXTURE2D) mappedResource;
            if ( SUCCEEDED( D3D1xMapTexture2D(pDeviceContext, pStagingTextures[itex], level, D3D1x(MAP_WRITE), 0, &mappedResource)))
            {
                plane.Pitch    = mappedResource.RowPitch;
                plane.pData    = (UByte*)mappedResource.pData;
                plane.DataSize = ImageData::GetMipLevelSize(Data.GetFormat(), plane.GetSize(), itex); 
            }
            else
            {
                plane.Pitch    = 0;
                plane.pData    = 0;
                plane.DataSize = 0;
                return false;
            }

            Data.SetPlane(level * textureCount + itex, plane);
            // Prepare for next level.
            plane.SetNextMipSize();
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
        Texture::HWTextureDesc &tdesc = pTexture->pTextures[itex];
        ImagePlane plane;

        // Unmap the entire resource first.
        for (int level = 0; level < LevelCount; level++)
            D3D1xUnmapTexture2D(pDeviceContext, pStagingTextures[itex], level);

        for (int level = 0; level < LevelCount; level++)
        {
            Data.GetPlane(level * textureCount + itex, &plane);
            if (plane.pData)
            {
                pDeviceContext->CopySubresourceRegion(tdesc.pTexture, level + StartMipLevel, 0, 0, 0, pStagingTextures[itex], level, 0);
                plane.pData = 0;
            }
        }
        pStagingTextures[itex]->Release();
        pStagingTextures[itex] = 0;
    }

    pTexture->pMap = 0;
    pTexture       = 0;
    StartMipLevel  = 0;
    LevelCount     = 0;    
}

// ***** TextureManager
TextureManager::TextureManager(ID3D1x(Device)* pdevice,
                               ID3D1x(DeviceContext)* pcontext,
                               ThreadId renderThreadId, 
                               ThreadCommandQueue* commandQueue) : 
    Render::TextureManager(),
    pDevice(pdevice), pDeviceContext(pcontext), 
    pRTCommandQueue(commandQueue)
{
    if (renderThreadId)
        RenderThreadId = renderThreadId;
    else
        RenderThreadId = GetCurrentThreadId();

    initTextureFormats();

    // Precreate the sampler states.
    memset(SamplerStates, 0, sizeof SamplerStates);
    for ( unsigned wrap = 0; wrap < Wrap_Count; wrap++ )
    {
        for ( unsigned filter = 0; filter < Sample_Count; ++filter )
        {
            D3D1x(SAMPLER_DESC) samplerDesc;
            memset(&samplerDesc, 0, sizeof samplerDesc );
            switch( filter << Sample_Shift )
            {
            default:
            case Sample_Point:  samplerDesc.Filter = D3D1x(FILTER_MIN_MAG_MIP_POINT); break;
            case Sample_Linear: samplerDesc.Filter = D3D1x(FILTER_MIN_MAG_MIP_LINEAR); break;
            }

            switch( wrap )
            {
            default:
            case Wrap_Clamp:    samplerDesc.AddressU = samplerDesc.AddressV = D3D1x(TEXTURE_ADDRESS_CLAMP); break;
            case Wrap_Repeat:   samplerDesc.AddressU = samplerDesc.AddressV = D3D1x(TEXTURE_ADDRESS_WRAP); break;
            }
            samplerDesc.AddressW        = D3D1x(TEXTURE_ADDRESS_CLAMP);
            samplerDesc.ComparisonFunc  = D3D1x(COMPARISON_ALWAYS);
            samplerDesc.MaxLOD          = D3D1x(FLOAT32_MAX);
            samplerDesc.MipLODBias      = -0.75f;
            samplerDesc.MaxAnisotropy   = 1;

            ImageFillMode fm((WrapMode)wrap, (SampleMode)(filter << Sample_Shift));
            SF_ASSERT( fm.Fill < SamplerTypeCount );
            HRESULT hr = pDevice->CreateSamplerState( &samplerDesc, &SamplerStates[fm.Fill] );        
            SF_DEBUG_MESSAGE1( FAILED(hr), "Failed CreateSamplerState (hr=0x%x)", hr ); SF_UNUSED(hr);
        }
    }
}

TextureManager::~TextureManager()
{   
    Mutex::Locker lock(&pLocks->TextureMutex);
    Reset();
    pLocks->pManager = 0;
}

void TextureManager::Reset()
{
    Mutex::Locker lock(&pLocks->TextureMutex);

    // InitTextureQueue MUST be empty, or there was a thread
    // service problem.
    SF_ASSERT(TextureInitQueue.IsEmpty());

    // Notify all textures
    while (!Textures.IsEmpty())
        Textures.GetFirst()->LoseManager();

    processTextureKillList();

    // Destroy sampler states.
    for ( unsigned wrap = 0; wrap < Wrap_Count; wrap++ )
    {
        for ( unsigned filter = 0; filter < Sample_Count; ++filter )
        {
            ImageFillMode fm((WrapMode)wrap, (SampleMode)(filter << Sample_Shift));
            SF_ASSERT( fm.Fill < SamplerTypeCount );
            if ( SamplerStates[fm.Fill] )
                SamplerStates[fm.Fill]->Release();
        }
    }
    memset(SamplerStates, 0, sizeof SamplerStates);

    pDevice = 0;
}

void TextureManager::SetSamplerState( unsigned stage, unsigned viewCount, ID3D1x(ShaderResourceView)** views, ID3D1x(SamplerState)* state)
{
	bool loadSamplers = false;
	bool loadTextures = false;
	ID3D1x(SamplerState)* states[16];
	for ( unsigned i = 0; i < viewCount; ++i)
	{
		states[i] = state;
		if ( CurrentSamplers[i+stage] != state )
		{
			loadSamplers = true;
		}

		if ( CurrentTextures[i+stage] != views[i] )
		{
			loadTextures = true;
		}
	}
	if ( loadSamplers )
	{
		pDeviceContext->PSSetSamplers( stage, viewCount, states );
		memcpy(&CurrentSamplers[stage], states, viewCount*sizeof(ID3D1x(SamplerState)*));
	}
	if ( loadTextures )
	{
		pDeviceContext->PSSetShaderResources( stage, viewCount, views );
		memcpy(&CurrentTextures[stage], views, viewCount*sizeof(ID3D1x(ShaderResourceView)*));
	}
}

void TextureManager::BeginScene()
{
    memset(CurrentSamplers, 0, sizeof(CurrentSamplers));
    memset(CurrentTextures, 0, sizeof(CurrentTextures));
    ID3D1x(ShaderResourceView)* views[MaximumStages];
    ID3D1x(SamplerState)* states[MaximumStages];
    memset(views, 0, sizeof(views));
    memset(states, 0, sizeof(states));
    pDeviceContext->PSSetSamplers( 0, MaximumStages, states);
    pDeviceContext->PSSetShaderResources( 0, MaximumStages, views);
}


// ***** D3D1x Format mapping and conversion functions

void SF_STDCALL D3D1x(CopyScanline8_Extend_A8_A8L8)(UByte* pd, const UByte* ps, UPInt size,
                                                  Palette*, void*)
{
    for (UPInt i = 0; i< size; i++, pd+=2, ps++)
    {        
        pd[0] = 255;
        pd[1] = ps[0];
    }
}

void SF_STDCALL D3D1x(CopyScanline8_Extend_A8_A4R4G4B4)(UByte* pd, const UByte* ps, UPInt size,
                                                      Palette*, void*)
{
    for (UPInt i = 0; i< size; i++, pd+=2, ps++)
    {        
        pd[0] = 255;
        pd[1] = ps[0] | 0x0f; // Copy high 4 bits; low bits set to F.
    }
}

void SF_STDCALL D3D1x(CopyScanline16_Retract_A8L8_A8)(UByte* pd, const UByte* ps, UPInt size,
                                                    Palette*, void*)
{
    for (UPInt i = 0; i< size; i++, pd++, ps+=2)
    {        
        pd[0] = ps[1];
    }
}

void SF_STDCALL D3D1x(CopyScanline16_Retract_A4R4G4B4_A8)(UByte* pd, const UByte* ps, UPInt size,
                                        Palette*, void*)
{
    for (UPInt i = 0; i< size; i++, pd++, ps+=2)
    {        
        pd[0] = ps[1] & ~0x0f; // Copy high 4 bits; zero low bits.
    }
}


// Image to Texture format conversion and mapping table,
// organized by the order of preferred image conversions.

TextureFormat::Mapping D3D1xTextureFormatMapping[] = 
{
    // Warning: Different versions of the same ImageFormat must go right after each-other,
    // as initTextureFormats relies on that fact to skip them during detection.
    { Image_R8G8B8A8,   DXGI_FORMAT_R8G8B8A8_UNORM, 4, true,  &Image::CopyScanlineDefault,             &Image::CopyScanlineDefault },
    { Image_B8G8R8A8,   DXGI_FORMAT_B8G8R8A8_UNORM, 4, true,  &Image::CopyScanlineDefault,             &Image::CopyScanlineDefault },
    { Image_R8G8B8,     DXGI_FORMAT_R8G8B8A8_UNORM, 4, true,  &Image_CopyScanline24_Extend_RGB_RGBA,   &Image_CopyScanline32_Retract_RGBA_RGB },
    { Image_B8G8R8,     DXGI_FORMAT_B8G8R8A8_UNORM, 4, true,  &Image_CopyScanline24_Extend_RGB_RGBA,   &Image_CopyScanline32_Retract_RGBA_RGB },
    { Image_A8,         DXGI_FORMAT_A8_UNORM,       1, true,  &Image::CopyScanlineDefault,             &Image::CopyScanlineDefault },

    // Compressed formats.
    { Image_DXT1,       DXGI_FORMAT_BC1_UNORM,      0, true,  &Image::CopyScanlineDefault,             &Image::CopyScanlineDefault},
    { Image_DXT3,       DXGI_FORMAT_BC2_UNORM,      0, true,  &Image::CopyScanlineDefault,             &Image::CopyScanlineDefault},
    { Image_DXT5,       DXGI_FORMAT_BC3_UNORM,      0, true,  &Image::CopyScanlineDefault,             &Image::CopyScanlineDefault},

    // Video formats.
    { Image_Y8_U2_V2,   DXGI_FORMAT_A8_UNORM,       1, true,  &Image::CopyScanlineDefault,             &Image::CopyScanlineDefault },
    { Image_Y8_U2_V2_A8,DXGI_FORMAT_A8_UNORM,       1, true,  &Image::CopyScanlineDefault,             &Image::CopyScanlineDefault },

    { Image_None,       DXGI_FORMAT_UNKNOWN,        0, false, 0,                                       0 }
};


void        TextureManager::initTextureFormats()
{
    TextureFormat::Mapping* pmapping = 0;
    for (pmapping = D3D1xTextureFormatMapping; pmapping->Format != Image_None; pmapping++)
    {
        UINT formatSupport;
        // See if format is supported.        
        if (SUCCEEDED( pDevice->CheckFormatSupport(pmapping->D3DFormat, &formatSupport) ) &&
            formatSupport & D3D1x(FORMAT_SUPPORT_TEXTURE2D) )
        {
            TextureFormat tf = { pmapping, 0 };
            // And now check its capabilities to assign extra Usage.
            if (formatSupport & D3D1x(FORMAT_SUPPORT_MIP_AUTOGEN) )
            {
                // tf.D3DUsage |= D3DUSAGE_AUTOGENMIPMAP;
            }
            TextureFormats.PushBack(tf);

            // If format added, skip additional mappings for it.
            while ((pmapping+1)->Format == pmapping->Format)
                pmapping++;
        }
    }

    // Add Image_None, which should always come last.
    TextureFormat tf = {pmapping, 0};
    TextureFormats.PushBack(tf);
}

const Render::TextureFormat*  TextureManager::getTextureFormat(ImageFormat format) const
{
    for (unsigned i = 0; i< TextureFormats.GetSize(); i++)    
        if (TextureFormats[i].GetImageFormat() == format)
            return (Render::TextureFormat*)&TextureFormats[i];
    return 0;
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
    
    if (pmap->Map(ptexture, pDevice, pDeviceContext, mipLevel, levelCount))
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
    for (unsigned i = 0; i<D3DTexViewKillList.GetSize(); i++)
        D3DTexViewKillList[i]->Release();
    for (unsigned i = 0; i<D3DTextureKillList.GetSize(); i++)
        D3DTextureKillList[i]->Release();
    D3DTexViewKillList.Clear();
    D3DTextureKillList.Clear();
}

void    TextureManager::processInitTextures()
{
    // TextureMutex lock expected externally.
    //Mutex::Locker lock(&TextureMutex);
   
    if (!TextureInitQueue.IsEmpty() || !DepthStencilInitQueue.IsEmpty())
    {
        while (!TextureInitQueue.IsEmpty())
        {
            Texture* ptexture = TextureInitQueue.GetFirst();
            ptexture->RemoveNode();
            ptexture->pPrev = ptexture->pNext = 0;
            if (ptexture->Initialize())
                Textures.PushBack(ptexture);
        } 
        while (!DepthStencilInitQueue.IsEmpty())
        {
            DepthStencilSurface* pdss = DepthStencilInitQueue.GetFirst();
            pdss->RemoveNode();
            pdss->pPrev = pdss->pNext = 0;
            pdss->Initialize();
        }
        pLocks->TextureInitWC.NotifyAll();
    }
}

Render::Texture* TextureManager::CreateTexture(ImageFormat format, unsigned mipLevels,
                                               const ImageSize& size,
                                               unsigned use, ImageBase* pimage,
                                               Render::MemoryManager* allocManager)
{
    SF_UNUSED(allocManager);
    SF_AMP_SCOPE_TIMER(RenderThreadId == GetCurrentThreadId() ? AmpServer::GetInstance().GetDisplayStats() : NULL, __FUNCTION__, Amp_Profile_Level_Medium);
    if (!pDevice)
    {
        SF_DEBUG_ERROR(1, "CreateTexture failed - TextueManager has been Reset");
        return 0;
    }

    TextureFormat* ptformat = (TextureFormat*)precreateTexture(format, use, pimage);
    if ( !ptformat )
        return 0;

    Texture* ptexture = 
        SF_HEAP_AUTO_NEW(this) Texture(pLocks, ptformat, mipLevels, size, use, pimage);
    if (!ptexture)
        return false;
    if (!ptexture->IsValid())
    {
        ptexture->Release();
        return false;
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
    if (use & ImageUse_InitOnly)
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

Render::Texture* TextureManager::CreateTexture(ID3D1x(Texture2D)* pd3dtexture, ImageSize imgSize, ImageBase* image )
{
    if ( pd3dtexture == 0 )
        return 0;

    Texture* ptexture = SF_HEAP_AUTO_NEW(this) Texture(pLocks, pd3dtexture, imgSize, image);
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

unsigned TextureManager::GetTextureUseCaps(ImageFormat format)
{
    // ImageUse_InitOnly (ImageUse_NoDataLoss alias) ok while textures are Managed    
    unsigned use = ImageUse_InitOnly | ImageUse_Update;
    if (!ImageData::IsFormatCompressed(format))
        use |= ImageUse_PartialUpdate | ImageUse_GenMipmaps;

    const Render::TextureFormat* ptformat = getTextureFormat(format);
    if (!ptformat)
        return 0;
    if (isMappable(ptformat))
        use |= ImageUse_MapRenderThread;
    return use;   
}

Render::DepthStencilSurface* TextureManager::CreateDepthStencilSurface(const ImageSize& size, MemoryManager* manager)
{
    SF_UNUSED(manager);
    if (!pDevice)
    {
        SF_DEBUG_ERROR(1, "CreateDepthStencilSurface failed - TextueManager has been Reset");
        return 0;
    }

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

Render::DepthStencilSurface* TextureManager::CreateDepthStencilSurface(ID3D1x(Texture2D)* psurface)
{
    if ( !psurface ) 
        return 0;

    psurface->AddRef();
    D3D1x(TEXTURE2D_DESC) desc;
    psurface->GetDesc(&desc);
    DepthStencilSurface* pdss = SF_HEAP_AUTO_NEW(this) DepthStencilSurface(pLocks, ImageSize(desc.Width, desc.Height) );
    pdss->pDepthStencilSurface = psurface;
    pdss->State = Texture::State_Valid;
    return pdss;
}

}}};  // namespace Scaleform::Render::D3D1x

