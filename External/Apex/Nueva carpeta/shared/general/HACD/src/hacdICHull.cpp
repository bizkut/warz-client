/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <hacdICHull.h>

#pragma warning(disable:4706)

namespace HACD
{   

static hacd::HaF64 max(hacd::HaF64 v1,hacd::HaF64 v2)
{
	return v1 > v2 ? v1 : v2;
}


    const hacd::HaI32 ICHull::sc_dummyIndex = -1;
	ICHull::ICHull(void)
    {
		m_distPoints = 0;
		m_isFlat = false;
		m_dummyVertex = 0;
    }
	bool ICHull::AddPoints(const Vec3<hacd::HaF64> * points, hacd::HaU32 nPoints)
	{
		if (!points)
		{
			return false;
		}
		CircularListElement<TMMVertex> * vertex = NULL;
		for (hacd::HaU32 i = 0; i < nPoints; i++)
		{
			vertex = m_mesh.AddVertex();
			vertex->GetData().m_pos = points[i];
            vertex->GetData().m_name = i;
		}     
		return true;
	}
	bool ICHull::AddPoints(Vec3Vector points)
	{
		CircularListElement<TMMVertex> * vertex = NULL;
		for (hacd::HaI32 i = 0; i < (hacd::HaI32)points.size(); i++)
		{
			vertex = m_mesh.AddVertex();
			vertex->GetData().m_pos = points[i];
		}     
		return true;
	}

    bool ICHull::AddPoint(const Vec3<hacd::HaF64> & point, hacd::HaI32 id)
	{
		if (AddPoints(&point, 1))
		{
			m_mesh.m_vertices.GetData().m_name = id;
			return true;
		}
		return false;
	}

