/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <hacdManifoldMesh.h>
using namespace std;


namespace HACD
{    
    Material::Material(void)
    {
        m_diffuseColor.X()  = 0.5;
        m_diffuseColor.Y()  = 0.5;
        m_diffuseColor.Z()  = 0.5;
        m_specularColor.X() = 0.5;
        m_specularColor.Y() = 0.5;
        m_specularColor.Z() = 0.5;
        m_ambientIntensity  = 0.4;
        m_emissiveColor.X() = 0.0;
        m_emissiveColor.Y() = 0.0;
        m_emissiveColor.Z() = 0.0;
        m_shininess         = 0.4;
        m_transparency      = 0.0;
    }
    
	TMMVertex::TMMVertex(void)
	{
		m_name = 0;
        m_id = 0;
		m_duplicate = 0;
		m_onHull = false;
		m_tag = false;
	}
	TMMVertex::~TMMVertex(void)
	{
	}
	TMMEdge::TMMEdge(void)
	{
        m_id = 0;
		m_triangles[0] = m_triangles[1] = m_newFace = 0;
		m_vertices[0] = m_vertices[1] = 0;
	}
	TMMEdge::~TMMEdge(void)
	{
	}
	TMMTriangle::TMMTriangle(void)
	{
        m_id = 0;
		for(int i = 0; i < 3; i++)
		{
			m_edges[i] = 0;
			m_vertices[0] = 0;
		}
		m_visible = false;
	}
	TMMTriangle::~TMMTriangle(void)
	{
	}
	TMMesh::TMMesh(void)
	{
		m_barycenter = Vec3<hacd::HaF64>(0.0,0.0,0.0);
		m_diag = 1.0;
	}
	TMMesh::~TMMesh(void)
	{
	}

