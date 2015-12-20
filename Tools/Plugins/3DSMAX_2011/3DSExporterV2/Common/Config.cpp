#include "stdafx.h"
#include "Config.h"
#include "Utils.h"

	CConfig		g_Config;

CConfig::CConfig()
{
  n_vars = 0;
  *fname = 0;
}

CConfig::~CConfig()
{
}

CVar* CConfig::AddVar(const char* name, const char* val)
{
  CVar* v = GetVar(name);
  if(v) {
    U_DLog("CConfig:: duplicating var %s\n", name);
  } else {
    v = &vars[n_vars++];
  }

  strcpy(v->name, name);
  strcpy(v->data, val);
  return v;
}

CVar* CConfig::GetVar(const char* name)
{
  for(int i=0; i<n_vars; i++) {
    if(strcmp(vars[i].name, name) == NULL)
      return &vars[i];
  }
  
  return NULL;
}

void CConfig::LoadConfig(const char* in_fname)
{
  strcpy_s(fname, sizeof(fname), in_fname);
  U_DLog("CConfig: loading from %s\n", fname);
  FILE* f = fopen(fname, "rt");
  if(f == NULL)
    return;
    
  char buf[1024];
  while(!feof(f)) 
  {
    if(fgets(buf, sizeof(buf), f) == NULL)
      break;
      
    // remove cr lf
    if(strlen(buf) >= 1) buf[strlen(buf)-1] = 0;

    char* arg = strchr(buf, '=');
    if(!arg) continue;
    *arg++ = 0;
    
    CVar* v = AddVar(buf, arg);
    U_DLog("var: %s=%s\n", v->name, v->data);
  }
    
  fclose(f);
  return;
}

void CConfig::SaveConfig()
{
  if(*fname == 0)
    return;
    
  FILE* f = fopen(fname, "wt");
  if(!f) 
    return;
    
  for(int i=0; i<n_vars; i++) {
    fprintf(f, "%s=%s\n", vars[i].name, vars[i].data);
  }
    
  fclose(f);
}

const char* CConfig::GetStr(const char* name, const char* def)
{
  CVar* var = GetVar(name);
  if(!var) return def;
  return var->data;
}

void CConfig::SetStr(const char* name, const char* val)
{
  CVar* var = GetVar(name);
  if(var) strcpy(var->data, val);
  else    AddVar(name, val);
}

const int CConfig::GetInt(const char* name, int def)
{
  CVar* var = GetVar(name);
  if(!var) return def;
  return atoi(var->data);
}

void CConfig::SetInt(const char* name, int val)
{
  char buf[64];
  sprintf(buf, "%d", val);
  SetStr(name, buf);
}
