#include "r3dPCH.h"
#include "r3d.h"

#include <Shlwapi.h>
#include <ShellAPI.h>

#include "FileSystem/r3dFileSystem.h"
#include "FileSystem\r3dFSStructs.h"

static	r3dFileSystem	g_filesys;
extern CRITICAL_SECTION g_FileSysCritSection ;

void r3dCopyFixedFileName(const char* in_fname, char* out_fname)
{
  if(in_fname[0] == 0) {
    out_fname[0] = 0;
    return;
  }

  // make a working copy, fix backslashes, fix double slashes
  bool hasdirectory = false;
  char fname[MAX_PATH];
  char* pfname = fname;
  for(const char *in = in_fname; *in; pfname++, in++) {
    *pfname = *in;
    if(*pfname == '/') 
      *pfname = '\\';
    
    if(*pfname == '\\')
      hasdirectory = true;
    
    if(*pfname == '\\' && (in[1] == '/' || in[1] == '\\'))
      in++;
  }
  *pfname = 0;

  // remove trailing whitespaces
  for(--pfname; *pfname == ' '; --pfname)
    *pfname = 0;
  
  if(!hasdirectory) {
    // copy back fixed filename
    r3dscpy(out_fname, fname);
    return;
  }
    
  // make directory structure
  char* pdirs1[128];	// start of each dir name
  char* pdirs2[128];	// end of each dir name
  int   ndirs = 0;
  
  pdirs1[0] = fname;
  for(char* p = fname; *p; p++)
  {
    if(*p == '\\') {
      if(ndirs) pdirs1[ndirs] = pdirs2[ndirs-1] + 1;
      pdirs2[ndirs] = p;  
      ndirs++;
    }
  }
  
  // check if we have relative path
  bool  hasrelpath = false;
  for(char* p = fname; *p; p++) {
    if(p[0] == '.' && p[1] == '\\')
      hasrelpath = true;
  }
  
  if(!hasrelpath) {
    // copy back fixed filename
    r3dscpy(out_fname, fname);
    return;
  }
    
  // we have . or .. - reconstruct path
  if(!ndirs) 
    r3dError("bad filename %s\n", fname);

  for(int i=0; i<ndirs; i++) {
    *pdirs2[i] = 0;
  }
    
  char* rdirs[128];
  int   nrdirs = 0;
    
  for(int i=0; i<ndirs; i++) 
  {
    char* dir = pdirs1[i];
    if(dir[0] == '.' && dir[1] == 0) {
      // skip it
      continue;
    }

    if(dir[0] == '.' && dir[1] == '.') {
      if(ndirs == 0)
        r3dError("can't go beyond root directory: %s\n", fname);
      
      // go back one level
      nrdirs--;
      continue;
    }
      
    rdirs[nrdirs++] = dir;
  }
    
  // reconstruct final path
  *out_fname = 0;
  for(int i=0; i<nrdirs; i++) {
    sprintf(out_fname + strlen(out_fname), "%s\\", rdirs[i]);
  }
  
  // append filename
  r3d_assert(ndirs);
  sprintf(out_fname + strlen(out_fname), "%s", pdirs2[ndirs-1] + 1);
  
  return;
}

// internal create r3dFile
r3dFile* r3dFile_IntOpen(const char* fname, const char* mode)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  bool allowDirectAccess = true;

#if defined( FINAL_BUILD ) && 1
  // disable all data/ files in final build
  // if(strnicmp(fname, "data/", 5) == 0)
    if(strnicmp(fname, "data", 4) == 0 || strnicmp(fname, "levels", 6) == 0)
    allowDirectAccess = false;
#endif

  // check for direct file
  FILE* stream;
  if(allowDirectAccess && (stream = fopen(fname, mode)) != NULL)
  {
    // init from stream
    r3dFile* f = game_new r3dFile();
    sprintf(f->Location.FileName, "%s", fname);
    f->Location.Where  = FILELOC_File;
    f->Location.id     = 0;

    fseek(stream, 0, SEEK_END);
    f->size = ftell(stream);
    fseek(stream, 0, SEEK_SET);
    setvbuf(stream, NULL, _IOFBF, 64000);

    f->stream = stream;
    return f;
  }
  
  const r3dFS_FileEntry* fe = g_filesys.GetFileEntry(fname);
  if(fe != NULL)
  {
    BYTE* data   = NULL;
    DWORD dwsize = 0;
    if(!g_filesys.GetFileData(fe, &data, &dwsize)) {
      r3dError("failed to get file data for %s\n", fname);
      return 0;
    }
    r3d_assert(dwsize < 0x7FFFFFFF); // file should be smaller that 2gb
    r3d_assert(data);

    // init from memory
    r3dFile* f = game_new r3dFile();
    sprintf(f->Location.FileName, "%s", fname);
    f->Location.Where  = FILELOC_Resource;
    f->Location.id     = (DWORD)fe;
    f->data = data;
    f->size = (int)dwsize;
    return f;
  }
  
  r3dOutToLog("r3dFile: can't open %s\n", fname);
  return NULL;
}


//
//
//
void r3dFile::r3dFile_Init()
{
  stream     = NULL;
  data       = NULL;
  size       = 0;
  pos        = 0;
}

r3dFile::~r3dFile()
{
  if(data)
    delete[] data;
  if(stream)
    fclose(stream);
}

//------------------------------------------------------------------------

r3dUnpackedFile::r3dUnpackedFile()
{
	offset = 0;

	entry = NULL;
}

//------------------------------------------------------------------------

template < typename T >
r3dChunkedFileT< T >::r3dChunkedFileT()
: mChunkSize( 16 * 1024 * 1024 )
, mCurrentChunk( 0 )
{

}

