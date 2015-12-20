/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: BRGR - secondary contact: MAMU
#ifndef Navigation_DisplayList_H
#define Navigation_DisplayList_H

#include "gwnavruntime/visualsystem/displaylistblob.h"

namespace Kaim
{

class DisplayListMemory;
class IVisualGeometry;
class VisualDebugServer;
class World;
class Triangle3f;
class Vec3f;
class Box3f;
class Box2i;
class Transform;
class OrientedBox2d;
class DatabaseGenMetrics;


/// Indicates if the display list is active or not.
/// If DisplayList_Disable is used the ScopedDisplayList won't be sent to the NavigationLab,
/// this is useful when you want to dynamically switch on/off the use of the ScopedDisplayList.
enum DisplayListActivation 
{
	DisplayList_Disable, ///< Calls on the ScopedDisplayList will do nothing
	DisplayList_Enable,  ///< The display list is activated
};

}

// Define KY_DISABLE_DISPLAYLIST to disable ScopedDisplayList and DisplayListManager
// In that case these classes will be replaced by fully inlined classes, DisabledScopedDisplayList and DisabledDisplayListManager, 
// thanks to the define at the end of this file.
// If ScopedDisplayList and DisplayListManager are modified, please apply the changes into disableddisplaylist.inl as well.
#ifdef KY_BUILD_SHIPPING
# define KY_DISABLE_DISPLAYLIST
#endif

#ifndef KY_DISABLE_DISPLAYLIST

#include "gwnavruntime/blob/blobhandler.h"

namespace Kaim
{

class DisplayListManager;

/// ScopedDisplayList is used to push text, lines or shapes for rendering in the NavigationLab
/// e.g.
/// \code {
///   ScopedDisplayList displayList(GetWorld());
///   displayList->InitSingleFrameLifespan("MyDisplayList", "MyGroupOfDisplayLists");
///   DisplayShapeColor color;
///   color.SetOnlyLineColor(Kaim::VisualColor::Red);
///   displayList->PushText(m_head, Kaim::VisualColor::Red, "Error");
///   displayList->PushPoint(m_position, color);
/// } // Here the ScopedDisplayList is destroyed and committed to be sent to the NavigationLab and rendered. \endcode
///
/// In order to be able to easily find, sort and control ScopedDisplayList in the UI of the NavigationLab:
///  - a name must be provided to be able to control manually the rendering of a ScopedDisplayList,
/// so it will serve as a label for a check box inside the NavigationLab UI
///  - a group identifier must be provided, it can be either a string or a WorldElement's VisualDebugId but it can't be both,
/// if by mistake both would be provided the string will be ignored
///    * If a WorldElement Id is given, the rendering of the displayList in the NavigationLab is configured 
/// within the element's display widget. This allows for example to render some bot custom information only for a selected Bot.
///    * If it is a string, ScopedDisplayLists sharing this string will use it as a name for the display widget in the NavigationLab
/// where all these ScopedDiaplyLists will be gathered.
/// In case of several ScopedDisplayList were sent with the same name and group identifier in the same frame,
/// they will be controlled all together by the same widget labeled by their common name inside their common group widget in the NavigationLab UI.
///
/// ScopedDisplayList can per frame, i.e. the ScopedDisplayList is sent once and it is rendered only for the frame at which it is sent.
/// But lifespan of rendering can be controlled by providing a unique identifier for the ScopedDisplayList.
/// This identifier must be unique amongst all ScopedDisplayList, DisplayListManager::GenerateDisplayListId() must be used to get such ids.
/// So, instead of sending a ScopedDisplayList each frame to see it over several frame whereas its content doesn't change,
/// the ScopedDisplayList can be sent only when its content should change, 
/// thus a newly received ScopedDisplayList with an id will replace a previously received ScopedDisplayList
/// and the end of the ScopedDisplayList can be triggered whenever you want.
/// \code {
///   class MyObject
///   {
///   public:
///     Kaim::World* m_world
///     KyUInt32 m_displayListId;
///     MyObject(Kaim::World* world)
///       : m_world(world)
///       , m_displayListId(KyUInt32MAXVal)
///     {}
///
///     ~MyObject()
///     {
///        // destroy the ScopedDisplayList into the NavigationLab
///        m_world->GetDisplayListManager()->RemoveDisplayList(m_displayListId);
///     }
///
///     void OnChange()
///     {
///         // some relevant changes are done when needed and not each frame
///         // hence display list can be sent only on this event
///         // it will be replaced in the NavigationLab
///         SendVisualdebug()
///     }
///
///     void SendVisualdebug()
///     {
///        ScopedDisplayList displayList(world);
///        // Following line use the id to replace the previous ScopedDisplayList if any by this newly constructed one.
///        // On first call since m_displayListId == KyUInt32MAXVal, 
///        // it will assign m_displayListId with the result of m_world->GetDisplayListManager()->GenerateDisplayListId();
///        m_displayListId = displayList->InitUserControlledLifespan("MyDisplayList", "MyGroupOfDisplayLists", m_displayListId);
///        // ... use displayList as usual
///     }
///   } \endcode
///
/// With KY_BUILD_SHIPPING defined, this class is still present but replaced by a fully inlined emptied version.
class ScopedDisplayList
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)
	KY_CLASS_WITHOUT_COPY(ScopedDisplayList)

