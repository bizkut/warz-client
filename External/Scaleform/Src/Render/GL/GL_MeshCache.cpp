/**************************************************************************

Filename    :   GL_MeshCache.cpp
Content     :   GL Mesh Cache implementation
Created     :   
Authors     :   

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "GL_MeshCache.h"
#include "Kernel/SF_Debug.h"
#include "Kernel/SF_Alg.h"
#include "Kernel/SF_HeapNew.h"

#if defined(SF_USE_GLES)
    #include "Render/GL/GLES11_ExtensionMacros.h"
    // GLES 1.1 does not have batching (or shaders), so just set the max batches to 1
    #if !defined SF_RENDER_MAX_BATCHES
        #define SF_RENDER_MAX_BATCHES 1
    #endif
#elif defined(GL_ES_VERSION_2_0)
    #include "Render/GL/GLES_ExtensionMacros.h"
#else
    #include "Render/GL/GL_ExtensionMacros.h"
#endif

//#define SF_RENDER_LOG_CACHESIZE

namespace Scaleform { namespace Render { namespace GL {


MeshBuffer::~MeshBuffer()
{
    if (Buffer)
        glDeleteBuffers(1, &Buffer);
    if ( BufferData )
        SF_FREE(BufferData);
}

bool MeshBuffer::DoMap()
{
    SF_ASSERT(!pData);
    if (pHal->Caps & Cap_MapBuffer)
    {
        glBindBuffer(Type, Buffer);
        pData = glMapBuffer(Type, GL_WRITE_ONLY);
    }
    else
    {
        if (!BufferData)
            BufferData = (UByte*)SF_ALLOC(Size, StatRender_Buffers_Mem);
        pData = BufferData;
    }
    return pData != 0;
}

void MeshBuffer::Unmap()
{
    if (pData && Buffer)
    {
        glBindBuffer(Type, Buffer);
        if (pHal->Caps & Cap_MapBuffer)
        {
            GLboolean result = glUnmapBuffer(Type); // XXX - data loss can occur here
            SF_ASSERT(result);
            SF_UNUSED(result);
        }
        else if (pHal->Caps & Cap_BufferUpdate)
        {
            glBufferSubData(Type, 0, Size, pData);       
        }
    }
    pData = 0;
}

UByte*  MeshBuffer::GetBufferBase() const
{
    return (pHal->Caps & Cap_UseMeshBuffers) ? 0 : BufferData;
}

bool MeshBuffer::allocBuffer()
{
    if (Buffer)
        glDeleteBuffers(1, &Buffer);

    if (pHal->Caps & Cap_UseMeshBuffers)
    {
        glGenBuffers(1, &Buffer);

        // Binding to the array or element target at creation is supposed to let drivers that need
        // separate vertex/index storage to know what the buffer will be used for.
        glBindBuffer(Type, Buffer);
        glBufferData(Type, Size, 0, GL_STATIC_DRAW);
    }
    return 1;
}


// Helpers used to initialize default granularity sizes,
// splitting VB/Index size by 5/9.
inline UPInt calcVBGranularity(UPInt granularity)
{
    return (((granularity >> 4) * 5) / 9) << 4;
}
inline UPInt calcIBGranularity(UPInt granularity, UPInt vbGranularity)
{
    return (((granularity >> 4) - (vbGranularity >> 4)) << 4);
}


//------------------------------------------------------------------------
// ***** MeshCache

MeshCache::MeshCache(MemoryHeap* pheap, const MeshCacheParams& params)
  : Render::MeshCache(pheap, params),
    pHal(0), CacheList(getThis()),
    VertexBuffers(GL_ARRAY_BUFFER, pheap, calcVBGranularity(params.MemGranularity)),
    IndexBuffers(GL_ELEMENT_ARRAY_BUFFER, pheap,
                 calcIBGranularity(params.MemGranularity, VertexBuffers.GetGranularity())),
    Mapped(false), VBSizeEvictedInMap(0),
    MaskEraseBatchVertexBuffer(0)
{
}

MeshCache::~MeshCache()
{
    Reset();
}

// Initializes MeshCache for operation, including allocation of the reserve
// buffer. Typically called from SetVideoMode.
bool    MeshCache::Initialize(HAL* phal)
{
    pHal = phal;

    // Determine GL-capability settings. Needs to be called after the GL context is created.
    adjustMeshCacheParams(&Params);

    if (!StagingBuffer.Initialize(pHeap, Params.StagingBufferSize))
        return false;

    UseSeparateIndexBuffers = true;

    if (!createStaticVertexBuffers())
    {
        Reset();
        return false;
    }

    if (Params.MemReserve &&
        !allocCacheBuffers(Params.MemReserve, MeshBuffer::AT_Reserve))
    {
        Reset();
        return false;
    }

    return true;
}

void MeshCache::Reset(bool lost)
{
    if (pHal)
    {
        destroyBuffers(MeshBuffer::AT_None, lost);
        if (MaskEraseBatchVertexBuffer)
            glDeleteBuffers(1, &MaskEraseBatchVertexBuffer);
        MaskEraseBatchVertexBuffer = 0;
        pHal = 0;
    }

    StagingBuffer.Reset();
}

void MeshCache::ClearCache()
{
    destroyBuffers(MeshBuffer::AT_Chunk);
    SF_ASSERT(BatchCacheItemHash.GetSize() == 0);
}

void MeshCache::destroyBuffers(MeshBuffer::AllocType at, bool lost)
{
    // TBD: Evict everything first!
    CacheList.EvictAll();
    VertexBuffers.DestroyBuffers(at, lost);
    IndexBuffers.DestroyBuffers(at, lost);
    ChunkBuffers.Clear();
}

bool MeshCache::SetParams(const MeshCacheParams& argParams)
{
    MeshCacheParams params(argParams);
    adjustMeshCacheParams(&params);

    if (pHal)
    {
        CacheList.EvictAll();

        if (Params.StagingBufferSize != params.StagingBufferSize)
        {
            if (!StagingBuffer.Initialize(pHeap, params.StagingBufferSize))
            {
                if (!StagingBuffer.Initialize(pHeap, Params.StagingBufferSize))
                {
                    SF_DEBUG_ERROR(1, "MeshCache::SetParams - couldn't restore StagingBuffer after fail");
                }
                return false;
            }
        }

        if ((Params.MemReserve != params.MemReserve) ||
            (Params.MemGranularity != params.MemGranularity))
        {
            destroyBuffers();

            // Allocate new reserve. If not possible, restore previous one and fail.
            if (params.MemReserve &&
                !allocCacheBuffers(params.MemReserve, MeshBuffer::AT_Reserve))
            {
                if (Params.MemReserve &&
                    !allocCacheBuffers(Params.MemReserve, MeshBuffer::AT_Reserve))
                {
                    SF_DEBUG_ERROR(1, "MeshCache::SetParams - couldn't restore Reserve after fail");
                }
                return false;
            }

            VertexBuffers.SetGranularity(calcVBGranularity(params.MemGranularity));
            IndexBuffers.SetGranularity(calcIBGranularity(params.MemGranularity,
                                        VertexBuffers.GetGranularity()));
        }
    }
    Params = params;
    return true;
}

void MeshCache::adjustMeshCacheParams(MeshCacheParams* p)
{
    // TBD: Detect/record HW instancing capability.

    // No shaders on GLES 1.1, so detecting the number of batches is not possible, because it's 1.
#if !defined(SF_USE_GLES)
    // Get the maximum number of uniforms, which will determine the maximum batch count.
    // This can be as low as 128 on GLES2.0 platforms.

    GLint maxUniforms = (pHal->Caps & Cap_MaxUniforms) >> Cap_MaxUniforms_Shift;

    SF_ASSERT(maxUniforms >= 64); // should be guaranteed by both GL and GLES.
    unsigned maxInstances = Alg::Min<unsigned>(SF_RENDER_MAX_BATCHES, 
    maxUniforms / ShaderInterface::GetMaximumRowsPerInstance());
#else
    unsigned maxInstances = 1;
#endif

    if (p->MaxBatchInstances > maxInstances)
        p->MaxBatchInstances = maxInstances;
    if (p->VBLockEvictSizeLimit < 1024 * 256)
        p->VBLockEvictSizeLimit = 1024 * 256;

    UPInt maxStagingItemSize = p->MaxVerticesSizeInBatch +
                               sizeof(UInt16) * p->MaxIndicesInBatch;
    if (maxStagingItemSize * 2 > p->StagingBufferSize)
        p->StagingBufferSize = maxStagingItemSize * 2;
}


void MeshCache::EndFrame()
{
    CacheList.EndFrame();

    // Simple Heuristic used to shrink cache. Shrink is possible once the
    // (Total_Frame_Size + LRUTailSize) exceed the allocated space by more then
    // one granularity unit. If this case, we destroy the cache buffer in the
    // order opposite to that of which it was created.

    // TBD: This may have a side effect of throwing away the current frame items
    // as well. Such effect is undesirable and can perhaps be avoided on consoles
    // with buffer data copies (copy PrevFrame content into other buffers before evict).

    UPInt totalFrameSize = CacheList.GetSlotSize(MCL_PrevFrame);
    UPInt lruTailSize    = CacheList.GetSlotSize(MCL_LRUTail);
    UPInt expectedSize   = totalFrameSize + Alg::PMin(lruTailSize, Params.LRUTailSize);
    expectedSize += expectedSize / 4; // + 25%, to account for fragmentation.

    SPInt extraSpace = getTotalSize() - (SPInt)expectedSize;
    if (extraSpace > (SPInt)Params.MemGranularity)
    {        
        while (!ChunkBuffers.IsEmpty() && (extraSpace > (SPInt)Params.MemGranularity))
        {
            MeshBuffer* p = (MeshBuffer*)ChunkBuffers.GetLast();
            p->RemoveNode();
            extraSpace -= (SPInt)p->GetSize();

            MeshBufferSet&  mbs = (p->GetBufferType() == GL_ARRAY_BUFFER) ?
                                  (MeshBufferSet&)VertexBuffers : (MeshBufferSet&)IndexBuffers;
            // Evict first!
            evictMeshesInBuffer(CacheList.GetSlots(), MCL_ItemCount, p);
            mbs.DestroyBuffer(p);

#ifdef SF_RENDER_LOG_CACHESIZE
            LogDebugMessage(Log_Message,
                "Cache shrank to %dK. Start FrameSize = %dK, LRU = %dK\n",
                getTotalSize() / 1024, totalFrameSize/1024, lruTailSize/1024);
#endif
        }
    }    
}


// Adds a fixed-size buffer to cache reserve; expected to be released at Release.
/*
bool MeshCache::AddReserveBuffer(unsigned size, unsigned arena)
{
}
bool MeshCache::ReleaseReserveBuffer(unsigned size, unsigned arena)
{
}
*/


