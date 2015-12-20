/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "hacdGraph.h"

#pragma warning(disable:4702)

namespace HACD
{    
    
    GraphEdge::GraphEdge()
    {
        m_convexHull = 0;
        m_v1 = -1;
        m_v2 = -1;
		m_name = -1;
        m_error = 0.0;
        m_surf = 0.0;
        m_perimeter = 0.0;
        m_concavity = 0.0;
		m_volume = 0.0;
        m_deleted = false;
	}
   
    GraphVertex::GraphVertex()
    {
        m_convexHull = 0;
		m_name = -1;
        m_cc = -1;        
        m_error = 0.0;
        m_surf = 0.0;
        m_perimeter = 0.0;
        m_concavity = 0.0;
		m_volume = 0.0;
        m_deleted = false;
    }
    
    bool GraphVertex::DeleteEdge(hacd::HaI32 name)
    {
        HaI32Set::iterator it = m_edges.find(name);
        if (it != m_edges.end() )
		{
			m_edges.erase(it);
			return true;
		}
        return false;
    }

    Graph::Graph()
    {
        m_nV = 0;
        m_nE = 0;
        m_nCCs = 0;
    }
    
    Graph::~Graph()
    {
    }
    
	void Graph::Allocate(hacd::HaU32 nV, hacd::HaU32 nE)
	{ 
		m_nV = nV;
		m_edges.reserve(nE);
		m_vertices.resize(nV);
		for(hacd::HaU32 i = 0; i < nV; i++)
		{
			m_vertices[i].m_name = static_cast<hacd::HaI32>(i);
		}
	}

    hacd::HaI32 Graph::AddVertex()
    {
		hacd::HaU32 name = (hacd::HaU32)m_vertices.size();
		m_vertices.resize(name+1);
        m_vertices[name].m_name = name;
        m_nV++;
        return static_cast<hacd::HaI32>(name);
    }
    
    hacd::HaI32 Graph::AddEdge(hacd::HaI32 v1, hacd::HaI32 v2)
    {
		hacd::HaU32 name =(hacd::HaU32) m_edges.size();
		m_edges.push_back(GraphEdge());
        m_edges[name].m_name = name;
        m_edges[name].m_v1 = v1;
        m_edges[name].m_v2 = v2;
        m_vertices[v1].AddEdge(name);
        m_vertices[v2].AddEdge(name);
        m_nE++;
		return static_cast<hacd::HaI32>(name);
    }

    bool Graph::DeleteEdge(hacd::HaI32 name)
    {
		if (name < static_cast<hacd::HaI32>(m_edges.size()))
		{
            hacd::HaI32 v1 = m_edges[name].m_v1;
            hacd::HaI32 v2 = m_edges[name].m_v2;
			m_edges[name].m_deleted = true;
            m_vertices[v1].DeleteEdge(name);
            m_vertices[v2].DeleteEdge(name);
            delete m_edges[name].m_convexHull;
			m_edges[name].m_distPoints.clear();
			m_edges[name].m_boudaryEdges.clear();
            m_edges[name].m_convexHull = 0;
			m_nE--;
			return true;
		}
		return false;
    }
    bool Graph::DeleteVertex(hacd::HaI32 name)
    {
		if (name < static_cast<hacd::HaI32>(m_vertices.size()))
		{
			m_vertices[name].m_deleted = true;
            m_vertices[name].m_edges.clear();
            m_vertices[name].m_ancestors = HaU32Vector();
            delete m_vertices[name].m_convexHull;
			m_vertices[name].m_distPoints.clear();
			m_vertices[name].m_boudaryEdges.clear();
            m_vertices[name].m_convexHull = 0;
			m_nV--;
			return true;
		}
		return false;
    }    
    bool Graph::EdgeCollapse(hacd::HaI32 v1, hacd::HaI32 v2)
	{
		hacd::HaI32 edgeToDelete = GetEdgeID(v1, v2);
        if (edgeToDelete >= 0) 
		{
			// delete the edge (v1, v2)
			DeleteEdge(edgeToDelete);
			// add v2 to v1 ancestors
            m_vertices[v1].m_ancestors.push_back(v2);
			// add v2's ancestors to v1's ancestors
			HaU32Vector newArray;
			HaU32Vector	&oldArray = m_vertices[v1].m_ancestors;
			HaU32Vector	&insertArray = m_vertices[v2].m_ancestors;
			newArray.reserve( insertArray.size() + oldArray.size() );
			for (hacd::HaU32 i=0; i<insertArray.size(); i++)
			{
				newArray.push_back( insertArray[i] );
			}
			for (hacd::HaU32 i=0; i<oldArray.size(); i++)
			{
				newArray.push_back( oldArray[i] );
			}
			m_vertices[v1].m_ancestors = newArray;

			// update adjacency information
			HaI32Set & v1Edges =  m_vertices[v1].m_edges;
			HaI32Set::const_iterator ed(m_vertices[v2].m_edges.begin());
			HaI32Set::const_iterator itEnd(m_vertices[v2].m_edges.end());
			hacd::HaI32 b = -1;
			for(; ed != itEnd; ++ed) 
			{
				if (m_edges[*ed].m_v1 == v2)
				{
					b = m_edges[*ed].m_v2;
				}
				else
				{
					b = m_edges[*ed].m_v1;
				}
				if (GetEdgeID(v1, b) >= 0)
				{
					m_edges[*ed].m_deleted = true;
					m_vertices[b].DeleteEdge(*ed);
					m_nE--;
				}
				else
				{
					m_edges[*ed].m_v1 = v1;
					m_edges[*ed].m_v2 = b;
					v1Edges.insert(*ed);
				}
			}
			// delete the vertex v2
            DeleteVertex(v2);			
            return true;
        }
		return false;
    }
    
