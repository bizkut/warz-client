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

#include "Precompiled.h"
#include "FileUtils.h"

// -------------------------------------------------------------------------------------------------------------------
void deleteFiles(const char* folderPath, const char* wildcard)
{
  char fileFound[MAX_PATH];
  WIN32_FIND_DATA info;
  HANDLE hp; 

  NMP_SPRINTF(fileFound, "%s\\%s", folderPath, wildcard);
  hp = FindFirstFile(fileFound, &info);
  if (hp == INVALID_HANDLE_VALUE)
    return;

  do
  {
    NMP_SPRINTF(fileFound, "%s\\%s", folderPath, info.cFileName);
    DeleteFile(fileFound);

  } while(FindNextFile(hp, &info)); 
  FindClose(hp);
}

// -------------------------------------------------------------------------------------------------------------------
bool fileContentsCompare(const char* filename, const char* contentsToMatch)
{
  FILE *myFile = NULL;

  fopen_s(&myFile, filename, "rt");

  if (myFile == 0)
    return false;

  // get size of the file to read in
  fseek(myFile, 0, SEEK_END);
  size_t fileSize = ftell(myFile);
  fseek(myFile, 0, SEEK_SET);
  if (fileSize == 0)
  {
    fclose(myFile);
    return false;
  }

  // load it up
  char* fBuf = (char*)calloc(1, fileSize + 1);
  size_t bRead = fread(fBuf, 1, fileSize, myFile);
  fclose(myFile);

  bool match = (strncmp(fBuf, contentsToMatch, bRead) == 0);
  free(fBuf);

  return match;
}
