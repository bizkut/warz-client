/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY

#ifndef Navigation_ChannelArray_Display_H
#define Navigation_ChannelArray_Display_H

namespace Kaim
{

class ScopedDisplayList;
class ChannelArrayBlob;
class ChannelBlob;
class ChannelDisplayConfig;

class ChannelArrayDisplayListBuilder
{
public:
	static void DisplayChannelArray(ScopedDisplayList* displayList, const ChannelArrayBlob* channelArray, const ChannelDisplayConfig& displayConfig);
	static void DisplayChannel     (ScopedDisplayList* displayList, const ChannelBlob* channel          , const ChannelDisplayConfig& displayConfig);
};

} // namespace Kaim

#endif