    void TMMesh::GetIFS(Vec3<hacd::HaF64> * const points, Vec3<hacd::HaI32> * const triangles)
    {
        hacd::HaU32 nV = m_vertices.GetSize();
        hacd::HaU32 nT = m_triangles.GetSize(); 

        for(hacd::HaU32 v = 0; v < nV; v++)
        {
            points[v] = m_vertices.GetData().m_pos;
            m_vertices.GetData().m_id = v;
            m_vertices.Next();
        }
        for(hacd::HaU32 f = 0; f < nT; f++)
        {
            TMMTriangle & currentTriangle = m_triangles.GetData();
			triangles[f].X() = (hacd::HaI32)currentTriangle.m_vertices[0]->GetData().m_id;
            triangles[f].Y() = (hacd::HaI32)currentTriangle.m_vertices[1]->GetData().m_id;
            triangles[f].Z() = (hacd::HaI32)currentTriangle.m_vertices[2]->GetData().m_id;
            m_triangles.Next();
        }
    }
	void TMMesh::Clear()
    {
		m_vertices.Clear();
		m_edges.Clear();
		m_triangles.Clear();
	}
    void TMMesh::Copy(TMMesh & mesh)
    {
        Clear();
        // updating the id's
        hacd::HaU32 nV = mesh.m_vertices.GetSize();
        hacd::HaU32 nE = mesh. m_edges.GetSize();
        hacd::HaU32 nT = mesh.m_triangles.GetSize();
        for(hacd::HaU32 v = 0; v < nV; v++)
        {
            mesh.m_vertices.GetData().m_id = v;
            mesh.m_vertices.Next();            
        }
        for(hacd::HaU32 e = 0; e < nE; e++)
        {
            mesh.m_edges.GetData().m_id = e;
            mesh.m_edges.Next();
            
        }        
        for(hacd::HaU32 f = 0; f < nT; f++)
        {
            mesh.m_triangles.GetData().m_id = f;
            mesh.m_triangles.Next();
        }
        // copying data
        m_vertices  = mesh.m_vertices;
        m_edges     = mesh.m_edges;
        m_triangles = mesh.m_triangles;
 
        // generating mapping
        CircularListElement<TMMVertex> ** vertexMap     = (CircularListElement<TMMVertex> **) PX_ALLOC(sizeof(CircularListElement<TMMVertex> *)*nV, PX_DEBUG_EXP("TMMesh::Copy"));
        CircularListElement<TMMEdge> ** edgeMap         = (CircularListElement<TMMEdge> **)PX_ALLOC(sizeof(CircularListElement<TMMEdge> *)*nE, PX_DEBUG_EXP("TMMesh::Copy"));
        CircularListElement<TMMTriangle> ** triangleMap = (CircularListElement<TMMTriangle> **) PX_ALLOC(sizeof(CircularListElement<TMMTriangle> *)*nT, PX_DEBUG_EXP("TMMesh::Copy"));
        for(hacd::HaU32 v = 0; v < nV; v++)
        {
            vertexMap[v] = m_vertices.GetHead();
            m_vertices.Next();            
        }
        for(hacd::HaU32 e = 0; e < nE; e++)
        {
            edgeMap[e] = m_edges.GetHead();
            m_edges.Next();            
        }        
        for(hacd::HaU32 f = 0; f < nT; f++)
        {
            triangleMap[f] = m_triangles.GetHead();
            m_triangles.Next();
        }
        
        // updating pointers
        for(hacd::HaU32 v = 0; v < nV; v++)
        {
            if (vertexMap[v]->GetData().m_duplicate)
            {
                vertexMap[v]->GetData().m_duplicate = edgeMap[vertexMap[v]->GetData().m_duplicate->GetData().m_id];
            }
        }
        for(hacd::HaU32 e = 0; e < nE; e++)
        {
            if (edgeMap[e]->GetData().m_newFace)
            {
                edgeMap[e]->GetData().m_newFace = triangleMap[edgeMap[e]->GetData().m_newFace->GetData().m_id];
            }
			if (nT > 0)
			{
				for(int f = 0; f < 2; f++)
				{
					if (edgeMap[e]->GetData().m_triangles[f])
					{
						edgeMap[e]->GetData().m_triangles[f] = triangleMap[edgeMap[e]->GetData().m_triangles[f]->GetData().m_id];
					}
				}            
			}
            for(int v = 0; v < 2; v++)
            {
                if (edgeMap[e]->GetData().m_vertices[v])
                {
                    edgeMap[e]->GetData().m_vertices[v] = vertexMap[edgeMap[e]->GetData().m_vertices[v]->GetData().m_id];
                }
            }
        }        
        for(hacd::HaU32 f = 0; f < nT; f++)
        {
			if (nE > 0)
			{
				for(int e = 0; e < 3; e++)
				{
					if (triangleMap[f]->GetData().m_edges[e])
					{
						triangleMap[f]->GetData().m_edges[e] = edgeMap[triangleMap[f]->GetData().m_edges[e]->GetData().m_id];
					}
				}            
			}
            for(int v = 0; v < 3; v++)
            {
                if (triangleMap[f]->GetData().m_vertices[v])
                {
                    triangleMap[f]->GetData().m_vertices[v] = vertexMap[triangleMap[f]->GetData().m_vertices[v]->GetData().m_id];
                }
            }
        }
        PX_FREE(vertexMap);
        PX_FREE(edgeMap);
        PX_FREE(triangleMap);
        
    }
	hacd::HaI32  IntersectRayTriangle(const Vec3<hacd::HaF64> & P0, const Vec3<hacd::HaF64> & dir, 
							   const Vec3<hacd::HaF64> & V0, const Vec3<hacd::HaF64> & V1, 
							   const Vec3<hacd::HaF64> & V2, hacd::HaF64 &t)
	{
		Vec3<hacd::HaF64> edge1, edge2, edge3;
		hacd::HaF64 det, invDet;
		edge1 = V1 - V2;
		edge2 = V2 - V0;
		Vec3<hacd::HaF64> pvec = dir ^ edge2;
		det = edge1 * pvec;
		if (det == 0.0)
			return 0;
		invDet = 1.0/det;
		Vec3<hacd::HaF64> tvec = P0 - V0;
		Vec3<hacd::HaF64> qvec = tvec ^ edge1;
		t = (edge2 * qvec) * invDet;
        if (t < 0.0)
        {
            return 0;
        }
		edge3 = V0 - V1;
		Vec3<hacd::HaF64> I(P0 + t * dir);
		Vec3<hacd::HaF64> s0 = (I-V0) ^ edge3;
		Vec3<hacd::HaF64> s1 = (I-V1) ^ edge1;
		Vec3<hacd::HaF64> s2 = (I-V2) ^ edge2;
		if (s0*s1 >= -1e-9 && s2*s1 >= -1e-9 )
		{
			return 1;
		}
		return 0;
	}

