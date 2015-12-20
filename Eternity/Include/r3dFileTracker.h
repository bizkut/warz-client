#pragma once

// file changes
typedef void ( CallbackFileChange_t )( const char * szFileName );

void		FileTrackReInit		();
void		FileTrackShutdown	();
void		FileTrackDoWork		();
void		FileTrackChanges	( const char * szPath, CallbackFileChange_t * pFunc );

void		FixFileName		( const char * szFileName, char * szResult );
