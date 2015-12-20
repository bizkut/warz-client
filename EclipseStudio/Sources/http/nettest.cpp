// nettest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mynetwork.h"
#include <time.h>
#include <stdio.h>
#include <assert.h>

extern void UnittestMyNetwork();

static void get_url_path( char * buf, int len, char const * path )
{
  if( strlen( path ) <= 7 ) {
    strncpy( buf, "url.out", len );
    buf[len-1] = 0;
    return;
  }
  path += 7;
  strncpy( buf, path, len );
  buf[len-1] = 0;
  char * e = buf;
  while( *e ) {
    if( *e == '/' || *e == '.' || *e == ':' || *e == '\\' || *e == ' ' ) {
      *e = '_';
    }
    ++e;
  }
}

int _tmain(int argc, _TCHAR* argv[])
{
  UnittestMyNetwork();
  while( *++argv ) {
    fprintf( stderr, "%s:\n", *argv );
    I_HTTPRequest * r = NewHTTPRequest( *argv );
    if( !r ) {
      fprintf( stderr, "%s: error getting URL\n" );
    }
    else {
      time_t t;
      t = 0;
      char buf[4096];
      FILE * f = 0;
      while( true ) {
        r->step();
        size_t rd = r->read( buf, 4096 );
        if( rd > 0 ) {
          if( !f ) {
            char opath[1024];
            get_url_path( opath, 1024, *argv );
            f = fopen( opath, "wb" );
            if( !f ) {
              fprintf( stderr, "%s: cannot create %s\n", *argv, opath );
              goto next;
            }
          }
          fwrite( buf, 1, rd, f );
        }
        else {
          if( r->complete() ) {
            assert( f != 0 );
            fclose( f );
            goto next;
          }
          if( !t ) {
            time( &t );
          }
          else {
            time_t t2;
            time( &t2 );
            if( t2 > t+10 ) {
              fprintf( stderr, "%s: timeout\n", *argv );
              if( f ) {
                fclose( f );
              }
              goto next;
            }
          }
        }
      }
next:
      r->dispose();
    }
  }
	return 0;
}

