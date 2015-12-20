// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

/**
 * Delete all files matching the given wildcard in folderPath; does not recurse to sub-directories.
 */
void deleteFiles(const char* folderPath, const char* wildcard);

/**
 * load up the contents of 'filename' and check to see if it matches the 'contentsToMatch' data
 * exactly - returning true if it does match, false if not
 */
bool fileContentsCompare(const char* filename, const char* contentsToMatch);
