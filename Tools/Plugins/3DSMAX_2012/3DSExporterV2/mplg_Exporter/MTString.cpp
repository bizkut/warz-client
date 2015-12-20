#include "StdAfx.h"
#include "MTString.h"

void MTFileName::Init(const char* pSrc)
{
  strcpy(m_pFullName, pSrc);
  //strlwr(m_pFullName);
  
  if(strrchr(m_pFullName, '\\') == NULL) {
    strcpy(m_pFileName, m_pFullName);
    strcpy(m_pPath, ".");
  } else {
    strcpy(m_pFileName, strrchr(m_pFullName, '\\') + 1);
    strcpy(m_pPath, m_pFullName);
    *strrchr(m_pPath, '\\') = 0;
  }
  
  return;
}

char *MTFileName::GetName(char *ext)
{
  if(ext == NULL) return m_pFileName;
  
  char *p = m_pFileName;
  if(strrchr(p, '.') == NULL) {
    sprintf(m_pTemp, "%s.%s", p, ext);
  } else {
    strcpy(m_pTemp, p);
    strcpy(strrchr(m_pTemp, '.')+1, ext);
  }
  
  return m_pTemp;
}

char *MTFileName::GetFull(char *ext)
{
  if(ext == NULL) return m_pFullName;
  
  char *p = m_pFullName;
  if(strrchr(p, '.') == NULL) {
    sprintf(m_pTemp, "%s.%s", p, ext);
  } else {
    strcpy(m_pTemp, p);
    strcpy(strrchr(m_pTemp, '.')+1, ext);
  }
  
  return m_pTemp;
}

/*
char* MTFileName::GetRelPath( char* pBasePath )
{
  strlwr( pBasePath );
  strlwr( m_pFullName );
  if( !strncmp( m_pFullName, pBasePath, strlen( pBasePath ) ) )
  {
      char*	pTemp = &m_pFullName[ strlen( pBasePath ) + 1 ];
      strcpy( m_pRelPath, pTemp );
      return m_pRelPath;
  }
    
  return m_pFullName;
}
*/