// Allocates Vertex/Index buffer of specified size and adds it to free list.
bool MeshCache::allocCacheBuffers(UPInt size, MeshBuffer::AllocType type, unsigned arena)
{
    if (UseSeparateIndexBuffers)
    {
        UPInt vbsize = calcVBGranularity(size);
        UPInt ibsize = calcIBGranularity(size, vbsize);

        MeshBuffer *pvb = VertexBuffers.CreateBuffer(vbsize, type, arena, pHeap, pHal);
        if (!pvb)
            return false;
        MeshBuffer   *pib = IndexBuffers.CreateBuffer(ibsize, type, arena, pHeap, pHal);
        if (!pib)
        {
            VertexBuffers.DestroyBuffer(pvb);
            return false;
        }
    }
    else
    {
        MeshBuffer *pb = VertexBuffers.CreateBuffer(size, type, arena, pHeap, pHal);
        if (!pb)
            return false;
    }

#ifdef SF_RENDER_LOG_CACHESIZE
    LogDebugMessage(Log_Message, "Cache grew to %dK\n", getTotalSize() / 1024);
#endif
    return true;
}

bool MeshCache::createStaticVertexBuffers()
{
    return createInstancingVertexBuffer() &&
           createMaskEraseBatchVertexBuffer();
}

bool MeshCache::createInstancingVertexBuffer()
{
    return true;
}

