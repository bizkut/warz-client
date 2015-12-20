/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   BoundingBox.c
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Vector3.h"
#include "XM2/Matrix34.h"
#include "XM2/Vector3Array.h"
#include "XM2/Vector4Array.h"
#include "XM2/Vector3SOAArray.h"
#include "XM2/Vector4SOAArray.h"
#include "XM2/Vector3PackedArray.h"
#include "XM2/BoundingBox.h"
#include "XM2/Plane.h"
#include "XM2/Ray.h"

namespace XM2
{    
//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::expand(const XVector3Array& p)
{
  XM2_ALIGN_ASSERT(this);
  const int num = (int)p.size();

  #if !XM2_USE_OMP
  for(int i=0;i<num;++i)
  {
    expand(p[i]);
  }
  #else
    //
    // can't use a simple parallel for here since there's a possibility
    // that each thread will ignore each other. Instead split the array into 
    // 4, and do 4 concurrent sections of the array, then re-combine later... 
    //
    XBoundingBox a;
    XBoundingBox b;
    XBoundingBox c;
    XBoundingBox d;
    int num4 = num/4;
    int num_start0 = 0;
    int num_start1 = num4;
    int num_start2 = num4*2;
    int num_start3 = num4*3;
    #pragma omp parallel       
    {
      #pragma omp sections  
      {  
        #pragma omp section
        {
          a.clear();
          for (int i=num_start0;i<num_start1;++i)
          {
            a.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          b.clear();
          for (int i=num_start1;i<num_start2;++i)
          {
            b.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          c.clear();
          for (int i=num_start2;i<num_start3;++i)
          {
            c.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          d.clear();
          for (int i=num_start3;i<num;++i)
          {
            d.expand(p[i]);
          }
        } 
      }
    }

    // now expand this bounding box by the 4 computed by each thread
    expand(a);
    expand(b);
    expand(c);
    expand(d);
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::expand(const XVector4Array& p)
{
  XM2_ALIGN_ASSERT(this);
  const int num = (int)p.size();

  #if !XM2_USE_OMP
  for(int i=0;i<num;++i)
  {
    expand(p[i]);
  }
  #else
    //
    // can't use a simple parallel for here since there's a possibility
    // that each thread will ignore each other. Instead split the array into 
    // 4, and do 4 concurrent sections of the array, then re-combine later... 
    //
    XBoundingBox a;
    XBoundingBox b;
    XBoundingBox c;
    XBoundingBox d;
    int num4 = num/4;
    int num_start0 = 0;
    int num_start1 = num4;
    int num_start2 = num4*2;
    int num_start3 = num4*3;
    #pragma omp parallel       
    {
      #pragma omp sections  
      {  
        #pragma omp section
        {
          a.clear();
          for (int i=num_start0;i<num_start1;++i)
          {
            a.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          b.clear();
          for (int i=num_start1;i<num_start2;++i)
          {
            b.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          c.clear();
          for (int i=num_start2;i<num_start3;++i)
          {
            c.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          d.clear();
          for (int i=num_start3;i<num;++i)
          {
            d.expand(p[i]);
          }
        } 
      }
    }

    // now expand this bounding box by the 4 computed by each thread
    expand(a);
    expand(b);
    expand(c);
    expand(d);
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::expand(const XVector3PackedArray& p)
{
  XM2_ALIGN_ASSERT(this);
  const int num = (int)p.size();

  #if !XM2_USE_OMP
  for(int i=0;i<num;++i)
  {
    expand(p[i]);
  }
  #else
    //
    // can't use a simple parallel for here since there's a possibility
    // that each thread will ignore each other. Instead split the array into 
    // 4, and do 4 concurrent sections of the array, then re-combine later... 
    //
    XBoundingBox a;
    XBoundingBox b;
    XBoundingBox c;
    XBoundingBox d;
    int num4 = num/4;
    int num_start0 = 0;
    int num_start1 = num4;
    int num_start2 = num4*2;
    int num_start3 = num4*3;
    #pragma omp parallel       
    {
      #pragma omp sections  
      {  
        #pragma omp section
        {
          a.clear();
          for (int i=num_start0;i<num_start1;++i)
          {
            a.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          b.clear();
          for (int i=num_start1;i<num_start2;++i)
          {
            b.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          c.clear();
          for (int i=num_start2;i<num_start3;++i)
          {
            c.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          d.clear();
          for (int i=num_start3;i<num;++i)
          {
            d.expand(p[i]);
          }
        } 
      }
    }

    // now expand this bounding box by the 4 computed by each thread
    expand(a);
    expand(b);
    expand(c);
    expand(d);
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::expand(const XVector3SOAArray& p)
{
  XM2_ALIGN_ASSERT(this);
  const int num = (int)p.size();

  #if !XM2_USE_OMP
  for(int i=0;i<num;++i)
  {
    expand(p[i]);
  }
  #else
    //
    // can't use a simple parallel for here since there's a possibility
    // that each thread will ignore each other. Instead split the array into 
    // 4, and do 4 concurrent sections of the array, then re-combine later... 
    //
    XBoundingBox a;
    XBoundingBox b;
    XBoundingBox c;
    XBoundingBox d;
    int num4 = num/4;
    int num_start0 = 0;
    int num_start1 = num4;
    int num_start2 = num4*2;
    int num_start3 = num4*3;
    #pragma omp parallel       
    {
      #pragma omp sections  
      {  
        #pragma omp section
        {
          a.clear();
          for (int i=num_start0;i<num_start1;++i)
          {
            a.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          b.clear();
          for (int i=num_start1;i<num_start2;++i)
          {
            b.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          c.clear();
          for (int i=num_start2;i<num_start3;++i)
          {
            c.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          d.clear();
          for (int i=num_start3;i<num;++i)
          {
            d.expand(p[i]);
          }
        } 
      }
    }

    // now expand this bounding box by the 4 computed by each thread
    expand(a);
    expand(b);
    expand(c);
    expand(d);
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::expand(const XVector4SOAArray& p)
{
  XM2_ALIGN_ASSERT(this);
  const int num = (int)p.size();

  #if !XM2_USE_OMP
  for(int i=0;i<num;++i)
  {
    expand(p[i]);
  }
  #else
    //
    // can't use a simple parallel for here since there's a possibility
    // that each thread will ignore each other. Instead split the array into 
    // 4, and do 4 concurrent sections of the array, then re-combine later... 
    //
    XBoundingBox a;
    XBoundingBox b;
    XBoundingBox c;
    XBoundingBox d;
    int num4 = num/4;
    int num_start0 = 0;
    int num_start1 = num4;
    int num_start2 = num4*2;
    int num_start3 = num4*3;
    #pragma omp parallel       
    {
      #pragma omp sections  
      {  
        #pragma omp section
        {
          a.clear();
          for (int i=num_start0;i<num_start1;++i)
          {
            a.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          b.clear();
          for (int i=num_start1;i<num_start2;++i)
          {
            b.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          c.clear();
          for (int i=num_start2;i<num_start3;++i)
          {
            c.expand(p[i]);
          }
        }
        #pragma omp section
        {  
          d.clear();
          for (int i=num_start3;i<num;++i)
          {
            d.expand(p[i]);
          }
        } 
      }
    }

    // now expand this bounding box by the 4 computed by each thread
    expand(a);
    expand(b);
    expand(c);
    expand(d);
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::transformUsing(const XMatrix& mat)
{
  XM2_ALIGN_ASSERT(&mat);
  XM2_ALIGN_ASSERT(this);

  XVector3 verts[8];

  /* init 8 corner points */
  verts[0].set(maximum.x, maximum.y, maximum.z );
  verts[1].set(minimum.x, maximum.y, maximum.z );
  verts[2].set(maximum.x, minimum.y, maximum.z );
  verts[3].set(maximum.x, maximum.y, minimum.z );
  verts[4].set(minimum.x, minimum.y, maximum.z );
  verts[5].set(minimum.x, maximum.y, minimum.z );
  verts[6].set(maximum.x, minimum.y, minimum.z );
  verts[7].set(minimum.x, minimum.y, minimum.z );

  /* clear bbox */
  clear();

  for(int i=0;i<8;++i)
  {
    XVector3 temp;
    mat.transformPoint(temp,verts[i]);
    expand(temp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XBoundingBox::intersects(const XBoundingBox& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);

  /* seperate axis theorum */
  return (minimum.x < b.maximum.x) && (minimum.y < b.maximum.y) && (minimum.z < b.maximum.z) && 
         (maximum.x > b.minimum.x) && (maximum.y > b.minimum.y) && (maximum.z > b.minimum.z);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBoundingBox::intersects(const XRay& b,XVector3& output) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  /*
   *  This makes a number of assumptions due to the bounding box being axis aligned.
   *  First off, it reduces the number of planes to test by ignoring all back faces.
   *  This leaves us with 3 possible intersection distances, xd, yd, and zd
   */
  XReal distances[3];
  int order[3];

  /*
   *  if we are inside the box, the test gets reversed, hence the temp var dir... 
   */
  XVector3 dir = b.direction;

  if (contains(b.origin))
  {
    dir.negate();
  }

  if(dir.x<0)
  {
    XPlane p(1.0f,0.0f,0.0f,-maximum.x);
    p.lineIntersect( b, distances[0] );
  }
  else
  if(dir.x>=0)
  {
    XPlane p(-1.0f,0.0f,0.0f,minimum.x);
    p.lineIntersect( b, distances[0] );
  }

  if(dir.y<0)
  {
    XPlane p(0.0f,1.0f,0.0f,-maximum.y);
    p.lineIntersect( b, distances[1] );
  }
  else
  if(dir.y>=0)
  {
    XPlane p(0.0f,-1.0f,0.0f,minimum.y);
    p.lineIntersect( b, distances[1] );
  }

  if(dir.z<0)
  {
    XPlane p(0.0f,0.0f,1.0f,-maximum.z);
    p.lineIntersect( b, distances[2] );
  }
  else
  if(dir.z>=0)
  {
    XPlane p(0.0f,0.0f,-1.0f,minimum.z);
    p.lineIntersect( b, distances[2] );
  }

  if (distances[0]>=distances[1] && 
      distances[0]>=distances[2])
  {
    order[0] = 0;
    if (distances[1]>=distances[2])
    {
      order[1] = 1;
      order[2] = 2;
    }
    else
    {
      order[1] = 2;
      order[2] = 1;
    }
  }
  else
  if (distances[1]>=distances[0] && 
      distances[1]>=distances[2])
  {
    order[0] = 1;
    if (distances[0]>=distances[2])
    {
      order[1] = 0;
      order[2] = 2;
    }
    else
    {
      order[1] = 2;
      order[2] = 0;
    }
  }
  else
  {
    order[0] = 2;
    if (distances[0]>=distances[1])
    {
      order[1] = 0;
      order[2] = 1;
    }
    else
    {
      order[1] = 1;
      order[2] = 0;
    }
  }

  output.mul(b.direction,distances[order[0]]);
  output.add(b.origin);

  if( contains(output) )
  {
    return true;
  }

  output.mul(b.direction,distances[order[1]]);
  output.add(b.origin);

  if( contains(output) )
  {
    return true;
  }

  output.mul(b.direction,distances[order[2]]);
  output.add(b.origin);

  if( contains(output) )
  {
    return true;
  }

  return false;
}

#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::fprint32(FILE* fp) const
{
  minimum.fprint32(fp);
  maximum.fprint32(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::fprint64(FILE* fp) const
{
  minimum.fprint64(fp);
  maximum.fprint64(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::fscan32(FILE* fp)
{
  minimum.fscan32(fp);
  maximum.fscan32(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::fscan64(FILE* fp)
{
  minimum.fscan64(fp);
  maximum.fscan64(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::print32() const
{
  minimum.print32();
  maximum.print32();
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::print64() const
{
  minimum.print64();
  maximum.print64();
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::fread32(FILE* fp,bool flip_bytes)
{
  minimum.fread32(fp,flip_bytes);
  maximum.fread32(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::fread64(FILE* fp,bool flip_bytes)
{
  minimum.fread64(fp,flip_bytes);
  maximum.fread64(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::fwrite32(FILE* fp,bool flip_bytes) const
{
  minimum.fwrite32(fp,flip_bytes);
  maximum.fwrite32(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XBoundingBox::fwrite64(FILE* fp,bool flip_bytes) const
{
  minimum.fwrite64(fp,flip_bytes);
  maximum.fwrite64(fp,flip_bytes);
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& os,const XBoundingBox& data)
{
  return os << data.minimum << " " << data.maximum << std::endl;
}
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& is,XBoundingBox& data)
{
  return is >> data.minimum >> data.maximum;
}
#endif
}
