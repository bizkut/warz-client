#pragma once

struct r3dGrannyFile
{
	friend r3dGrannyFile*	r3dOpenGranny( const char* fileName ) ;
	friend void				r3dCloseGranny( r3dGrannyFile* file ) ;

	struct granny_file *		file ;
	struct granny_file_info *	fileInfo ;

private:
	explicit r3dGrannyFile();
	~r3dGrannyFile();
};
