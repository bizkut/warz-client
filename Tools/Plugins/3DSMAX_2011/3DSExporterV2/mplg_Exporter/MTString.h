#ifndef MTFILENAME_H
#define MTFILENAME_H

#include <string>

class MTFileName
{
protected:
	char	m_pPath[MAX_PATH];
	char	m_pFileName[MAX_PATH];
	char	m_pFullName[MAX_PATH];
	
	char	m_pTemp[MAX_PATH];

public:
	MTFileName(const char* pSrc)       { Init(pSrc); }
	MTFileName(const std::string& str) { Init(str.c_str()); }
	void	Init(const char *pSrc);

	char*	GetFull(char *ext = NULL);
	char*	GetName(char *ext = NULL);
	char*	GetPath()			{ return m_pPath; }
	char*	GetRelPath(char* pBasePath);
};

#endif