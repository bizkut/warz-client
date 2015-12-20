/**********************************************************************

PublicHeader:   Render
Filename    :   D3D1x_Sync.h
Content     :   
Created     :   Feb 2012
Authors     :   Bart Muzzin

Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

***********************************************************************/

#ifndef INC_SF_D3D1x_Sync_H
#define INC_SF_D3D1x_Sync_H

#include "Render/Render_Sync.h"
#include "Kernel/SF_Threads.h"

namespace Scaleform { namespace Render { namespace D3D1x {

class RenderSync : public Render::RenderSync
{
public:
    RenderSync() : pDevice(0), pNextEndFrameFence(0) { };    

    bool SetDevice(ID3D1x(Device)* pdevice, ID3D1x(DeviceContext)* pdeviceCtx) 
    { 
        // If we are removing the device, destroy any outstanding queries. This happens when
        // resetting the device or shutting down, so otherwise they would leak.
        if ( pdevice == 0 || pdeviceCtx == 0)
        {
            pDevice = 0;
            pDeviceContext = 0;
            ReleaseOutstandingFrames();
            return true;
        }
        else
        {
            // Test creating a query, just to make sure it works.
            D3D1x(QUERY_DESC) desc;
            memset(&desc, 0, sizeof(D3D1x(QUERY_DESC)));
            desc.Query = D3D1x(QUERY_EVENT);

            ID3D1x(Query)* pquery = 0;
            HRESULT hr = pdevice->CreateQuery(&desc, &pquery);
            if ( pquery )
            {
                pquery->Release();
                pDevice = pdevice; 
                pDeviceContext = pdeviceCtx;
            }
            return SUCCEEDED(hr);
        }
    }

    virtual void    BeginFrame()
    {
        if ( !pDevice )
            return;

        // Create a query object, but do not issue it; the fences that are inserted this frame
        // need to have a handle to their query object, but it must be issued after they are.
        SF_ASSERT(pNextEndFrameFence == 0);

        D3D1x(QUERY_DESC) desc;
        memset(&desc, 0, sizeof(D3D1x(QUERY_DESC)));
        desc.Query = D3D1x(QUERY_EVENT);
        HRESULT hr = pDevice->CreateQuery(&desc, &pNextEndFrameFence);
        if ( FAILED(hr) )
            pNextEndFrameFence = 0;

        Render::RenderSync::BeginFrame();
    }

    virtual bool    EndFrame()
    {
        if ( !pDevice || !pDeviceContext)
            return true;

        if ( !Render::RenderSync::EndFrame())
            return false;

        // Now issue the query from this frame.
        if (pNextEndFrameFence)
        {
            D3D1xEndAsynchronous(pDeviceContext, pNextEndFrameFence);
            pNextEndFrameFence->Release();
        }
        pNextEndFrameFence = 0;
        return true;
    }

    virtual void    KickOffFences(FenceType waitType)     
    {  
        // Unneeded, GPU will always continue processing.
        SF_UNUSED(waitType);
    }

protected:

    virtual UInt64   SetFence()
    { 
        if ( pNextEndFrameFence )
            pNextEndFrameFence->AddRef();
        return (UInt64)pNextEndFrameFence;
    }

    virtual bool    IsPending(FenceType waitType, UInt64 handle, const FenceFrame& parent) 
    { 
        SF_UNUSED2(waitType, parent);
        ID3D1x(Query)* pquery = (ID3D1x(Query)*)handle;
        if ( pquery == 0 )
            return false;

        // No fence frames indicates either an impending reset, or before the first BeginFrame.
        // Either way, report that no queries are pending.
        if ( FenceFrames.IsEmpty() )
            return false;

        // If we are querying the current frame's fence, it must be unsignaled, because the
        // query is always issued immediately before 'forgetting' the next frame's query pointer.
        if ( pquery == pNextEndFrameFence )
            return true;

        HRESULT hr = D3D1xGetDataAsynchronous(pDeviceContext, pquery, 0, 0, D3D1x(ASYNC_GETDATA_DONOTFLUSH));
        return hr != S_OK;
    }
    virtual void    WaitFence(FenceType waitType, UInt64 handle, const FenceFrame& parent)
    {
        SF_UNUSED2(waitType, parent);
        ID3D1x(Query)* pquery = (ID3D1x(Query)*)handle;
        if ( pquery == 0 )
            return;        

        // If we are waiting on the current frame's query, we must issue it first. This is the worst
        // can scenario, as we will be waiting on a query that was just issued; causing a CPU/GPU sync.
        if ( pquery == pNextEndFrameFence )
        {
            D3D1xEndAsynchronous(pDeviceContext, pNextEndFrameFence);
            pquery = pNextEndFrameFence;

            // Create another query for the (possibly) remaining items in this frame.
            D3D1x(QUERY_DESC) desc;
            memset(&desc, 0, sizeof(D3D1x(QUERY_DESC)));
            desc.Query = D3D1x(QUERY_EVENT);
            HRESULT hr = pDevice->CreateQuery(&desc, &pNextEndFrameFence);
            if ( FAILED(hr) )
                pNextEndFrameFence = 0;            
        }

        // Poll until the data is ready.
        while ( D3D1xGetDataAsynchronous(pDeviceContext, pquery, 0, 0, 0) != S_OK)
            Thread::Sleep(0);
    }

    virtual void    ReleaseFence(UInt64 handle)
    {
        ID3D1x(Query)* pquery = (ID3D1x(Query)*)handle;
        if ( pquery == 0 )
            return;
        pquery->Release();
    }

    // No need to implement wraparound; D3D1x queries have no external ordering.

private:
    ID3D1x(Query)*              pNextEndFrameFence;
    Ptr<ID3D1x(Device)>         pDevice;
    Ptr<ID3D1x(DeviceContext)>  pDeviceContext;
};

}}} // Scaleform::Render::D3D1x

#endif // INC_SF_D3D1x_Sync_H
