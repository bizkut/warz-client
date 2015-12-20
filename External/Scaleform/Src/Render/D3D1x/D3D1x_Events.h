/**********************************************************************

PublicHeader:   Render
Filename    :   D3D1x_Events.h
Content     :   Implementation of GPU events for use with PIX.
Created     :   Feb 2012
Authors     :   Bart Muzzin

Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

***********************************************************************/

#ifndef INC_SF_D3D1x_Events_H
#define INC_SF_D3D1x_Events_H

#include "Render/Render_Events.h"
#include <windows.h>

namespace Scaleform { namespace Render { namespace D3D1x { 

// Used to create heirarchies in PIX captures.
#if !defined(SF_BUILD_SHIPPING) && !defined(SF_OS_WINMETRO)
    typedef INT (WINAPI * LPD3DPERF_BEGINEVENT)(D3DCOLOR, LPCWSTR);
    typedef INT (WINAPI * LPD3DPERF_ENDEVENT)(void);
#endif

class RenderEvent : public Render::RenderEvent
{
public:
    static void InitializeEvents()
    {
#if !defined(SF_BUILD_SHIPPING) && !defined(SF_OS_WINMETRO)
        HMODULE hdll = LoadLibrary( "D3D9.dll" );
        if (hdll)
        {
            RenderEvent::BeginEventFn = (LPD3DPERF_BEGINEVENT)GetProcAddress( hdll, "D3DPERF_BeginEvent" );
            RenderEvent::EndEventFn   = (LPD3DPERF_ENDEVENT)GetProcAddress( hdll, "D3DPERF_EndEvent" );
        }
        SF_DEBUG_WARNING(hdll == 0, "Failed to load D3D9.dll - PIX captures will be unavailable.");
#endif
    }

    virtual void Begin( String eventName )
    {
#if !defined(SF_BUILD_SHIPPING) && !defined(SF_OS_WINMETRO)
        if ( BeginEventFn )
        {
            // PIX events only take wide-character strings.
            wchar_t dest[256];
            size_t  chars;
            mbstowcs_s(&chars, dest, eventName.ToCStr(), 256);
            BeginEventFn(D3DCOLOR_XRGB(0,0,0), dest);
        }
#endif
    }
    void End()
    {
#if !defined(SF_BUILD_SHIPPING) && !defined(SF_OS_WINMETRO)
        if ( EndEventFn )
            EndEventFn();
#endif
    }

#if !defined(SF_BUILD_SHIPPING) && !defined(SF_OS_WINMETRO)
    static LPD3DPERF_BEGINEVENT  BeginEventFn;
    static LPD3DPERF_ENDEVENT    EndEventFn;
#endif
};

}}} // Scaleform::Render::D3D1x

#endif // INC_SF_D3D1x_Events_H
