/**************************************************************************

PublicHeader:   None
Filename    :   GFx_Types.h
Content     :   General GFx Types.
Created     :   August 1, 2010
Authors     :   Artem, Bolgar

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_SF_GFX_TYPES_H
#define INC_SF_GFX_TYPES_H

#include "Kernel/SF_Types.h"
#include "Kernel/SF_Stats.h"
#include "Kernel/SF_Alg.h"
#include "GFx/GFx_PlayerStats.h"
#include "GFx/GFx_Event.h"
#include "GFx/GFx_ASString.h"
#include "Render/Render_Types2D.h"
#include "Render/Render_Color.h"
#include "Render/Render_Matrix2x4.h"
#include "Render/Render_Matrix3x4.h"
#include "Render/Render_Matrix4x4.h"
#include "Render/Render_Twips.h"
#include "Render/Render_CxForm.h"

#define GFX_UNLOAD_TRACE_ENABLED 1

namespace Scaleform { 

namespace Render {
    class GCompoundShape;
    class GRasterizer;
}

namespace GFx {

using Render::Cxform;
using Render::Color;
using Render::Rect;
using Render::Point;
using Render::RectF;
using Render::SizeF;
using Render::Size;
using Render::PointF;
using Render::Matrix2F;
using Render::Matrix3F;
using Render::Matrix4F;
using Render::TwipsToPixels;
using Render::PixelsToTwips;
using Render::GCompoundShape;
using Render::GRasterizer;

// 3-way bool type - Undefined, True, False states
class Bool3W
{
    enum
    {
        B3W_Undefined, B3W_True, B3W_False
    };
    UByte Value;
public:
    inline Bool3W() : Value(B3W_Undefined) {}
    inline Bool3W(bool v) : Value(UByte((v)?B3W_True:B3W_False)) {}
    inline Bool3W(const Bool3W& v): Value(v.Value) {}

    inline Bool3W& operator=(const Bool3W& o)
    {
        Value = o.Value;
        return *this;
    }
    inline Bool3W& operator=(bool o)
    {
        Value = UByte((o) ? B3W_True : B3W_False);
        return *this;
    }
    inline bool operator==(const Bool3W& o) const
    {
        return (Value == B3W_Undefined || o.Value == B3W_Undefined) ? false : Value == o.Value;
    }
    inline bool operator==(bool o) const
    {
        return (Value == B3W_Undefined) ? false : Value == ((o) ? B3W_True : B3W_False);
    }
    inline bool operator!=(const Bool3W& o) const
    {
        return (Value == B3W_Undefined || o.Value == B3W_Undefined) ? false : Value != o.Value;
    }
    inline bool operator!=(bool o) const
    {
        return (Value == B3W_Undefined) ? false : Value != ((o) ? B3W_True : B3W_False);
    }
    inline bool IsDefined() const { return Value != B3W_Undefined; }
    inline bool IsTrue() const    { return Value == B3W_True; }
    inline bool IsFalse() const   { return Value == B3W_False; }
};

enum PlayState
{
    State_Playing,
    State_Stopped
};

// Used to indicate how focus was transfered
enum FocusMovedType
{
    GFx_FocusMovedByMouse     = 1,
    GFx_FocusMovedByKeyboard  = 2,
    GFx_FocusMovedByAS        = 3
};

// Utility functions
template <UInt32 mask>
inline void G_SetFlag(UInt32& pvalue, bool state)
{
    (state) ? (pvalue |= mask) : (pvalue &= (~mask));
}


template <UInt32 mask>
inline bool G_IsFlagSet(UInt32 value)
{
    return (value & mask) != 0;
}


// set 3 way flag: 0 - not set, -1 - false, 1 - true
template <UInt32 shift>
inline void G_Set3WayFlag(UInt32& pvalue, int state)
{
    pvalue = (pvalue & (~(3U<<shift))) | UInt32((state & 3) << shift);
}

template <UInt32 shift>
inline int G_Get3WayFlag(UInt32 value)
{
    UInt32 v = ((value >> shift) & 3);
    return (v == 3) ? -1 : int(v);
}

template <UInt32 shift>
inline bool G_Is3WayFlagTrue(UInt32 value)
{
    return G_Get3WayFlag<shift>(value) == 1;
}

template <UInt32 shift>
inline bool G_Is3WayFlagSet(UInt32 value)
{
    return G_Get3WayFlag<shift>(value) != 0;
}

template <UInt32 shift>
inline bool G_Is3WayFlagFalse(UInt32 value)
{
    return G_Get3WayFlag<shift>(value) != 1;
}

}} // Scaleform::GFx

#endif // INC_SF_GFX_TYPES_H
