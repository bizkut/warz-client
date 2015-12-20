#include "r3dPCH.h"
#include "r3d.h"

#include "r3dMeshConvert.h"

DWORD r3dGetFileSize(const char* fname)
{
  FILE* f = fopen(fname, "rb");
  if(!f)
    r3dError("can't open %s\n", fname);
    
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fclose(f);
  
  return size;
}

#if !RECONVERT_SCO_MESHES

// copy from r3dObjLS.cpp to avoid linking it
bool getFileTimestamp(const char* fname, FILETIME& writeTime)
{
	HANDLE hFile = CreateFile(fname, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	GetFileTime(hFile, 0, 0, &writeTime);
	CloseHandle(hFile);

	return true;
}

bool convertScoToBinary(const char* sco, const char* scb)
{
  return 0;
}

#else

int convertScoToBinary(const char* sco, const char* scb)
{
  // Load() will convert to binary
  r3dMesh* msh = new r3dMesh(0);
  bool res = msh->Load(sco);

  delete msh;
  if(!res) {
    return 1;
  }
  
  // check that conversion was Ok
  FILETIME t1, t2;
  if(!getFileTimestamp(sco, t1))
    return 2;
  if(!getFileTimestamp(scb, t2))
    return 3;

  if(t1.dwLowDateTime != t2.dwLowDateTime || t1.dwHighDateTime != t2.dwHighDateTime) {
    return 4;
  }
  
  return 0;
}

#endif