//------------------------------------------------------------------------

template < typename T >
r3dChunkedFileT< T >::~r3dChunkedFileT()
{

}

//------------------------------------------------------------------------

template < typename T >
int r3dChunkedFileT< T >::Open( const char* baseName, const char* ext, int chunkSize )
{
	mBaseName = baseName;
	mExt = ext;

	mCurrentChunk = 0;

	char fullName[ 1024 ];

	for( int i = 0 ; ; i ++ )
	{
		PrintChunkName( fullName, i );

		if( r3dFileExists( fullName ) )
		{
			T* file = DoOpen( fullName );
			mFileArr.PushBack( file );
		}
		else
			break;
	}

	return 1;
}

//------------------------------------------------------------------------

template < typename T >
int r3dChunkedFileT< T >::Close()
{
	for( int i = 0, e = (int)mFileArr.Count(); i < e; i ++ )
	{
		if( T * file = mFileArr[ i ] )
		{
			fclose( file );
		}
	}

	mFileArr.Clear();

	return 1;
}

//------------------------------------------------------------------------

template < typename T >
int r3dChunkedFileT< T >::Read( void* ptr, int size )
{
	int totalRead = 0;

	char* cptr = (char*)ptr;

	if( mCurrentChunk < (int)mFileArr.Count() )
	{
		for( ; ; )
		{
			T* current = mFileArr[ mCurrentChunk ];
			int toRead = R3D_MIN( mChunkSize - (int)ftell( current ), size );

			if( !fread( cptr, toRead, 1, current ) )
				return totalRead;

			cptr += toRead;

			size -= toRead;

			totalRead += toRead;

			if( ftell( current ) == mChunkSize )
			{
				mCurrentChunk ++;

				if( mCurrentChunk < (int)mFileArr.Count() )
				{
					fseek( mFileArr[ mCurrentChunk ], 0, SEEK_SET );
				}
			}

			if( size <= 0 )
			{
				r3d_assert( size == 0 );
				break;
			}
		}
	}

	return totalRead;
}

//------------------------------------------------------------------------

template <>
int r3dChunkedFileT< FILE >::Write( const void * ptr, int size )
{
	int totalWritten = 0;

	const char* cptr = (const char*)ptr;

	if( mCurrentChunk >= (int)mFileArr.Count() )
	{
		AppendChunk();
	}

	for( ; ; )
	{
		FILE* current = mFileArr[ mCurrentChunk ];
		int toWrite = R3D_MIN( mChunkSize - (int)ftell( current ), size );

		if( !fwrite( cptr, toWrite, 1, current ) )
			return totalWritten;

		cptr += toWrite;

		totalWritten += toWrite;

		size -= toWrite;

		int currOffset = ftell( current );

		if( currOffset >= mChunkSize )
		{
			r3d_assert( currOffset == mChunkSize );

			mCurrentChunk ++;

			if( mCurrentChunk >= (int)mFileArr.Count() )
				AppendChunk();

			fseek( mFileArr[ mCurrentChunk ], 0, SEEK_SET );
		}

		if( size <= 0 )
		{
			r3d_assert( size == 0 );
			break;
		}
	}

	return totalWritten;
}

//------------------------------------------------------------------------

template <>
int r3dChunkedFileT< r3dFile >::Write( const void * ptr, int size )
{
	r3dError( "r3dChunkedFileT< r3dFile >::Write: shouldn't get here!" );

	return 0;
}

//------------------------------------------------------------------------

template < typename T >
int r3dChunkedFileT< T >::Seek( int offset, int wence )
{
	int index = 0;
	int suboffset = 0;

	int curOffset = 0;

	switch( wence )
	{
	case SEEK_CUR:
		{
			if( mCurrentChunk < (int)mFileArr.Count() )
			{
				curOffset = mCurrentChunk * mChunkSize + ftell( mFileArr[ mCurrentChunk ] );
			}
			else
			{
				if( mFileArr.Count() )
				{
					fseek( mFileArr.GetLast(), 0, SEEK_END );
					int lastOffset = ftell( mFileArr.GetLast() );
					curOffset = ( mCurrentChunk - 1 ) * mChunkSize + lastOffset;
				}
				else
				{
					curOffset = 0;
				}
			}
		}
		break;
	case SEEK_END:
		if( mFileArr.Count() )
		{
			fseek( mFileArr.GetLast(), 0, SEEK_END );
			curOffset = R3D_MAX( (int)mFileArr.Count() - 1, 0 ) * mChunkSize + ftell( mFileArr.GetLast() );
		}
		else
		{
			curOffset = 0;
		}
		break;
	}

	curOffset += offset;

	index = curOffset / mChunkSize;
	suboffset = curOffset % mChunkSize;

	for( int i = mFileArr.Count(), e = index; i < e; i ++ )
	{
		AppendChunk();
		if( i == e - 1 )
			fseek( mFileArr[ i ], suboffset, SEEK_SET );
		else
			fseek( mFileArr[ i ], mChunkSize, SEEK_SET );
	}

	mCurrentChunk = index;

	if( mCurrentChunk < (int)mFileArr.Count() )
	{
		fseek( mFileArr[ mCurrentChunk ], suboffset, SEEK_SET );
	}

	return 1;
}

//------------------------------------------------------------------------