    hacd::HaI32 Graph::GetEdgeID(hacd::HaI32 v1, hacd::HaI32 v2) const
    {
		if (v1 < static_cast<hacd::HaI32>(m_vertices.size()) && !m_vertices[v1].m_deleted)
		{
			HaI32Set::const_iterator ed(m_vertices[v1].m_edges.begin());
			HaI32Set::const_iterator itEnd(m_vertices[v1].m_edges.end());
			for(; ed != itEnd; ++ed) 
			{
				if ( (m_edges[*ed].m_v1 == v2) || 
					 (m_edges[*ed].m_v2 == v2)   ) 
				{
					return m_edges[*ed].m_name;
				}
			}
		}
        return -1;
    }
    
    hacd::HaI32 Graph::ExtractCCs()
	{
        // all CCs to -1
        for (hacd::HaU32 v = 0; v < m_vertices.size(); ++v) 
		{
			if (!m_vertices[v].m_deleted)
			{
				m_vertices[v].m_cc = -1;
			}
        }
        
        // we get the CCs
        m_nCCs = 0;
		hacd::HaI32 v2 = -1;
		HaU32Vector temp;
        for (hacd::HaU32 v = 0; v < m_vertices.size(); ++v) 
		{
			if (!m_vertices[v].m_deleted && m_vertices[v].m_cc == -1) 
			{
                m_vertices[v].m_cc = m_nCCs;
                temp.clear();
                temp.push_back(m_vertices[v].m_name);
                while (temp.size()) 
				{
                    hacd::HaI32 vertex = temp[temp.size()-1];
                    temp.pop_back();                    
					HaI32Set::const_iterator ed(m_vertices[vertex].m_edges.begin());
					HaI32Set::const_iterator itEnd(m_vertices[vertex].m_edges.end());
					for(; ed != itEnd; ++ed) 
					{
                        if (m_edges[*ed].m_v1 == vertex) 
						{
                            v2 = m_edges[*ed].m_v2;
                        }
                        else 
						{
                            v2 = m_edges[*ed].m_v1;
                        }
                        if ( !m_vertices[v2].m_deleted && m_vertices[v2].m_cc == -1) 
						{
                            m_vertices[v2].m_cc = m_nCCs;
                            temp.push_back(v2);
                        }
                    }
                }
                m_nCCs++;
            }
        }        
        return m_nCCs;
    }
    void Graph::ConnectCCs()
	{
#if 0
        ExtractCCs();
		HaU32Vector clustersRep;
		clustersRep.resize(m_nCCs,(hacd::HaU32) -1);
		hacd::HaU32 done = 0;
        for (hacd::HaU32 v = 0; (done<m_nCCs) && (v < m_vertices.size()); ++v) 
		{
			if (!m_vertices[v].m_deleted && m_vertices[v].m_cc >=0 && clustersRep[m_vertices[v].m_cc] == (hacd::HaU32) -1)
			{
				clustersRep[m_vertices[v].m_cc] = static_cast<hacd::HaI32>(done);
				done++;
			}
        }
        for (hacd::HaU32 i = 0; i < m_nCCs; i++) 
		{
            for (hacd::HaU32 j = i+1; j < m_nCCs; j++) 
			{
                AddEdge(clustersRep[i], clustersRep[j]);
            }
		}
#endif
    }
	void Graph::Clear()
	{
		m_vertices.clear();
		m_edges.clear();
		m_nV = 0;
        m_nE = 0;
        m_nCCs = 0;
	}

}