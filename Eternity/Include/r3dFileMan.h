#ifndef	__R3D_FILEMAN_H
#define __R3D_FILEMAN_H

class r3dFile;
class r3dZipFile;
class r3dFileManager;

#include "r3dConst.h"

class r3dFileLoc
{
  public:
	char		FileName[R3D_MAX_FILE_NAME];
	#define FILELOC_File		0
	#define FILELOC_Resource	1
	int		Where;
	DWORD		id;
	r3dFileLoc() {
	  FileName[0] = 0;
	  Where       = 0xff;
	  id          = 0;
	}
};

class r3dFile
{
  private:
	friend r3dFile* r3dFile_IntOpen(const char* fname, const char* mode);
	friend void fclose(r3dFile *f);
	friend int fseek(r3dFile *f, long offset, int whence);
	friend size_t fread(void *ptr, size_t size, size_t n, r3dFile *f);
	friend char* fgets(char* s, int n, r3dFile *f);
	friend long ftell(r3dFile *f);
	friend int feof(r3dFile* f);

	FILE*		stream;
	const BYTE*	data;
	int		pos;
  
  public:
	r3dFileLoc	Location;
	int		size;

  public:
	void r3dFile_Init();
	~r3dFile();

	// additional constructors
	r3dFile()                              { r3dFile_Init(); }
	//r3dFile(const char* fname, const char* mode="rb")  { r3dFile_Init(); Open(fname, mode); }
	//int		Open(const char* fname, const char* mode);

	int		IsValid() const 
	{ 
	  return (this != NULL && (data != NULL || stream != NULL)); 
	}
	const char*	GetFileName() const 
	{ 
	  return (this == NULL)  ?  "???" : Location.FileName; 
	}
	
	// automatic conversion to pointer
	operator	r3dFile *() { return this; }
};

class r3dUnpackedFile
{
public:
	r3dUnpackedFile();

	int offset;

	const class r3dFS_FileEntry* entry;
};

template < typename T >
class r3dChunkedFileT
{
public:
	typedef r3dTL::TArray< T* > FileArr;

public:
	r3dChunkedFileT();
	~r3dChunkedFileT();

public:
	int Open( const char* baseName, const char* ext, int chunkSize );
	int Close();

	int Read( void* ptr, int size );
	int Write( const void * ptr, int size );

	int Seek( int offset, int wence );

	void Truncate( int size );

	int GetSize() const;

	int GetPos() const;

	int GetChunkCount() const;

	const r3dString& GetBaseName() const;
	const r3dString& GetExt() const;

private:
	static T* DoOpen( const char* name );
	void PrintChunkName( char* buffer, int index );
	void AppendChunk();

private:
	r3dString mBaseName;
	r3dString mExt;

	int mChunkSize;

	int mCurrentChunk;

	FileArr mFileArr;
};

typedef r3dChunkedFileT< r3dFile > r3dChunkedFile;
typedef r3dChunkedFileT< FILE > r3dChunkedWriteFile;

bool r3dIsSamePath(const char* path0, const char* path1);

int		r3dFileManager_OpenArchive(const char* fname);

r3dFile		*r3d_open(const char* fname, const char* mode = "rb");
r3dChunkedFile* r3d_open_chunked( const char* base, const char* ext, int chunkSize );
r3dUnpackedFile* r3d_open_unpacked( const char* fileName );
int fclose( r3dUnpackedFile * file );
r3dChunkedWriteFile* r3d_open_write_chunked( const char* base, const char* ext, int chunkSize );
bool		r3dFileExists( const char* fname );

int		r3d_access( const char* fname, int mode );
INT64	r3d_fstamp( const char* fname );

struct r3dWindowsFileHandle
{
	HANDLE Handle;
};

struct r3dMappedViewOfFile
{
	HANDLE Handle;
	void* Mapped;
	void* Start;
	int Ptr;
	int Size;

	r3dMappedViewOfFile();
};

r3dMappedViewOfFile	r3dCreateMappedViewOfFile( HANDLE fileMapping, int access, int offset, int size );

void		fclose( r3dFile *f );
void		fclose( r3dChunkedFile *f );
void		fclose( r3dChunkedWriteFile *f );
int			fseek( r3dUnpackedFile *f, long offset, int whence );
int			fseek( r3dFile *f, long offset, int whence );
int			fseek( r3dWindowsFileHandle file, long offset, int whence );
int			fseek( r3dMappedViewOfFile* view, long offset, int whence );
int			fseek( r3dChunkedFile* f, long offset, int wence );
int			fseek( r3dChunkedWriteFile* f, long offset, int wence );

int			fread( void *ptr, size_t size, size_t n, r3dUnpackedFile *f );
size_t		fread( void *ptr, size_t size, size_t n, r3dFile *f );
size_t		fread( void *ptr, size_t size, size_t n, r3dWindowsFileHandle file );
size_t		fread( void *ptr, size_t size, size_t n, r3dMappedViewOfFile* view );
size_t		fread( void *ptr, size_t size, size_t n, r3dChunkedFile* file );
size_t		fwrite( const void* ptr, size_t size, size_t n, r3dWindowsFileHandle file );
size_t		fwrite( const void* ptr, size_t size, size_t n, r3dMappedViewOfFile* view );
size_t		fwrite( const void* ptr, size_t size, size_t n, r3dChunkedWriteFile* file );
char* 		fgets(char* s, int n, r3dFile *f);
long		ftell( r3dFile *f );
long		ftell( r3dChunkedFile * f );
long		ftell( r3dChunkedWriteFile * f );
long		ftell( r3dWindowsFileHandle file );
int		feof(r3dFile* f);

int		r3d_create_path( const char* path ) ;
int		r3d_delete_dir( const char* path ) ;
bool	r3d_remove_files_in_folder( const char* pathWithFilePattern );

int		r3dCompareFileNameInPath( const char* path0, const char* path1 );
int		r3dFilesEqual( const char* path0, const char* path1 );

FILE*		fopen_for_write(const char* fname, const char* mode);

bool CreateWorkPath(char* dest);
bool CreateConfigPath(char* dest);

void ftruncate( r3dChunkedWriteFile* file, size_t size );
void ftruncate( FILE* file, size_t size );

int r3dDirectoryExists( const char* path );

INT64 r3dGetDiskSpace( const char* filePath );

struct r3dFileSystemLock
{
	r3dFileSystemLock();
	~r3dFileSystemLock();

	 r3dCSHolderWithDeviceQueue csHolder;
};

#define R3D_FILE_SYSTEM_LOCK_SCOPE() r3dFileSystemLock fs_locked_scope; (void)fs_locked_scope;

#endif	// __R3D_FILEMAN_H
