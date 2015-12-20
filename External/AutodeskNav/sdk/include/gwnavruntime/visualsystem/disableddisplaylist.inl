/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: BRGR - secondary contact: MAMU
#ifndef Navigation_DisabledDisplayList_H
#define Navigation_DisabledDisplayList_H

// We use a separated file from displaylist.h in order to easily maintain both versions by comparing the two files (enableddisplaylist.inl and disableddisplaylist.inl)
#ifdef KY_DISABLE_DISPLAYLIST

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/base/memory.h"

namespace Kaim
{

class DisabledDisplayListManager;

// ScopedDisplayList is used to push text, lines or shapes for rendering in the NavigationLab
// e.g.
// \code {
//   ScopedDisplayList displayList(GetWorld());
//   displayList->InitSingleFrameLifespan("MyDisplayList", "MyGroupOfDisplayLists");
//   DisplayShapeColor color;
//   color.SetOnlyLineColor(Kaim::VisualColor::Red);
//   displayList->PushText(m_head, Kaim::VisualColor::Red, "Error");
//   displayList->PushPoint(m_position, color);
// } // Here the ScopedDisplayList is destroyed and committed to be sent to the NavigationLab and rendered. \endcode
//
// In order to be able to easily find, sort and control ScopedDisplayList in the UI of the NavigationLab:
//  - a name must be provided to be able to control manually the rendering of a ScopedDisplayList,
// so it will serve as a label for a check box inside the NavigationLab UI
//  - a group identifier must be provided, it can be either a string or a WorldElement's VisualDebugId but it can't be both,
// if by mistake both would be provided the string will be ignored
//    * If a WorldElement Id is given, the rendering of the displayList in the NavigationLab is configured 
// within the element's display widget. This allows for example to render some bot custom information only for a selected Bot.
//    * If it is a string, ScopedDisplayLists sharing this string will use it as a name for the display widget in the NavigationLab
// where all these ScopedDiaplyLists will be gathered.
// In case of several ScopedDisplayList were sent with the same name and group identifier in the same frame,
// they will be controlled all together by the same widget labeled by their common name inside their common group widget in the NavigationLab UI.
//
// ScopedDisplayList can per frame, i.e. the ScopedDisplayList is sent once and it is rendered only for the frame at which it is sent.
// But lifespan of rendering can be controlled by providing a unique identifier for the ScopedDisplayList.
// This identifier must be unique amongst all ScopedDisplayList, DisplayListManager::GenerateDisplayListId() must be used to get such ids.
// So, instead of sending a ScopedDisplayList each frame to see it over several frame whereas its content doesn't change,
// the ScopedDisplayList can be sent only when its content should change, 
// thus a newly received ScopedDisplayList with an id will replace a previously received ScopedDisplayList
// and the end of the ScopedDisplayList can be triggered whenever you want.
// \code {
//   class MyObject
//   {
//   public:
//     Kaim::World* m_world
//     KyUInt32 m_displayListId;
//     MyObject(Kaim::World* world)
//       : m_world(world)
//       , m_displayListId(KyUInt32MAXVal)
//     {}
//
//     ~MyObject()
//     {
//        // destroy the ScopedDisplayList into the NavigationLab
//        m_world->GetDisplayListManager()->RemoveDisplayList(m_displayListId);
//     }
//
//     void OnChange()
//     {
//         // some relevant changes are done when needed and not each frame
//         // hence display list can be sent only on this event
//         // it will be replaced in the NavigationLab
//         SendVisualdebug()
//     }
//
//     void SendVisualdebug()
//     {
//        ScopedDisplayList displayList(world);
//        // Following line use the id to replace the previous ScopedDisplayList if any by this newly constructed one.
//        // On first call since m_displayListId == KyUInt32MAXVal, 
//        // it will assign m_displayListId with the result of m_world->GetDisplayListManager()->GenerateDisplayListId();
//        m_displayListId = displayList->InitUserControlledLifespan("MyDisplayList", "MyGroupOfDisplayLists", m_displayListId);
//        // ... use displayList as usual
//     }
//   } \endcode
//
// With KY_BUILD_SHIPPING defined, this class is still present but replaced by a fully inlined emptied version.
class DisabledScopedDisplayList
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)
	KY_CLASS_WITHOUT_COPY(DisabledScopedDisplayList)

