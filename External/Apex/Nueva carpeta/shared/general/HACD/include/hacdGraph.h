/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#ifndef HACD_GRAPH_H
#define HACD_GRAPH_H
#include <hacdVersion.h>
#include <hacdVector.h>
#include <hacdICHull.h>
#include <hacdContainer.h>

namespace HACD
{
    class GraphVertex;
    class GraphEdge;
    class Graph;
	class HACD;
       
    class Graph  
    {
    public:
		hacd::HaU32									GetNEdges() const { return m_nE;}
		hacd::HaU32									GetNVertices() const { return m_nV;}
        bool                                    EdgeCollapse(hacd::HaI32 v1, hacd::HaI32 v2);
        hacd::HaI32                                    AddVertex();
        hacd::HaI32                                    AddEdge(hacd::HaI32 v1, hacd::HaI32 v2);
        bool                                    DeleteEdge(hacd::HaI32 name);	
        bool                                    DeleteVertex(hacd::HaI32 name);
        hacd::HaI32                                    GetEdgeID(hacd::HaI32 v1, hacd::HaI32 v2) const;
        hacd::HaI32                                    ExtractCCs();
        void                                    ConnectCCs();
		void									Clear();
        
                                                Graph();
        virtual                                 ~Graph();      
		void									Allocate(hacd::HaU32 nV, hacd::HaU32 nE);

    private:
        hacd::HaU32                                  m_nV;
        hacd::HaU32                                  m_nE;
        hacd::HaU32                                  m_nCCs;
        GraphEdgeVector                  m_edges;
        GraphVertexVector                m_vertices;

		friend class HACD;
    };
}
#endif