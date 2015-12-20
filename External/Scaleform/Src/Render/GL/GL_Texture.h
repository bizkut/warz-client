/**************************************************************************

Filename    :   GL_Texture.h
Content     :   GL Texture and TextureManager header
Created     :   
Authors     :   

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_SF_Render_GL_Texture_H
#define INC_SF_Render_GL_Texture_H

#include "Render/GL/GL_Common.h"

#include "Kernel/SF_List.h"
#include "Kernel/SF_Threads.h"
#include "Render/Render_Image.h"
#include "Render/Render_MemoryManager.h"
#include "Render/Render_ThreadCommandQueue.h"
#include "Kernel/SF_HeapNew.h"

namespace Scaleform { namespace Render { namespace GL {


// TextureFormat describes format of the texture and its caps.
// Format includes allowed usage capabilities and ImageFormat
// from which texture is supposed to be initialized.

struct TextureFormat
{
    ImageFormat              Format, ConvFormat;
    GLenum                   GLColors, GLFormat, GLData;
    UByte                    BytesPerPixel;
    bool                     Mappable;
    UByte                    DrawModes;
    const char*              Extension;
    Image::CopyScanlineFunc  CopyFunc;
    Image::CopyScanlineFunc  UncopyFunc;

    ImageFormat     GetImageFormat() const { return Format; }

    unsigned        GetPlaneCount() const
    { return ImageData::GetFormatPlaneCount(GetImageFormat()); }
};

class MappedTexture;
class TextureManager;
class HAL;


// GL Texture class implementation; it man actually include several HW 
// textures (one for each ImageFormat plane).

class Texture : public Render::Texture
{
public:
    // Bits stored in TextureFlags.
    enum TextureFlagBits
    {
        TF_Rescale     = 0x01,
        TF_SWMipGen    = 0x02,
        TF_UserAlloc   = 0x04,
        TF_DoNotDelete = 0x08,
    };

    Ptr<RawImage>            pBackingImage;
    const TextureFormat*     pFormat;

    // If texture is currently mapped, it is here.
    MappedTexture*          pMap;

    struct HWTextureDesc
    {
        ImageSize           Size;
        GLuint              TexId;
    };

    // TextureDesc array is allocated if more then one is needed.
    HWTextureDesc*          pTextures;
    HWTextureDesc           Texture0;

    GLint                   LastMinFilter, LastAddress;

    Texture(TextureManagerLocks* pmanagerLocks, const TextureFormat* pformat, unsigned mipLevels,
            const ImageSize& size, unsigned use, ImageBase* pimage);
    Texture(TextureManagerLocks* pmanagerLocks, GLuint texID, bool deleteTexture,
            const ImageSize& size, ImageBase* pimage);
    ~Texture();

    ImageFormat             GetImageFormat() const { return pFormat->Format; }
    TextureManager*         GetManager() const     { return (TextureManager*)pManagerLocks->pManager; }
    inline  HAL*            GetHAL() const;
    bool                    IsValid() const        { return pTextures != 0; }

    void                    LoseManager();
    void                    LoseTextureData();
    void                    RestoreAfterLoss();
    bool                    Initialize();
    bool                    Initialize(GLuint texID);
    void                    ReleaseHWTextures(bool staging = true);

    // *** Interface implementation

    //    virtual HAL*      GetRenderHAL() const { SF_ASSERT(0); return 0; } // TBD
    virtual Image*          GetImage() const
        { SF_ASSERT(!pImage || (pImage->GetImageType() != Image::Type_ImageBase)); return (Image*)pImage; }
    virtual ImageFormat     GetFormat() const         { return GetImageFormat(); }

    virtual bool            Map(ImageData* pdata, unsigned mipLevel, unsigned levelCount);
    virtual bool            Unmap();

    virtual bool            UpdateRenderTargetData(Render::RenderTargetData* prtData, Render::HAL* =0);
    virtual bool            UpdateStagingData(Render::RenderTargetData* prtData);

    virtual bool            Update(const UpdateDesc* updates, unsigned count = 1, unsigned mipLevel = 0);    
    virtual bool            Update();

    virtual bool            Upload(unsigned itex, unsigned level, const ImagePlane& plane);
    virtual void            MakeMappable();

    // Copies the image data from the hardware.
    SF_AMP_CODE( virtual bool Copy(ImageData* pdata); )
};

// GL DepthStencilSurface implementation. 
class DepthStencilSurface : public Render::DepthStencilSurface
{
public:
    DepthStencilSurface(TextureManagerLocks* pmanagerLocks, const ImageSize& size);
    ~DepthStencilSurface();

    virtual ImageSize               GetSize() const { return Size; }
    bool                            Initialize();
    inline HAL*                     GetHAL() const;

    ImageSize                 Size;
    GLuint                    RenderBufferID;
    Texture::CreateState      State;

    // We can't query ahead of time which stencil format is supported. So, we have to attempt
    // creating them, and if they fail, try the next format. Returns false once there are no
    // more formats to try.
    static bool               SetNextGLFormatIndex();

    // Returns true of the current format has a packed depth component (and is not just stencil data).
    static bool               CurrentFormatHasDepth();

    // The list of GL formats we will try to use to allocate stencil buffers (in order of preference).
    static int                GLFormatIndex;
    static unsigned           GLStencilFormats[];
};

// MappedTexture object repents a Texture mapped into memory with Texture::Map() call;
// it is also used internally during updates.
// The key part of this class is the Data object, stored Locked texture level plains.

class MappedTexture : public NewOverrideBase<StatRender_TextureManager_Mem>
{
    friend class Texture;

    Texture*      pTexture;
    // We support mapping sub-range of levels, in which case
    // StartMipLevel may be non-zero.
    unsigned      StartMipLevel;
    int           LevelCount;
    // Pointer data that can be copied to.
    ImageData     Data;

    enum { PlaneReserveSize = 4 };
    ImagePlane    Planes[PlaneReserveSize];

public:
    MappedTexture()
        : pTexture(0), StartMipLevel(false), LevelCount(0) { }
    ~MappedTexture()
    {
        SF_ASSERT(!IsMapped());
    }

    bool        Reserve()  { AtomicOps<int>::CompareAndSet_Sync(&LevelCount, 0, -1); return LevelCount == -1; }
    bool        IsMapped() { return (LevelCount > 0); }
    bool        Map(Texture* ptexture, unsigned mipLevel, unsigned levelCount);
    void        Unmap();
};


// GL Texture Manger.
// This class is responsible for creating textures and keeping track of them
// in the list.
// 

class TextureManager : public Render::TextureManager
{
    friend class Texture;
    friend class DepthStencilSurface;

    typedef ArrayConstPolicy<8, 8, false>   KillListArrayPolicy;
    typedef ArrayLH<GLuint,
        StatRender_TextureManager_Mem,
        KillListArrayPolicy>    GLTextureArray;
    typedef List<Texture, Render::Texture>  TextureList;
    typedef List<DepthStencilSurface, Render::DepthStencilSurface> DepthStencilList;

    enum TextureCaps
    {
        TC_NonPower2Limited = 1,
        TC_NonPower2Full    = 2,
#if defined(SF_USE_GLES) || defined(GL_ES_VERSION_2_0)
            TC_UseBgra          = 4,
#endif
    };

    MappedTexture       MappedTexture0;

    HAL*                     pHal;
    unsigned                 Caps;
    // Lists protected by TextureManagerLocks::TextureMutex.
    TextureList               Textures;
    TextureList               TextureInitQueue;
    DepthStencilList          DepthStencilInitQueue;
    GLTextureArray            GLTextureKillList;
    GLTextureArray            GLDepthStencilKillList;
    GLTextureArray            GLFrameBufferKillList;

    ThreadId                  RenderThreadId;
    ThreadCommandQueue*       pRTCommandQueue;    

    // Texture format table, organized by supported HW features.
    ArrayLH<TextureFormat*>  TextureFormats;

    // Detecting redundant texture bindings.
    static const int MaximumStages = 4;
    GLint                    CurrentTextures[MaximumStages];

    HAL* GetHAL() const { return pHal; }

    const Render::TextureFormat* getTextureFormat(ImageFormat format) const;
    static GLenum getBaseTextureFormatFromInternal(GLenum intfmt);
    virtual bool    isMappable(const Render::TextureFormat* ptformat) { return ((TextureFormat*)ptformat)->Mappable; }

    // Texture Memory-mapping support.
    MappedTexture*  mapTexture(Texture* p, unsigned mipLevel, unsigned levelCount);
    MappedTexture*  mapTexture(Texture* p) { return mapTexture(p, 0, p->MipLevels); }
    void            unmapTexture(Texture *ptexture);    

    virtual void    processInitTextures();
    virtual void    processTextureKillList();

public:
    TextureManager(ThreadId renderThreadId, ThreadCommandQueue* commandQueue);
    ~TextureManager();

    // XXX - use Extensions instead if needed
    void            Initialize(HAL* phal);
    void            NotifyLostContext();
    void            RestoreAfterLoss();

    // Applies a texture to a stage.
    void            ApplyTexture(unsigned stageIndex, GLint texture);

    // Adds a FrameBuffer to the FBO kill list, or deletes it immediately if in RT.
    void            DestroyFBO(GLuint fboid);

    // *** TextureManager
    virtual unsigned         GetTextureFormatSupport() const;

    virtual Render::Texture* CreateTexture(ImageFormat format, unsigned mipLevels,
                                           const ImageSize& size, unsigned use, 
                                           ImageBase* pimage = 0,
                                           Render::MemoryManager* manager = 0);
    virtual Render::Texture* CreateTexture(GLuint texID, bool deleteTexture, 
                                           ImageSize imgSize = ImageSize(0),
                                           ImageBase* pimage = 0);
    virtual Render::DepthStencilSurface* CreateDepthStencilSurface(const ImageSize& size,
                                                           MemoryManager* manager = 0);
    virtual Render::DepthStencilSurface* CreateDepthStencilSurface(GLuint id);

	virtual bool	IsDrawableImageFormat(ImageFormat format) const { return (format == Image_B8G8R8A8) || (format == Image_R8G8B8A8); }

	bool            IsMultiThreaded() { return false; }
    unsigned        GetTextureUseCaps(ImageFormat format);
    bool            IsNonPow2Supported(ImageFormat format, UInt16 use);

    bool            CanCreateTextureCurrentThread();

    // Should be called before each frame on RenderThread.
    virtual void    BeginFrame( );
};

GL::HAL* Texture::GetHAL() const
{
    return ((GL::TextureManager*)pManagerLocks->pManager)->pHal;
}

GL::HAL* DepthStencilSurface::GetHAL() const
{
    return ((GL::TextureManager*)pManagerLocks->pManager)->pHal;
}

}}};  // namespace Scaleform::Render::GL

#endif
