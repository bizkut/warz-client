#ifndef	__ETERNITY_POLY2D_H
#define	__ETERNITY_POLY2D_H

//----------------------------------------------------------------
class r3dRect
//----------------------------------------------------------------
{
  public:

	r3dPoint2D	org, size;		// Origin and size of the rectangle

	r3dRect( float ax, float ay, float sx, float sy ){ org.x = ax; org.y = ay; size.x = sx; size.y = sy; }
	r3dRect( r3dPoint2D p1, r3dPoint2D sz ){ org = p1; size = sz;}
	r3dRect(){}

inline	void	Assign(float x, float y, float w, float h) { org.x = x; org.y = y; size.x = w; size.y = h; }
	void	Move(int aDX, int aDY){ org.x += aDX; org.y += aDY; }
	void	Grow(int aDX, int aDY){ size.x += aDX; size.y += aDY; }
	void	Intersect( const r3dRect& r );
	void	Union( const r3dRect& r );
	BOOL	Contains( const r3dPoint2D& p ) const;

        BOOL	IsIntersect(const r3dPoint2D & p);
        BOOL	IsIntersect(float X, float Y);        
	BOOL	IsEmpty() const	{ return BOOL(size.x <=0 || size.y <=0); }
};


//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
class r3dPolygon2D
//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
{
  public:
  	enum BoundTypes_e
  	{
    	  bPoint,
    	  bRect,
    	  bPolygon,

    	  MaxEdges = 30,
  	};
	int		Type;
	int		points;
	r3dPoint2D		edges[MaxEdges];

	float		X, Y;
	float		Width, Height;

  public:
			r3dPolygon2D();
			r3dPolygon2D(r3dPoint2D);
			r3dPolygon2D(r3dRect);
			~r3dPolygon2D();

	int		Init(float x, float y);
	int		Init(float x, float y, float W, float H);
	void		FixBound();
	int		Contains(int x, int y);
	int		ContainsPoly(int x, int y);

inline	void		SetX(float v)       { X      = v; }
inline	void		SetY(float v)       { Y      = v; }
inline	void		SetWidth(float v)	  { Width  = v; }
inline	void		SetHeight(float v)  { Height = v; }

	int		GetPoints();
	r3dPoint2D		GetEdge(int v);
};


#endif	// __ETERNITY_POLY2D_H

