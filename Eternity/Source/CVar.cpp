#include "r3dPCH.h"
#include "r3d.h"

//ebana v rot

class CVarContainer
{
  private:
static	CVarContainer	*pInstance;
	CVarContainer();
	~CVarContainer();
  public:
static	CVarContainer*	This();
static	void		Register(CVar *var);

  public:
	r3dgameVector(CVar*)	vList;
  public:
	int		Write(const char* fname, const char* prefix = NULL);
	int		Read(const char* fname);
	CVar*		Find(const char* name);
};


	CVarContainer*	CVarContainer::pInstance = NULL;

CVarContainer::CVarContainer()
{
}

CVarContainer::~CVarContainer()
{
}
	
CVarContainer* CVarContainer::This()
{
  if(!pInstance)
    pInstance = game_new CVarContainer;
  return pInstance;
}

void CVarContainer::Register(CVar *var)
{
  This()->vList.push_back(var);
}

CVar *CVarContainer::Find(const char* name)
{
  for(unsigned int i = 0, iEnd = vList.size(); i < iEnd; ++i) {
    if(stricmp(vList[i]->pName, name) == NULL)
      return vList[i];
  }
  return NULL;
}

int CVarContainer::Write(const char* fname, const char* prefix)
{
  FILE *f = fopen_for_write(fname, "wt");
  if(!f) return 0;
  
  //fprintf(f, "; Variables dump\n");
  //fprintf(f, "\n");
  
  char buf[512];
  int  plen = prefix ? strlen(prefix) : 0;
  for(unsigned int i = 0, iEnd = vList.size(); i < iEnd; ++i) {
    if(vList[i]->pName[0] == '@') {
      if(prefix && strnicmp(vList[i]->pName+1, prefix, plen) != NULL) continue;
      fprintf(f, "\n; %s\n", vList[i]->pComment);
      continue;
    }
    if(prefix && strnicmp(vList[i]->pName, prefix, plen) != NULL) continue;
    fprintf(f, "%-23s = %-10s", vList[i]->pName, vList[i]->Put(buf));
    if(*vList[i]->pComment) fprintf(f, "; %s", vList[i]->pComment);
    fprintf(f, "\n");
  }
  
  fclose(f);
  return 1;
}

int CVarContainer::Read(const char* fname)
{
  r3dFile *f = r3d_open(fname, "rt");
  if(!f) return 0;

  char buf[512];
  while(!feof(f)) {
    if(fgets(buf, sizeof(buf), f) == NULL) continue;
    if(*buf == ';') continue;
    
    char var[256] = "";
    char arg[256] = "";
    sscanf(buf, "%s = %s", var, arg);
    if(!*var) continue;
    
    CVar *vp = Find(var);
    if(!vp) continue;
    
    vp->Get(arg);
  }
  
  fclose(f);
  return 1;
}

int cvars_Write(const char* fname, const char* prefix) { return CVarContainer::This()->Write(fname, prefix); }
int cvars_Read(const char* fname)                { return CVarContainer::This()->Read(fname);  }


CVar::CVar(const char* _pName, const char* _pComment)
{
  pName    = _pName;
  pComment = _pComment;
  
  CVarContainer::Register(this);
}