template < typename T >
void r3dChunkedFileT< T >::Truncate( int size )
{
	int index = size / mChunkSize;
	int suboffset = size % mChunkSize;

	r3d_assert( index < (int)mFileArr.Count() );

	for( int i = index + 1, e = (int)mFileArr.Count(); i < e; i ++ )
	{
		char name[ 1024 ];
		PrintChunkName( name, i );

		fclose( mFileArr[ i ] );
		remove( name );
	}

	mFileArr.Resize( index + 1 );

	char truncateeName[ 1024 ];

	PrintChunkName( truncateeName, index );

	fclose( mFileArr[ index ] );

	HANDLE file = ::CreateFile( truncateeName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	r3d_assert( file != INVALID_HANDLE_VALUE );

	SetFilePointer( file, suboffset, NULL, FILE_BEGIN );

	SetEndOfFile( file );

	CloseHandle( file );

	mFileArr[ index ] = DoOpen( truncateeName );
}

//------------------------------------------------------------------------

template < typename T >
int r3dChunkedFileT< T >::GetSize() const
{
	if( !mFileArr.Count() )
		return 0;

	int size = ( mFileArr.Count() - 1 ) * mChunkSize;

	int curPos = ftell( mFileArr.GetLast() );
	fseek( mFileArr.GetLast(), 0, SEEK_END );
	int maxPos = ftell( mFileArr.GetLast() );

	fseek( mFileArr.GetLast(), curPos, SEEK_SET );

	return size + maxPos;
}

//------------------------------------------------------------------------

template <>
/*static*/ r3dFile* r3dChunkedFileT< r3dFile >::DoOpen( const char* name )
{
	return r3d_open( name, "rb" );
}

//------------------------------------------------------------------------

template <>
/*static*/ FILE* r3dChunkedFileT< FILE >::DoOpen( const char* name )
{
	return fopen( name, "r+b" );
}

//------------------------------------------------------------------------/

template < typename T >
int r3dChunkedFileT< T >::GetPos() const
{
	if( mFileArr.Count() )
	{
		if( mCurrentChunk < (int)mFileArr.Count() )
			return ftell( mFileArr[ mCurrentChunk ] ) + mCurrentChunk * mChunkSize;

		return R3D_MAX( (int)mFileArr.Count() - 1, 0 ) * mChunkSize + ftell( mFileArr.GetLast() );
	}
	else
	{
		return 0;
	}
}

//------------------------------------------------------------------------

template < typename T >
int r3dChunkedFileT< T >::GetChunkCount() const
{
	return (int)mFileArr.Count();
}

//------------------------------------------------------------------------

template< typename T >
const r3dString&
r3dChunkedFileT< T >::GetBaseName() const
{
	return mBaseName;
}

//------------------------------------------------------------------------

template< typename T >
const r3dString&
r3dChunkedFileT< T >::GetExt() const
{
	return mExt;
}


//------------------------------------------------------------------------

template < typename T >
void r3dChunkedFileT< T >::PrintChunkName( char* buffer, int index )
{
	sprintf( buffer, "%s_%d.%s", mBaseName.c_str(), index, mExt.c_str() );
}

//------------------------------------------------------------------------

template <>
void r3dChunkedFileT< FILE >::AppendChunk()
{
	char chunkName[ 512 ];
	PrintChunkName( chunkName, mFileArr.Count() );

	FILE* file = fopen( chunkName, "w+b" );
	mFileArr.PushBack( file );
}

//------------------------------------------------------------------------

template <>
void r3dChunkedFileT< r3dFile >::AppendChunk()
{
	r3dError( "r3dChunkedFileT< r3dFile >::AppendChunk: shouldn't get here!" );
}

template class r3dChunkedFileT< r3dFile >;
template class r3dChunkedFileT< FILE >;

//------------------------------------------------------------------------

  
/*
int r3dFile::Open(const char* fname, const char* mode)
{
  r3d_assert(data == NULL);
  r3d_assert(stream == NULL);
  r3d_assert(pos == 0);
  
  r3dFile* f = r3dFile_IntOpen(fname, mode);
  if(f == NULL)
    return 0;
    
  // copy all content from tempf to this
  Location = f->Location;
  stream   = f->stream;
  data     = f->data;
  pos      = f->pos;
  size     = f->size;
  delete f;

  return 1;
}
*/

void fclose(r3dFile *f)
{
  delete f;
}

void fclose( r3dChunkedFile *f )
{
	f->Close();
	delete f;
}

void fclose( r3dChunkedWriteFile *f )
{
	f->Close();
	delete f;
}

long ftell(r3dFile *f)
{
  if(f->stream) return ftell(f->stream);
  return f->pos;
}

long ftell( r3dWindowsFileHandle file )
{
	DWORD curr = SetFilePointer( file.Handle, 0,  NULL, FILE_CURRENT );

	return (long)curr;
}

long ftell( r3dChunkedFile * f )
{
	return f->GetPos();
}

long ftell( r3dChunkedWriteFile * f )
{
	return f->GetPos();
}

int feof(r3dFile* f) 
{
  if(f->stream) return feof(f->stream);
  if(f->pos >= f->size) return 1;
  else                  return 0;
}

int	fseek( r3dUnpackedFile *f, long offset, int whence )
{
	switch( whence )
	{
	case SEEK_SET:
		f->offset = offset;		
		break;
	case SEEK_CUR:
		f->offset += offset;
		break;
	case SEEK_END: 
		f->offset = f->entry->offset + offset;	
		break;
	}

	if( f->offset > (int)f->entry->size )
	{
		f->offset = f->entry->size;
	}

	return f->offset;
}

int fseek(r3dFile *f, long offset, int whence)
{
  if(f->stream) {
    int val = fseek(f->stream, offset, whence);
    return val;
  }

  long pos;
  switch(whence) {
    default:
    case SEEK_SET: pos = offset;		break;
    case SEEK_CUR: pos = f->pos + offset;	break;
    case SEEK_END: pos = f->size + offset;	break;
  }

  // set EOF flag
  if(pos < 0)        { pos = 0; }
  if(pos >= f->size) { pos = f->size;}

  f->pos = pos;

  return 0;
}

int fseek( r3dWindowsFileHandle file, long offset, int whence)
{
	int method = FILE_BEGIN;

	if( whence == SEEK_CUR )
		method = FILE_CURRENT;

	if( whence == SEEK_END )
		method = FILE_END;

	DWORD result = SetFilePointer( file.Handle, offset, NULL, method );

	if( result == INVALID_SET_FILE_POINTER )
	{
		return 1;
	}

	return 0;
}

int fseek( r3dMappedViewOfFile* view, long offset, int whence )
{
	switch( whence )
	{
	case SEEK_SET:
		view->Ptr = offset;
		break;
	case SEEK_CUR:
		view->Ptr += offset;
		break;
	case SEEK_END:
		view->Ptr = view->Size - offset;
		break;
	}

	r3d_assert( view->Ptr >= 0 );
	r3d_assert( view->Ptr <= view->Size );

	return 1;
}

int fseek( r3dChunkedFile* f, long offset, int wence )
{
	return f->Seek( offset, wence );
}

int fseek( r3dChunkedWriteFile* f, long offset, int wence )
{
	return f->Seek( offset, wence );
}

char* fgets(char* s, int n, r3dFile *f)
{
  if(f->stream) {
    char* val = fgets(s, n, f->stream);
    return val;
  }

  if(f->pos >= f->size)
    return NULL;

  char  *out = s;
  int	i    = 0;

  while(1) {
    if(f->pos >= f->size)	break;
    if(i >= n-1)            	break;

    char in = f->data[f->pos++];
    if(in == '\r')       	continue;
    if(in == 0)          	break;

    out[i++] = in;
    if(in == '\n')      	break;
  }
  // add trailing zero
  out[i] = 0;

  return s;
}

int fread( void *ptr, size_t size, size_t n, r3dUnpackedFile *f )
{
	int sizeLeft = f->entry->size;
	
	int toCopy = R3D_MIN( sizeLeft, (int)size );

	HANDLE handle = g_filesys.volumeHandles[ f->entry->volume ];

	DWORD res = SetFilePointer( handle, f->entry->offset + f->offset + 4, NULL, FILE_BEGIN );

	r3d_assert( res != INVALID_SET_FILE_POINTER );

	DWORD numRead;
	res = ReadFile( handle, ptr, toCopy, &numRead, NULL );

	r3d_assert( res );

	f->offset += toCopy;
	
	return toCopy / size;
}

size_t fread(void *ptr, size_t size, size_t n, r3dFile *f)
{
  if(f->stream) {
    size_t val = fread(ptr, size, n, f->stream);
    return val;
  }

  size_t len = n * size;

  // NOTE:
  //  add \r removal in text-mode reading..
  //  i'm not sure it's needed, but it's needed for full compatibility
  if(f->pos + len >= (size_t) f->size)
    len = f->size - f->pos;
  if(len == 0)
    return 0;
  memcpy(ptr, f->data + f->pos, len);
  f->pos += len;
  return len / size;
}

size_t fread(void *ptr, size_t size, size_t n, r3dWindowsFileHandle file )
{
	DWORD bytesRead( 0 );
	if( ReadFile( file.Handle, ptr, size* n, &bytesRead, NULL ) )
		return bytesRead / size;
	else
		return 0;
}

size_t fread( void *ptr, size_t size, size_t n, r3dMappedViewOfFile* view )
{
	r3d_assert( int( size * n ) + view->Ptr <= view->Size );

	memcpy( ptr, (char*)view->Start + view->Ptr, size * n );

	view->Ptr += size * n;

	return n;
}

size_t fread( void *ptr, size_t size, size_t n, r3dChunkedFile* file )
{
	return file->Read( ptr, size * n ) / size;
}

size_t fwrite( const void* ptr, size_t size, size_t n, r3dWindowsFileHandle file )
{
	DWORD written( 0 );
	if( WriteFile( file.Handle, ptr, size * n, &written, NULL ) )
		return written / size;
	else
		return 0;
}

size_t fwrite( const void* ptr, size_t size, size_t n, r3dMappedViewOfFile* view )
{
	r3d_assert( view->Ptr + (int)size <= view->Size );
	memcpy( (char*)view->Start + view->Ptr, ptr, size * n );

	view->Ptr += size * n;

	return n;
}

size_t fwrite( const void* ptr, size_t size, size_t n, r3dChunkedWriteFile* file )
{
	return file->Write( ptr, size * n ) / size;
}

int r3dFileManager_OpenArchive(const char* fname)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);
  if(!g_filesys.OpenArchive(fname))
    return 0;

  g_filesys.OpenVolumesForRead();
  return 1;
}

