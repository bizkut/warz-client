#include	"r3dPCH.h"
#include	"r3d.h"

template<class T>
inline T sgMax(T a, T b) { return a > b ? a : b; }

template<class T>
inline T sgMin(T a, T b) { return a < b ? a : b; }


//
//
//
BOOL r3dRect::Contains(const r3dPoint2D &p) const
{
  float px = p.x - org.x;
  float py = p.y - org.y;
  if(px < 0 || py < 0) return FALSE;
  if(px >= size.x || py >= size.y) return FALSE;
  return TRUE;
}

void r3dRect::Intersect(const r3dRect& r)
{
  org.x = sgMax(org.x, r.org.x);
  org.y = sgMax(org.y, r.org.y);
  float xEnd = org.x + size.x;
  float yEnd = org.y + size.y;
  float xEnd2 = r.org.x + r.size.x;
  float yEnd2 = r.org.y + r.size.x;
  size.x = sgMin(xEnd, xEnd2);
  size.y = sgMin(yEnd, yEnd2);

  size.x -= org.x;
  size.y -= org.y;
}

void r3dRect::Union(const r3dRect& r)
{
  org.x = sgMin(org.x, r.org.x);
  org.y = sgMin(org.y, r.org.y);
  float xEnd  = org.x + size.x;
  float yEnd  = org.y + size.y;
  float xEnd2 = r.org.x + r.size.x;
  float yEnd2 = r.org.y + r.size.y;
  size.x = sgMax(xEnd, xEnd2);
  size.y = sgMax(yEnd, yEnd2);

  size.X -= org.x;
  size.Y -= org.y;
}


BOOL r3dRect::IsIntersect(const r3dPoint2D & p)
{
 if( ( (p.x >= org.x) && (p.x <= (org.x + size.x)) ) &&
     ( (p.y >= org.y) && (p.y <= (org.y + size.y)) )
   )
 return TRUE;
 else return FALSE;
}

BOOL r3dRect::IsIntersect(float X, float Y)
{
 return IsIntersect(r3dPoint2D(X, Y));
}




r3dPolygon2D::r3dPolygon2D()
{
  points = 0;
  Type   = -1;
  X      = 0;
  Y      = 0;
  Width  = 0;
  Height = 0;
}


r3dPolygon2D::~r3dPolygon2D()
{
}



int r3dPolygon2D::Init(float x, float y)
{
  Type   = bRect;
  X      = x;
  Y      = y;
  Width  = 0;
  Height = 0;

return 1;
}

int r3dPolygon2D::Init(float x, float y, float W, float H)
{
  Type   = bRect;
  X      = x;
  Y      = y;
  Width  = W;
  Height = H;

return 1;
}


void r3dPolygon2D::FixBound()
{
  switch(Type) {
    case bPolygon:
      return;

    case bRect:
      if(X < 0)  			X      = 0;
      if(Y < 0)  			Y      = 0;
      if(X >= 2000) 		X      = 2000 - 1;
      if(Y >= 2000)			Y      = 2000 - 1;
      if(X + Width  > 2000) Width  = 2000 - X;
      if(Y + Height > 2000) Height = 2000 - Y;
      break;

    case bPoint:
      if(X < 0)  			X = 0;
      if(Y < 0)  			Y = 0;
      if(X >= 2000) 		X = 2000 - 1;
      if(Y >= 2000)			Y = 2000 - 1;
      break;
  }

return;
}

int r3dPolygon2D::GetPoints()
{
  switch(Type) {
    case bPolygon:	return points;
    case bRect:		return 4;
    case bPoint:	return 1;
  }
  return 0;
}

r3dPoint2D r3dPolygon2D::GetEdge(int v)
{
  switch(Type) {
    case bPolygon:
      if(v < points)
	return edges[v];
      break;
    case bRect:
      switch(v) {
	case 0: return r3dPoint2D( X,         Y);
	case 1: return r3dPoint2D( X + Width, Y);
	case 2: return r3dPoint2D( X + Width, Y + Height);
	case 3: return r3dPoint2D( X        , Y + Height);
      }
      break;
    case bPoint:
      break;
  }
  return r3dPoint2D(X, Y);
}

int r3dPolygon2D::Contains(int x, int y)
{
  switch(Type) {
    case bPoint:
      return x == X && y == Y;
    case bRect:
      return x >= X && x <= (X + Width) &&
	     y >= Y && y <= (Y + Height);
    case bPolygon:
      return ContainsPoly(x, y);
  }
return 0;
}

/* Shoot a test ray along +X axis.  The strategy is to compare vertex Y values
 * to the testing point's Y and quickly discard edges which are entirely to one
 * side of the test ray.  Note that CONVEX and WINDING code can be added as
 * for the CrossingsTest() code; it is left out here for clarity.
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.
 */
int r3dPolygon2D::ContainsPoly(int tx, int ty)
{
	int	j, yflag0, yflag1, inside_flag ;
	r3dPoint2D	vtx0, vtx1;

  inside_flag = 0 ;

  if(!points)
    return inside_flag;

  vtx0   = edges[points-1];
  vtx1   = edges[0];
  /* get test bit for above/below X axis */
  yflag0 = (vtx0.y >= ty ) ;

  for(j=0; j<points; j++) {
    yflag1 = (vtx1.y >= ty);
    /* Check if endpoints straddle (are on opposite sides) of X axis
     * (i.e. the Y's differ); if so, +X ray could intersect this edge.
     * The old test also checked whether the endpoints are both to the
     * right or to the left of the test point.  However, given the faster
     * intersection point computation used below, this test was found to
     * be a break-even proposition for most polygons and a loser for
     * triangles (where 50% or more of the edges which survive this test
     * will cross quadrants and so have to have the X intersection computed
     * anyway).  I credit Joseph Samosky with inspiring me to try dropping
     * the "both left or both right" part of my code.
     */
    if( yflag0 != yflag1 ) {
      /* Check intersection of pgon segment with +X ray.
       * Note if >= point's X; if so, the ray hits it.
       * The division operation is avoided for the ">=" test by checking
       * the sign of the first vertex wrto the test point; idea inspired
       * by Joseph Samosky's and Mark Haigh-Hutchinson's different
       * polygon inclusion tests.
       */
      if( ((vtx1.y - ty) * (vtx0.x - vtx1.x) >=
	   (vtx1.x - tx) * (vtx0.y - vtx1.y)) == yflag1 ) {
	inside_flag = !inside_flag;
      }
    }
    /* Move to the next pair of vertices, retaining info as possible. */
    yflag0 = yflag1;
    vtx0   = vtx1;
    vtx1   = edges[j+1];
  }

  return( inside_flag );
}