	ICHullError ICHull::Process()
	{
        hacd::HaU32 addedPoints = 0;	
		if (m_mesh.GetNVertices() < 3)
		{
			return ICHullErrorNotEnoughPoints;
		}
		if (m_mesh.GetNVertices() == 3)
		{
			m_isFlat = true;
			CircularListElement<TMMTriangle> * t1 = m_mesh.AddTriangle();
			CircularListElement<TMMTriangle> * t2 = m_mesh.AddTriangle();
			CircularListElement<TMMVertex> * v0 = m_mesh.m_vertices.GetHead();
	        CircularListElement<TMMVertex> * v1 = v0->GetNext();
	        CircularListElement<TMMVertex> * v2 = v1->GetNext();
			// Compute the normal to the plane
            Vec3<hacd::HaF64> p0 = v0->GetData().m_pos;
            Vec3<hacd::HaF64> p1 = v1->GetData().m_pos;            
            Vec3<hacd::HaF64> p2 = v2->GetData().m_pos;			
            m_normal = (p1-p0) ^ (p2-p0);
            m_normal.Normalize();
			t1->GetData().m_vertices[0] = v0;
			t1->GetData().m_vertices[1] = v1;
			t1->GetData().m_vertices[2] = v2;
			t2->GetData().m_vertices[0] = v1;
			t2->GetData().m_vertices[1] = v2;
			t2->GetData().m_vertices[2] = v2;
			return ICHullErrorOK;
		}
		if (m_isFlat)
		{
            m_mesh.m_edges.Clear();
			m_mesh.m_triangles.Clear();
			m_isFlat = false;
		}
        if (m_mesh.GetNTriangles() == 0) // we have to create the first polyhedron
        {
			ICHullError res = DoubleTriangle();
            if (res != ICHullErrorOK)
            {
                return res;
            }
            else
            {
                addedPoints += 3;
            }
        }
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        // go to the first added and not processed vertex
        while (!(vertices.GetHead()->GetPrev()->GetData().m_tag))
        {
            vertices.Prev();
        }
        while (!vertices.GetData().m_tag) // not processed
        {
            vertices.GetData().m_tag = true;
            if (ProcessPoint())
            {
                addedPoints++;
                CleanUp(addedPoints);
	            vertices.Next();
				if (!GetMesh().CheckConsistancy())
				{
					return ICHullErrorInconsistent;
				}
            }
        }
		if (m_isFlat)
		{
			CircularListElementTMMTriangleVector trianglesToDuplicate;
			hacd::HaU32 nT = m_mesh.GetNTriangles();
			for(hacd::HaU32 f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
				if( currentTriangle.m_vertices[0]->GetData().m_name == sc_dummyIndex ||
                   currentTriangle.m_vertices[1]->GetData().m_name == sc_dummyIndex ||
                   currentTriangle.m_vertices[2]->GetData().m_name == sc_dummyIndex )
				{
					m_trianglesToDelete.push_back(m_mesh.m_triangles.GetHead());
                    for(int k = 0; k < 3; k++)
                    {
                        for(int h = 0; h < 2; h++)
                        {
                            if (currentTriangle.m_edges[k]->GetData().m_triangles[h] == m_mesh.m_triangles.GetHead())
                            {
                                currentTriangle.m_edges[k]->GetData().m_triangles[h] = 0;
                                break;
                            }
                        }
                    }
				}
				else
				{
					trianglesToDuplicate.push_back(m_mesh.m_triangles.GetHead());
				}
				m_mesh.m_triangles.Next();
			}
            hacd::HaU32 nE = m_mesh.GetNEdges();
            for(hacd::HaU32 e = 0; e < nE; e++)
			{
				TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
				if( currentEdge.m_triangles[0] == 0 && currentEdge.m_triangles[1] == 0) 
				{
					m_edgesToDelete.push_back(m_mesh.m_edges.GetHead());
				}
				m_mesh.m_edges.Next();
			}
			m_mesh.m_vertices.Delete(m_dummyVertex);
			m_dummyVertex = 0;
            hacd::HaU32 nV = m_mesh.GetNVertices();
            CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
			for(hacd::HaU32 v = 0; v < nV; ++v)
			{
                vertices.GetData().m_tag = false;
                vertices.Next();
            }
            CleanEdges();
			CleanTriangles();
			CircularListElement<TMMTriangle> * newTriangle;
			for(hacd::HaU32 t = 0; t < (hacd::HaU32)trianglesToDuplicate.size(); t++)
			{
				newTriangle = m_mesh.AddTriangle();
				newTriangle->GetData().m_vertices[0] = trianglesToDuplicate[t]->GetData().m_vertices[1];
				newTriangle->GetData().m_vertices[1] = trianglesToDuplicate[t]->GetData().m_vertices[0];
				newTriangle->GetData().m_vertices[2] = trianglesToDuplicate[t]->GetData().m_vertices[2];
			}
		}
		return ICHullErrorOK;
	}
    ICHullError ICHull::Process(hacd::HaU32 nPointsCH)
	{
        hacd::HaU32 addedPoints = 0;  
        if (nPointsCH < 3 || m_mesh.GetNVertices() < 3)
		{
			return ICHullErrorNotEnoughPoints;
		}
		if (m_mesh.GetNVertices() == 3)
		{
			m_isFlat = true;
			CircularListElement<TMMTriangle> * t1 = m_mesh.AddTriangle();
			CircularListElement<TMMTriangle> * t2 = m_mesh.AddTriangle();
			CircularListElement<TMMVertex> * v0 = m_mesh.m_vertices.GetHead();
	        CircularListElement<TMMVertex> * v1 = v0->GetNext();
	        CircularListElement<TMMVertex> * v2 = v1->GetNext();
			// Compute the normal to the plane
            Vec3<hacd::HaF64> p0 = v0->GetData().m_pos;
            Vec3<hacd::HaF64> p1 = v1->GetData().m_pos;            
            Vec3<hacd::HaF64> p2 = v2->GetData().m_pos;			
            m_normal = (p1-p0) ^ (p2-p0);
            m_normal.Normalize();
			t1->GetData().m_vertices[0] = v0;
			t1->GetData().m_vertices[1] = v1;
			t1->GetData().m_vertices[2] = v2;
			t2->GetData().m_vertices[0] = v1;
			t2->GetData().m_vertices[1] = v0;
			t2->GetData().m_vertices[2] = v2;
			return ICHullErrorOK;
		}

		if (m_isFlat)
		{
			m_mesh.m_triangles.Clear();
            m_mesh.m_edges.Clear();
            m_isFlat = false;
		}
        
        if (m_mesh.GetNTriangles() == 0) // we have to create the first polyhedron
        {
			ICHullError res = DoubleTriangle();
            if (res != ICHullErrorOK)
            {
                return res;
            }
            else
            {
                addedPoints += 3;
            }
        }
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        while (!vertices.GetData().m_tag && addedPoints < nPointsCH) // not processed
        {
            if (!FindMaxVolumePoint())
            {
                break;
            }                  
            vertices.GetData().m_tag = true;                      
            if (ProcessPoint())
            {
                addedPoints++;
                CleanUp(addedPoints);
				if (!GetMesh().CheckConsistancy())
				{
					return ICHullErrorInconsistent;
				}
                vertices.Next();
            }
        }
        // delete remaining points
        while (!vertices.GetData().m_tag)
        {
            vertices.Delete();
        }
		if (m_isFlat)
		{
			CircularListElementTMMTriangleVector trianglesToDuplicate;
			hacd::HaU32 nT = m_mesh.GetNTriangles();
			for(hacd::HaU32 f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
				if( currentTriangle.m_vertices[0]->GetData().m_name == sc_dummyIndex ||
                   currentTriangle.m_vertices[1]->GetData().m_name == sc_dummyIndex ||
                   currentTriangle.m_vertices[2]->GetData().m_name == sc_dummyIndex )
				{
					m_trianglesToDelete.push_back(m_mesh.m_triangles.GetHead());
                    for(int k = 0; k < 3; k++)
                    {
                        for(int h = 0; h < 2; h++)
                        {
                            if (currentTriangle.m_edges[k]->GetData().m_triangles[h] == m_mesh.m_triangles.GetHead())
                            {
                                currentTriangle.m_edges[k]->GetData().m_triangles[h] = 0;
                                break;
                            }
                        }
                    }
				}
				else
				{
					trianglesToDuplicate.push_back(m_mesh.m_triangles.GetHead());
				}
				m_mesh.m_triangles.Next();
			}
            hacd::HaU32 nE = m_mesh.GetNEdges();
            for(hacd::HaU32 e = 0; e < nE; e++)
			{
				TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
				if( currentEdge.m_triangles[0] == 0 && currentEdge.m_triangles[1] == 0) 
				{
					m_edgesToDelete.push_back(m_mesh.m_edges.GetHead());
				}
				m_mesh.m_edges.Next();
			}
			m_mesh.m_vertices.Delete(m_dummyVertex);
			m_dummyVertex = 0;
            hacd::HaU32 nV = m_mesh.GetNVertices();
            CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
			for(hacd::HaU32 v = 0; v < nV; ++v)
			{
                vertices.GetData().m_tag = false;
                vertices.Next();
            }
            CleanEdges();
			CleanTriangles();
			CircularListElement<TMMTriangle> * newTriangle;
			for(hacd::HaU32 t = 0; t < (hacd::HaU32)trianglesToDuplicate.size(); t++)
			{
				newTriangle = m_mesh.AddTriangle();
				newTriangle->GetData().m_vertices[0] = trianglesToDuplicate[t]->GetData().m_vertices[1];
				newTriangle->GetData().m_vertices[1] = trianglesToDuplicate[t]->GetData().m_vertices[0];
				newTriangle->GetData().m_vertices[2] = trianglesToDuplicate[t]->GetData().m_vertices[2];
			}
		}
		return ICHullErrorOK;
	}
    bool ICHull::FindMaxVolumePoint()
	{
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        CircularListElement<TMMVertex> * vMaxVolume = 0;
        CircularListElement<TMMVertex> * vHeadPrev = vertices.GetHead()->GetPrev();
        
        hacd::HaF64 maxVolume = 0.0;
        hacd::HaF64 volume = 0.0;
        
        while (!vertices.GetData().m_tag) // not processed
        {
            if (ComputePointVolume(volume, false))
            {
                if ( maxVolume < volume)
                {
                    maxVolume = volume;
                    vMaxVolume = vertices.GetHead();
                }
                vertices.Next();
            }
        }
        CircularListElement<TMMVertex> * vHead = vHeadPrev->GetNext();
        vertices.GetHead() = vHead;  
        
        if (!vMaxVolume)
        {
            return false;
        }
        
        if (vMaxVolume != vHead)
        {
            Vec3<hacd::HaF64> pos = vHead->GetData().m_pos;
            hacd::HaI32 id = vHead->GetData().m_name;
            vHead->GetData().m_pos = vMaxVolume->GetData().m_pos;
            vHead->GetData().m_name = vMaxVolume->GetData().m_name;
            vMaxVolume->GetData().m_pos = pos;
            vHead->GetData().m_name = id;
        }
        
 
        return true;
    }
	ICHullError ICHull::DoubleTriangle()
	{
        // find three non colinear points
		m_isFlat = false;
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        CircularListElement<TMMVertex> * v0 = vertices.GetHead();
        while( Colinear(v0->GetData().m_pos, 
                        v0->GetNext()->GetData().m_pos, 
                        v0->GetNext()->GetNext()->GetData().m_pos))
        {
            if ( (v0 = v0->GetNext()) == vertices.GetHead())
            {
                return ICHullErrorCoplanarPoints;
            }
        }
        CircularListElement<TMMVertex> * v1 = v0->GetNext();
        CircularListElement<TMMVertex> * v2 = v1->GetNext();
        // mark points as processed
        v0->GetData().m_tag = v1->GetData().m_tag = v2->GetData().m_tag = true;
        
        // create two triangles
        CircularListElement<TMMTriangle> * f0 = MakeFace(v0, v1, v2, 0);
        MakeFace(v2, v1, v0, f0);

        // find a fourth non-coplanar point to form tetrahedron
        CircularListElement<TMMVertex> * v3 = v2->GetNext();
        vertices.GetHead() = v3;

		hacd::HaF64 vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
		while (vol == 0.0 && !v3->GetNext()->GetData().m_tag)
		{
			v3 = v3->GetNext();
			vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
		}			
		if (vol == 0.0)
		{
			// compute the barycenter
			Vec3<hacd::HaF64> bary(0.0,0.0,0.0);
			CircularListElement<TMMVertex> * vBary = v0;
			do
			{
				bary += vBary->GetData().m_pos;
			}
			while ( (vBary = vBary->GetNext()) != v0);
			bary /= (hacd::HaF64)vertices.GetSize();

			// Compute the normal to the plane
            Vec3<hacd::HaF64> p0 = v0->GetData().m_pos;
            Vec3<hacd::HaF64> p1 = v1->GetData().m_pos;            
            Vec3<hacd::HaF64> p2 = v2->GetData().m_pos;			
            m_normal = (p1-p0) ^ (p2-p0);
            m_normal.Normalize();
			// add dummy vertex placed at (bary + normal)
			vertices.GetHead() = v2;
			AddPoint(bary + m_normal, sc_dummyIndex); 
			m_dummyVertex = vertices.GetHead();
			m_isFlat = true;
            v3 = v2->GetNext();
            vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
			return ICHullErrorOK;
		}
		else if (v3 != vertices.GetHead())
		{
			TMMVertex temp;
			temp.m_name = v3->GetData().m_name;
			temp.m_pos = v3->GetData().m_pos;
			v3->GetData().m_name = vertices.GetHead()->GetData().m_name;
			v3->GetData().m_pos = vertices.GetHead()->GetData().m_pos;
			vertices.GetHead()->GetData().m_name = temp.m_name;
			vertices.GetHead()->GetData().m_pos = temp.m_pos;
		}
		return ICHullErrorOK;
	}
    CircularListElement<TMMTriangle> *	ICHull::MakeFace(CircularListElement<TMMVertex> * v0,  
                                                         CircularListElement<TMMVertex> * v1,
                                                         CircularListElement<TMMVertex> * v2,
                                                         CircularListElement<TMMTriangle> * fold)
	{        
        CircularListElement<TMMEdge> * e0;
        CircularListElement<TMMEdge> * e1;
        CircularListElement<TMMEdge> * e2;
        hacd::HaI32 index = 0;
        if (!fold) // if first face to be created
        {
            e0 = m_mesh.AddEdge(); // create the three edges
            e1 = m_mesh.AddEdge();
            e2 = m_mesh.AddEdge();            
        }
        else // otherwise re-use existing edges (in reverse order)
        {
            e0 = fold->GetData().m_edges[2];
            e1 = fold->GetData().m_edges[1];
            e2 = fold->GetData().m_edges[0];
            index = 1;
        }
        e0->GetData().m_vertices[0] = v0; e0->GetData().m_vertices[1] = v1;
        e1->GetData().m_vertices[0] = v1; e1->GetData().m_vertices[1] = v2;
        e2->GetData().m_vertices[0] = v2; e2->GetData().m_vertices[1] = v0;
        // create the new face
        CircularListElement<TMMTriangle> * f = m_mesh.AddTriangle();   
        f->GetData().m_edges[0]    = e0; f->GetData().m_edges[1]    = e1; f->GetData().m_edges[2]    = e2;
        f->GetData().m_vertices[0] = v0; f->GetData().m_vertices[1] = v1; f->GetData().m_vertices[2] = v2;     
        // link edges to face f
        e0->GetData().m_triangles[index] = e1->GetData().m_triangles[index] = e2->GetData().m_triangles[index] = f;
		return f;
	}
	CircularListElement<TMMTriangle> * ICHull::MakeConeFace(CircularListElement<TMMEdge> * e, CircularListElement<TMMVertex> * p)
	{
        // create two new edges if they don't already exist
        CircularListElement<TMMEdge> * newEdges[2];
        for(int i = 0; i < 2; ++i)
        {
            if ( !( newEdges[i] = e->GetData().m_vertices[i]->GetData().m_duplicate ) )  
            { // if the edge doesn't exits add it and mark the vertex as duplicated
                newEdges[i] = m_mesh.AddEdge();
                newEdges[i]->GetData().m_vertices[0] = e->GetData().m_vertices[i];
                newEdges[i]->GetData().m_vertices[1] = p;
                e->GetData().m_vertices[i]->GetData().m_duplicate = newEdges[i];
            }
        }
        // make the new face
        CircularListElement<TMMTriangle> * newFace = m_mesh.AddTriangle();
        newFace->GetData().m_edges[0] = e;
        newFace->GetData().m_edges[1] = newEdges[0];
        newFace->GetData().m_edges[2] = newEdges[1];
        MakeCCW(newFace, e, p);
        for(int i=0; i < 2; ++i)
        {
            for(int j=0; j < 2; ++j)
            {
                if ( ! newEdges[i]->GetData().m_triangles[j] )
                {
                    newEdges[i]->GetData().m_triangles[j] = newFace;
                    break;
                }
            }
        }
		return newFace;
	}
    bool ICHull::ComputePointVolume(hacd::HaF64 &totalVolume, bool markVisibleFaces)
    {
        // mark visible faces
        CircularListElement<TMMTriangle> * fHead = m_mesh.GetTriangles().GetHead();
        CircularListElement<TMMTriangle> * f = fHead;
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        CircularListElement<TMMVertex> * vertex0 = vertices.GetHead();
        bool visible = false;
        Vec3<hacd::HaF64> pos0 = vertex0->GetData().m_pos;
        hacd::HaF64 vol = 0.0;
        totalVolume = 0.0;
		Vec3<hacd::HaF64> ver0, ver1, ver2;
        do 
        {
			ver0 = f->GetData().m_vertices[0]->GetData().m_pos;
			ver1 = f->GetData().m_vertices[1]->GetData().m_pos;
			ver2 = f->GetData().m_vertices[2]->GetData().m_pos;
			vol = Volume(ver0, ver1, ver2, pos0);
			if ( vol < 0.0 )
			{
				vol = fabs(vol);
				totalVolume += vol;
				if (markVisibleFaces)
				{
					f->GetData().m_visible = true;
					m_trianglesToDelete.push_back(f);
				}
				visible = true;
			}
			f = f->GetNext();
        } 
        while (f != fHead);

		if (m_trianglesToDelete.size() == m_mesh.m_triangles.GetSize())
		{
			for(hacd::HaU32 i = 0; i < (hacd::HaU32)m_trianglesToDelete.size(); i++)
			{
				m_trianglesToDelete[i]->GetData().m_visible = false;
			}
			visible = false;
		}
        // if no faces visible from p then p is inside the hull
        if (!visible && markVisibleFaces)
        {
            vertices.Delete();
			m_trianglesToDelete.clear();
            return false;
        }
        return true;
    }
	bool ICHull::ProcessPoint()
	{
        hacd::HaF64 totalVolume = 0.0;
        if (!ComputePointVolume(totalVolume, true))
        {
            return false;
        }
        // Mark edges in interior of visible region for deletion.
        // Create a new face based on each border edge
        CircularListElement<TMMVertex> * v0 = m_mesh.GetVertices().GetHead();
        CircularListElement<TMMEdge> * eHead = m_mesh.GetEdges().GetHead();
        CircularListElement<TMMEdge> * e = eHead;    
        CircularListElement<TMMEdge> * tmp = 0;
        hacd::HaI32 nvisible = 0;
        m_edgesToDelete.clear();
        m_edgesToUpdate.clear();
        do 
        {
            tmp = e->GetNext();
            nvisible = 0;
            for(int k = 0; k < 2; k++)
            {
                if ( e->GetData().m_triangles[k]->GetData().m_visible )
                {
                    nvisible++;
                }
            }
            if ( nvisible == 2)
            {
                m_edgesToDelete.push_back(e);
            }
            else if ( nvisible == 1)
            {
                e->GetData().m_newFace = MakeConeFace(e, v0);
                m_edgesToUpdate.push_back(e);
            }
            e = tmp;
        }
        while (e != eHead);        
		return true;
	}
    bool ICHull::MakeCCW(CircularListElement<TMMTriangle> * f,
                         CircularListElement<TMMEdge> * e, 
                         CircularListElement<TMMVertex> * v)
    {
        // the visible face adjacent to e
        CircularListElement<TMMTriangle> * fv; 
        if (e->GetData().m_triangles[0]->GetData().m_visible)
        {
            fv = e->GetData().m_triangles[0];
        }
        else
        {
            fv = e->GetData().m_triangles[1];
        }
        
        //  set vertex[0] and vertex[1] to have the same orientation as the corresponding vertices of fv.
        hacd::HaI32 i;                                 // index of e->m_vertices[0] in fv
        CircularListElement<TMMVertex> * v0 = e->GetData().m_vertices[0];
        CircularListElement<TMMVertex> * v1 = e->GetData().m_vertices[1];
        for(i = 0; fv->GetData().m_vertices[i] !=  v0; i++);
        
        if ( fv->GetData().m_vertices[(i+1) % 3] != e->GetData().m_vertices[1] )
        {
            f->GetData().m_vertices[0] = v1;
            f->GetData().m_vertices[1] = v0;
        }
        else
        {
            f->GetData().m_vertices[0] = v0;
            f->GetData().m_vertices[1] = v1;  
            // swap edges
            CircularListElement<TMMEdge> * tmp = f->GetData().m_edges[0];
            f->GetData().m_edges[0] = f->GetData().m_edges[1];
            f->GetData().m_edges[1] = tmp;
        }
        f->GetData().m_vertices[2] = v;
        return true;
    }
    bool ICHull::CleanUp(hacd::HaU32 & addedPoints)
    {
        bool r0 = CleanEdges();
        bool r1 = CleanTriangles();
        bool r2 = CleanVertices(addedPoints);
        return  r0 && r1 && r2;
    }
    bool ICHull::CleanEdges()
    {
        // integrate the new faces into the data structure
        CircularListElement<TMMEdge> * e;
        const CircularListElementTMMEdgeVector::iterator itEndUpdate = m_edgesToUpdate.end();
        for(CircularListElementTMMEdgeVector::iterator it = m_edgesToUpdate.begin(); it != itEndUpdate; ++it)
        {
            e = *it;
            if ( e->GetData().m_newFace )
            {
                if ( e->GetData().m_triangles[0]->GetData().m_visible)
                {
                    e->GetData().m_triangles[0] = e->GetData().m_newFace;
                }
                else
                {
                    e->GetData().m_triangles[1] = e->GetData().m_newFace;
                }
                e->GetData().m_newFace = 0;
            }           
        }
        // delete edges maked for deletion
        CircularList<TMMEdge> & edges = m_mesh.GetEdges();
        const CircularListElementTMMEdgeVector::iterator itEndDelete = m_edgesToDelete.end();
        for(CircularListElementTMMEdgeVector::iterator it = m_edgesToDelete.begin(); it != itEndDelete; ++it)
        {
            edges.Delete(*it);         
        }
		m_edgesToDelete.clear();
        m_edgesToUpdate.clear();
        return true;
    }
    bool ICHull::CleanTriangles()
    {
        CircularList<TMMTriangle> & triangles = m_mesh.GetTriangles();
        const CircularListElementTMMTriangleVector::iterator itEndDelete = m_trianglesToDelete.end();
        for(CircularListElementTMMTriangleVector::iterator it = m_trianglesToDelete.begin(); it != itEndDelete; ++it)
        {
			if (m_distPoints)
			{
				if (m_isFlat)
				{
					// to be updated
				}
				else
				{
					HaI32Set::const_iterator itPEnd((*it)->GetData().m_incidentPoints.end());
					HaI32Set::const_iterator itP((*it)->GetData().m_incidentPoints.begin());
					DPointMap::iterator itPoint;
					for(; itP != itPEnd; ++itP) 
					{
						itPoint = m_distPoints->find(*itP);
						if (itPoint != m_distPoints->end())
						{
							itPoint->second.m_computed = false;
						}
					}
				}
			}
            triangles.Delete(*it);         
        }
		m_trianglesToDelete.clear();
        return true;
    }
    bool ICHull::CleanVertices(hacd::HaU32 & addedPoints)
    {
        // mark all vertices incident to some undeleted edge as on the hull
        CircularList<TMMEdge> & edges = m_mesh.GetEdges();
        CircularListElement<TMMEdge> * e = edges.GetHead();
        hacd::HaU32 nE = edges.GetSize();
        for(hacd::HaU32 i = 0; i < nE; i++)
        {
            e->GetData().m_vertices[0]->GetData().m_onHull = true;
            e->GetData().m_vertices[1]->GetData().m_onHull = true;
            e = e->GetNext();
        }
        // delete all the vertices that have been processed but are not on the hull
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        CircularListElement<TMMVertex> * vHead = vertices.GetHead();
        CircularListElement<TMMVertex> * v = vHead;
        v = v->GetPrev();
        do 
        {
            if (v->GetData().m_tag && !v->GetData().m_onHull)
            {
                CircularListElement<TMMVertex> * tmp = v->GetPrev();
                vertices.Delete(v);
                v = tmp;
                addedPoints--;
            }
            else
            {
                v->GetData().m_duplicate = 0;
                v->GetData().m_onHull = false;
                v = v->GetPrev();
            }
        } 
        while (v->GetData().m_tag && v != vHead);
        return true;
    }
	void ICHull::Clear()
	{	
		m_mesh.Clear();
		m_edgesToDelete = CircularListElementTMMEdgeVector();
		m_edgesToUpdate = CircularListElementTMMEdgeVector();
		m_trianglesToDelete= CircularListElementTMMTriangleVector();
		m_isFlat = false;
	}
    const ICHull & ICHull::operator=(ICHull & rhs)
    {
        if (&rhs != this)
        {
            m_mesh.Copy(rhs.m_mesh);
            m_edgesToDelete = rhs.m_edgesToDelete;
            m_edgesToUpdate = rhs.m_edgesToUpdate;
            m_trianglesToDelete = rhs.m_trianglesToDelete;
			m_isFlat = rhs.m_isFlat;
        }
        return (*this);
    }   
    hacd::HaF64 ICHull::ComputeVolume()
    {
        hacd::HaU32 nV = m_mesh.m_vertices.GetSize();
		if (nV == 0 || m_isFlat)
		{
			return 0.0;
		}       
        Vec3<hacd::HaF64> bary(0.0, 0.0, 0.0);

        for(hacd::HaU32 v = 0; v < nV; v++)
        {
			bary +=  m_mesh.m_vertices.GetHead()->GetData().m_pos;
			m_mesh.m_vertices.Next();
        }
		bary /= (hacd::HaF64)nV;
        
        hacd::HaU32 nT = m_mesh.m_triangles.GetSize();
        Vec3<hacd::HaF64> ver0, ver1, ver2;
        hacd::HaF64 totalVolume = 0.0;
        for(hacd::HaU32 t = 0; t < nT; t++)
        {
            ver0 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos;
			ver1 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos;
			ver2 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos;
			totalVolume += Volume(ver0, ver1, ver2, bary);
			m_mesh.m_triangles.Next();
        }
        return totalVolume;
    }
    bool ICHull::IsInside(const Vec3<hacd::HaF64> pt)
    {
		if (m_isFlat)
		{
			hacd::HaU32 nT = m_mesh.m_triangles.GetSize();
			Vec3<hacd::HaF64> ver0, ver1, ver2, a, b, c;
			hacd::HaF64 u,v;
			for(hacd::HaU32 t = 0; t < nT; t++)
			{
				ver0 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos;
				ver1 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos;
				ver2 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos;
				a = ver1 - ver0;
				b = ver2 - ver0;
				c = pt - ver0;
				u = c * a;
				v = c * b;
				if ( u >= 0.0 && u <= 1.0 && v >= 0.0 && u+v <= 1.0)
				{
					return true;
				}
				m_mesh.m_triangles.Next();
			}
			return false;
		}
		else
		{
			hacd::HaU32 nT = m_mesh.m_triangles.GetSize();
			Vec3<hacd::HaF64> ver0, ver1, ver2;
			for(hacd::HaU32 t = 0; t < nT; t++)
			{
				ver0 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos;
				ver1 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos;
				ver2 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos;
				if (Volume(ver0, ver1, ver2, pt) < 0.0)
				{
					return false;
				}
				m_mesh.m_triangles.Next();
			}
			return true;
		}
    }
	hacd::HaF64 ICHull::ComputeDistance(hacd::HaI32 name, const Vec3<hacd::HaF64> & pt, const Vec3<hacd::HaF64> & normal, bool & insideHull, bool updateIncidentPoints)
	{
		if (m_isFlat)
		{
			hacd::HaF64 distance = 0.0;
			Vec3<hacd::HaF64> ptNormal = normal;
			ptNormal -= (ptNormal * m_normal) * m_normal;
			if (ptNormal.GetNorm() > 0.0)
			{
				ptNormal.Normalize();
				hacd::HaI32 nameVE1;
				hacd::HaI32 nameVE2;
				Vec3<hacd::HaF64> pa, pb, d0, d1, d2, d3;
				Vec3<hacd::HaF64> p0 = pt;
				Vec3<hacd::HaF64> p1 = p0 + ptNormal;
				Vec3<hacd::HaF64> p2, p3;
				hacd::HaF64 mua, mub, s;
				const hacd::HaF64 EPS = 0.00000000001;
				hacd::HaU32 nE = m_mesh.GetNEdges();
				for(hacd::HaU32 e = 0; e < nE; e++)
				{
					TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
                    nameVE1 = currentEdge.m_vertices[0]->GetData().m_name;
                    nameVE2 = currentEdge.m_vertices[1]->GetData().m_name;
                    if (currentEdge.m_triangles[0] == 0 || currentEdge.m_triangles[1] == 0)
                    {
                        if ( nameVE1==name || nameVE2==name )
                        {
                            return 0.0;
                        }
                        /*
                        if (debug) std::cout << "V" << name 
                                             << " E "  << nameVE1 << " " << nameVE2 << std::endl;
                         */

                        p2 = currentEdge.m_vertices[0]->GetData().m_pos;
                        p3 = currentEdge.m_vertices[1]->GetData().m_pos;
                        d0 = p3 - p2;
                        if (d0.GetNorm() > 0.0)
                        {
                            if (IntersectLineLine(p0, p1, p2, p3, pa, pb, mua, mub))
                            {
                                d1 = pa - p2;
                                d2 = pa - pb;
                                d3 = pa - p0;
                                mua = d1.GetNorm()/d0.GetNorm();
                                mub = d1*d0;
                                s = d3*ptNormal;
                                if (d2.GetNorm() < EPS &&  mua <= 1.0 && mub>=0.0 && s>0.0)
                                {
                                    distance = max(distance, d3.GetNorm());
                                }
                            }
                        }
                    }
					m_mesh.m_edges.Next();
				}
			}
			return distance;
		}
		else
		{
			Vec3<hacd::HaF64> impact;
			hacd::HaI32 nhit;
			hacd::HaF64 dist;
			hacd::HaF64 distance = 0.0; 
			hacd::HaU32 nT = m_mesh.GetNTriangles();
			insideHull = false;
			CircularListElement<TMMTriangle> * face = 0;
			for(hacd::HaU32 f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
	/*
				if (debug) std::cout << "T " << currentTriangle.m_vertices[0]->GetData().m_name << " "
														  << currentTriangle.m_vertices[1]->GetData().m_name << " "
														  << currentTriangle.m_vertices[2]->GetData().m_name << std::endl;
     */
				if (currentTriangle.m_vertices[0]->GetData().m_name == name ||
					currentTriangle.m_vertices[1]->GetData().m_name == name ||
					currentTriangle.m_vertices[2]->GetData().m_name == name)
				{
					nhit = 1;
					dist = 0.0;
				}
				else
				{
					nhit = IntersectRayTriangle(pt, normal, 
										currentTriangle.m_vertices[0]->GetData().m_pos, 
										currentTriangle.m_vertices[1]->GetData().m_pos, 
										currentTriangle.m_vertices[2]->GetData().m_pos, dist);
				}

				if (nhit == 1 && distance <= dist)
				{
					distance = dist;
					insideHull = true;
					face = m_mesh.m_triangles.GetHead();	
/*
					std::cout << name << " -> T " << currentTriangle.m_vertices[0]->GetData().m_name << " "
												  << currentTriangle.m_vertices[1]->GetData().m_name << " "
												  << currentTriangle.m_vertices[2]->GetData().m_name << " Dist "
												  << dist << " P " << currentTriangle.m_normal * normal << std::endl;
*/
					if (dist > 0.1)
					{
						break;
					}
				}
				m_mesh.m_triangles.Next();
			}
			if (updateIncidentPoints && face && m_distPoints)
			{
				(*m_distPoints)[name].m_dist = distance;
				face->GetData().m_incidentPoints.insert(name);
			}
			return distance;
		}
	}
}