bool MeshCache::createMaskEraseBatchVertexBuffer()
{
    VertexXY16iAlpha pbuffer[6 * SF_RENDER_MAX_BATCHES];

    for(unsigned i = 0; i< SF_RENDER_MAX_BATCHES; i++)
    {
        // This assumes Alpha in first byte. Effect may depend on byte order and
        // ShaderManager vertex format mapping (offset assigned for VET_Instance8
        // for ShaderManager::registerVertexFormat).
        pbuffer[i * 6 + 0].x  = 0;
        pbuffer[i * 6 + 0].y  = 1;
        pbuffer[i * 6 + 0].Alpha[0] = (UByte)i;
        pbuffer[i * 6 + 1].x  = 0;
        pbuffer[i * 6 + 1].y  = 0;
        pbuffer[i * 6 + 1].Alpha[0] = (UByte)i;
        pbuffer[i * 6 + 2].x  = 1;
        pbuffer[i * 6 + 2].y  = 0;
        pbuffer[i * 6 + 2].Alpha[0] = (UByte)i;

        pbuffer[i * 6 + 3].x  = 0;
        pbuffer[i * 6 + 3].y  = 1;
        pbuffer[i * 6 + 3].Alpha[0] = (UByte)i;
        pbuffer[i * 6 + 4].x  = 1;
        pbuffer[i * 6 + 4].y  = 0;
        pbuffer[i * 6 + 4].Alpha[0] = (UByte)i;
        pbuffer[i * 6 + 5].x  = 1;
        pbuffer[i * 6 + 5].y  = 1;
        pbuffer[i * 6 + 5].Alpha[0] = (UByte)i;
    }

    glGenBuffers(1, &MaskEraseBatchVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, MaskEraseBatchVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pbuffer), pbuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

bool MeshCache::LockBuffers()
{
    SF_ASSERT(!Mapped);
    Mapped = true;
    VBSizeEvictedInMap = 0;
    if (pRQCaches)
        pRQCaches->SetCacheLocked(Cache_Mesh);
    return true;
}

void MeshCache::UnlockBuffers()
{
    SF_ASSERT(Mapped != 0); 
    MappedBuffers.UnmapAll();
    Mapped = false;
    if (pRQCaches)
        pRQCaches->ClearCacheLocked(Cache_Mesh);
}


void MeshCache::evictMeshesInBuffer(MeshCacheListSet::ListSlot* plist, UPInt count,
                                    MeshBuffer* pbuffer)
{
    for (unsigned i=0; i< count; i++)
    {
        MeshCacheItem* pitem = (MeshCacheItem*)plist[i].GetFirst();
        while (!plist[i].IsNull(pitem))
        {
            if ((pitem->pVertexBuffer == pbuffer) || (pitem->pIndexBuffer == pbuffer))
            {
                // Evict may potentially modify the cache items, so start again.
                // This is less than ideal, but better than accessing a dangling pointer.
                Evict(pitem);
                pitem = (MeshCacheItem*)plist[i].GetFirst();
            }
            else
            {
                pitem = (MeshCacheItem*)pitem->pNext;
            }
        }
    }
}


UPInt MeshCache::Evict(Render::MeshCacheItem* pbatch, AllocAddr* pallocator, MeshBase* pskipMesh)
{
    MeshCacheItem* p = (MeshCacheItem*)pbatch;
    // - Free allocator data.
    UPInt vbfree = VertexBuffers.Free(p->VBAllocSize, p->pVertexBuffer, p->VBAllocOffset);
    UPInt ibfree = IndexBuffers.Free(p->IBAllocSize, p->pIndexBuffer, p->IBAllocOffset);
    UPInt freedSize = pallocator ? ((&VertexBuffers.GetAllocator() == pallocator) ? vbfree : ibfree) : vbfree + ibfree;
    
    VBSizeEvictedInMap += (unsigned)  p->VBAllocSize;
    p->Destroy(pskipMesh);
    return freedSize;
}


// Allocates the buffer, while evicting LRU data.
bool MeshCache::allocBuffer(UPInt* poffset, MeshBuffer** pbuffer,
                            MeshBufferSet& mbs, UPInt size, bool waitForCache)
{
    SF_UNUSED(waitForCache);

    if (mbs.Alloc(size, pbuffer, poffset))
        return true;

    // If allocation failed... need to apply swapping or grow buffer.
    MeshCacheItem* pitems;
    MeshCacheListSet::ListSlot& prevFrameList = CacheList.GetSlot(MCL_PrevFrame);
    MeshCacheListSet::ListSlot& thisFrameList = CacheList.GetSlot(MCL_ThisFrame);

    // 1) First, apply LRU (least recently used) swapping from data stale in
    //    earlier frames until the total size 

    if ((getTotalSize() + MinSupportedGranularity) <= Params.MemLimit)
    {
        if (CacheList.EvictLRUTillLimit(MCL_LRUTail, mbs.GetAllocator(),
                                        size, Params.LRUTailSize))
                                        goto alloc_size_available;

        // TBD: May cause spinning? Should we have two error codes?
        SF_ASSERT(size <= mbs.GetGranularity());
        if (size > mbs.GetGranularity())
            return false;

        UPInt allocSize = Alg::PMin(Params.MemLimit - getTotalSize(), mbs.GetGranularity());
        if (size <= allocSize)
        {
            MeshBuffer* pbuff = mbs.CreateBuffer(allocSize, MeshBuffer::AT_Chunk, 0, pHeap, pHal);
            if (pbuff)
            {
                ChunkBuffers.PushBack(pbuff);
#ifdef SF_RENDER_LOG_CACHESIZE
                LogDebugMessage(Log_Message, "Cache grew to %dK\n", getTotalSize() / 1024);
#endif
                goto alloc_size_available;
            }
        }
    }

    if (CacheList.EvictLRU(MCL_LRUTail, mbs.GetAllocator(), size))
        goto alloc_size_available;

    if (VBSizeEvictedInMap > Params.VBLockEvictSizeLimit)
        return false;

    // 2) Apply MRU (most recently used) swapping to the current frame content.
    // NOTE: MRU (GetFirst(), pNext iteration) gives
    //       2x improvement here with "Stars" test swapping.
    pitems = (MeshCacheItem*)prevFrameList.GetFirst();
    while(!prevFrameList.IsNull(pitems))
    {
        if (Evict(pitems, &mbs.GetAllocator()) >= size)
            goto alloc_size_available;
        pitems = (MeshCacheItem*)prevFrameList.GetFirst();
    }
    
    pitems = (MeshCacheItem*)thisFrameList.GetFirst();
    while(!thisFrameList.IsNull(pitems))
    {
        if (Evict(pitems, &mbs.GetAllocator()) >= size)
            goto alloc_size_available;
        pitems = (MeshCacheItem*)thisFrameList.GetFirst();
    }
    return false;

    // At this point we know we have a large enough block either due to
    // swapping or buffer growth, so allocation shouldn't fail.
alloc_size_available:
    if (!mbs.Alloc(size, pbuffer, poffset))
    {
        SF_ASSERT(0);
        return false;
    }

    return true;
}


// Generates meshes and uploads them to buffers.
bool MeshCache::PreparePrimitive(PrimitiveBatch* pbatch,
                                 MeshCacheItem::MeshContent &mc,
                                 bool waitForCache)
{
    Primitive* prim = pbatch->GetPrimitive();

    if (mc.IsLargeMesh())
    {
        SF_ASSERT(mc.GetMeshCount() == 1);
        MeshResult mr = GenerateMesh(mc[0], prim->GetVertexFormat(),
                                     pbatch->pFormat, 0, waitForCache);

        if (mr.Succeded())
            pbatch->SetCacheItem(mc[0]->CacheItems[0]);
        // Return 'false' if we just need more cache, to flush and retry.
        if (mr == MeshResult::Fail_LargeMesh_NeedCache)
            return false;
        return true;
    }

    // NOTE: We always know that meshes in one batch fit into Mesh Staging Cache.
    unsigned totalVertexCount, totalIndexCount;
    pbatch->CalcMeshSizes(&totalVertexCount, &totalIndexCount);

    SF_ASSERT(pbatch->pFormat);

    Render::MeshCacheItem* batchData = 0;
    unsigned       destVertexSize = pbatch->pFormat->Size;
    UByte*         pvertexDataStart;
    IndexType*     pindexDataStart;
    AllocResult    allocResult;

    allocResult = AllocCacheItem(&batchData, &pvertexDataStart, &pindexDataStart,
                                 MeshCacheItem::Mesh_Regular, mc,
                                 totalVertexCount * destVertexSize,
                                 totalVertexCount, totalIndexCount, waitForCache, 0);
    if (allocResult != Alloc_Success)
    {
        // Return 'true' for state error (we can't recover by swapping cache and re-trying).
        return (allocResult == Alloc_Fail) ? false : true;
    }

    pbatch->SetCacheItem(batchData);

    // Prepare and Pin mesh data with the StagingBuffer.
    StagingBufferPrep meshPrep(this, mc, prim->GetVertexFormat(), false);

    // Copy meshes into the Vertex/Index buffers.

    // All the meshes have been pinned, so we can
    // go through them and copy them into buffers
    UByte*      pstagingBuffer   = StagingBuffer.GetBuffer();
    const VertexFormat* pvf      = prim->GetVertexFormat();
    const VertexFormat* pdvf     = pbatch->pFormat;

    unsigned    i;
    unsigned    indexStart = 0;

    for(i = 0; i< mc.GetMeshCount(); i++)
    {
        Mesh* pmesh = mc[i];
        SF_ASSERT(pmesh->StagingBufferSize != 0);

        // Convert vertices and initialize them to the running index
        // within this primitive.
        void*   convertArgArray[1] = { &i };
        ConvertVertices_Buffered(*pvf, pstagingBuffer + pmesh->StagingBufferOffset,
                                 *pdvf, pvertexDataStart,
                                 pmesh->VertexCount, &convertArgArray[0]);
        // Copy and assign indices.
        ConvertIndices(pindexDataStart,
            (IndexType*)(pstagingBuffer + pmesh->StagingBufferIndexOffset),
            pmesh->IndexCount, (IndexType)indexStart);

        pvertexDataStart += pmesh->VertexCount * destVertexSize;
        pindexDataStart  += pmesh->IndexCount;
        indexStart       += pmesh->VertexCount;
    }

    // ~StagingBufferPrep will Unpin meshes in the staging buffer.
    return true;
}

MeshCache::AllocResult
MeshCache::AllocCacheItem(Render::MeshCacheItem** pdata,
                          UByte** pvertexDataStart, IndexType** pindexDataStart,
                          MeshCacheItem::MeshType meshType,
                          MeshCacheItem::MeshBaseContent &mc,
                          UPInt vertexBufferSize,
                          unsigned vertexCount, unsigned indexCount,
                          bool waitForCache, const VertexFormat*)
{
    if (!AreBuffersMapped() && !LockBuffers())
        return Alloc_StateError;

    // Compute and allocate appropriate VB/IB space.
    UPInt       vbOffset = 0, ibOffset = 0;
    MeshBuffer  *pvb = 0, *pib = 0;
    UByte       *pvdata, *pidata;
    MeshCache::AllocResult failType = Alloc_Fail;

    if (!allocBuffer(&vbOffset, &pvb, VertexBuffers,
                     vertexBufferSize, waitForCache))
    {
        if (!VertexBuffers.CheckAllocationSize(vertexBufferSize, "Vertex"))
            failType = Alloc_Fail_TooBig;
handle_alloc_fail:
        if (pvb) VertexBuffers.Free(vertexBufferSize, pvb, vbOffset);
        if (pib) IndexBuffers.Free(indexCount * sizeof(IndexType), pib, ibOffset);
        return failType;
    }
    if (!allocBuffer(&ibOffset, &pib, IndexBuffers,
                     indexCount * sizeof(IndexType), waitForCache))
    {
        if (!IndexBuffers.CheckAllocationSize(indexCount * sizeof(IndexType), "Index"))
            failType = Alloc_Fail_TooBig;
        goto handle_alloc_fail;
    }

    pvdata = pvb->Map(MappedBuffers);
    pidata = pib->Map(MappedBuffers);

    if (!pvdata || !pidata)
        goto handle_alloc_fail;

    // Create new MeshCacheItem; add it to hash.
    *pdata = MeshCacheItem::Create(meshType, &CacheList, mc, pvb, pib,
                    (unsigned)vbOffset, vertexBufferSize, vertexCount,
                    (unsigned)ibOffset, indexCount * sizeof(IndexType), indexCount);

    if (!*pdata)
    {
        // Memory error; free buffers, skip mesh.
        SF_ASSERT(0);
        failType = Alloc_StateError;
        goto handle_alloc_fail;
    }

    *pvertexDataStart = pvdata + vbOffset;
    *pindexDataStart  = (IndexType*)(pidata + ibOffset);
    return Alloc_Success;
}

void MeshCache::GetStats(Stats* stats)
{
    *stats = Stats();
    unsigned memType = (pHal->Caps & Cap_UseMeshBuffers) ? MeshBuffer_GpuMem : 0;

    stats->TotalSize[memType + MeshBuffer_Vertex] = VertexBuffers.GetTotalSize();
    stats->UsedSize[memType + MeshBuffer_Vertex] = VertexBuffers.Allocator.GetFreeSize() << MeshCache_AllocatorUnitShift;

    stats->TotalSize[memType + MeshBuffer_Index] = IndexBuffers.GetTotalSize();
    stats->UsedSize[memType + MeshBuffer_Index] = IndexBuffers.Allocator.GetFreeSize() << MeshCache_AllocatorUnitShift;
}

}}}; // namespace Scaleform::Render::GL

