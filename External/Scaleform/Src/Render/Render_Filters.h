/**************************************************************************

PublicHeader:   Render
Filename    :   Render_Filterss.h
Content     :   Filter and FilterSet class declarations.
Created     :   May 17, 2011
Authors     :   Michael Antonov

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_SF_Render_Filters_H
#define INC_SF_Render_Filters_H

#include "Kernel/SF_Array.h"
#include "Kernel/SF_RefCount.h"
#include "Kernel/SF_Math.h"

#include "Render/Render_FilterParams.h"
#include "Render/Render_Twips.h"

namespace Scaleform { namespace Render {

//--------------------------------------------------------------------
// ***** Filter

// Allocatable Filter base class. Filters are combined into FilterSets
// and associated with FilterState(s).
// A filter can be modified until it is frozen, at which point it
// becomes read-only for thread safety reasons.

class Filter : public RefCountBase<Filter, Stat_Default_Mem>
{
public:
    Filter(FilterType type)
        : Type(type), Frozen(false)
    { }

    FilterType GetFilterType() const { return Type; }

    // Determine if the filter was frozen for thread-safety reasons.
    // Once frozen, filter can no longer be modified.
    bool    IsFrozen() const { return Frozen; }
    void    Freeze() { Frozen = true;}

    // Creates a new, un-frozen copy of the filter.
    virtual Filter* Clone(MemoryHeap *heap = 0) const = 0;

    // Depending on parameters, filters may not produce any visible effect, but
    // still may be enabled. This function determines whether this is the case.
    virtual bool    IsContributing() const = 0;

protected:    
    FilterType Type;
    bool       Frozen;
};


// BlurFilterImpl is a general implementation for a blur family of filters,
// which include Glow and Shadow functionality.

class BlurFilterImpl : public Filter
{
public:
    BlurFilterImpl(FilterType type)
        : Filter(type), Distance(0.0f), Angle(0.0f)
    { 
        Params.Mode &= ~BlurFilterParams::Mode_FilterMask;
        Params.Mode |= type;
        SF_ASSERT(type <= Filter_Blur_End); 
    }
    BlurFilterImpl(FilterType type, const BlurFilterParams& params)
        : Filter(type), Params(params), Distance(0.0f), Angle(0.0f)
    {
    }

    // TBD: Add filter accessors.
    const BlurFilterParams& GetParams() const { return Params; }
    BlurFilterParams& GetParams()             { SF_ASSERT(!IsFrozen()); return Params; }

    void    SetParams(const BlurFilterParams& params)
    {
        SF_ASSERT(!IsFrozen());
        Params = params;
    }
    void    SetFlags(unsigned mode)
    {
        // Could also check if the mode is valid for the given filter type.
        Params.Mode &= ~BlurFilterParams::Mode_FlagsMask;
        Params.Mode |= mode;
        SF_ASSERT((mode & ~BlurFilterParams::Mode_FlagsMask) == 0 );
    }
    unsigned GetFlags() const
    {
        return Params.Mode & BlurFilterParams::Mode_FlagsMask;
    }

    float   GetDistance() const   { return Distance; }
    float   GetAngle() const      { return Angle; }
    PointF  GetOffset() const   { return Params.Offset; }
    
    void  SetAngleDistance(float angle, float distance)
    {
        SF_ASSERT(!IsFrozen());
        Angle = angle;
        Distance = distance;
        Params.Offset = BlurFilterParams::CalcOffsetByAngleDistance(angle, distance);
    }

protected:
    BlurFilterParams Params;
    // Distance and Angle (in radians) are stored here to preserve value,
    // as they are redundant with Params.Offset.
    float            Distance, Angle;
};


class BlurFilter : public BlurFilterImpl
{
public:
    BlurFilter(float blurx = 4.0f, float blury = 4.0f, unsigned passes = 1)
        : BlurFilterImpl(Filter_Blur)
    {
        Params.BlurX = PixelsToTwips(blurx);
        Params.BlurY = PixelsToTwips(blury);
        Params.Passes = passes;
    }
    BlurFilter(const BlurFilterParams& params)
        : BlurFilterImpl(Filter_Blur, params)
    {

    }

    BlurFilter& operator=(const BlurFilter& rhs)
    {
        SetParams(rhs.GetParams());
        return *this;
    }

    virtual Filter* Clone(MemoryHeap *heap = 0) const;
    virtual bool    IsContributing() const;
};

class ShadowFilter : public BlurFilterImpl
{
public:
    ShadowFilter(float angle = SF_DEGTORAD(45.0f), float dist = 4.0f,
                 float blurx = 4.0f, float blury = 4.0f, 
                 unsigned passes = 1) : 
        BlurFilterImpl(Filter_Shadow)
    {
        Params.BlurX  = PixelsToTwips(blurx);
        Params.BlurY  = PixelsToTwips(blury);
        Params.Passes = passes;
        SetAngleDistance(angle, PixelsToTwips(dist));
        Params.Colors[0].SetRGBA(0,0,0,0xFF);
    }

    ShadowFilter( const BlurFilterParams& params, float angle, float dist) :
        BlurFilterImpl(Filter_Shadow, params)
    {
        SetAngleDistance(angle, PixelsToTwips(dist));        
    }

    ShadowFilter& operator=(const ShadowFilter& rhs)
    {
        SetParams(rhs.GetParams());
        SetAngleDistance(rhs.GetAngle(), rhs.GetDistance());
        return *this;
    }

    virtual Filter* Clone(MemoryHeap *heap = 0) const;
    virtual bool    IsContributing() const;
};

class GlowFilter : public BlurFilterImpl
{
public:
    GlowFilter(float blurx = 6.0f, float blury = 6.0f, 
               unsigned passes = 1) : 
        BlurFilterImpl(Filter_Glow)
    {
        Params.BlurX  = PixelsToTwips(blurx);
        Params.BlurY  = PixelsToTwips(blury);
        Params.Passes = passes;
        Params.Offset = PointF(0,0);
        Params.Colors[0].SetRGBA(0xFF,0,0,0xFF);
        Params.Strength = 2.0f;
    }
    GlowFilter(const BlurFilterParams& params) :
        BlurFilterImpl(Filter_Glow, params)
    {
    }

    GlowFilter& operator=(const GlowFilter& rhs)
    {
        SetParams(rhs.GetParams());
        return *this;
    }

    virtual Filter* Clone(MemoryHeap *heap = 0) const;
    virtual bool    IsContributing() const;
};

class BevelFilter : public BlurFilterImpl
{
public:
    BevelFilter(float blurx = 4.0f, float blury = 4.0f, 
               unsigned passes = 1) : 
        BlurFilterImpl(Filter_Bevel)
    {
        Params.BlurX  = PixelsToTwips(blurx);
        Params.BlurY  = PixelsToTwips(blury);
        Params.Passes = passes;
        SetAngleDistance((float)SF_DEGTORAD(45.0f), PixelsToTwips(4.0f));
        Params.Mode   |= (BlurFilterParams::Mode_Inner | BlurFilterParams::Mode_Highlight);
        Params.Colors[0].SetRGBA(0,0,0,0xFF);
        Params.Colors[1].SetRGBA(0xFF,0xFF,0xFF,0xFF);
    }

    BevelFilter(const BlurFilterParams& params, float angle, float dist) :
        BlurFilterImpl(Filter_Bevel, params)
    {
        // Force highlight mode for bevel, LoadFilters doesn't always set it apparently.
        Params.Mode |= BlurFilterParams::Mode_Highlight;
        SetAngleDistance(angle, PixelsToTwips(dist));
    }

    BevelFilter& operator=(const BevelFilter& rhs)
    {
        SetParams(rhs.GetParams());
        SetAngleDistance(rhs.GetAngle(), rhs.GetDistance());
        return *this;
    }

    virtual Filter* Clone(MemoryHeap *heap = 0) const;
    virtual bool    IsContributing() const;
};

// ColorMatrixFilter represents a 4x5 color transform, applied as follows:
//    red   = (F[0]  * srcR) + (F[1]  * srcG) + (F[2]  * srcB) + (F[3]  * srcA) + F[16]
//    green = (F[4]  * srcR) + (F[5]  * srcG) + (F[6]  * srcB) + (F[7]  * srcA) + F[17]
//    blue  = (F[8]  * srcR) + (F[9]  * srcG) + (F[10] * srcB) + (F[11] * srcA) + F[18]
//    alpha = (F[12] * srcR) + (F[13] * srcG) + (F[14] * srcB) + (F[15] * srcA) + F[19]

class ColorMatrixFilter : public Filter
{
public:
    ColorMatrixFilter();
    ~ColorMatrixFilter();

    static const unsigned ColorMatrixEntries = 20;

    float operator [] (unsigned i) const 
    { 
        return i < ColorMatrixEntries ? MatrixData[i] : 0.0f; 
    }
    float& operator [] (unsigned i)      
    { 
        SF_ASSERT(!IsFrozen()); 
        SF_ASSERT(i < ColorMatrixEntries);
        return MatrixData[i]; 
    }
    
    virtual Filter* Clone(MemoryHeap *heap = 0) const;

    ColorMatrixFilter& operator=(const ColorMatrixFilter& rhs)
    {
        memcpy(MatrixData, rhs.MatrixData, sizeof MatrixData);
        return *this;
    }

    virtual bool    IsContributing() const;

protected:
    float MatrixData[ColorMatrixEntries];
};



//--------------------------------------------------------------------
// FilterSet describes an array of filters that can be stored within a
// a FilterState. Filters within a FilterSet can be modified, added and/or removed
// until the FilterSet is frozen, at which point is becomes read-only
// for thread safety reasons.

class FilterSet : public RefCountBase<FilterSet, Stat_Default_Mem>
{
public:
    FilterSet(Filter* filter = 0);
    ~FilterSet();

    // Determine if the filter was frozen for thread-safety reasons.
    // Once frozen, filter can no longer be modified.
    bool    IsFrozen() const { return Frozen; }
    void    Freeze();

    unsigned      GetFilterCount() const { return (unsigned)Filters.GetSize(); }
    const Filter* GetFilter(UPInt index) const { return Filters[index].GetPtr(); }
    void          SetFilter(UPInt index, Filter* filter)
    { SF_ASSERT(!IsFrozen()); Filters[index] = filter; }
    
    // Filter array access
    void          AddFilter(Filter* filter)
    {
        SF_ASSERT(!IsFrozen());
        Filters.PushBack(filter);
    }
    void          InsertFilterAt(UPInt index, Filter* filter)
    {
        SF_ASSERT(!IsFrozen());
        Filters.InsertAt(index, filter);
    }
    void          RemoveFilterAt(UPInt index)
    {
        SF_ASSERT(!IsFrozen());
        Filters.RemoveAt(index);
    }

    // Removes all copies of the filter from the set.
    void          RemoveFilter(Filter* filter);

    const Array<Ptr<Filter> >& GetFilters() const
    {
        return Filters;
    }
    
    // Clones a filter set. If deepCopy == true, individual filters
    // are also cloned.
    FilterSet*    Clone(bool deepCopy = true, MemoryHeap *heap = 0) const;

    // Returns true if any filter within the set contributes.
    bool          IsContributing() const;

protected:    
    Array<Ptr<Filter> > Filters;
    bool                Frozen;
};


}} // Scaleform::Render

#endif
