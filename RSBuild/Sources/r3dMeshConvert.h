#pragma once

#define RECONVERT_SCO_MESHES 1

extern	int	convertScoToBinary(const char* sco, const char* scb);

// they from r3dObjLS.cpp
extern	void	setFileTimestamp(const char* fname, const FILETIME& time);
extern	bool	getFileTimestamp(const char* fname, FILETIME& writeTime);

extern	DWORD	r3dGetFileSize(const char* fname);