r3dFile* r3d_open(const char* fname, const char* mode)
{
  if(strchr(mode, 'w') != NULL)
    r3dError("[%s] do not use r3dFile for writing, use FILE* and fopen_for_write instead", fname);
    
  return r3dFile_IntOpen(fname, mode);
}

r3dChunkedFile* r3d_open_chunked( const char* base, const char* ext, int chunkSize )
{
	r3dChunkedFile* file = new r3dChunkedFile;

	file->Open( base, ext, chunkSize );

	return file;
}

r3dUnpackedFile* r3d_open_unpacked( const char* fileName )
{
	const class r3dFS_FileEntry* entry = g_filesys.GetFileEntry( fileName );

	if( entry )
	{
		r3dUnpackedFile* file = new r3dUnpackedFile;

		file->entry = entry;

		return file;
	}

	return NULL;
}

int fclose( r3dUnpackedFile * file )
{
	delete file;

	return 1;
}

r3dChunkedWriteFile* r3d_open_write_chunked( const char* base, const char* ext, int chunkSize )
{
	r3dChunkedWriteFile* file = new r3dChunkedWriteFile;

	file->Open( base, ext, chunkSize );

	return file;
}

int r3d_access(const char* fname, int mode)
{
  r3dCSHolderWithDeviceQueue csHolder(g_FileSysCritSection);

  if(_access_s(fname, mode) == 0)
    return 0;

  errno_t cur_errno ;
  _get_errno( &cur_errno ) ;
  if(cur_errno == EACCES/* || cur_errno == ENOENT*/)
    return -1;

  const r3dFS_FileEntry* fe = g_filesys.GetFileEntry(fname);
  if(fe == NULL) {
    // not found
	  _set_errno( ENOENT ) ;
    return -1;
  }
    
  // file found, check for access
  _set_errno( EACCES );
  switch(mode) {
    case 0: return 0;  // Existence only 
    case 2: return -1; // Write permission 
    case 4: return 0;  // Read permission 
    case 6: return -1; // Read & Write permissions
  }

  return 0;
}

