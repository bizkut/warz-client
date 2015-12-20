#include "r3dPCH.h"
#include "r3dString.h"

r3dString::r3dString()
{
  len = 1;
  maxlen = 0;
  data   = NULL;
}

r3dString::r3dString(const char* d)
{
  maxlen = 0;
  data   = NULL;
  Assign(d);
}

r3dString::r3dString( const r3dString& cpy )
{
	maxlen	= 0;
	data	= NULL;
	if( cpy.data )
	{
		Assign(cpy.data);
	}
}

r3dString::~r3dString()
{
  delete[] data;
}


void r3dString::Assign(const char* str)
{
	// length PLUS terminating zero 
	int	slen = strlen(str) + 1;

  if(maxlen < slen) {
    delete[] data;
    data   = game_new char[slen];
    maxlen = slen;
  }
  r3dscpy(data, str);
  len = slen;

  return;
}

char r3dString::operator[](int idx)
{
  if(data == NULL || idx >= len)
    return '\0';
  return data[idx];
}


r3dString& r3dString::operator=(const char* str)
{
  Assign(str);
  return *this;
}

r3dString& r3dString::operator=(const r3dString& str)
{
  if( this != &str )
  {
    Assign(str.c_str());
  }
  return *this;
}


int operator==( const r3dString& u, const char* str )
{
  if( u.data == NULL)
    return 0;
  return( stricmp( u.data, str) == NULL );
}

int operator!=( const r3dString& u, const char* str )
{
  if( u.data == NULL)
    return 1;
  return( stricmp( u.data, str) != NULL );
}

int operator == ( const r3dString& u0, const r3dString& u1 )
{
	return u0 == u1.c_str();
}

int operator != ( const r3dString& u0, const r3dString& u1 )
{
	return u0 != u1.c_str();
}

const char* r3dString::c_str() const
{
  if(data == NULL)
    return "";
  return data;
}

r3dString operator + ( const r3dString& s0, const r3dString& s1 )
{
	const char* cs0 = s0.c_str();
	const char* cs1 = s1.c_str();

	int s0len = strlen( cs0 );
	int s1len = strlen( cs1 );

	char* newString = gfx_new char[ s0len + s1len + 1 ];

	strcat( r3dscpy( newString, cs0 ), cs1 );
	
	r3dString res( newString );

	delete [] newString;

	return res;
}

r3dString& operator += ( r3dString& s0, const r3dString& s1 )
{
	r3dString sum = s0 + s1;
	return s0 = sum;
}

//------------------------------------------------------------------------

char *stristr (const char *s1, const char *s2)
{
	char *cp = (char*) s1;
	char *s, *t, *endp;
	char l, r;

	endp = (char*)s1 + ( strlen(s1) - strlen(s2)) ;
	while (*cp && (cp <= endp)) {
		s = cp;
		t = (char*)s2;
		while (*s && *t) {
			l = toupper(*s);
			r = toupper(*t);
			if (l != r)
				break;
			s++, t++;
		}

		if (*t == 0)
			return cp;

		cp ++ ;
	}

	return 0;
}

