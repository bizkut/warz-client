/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_NavGraphLinkInfo_DisplayListBuilder_H
#define Navigation_NavGraphLinkInfo_DisplayListBuilder_H

namespace Kaim
{

class ScopedDisplayList;
class NavGraphVertexSpatializationBlob;
class NavGraphArrayLinkInfoBlob;
class NavGraphLinkInfoBlob;
class VisualShapeColor;

class NavGraphLinksDisplayListBuilder
{
public:
	NavGraphLinksDisplayListBuilder()
	{
		m_displaySpatialization = true;
	}

	/// Display all links of each NavGraph in the array.
	/// Calls DisplayNavGraphLinkInfo for each link, which call DisplayNavGraphVertexSpatialization if m_displaySpatialization is true.
	void DisplayNavGraphArrayLinkInfo(ScopedDisplayList* displayList, const NavGraphArrayLinkInfoBlob* graphArrayLinkInfoBlob);

public: // Internal

	// Display navGraph links and eventually calls DisplayNavGraphVertexSpatialization if m_displaySpatialization is true.
	void DisplayNavGraphLinkInfo(ScopedDisplayList* displayList, const NavGraphLinkInfoBlob* linkInfoBlobs);

	void DisplayNavGraphVertexSpatialization(ScopedDisplayList* displayList,
		const NavGraphVertexSpatializationBlob* spatialization,
		const VisualShapeColor& color);

public:
	bool m_displaySpatialization;
};

}

#endif