INT64	r3d_fstamp( const char* fname )
{
	struct _stat buf;
	int fd, result;

	_sopen_s( &fd, fname, _O_RDONLY, _SH_DENYNO, 0 );

	INT64 stamp = 0;

	if( fd != -1 )
	{
		result = _fstat( fd, &buf );

		// Check if statistics are valid: 
		if( !result )
		{
			stamp = buf.st_mtime;
		}

		_close( fd );
	}

	return stamp;
}

bool r3dFileExists(const char* fname)
{
  return r3d_access(fname, 0) == 0;
}

const char* GetErnoString()
{
	switch( errno )
	{
	case EPERM:
		return "Operation not permitted";
	case ENOENT:
		return "No such file or directory";
	case ESRCH:
		return "No such process";
	case EINTR:
		return "Interrupted function";
	case EIO:
		return "I/O error";
	case ENXIO:
		return "No such device or address";
	case E2BIG:
		return "Argument list too long";
	case ENOEXEC:
		return "Exec format error";
	case EBADF:
		return "Bad file number";
	case ECHILD:
		return "No spawned processes";
	case EAGAIN:
		return "No more processes or not enough memory or maximum nesting level reached";
	case ENOMEM:
		return "Not enough memory";
	case EACCES:
		return "Permission denied";
	case EFAULT:
		return "Bad address";
	case EBUSY:
		return "Device or resource busy";
	case EEXIST:
		return "File exists";
	case EXDEV:
		return "Cross-device link";
	case ENODEV:
		return "No such device";
	case ENOTDIR:
		return "Not a directory";
	case EISDIR:
		return "Is a directory";
	case EINVAL:
		return "Invalid argument";
	case ENFILE:
		return "Too many files open in system";
	case EMFILE:
		return "Too many open files";
	case ENOTTY:
		return "Inappropriate I/O control operation";
	case EFBIG:
		return "File too large";
	case ENOSPC:
		return "No space left on device";
	case ESPIPE:
		return "Invalid seek";
	case EROFS:
		return "Read-only file system";
	case EMLINK:
		return "Too many links";
	case EPIPE:
		return "Broken pipe";
	case EDOM:
		return "Math argument";
	case ERANGE:
		return "Result too large";
	case EDEADLK:
		return "Resource deadlock would occur";
	case ENAMETOOLONG:
		return "Filename too long";
	case ENOLCK:
		return "No locks available";
	case ENOSYS:
		return "Function not supported";
	case ENOTEMPTY:
		return "Directory not empty";
	case EILSEQ:
		return "Illegal byte sequence";
	case STRUNCATE:
		return "String was truncated";
	}

	return "Unknown error";
}

FILE* fopen_for_write(const char* fname, const char* mode)
{
  ::SetFileAttributes(fname, FILE_ATTRIBUTE_NORMAL);

  FILE* f = fopen(fname, mode);
  if(f == NULL) {
	  r3dError("!!warning!!! can't open %s for writing\nError: %s(%d)\n", fname, GetErnoString(), errno );
  }
  
  return f;
}

bool r3dIsAbsolutePath(const char* path)
{
	return path[1] == ':';
}

void r3dFullCanonicalPath(const char* relativePath, char* result)
{
	char fullPath[MAX_PATH];
	if(!r3dIsAbsolutePath(relativePath))
	{
		char path[MAX_PATH];
		GetFullPathNameA(".\\", MAX_PATH, path, NULL);
		sprintf(fullPath,"%s%s", path, relativePath);
	}
	else
	{
		sprintf(fullPath,"%s", relativePath);
	}


	for (char* it = fullPath; *it != 0; ++it)
	{
		if (*it == '/')
			*it = '\\';
	}

	PathCanonicalizeA(result, fullPath);
}

bool r3dIsSamePath(const char* path0, const char* path1)
{
	char t0[MAX_PATH];
	char t1[MAX_PATH];
	r3dFullCanonicalPath(path0, t0);
	r3dFullCanonicalPath(path1, t1);

	return stricmp(t0, t1) == 0;
}

bool CreateConfigPath(char* dest)
{
	if( SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE, NULL, 0, dest)) ) 
	{
		strcat( dest, "\\Kongsi Games\\" );
		mkdir( dest );
		strcat( dest, "WarZ\\" );
		mkdir( dest );

		return true;
	}

	return false;
}