    bool IntersectLineLine(const Vec3<hacd::HaF64> & p1, const Vec3<hacd::HaF64> & p2, 
                          const Vec3<hacd::HaF64> & p3, const Vec3<hacd::HaF64> & p4,
                          Vec3<hacd::HaF64> & pa, Vec3<hacd::HaF64> & pb, 
                          hacd::HaF64 & mua, hacd::HaF64 & mub)
    {
        Vec3<hacd::HaF64> p13,p43,p21;
        hacd::HaF64 d1343,d4321,d1321,d4343,d2121;
        hacd::HaF64 numer,denom;
        
        p13.X() = p1.X() - p3.X();
        p13.Y() = p1.Y() - p3.Y();
        p13.Z() = p1.Z() - p3.Z();
        p43.X() = p4.X() - p3.X();
        p43.Y() = p4.Y() - p3.Y();
        p43.Z() = p4.Z() - p3.Z();
        if (p43.X()==0.0 && p43.Y()==0.0 && p43.Z()==0.0)
            return false;
        p21.X() = p2.X() - p1.X();
        p21.Y() = p2.Y() - p1.Y();
        p21.Z() = p2.Z() - p1.Z();
        if (p21.X()==0.0 && p21.Y()==0.0 && p21.Z()==0.0)
            return false;
        
        d1343 = p13.X() * p43.X() + p13.Y() * p43.Y() + p13.Z() * p43.Z();
        d4321 = p43.X() * p21.X() + p43.Y() * p21.Y() + p43.Z() * p21.Z();
        d1321 = p13.X() * p21.X() + p13.Y() * p21.Y() + p13.Z() * p21.Z();
        d4343 = p43.X() * p43.X() + p43.Y() * p43.Y() + p43.Z() * p43.Z();
        d2121 = p21.X() * p21.X() + p21.Y() * p21.Y() + p21.Z() * p21.Z();
        
        denom = d2121 * d4343 - d4321 * d4321;
        if (denom==0.0)
            return false;
        numer = d1343 * d4321 - d1321 * d4343;
        
        mua = numer / denom;
        mub = (d1343 + d4321 * (mua)) / d4343;
        
        pa.X() = p1.X() + mua * p21.X();
        pa.Y() = p1.Y() + mua * p21.Y();
        pa.Z() = p1.Z() + mua * p21.Z();
        pb.X() = p3.X() + mub * p43.X();
        pb.Y() = p3.Y() + mub * p43.Y();
        pb.Z() = p3.Z() + mub * p43.Z();
        
        return true;
    }

