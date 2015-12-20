//----------------------------------------------------------------------------------------------------------------------
/// \file   Tangents.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file provides a re-usable template function to calculate tangents and normals for 
///         an indexed list of triangles. It *would* be sensible of me to add in striding parameters to these 
///         functions to make it slightly more useful. There are three functions in all:
/// 
///              GenerateTangentsA
///              AverageTangents
///              GenerateTangentsB
/// 
///         The reason for this is that you can get some really nasty seams where the normals are not averaged 
///         properly. To fix this the VertexArray utilitiy class builds up a list of points where the vertex and 
///         normal are the same, but the tex coords differ.
///           Each set can be passed to the AverageTangents function so that the normals and tangents can be 
///         averaged correctly. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <math.h>
#include "XMD/XFn.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMU  
{
/// if 1 then when calculating the face normal, the output gets normalised.
/// If adjacent polys have almost the same area then when the per vertex normalisation
/// should still sort them out.
#define NORMALISE_PER_FACE 1

/// if set to 1, the normals get normalised. use 0 if you enable GL_NORMALIZE to sort
/// the normalisation for you.
#define NORMALISE_PER_VERTEX 1

/// if set to 1, the normals get normalised. use 0 if you enable GL_NORMALIZE to sort
/// the normalisation for you.
#define NORMALISE_TANGENTS 0

//----------------------------------------------------------------------------------------------------------------------
/// \brief  performs the first pass required to calculate tangents, normals and bi-normals on a geometry object. 
///         This pass simply calculates the TBN for each face and sums into the output TBN array. The result at the 
///         end of this pass will therefore be un-normalised TBN's for each vertex in the mesh. 
/// \param  num_indices - the vertex index data
/// \param  num_elems - the number of vertex elements in the data array
/// \param  indices - an array of vertex indices
/// \param  verts - the vertex data array
/// \param  uv_data - the texture co-ordinate data array
/// \param  TBN - the output TBN data
/// \note   This is intended to work only with vertex array data
//---------------------------------------------------------------------------------------------------------------------- 
template
<
  typename TIndex,
  typename TData
>
void GenerateTangentsA(const XMD::XU32 num_indices,
                       const TIndex num_elems,
                       TIndex* indices,
                       TData* verts,
                       TData* uv_data,
                       TData* TBN)
{
  {
    TData* n = TBN;
    TData* end = n + num_elems*9;

    for( ; n != end; ++n )
    {
      *n = 0;
    }
  }

  TData* uv = uv_data;

  TIndex* it = indices;
  TIndex* end = it + num_indices;
  TData* tangent = TBN;

  for( ; it != end; it+=3 )
  {
    TData* binormal = tangent+3;
    TData* normal = tangent+6;

    TIndex idx1 = *it ;
    TIndex idx2 = *(it+1);
    TIndex idx3 = *(it+2);

    TData* tbn1 = TBN + idx1 * 9;
    TData* tbn2 = TBN + idx2 * 9;
    TData* tbn3 = TBN + idx3 * 9;

    TData* uv1 = uv_data + idx1 * 2;
    TData* uv2 = uv_data + idx2 * 2;
    TData* uv3 = uv_data + idx3 * 2;

    idx1 *= 3;
    idx2 *= 3;
    idx3 *= 3;

    const TData* v1 = verts + idx1 ;
    const TData* v2 = verts + idx2 ;
    const TData* v3 = verts + idx3 ;

    TData e1[3] = {
      v2[0] - v1[0],
      v2[1] - v1[1],
      v2[2] - v1[2]
    };

    TData e2[3] = {
      v3[0] - v1[0],
      v3[1] - v1[1],
      v3[2] - v1[2]
    };

    TData uve1[2] = {
      uv2[0] - uv1[0],
      uv2[1] - uv1[1]
    };
    TData uve2[2] = {
      uv3[0] - uv1[0],
      uv3[1] - uv1[1]
    };

    XM2::XReal r = 1.0F / (uve1[0] * uve2[1] - uve2[0] * uve1[1]);

    TData sdir[3] = {
        (uve2[1] * e1[0] - uve1[1] * e2[0]) * r,
        (uve2[1] * e1[1] - uve1[1] * e2[1]) * r,
        (uve2[1] * e1[2] - uve1[1] * e2[2]) * r
    };
    TData tdir[3] = {
        (uve1[0] * e1[0] - uve2[0] * e2[0]) * r,
        (uve1[0] * e1[1] - uve2[0] * e2[1]) * r,
        (uve1[0] * e1[2] - uve2[0] * e2[2]) * r
    };

    tbn1[0] += sdir[0];
    tbn1[1] += sdir[1];
    tbn1[2] += sdir[2];

    tbn2[0] += sdir[0];
    tbn2[1] += sdir[1];
    tbn2[2] += sdir[2];

    tbn3[0] += sdir[0];
    tbn3[1] += sdir[1];
    tbn3[2] += sdir[2];


    tbn1[3] += tdir[0];
    tbn1[4] += tdir[1];
    tbn1[5] += tdir[2];

    tbn2[3] += tdir[0];
    tbn2[4] += tdir[1];
    tbn2[5] += tdir[2];

    tbn3[3] += tdir[0];
    tbn3[4] += tdir[1];
    tbn3[5] += tdir[2];

    TData e1_cross_e2[3] = {
      e1[1] * e2[2] - e1[2] * e2[1],
      e1[2] * e2[0] - e1[0] * e2[2],
      e1[0] * e2[1] - e1[1] * e2[0]
    };

    #if NORMALISE_PER_FACE
    {
      TData itt = 1.0f/((TData)sqrt(  e1_cross_e2[0]*e1_cross_e2[0] + 
                      e1_cross_e2[1]*e1_cross_e2[1] + 
                      e1_cross_e2[2]*e1_cross_e2[2] ));
      e1_cross_e2[0] *= itt;
      e1_cross_e2[1] *= itt;
      e1_cross_e2[2] *= itt;
    }
    #endif

    tbn1[6] += e1_cross_e2[0];
    tbn1[7] += e1_cross_e2[1];
    tbn1[8] += e1_cross_e2[2];

    tbn2[6] += e1_cross_e2[0];
    tbn2[7] += e1_cross_e2[1];
    tbn2[8] += e1_cross_e2[2];

    tbn3[6] += e1_cross_e2[0];
    tbn3[7] += e1_cross_e2[1];
    tbn3[8] += e1_cross_e2[2];
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This stage of the tangent calculation basically averages the normals, tangents and bi-normals where 
///         there are UV borders (ie, the texture coords wrap around).
/// \param  num - number of TBN's to average.
/// \param  data - the indices to average
/// \param  normal_array - the TBN data array
/// \note   This is intended to work only with vertex array data
//----------------------------------------------------------------------------------------------------------------------
template<typename TIndex,typename TData>
void AverageTangents( const XMD::XU8 num, const TIndex* data, TData* normal_array )
{
  TData* tbn0 = normal_array + 9*data[0];

  // sum the other normals into the first one
  for(XMD::XU8 i=1;i<num;++i)
  {
    TData* tbn1 = normal_array + 9*data[i];
#if NORMALISE_TANGENTS
    tbn0[0] += tbn1[0];
    tbn0[1] += tbn1[1];
    tbn0[2] += tbn1[2];
    tbn0[3] += tbn1[3];
    tbn0[4] += tbn1[4];
    tbn0[5] += tbn1[5];  
#endif
    tbn0[6] += tbn1[6];
    tbn0[7] += tbn1[7];
    tbn0[8] += tbn1[8];
  }

  // then write the first one over the other normals
  for(XMD::XU8 i=1;i<num;++i)
  {
    TData* tbn1 = normal_array + 9*data[i];
#if NORMALISE_TANGENTS
    tbn1[0] = tbn0[0];
    tbn1[1] = tbn0[1];
    tbn1[2] = tbn0[2];
    tbn1[3] = tbn0[3];
    tbn1[4] = tbn0[4];
    tbn1[5] = tbn0[5];  
#endif
    tbn1[6] = tbn0[6];
    tbn1[7] = tbn0[7];
    tbn1[8] = tbn0[8];
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This final stage in the calculation iterates over all of the 
///         normals, tangents, and bi-normals and normalises them. It also 
///         makes sure that all 3 vectors are orthogonal to each other.
/// \param  num_indices - the number of triangle indices in the array
/// \param  num_elems - the number of vertex array elements
/// \param  indices - the triangle indices
/// \param  verts - the vertex data array
/// \param  uv_data - the UV coordinate data
/// \param  TBN - the TBN data. 9 elements per vertex
/// \note   This is intended to work only with vertex array data
//----------------------------------------------------------------------------------------------------------------------
template
<
  typename TIndex,
  typename TData
>
void GenerateTangentsB(const XMD::XU32 num_indices,
                       const TIndex num_elems,
                       TIndex* indices,
                       TData* verts,
                       TData* uv_data,
                       TData* TBN)
{
  #if NORMALISE_PER_VERTEX

  {
    TData* t = TBN;
    TData* end = t + num_elems*9;

    for( ; t != end; t+=9 )
    {
      TData nnt[3];
      TData* b = t+3;
      TData* n = b+3;

      // normalise the normal vector
      {
        TData it = 1.0f/((XM2::XReal)sqrt( n[0] * n[0] + 
                      n[1] * n[1] + 
                      n[2] * n[2] ));
        n[0] *= it;
        n[1] *= it;
        n[2] *= it;
      }

      TData il;
      TData n_dot_t = ( n[0] * t[0] + n[1] * t[1] + n[2] * t[2] );

      nnt[0] = t[0] - n_dot_t * n[0];
      nnt[1] = t[1] - n_dot_t * n[1];
      nnt[2] = t[2] - n_dot_t * n[2];

      il = 1.0f/((TData)sqrt( nnt[0]*nnt[0] + nnt[1]*nnt[1] + nnt[2]*nnt[2] ));

      // Gram-Schmidt orthogonalize
      t[0] = il * nnt[0];
      t[1] = il * nnt[1];
      t[2] = il * nnt[2];

      bool left_hand;

      // Calculate handedness
      {
        TData n_cross_t[3] = {
          n[1] * t[2] - n[2] * t[1], 
          n[2] * t[0] - n[0] * t[2],
          n[0] * t[1] - n[1] * t[0]
        };

        TData nt_dot_tan2 = 
          n_cross_t[0] * b[0] +
          n_cross_t[1] * b[1] +
          n_cross_t[2] * b[2];

        left_hand = (nt_dot_tan2 < 0.0f) ? true : false;

        if (!left_hand) {
          b[0] = -n_cross_t[0];
          b[1] = -n_cross_t[1];
          b[2] = -n_cross_t[2];
        }
        else {
          b[0] = n_cross_t[0];
          b[1] = n_cross_t[1];
          b[2] = n_cross_t[2];
        }
      }


    }
  #endif
  }
}
}