bool CreateWorkPath(char* dest)
{
	if( SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, dest)) ) 
	{
		strcat( dest, "\\Kongsi Games\\" );
		mkdir( dest );
		strcat( dest, "WarZ\\" );
		mkdir( dest );

		return true;
	}

	return false;
}

//------------------------------------------------------------------------

static int do_mkdir( const char *path )
{
	struct stat     st;
	int             status = 0;

	if ( stat(path, &st) != 0)
	{
		/* Directory does not exist */
		if (mkdir( path ) != 0)
			status = -1;
	}
	else if (! ( st.st_mode & _S_IFDIR ) )
	{
		status = -1;
	}

	return(status);
}


int r3d_create_path( const char *path )
{
	char drive[ 16 ], dir[ MAX_PATH * 3 ], file[ MAX_PATH * 3 ], ext[ MAX_PATH * 3 ] ;

	drive[ 0 ] = 0 ;

	_splitpath( path, drive, dir, file, ext ) ;

	char           *pp;
	char           *sp;
	int             status;
	// we may get trick folder with date (extensionesque) - treat it as a folder

	strcat ( dir, file ) ;
	strcat ( dir, ext ) ;
	char           *copypath = strdup( dir );

	for( int i = 0, e = strlen( copypath ) ; i < e ; i ++ )
	{
		if( copypath[ i ] == '\\' )
			copypath[ i ] = '/' ;
	}

	status = 0;
	pp = copypath;
	while (status == 0 && (sp = strchr(pp, '/')) != 0)
	{
		if (sp != pp)
		{
			/* Neither root nor double slash in path */
			*sp = '\0';
			status = do_mkdir( drive[0] ? ( r3dString( drive ) + copypath ).c_str() : copypath );
			*sp = '/';
		}
		pp = sp + 1;
	}
	if (status == 0)
		status = do_mkdir( drive[0] ? ( r3dString( drive ) + dir ).c_str() : path );
	free(copypath);
	return (status);
}

//------------------------------------------------------------------------

int r3d_delete_dir( const char* path )
{
  r3dSTLString refcstrRootDirectory = path ;
  bool bDeleteSubdirectories = true ;

  bool            bSubdirectory = false;       // Flag, indicating whether
                                               // subdirectories have been found
  HANDLE          hFile;                       // Handle to directory
  r3dSTLString     strFilePath;                 // Filepath
  r3dSTLString     strPattern;                  // Pattern
  WIN32_FIND_DATA FileInformation;             // File information


  strPattern = refcstrRootDirectory + "\\*.*";
  hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
  if(hFile != INVALID_HANDLE_VALUE)
  {
    do
    {
      if(FileInformation.cFileName[0] != '.')
      {
        strFilePath.erase();
        strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

        if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if(bDeleteSubdirectories)
          {
            // Delete subdirectory
            int iRC = r3d_delete_dir( strFilePath.c_str() );
            if(iRC)
              return iRC;
          }
          else
            bSubdirectory = true;
        }
        else
        {
          // Set file attributes
          if(::SetFileAttributes(strFilePath.c_str(),
                                 FILE_ATTRIBUTE_NORMAL) == FALSE)
            return ::GetLastError();

          // Delete file
          if(::DeleteFile(strFilePath.c_str()) == FALSE)
            return ::GetLastError();
        }
      }
    } while(::FindNextFile(hFile, &FileInformation) == TRUE);

    // Close handle
    ::FindClose(hFile);

    DWORD dwError = ::GetLastError();
    if(dwError != ERROR_NO_MORE_FILES)
      return dwError;
    else
    {
      if(!bSubdirectory)
      {
        // Set directory attributes
        if(::SetFileAttributes(refcstrRootDirectory.c_str(),
                               FILE_ATTRIBUTE_NORMAL) == FALSE)
          return ::GetLastError();

        // Delete directory
        if(::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
          return ::GetLastError();
      }
    }
  }

  return 0;
}

//-------------------------------------------------------------------------

bool r3d_remove_files_in_folder(const char *pathWithFilePattern)
{
	if (!pathWithFilePattern)
		return false;

	SHFILEOPSTRUCT op;
	ZeroMemory(&op, sizeof(SHFILEOPSTRUCT));
	op.hwnd = 0;
	op.wFunc = FO_DELETE;

	//	SHFileOperation need double trailing zero
	char buf[MAX_PATH] = {0};
	strcpy_s(buf, _countof(buf), pathWithFilePattern);
	size_t l = strlen(buf);
	r3d_assert(l + 1 < MAX_PATH);
	buf[strlen(buf) + 1] = 0;

	op.pFrom = buf;
	op.fFlags = FOF_FILESONLY | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

	int rv = SHFileOperation(&op);
	return rv == 0;
}

//-------------------------------------------------------------------------

r3dMappedViewOfFile::r3dMappedViewOfFile()
{
	Handle = INVALID_HANDLE_VALUE;
	Mapped = NULL;
	Start = NULL;
	Ptr = 0;
	Size = 0;
}

static int g_MappedAllocGranularity = -1;

r3dMappedViewOfFile	r3dCreateMappedViewOfFile( HANDLE fileMapping, int access, int offset, int size )
{
	if( g_MappedAllocGranularity < 0 )
	{
		SYSTEM_INFO sinfo;
		GetSystemInfo( &sinfo );

		g_MappedAllocGranularity = sinfo.dwAllocationGranularity;
	}

	r3dMappedViewOfFile mappedViewOfFile;

	mappedViewOfFile.Handle = fileMapping;

	int offsetDiff = offset % g_MappedAllocGranularity;
	int realOffset = offset - offsetDiff;

	mappedViewOfFile.Mapped = MapViewOfFile( fileMapping, access, 0, realOffset, size + offsetDiff );
	mappedViewOfFile.Start = (char*)mappedViewOfFile.Mapped + offsetDiff;
	r3d_assert( mappedViewOfFile.Mapped );

	mappedViewOfFile.Size = size;

	return mappedViewOfFile;
}

