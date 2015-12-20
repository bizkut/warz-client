/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#ifndef RAPIDXML_ITERATORS_HPP_INCLUDED
#define RAPIDXML_ITERATORS_HPP_INCLUDED

// Revision $DateTime:$
//! \file rapidxml_iterators.hpp This file contains rapidxml iterators

#include "rapidxml.hpp"

namespace rapidxml
{

    //! Iterator of child nodes of xml_node
    template<class Ch>
    class node_iterator
    {
    
    public:

        typedef typename xml_node<Ch> value_type;
        typedef typename xml_node<Ch> &reference;
        typedef typename xml_node<Ch> *pointer;
        typedef std::ptrdiff_t difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        
        node_iterator()
            : m_node(0)
        {
        }

        node_iterator(xml_node<Ch> *node)
            : m_node(node->first_node())
        {
        }
        
        reference operator *() const
        {
            assert(m_node);
            return *m_node;
        }

        pointer operator->() const
        {
            assert(m_node);
            return m_node;
        }

        node_iterator& operator++()
        {
            assert(m_node);
            m_node = m_node->next_sibling();
            return *this;
        }

        node_iterator operator++(int)
        {
            node_iterator tmp = *this;
            ++this;
            return tmp;
        }

        node_iterator& operator--()
        {
            assert(m_node && m_node->previous_sibling());
            m_node = m_node->previous_sibling();
            return *this;
        }

        node_iterator operator--(int)
        {
            node_iterator tmp = *this;
            ++this;
            return tmp;
        }

        bool operator ==(const node_iterator<Ch> &rhs)
        {
            return m_node == rhs.m_node;
        }

        bool operator !=(const node_iterator<Ch> &rhs)
        {
            return m_node != rhs.m_node;
        }

    private:

        xml_node<Ch> *m_node;

    };

    //! Iterator of child attributes of xml_node
    template<class Ch>
    class attribute_iterator
    {
    
    public:

        typedef typename xml_attribute<Ch> value_type;
        typedef typename xml_attribute<Ch> &reference;
        typedef typename xml_attribute<Ch> *pointer;
        typedef std::ptrdiff_t difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        
        attribute_iterator()
            : m_attribute(0)
        {
        }

        attribute_iterator(xml_node<Ch> *node)
            : m_attribute(node->first_attribute())
        {
        }
        
        reference operator *() const
        {
            assert(m_attribute);
            return *m_attribute;
        }

        pointer operator->() const
        {
            assert(m_attribute);
            return m_attribute;
        }

        attribute_iterator& operator++()
        {
            assert(m_attribute);
            m_attribute = m_attribute->next_attribute();
            return *this;
        }

        attribute_iterator operator++(int)
        {
            attribute_iterator tmp = *this;
            ++this;
            return tmp;
        }

        attribute_iterator& operator--()
        {
            assert(m_attribute && m_attribute->previous_attribute());
            m_attribute = m_attribute->previous_attribute();
            return *this;
        }

        attribute_iterator operator--(int)
        {
            attribute_iterator tmp = *this;
            ++this;
            return tmp;
        }

        bool operator ==(const attribute_iterator<Ch> &rhs)
        {
            return m_attribute == rhs.m_attribute;
        }

        bool operator !=(const attribute_iterator<Ch> &rhs)
        {
            return m_attribute != rhs.m_attribute;
        }

    private:

        xml_attribute<Ch> *m_attribute;

    };

}

#endif