public:
	// --------------- Constructors -------------
	// Construct a ScopedDisplayList, then, you must call one of the InitSingleFrameLifespan function in order to classify the DisplayList in the NavigationLab.
	// Lifespan of a ScopedDisplayList is determined by the Init function, the ScopedDisplayList will be either available one single frame so you have to sent it over several frames
	// in order to see it several frames, or you can control its lifetime by assigning it an id, that must be provided by the DisplayListManager.
	// ScopedDisplayList with same name and same group will have their display control together under one section and one label in the NavigationLab.
	// however, lifespan remains individual.

	DisabledScopedDisplayList(DisabledDisplayListManager*, DisplayListActivation activation = DisplayList_Enable) { KY_UNUSED(activation); }
	DisabledScopedDisplayList(World*, DisplayListActivation activation = DisplayList_Enable) { KY_UNUSED(activation); } //< same as above world->GetDisplayListManager() is used internally to provide the DisplayListManager

	//--------------- Init for ScopedDisplayList stored only in the frame it was sent -------------
	// A name and a group identifier are mandatory. They allow to sort display lists in the NavigationLab UI and turning their display on/off.
	// A group identifier is either a name or a worldElementID.
	// This ScopedDisplayList will be only available the frame it is sent

	KY_INLINE void InitSingleFrameLifespan(const char*, const char*) {} //< group all displayLists with the same groupName under the same label 
	KY_INLINE void InitSingleFrameLifespan(const char*, KyUInt32) {}  //< associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type
	KY_INLINE void InitSingleFrameLifespan(const char*, const char*, KyUInt32) {}  //< associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type


	// --------------- Init for ScopedDisplayList with User-Controlled Lifespan -------------
	// As above, a name and a group identifier are mandatory.
	// Assigning a lifespan identifier allows to control the lifespan of the display list over several frames,
	// sending back this id via DisplayListManager::RemoveDisplayLIst(displayListId) will stop displaying the list.
	// Not calling this function will display the list for only one frame.
	// If a ScopedDisplayList is sent with the same displayListId the previous one will be replaced.
	// displayListId must be provided by DisplayListManager::GenerateDisplayListId(),
	// or its value must be KyUInt32MAXVal so DisplayListManager::GenerateDisplayListId() will be called
	// and the returned value must be assign to a maintain displayListId e.g.
	// /code {
	//     m_displayListId = KyUInt32MAXVAL; // initialized somewhere, out of the fonction creating the displayList
	//     // ...
	//     // later in your code in a function that creates a displayList:
	//     // so first time you call this function m_displayListId is assigned and used
	//     // next calls to this function will simply use m_displayListId
	//     m_displayListId = scopedDisplayList.InitUserControlledLifespan("myList", "myGroup", m_displayListId);
	//  } /endcode
	//  Alternatively,
	// /code {
	//     m_displayListId = m_world->GetDisplayListManager()->GenerateDisplayListId(); // initialized somewhere, out of the fonction creating the displayList
	//     // ...
	//     // later in your code in a function that creates a displayList:
	//     m_displayListId = scopedDisplayList.InitUserControlledLifespan("myList", "myGroup", m_displayListId); // the return value could be ignored since it is m_displayListId.
	//  } /endcode

	KY_INLINE KyUInt32 InitUserControlledLifespan(const char*, const char*, KyUInt32) { return KyUInt32MAXVAL; }           //< group all displayLists with the same groupName under the same label 
	KY_INLINE KyUInt32 InitUserControlledLifespan(const char*, KyUInt32, KyUInt32) { return KyUInt32MAXVAL; }              //< associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type
	KY_INLINE KyUInt32 InitUserControlledLifespan(const char*, const char*, KyUInt32, KyUInt32) { return KyUInt32MAXVAL; } //< associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type

	// group all displayLists with the same groupName under the same label 
	KY_INLINE void InitUserControlledLifespan(const char*, const char*) {}

	// Associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type.
	// If the same name and worldElementId is used, this display list will overwrite the previous displayList
	// To destroy this display list, call DisplayListManager::RemoveDisplayList(listName);
	// If the element is destroyed, the display list will be automatically destroyed
	// Returns the Id generated from the name
	KY_INLINE void InitUserControlledLifespan_AssociateWithWorldElement(const char*, const char*, KyUInt32) {}

	// --------------- Optional post-Init functions -------------
	// Indicates the state of the displayList the first time it is created in the NavigationLab's Visual Debug Widget.
	// User should consistently call this function over frame to prevent inconsistent behaviors depending on the connection frame!
	// If group identifier is a WorldElement's VisualDebugId both state applies.
	// If group identifier is a string, only notSelectedCheckBox applies (since no element as to be selected).
	// Default values are used if not called.

	KY_INLINE void SetDefaultCheckBoxStateInNavigationLab(DisplayListActivation notSelectedCheckBox = DisplayList_Enable, DisplayListActivation selectedCheckBox = DisplayList_Enable) { KY_UNUSED2(notSelectedCheckBox, selectedCheckBox); }
	KY_INLINE void HideByDefaultInNavigationLab() {}

	//--------------- Shapes -------------

	// Pushes a text.
	inline void PushTextVarg( const Vec3f&, const VisualColor&, const char*, ...) {}
	KY_INLINE void PushText(const Vec3f&, const VisualColor&, const char*, KyUInt32 = 0) {}

	// Pushes a line.
	KY_INLINE void PushLine(const Vec3f&, const Vec3f&, const VisualColor&) {}

	// Pushes a single triangle. 
	KY_INLINE void PushTriangle(const Vec3f&, const Vec3f&, const Vec3f&, const VisualShapeColor&) {}

	// Pushes a single triangle. 
	KY_INLINE void PushTriangle(const Triangle3f&, const VisualShapeColor&) {}

	// Pushes a cylinder whose base is centered on P. 
	KY_INLINE void PushVerticalCylinder(const Vec3f&, KyFloat32, KyFloat32, KyUInt32, const VisualShapeColor&) {}

	// Pushes a segment. P and Q are the centers of the two opposite ends, and halfWidth is half the width of the segment.
	KY_INLINE void PushSegment(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes a flat Quad
	KY_INLINE void PushQuad(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes a flat Quad
	KY_INLINE void PushQuad(const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&, const VisualShapeColor&) {}

	// Pushes an axis-aligned box centered on P. Or, a cross made of lines.
	KY_INLINE void PushPoint(const Vec3f&, const VisualShapeColor&) {}

	// Pushes an axis-aligned box centered on P. Or, a cross made of lines.
	KY_INLINE void PushPoint(const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Push a pyramid composed of a base and four sides. P the is center of the base, Q is the peak. 
	KY_INLINE void PushPyramid(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes a hollow tube with a square cross-section. P and Q are at the center of two opposite faces, and the width of the
	// tube is equal to twicehalfWidth. 
	KY_INLINE void PushSquareTubeSegment( const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes an arrow from P to Q.
	// \param P                 Starting point of the arrow.
	// \param Q                 Ending point of the arrow.
	// \param bodyHalfWidth     Half of the width of the body or trunk of the arrow.
	// \param color             Color of the arrow.
	// \param headWidthRatio    The ratio between the width of the head and the width of the body. In other words, the head or point
	//                           of the arrow will be this many times wider than the body or trunk of the arrow.
	//                           \defaultvalue   3.0f.
	// \param headLengthRatio   The ratio between the length of the head and the length of the body. In other words, the head or point
	//                           of the arrow will be this many times longer than the body or trunk of the arrow.
	//                           \defaultvalue  0.33f.
	KY_INLINE void PushArrow(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&, KyFloat32 = 3.0f, KyFloat32 = 0.33f) {}

	// Pushes a bezier arrow from P to Q.
	// \param P                  Starting point of the arrow.
	// \param Q                  Ending point of the arrow.
	// \param bodyHalfWidth      Half of the width of the body or trunk of the arrow.
	// \param color              Color of the arrow.
	// \param headWidthRatio     The ratio between the width of the head and the width of the body. In other words, the head or point
	//                            of the arrow will be this many times wider than the body or trunk of the arrow.
	//                            \defaultvalue   3.0f
	// \param directionnality    If 0, has no arrow. If 1, has an arrow at the end. If 2, has an arrow at start and end.
	//                            \defaultvalue   0 
	KY_INLINE void PushBezierArrow(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&, KyFloat32 = 3.0f, KyUInt32 = 0) {}

	// Pushes a Christmas tree: two orthogonal triangles with the same central axes. P is the center of the base of
	// both triangles, and Q is at the top of both triangles. Both triangles extend a distance of bodyHalfWidth
	// from the central axes.
	KY_INLINE void PushChristmasTree(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes a silex from P to Q with the specified bodyHalfWidth. 
	KY_INLINE void PushSilex(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes a double silex from P to Q with the specified bodyHalfWidth. Equivalent to a silex from P to the midpoint
	// between P and Q, and a silex from Q to the midpoint between P and Q. 
	KY_INLINE void PushDoubleSilex(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes two orthogonal rectangles with the same central axes. P is the center of one end of both rectangles, and Q 
	// is at the center of the opposite end of both rectangles. Both rectangles extend a distance of bodyHalfWidth
	// from the central axes. 
	KY_INLINE void PushCrossedRectangles(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes an axis-aligned box whose base is centered on P. 
	KY_INLINE void PushColumn(const Vec3f&, KyFloat32, KyFloat32, const VisualShapeColor&) {}

	KY_INLINE void PushOrientedBox(const Transform&, const Box3f&, const VisualShapeColor&) {}
	KY_INLINE void PushOrientedBox2d(const OrientedBox2d&, const VisualShapeColor&) {}

	// Pushes an axis-aligned tetrahedron centered on P and pointing down. 
	KY_INLINE void PushPoint_Tetrahedron(const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes a 2D flat disk centered on P. 
	KY_INLINE void PushDisk(const Vec3f&, KyFloat32, KyUInt32, const VisualShapeColor&) {}

	// Pushes a 2D flat corona centered on P. 
	KY_INLINE void PushCorona(const Vec3f&, KyFloat32, KyFloat32, KyUInt32, const VisualShapeColor&) {}

	KY_INLINE void PushStadium(const Vec3f&, const Vec3f&, KyFloat32, KyUInt32, const VisualShapeColor&) {}

	// Pushes an axis-aligned box. 
	KY_INLINE void PushBox(const Box3f&, const VisualShapeColor&) {}

	/// Pushes a Box from a CellBox using -altitude and +altitude for z values for min and max.
	KY_INLINE void PushCellBox(const Box2i&, const KyFloat32[/*2*/], const DatabaseGenMetrics&, const VisualShapeColor&) {}

	// Pushes a hollow tube with a triangular cross-section. P and Q are at the center of two opposite faces, and the width of the
	// tube is equal to twicehalfWidth. 
	KY_INLINE void PushTriangleTubeSegment(const Vec3f&, const Vec3f&, KyFloat32, const VisualShapeColor&) {}

	// Pushes a three-dimensional oriented box.
	// \param P             A point at one end of the box.
	// \param Q             A point at the opposite end of the box.
	// \param upHeight      The distance of the top of the box above P and Q.
	// \param downHeight    The distance of the bottom of the box below P and Q.
	// \param halfWidth     The distance of the sides of the box on either side ofP and Q. 
	// \param color         Color of the wall.
	KY_INLINE void PushWall(const Vec3f&, const Vec3f&, KyFloat32, KyFloat32, KyFloat32, const VisualShapeColor&) {}

	KY_INLINE void PushLadder(const Transform&, const Box3f&, KyFloat32, KyFloat32, KyFloat32, const VisualShapeColor&) {}

	KY_INLINE void PushParabola(const Kaim::Vec3f&, const Kaim::Vec3f&, KyFloat32, KyUInt32, const VisualShapeColor&) {}

	KY_INLINE void PushFlag(const Vec3f&, KyFloat32, KyFloat32, const VisualShapeColor&) {}

	// ...


private:

public: // internal
	~DisabledScopedDisplayList() {}
	KY_INLINE void Flush() {} //< Flush the ScopedDisplayList to the DisplayListManager, the ScopedDisplayList can be reused.

	KY_INLINE DisabledDisplayListManager* GetManager() { return KY_NULL; }

public: // internal
};


// This class is used for ScopedDisplayLists to send them for remote rendering into the NavigationLab.
// It also tracks the memory used by ScopedDisplayList and reuse it for next ScopedDisplayList, saving some allocations and deallocations.
class DisabledDisplayListManager
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)

public:
	DisabledDisplayListManager() {}
	~DisabledDisplayListManager() {}

	KY_INLINE void SetVisualDebugServer(VisualDebugServer*) {}
	KY_INLINE bool IsVisualDebugSupported() { return false; }

	// Returns a unique display list ID, which you can use when creating a new ScopedDisplayList
	// with a user-controlled lifespan.
	KY_INLINE KyUInt32 GenerateDisplayListId() { return KyUInt32MAXVAL; }

	// Stops rendering the display list with the specified ID in the Navigation Lab.
	KY_INLINE void RemoveDisplayList(KyUInt32) {}
	// Stops rendering the display list with the specified name in the Navigation Lab.
	KY_INLINE void RemoveDisplayList(const char*, const char*, KyUInt32) {}

private:

public: // internal
	KY_INLINE DisplayListMemory* GetUnusedMemory() { return KY_NULL; } // return the first unused memory in m_displayListMemories
	KY_INLINE void PushMemory(DisplayListMemory*) {} // used by a display list to add the content of its assigned memory to m_displayLists
	KY_INLINE void ReleaseMemory(DisplayListMemory*) {} // register this memory has free so that it can be reused by another display list

	// ------------ Rendering functions expected to be used Client-Side in the NavigationLab -------------
	KY_INLINE void PushBlob(const char*, KyUInt32) {}
	KY_INLINE DisplayListBlob* GetNext(KyUInt32&) { return KY_NULL; }
	KY_INLINE void SubmitToRenderer() {} // submit the content of m_displayLists
	KY_INLINE void Clear() {}  // clear the content of m_displayLists
	KY_INLINE void SetVisualGeometry(IVisualGeometry*) {}
	KY_INLINE IVisualGeometry* GetVisualGeometry() { return KY_NULL; }
	KY_INLINE bool IsVisualGeometrySupported() { return false; }
};

}

#else
KY_COMPILE_TIME_MESSAGE("Please include displaylist.h instead of disableddisplaylist.inl")
//KY_COMPILER_ASSERT(0) // make sure this doesn't compile
#endif // KY_DISABLE_DISPLAYLIST

#endif // Navigation_DisabledDisplayList_H