//------------------------------------------------------------------------

void DEBUG_CheckChunked()
{
	for( int s = 0, e = 256; s < e; s += 4 )
	{
		FILE* fout0 = fopen( "testme.bin", "wb" );

		for( int ii = 0; ; ii ++ )
		{
			char chunkedName[ 1024 ];
			sprintf( chunkedName, "%s_%d.%s", "testme_ch", ii, "bin" );

			if( r3dFileExists( chunkedName ) )
			{
				remove( chunkedName );
			}
			else
				break;
		}

		r3dChunkedWriteFile* fout1 = r3d_open_write_chunked( "testme_ch", "bin", 16 * 1024 * 1024 );

		typedef r3dTL::TArray< char > Bytes;
		Bytes bytes;

		for( int i = 0, e = s; i < e; i ++ )
		{
			bytes.Resize( ( rand() % 1024 ) * 1024 + rand() + 1 );

			for( int i = 0, e = bytes.Count(); i < e; i ++ )
			{
				bytes[ i ] = rand();
			}

			fwrite( &bytes[ 0 ], bytes.Count(), 1, fout0 );
			fwrite( &bytes[ 0 ], bytes.Count(), 1, fout1 );
		}

		fclose( fout0 );
		fclose( fout1 );

		FILE* fin0 = fopen( "testme.bin", "rb" );
		r3dChunkedFile* fin1 = r3d_open_chunked( "testme_ch", "bin", 16 * 1024 * 1024 );

		fseek( fin0, 0, SEEK_END );
		fseek( fin1, 0, SEEK_END );

		int size0 = ftell( fin0 );
		int size1 = ftell( fin1 );

		fseek( fin0, 0, SEEK_SET );
		fseek( fin1, 0, SEEK_SET );

		r3d_assert( size0 == size1 );

		int ptr = 0;

		Bytes bytes1;

		for( ; ; )
		{
			int toRead = R3D_MIN( size0 - ptr, ( rand() % 1024 ) * 1024 + rand() + 1 );

			if( !toRead )
			{
				bytes.Resize( 0 );
				bytes1.Resize( 0 );

				bytes.Resize( 1, 0 );
				bytes1.Resize( 1, 0 );
			}
			else
			{
				bytes.Resize( toRead );
				bytes1.Resize( toRead );
			}

			fread( &bytes[ 0 ], bytes.Count(), 1, fin0 );
			fread( &bytes1[ 0 ], bytes1.Count(), 1, fin1 );

			if( memcmp( &bytes[ 0 ], &bytes1[ 0 ], bytes.Count() ) )
			{
				r3dOutToLog( "Mismatch!\n" );
			}

			ptr += toRead;

			if( ptr == size0 )
				break;
		}

		if( size0 )
		{
			for( int i = 0 ; i < 128; i ++ )
			{
				int offset = INT64( rand() ) * ( size0 - 1 ) / RAND_MAX;

				int size = R3D_MIN( rand() * 64, size0 - offset );

				if( size )
				{
					bytes.Resize( size );
					bytes1.Resize( size );

					fseek( fin0, offset, SEEK_SET );
					fseek( fin1, offset, SEEK_SET );

					fread( &bytes[ 0 ], bytes.Count(), 1, fin0 );
					fread( &bytes1[ 0 ], bytes1.Count(), 1, fin1 );

					if( memcmp( &bytes[ 0 ], &bytes1[ 0 ], bytes.Count() ) )
					{
						r3dOutToLog( "__Mismatch!\n" );
					}
				}
			}

			int curPtr = 0;

			fseek( fin0, curPtr, SEEK_SET );
			fseek( fin1, curPtr, SEEK_SET );

			for( int i = 0 ; i < 256; i ++ )
			{
				int offset = rand() - RAND_MAX / 2;

				offset = R3D_MIN( curPtr + offset, size0 ) - curPtr;
				offset = R3D_MAX( curPtr + offset, 0 ) - curPtr;

				curPtr += offset;

				int size = R3D_MIN( rand() * 64, size0 - curPtr );

				bytes.Resize( size );
				bytes1.Resize( size );

				fseek( fin0, offset, SEEK_CUR );
				fseek( fin1, offset, SEEK_CUR );

				int ptr0 = ftell( fin0 );
				int ptr1 = ftell( fin1 );

				r3d_assert( ptr0 == ptr1 );
				r3d_assert( ptr0 == curPtr );

				if( size )
				{
					fread( &bytes[ 0 ], bytes.Count(), 1, fin0 );
					fread( &bytes1[ 0 ], bytes1.Count(), 1, fin1 );

					curPtr += size;

					if( memcmp( &bytes[ 0 ], &bytes1[ 0 ], bytes.Count() ) )
					{
						r3dOutToLog( "~~~~__Mismatch!\n" );
					}
				}
			}
		}

		fclose( fin0 );
		fclose( fin1 );

		if( size0 )
		{
			FILE* fout0 = fopen( "testme.bin", "r+b" );
			r3dChunkedWriteFile* fout1 = r3d_open_write_chunked( "testme_ch", "bin", 16 * 1024 * 1024 );

			for( int i = 0, e = 256; i < e; i ++ )
			{
				int offset = INT64( rand() ) * ( size0 - 1 ) / RAND_MAX;
				int size = R3D_MIN( rand() * 64, size0 - offset );

				fseek( fout0, offset, SEEK_SET );
				fseek( fout1, offset, SEEK_SET );

				bytes.Resize( size );

				for( int i = 0, e = (int)bytes.Count(); i < e; i ++ )
				{
					bytes[ i ] = rand();
				}

				fwrite( &bytes[ 0 ], bytes.Count(), 1, fout0 );
				fwrite( &bytes[ 0 ], bytes.Count(), 1, fout1 );
			}

			fclose( fout0 );
			fclose( fout1 );
		}

		//------------------------------------------------------------------------

		{
			FILE* fin0 = fopen( "testme.bin", "rb" );
			r3dChunkedFile* fin1 = r3d_open_chunked( "testme_ch", "bin", 16 * 1024 * 1024 );

			fseek( fin0, 0, SEEK_END );
			fseek( fin1, 0, SEEK_END );

			int size0 = ftell( fin0 );
			int size1 = ftell( fin1 );

			fseek( fin0, 0, SEEK_SET );
			fseek( fin1, 0, SEEK_SET );

			r3d_assert( size0 == size1 );

			int ptr = 0;

			Bytes bytes1;

			for( ; ; )
			{
				int toRead = R3D_MIN( size0 - ptr, ( rand() % 1024 ) * 1024 + rand() + 1 );

				if( !toRead )
				{
					bytes.Resize( 0 );
					bytes1.Resize( 0 );

					bytes.Resize( 1, 0 );
					bytes1.Resize( 1, 0 );
				}
				else
				{
					bytes.Resize( toRead );
					bytes1.Resize( toRead );
				}

				fread( &bytes[ 0 ], bytes.Count(), 1, fin0 );
				fread( &bytes1[ 0 ], bytes1.Count(), 1, fin1 );

				if( memcmp( &bytes[ 0 ], &bytes1[ 0 ], bytes.Count() ) )
				{
					r3dOutToLog( "!!__!!Mismatch!\n" );
				}

				ptr += toRead;

				if( ptr == size0 )
					break;
			}

			fclose( fin0 );
			fclose( fin1 );
		}

		//------------------------------------------------------------------------

		r3dOutToLog( "%d\n", s );
	}
}

