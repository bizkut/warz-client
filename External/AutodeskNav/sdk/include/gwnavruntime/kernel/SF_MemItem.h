/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef INC_KY_KERNEL_MEM_ITEM_H
#define INC_KY_KERNEL_MEM_ITEM_H

#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/kernel/SF_File.h"

namespace Kaim {


// Image data, part of MemItem
struct MemItemExtra : public RefCountBase<MemItemExtra, Stat_Default_Mem>
{
    UInt32                              ImageId;
    UInt32                              AtlasId;
    SInt32                              AtlasRectTop;
    SInt32                              AtlasRectBottom;
    SInt32                              AtlasRectLeft;
    SInt32                              AtlasRectRight;

    MemItemExtra(UInt32 imageId) : ImageId(imageId), AtlasId(0), 
        AtlasRectTop(0), AtlasRectBottom(0), AtlasRectLeft(0), AtlasRectRight(0) { }

    // Serialization
    void                        Read(File& str, UInt32 version);
    void                        Write(File& str, UInt32 version) const;
};


// This struct is used for sending the memory report
// It is a tree structure with each node corresponding to a heap
//
struct MemItem : public RefCountBase<MemItem, Stat_Default_Mem>
{
    StringLH                            Name;
    UInt32                              Value;
    bool                                HasValue;
    bool                                StartExpanded;
    UInt32                              ID;
    Ptr<MemItemExtra>                   ImageExtraData;
    ArrayLH< Ptr<MemItem> >             Children;

    MemItem(UInt32 id) : Value(0), HasValue(false), StartExpanded(false), ID(id)  { }

    void                        SetValue(UInt32 memValue);
    MemItem*                    AddChild(UInt32 id, const char* name);
    MemItem*                    AddChild(UInt32 id, const char* name, UInt32 memValue);
    MemItem*                    SearchForName(const char* name);
    UInt32                      SumValues(const char* name) const;
    UInt32                      GetValue(const char* name) const;
    UInt32                      GetMaxId() const;

    // operators
    MemItem&                    operator=(const MemItem& rhs);
    MemItem&                    operator/=(unsigned numFrames);
    MemItem&                    operator*=(unsigned num);
    bool                        Merge(const MemItem& other);

    // Serialization
    void                        Read(File& str, UInt32 version);
    void                        Write(File& str, UInt32 version) const;
    void                        ToString(StringBuffer* report, UByte indent = 0) const;
    void                        ToXml(StringBuffer* report, UByte indent = 0) const;
};

} // namespace Kaim

#endif
