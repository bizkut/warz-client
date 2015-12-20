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
#ifndef RAPIDXML_UTILS_HPP_INCLUDED
#define RAPIDXML_UTILS_HPP_INCLUDED

// Revision $DateTime: 2007/08/08 00:56:15 $
//! \file rapidxml_utils.hpp This file contains high-level rapidxml utilities that can be useful
//! in certain simple scenarios. They should probably not be used if maximizing performance is the main objective.

#include "rapidxml.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

namespace rapidxml
{

    //! Represents data loaded from a file
    template<class Ch = char>
    class file
    {
        
    public:
        
        //! Loads file into the memory. Data will be automatically destroyed by the destructor.
        //! \param filename Filename to load.
        file(const char *filename)
        {
            using namespace std;

            // Open stream
            basic_ifstream<Ch> stream(filename);
            if (!stream)
                throw runtime_error(string("cannot open file ") + filename);
            stream.unsetf(ios::skipws);
            
            // Determine stream size
            stream.seekg(0, ios::end);
            size_t size = stream.tellg();
            stream.seekg(0);   
            
            // Load data and add terminating 0
            m_data.resize(size + 1);
            stream.read(&m_data.front(), static_cast<streamsize>(size));
            m_data[size] = 0;
        }

        //! Loads file into the memory. Data will be automatically destroyed by the destructor
        //! \param stream Stream to load from
        file(std::basic_istream<Ch> &stream)
        {
            using namespace std;

            // Load data and add terminating 0
            stream.unsetf(ios::skipws);
            m_data.assign(istreambuf_iterator<Ch>(stream), istreambuf_iterator<Ch>());
            if (stream.fail() || stream.bad())
                throw runtime_error("error reading stream");
            m_data.push_back(0);
        }
        
        //! Gets file data.
        //! \return Pointer to data of file.
        Ch *data()
        {
            return &m_data.front();
        }

        //! Gets file data.
        //! \return Pointer to data of file.
        const Ch *data() const
        {
            return &m_data.front();
        }

        //! Gets file data size.
        //! \return Size of file data, in characters.
        std::size_t size() const
        {
            return m_data.size();
        }

    private:

        std::vector<Ch> m_data;   // File data

    };

    //! Counts children of node. Time complexity is O(n).
    //! \return Number of children of node
    template<class Ch>
    inline std::size_t count_children(xml_node<Ch> *node)
    {
        xml_node<Ch> *child = node->first_node();
        std::size_t count = 0;
        while (child)
        {
            ++count;
            child = child->next_sibling();
        }
        return count;
    }

    //! Counts attributes of node. Time complexity is O(n).
    //! \return Number of attributes of node
    template<class Ch>
    inline std::size_t count_attributes(xml_node<Ch> *node)
    {
        xml_attribute<Ch> *attr = node->first_attribute();
        std::size_t count = 0;
        while (attr)
        {
            ++count;
            attr = attr->next_attribute();
        }
        return count;
    }

}

#endif