void ftruncate( r3dChunkedWriteFile* file, size_t size )
{
	file->Truncate( size );
}

void ftruncate( FILE* file, size_t size )
{
	_chsize( fileno(file), size );
}

int	r3dCompareFileNameInPath( const char* path0, const char* path1 )
{
	int l0 = strlen( path0 );
	int l1 = strlen( path1 );

	for( ; l0 >= 0; l0 -- )
	{
		if( path0[ l0 ] == '/' || path0[ l0 ] == '\\' )
		{
			l0 ++;
			break;
		}
	}

	for( ; l1 >= 0; l1 -- )
	{
		if( path1[ l1 ] == '/' || path1[ l1 ] == '\\' )
		{
			l1 ++;
			break;
		}
	}

	int res = stricmp( path0 + l0, path1 + l1 );

	return res;
}

int r3dFilesEqual( const char* path0, const char* path1 )
{
	struct _stat32 st0;
	struct _stat32 st1;

	_stat32( path0, &st0 );
	_stat32( path1, &st1 );

	if( st0.st_size != st1.st_size )
		return 0;

	FILE* f0 = fopen( path0, "rb" );
	FILE* f1 = fopen( path1, "rb" );

	if( !f0 || !f1 )
	{
		if( f0 ) fclose( f0 );
		if( f1 ) fclose( f1 );

		return 0;
	}

	char buff0[ 8192 ];
	char buff1[ 8192 ];

	int res = 1;

	while( !feof( f0 ) )
	{
		int read0 = fread( buff0, 1, sizeof buff0, f0 );
		int read1 = fread( buff1, 1, sizeof buff1, f1 );

		r3d_assert( read0 == read1 );

		if( memcmp( buff0, buff1, read0 ) )
		{
			res = 0;
			break;
		}
	}

	if( f0 ) fclose( f0 );
	if( f1 ) fclose( f1 );

	return res;
}

//------------------------------------------------------------------------

int r3dDirectoryExists( const char* path )
{
	DWORD ftyp = GetFileAttributesA( path );
	if( ftyp == INVALID_FILE_ATTRIBUTES )
		return 0;  //something is wrong with your path!

	if( ftyp & FILE_ATTRIBUTE_DIRECTORY )
		return 1;   // this is a directory!

	return 0;    // this is not a directory!
}

//------------------------------------------------------------------------

INT64 r3dGetDiskSpace( const char* filePath )
{
	char drive[ 64 ], path[ 512 ], filename[ 512 ], ext[ 64 ];
	_splitpath( filePath, drive, path, filename, ext );

	strcat( drive, "\\");

	DWORD sectorsPerCluster = 0;
	DWORD bytesPerSector = 0;
	DWORD numberOfFreeClusters = 0;
	DWORD totalNumberOfClusters = 0;

	GetDiskFreeSpace( drive,
						&sectorsPerCluster, &bytesPerSector,
						&numberOfFreeClusters, &totalNumberOfClusters );

	return INT64( numberOfFreeClusters ) * INT64( sectorsPerCluster ) * INT64 ( bytesPerSector );
}

//------------------------------------------------------------------------


r3dFileSystemLock::r3dFileSystemLock()
: csHolder( g_FileSysCritSection )
{
	
}

//------------------------------------------------------------------------

r3dFileSystemLock::~r3dFileSystemLock()
{
	
}
