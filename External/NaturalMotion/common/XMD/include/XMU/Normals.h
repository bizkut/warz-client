//----------------------------------------------------------------------------------------------------------------------
/// \file   Normals.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file provides a re-usable template function to calculate normals for an indexed list 
///         of triangles. 
///         If the mesh has texture coordinates, then the normal calculation will not be quite accurate. Therefore 
///         the texture seams have to be modified slightly. See the vertex array utility class for more details. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <math.h>
//----------------------------------------------------------------------------------------------------------------------
namespace XMU  
{
/// if 1 then when calculating the face normal, the output gets normalised.
/// If adjacent polys have almost the same area then when the per vertex normalisation
/// should still sort them out.
#define GN_NORMALISE_PER_FACE 1

/// if set to 1, the normals get normalised. use 0 if you enable GL_NORMALIZE to sort
/// the normalisation for you.
#define GN_NORMALISE_PER_VERTEX 1

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This function generates the normal vectors for a mesh stored within a vertex array. 
/// \param  num_indices -  num indices within the vertex index array
/// \param  num_elems -  num elements within the vertex arrays
/// \param  indices -  the index array
/// \param  verts -  the vertex array
/// \param  vstride -  the vertex stride size
/// \param  norms -  the normal array
/// \param  nstride -  the normal stride size
//----------------------------------------------------------------------------------------------------------------------
template
<
  typename TIndex,
  typename TData
>
void GenerateNormals(const XMD::XU32 num_indices,
          const TIndex num_elems,
          TIndex* indices,
          TData* verts,
          XMD::XU32 vstride,
          TData* norms,
          XMD::XU32 nstride)
{
  // set all normals to 0,0,0
  {
    XMD::XU8 *p =  reinterpret_cast<XMD::XU8*>( (void*)norms );
    XMD::XU8 *pe = p + nstride*num_elems;

    for( ; p < pe; p += nstride )
    {
      TData* pn = reinterpret_cast<TData*>( (void*)p );
      pn[0] = pn[1] = pn[2] = 0;
    }
  }

  // loop through all the faces in the mesh
  TIndex* it = indices;
  TIndex* end = it + num_indices;

  for( ; it != end; it+=3 )
  {
    XMD::XU8* pverts = reinterpret_cast<XMD::XU8*>( (void*)verts );
    XMD::XU8* pv0 = pverts + *it * vstride;
    XMD::XU8* pv1 = pverts + *(it+1) * vstride;
    XMD::XU8* pv2 = pverts + *(it+2) * vstride;

    // get the vertices for the triangle
    TData* v1 =  reinterpret_cast<TData*>( (void*)pv0 );
    TData* v2 =  reinterpret_cast<TData*>( (void*)pv1 );
    TData* v3 =  reinterpret_cast<TData*>( (void*)pv2 );

    XMD::XU8* pnorms = reinterpret_cast<XMD::XU8*>( (void*)norms );
    XMD::XU8* pn0 = pnorms + *it * nstride;
    XMD::XU8* pn1 = pnorms + *(it+1) * nstride;
    XMD::XU8* pn2 = pnorms + *(it+2) * nstride;

    // get the vertices for the triangle
    TData* n1 =  reinterpret_cast<TData*>( (void*)pn0 );
    TData* n2 =  reinterpret_cast<TData*>( (void*)pn1 );
    TData* n3 =  reinterpret_cast<TData*>( (void*)pn2 );

    // calculate vector between v2 and v1
    TData e1[3] = {
      v1[0] - v2[0],
      v1[1] - v2[1],
      v1[2] - v2[2]
    };

    // calculate vector between v2 and v3
    TData e2[3] = {
      v3[0] - v2[0],
      v3[1] - v2[1],
      v3[2] - v2[2]
    };

    // cross product them
    TData e1_cross_e2[3] = {
      e2[1]*e1[2] - e2[2]*e1[1],
      e2[2]*e1[0] - e2[0]*e1[2],
      e2[0]*e1[1] - e2[1]*e1[0]
    };

    // might want to normalise the face normals
#if GN_NORMALISE_PER_FACE
    TData itt = 1.0f/((TData)sqrt( e1_cross_e2[0]*e1_cross_e2[0] + 
                                   e1_cross_e2[1]*e1_cross_e2[1] + 
                                   e1_cross_e2[2]*e1_cross_e2[2] ));

    e1_cross_e2[0] *= itt;
    e1_cross_e2[1] *= itt;
    e1_cross_e2[2] *= itt;
#endif

    // sum the face normal into all the vertex normals this face uses
    n1[0] += e1_cross_e2[0];
    n1[1] += e1_cross_e2[1];
    n1[2] += e1_cross_e2[2];

    n2[0] += e1_cross_e2[0];
    n2[1] += e1_cross_e2[1];
    n2[2] += e1_cross_e2[2];

    n3[0] += e1_cross_e2[0];
    n3[1] += e1_cross_e2[1];
    n3[2] += e1_cross_e2[2];
  }

  //  we will now have un-normalised versions of the normals. For lighting 
  //  to work correctly, we really need to normalise them. We could either
  //  use code like this, or just use glEnable(GL_NORMALIZE) 
  // 
#if GN_NORMALISE_PER_VERTEX
  {
    TData* n = norms;
    TData* end = n + num_elems*3;

    XMD::XU8 *p =  reinterpret_cast<XMD::XU8*>( (void*)norms );
    XMD::XU8 *pe = p + nstride*num_elems;

    for( ; p < pe; p += nstride )
    {
      TData* n = reinterpret_cast<TData*>( (void*)p );

      TData it = 1.0f/((TData)sqrt( n[0] * n[0] +
                    n[1] * n[1] +
                    n[2] * n[2] ));
      n[0] *= it;
      n[1] *= it;
      n[2] *= it;
    }
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  A utility function to average the vertex normals at UV coordinate border edges.
/// \param  num - the number of normals to average
/// \param  data - the indices of the normals to average together. Should be an array of size 'num'
/// \param  normal_array - the normal data
/// \param  nstride - the stride between each normal
//----------------------------------------------------------------------------------------------------------------------
template<typename TIndex,typename TData>
void AverageNormals( const XMD::XU8 num, const TIndex* data, TData* normal_array, XMD::XU32 nstride=3*sizeof(TData) )
{
  XMD::XU8* pn = reinterpret_cast<XMD::XU8*>(normal_array) + nstride*data[0];
  TData* n0 =  reinterpret_cast<TData*>(pn);

  // sum the other normals into the first one
  for(XMD::XU8 i=1;i<num;++i)
  {
    pn = reinterpret_cast<XMD::XU8*>(normal_array) + nstride*data[i];
    TData* n1 =  reinterpret_cast<TData*>(pn);
    n0[0] += n1[0];
    n0[1] += n1[1];
    n0[2] += n1[2];
  }

  // then write the first one over the other normals
  for(XMD::XU8 i=1;i<num;++i)
  {
    pn = reinterpret_cast<XMD::XU8*>(normal_array) + nstride*data[i];
    TData* n1 =  reinterpret_cast<TData*>(pn);
    n1[0] = n0[0];
    n1[1] = n0[1];
    n1[2] = n0[2];
  }
}
}
