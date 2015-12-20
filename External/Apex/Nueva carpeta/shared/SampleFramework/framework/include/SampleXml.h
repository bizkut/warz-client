/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
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

#ifndef _SAMPLE_XML_H
#define	_SAMPLE_XML_H

#include "FastXml.h"
#include "SampleTree.h"

namespace FAST_XML
{

class xml_node : private SampleFramework::Tree::Node
{
public:
	xml_node(const char* name, const char* data, physx::PxI32 argc, const char** argv)
		: mName(name), mData(data), mArgc(argc)
	{
		PX_ASSERT(argc < MAX_ARGS);
		for (physx::PxI32 i = 0; i < argc; ++i)
			mArgv[i] = argv[i];
	}

	~xml_node()
	{
		xml_node* node = first_node();
		while (node != NULL)
		{
			xml_node* child = node;
			node = node->next_sibling();
			delete child;
		}
	}

public:
	xml_node* parent() const { return static_cast<xml_node*>(getParent()); }
	xml_node* first_node() const { return static_cast<xml_node*>(getFirstChild()); }
	xml_node* next_sibling() const { return static_cast<xml_node*>(getNextSibling()); }
	bool append_node(xml_node& node) { return appendChild(node); }

	xml_node* first_node(const char* name) const
	{
		for (xml_node* node = first_node(); node != NULL; node = node->next_sibling())
		{
			if (0 == strcmp(node->name(), name))
			{
				return node;
			}
		}

		return NULL;
	}

public:
	const char* name() const { return mName; }
	const char* value() const { return mData; }
	const char *getXMLAttribute(const char *attr) const
	{
		const char* value = NULL;
		for (physx::PxI32 i = 0; i < mArgc; i += 2)
		{
			if (0 == strcmp(mArgv[i], attr))
			{
				value = mArgv[i + 1];
				break;
			}
		}

		return value;
	}

private:
	static const physx::PxI32 MAX_ARGS = 50;

	const char*		mName;
	const char*		mData;
	physx::PxI32	mArgc;
	const char*		mArgv[MAX_ARGS];
};

class XmlBuilder : public FastXml::Callback
{
public:
	XmlBuilder() : mRoot(NULL), mThis(NULL) {}
	virtual ~XmlBuilder()
	{
		delete mRoot;
	}

public:
	xml_node* rootnode() const { return mRoot; }

	virtual bool processComment(const char *comment)
	{
		return true;
	}

	virtual bool processClose(const char *element, physx::PxU32 depth, bool &isError)
	{
		mThis = mThis->parent();
		return true;
	}

	virtual bool processElement(
			const char *elementName,   // name of the element
			physx::PxI32 argc,         // number of attributes pairs
			const char **argv,         // list of attributes.
			const char  *elementData,  // element data, null if none
			physx::PxI32 lineno)
	{
		xml_node* node = new xml_node(elementName, elementData, argc, argv);

		if (NULL != mThis)
			mThis->append_node(*node);
		else if (NULL == mRoot)
			mRoot = node;
		else
		{
			printf("error: more than 1 root node in xml file\n");
			return false;
		}

		mThis = node;
		return true;
	}

	virtual void*	fastxml_malloc(physx::PxU32 size)
	{
		return malloc(size);
	}

	virtual void	fastxml_free(void *mem)
	{
		free(mem);
	}

private:
	xml_node*	mRoot;
	xml_node*	mThis;
};

}

#endif
