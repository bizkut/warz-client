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

#ifndef FAST_XML_H
#define FAST_XML_H

#include "PsShare.h"
#include "foundation/PxSimpleTypes.h"	 // defines basic data types; modify for your platform as needed.
#include "PxFileBuf.h"		// defines the basic file stream interface.

namespace FAST_XML
{

class FastXml
{
public:
	/***
	* Callbacks to the user with the contents of the XML file properly digested.
	*/
	class Callback
	{
	public:

		virtual bool processComment(const char *comment) = 0; // encountered a comment in the XML

		// 'element' is the name of the element that is being closed.
		// depth is the recursion depth of this element.
		// Return true to continue processing the XML file.
		// Return false to stop processing the XML file; leaves the read pointer of the stream right after this close tag.
		// The bool 'isError' indicates whether processing was stopped due to an error, or intentionally canceled early.
		virtual bool processClose(const char *element,physx::PxU32 depth,bool &isError) = 0;	  // process the 'close' indicator for a previously encountered element

		// return true to continue processing the XML document, false to skip.
		virtual bool processElement(
			const char *elementName,   // name of the element
			physx::PxI32 argc,         // number of attributes pairs
			const char **argv,         // list of attributes.
			const char  *elementData,  // element data, null if none
			physx::PxI32 lineno) = 0;  // line number in the source XML file

		// process the XML declaration header
		virtual bool processXmlDeclaration(
			physx::PxI32 /*argc*/,
			const char ** /*argv*/,
			const char  * /*elementData*/,
			physx::PxI32 /*lineno*/)
		{
			return true;
		}

		virtual bool processDoctype(
			const char * /*rootElement*/, //Root element tag
			const char * /*type*/,        //SYSTEM or PUBLIC
			const char * /*fpi*/,         //Formal Public Identifier
			const char * /*uri*/)         //Path to schema file
		{
			return true;
		}

		virtual void *  fastxml_malloc(physx::PxU32 size) = 0;
		virtual void	fastxml_free(void *mem) = 0;

	};

	virtual bool processXml(physx::PxFileBuf &buff,bool streamFromMemory=false) = 0;

	virtual const char *getError(physx::PxI32 &lineno) = 0; // report the reason for a parsing error, and the line number where it occurred.

	virtual void release(void) = 0;
};

const char *getAttribute(const char *attr, physx::PxI32 argc, const char **argv);

FastXml * createFastXml(FastXml::Callback *iface);

}; // end of namespace FAST_XML

#endif // FAST_XML_H
