/**************************************************************************

PublicHeader:   Render
Filename    :   Render_DrawableImage_Queue.cpp
Content     :   Commands and command queue implementation for DrawableImage
Created     :   January 2012
Authors     :   Michael Antonov

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_SF_Render_DrawableImage_Queue_H
#define INC_SF_Render_DrawableImage_Queue_H


#include "Render_DrawableImage.h"
#include "Render_Context.h"
#include "Kernel/SF_HeapNew.h"

namespace Scaleform { namespace Render {

// Helper functions.
bool MapImageSource(ImageData* data, ImageBase* i, bool rtMap = true);
bool IsCPURenderableSource(Image* i);

// 'RTTI' enum for DICommand types. This is used when querying the HAL about which
// DICommands it supports; it may be that a HAL partially supports a command, and thus
// it will need to inspect the DICommand object (and thus cast it to the appropriate type).
enum DICommandType
{
    // These commands *must* be supported by the GPU.
    DICommandType_Map,
    DICommandType_Unmap,
    DICommandType_CreateTexture,

    DICommandType_Clear,
    DICommandType_ApplyFilter,      // No CPU implementation possible.
    DICommandType_Draw,             // No CPU implementation possible.
    DICommandType_CopyChannel,
    DICommandType_CopyPixels,
    DICommandType_ColorTransform,
    DICommandType_Compare,
    DICommandType_FillRect,
    DICommandType_FloodFill,
    DICommandType_Merge,
    DICommandType_Noise,
    DICommandType_PaletteMap,
    DICommandType_PerlinNoise,
    DICommandType_Scroll,
    DICommandType_Threshold,
    DICommandType_Count
};


//--------------------------------------------------------------------
// DrawableImage Command and CommandQueue implementations

struct DICommandContext
{ 
    Renderer2D*     pR2D;
    Render::HAL*    pHAL;

    // Constructor for SWF rendering (no HAL)
    DICommandContext() : pR2D(0), pHAL(0)
    { }
    // Constructor for HW rendering
    DICommandContext(ThreadCommandQueue* rtQueue)
    {
        Interfaces i;
        if ( !rtQueue )
            return;
        rtQueue->GetRenderInterfaces(&i);
        pR2D = i.pRenderer2D;
        pHAL = i.pHAL;
    }
};



struct DISourceImages
{
    static const unsigned MaximumSources = 2;
    Image* pImages[MaximumSources];

    DISourceImages()        
    { 
        memset(pImages, 0, sizeof(pImages));
    }

    Image* operator [] (int i) const 
    { 
        SF_DEBUG_ASSERT(i < MaximumSources, "OOB image access.");
        return pImages[i]; 
    }
};


// DICommand is a base command for DrawableImage commands that are
// queued up for the render thread.
struct DICommand
{
    friend class DICommandSet;

    enum RenderCaps
    {
        RC_CPU           = 0x01,  // Command is executable on the CPU.
        RC_GPU           = 0x02,  // Command is executable on the GPU.
        RC_GPUPreference = 0x04,  // GPU execution is preferred but not necessary (prevents immediate CPU execution even when image is mapped).
        RC_GPU_NoRT      = 0x08,  // GPU command does not require the Image's RenderTarget to be set (eg. for Create/Map/Unmap).
    };

    DICommand(DrawableImage* image = 0) : pImage(image) { }

    virtual ~DICommand() { }
    
    virtual DICommandType GetType() const = 0;
    virtual unsigned HasCPUImplementation() const = 0;
    virtual unsigned GetRenderCaps() const;
    virtual unsigned GetSize() const = 0;

    // Fills in SourceImages data struncture; returns the number
    // of images used.
    virtual unsigned GetSourceImages(DISourceImages*) const { return 0; }

    // Returns 'true' if CPU command has no sources and/or if sources are
    // mapped/mappable. Dest must also be mapped.
    bool             IsCPURenderableOnMainThread() const;
    static bool      IsCPURenderableSource(Image* i);

    bool             ExecuteSWOnAddCommand(DrawableImage* i) const;

    // RenderThread execution function; does proper map/unmap and dispatches
    // to either ExecuteHW or ExecuteSW. 
    void             ExecuteRT(DICommandContext& context) const;


    virtual void     ExecuteHW(DICommandContext&) const { }
    virtual void     ExecuteSW(DICommandContext& context,
                               ImageData& dest, ImageData** psrc = 0) const
    { SF_UNUSED3(context, dest, psrc); }

protected:

    bool     executeSWHelper(DICommandContext& context,
                             DrawableImage* i,
                             DISourceImages& images,
                             unsigned imageCount,
                             bool rtMap = true) const;

    bool     executeHWHelper(DICommandContext& context,
                             DrawableImage* i) const;

    Ptr<DrawableImage> pImage;
};
template <class D, class B = DICommand>
struct DICommandImpl : public B
{
    DICommandImpl() {}
    DICommandImpl(DrawableImage* image) : B(image) { }
    
    DICommandImpl(const DICommandImpl& other) : B(other) { }
    virtual unsigned GetSize() const { return sizeof(D); }
};


//--------------------------------------------------------------------

// DIQueuePage represents a block of memory that can hold multiple DICommands,
// allocated and executed consecutively.
struct DIQueuePage : public ListNode<DIQueuePage>, public NewOverrideBase<Stat_Default_Mem>
{
    enum { BufferSize = 512 - 16 };
    UByte        Buffer[BufferSize];
    // Allocation offset in page.
    unsigned     Offset;
    unsigned     CaptureFrameId;

    DIQueuePage() : Offset(0), CaptureFrameId(0) { }

    DICommand* getFirst()
    {
        return Offset ? ((DICommand*)Buffer) : 0;
    }
    DICommand* getNext(DICommand* prevCommand)
    {
        SF_ASSERT((prevCommand >= (DICommand*)Buffer) &&
                  (prevCommand < (DICommand*)(Buffer + Offset)));
        UPInt   commandSize = prevCommand->GetSize();
        UByte*  next = ((UByte*)prevCommand) + commandSize;
        if ((UByte*)next >= (Buffer + Offset))
            return 0;
        return (DICommand*)next;
    }

    DICommand* destroyAndGetNext(DICommand* prevCommand)
    {
        DICommand* next = getNext(prevCommand);
        Destruct(prevCommand);
        return next;
    }


    void* AllocSize(unsigned size)
    {
        if (size > (BufferSize-Offset))
            return 0;
        UByte* data = Buffer + Offset;
        Offset += size;
        return data;
    }

    bool HasFreeSize(unsigned size)
    {
        return (size <= (BufferSize-Offset));
    }
};

enum DICommandSetType
{
    DICommand_All,
    DICommand_Displaying
};


// DICommandSet represents a set of commands obtained from DICommandQueue.
class DICommandSet
{
    friend class DICommandQueue;
public:

    DICommandSet(DICommandQueue* queue)
        : pQueue(queue)
    { }
    ~DICommandSet()
    {
        // All obtained commands should've been executed.
        SF_ASSERT(QueueList.IsEmpty());
    }

    void ExecuteCommandsRT(DICommandContext& context);
    void ExecuteCommandsMT(DICommandContext& context);

    bool IsEmpty() const { return QueueList.IsEmpty(); }

private:
    DICommandQueue*     pQueue;
    List<DIQueuePage>   QueueList;
};



// DICommandQueue is ref-counted because it can be shared among drawable images.
// Such sharing is done by merging queues when one DrawableImage is copied into
// another.


class DICommandQueue  : public RefCountBase<DICommandQueue, Stat_Default_Mem>,
                        public ListNode<DICommandQueue>
{
    friend class DICommandSet;
    friend struct DICommand;

    enum {
        FreePageLimit  = 3,
        AllocPageLimit = 16
    };

public:
    DICommandQueue(DrawableImageContext* dicontext);
    ~DICommandQueue();

    // We keep a separate queue for each snapshot state
    enum DIQueueType 
    {
        DIQueue_Active,
        DIQueue_Captured,
        DIQueue_Displaying,
        DIQueue_Free, // Small free list
        DIQueue_Item_Count
    };


    
    template<class C>
    bool AddCommand_NTS(const C& src)
    {
        SF_COMPILER_ASSERT(sizeof(C) < DIQueuePage::BufferSize);
        void* buffer = allocCommandFromPage(sizeof(C), &QueueLock);
        if (buffer)
        {
            Construct(buffer, src);
            return true;
        }
        return false;
    }

    template<class C>
    bool AddCommand(const C& src)
    {
        Lock::Locker lock(&QueueLock);
        return AddCommand_NTS(src);
    }

    // Mimics interface of ContextCaptureNotify; DrawableImageContext is the real notify, these
    // are called in turn by the context, to ensure proper ordering.
    void OnCapture();
    void OnNextCapture(ContextImpl::RenderNotify* notify);
    void OnShutdown();


    void ExecuteNextCapture(ContextImpl::RenderNotify* notify);
    void ExecuteCommandsAndWait()
    {
        // ExecuteCmd/Queue may not be reference counted by the ThreadCommandQueue, so protect them
        // against its deletion until after it executes here by adding extra refs.
        AddRef();
        ExecuteCmd->AddRef();

        pRTCommandQueue->PushThreadCommand(ExecuteCmd);
        ExecuteCmd->WaitDoneAndReset();
    }   

    class ExecuteCommand : public ThreadCommand
    {
        DICommandQueue* pQueue;
        Event           ExecuteDone;
    public:
        ExecuteCommand(DICommandQueue* queue) : pQueue(queue)
        {
        }

        virtual void Execute();

        void   WaitDoneAndReset()
        {
            ExecuteDone.Wait();
            ExecuteDone.ResetEvent();
        }
    };

protected:
    friend class ExecuteCommand;
    friend class DrawableImage;

    void*           allocCommandFromPage(unsigned size, Lock* locked);
    DIQueuePage*    allocPage();
    void            freePage(DIQueuePage* page);
    void            popCommandSet(DICommandSet* cmdSet, DICommandSetType type);
    bool            isQueueListCPURenderable(List<DIQueuePage>& queue) const;
    bool            isQueueEmpty_NTS() const;
    bool            isCPURenderable_NTS();
    DICommandQueue* getThis() { return this; }

    void            updateCPUModifiedImagesRT();
    void            updateGPUModifiedImagesRT();

    void lockCommandSetAndWait(DICommandSet** pcommands)
    {
        CommandSetMutex.DoLock();
        while(*pcommands)
            CommandSetWC.Wait(&CommandSetMutex);
    }
    void unlockCommandSet()
    {
        CommandSetMutex.Unlock();
    }
    void notifyCommandSetFinished(DICommandSet** pcommands)
    {
        Mutex::Locker mlock(&CommandSetMutex);
        *pcommands = 0;
        CommandSetWC.NotifyAll();
    }

    Mutex               CommandSetMutex;
    WaitCondition       CommandSetWC;
    DICommandSet*       pRTCommands;        // Render thread command in progress.
    DICommandSet*       pATCommands;        // Advance thread command in progress.

    // Keep a list of images relying on this queue.
    List<DrawableImage> ImageList;

    Ptr<DrawableImageContext>   pDIContext;

    // Drawable image update lists. One tracks images that were modified by the CPU, and thus
    // will require the GPU data to be updated, the other is the reverse, GPU modified data that
    // will need to be propagated to the CPU.
    Ptr<DrawableImage>      pCPUModifiedImageList;
    Ptr<DrawableImage>      pGPUModifiedImageList;

    ThreadCommandQueue* pRTCommandQueue;
    Lock                QueueLock;
    
    Ptr<ExecuteCommand> ExecuteCmd;

    List<DIQueuePage> Queues[DIQueue_Item_Count];
    unsigned          CaptureFrameId;
    unsigned          FreePageCount;
    unsigned          AllocPageCount;
};



//--------------------------------------------------------------------
// ***** Commands
//--------------------------------------------------------------------

struct DICommand_Map : public DICommandImpl<DICommand_Map>
{
    bool ReadOnlyMapping;   // Map the image with the read-only capabilities.
	bool ForceRemapping;	// Re-map the image after updating the staging target.

    DICommand_Map(DrawableImage* image, bool readOnly, bool forced = false) : 
        DICommandImpl<DICommand_Map>(image),
        ReadOnlyMapping(readOnly),
		ForceRemapping(forced)
    { }

    virtual DICommandType GetType() const { return DICommandType_Map; }
    virtual unsigned HasCPUImplementation() const { return false; }
    virtual void ExecuteHW(DICommandContext&) const
    {
        pImage->mapTextureRT(ReadOnlyMapping, ForceRemapping);
    }
};

struct DICommand_Unmap : public DICommandImpl<DICommand_Unmap>
{
    // ModifyRect reports the modified rectangle. If empty,
    // no modification took place so no upload is necessary.
    Rect<SInt32> ModifyRect;
    
    DICommand_Unmap(DrawableImage* image, const Rect<SInt32>& modifyRect)
        : DICommandImpl<DICommand_Unmap>(image), ModifyRect(modifyRect)
    { }

    virtual DICommandType GetType() const { return DICommandType_Unmap; }
    virtual unsigned HasCPUImplementation() const { return false; }
    virtual void ExecuteHW(DICommandContext&) const
    {
        // TODOBM: Provide a way to specify update rectangle
        pImage->unmapTextureRT();
    }
};



// CreateTexture command is emitted to the thread queue to initialize texture
// in constructor.

struct DICommand_CreateTexture : public DICommandImpl<DICommand_CreateTexture>
{    
    DICommand_CreateTexture(DrawableImage* image)
        : DICommandImpl<DICommand_CreateTexture>(image)
    { }
    DICommand_CreateTexture(const DICommand_CreateTexture& other)
        : DICommandImpl<DICommand_CreateTexture>(other.pImage)
    { }

    virtual DICommandType GetType() const { return DICommandType_CreateTexture; }
    virtual unsigned HasCPUImplementation() const { return false; }

    virtual void ExecuteHW(DICommandContext& context) const;
};




struct DICommand_Clear : public DICommandImpl<DICommand_Clear>
{    
    Color FillColor;

    DICommand_Clear(DrawableImage* image, Color color)
        : DICommandImpl<DICommand_Clear>(image), FillColor(color)
    { }
    DICommand_Clear(const DICommand_Clear& other)
        : DICommandImpl<DICommand_Clear>(other.pImage), FillColor(other.FillColor)
    { }

    virtual DICommandType GetType() const { return DICommandType_Clear; }
    virtual unsigned HasCPUImplementation() const { return true; }

    virtual void ExecuteSW(DICommandContext& context,
        ImageData& dest, ImageData** src = 0) const;

    virtual void ExecuteHW(DICommandContext&) const;
};



struct DICommand_SourceRect : public DICommand
{
    Ptr<DrawableImage>  pSource;
    Rect<SInt32>        SourceRect;
    Point<SInt32>       DestPoint;    
	Rect<SInt32>		ClippedSourceRect;
  
    DICommand_SourceRect(DrawableImage* image, DrawableImage* source,
                         const Rect<SInt32>& sr, const Point<SInt32>& dp)
        : DICommand(image), pSource(source), SourceRect(sr), DestPoint(dp),
		  ClippedSourceRect(sr)
    {
		UpdateClippedRects();
	}
    DICommand_SourceRect(const DICommand_SourceRect& other)
        : DICommand(other.pImage), pSource(other.pSource),
          SourceRect(other.SourceRect), DestPoint(other.DestPoint),
		  ClippedSourceRect(other.SourceRect)
    {
		UpdateClippedRects();
	}

	virtual void UpdateClippedRects()
	{
		ImageSize sz = pSource->GetSize();
		SInt32 tlx = SourceRect.TopLeft().x;
		SInt32 tly = SourceRect.TopLeft().y;
		SInt32 brx = SourceRect.BottomRight().x;
		SInt32 bry = SourceRect.BottomRight().y;

		if(SourceRect.Width() < 0)
		{
			SInt32 temp = tlx;
			tlx = brx;
			brx = temp;
		}

		if(SourceRect.Height() < 0)
		{
			SInt32 temp = tly;
			tly = bry;
			bry = temp;
		}

		tlx = ((tlx < 0) ? 0 : tlx);
		tly = ((tly < 0) ? 0 : tly);
		brx = ((brx > (SInt32)sz.Width) ? sz.Width : brx);
		bry = ((bry > (SInt32)sz.Height) ? sz.Height : bry);

		ClippedSourceRect.SetTopLeft(Point<SInt32>(tlx, tly));
		ClippedSourceRect.SetBottomRight(Point<SInt32>(brx, bry));
	}

    virtual unsigned GetSourceImages(DISourceImages* ps) const
    {
        ps->pImages[0] = pSource;
        return 1;
    }

    // Calculates the destination clipped rectangle. Used for software implementations, to determine the
    // area on the destination which should be overwritten, based on the SourceRect size, DestPoint and
    // dimensions of both images.
    bool CalculateDestClippedRect( const ImageData &src, ImageData &dest, const Rect<SInt32>& srcRect,
        Rect<SInt32> &dstClippedRect, Point<SInt32> &delta ) const;
};

template<class D>
struct DICommand_SourceRectImpl : public DICommand_SourceRect
{
    DICommand_SourceRectImpl(DrawableImage* image, DrawableImage* source,
                             const Rect<SInt32>& sr, const Point<SInt32>& dp)
        : DICommand_SourceRect(image, source, sr, dp)
    { }
    DICommand_SourceRectImpl(const DICommand_SourceRectImpl& other)
        : DICommand_SourceRect(other)
    { }

    virtual unsigned GetSize() const { return sizeof(D); }

    virtual void ExecuteHW(DICommandContext& context) const;

    // Helper function to fill out an array of images, and their offsets for texturing (in pixels). If
    // the destination image is required, it should be placed in slot 0.
    virtual void ExecuteHWGetImages( DrawableImage** images, Size<float>* readOffsets) const
    {
        unsigned index = 0;
        if ( GetRequireSourceRead() )
        {
            images[index] = pImage;
            readOffsets[index] = Size<float>((float)DestPoint.x, (float)DestPoint.y);
            index++;
        }
        images[index] = pSource;
        readOffsets[index] = Size<float>((float)SourceRect.TopLeft().x, (float)SourceRect.TopLeft().y);
    }

    static const unsigned MaximumDrawableSources = 3;

    // Peforms the actual operation, based on the filter type.
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const 
    { SF_UNUSED3(context, tex, texgen); };

    // Returns whether this command will require reading from the destination's source data (in HW commands). 
    // This happens always for certain commands (eg. CopyChannel, as it will need to always read the other
    // channels of the destination image), never for certain commands (CopyPixels, as it just blends
    // on top of the destination).
    virtual bool GetRequireSourceRead() const { return false; };
};

struct DICommand_ApplyFilter : public DICommand_SourceRectImpl<DICommand_ApplyFilter>
{
    Ptr<Filter> pFilter;
    
    DICommand_ApplyFilter(DrawableImage* image, DrawableImage* source,
                          const Rect<SInt32>& sr, const Point<SInt32>& dp,
                          Filter* filter)
        : DICommand_SourceRectImpl<DICommand_ApplyFilter>(image, source, sr, dp), pFilter(filter)
    { }
    DICommand_ApplyFilter(const DICommand_ApplyFilter& other)
        : DICommand_SourceRectImpl<DICommand_ApplyFilter>(other), pFilter(other.pFilter)
    { }

    virtual DICommandType GetType() const { return DICommandType_ApplyFilter; }
    virtual unsigned HasCPUImplementation() const { return false; }

    virtual void ExecuteHWGetImages( DrawableImage** images, Size<float>* readOffsets) const;
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen) const;

    // Filters should always generate their own RenderTargets, so reporting no-read is valid.
    virtual bool GetRequireSourceRead() const { return false; };
};

struct DICommand_Draw : public DICommandImpl<DICommand_Draw>
{
    TreeRoot*     pRoot;
    Rect<SInt32>  ClipRect;
    bool          HasClipRect;

    DICommand_Draw(DrawableImage* image, TreeRoot* root, const Rect<SInt32>* clipRect)
        : DICommandImpl<DICommand_Draw>(image),
          pRoot(root), HasClipRect(clipRect ? true : false)
    {
        if (clipRect)
            ClipRect = *clipRect;
    }
    DICommand_Draw(const DICommand_Draw& other)
        : DICommandImpl<DICommand_Draw>(other.pImage),
          pRoot(other.pRoot), ClipRect(other.ClipRect), HasClipRect(other.HasClipRect)
    { }

    virtual DICommandType GetType() const { return DICommandType_Draw; }
    virtual unsigned HasCPUImplementation() const { return false; }

    virtual void ExecuteHW(DICommandContext&) const;
};





struct DICommand_CopyChannel : public DICommand_SourceRectImpl<DICommand_CopyChannel>
{
    typedef DrawableImage::ChannelBits ChannelBits;
    ChannelBits SourceChannel;
    ChannelBits DestChannel;

    DICommand_CopyChannel(DrawableImage* image, DrawableImage* source,
                          const Rect<SInt32>& sr, const Point<SInt32>& dp,
                          ChannelBits sourceChannel, ChannelBits destChannel)
    : DICommand_SourceRectImpl<DICommand_CopyChannel>(image, source, sr, dp),
      SourceChannel(sourceChannel), DestChannel(destChannel)
    {

    }

    virtual DICommandType GetType() const { return DICommandType_CopyChannel; }
    virtual unsigned HasCPUImplementation() const { return true; }

    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const;
    virtual bool GetRequireSourceRead() const { return true; };

    virtual void ExecuteSW(DICommandContext& context,
                           ImageData& dest, ImageData** src = 0) const;
};


struct DICommand_CopyPixels : public DICommand_SourceRectImpl<DICommand_CopyPixels>
{
    Ptr<DrawableImage>  pAlphaSource;
    Point<SInt32>       AlphaPoint;
    bool                MergeAlpha;

    DICommand_CopyPixels(DrawableImage* image, DrawableImage* source,
                         const Rect<SInt32>& sourceRect, const Point<SInt32>& destPoint,
                         DrawableImage* alphaSource, const Point<SInt32>* alphaPoint,
                         bool mergeAlpha)
    : DICommand_SourceRectImpl<DICommand_CopyPixels>(image, source, sourceRect, destPoint),
      pAlphaSource(alphaSource),
      AlphaPoint(alphaPoint ? *alphaPoint : Point<SInt32>(0)),
      MergeAlpha(mergeAlpha)
    { }

    DICommand_CopyPixels(const DICommand_CopyPixels& other)
    : DICommand_SourceRectImpl<DICommand_CopyPixels>(other),
        pAlphaSource(other.pAlphaSource),
        AlphaPoint(other.AlphaPoint),
        MergeAlpha(other.MergeAlpha)
    { }

    virtual DICommandType GetType() const { return DICommandType_CopyPixels; }
    virtual unsigned HasCPUImplementation() const { return true; }

    virtual void Execute(DICommandContext&) const { }

    // CopyPixels requires an overriden ExecuteHWGetImages, because it has an additional alphasource image.
    virtual void ExecuteHWGetImages( DrawableImage** images, Size<float>* readOffsets) const;
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const;
    virtual bool GetRequireSourceRead() const { return true; };

    virtual void ExecuteSW(DICommandContext& context,
                           ImageData& dest, ImageData** src = 0) const;

    virtual unsigned GetSourceImages(DISourceImages* ps) const
    {
        unsigned count = 1;
        ps->pImages[0] = pSource;
        if (pAlphaSource)
        {
            ps->pImages[1] = pAlphaSource;
            count++;
        }
        return count;
    }

};

struct DICommand_ColorTransform : public DICommand_SourceRectImpl<DICommand_ColorTransform>
{
    Cxform       Cx;

    DICommand_ColorTransform(DrawableImage* image, const Rect<SInt32>& rect, const Cxform& cxform)
        : DICommand_SourceRectImpl<DICommand_ColorTransform>(image, image, rect, rect.TopLeft()),
          Cx(cxform)
    { }
    DICommand_ColorTransform(const DICommand_ColorTransform& other)
        : DICommand_SourceRectImpl<DICommand_ColorTransform>(other),
          Cx(other.Cx)
    { }

    virtual DICommandType GetType() const { return DICommandType_ColorTransform; }
    virtual unsigned HasCPUImplementation() const { return true; }
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const;
    virtual bool GetRequireSourceRead() const { return true; };
	virtual void ExecuteSW(DICommandContext& context, ImageData& dest, ImageData** src = 0) const;
};

struct DICommand_Compare : public DICommand_SourceRectImpl<DICommand_Compare>
{
    Ptr<DrawableImage> pImageCompare1;

    DICommand_Compare(DrawableImage* image, DrawableImage* cmp0, DrawableImage* cmp1) :
        DICommand_SourceRectImpl<DICommand_Compare>(image, cmp0, Rect<SInt32>(image->GetSize()), Point<SInt32>(0,0) ),
        pImageCompare1(cmp1)
    { }

    DICommand_Compare(const DICommand_Compare& other) :
        DICommand_SourceRectImpl<DICommand_Compare>(other),
        pImageCompare1(other.pImageCompare1)
    { }

    virtual unsigned GetSourceImages(DISourceImages* ps) const
    {
        ps->pImages[0] = pSource;
        ps->pImages[1] = pImageCompare1;
        return 2;
    }
    virtual DICommandType GetType() const { return DICommandType_Compare; }
    virtual unsigned HasCPUImplementation() const { return true; }
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const;
    virtual bool GetRequireSourceRead() const { return pSource == pImage || pImageCompare1 == pImage; };
	virtual void ExecuteSW(DICommandContext& context, ImageData& dest, ImageData** src = 0) const;
};


struct DICommand_FillRect : public DICommandImpl<DICommand_FillRect>
{
    Rect<SInt32> ApplyRect;
    Color        FillColor;

    DICommand_FillRect(DrawableImage* image, const Rect<SInt32>& rect, Color color)
        : DICommandImpl<DICommand_FillRect>(image), ApplyRect(rect), FillColor(color)
    { }
    DICommand_FillRect(const DICommand_FillRect& other)
        : DICommandImpl<DICommand_FillRect>(other.pImage),
          ApplyRect(other.ApplyRect), FillColor(other.FillColor)
    { }

    virtual DICommandType GetType() const { return DICommandType_FillRect; }
    virtual unsigned HasCPUImplementation() const { return true; }

    virtual void ExecuteSW(DICommandContext& context,
                           ImageData& dest, ImageData** src = 0) const;

    virtual void ExecuteHW(DICommandContext& context) const;
};


struct DICommand_FloodFill : public DICommandImpl<DICommand_FloodFill>
{
    Point<SInt32> Pt;
    Color         FillColor;

    DICommand_FloodFill(DrawableImage* image, const Point<SInt32>& pt, Color color)
        : DICommandImpl<DICommand_FloodFill>(image), Pt(pt), FillColor(color)
    { }
    DICommand_FloodFill(const DICommand_FloodFill& other)
        : DICommandImpl<DICommand_FloodFill>(other.pImage), Pt(other.Pt), FillColor(other.FillColor)
    { }

    virtual DICommandType GetType() const { return DICommandType_FloodFill; }
    virtual unsigned HasCPUImplementation() const { return true; }

    virtual void ExecuteSW(DICommandContext& context,
                           ImageData& dest, ImageData** src = 0) const;
};

struct DICommand_Merge : public DICommand_SourceRectImpl<DICommand_Merge>
{
    unsigned RedMultiplier;
    unsigned GreenMultiplier;
    unsigned BlueMultiplier;
    unsigned AlphaMultiplier;

    DICommand_Merge(DrawableImage* image, DrawableImage* source,
        const Rect<SInt32>& sr, const Point<SInt32>& dp, 
        unsigned rmul, unsigned gmul, unsigned bmul, unsigned amul) :
        DICommand_SourceRectImpl<DICommand_Merge>(image, source, sr, dp),
        RedMultiplier(rmul), GreenMultiplier(gmul), BlueMultiplier(bmul), AlphaMultiplier(amul)
    { }

    DICommand_Merge(const DICommand_Merge& other) :
        DICommand_SourceRectImpl<DICommand_Merge>(other),
        RedMultiplier(other.RedMultiplier),
        GreenMultiplier(other.GreenMultiplier),
        BlueMultiplier(other.BlueMultiplier),
        AlphaMultiplier(other.AlphaMultiplier)
    { }

    virtual DICommandType GetType() const { return DICommandType_Merge; }
    virtual unsigned HasCPUImplementation() const { return true; }
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const;
    virtual bool GetRequireSourceRead() const { return true; };
	virtual void ExecuteSW(DICommandContext& context, ImageData& dest, ImageData** src = 0) const;
};

struct DICommand_Noise : public DICommandImpl<DICommand_Noise>
{
    unsigned RandomSeed;
    unsigned Low;
    unsigned High;
    unsigned ChannelMask;
    bool     GrayScale;

    DICommand_Noise(DrawableImage* image, unsigned randomSeed, 
        unsigned low, unsigned high,unsigned channelMask, bool grayscale)
        : DICommandImpl<DICommand_Noise>(image), RandomSeed(randomSeed), Low(low), 
        High(high), ChannelMask(channelMask), GrayScale(grayscale)
    { }
    DICommand_Noise(const DICommand_Noise& other)
        : DICommandImpl<DICommand_Noise>(other.pImage), RandomSeed(other.RandomSeed), Low(other.Low), 
        High(other.High), ChannelMask(other.ChannelMask), GrayScale(other.GrayScale)
    { }

    virtual DICommandType GetType() const { return DICommandType_Noise; }
    virtual unsigned HasCPUImplementation() const { return true; }

    virtual void ExecuteSW(DICommandContext& context, ImageData& dest, 
        ImageData** psrc = 0) const;
};

struct DICommand_PaletteMap : public DICommand_SourceRectImpl<DICommand_PaletteMap>
{
    static const unsigned ChannelSize = 256 * sizeof(UInt32);
    static const unsigned PaletteSize = 4 * ChannelSize;
    UInt32* Channels;

    // Holds which channel remappings were actually provided.
    unsigned ChannelMask;

    DICommand_PaletteMap(DrawableImage* image, DrawableImage* source,
        const Rect<SInt32>& sr, const Point<SInt32>& dp, 
        UInt32** channels) :
        DICommand_SourceRectImpl<DICommand_PaletteMap>(image, source, sr, dp),
        ChannelMask(0)
    {
        Channels = (UInt32*)SF_ALLOC(PaletteSize, Stat_Default_Mem);
        memset(Channels, 0, PaletteSize);
        for ( int i = 0; i < 4; ++i )
        {
            if ( channels[i] )
            {
                ChannelMask |= 1<<i;
                memcpy(((char*)Channels)+(i*ChannelSize), channels[i], ChannelSize);
            }
        }
    }

    DICommand_PaletteMap(const DICommand_PaletteMap& other) :
    DICommand_SourceRectImpl<DICommand_PaletteMap>(other),
        ChannelMask(other.ChannelMask)
    {
        Channels = (UInt32*)SF_ALLOC(PaletteSize, Stat_Default_Mem);
        memcpy(Channels, other.Channels, PaletteSize );
    }

    ~DICommand_PaletteMap()
    {
        if (Channels)
            SF_FREE(Channels);
        Channels = 0;
    }

    virtual DICommandType GetType() const { return DICommandType_PaletteMap; }
    virtual unsigned HasCPUImplementation() const { return true; }
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const;
    virtual bool GetRequireSourceRead() const { return pImage == pSource; };
	virtual void ExecuteSW(DICommandContext& context, ImageData& dest, ImageData** src = 0) const;
};

struct DICommand_PerlinNoise : public DICommandImpl<DICommand_PerlinNoise>
{
    float       FrequencyX, FrequencyY;
    unsigned    NumOctaves;
    unsigned    RandomSeed;
    bool        Stitch;
    bool        FractalNoise;
    unsigned    ChannelMask;
    bool        GrayScale;

    // The maximum number of octaves that offsets that can be applied to. 
    static const unsigned MaximumOffsetOctaves = 16;
    float       Offsets[MaximumOffsetOctaves*2];
    unsigned    OffsetCount;

    DICommand_PerlinNoise(DrawableImage* image, 
        float frequencyX, float frequencyY, unsigned octaves, unsigned seed,
        bool stitch, bool fractal, unsigned channels, bool grayScale, float* offsets =0,
        unsigned offsetCount = 0) :
        DICommandImpl<DICommand_PerlinNoise>(image),
        FrequencyX(frequencyX), FrequencyY(frequencyY), NumOctaves(octaves), RandomSeed(seed),
        Stitch(stitch), FractalNoise(fractal), ChannelMask(channels), GrayScale(grayScale), 
        OffsetCount(Alg::Min(offsetCount, DICommand_PerlinNoise::MaximumOffsetOctaves))
    {
        if ( offsetCount > 0 )
            memcpy(Offsets, offsets, sizeof(unsigned)*offsetCount );
    }

    DICommand_PerlinNoise(const DICommand_PerlinNoise& other) : 
        DICommandImpl<DICommand_PerlinNoise>(other), 
        FrequencyX(other.FrequencyX), FrequencyY(other.FrequencyY), NumOctaves(other.NumOctaves), 
        RandomSeed(other.RandomSeed), Stitch(other.Stitch), FractalNoise(other.FractalNoise), 
        ChannelMask(other.ChannelMask), GrayScale(other.GrayScale), OffsetCount(other.OffsetCount)        
    {
        if ( other.OffsetCount > 0 )
            memcpy(Offsets, other.Offsets, Alg::Min(sizeof(Offsets), sizeof(unsigned)*other.OffsetCount) );
    }

    virtual DICommandType GetType() const { return DICommandType_PerlinNoise; }
    virtual unsigned HasCPUImplementation() const { return true; }

    virtual void ExecuteSW(DICommandContext& context,
        ImageData& dest, ImageData** src = 0) const;
};

struct DICommand_Scroll : public DICommand_SourceRectImpl<DICommand_Scroll>
{
    signed X;
    signed Y;

    DICommand_Scroll(DrawableImage* image, int x, int y) :
    DICommand_SourceRectImpl<DICommand_Scroll>(image, image, 
        Rect<SInt32>(0, 0, image->GetSize().Width, image->GetSize().Height), Point<SInt32>(x, y)),
        X(x), Y(y)
    { }

    DICommand_Scroll(const DICommand_Scroll& other) :
    DICommand_SourceRectImpl<DICommand_Scroll>(other),
        X(other.X), Y(other.Y)
    { }

    virtual DICommandType GetType() const { return DICommandType_Scroll; }
    virtual unsigned HasCPUImplementation() const { return true; }
    // Note: does nothing, the copyback will handle the actual implementation
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const;
    virtual bool GetRequireSourceRead() const { return true; };
	virtual void ExecuteSW(DICommandContext& context, ImageData& dest, ImageData** src = 0) const;
};

struct DICommand_Threshold : public DICommand_SourceRectImpl<DICommand_Threshold>
{
    DrawableImage::OperationType Operation;
    unsigned      Threshold;
    unsigned      ThresholdColor;
    unsigned      Mask;
    bool          CopySource;

    DICommand_Threshold(DrawableImage* image, DrawableImage* source,
        const Rect<SInt32>& sr, const Point<SInt32>& dp, 
        DrawableImage::OperationType op, unsigned th, unsigned col, unsigned mask, bool copySource) :
        DICommand_SourceRectImpl<DICommand_Threshold>(image, source, sr, dp),
        Operation(op), Threshold(th), ThresholdColor(col), Mask(mask), CopySource(copySource)
    { }

    DICommand_Threshold(const DICommand_Threshold& other) :
        DICommand_SourceRectImpl<DICommand_Threshold>(other),
        Operation(other.Operation), Threshold(other.Threshold), ThresholdColor(other.ThresholdColor), 
        Mask(other.Mask), CopySource(other.CopySource)
    { }

    virtual DICommandType GetType() const { return DICommandType_Threshold; }
    virtual unsigned HasCPUImplementation() const { return true; }
    virtual void ExecuteHWCopyAction( DICommandContext& context, Render::Texture** tex, const Matrix2F* texgen ) const;
    virtual bool GetRequireSourceRead() const { return CopySource; };
	virtual void ExecuteSW(DICommandContext& context, ImageData& dest, ImageData** src = 0) const;
};

}}; // namespace Scaleform::Render

#endif // INC_SF_Render_DrawableImage_Queue_H
