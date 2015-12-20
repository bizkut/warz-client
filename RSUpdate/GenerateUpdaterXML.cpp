#include "r3dPCH.h"
#include "r3d.h"
#include "FileSystem/r3dFSCompress.h"

#include "GenerateUpdaterXML.h"
#include "UPDATER_CONFIG.h"

/*
<p>
 <u file="RSUpdate.exe" crc="0"/>
</p>
*/

void GenerateUpdaterXML()
{
  char exeMain[MAX_PATH];
  GetModuleFileName(NULL, exeMain, sizeof(exeMain));

  const char* fname = strrchr(exeMain, '\\') + 1;
  r3d_assert((DWORD)fname != 1);

  DWORD size;
  DWORD crc32;
  if(!r3dGetFileCrc32(exeMain, &crc32, &size))
    r3dError("failed to get crc for %s\n", exeMain);

  char newExeName[MAX_PATH];
  sprintf(newExeName, "%s_%s.exe", fname, UPDATER_VERSION);
  if(CopyFile(fname, newExeName, FALSE) == 0)
    r3dError("unable to copy %s->%s\n", fname, newExeName);

  const char* updater_file = strrchr(UPDATE_UPDATER_URL, '/');
  r3d_assert(updater_file);
  FILE* f = fopen_for_write(updater_file+1, "wt");
  
  fprintf(f, "<p>\n");
  fprintf(f, " <u url=\"%s%s\" crc=\"%u\" ver=\"%s\" size=\"%u\"/>\n", 
    UPDATE_UPDATER_HOST, newExeName, 
    crc32, 
    UPDATER_VERSION,
    size
    );
  fprintf(f, "</p>\n");
  
  fclose(f);
  
  char msg[1024];
  sprintf(msg, "XML %s generated\nnew exe:%s", updater_file+1, newExeName);
  MessageBox(NULL, msg, "Updater XML", MB_OK);
  return;
}