public:
	// --------------- Constructors -------------
	/// Construct a ScopedDisplayList, then, you must call one of the InitSingleFrameLifespan function in order to classify the DisplayList in the NavigationLab.
	/// Lifespan of a ScopedDisplayList is determined by the Init function, the ScopedDisplayList will be either available one single frame so you have to send it over several frames
	/// in order to see it several frames, or you can control its lifetime by assigning it an id, that must be provided by the DisplayListManager.
	/// ScopedDisplayList with same name and same group will have their display control together under one section and one label in the NavigationLab.
	/// however, lifespan remains individual.

	ScopedDisplayList(DisplayListManager* manager, DisplayListActivation activation = DisplayList_Enable);
	ScopedDisplayList(World* world, DisplayListActivation activation = DisplayList_Enable); ///< same as above, world->GetDisplayListManager() is used internally to provide the DisplayListManager

	// --------------- Init for ScopedDisplayList stored only in the frame it was sent -------------
	/// A name and a group identifier are mandatory. They allow to sort display lists in the NavigationLab UI and turning their display on/off.
	/// A group identifier is either a name or a worldElementID.
	/// This ScopedDisplayList will be only available the frame it is sent

	void InitSingleFrameLifespan(const char listName[64], const char groupName[64]); ///< group all displayLists with the same groupName under the same label 
	void InitSingleFrameLifespan(const char listName[64], KyUInt32 worldElementId);  ///< associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type
	void InitSingleFrameLifespan(const char listName[64], const char groupName[64], KyUInt32 worldElementId);  ///< associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type


	// --------------- Init for ScopedDisplayList with User-Controlled Lifespan -------------
	/// As above, a name and a group identifier are mandatory.
	/// Assigning a lifespan identifier allows to control the lifespan of the display list over several frames,
	/// sending back this id via DisplayListManager::RemoveDisplayLIst(displayListId) will stop displaying the list.
	/// Not calling this function will display the list for only one frame.
	/// If a ScopedDisplayList is sent with the same displayListId the previous one will be replaced.
	/// displayListId must be provided by DisplayListManager::GenerateDisplayListId(),
	/// or its value must be KyUInt32MAXVal so DisplayListManager::GenerateDisplayListId() will be called
	/// and the returned value must be assign to a maintained displayListId e.g.
	/// /code {
	///     m_displayListId = KyUInt32MAXVAL; // initialized somewhere, out of the fonction creating the displayList
	///     // ...
	///     // later in your code in a function that creates a displayList:
	///     // so first time you call this function m_displayListId is assigned and used
	///     // next calls to this function will simply use m_displayListId
	///     m_displayListId = scopedDisplayList.InitUserControlledLifespan("myList", "myGroup", m_displayListId);
	///  } /endcode
	///  Alternatively,
	/// /code {
	///     m_displayListId = m_world->GetDisplayListManager()->GenerateDisplayListId(); // initialized somewhere, out of the fonction creating the displayList
	///     // ...
	///     // later in your code in a function that creates a displayList:
	///     m_displayListId = scopedDisplayList.InitUserControlledLifespan("myList", "myGroup", m_displayListId); // the return value could be ignored since it is m_displayListId.
	///  } /endcode

	KyUInt32 InitUserControlledLifespan(const char listName[64], const char groupName[64], KyUInt32 displayListId); ///< group all displayLists with the same groupName under the same label 
	KyUInt32 InitUserControlledLifespan(const char listName[64], KyUInt32 worldElementId, KyUInt32 displayListId);  ///< associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type
	KyUInt32 InitUserControlledLifespan(const char listName[64], const char groupName[64], KyUInt32 worldElementId, KyUInt32 displayListId);  ///< associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type

	/// group all displayLists with the same groupName under the same label 
	void InitUserControlledLifespan(const char listName[64], const char groupName[64]);

	/// Associate the displayList to the given world element and so the list can then be turned on/off under the section of the world element type.
	/// If the same name and worldElementId is used, this display list will overwrite the previous displayList
	/// To destroy this display list, call DisplayListManager::RemoveDisplayList(listName);
	/// If the element is destroyed, the display list will be automatically destroyed
	/// Returns the Id generated from the name
	void InitUserControlledLifespan_AssociateWithWorldElement(const char listName[64], const char groupName[64], KyUInt32 worldElementId);

	// --------------- Optional post-Init functions -------------
	/// Indicates the state of the displayList the first time it is created in the NavigationLab's Visual Debug Widget.
	/// User should consistently call this function over frame to prevent inconsistent behaviors depending on the connection frame!
	/// If group identifier is a WorldElement's VisualDebugId both state applies.
	/// If group identifier is a string, only notSelectedCheckBox applies (since no element as to be selected).
	/// Default values are used if not called.

	void SetDefaultCheckBoxStateInNavigationLab(DisplayListActivation notSelectedCheckBox = DisplayList_Enable, DisplayListActivation selectedCheckBox = DisplayList_Enable);
	void HideByDefaultInNavigationLab() { SetDefaultCheckBoxStateInNavigationLab(DisplayList_Disable, DisplayList_Disable); }

	//--------------- Shapes -------------

	/// Pushes a text.
	void PushTextVarg( const Vec3f& pos, const VisualColor& color, const char* textFmt, ...);
	void PushText(const Vec3f& pos, const VisualColor& color, const char* text, KyUInt32 textLength = 0);

	/// Pushes a line.
	void PushLine(const Vec3f& P, const Vec3f& Q, const VisualColor& color);

	/// Pushes a single triangle. 
	void PushTriangle(const Vec3f& A, const Vec3f& B, const Vec3f& C, const VisualShapeColor& color);

	/// Pushes a single triangle. 
	void PushTriangle(const Triangle3f& triangle, const VisualShapeColor& color);

	/// Pushes a cylinder whose base is centered on P. 
	void PushVerticalCylinder(const Vec3f& P, KyFloat32 radius, KyFloat32 height, KyUInt32 subdivisions, const VisualShapeColor& color);

	/// Pushes a segment. P and Q are the centers of the two opposite ends, and halfWidth is half the width of the segment.
	void PushSegment(const Vec3f& A, const Vec3f& B, KyFloat32 radius, const VisualShapeColor& color);

	/// Pushes a flat Quad
	void PushQuad(const Vec3f& A, const Vec3f& B, KyFloat32 radius, const VisualShapeColor& color);

	/// Pushes a flat Quad
	void PushQuad(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& D, const VisualShapeColor& color);

	/// Pushes an axis-aligned box centered on P. Or, a cross made of lines.
	void PushPoint(const Vec3f& P, const VisualShapeColor& color) { PushPoint(P, 0.1f, color); }

	/// Pushes an axis-aligned box centered on P. Or, a cross made of lines.
	void PushPoint(const Vec3f& P, KyFloat32 radius, const VisualShapeColor& color);

	/// Push a pyramid composed of a base and four sides. P the is center of the base, Q is the peak. 
	void PushPyramid(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes a hollow tube with a square cross-section. P and Q are at the center of two opposite faces, and the width of the
	/// tube is equal to twicehalfWidth. 
	void PushSquareTubeSegment( const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes an arrow from P to Q.
	/// \param P                 Starting point of the arrow.
	/// \param Q                 Ending point of the arrow.
	/// \param bodyHalfWidth     Half of the width of the body or trunk of the arrow.
	/// \param color             Color of the arrow.
	/// \param headWidthRatio    The ratio between the width of the head and the width of the body. In other words, the head or point
	///                           of the arrow will be this many times wider than the body or trunk of the arrow.
	///                           \defaultvalue   3.0f.
	/// \param headLengthRatio   The ratio between the length of the head and the length of the body. In other words, the head or point
	///                           of the arrow will be this many times longer than the body or trunk of the arrow.
	///                           \defaultvalue  0.33f.
	void PushArrow(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color, KyFloat32 headWidthRatio = 3.0f, KyFloat32 headLengthRatio = 0.33f);

	/// Pushes a bezier arrow from P to Q.
	/// \param P                  Starting point of the arrow.
	/// \param Q                  Ending point of the arrow.
	/// \param bodyHalfWidth      Half of the width of the body or trunk of the arrow.
	/// \param color              Color of the arrow.
	/// \param headWidthRatio     The ratio between the width of the head and the width of the body. In other words, the head or point
	///                            of the arrow will be this many times wider than the body or trunk of the arrow.
	///                            \defaultvalue   3.0f
	/// \param directionnality    If 0, has no arrow. If 1, has an arrow at the end. If 2, has an arrow at start and end.
	///                            \defaultvalue   0 
	void PushBezierArrow(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color, KyFloat32 headWidthRatio = 3.0f, KyUInt32 directionnality = 0);

	/// Pushes a Christmas tree: two orthogonal triangles with the same central axes. P is the center of the base of
	/// both triangles, and Q is at the top of both triangles. Both triangles extend a distance of bodyHalfWidth
	/// from the central axes.
	void PushChristmasTree(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);

	/// Pushes a silex from P to Q with the specified bodyHalfWidth. 
	void PushSilex(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);

	/// Pushes a double silex from P to Q with the specified bodyHalfWidth. Equivalent to a silex from P to the midpoint
	/// between P and Q, and a silex from Q to the midpoint between P and Q. 
	void PushDoubleSilex(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);

	/// Pushes two orthogonal rectangles with the same central axes. P is the center of one end of both rectangles, and Q 
	/// is at the center of the opposite end of both rectangles. Both rectangles extend a distance of bodyHalfWidth
	/// from the central axes. 
	void PushCrossedRectangles(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);

	/// Pushes an axis-aligned box whose base is centered on P. 
	void PushColumn(const Vec3f& P, KyFloat32 halfWidth, KyFloat32 height, const VisualShapeColor& color);

	void PushOrientedBox(const Transform& transform, const Box3f& extents, const VisualShapeColor& color);
	void PushOrientedBox2d(const OrientedBox2d& orientedBox2d, const VisualShapeColor& color);

	/// Pushes an axis-aligned tetrahedron centered on P and pointing down. 
	void PushPoint_Tetrahedron(const Vec3f& P, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes a 2D flat disk centered on P. 
	void PushDisk(const Vec3f& P, KyFloat32 radius, KyUInt32 subdivisions, const VisualShapeColor& color);

	/// Pushes a 2D flat corona centered on P. 
	void PushCorona(const Vec3f& P, KyFloat32 innerRadius, KyFloat32 outerRadius, KyUInt32 subdivisions, const VisualShapeColor& color);

	void PushStadium(const Vec3f& P, const Vec3f& Q, KyFloat32 radius, KyUInt32 subdivisions, const VisualShapeColor& color);

	/// Pushes an axis-aligned box. 
	void PushBox(const Box3f& box, const VisualShapeColor& color);

	/// Pushes a Box from a CellBox using -altitude and +altitude for z values for min and max.
	void PushCellBox(const Box2i& cellBox, const KyFloat32 altitudeMinMax[/*2*/], const DatabaseGenMetrics& genMetrics, const VisualShapeColor& color);

	/// Pushes a hollow tube with a triangular cross-section. P and Q are at the center of two opposite faces, and the width of the
	/// tube is equal to twicehalfWidth. 
	void PushTriangleTubeSegment(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes a three-dimensional oriented box.
	/// \param P             A point at one end of the box.
	/// \param Q             A point at the opposite end of the box.
	/// \param upHeight      The distance of the top of the box above P and Q.
	/// \param downHeight    The distance of the bottom of the box below P and Q.
	/// \param halfWidth     The distance of the sides of the box on either side of P and Q. 
	/// \param color         Color of the wall.
	void PushWall(const Vec3f& P, const Vec3f& Q, KyFloat32 upHeight, KyFloat32 downHeight, KyFloat32 halfWidth, const VisualShapeColor& color);

	void PushLadder(const Transform& transform, const Box3f& entrances, KyFloat32 ladderWidth, KyFloat32 rungsThickness, KyFloat32 rungsGapSize, const VisualShapeColor& shapeColor);

	void PushParabola(const Kaim::Vec3f& P, const Kaim::Vec3f& Q, KyFloat32 heightBias, KyUInt32 subdivisions, const VisualShapeColor& color);

	void PushFlag(const Vec3f& P, KyFloat32 height, KyFloat32 radius, const VisualShapeColor& color);

	// ...


private:
	void PushSegmentBasedShape(const Vec3f& P, const Vec3f& Q, KyFloat32 size, const VisualShapeColor& color, KyUInt32 displayShapeType);
	void PushCurveBasedShape(const Vec3f& P, const Vec3f& Q, KyFloat32 size, KyFloat32 subdivisions, const VisualShapeColor& color, KyUInt32 displayShapeType);
	void PushText(DisplayText& text);
	void PushShape(DisplayShape& shape);
	KyUInt32 SetLifespanIdentifier(KyUInt32 displayListId);

private:
	KyArrayPOD<char> m_tmpTextBuffer;

public: // internal
	~ScopedDisplayList();
	void Flush(); ///< Flush the ScopedDisplayList to the DisplayListManager, the ScopedDisplayList can be reused.

	DisplayListManager* GetManager() { return m_manager; }

public: // internal
	DisplayListManager* m_manager;
	DisplayListMemory* m_memory; // memory assigned by the manager
};


/// This class is used for ScopedDisplayLists to send them for remote rendering into the NavigationLab.
/// It also tracks the memory used by ScopedDisplayList and reuse it for next ScopedDisplayList, saving some allocations and deallocations.
class DisplayListManager
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)

public:
	DisplayListManager();
	~DisplayListManager();

	void SetVisualDebugServer(VisualDebugServer* visualDebugServer) { m_visualDebugServer = visualDebugServer; }
	bool IsVisualDebugSupported();

	/// Returns a unique display list ID, which you can use when creating a new ScopedDisplayList
	/// with a user-controlled lifespan.
	KyUInt32 GenerateDisplayListId();
	
	/// Stops rendering the display list with the specified ID in the Navigation Lab.
	void RemoveDisplayList(KyUInt32 displayListId);
	/// Stops rendering the display list with the specified name in the Navigation Lab.
	void RemoveDisplayList(const char* displayListName, const char* displayListGroupName, KyUInt32 worldElementId = KyUInt32MAXVAL);

private:
	Ptr<IVisualGeometry> m_visualGeometry; //< intended to be used client-side in the NavigationLab
	VisualDebugServer* m_visualDebugServer;
	KyUInt32 m_currentDisplayListIndex;

	KyArrayPOD<char> m_displayLists;                       // buffer containing the display list blobs
	BlobHandler<DisplayListBlob> m_displayListBlobHandler; // shared handler used to create all display list blobs
	KyArray<DisplayListMemory*> m_displayListMemories;     // all memories used or free, thence if possible a display list does not allocate more memory than already used 

public: // internal
	DisplayListMemory* GetUnusedMemory(); // return the first unused memory in m_displayListMemories
	void PushMemory(DisplayListMemory*); // used by a display list to add the content of its assigned memory to m_displayLists
	void ReleaseMemory(DisplayListMemory*); // register this memory has free so that it can be reused by another display list

	// ------------ Rendering functions expected to be used Client-Side in the NavigationLab -------------
	void PushBlob(const char* displayListBlob, KyUInt32 blobsize);
	DisplayListBlob* GetNext(KyUInt32& sizeRead);
	void Clear();  // clear the content of m_displayLists

	void SubmitToRenderer(); // fill the m_visualGeometry with the content of m_displayLists.
	void SetVisualGeometry(IVisualGeometry* visualGeometry);
	IVisualGeometry* GetVisualGeometry() { return m_visualGeometry; }
	bool IsVisualGeometrySupported() { return m_visualGeometry != KY_NULL; }
};

}

#else
// Here we use a separated file in order to easily maintain both versions by comparing the two files (displaylist.h and disableddisplaylist.inl)
# include "gwnavruntime/visualsystem/disableddisplaylist.inl"

// This allows to forward declare DisplayListManager and ScopedDisplayList
// and to use DisabledDisplayListManager and DisabledScopedDisplayList separately in unit tests
namespace Kaim
{

class DisplayListManager : public DisabledDisplayListManager
{
};

class ScopedDisplayList : public DisabledScopedDisplayList
{
	KY_CLASS_WITHOUT_COPY(ScopedDisplayList)
public:
	ScopedDisplayList(DisplayListManager* manager, DisplayListActivation activation = DisplayList_Disable)
		: DisabledScopedDisplayList(manager, activation)	{}
	ScopedDisplayList(World* world, DisplayListActivation activation = DisplayList_Disable)
		: DisabledScopedDisplayList(world, activation)	{}
	~ScopedDisplayList() {}
};

}

#endif // KY_DISABLE_DISPLAYLIST


#endif // Navigation_DisplayList_H
