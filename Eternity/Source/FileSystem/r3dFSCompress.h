#pragma once

class r3dFSCompress
{
  public:
	enum EMethod
	{
	  COMPRESS_STORE   = 1,
	  COMPRESS_INFLATE = 2,
	  COMPRESS_LZO     = 4,
	};
	
	bool		failOnError;

  public:
	int		CompressStore(const BYTE* in_data, DWORD in_size, BYTE** out_data, DWORD* out_csize);
	int		CompressInflate(const BYTE* in_data, DWORD in_size, BYTE** out_data, DWORD* out_csize);
	int		CompressLzo(const BYTE* in_data, DWORD in_size, BYTE** out_data, DWORD* out_csize);

	int		DecompressStore(const BYTE* in_data, DWORD in_csize, DWORD in_size, BYTE* out_data, DWORD* out_size);
	int		DecompressInflate(const BYTE* in_data, DWORD in_csize, DWORD in_size, BYTE* out_data, DWORD* out_size);
	int		DecompressLzo(const BYTE* in_data, DWORD in_csize, DWORD in_size, BYTE* out_data, DWORD* out_size);

  public:
	r3dFSCompress();
	
	int		CompressFile(
	  const char* fname, 
	  int* method, 
	  BYTE** out_data, 
	  DWORD* out_size, 
	  DWORD* out_csize,
	  DWORD* out_crc32);
	  
	int		Decompress(
	  const char* fname_for_reference,
	  int method,
	  const BYTE* in_data,
	  DWORD in_csize,
	  DWORD in_size,
	  BYTE* out_data,
	  DWORD* out_size);
};

extern	bool		r3dGetFileCrc32(const char* fname, DWORD* out_crc32, DWORD* out_size);
