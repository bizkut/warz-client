/**********************************************************************

PublicHeader:   Render
Filename    :   D3D9_Sync.h
Content     :   D3D9 specific fencing implementation (once-a-frame queries).
Created     :   Jan 2012
Authors     :   Bart Muzzin

Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

***********************************************************************/

#ifndef INC_SF_D3D9_Sync_H
#define INC_SF_D3D9_Sync_H

#include "Render/Render_Sync.h"
#include <d3d9.h>

namespace Scaleform { namespace Render { namespace D3D9 {

class RenderSync : public Render::RenderSync
{
public:
    RenderSync() : pDevice(0), pNextEndFrameFence(0) { };    

    bool SetDevice(IDirect3DDevice9* pdevice) 
    { 
        // If we are removing the device, destroy any outstanding queries. This happens when
        // resetting the device or shutting down, so otherwise they would leak.
        if ( pdevice == 0 )
        {
            SF_DEBUG_ASSERT(pNextEndFrameFence == 0, "Cannot shutdown in the middle of a frame.");
            ReleaseOutstandingFrames();
            pDevice = 0;
            return true;
        }
        else
        {
            // Test creating a query, just to make sure it works.
            IDirect3DQuery9* pquery = 0;
            HRESULT hr = pdevice->CreateQuery(D3DQUERYTYPE_EVENT, &pquery);
            if ( pquery )
            {
                pquery->Release();
                pDevice = pdevice; 
            }
            return SUCCEEDED(hr);
        }
    }

    IDirect3DDevice9* GetDevice() const                    { return pDevice; }

    virtual void    BeginFrame()
    {
        if ( !pDevice )
            return;

        // Create a query object, but do not issue it; the fences that are inserted this frame
        // need to have a handle to their query object, but it must be issued after they are.
        SF_ASSERT(pNextEndFrameFence == 0);
        HRESULT hr = pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &pNextEndFrameFence);
        if ( FAILED(hr) )
            pNextEndFrameFence = 0;

        Render::RenderSync::BeginFrame();
    }

    virtual bool    EndFrame()
    {
        if ( !pDevice )
            return true;

        if ( !Render::RenderSync::EndFrame())
            return false;

        // Now issue the query from this frame.
        if (pNextEndFrameFence)
            pNextEndFrameFence->Issue(D3DISSUE_END);
        pNextEndFrameFence->Release();
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
        IDirect3DQuery9* pquery = (IDirect3DQuery9*)handle;
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

        HRESULT hr = pquery->GetData( 0, 0, 0 );
        return hr == S_FALSE;
    }
    virtual void    WaitFence(FenceType waitType, UInt64 handle, const FenceFrame& parent)
    {
        SF_UNUSED2(waitType, parent);
        IDirect3DQuery9* pquery = (IDirect3DQuery9*)handle;
        if ( pquery == 0 )
            return;        

        // If we are waiting on the current frame's query, we must issue it first. This is the worst
        // can scenario, as we will be waiting on a query that was just issued; causing a CPU/GPU sync.
        if ( pquery == pNextEndFrameFence )
        {
            pNextEndFrameFence->Issue(D3DISSUE_END);
            pquery = pNextEndFrameFence;

            // Create another query for the (possibly) remaining items in this frame.
            HRESULT hr = pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &pNextEndFrameFence);
            if ( FAILED(hr) )
                pNextEndFrameFence = 0;            
        }

        // Poll until the data is ready.
        while ( pquery->GetData(0, 0, D3DGETDATA_FLUSH) == S_FALSE )
			Thread::Sleep(0);
    }

    virtual void    ReleaseFence(UInt64 handle)
    {
        IDirect3DQuery9* pquery = (IDirect3DQuery9*)handle;
        if ( pquery == 0 )
            return;
        pquery->Release();
    }

    // No need to implement wraparound; D3D9 queries have no external ordering.

private:
    IDirect3DQuery9*      pNextEndFrameFence;
    Ptr<IDirect3DDevice9> pDevice;
};

}}} // Scaleform::Render::D3D9

#endif 