	hacd::HaI32  IntersectRayTriangle2(const Vec3<hacd::HaF64> & P0, const Vec3<hacd::HaF64> & dir, 
							   const Vec3<hacd::HaF64> & V0, const Vec3<hacd::HaF64> & V1, 
							   const Vec3<hacd::HaF64> & V2, hacd::HaF64 &r)
	{
		Vec3<hacd::HaF64> u, v, n;          // triangle vectors
		Vec3<hacd::HaF64> w0, w;          // ray vectors
		hacd::HaF64     a, b;             // params to calc ray-plane intersect

		// get triangle edge vectors and plane normal
		u = V1 - V0;
		v = V2 - V0;
		n = u ^ v;             // cross product
		if (n.GetNorm() == 0.0)            // triangle is degenerate
			return -1;                 // do not deal with this case

		w0 = P0 - V0;
		a = - n * w0;
		b =   n * dir;
		if (fabs(b) <= 0.0) {     // ray is parallel to triangle plane
			if (a == 0.0)                // ray lies in triangle plane
				return 2;
			else return 0;             // ray disjoint from plane
		}

		// get intersect point of ray with triangle plane
		r = a / b;
		if (r < 0.0)                   // ray goes away from triangle
			return 0;                  // => no intersect
		// for a segment, also test if (r > 1.0) => no intersect

		Vec3<hacd::HaF64> I = P0 + r * dir;           // intersect point of ray and plane

		// is I inside T?
		hacd::HaF64 uu, uv, vv, wu, wv, D;
		uu = u * u;
		uv = u * v;
		vv = v * v;
		w  = I - V0;
		wu = w * u;
		wv = w * v;
		D = uv * uv - uu * vv;

		// get and test parametric coords
		hacd::HaF64 s, t;
		s = (uv * wv - vv * wu) / D;
		if (s < 0.0 || s > 1.0)        // I is outside T
			return 0;
		t = (uv * wu - uu * wv) / D;
		if (t < 0.0 || (s + t) > 1.0)  // I is outside T
			return 0;
		return 1;                      // I is in T
	}
    
    
	bool TMMesh::CheckConsistancy()
    {
        hacd::HaU32 nE = m_edges.GetSize();
        hacd::HaU32 nT = m_triangles.GetSize();
        for(hacd::HaU32 e = 0; e < nE; e++)
        {
            for(int f = 0; f < 2; f++)
            {
                if (!m_edges.GetHead()->GetData().m_triangles[f])
                {
                    return false;
                }
            }            
			m_edges.Next();
        }        

        for(hacd::HaU32 f = 0; f < nT; f++)
        {
            for(int e = 0; e < 3; e++)
            {
                int found = 0;
                for(int k = 0; k < 2; k++)
                {
                    if (m_triangles.GetHead()->GetData().m_edges[e]->GetData().m_triangles[k] == m_triangles.GetHead())
                    {
                        found++;
                    }
                }
                if (found != 1)
                {
                    return false;
                }
            }            
			m_triangles.Next();
        }

		return true;
    }
	bool TMMesh::Normalize()
    {
        hacd::HaU32 nV = m_vertices.GetSize();
		if (nV == 0)
		{
			return false;
		}
        m_barycenter = m_vertices.GetHead()->GetData().m_pos;
		Vec3<hacd::HaF64> min = m_barycenter;
		Vec3<hacd::HaF64> max = m_barycenter;
		hacd::HaF64 x, y, z;
        for(hacd::HaU32 v = 1; v < nV; v++)
        {
			m_barycenter +=  m_vertices.GetHead()->GetData().m_pos;
            x = m_vertices.GetHead()->GetData().m_pos.X();
            y = m_vertices.GetHead()->GetData().m_pos.Y();
            z = m_vertices.GetHead()->GetData().m_pos.Z();
            if ( x < min.X()) min.X() = x;
			else if ( x > max.X()) max.X() = x;
            if ( y < min.Y()) min.Y() = y;
			else if ( y > max.Y()) max.Y() = y;
            if ( z < min.Z()) min.Z() = z;
			else if ( z > max.Z()) max.Z() = z;
			m_vertices.Next();
        }
		m_barycenter /= (hacd::HaF64)nV;
        m_diag = 0.001 * (max-min).GetNorm();
        hacd::HaF64 invDiag = 1.0 / m_diag;
		if (m_diag != 0.0)
		{
	        for(hacd::HaU32 v = 0; v < nV; v++)
		    {
				m_vertices.GetHead()->GetData().m_pos = (m_vertices.GetHead()->GetData().m_pos - m_barycenter) * invDiag;
				m_vertices.Next();
		    }
		}
		return true;
	}
	bool TMMesh::Denormalize()
	{
        hacd::HaU32 nV = m_vertices.GetSize();
		if (nV == 0)
		{
			return false;
		}
		if (m_diag != 0.0)
		{
	        for(hacd::HaU32 v = 0; v < nV; v++)
		    {
				m_vertices.GetHead()->GetData().m_pos = m_vertices.GetHead()->GetData().m_pos * m_diag + m_barycenter;
				m_vertices.Next();
			}
		}
		return false;
    }
}