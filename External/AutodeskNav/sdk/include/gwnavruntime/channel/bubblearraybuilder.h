/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_BubbleArrayBuilder_H
#define Navigation_BubbleArrayBuilder_H


#include "gwnavruntime/channel/bubblearray.h"


namespace Kaim
{

class DiagonalStrip;

/// This class helps to build a BubbleArray.
///
/// It can be used either in static mode, to directly feed an array corresponding to a
/// DiagonalStrip; or in Dynamic mode. In this last mode, the DiagonalStrip is used
/// to populate an internal Bubble list that can be enriched with additional Bubbles
/// before this completed list is converted to an array of Bubbles.
///
/// In both cases, the Bubble array can then be used as input for a StringPuller.
class BubbleArrayBuilder
{
public:
	BubbleArrayBuilder() {}
	~BubbleArrayBuilder() { Clear(); }

	void Clear();

	// ------------------------------ Static Bubble array construction ------------------------------

	static void ConvertDiagonalStripToBubbleArray(const DiagonalStrip& diagonalStrip, BubbleArray& bubbleArray);


	// ------------------------------ Dynamic Bubble array construction ------------------------------

	/// Use this set of methods when building a Bubble array that will integrate iteratively
	/// additional Bubbles.
	/// The typical usage is:
	///  1. Initialize the BubbleArrayBuilder with a DiagonalStrip
	///  2. Insert the additional Bubbles
	///  3. Convert the internal list into an array of Bubbles that can be used as StringPuller input.

	void Initialize(const DiagonalStrip& diagonalStrip);
	void PushBack(const Bubble& bubble);
	void ConvertToBubbleArray(BubbleArray& bubbleArray);

private:
	class BubbleListNode: public ListNode<BubbleListNode>
	{
	public:
		BubbleListNode() {}
		BubbleListNode(const Bubble& bubble) : m_bubble(bubble) {}
		Bubble m_bubble;
	};

	KyArray<BubbleListNode> m_bubbleListNodeArray;
	List<BubbleListNode> m_bubbleList;
};

} // namespace Kaim

#endif // Navigation_BubbleArrayBuilder_H
