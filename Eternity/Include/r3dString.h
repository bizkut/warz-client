#ifndef __TSG_STL_USTRING
#define __TSG_STL_USTRING

class r3dString
{
  private:
	char		*data;			// must be FIRST
	int		len, maxlen;

	void		Assign(const char* str);

  public:
			r3dString();
			r3dString(const char* str);
			r3dString( const r3dString& cpy );
			~r3dString();
	char		operator[](int idx);
	r3dString&	operator=(const char* str);
	r3dString& 	operator=(const r3dString& stringSrc);

	friend int operator == ( const r3dString& u, const char* str);
	friend int operator != ( const r3dString& u, const char* str);

	friend int operator == ( const r3dString& u0, const r3dString& u1 );
	friend int operator != ( const r3dString& u0, const r3dString& u1 );

	int Length() const;

	const char*	c_str() const;
};

r3dString operator + ( const r3dString& s0, const r3dString& s1 );

r3dString& operator += ( r3dString& s0, const r3dString& s1 );

inline
int
r3dString::Length() const
{
	return len - 1;
}

//------------------------------------------------------------------------

char *stristr (const char *s1, const char *s2);

R3D_FORCEINLINE r3dString& r3dscpy_s(r3dString& targ, const r3dString& src )
{
	targ = src;
	return targ;
}

R3D_FORCEINLINE r3dString& r3dscpy(r3dString& targ, const char* src )
{
	targ = src;
	return targ;
}

#endif //__TSG_STL_USTRING
