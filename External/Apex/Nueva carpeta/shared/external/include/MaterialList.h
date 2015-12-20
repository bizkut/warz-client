// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.

#ifndef MATERIAL_LIST_H
#define MATERIAL_LIST_H

#include <map>
#include <string>
#include <vector>

namespace Samples
{

class MaterialList
{
public:
	MaterialList();
	~MaterialList();

	void clear();
	void addPath(const char* path);

	struct MaterialInfo
	{
		MaterialInfo();
		bool isLit;
		bool vshaderStatic;
		bool vshader1bone;
		bool vshader4bones;
		unsigned int fromPath;

		std::string diffuseTexture;
		std::string normalTexture;
	};

	struct TextureInfo
	{
		TextureInfo();
		unsigned int fromPath;
	};

	const MaterialInfo* containsMaterial(const char* materialName) const;
	const char* findClosest(const char* materialName) const;

	const TextureInfo* containsTexture(const char* textureName) const;

	void getFirstMaterial(std::string& name, MaterialInfo& info);
	bool getNextMaterial(std::string& name, MaterialInfo& info);

private:
	unsigned int addMaterial(const char* directory, const char* prefix, const char* materialName);
	unsigned int addTexture(const char* directory, const char* prefix, const char* textureName);

	std::vector<std::string> mPaths;

	typedef std::map<std::string, MaterialInfo> tMaterialNames;
	tMaterialNames mMaterialNames;
	tMaterialNames::const_iterator mMaterialIterator;

	typedef std::map<std::string, TextureInfo> tTextureNames;
	tTextureNames mTextureNames;
};

} // namespace Samples


#endif // MATERIAL_LIST_H
