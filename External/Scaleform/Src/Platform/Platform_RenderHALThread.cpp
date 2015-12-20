/**************************************************************************

Filename    :   Platform_RenderHALThread.cpp
Content     :   Default RenderThread implementation used by applications.
Created     :   Jan 2011
Authors     :   Michael Antonov

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "Platform_RenderHALThread.h"
#include "Render/Render_MeshCache.h"

namespace Scaleform { namespace Platform {

//------------------------------------------------------------------------
// ***** RenderHALThread

RenderHALThread::RenderHALThread(RTCommandQueue::ThreadingType threadingType)
: Thread(128 * 1024, 1), // Default stack size, create on processor #1.
  RTCommandQueue(threadingType),
  pDevice(0),
  pTextureManager(0),
  Status(Device_NeedInit),
  Wireframe(false),
  Frames(0),
  BGColor(0),
  DrawFrameEnqueued(false),
  RTBlocked(false), RTResume(false), RTBlockedFlag(false),
  CursorState(true),
  WatchDogTrigger(1),
  WatchDogThread(watchDogThreadFn, (void*)&WatchDogTrigger)
{        
    memset(CursorPrims, 0, sizeof CursorPrims);

    //ToleranceParams p;
    //p.StrokeLowerScale = 0.99f;
    //p.StrokeUpperScale = 1.01f;
    //p.StrokeLowerScale = 0.9f;
    //p.StrokeUpperScale = 1.1f;
    //pRenderer->SetToleranceParams(p);
}

int RenderHALThread::Run()
{
    RTCommandBuffer cmd;

    do {
        if (PopCommand(&cmd))
        {
            cmd.Execute(*this);
            if (cmd.NeedsWait())
                cmd.GetNotifier()->Notify();
        }   

        AtomicOps<unsigned>::Store_Release(&WatchDogTrigger, 1);

    } while(!IsProcessingStopped());
    
    return 0;
}

// RTBlockScope helper class used to block RenderThread for duration of its scope.
RenderHALThread::RTBlockScope::RTBlockScope(RenderHALThread* thread)
: pThread(thread)
{
    // If the RT is already blocked, don't ask for it to block again.
    if ( !pThread->RTBlockedFlag )
    {
        pThread->PushCall(&RenderHALThread::blockForGraphicsInit);
        pThread->RTBlocked.Wait();
    }
    pThread->RTBlockedFlag = true;
}

RenderHALThread::RTBlockScope::~RTBlockScope()
{
    pThread->RTBlockedFlag = false;
    pThread->RTResume.PulseEvent();
}


bool RenderHALThread::AdjustViewConfig(ViewConfig* config)
{
    SF_ASSERT(pDevice);
    bool ok = false;
    if (!pDevice->GraphicsConfigOnMainThread() || IsSingleThreaded())
    {
        PushCallAndWaitResult(&RenderHALThread::adjustViewConfig, &ok, config);
    }
    else
    {
        RTBlockScope rtblock(this);
        ok = adjustViewConfig(config);
    }
    return ok;
}

bool RenderHALThread::InitGraphics(const Platform::ViewConfig& config, Device::Window* window,
                                   ThreadId renderThreadId)
{
    SF_ASSERT(pDevice);
    bool ok = false;
    if (!pDevice->GraphicsConfigOnMainThread() || IsSingleThreaded())
    {
        PushCallAndWaitResult(&RenderHALThread::initGraphics, &ok, config, window, renderThreadId);
    }
    else
    {
        RTBlockScope rtblock(this);
        ok = initGraphics(config, window, renderThreadId);
    }

    return ok;
}

bool RenderHALThread::ReconfigureGraphics(const Platform::ViewConfig& config)
{
    SF_ASSERT(pDevice);
    bool ok = false;
    if (!pDevice->GraphicsConfigOnMainThread() || IsSingleThreaded())
    {
        PushCallAndWaitResult(&RenderHALThread::reconfigureGraphics, &ok, config);
    }
    else
    {
        RTBlockScope rtblock(this);
        ok = reconfigureGraphics(config);
    }
    return ok;
}

void RenderHALThread::DestroyGraphics()
{
    SF_ASSERT(pDevice);

    if (!pDevice->GraphicsConfigOnMainThread() || IsSingleThreaded())
    {
        PushCallAndWait(&RenderHALThread::destroyGraphics);
    }
    else
    {
        RTBlockScope rtblock(this);
        destroyGraphics();
    }
}

void RenderHALThread::UpdateDeviceStatus()
{ 
    if (!pDevice->GraphicsConfigOnMainThread() || IsSingleThreaded())
    {
        // TestCooperativeLevel must be called on the same thread
        // in D3D9 to detect Restores.
        PushCallAndWait(&RenderHALThread::updateDeviceStatus);
    }
    else
    {
        RTBlockScope rtblock(this);
        updateDeviceStatus();
    }
}


void RenderHALThread::UpdateConfiguration()
{ 
    if (GetCurrentThreadId() == GetThreadId())
    {
        updateConfiguration();
    }
    else
    {
        PushCallAndWait(&RenderHALThread::updateConfiguration);
    }
}


void RenderHALThread::DrawFrame()
{
    PushCall(&RenderHALThread::drawFrame);
    DrawFrameEnqueued = true;
}

void RenderHALThread::WaitForOutstandingDrawFrame()
{
    if (DrawFrameEnqueued && !IsSingleThreaded())
    {
        DrawFrameDone.Wait();
        DrawFrameEnqueued = false;
    }
}

bool RenderHALThread::adjustViewConfig(Platform::ViewConfig* config)
{
    return pDevice->AdjustViewConfig(config);
}

bool RenderHALThread::initGraphics(const Platform::ViewConfig& config, Device::Window* window,
                                   ThreadId renderThreadId)
{
    if (pDevice->InitGraphics(config, window, renderThreadId))
    {
        ViewSize = config.ViewSize;
        pTextureManager = getHAL()->GetTextureManager();
        updateDeviceStatus();
        VConfig = config;
        pWindow = window;
        return true;
    }
    return false;
}

bool RenderHALThread::reconfigureGraphics(const Platform::ViewConfig& config)
{
    if (pDevice->ReconfigureGraphics(config))
    {
        ViewSize = config.ViewSize;
        updateDeviceStatus();
        VConfig = config;
        return true;
    }
    updateDeviceStatus();
    return false;
}

void RenderHALThread::destroyGraphics()
{       
    pTextureManager.Clear();
    pDevice->ShutdownGraphics();
    updateDeviceStatus();
    VConfig = ViewConfig();
}

void RenderHALThread::blockForGraphicsInit()
{
    RTBlocked.PulseEvent();
    RTResume.Wait();
}

void RenderHALThread::exitThread()
{   // Just in case.
    destroyGraphics();
    StopQueueProcessing();
}

void RenderHALThread::updateCursor(const Point<int> mousePos, SystemCursorState state)
{
    CursorState = state;
    for ( int i = 0; i < Cursor_Type_Count; i++ )
    {
        if ( !CursorMats[i].IsNull() )
            CursorMats[i].SetMatrix2D(Render::Matrix2F::Translation((float)mousePos.x, (float)mousePos.y));
    }
}

void RenderHALThread::setStereoParams(Render::StereoParams sparams)
{
    getHAL()->SetStereoParams(sparams);
}

void RenderHALThread::getMeshCacheParams(Render::MeshCacheParams* params)
{
    *params = getHAL()->GetMeshCache().GetParams();
}

void RenderHALThread::setMeshCacheParams(const Render::MeshCacheParams& params )
{
    getHAL()->GetMeshCache().SetParams(params);
}

bool RenderHALThread::takeScreenShot(const String& filename)
{
    return pDevice->TakeScreenShot(filename);
}

// ***** Render::ThreadCommandQueue implementation.

void RenderHALThread::PushThreadCommand(Render::ThreadCommand* command)
{
    Ptr<Render::ThreadCommand> cmd = command;
    // Execute commands directly if RT is blocked for init/shutdown service.
    if (RTBlockedFlag)
        cmd->Execute();
    else
        PushCall(&RenderHALThread::executeThreadCommand, cmd);
}
void RenderHALThread::executeThreadCommand(const Ptr<Render::ThreadCommand>& command)
{
    command->Execute();
}

int RenderHALThread::watchDogThreadFn(Thread*, void* triggerAddr)
{
    unsigned* trigger = reinterpret_cast<unsigned*>(triggerAddr);
    unsigned failureCount = 0;
    while (true)
    {
        if (!AtomicOps<unsigned>::CompareAndSet_Sync(trigger, 1, 0))
        {
            failureCount++;
            SF_DEBUG_WARNING1(1, "Watchdog thread unsatisfied (for %d seconds)", (failureCount * WatchDogInterval) / 1000);
            SF_DEBUG_ASSERT1(failureCount < WatchDogMaxFailureCount, "Watchdog thread unsatisfied for too long (%d seconds), "
                "system is likely deadlocked.", (failureCount * WatchDogInterval) / 1000);
            if (failureCount >= WatchDogMaxFailureCount)
                return -1;
        }
        else
            failureCount = 0;

        MSleep(RenderHALThread::WatchDogInterval);
    }
    return 0;
}

}} // Scaleform::Platform
