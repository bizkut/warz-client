

/*! \page whats_newin_version_4  What's new in version 4

\ref wn4_01_0 \n
\ref wn4_01_1 \n
\ref wn4_01_2 \n
\ref wn4_01_3 \n
\ref wn4_01_4 \n
\ref wn4_01_5 \n
\ref wn4_01_6 \n
\ref wn4_01_7 \n
\ref wn4_01_8 \n
\ref wn4_01_9 \n
\ref wn4_01_10 \n
\ref wn4_01_11 \n

\section wn4_01_0 Additions to Particle System data

A number of changes have appeared in the particle system data to make the data far more useful. 

\subsection XMD::XParticleShape

The following functions now provide information on the type of particle used \n

\code
XParticleType XMD::XParticleShape::GetParticleType() const; \n
void XMD::XParticleShape::SetParticleType(const XParticleType type); \n
\endcode

The following functions inform you if the particles need to be depth sorted or not \n

\code
bool XMD::XParticleShape::GetDepthSort() const; \n
void XMD::XParticleShape::SetDepthSort(const bool); \n
\endcode

The following functions provide information on the lifespan of the particles \n

\code
XReal XMD::XParticleShape::GetLifeSpan() const; \n
void XMD::XParticleShape::SetLifeSpan(const XReal lifespan); \n

XReal XMD::XParticleShape::GetLifeSpanRandom() const; \n
void XMD::XParticleShape::SetLifeSpanRandom(const XReal lifespan_random); \n

XLifespanType XMD::XParticleShape::GetLifespanType() const; \n
void XMD::XParticleShape::SetLifespanType(const XLifespanType type); \n
\endcode

see the documentation for XMD::XParticleShape for more information. 

\subsection XMD::XEmitter

This class has been improved to provide the additional information:

* the emitter type, omni, directional, volume or emit from geometry. 
* the emitter volume parameters
* colour, size and mass parameters for the emitted particles
* any geometry used to emit particles from

see the documentation for XMD::XEmitter for more details.

\subsection XMD::XField

XField has a set of additional functions which are intended to provide you with methods
to restrict the effect of the field, so that it either effects particles inside or outside
of a simple volume primitive. 

\section wn4_01_1 Additions to XMD::XBone

The main transform node type has been extended to include limit information, this information 
can be accessed via the following functions: \n

\code
bool  XMD::XBone::HasLimit(const Limit limit_type) const; \n
XReal XMD::XBone::GetLimitValue(const Limit limit_type) const; \n
void  XMD::XBone::SetLimit(const Limit limit_type, const XReal limit); \n
void  XMD::XBone::DisableLimit(const Limit limit_type); \n
\endcode

In addition, the XBone now contains an additional flag to determine whether the parent transform
is inherited or not. (i.e. if true, then the world space matrix of the transform will include 
the parent transform. If false, the local space transform will basically be the same as the world 
space). The following functions provide this information: \n

\code
void  XMD::XBone::SetInheritsTransform(const bool val); \n
bool  XMD::XBone::GetInheritsTransform() const; \n
\endcode

The XBone node now also includes information about the original Euler rotation order of the 
transforms. Although XMD stores all rotations as quaternions, this data is useful if you are
importing the data into a 3D package that requires euler trasnsforms.  \n

\code
RotationOrder XMD::XBone::GetRotationOrder() const; \n
void XMD::XBone::SetRotationOrder(const RotationOrder ro); \n
\endcode

\section wn4_01_2 Additions to XMD::XJoint

The XJoint class has been modified to include an additional parameter which determines whether 
the scale of the joint's parent should be compensated when computing it's transform. \n\n

\code
bool XMD::XJoint::GetSegmentScaleCompensate() const; \n
void XMD::XJoint::SetSegmentScaleCompensate(const bool val); \n
\endcode

\section wn4_01_3 Additions to XMD::XDeformer

The XDeformer now includes an additional envelope weight parameter to determine how much effect
the deformer has. a value of zero indicates no effect, a value of 1 indicate full effect. The 
following functions provide access to that data: \n

\code
XReal XMD::XDeformer::GetEnvelopeWeight(); \n
void XMD::XDeformer::SetEnvelopeWeight(const XReal weight); \n
\endcode

\section wn4_01_4 Additions to XMD::XBlendShape

The XBlendShape deformer has been modified to now include the weight values at the time of export, 
and also the input target shape for each blend shape target. The animation curves for the weights 
can now be extracted from the XAnimationTake node using the XBlendShape::kWeights property. With
the old XAnimCycle node, the animation curves could only be extracted from Maya using the extra 
attributes mechanism. The following functions now provide access to the blend shape weights: \n

\code
XReal XBlendShape::GetWeight(const XU32 target_num) const; \n
void  XBlendShape::SetWeight(const XU32 target_num, const XReal weight); \n
\endcode

\section wn4_01_5 Modifications to XMD::XWireDeformer

The XMD::XWireDeformer has now been modified to fix the inability to extract the original base wire used
within the deformer. Previously this information was not available which greatly restricted the 
use of the wire deformer node. The following functions have been added to provide access to the 
base wires: \n\n

\code
XBase* XMD::XWireDeformer::GetBaseWire(const XU32 index) const; \n
void XMD::XWireDeformer::GetBaseWires(XList& wires); \n
\endcode

In addition, the following two functions have been modified to allow you to specify the base wire 
shape when adding wires into the deformer: \n\n

\code
bool XMD::XWireDeformer::AddWire(const XBase* deformed, const XBase* base); \n
bool XMD::XWireDeformer::SetWires(xonst XList& wires,const XList& base_wires); \n
\endcode

\section wn4_01_6 Additions to XMD::XBaseLattice

The XBaseLattice node has been modified to now include additional information about the size
of the base lattice node. Previously this information was provided as the scale of the nodes
transform. This caused a problem with the exporters if the option was set to remove the scale 
from model at export time. The scale would then be set to 1,1,1, and the original size of the 
base lattice would be lost rendering the FFD data largely useless. To remedy this, the following 
functions are now provided to set and get the size: \n

\code
XReal XMD::XBaseLattice::GetXSize() const; \n
XReal XMD::XBaseLattice::GetXSize() const; \n
XReal XMD::XBaseLattice::GetXSize() const; \n

void XMD::XBaseLattice::SetXSize(const XReal x); \n
void XMD::XBaseLattice::SetYSize(const XReal y); \n
void XMD::XBaseLattice::SetZSize(const XReal z); \n
\endcode

Note: If the data has been exported without the remove scale option turned on, then the size
should still be extracted from the parent transforms scale. 

\section wn4_01_7 Additions to XLight

The XLight node type now includes additional boolean flags to determine whether the light 
emits ambient, diffuse and specular colours. The flags can be accessed via the following 
functions: \n

\code
bool XLight::GetEmitAmbient() const; \n
bool XLight::GetEmitDiffuse() const; \n
bool XLight::GetEmitSpecular() const; \n

void XLight::SetEmitAmbient(const bool value); \n
void XLight::SetEmitDiffuse(const bool value); \n
void XLight::SetEmitSpecular(const bool value); \n
\endcode

\section wn4_01_8 Additions to XParametricCurve

The XParametricCurve node has been extended to provide information on whether the curve 
is open, closed or periodic. The following functions now provide access to this data: \n\n

\code
XU32 XParametricCurve::GetCurveType() const; \n
void XParametricCurve::SetCurveType(const XU32 type); \n
\endcode


\section wn4_01_9 The new XNodeCollection class

A new node type, XNodeCollection with type id, XFn::NodeCollection has been added 
to provide a common base class to all nodes that store collections of other nodes, i.e. 
XDisplayLayer, XRenderLayer and XObjectSet. No changes have been made to the interfaces
of those node types, with the exception that functions that were common to all have been
moved into the XNodeCollection class, namely: \n\n

\code
XU32   XNodeCollection::GetNumItems() const; \n
XBase* XNodeCollection::GetItem(const XU32 idx) const; \n
bool   XNodeCollection::IsItem(const XBase *item) const; \n
bool   XNodeCollection::AddItem(const XBase *item); \n
bool   XNodeCollection::DeleteItem(const XBase *item); \n
bool   XNodeCollection::DeleteItem(const XU32 idx); \n
void   XNodeCollection::GetItems(XList &items) const; \n
void   XNodeCollection::SetItems(const XList &items); \n
\endcode


\section wn4_01_10 The new XShadingNode class

This node has been added to provide a common base class to all XMaterial and XTexture derived 
nodes. This now allows the XMD::XLayeredShader node to properly handle layers of bothe textures 
and materials. This change also allows XMD to handle full shading networks in future. \n


\section wn4_01_11 Changes to XMD::XMaterial

Previously, XMD::XMaterial contained ambient, diffuse, specular and emission colours, as
well as shininess, bump map and environment map data. In some situations this made 
sense (such as the XMD::XPhong node), however in many situations this data was simply 
confusing (i.e. XMD::XUseBackground has no colour information since it implicitly uses the 
background as it's colour). To remedy this inconsistancy, the XMD::XMaterial class has now 
been stripped of all parameters apart from Diffuse colour. 



*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 1
/*! \page _00_tutorial 1. Getting Started with XMD

\section _00_0 Static or Dll Builds

First of all, if you plan to build and compile plug-ins or applications using XMD, you have
two possible options. You can either use XMD as a DLL (XMD.vcproj) or as a static library
(XMD_static.vcproj). If you are using the static library, then make sure you define XMD_NO_DLL
in your pre-processor settings. \n\n\n

\section _00_1 The XMD Namespaces

The XMD SDK is split into 3 namespaces \n\n

XM - this contains all maths classes used by XMD. \n
XMD - this contains the core of XMD. \n
XMU - this contains a set of useful utility classes to pre-process data. \n\n\n

\section _00_2 Initialisation

XMD::XMDInit() should be called to initialise the XMD SDK before you use any XMD Data Types,
and XMD::XMDCleanup() should be called to free the internally allocated memory within the library.\n\n\n

\section _00_3 The XModel class

The main class within XMD, is XMD::XModel which is the main class to handle loading and saving of data. There
are currently two different flavours of XMD files, an ascii and a binary version. So, the simplest possible code 
we can write with XMD would be something like this :

\code

#include "XMD/Model.h"
#include <iostream>

bool LoadAndConvertData(const char* filename)
{
  // you *must* call this before using XMD.
  XMD::XMDInit();


  // create an instance of a model. This will hold all loaded data
  XMD::XModel model;

  // If it loaded ok....
  if( model.Load(filename) == XMD::XFileError::Success )
  {
    XMD::XString out_ascii = filename;
    XMD::XString out_bin = filename;

    out_ascii += ".ascii.xmd";
    out_bin += ".binary.xmd";

    // now save an Ascii copy
    if( model.Save(out_ascii,true) != XMD::XFileError::Success )
    {
      std::cerr << "Could not save XMD \"" << out_ascii << "\"" << std::endl;
    }

    // now save a Binary copy
    if( model.Save(out_ascii,false) != XMD::XFileError::Success )
    {
      std::cerr << "Could not save XMD \"" << out_bin << "\"" << std::endl;
    }
  }
  else
  {
    std::cerr << "Could not load XMD \"" << filename << "\"" << std::endl;
  }

  // free's all data in the model
  model.Release();

  // This cleans up all statically allocated XMD data
  XMD::XMDCleanup();
}

\endcode

*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 2
/*! \page _01_tutorial 2. Listing the Loaded Data


\section xinfo The XMD::XInfo data

Within an XMD file, there may be a contained XMD::XInfo class that provided some information about
the origins of the file. i.e. The host machine and user that created the file, the original art 
package it was created in, etc. This data can be accessed from the XMD::XModel like so: \n\n

\code
void DumpFileInfo(XMD::XModel& model)
{
  const XMD::XInfo* file_info = model.GetInfo();
  if (file_info) 
  {
    std::cout << "INFO\n";
    std::cout << "  user " << file_info->GetUser().c_str() << std::endl;
    std::cout << "  host " << file_info->GetHost().c_str() << std::endl;
    std::cout << "  orig " << file_info->GetOriginalFile().c_str() << std::endl;
    std::cout << "  texf " << file_info->GetTextureFile().c_str() << std::endl;
    std::cout << "  soft " << file_info->GetSoftware().c_str() << std::endl;
  }
}
\endcode
\n\n This data can be invaluable when debugging any graphics build pipelines that use XMD.\n\n\n

\section xbase Listing XMD::XBase derived classes.

In the previous tutorial, we saw how to Load and Save an XDM file. Whilst that is all very useful, 
to be truly useful you really need to be able to access data within the XMD node's themselves. \n\n

All data items (with one or two exceptions) are derived from the common base class, XMD::XBase. This
class provides some basic information as to what type it is, it's name, and additional custom 
properties (which you can add dynamically). All Node types are identified using the enum XMD::XFn::Type which
lists all supported node types within XMD. \n\n

You can use the XMD::XModel class to list a set of nodes with a specific type, for example to output 
all node names and their type, you can do :
\n
\code
void DumpNodeInfo(XMD::XModel& model)
{
  XMD::XList nodes;
  model.List(nodes);

  XMD::XList::iterator it = nodes.begin();
  for(; it != nodes.end(); ++it )
  {
    XMD::XBase* node = *it;
    std::cout << node->GetName() << " " << node->ApiTypeString() << std::endl;
  }
}
\endcode
\n
Again, this is useful to know what type a node is, however we still can't access it's data
from the base class XMD::XBase. So, as a starting example, lets assume that we want to 
iterate over all of the XMD::XMaterial nodes and be able to access some of it's data. To do this,
we need to use the template function XMD::XBase::HasFn. This function is akin to a dynamic_cast, i.e.
\n\n
\code
void DumpMaterialInfo(XMD::XModel& model)
{
  XMD::XList nodes;
  model.List(nodes);

  XMD::XList::iterator it = nodes.begin();
  for(; it != nodes.end(); ++it )
  {
    XMD::XBase* node = *it;

    // 
    // use the HasFn<> method to ask whether node is compatible with 
    // the XMD::XMaterial class.
    //
    XMD::XMaterial* material = node->HasFn<XMD::XMaterial>();
    if(material)
    {
      std::cout << material->GetName() << " " << node->ApiTypeString() << std::endl;

      // lets use the XMaterial class to access the colour data...
      XM::XColour diffuse;
      material->GetDiffuse(diffuse);

      std::cout << "\tcolor: " << diffuse << std::endl;
    }
  }
}
\endcode
\n
Whilst this is starting to become more useful, it's a little un-elegant. To improve things a little, we can
use XMD::XModel::List to actually list the material nodes only, i.e. \n\n


\code
void DumpMaterialInfo(XMD::XModel& model)
{
  XMD::XList nodes;

  //
  // list only the material nodes in the model
  // 
  model.List(nodes,XMD::XFn::Material);

  XMD::XList::iterator it = nodes.begin();
  for(; it != nodes.end(); ++it )
  {
    // 
    // don't need to test whether material is NULL, since we know that the nodes XList
    // will only contain XMaterial derived classes.
    //
    XMD::XMaterial* material = (*it)->HasFn<XMD::XMaterial>();

    std::cout << material->GetName() << " " << node->ApiTypeString() << std::endl;

    // lets use the XMaterial class to access the colour data...
    XM::XColour diffuse;
    material->GetDiffuse(diffuse);

    std::cout << "\tcolor: " << diffuse << std::endl;
  }
}
\endcode

\n\n Accessing any XMD::XBase derived data node works in exactly the same way. It's simply a
case of using XMD::XModel::List to retrieve an XMD::List of nodes of the required type, and 
then using XMD::XBase::HasFn to convert to the correct type. \n\n

\section ximage The XMD::XImage data

XMD can optionally store basic RGB and RGBA image format data within an XMD::XModel (usually this forms 
part of the binary *.xtd file, or contained directly within the *.xmd binary file). This data can be
useful if you do not want to support various different file formats within your graphics build. \n\n

To access this data, you need to query the XMD::XImage objects contained within the XMD::XModel class. i.e. \n\n

\code
void DumpImageInfo(XMD::XModel& model)
{
  for (XU32 i=0;i!=mod.GetNumImages();++i) 
  {
    XMD::XImage* image = model.GetImage(i);
    if (image) 
    {
      std::cout << "IMAGE ";
      std::cout << image->GetMayaName();
      std::cout << " ";
      std::cout << image->GetFileName();
      std::cout << " " << image->Width() 
                << "x" << image->Height() 
                << "x" << image->Depth() << std::endl;

      // the pixel data if you need it...
      const XMD::XU8* pixels = image->Pixels();
    }
  }
}
\endcode

*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 3
/*! \page _02_tutorial 3. Traversing the Scene Data

\section xintro Introduction

Within XMD, there are 3 main classes that provide access to the parenting information available. These are
XMD::XBone, XMD::XShape and XMD::XInstance. The first class, XMD::XBone provides the basis of the hierarchical
scene graph. The class XMD::XShape is the base class for all geometry objects that can exist within the scene 
(i.e. meshes, lights, camera's etc). It is important to note that <b>you cannot parent a shape to a transform
directly!</b>. Instead, XMD uses the XMD::XInstance structure to instance an XMD::XShape any number of times within
the scene hierarchy. The rational behind this, is to allow XMD to handle a situation where you have an instanced
piece of geometry, but with different materials assigned to each instance. \n\n

\section xbone Traversing The Hierarchical Transforms

To retrieve a list of XBone derived classes, you can use either XMD::XModel::List() or XMD::XModel::GetBones()
to return a flat list of XMD::XBone derived classes; or you can use XMD::XModel::GetRootBones() to return a 
list of those XMD::XBone derived classes that do not have any parents. The functions, XMD::XBone::GetParent() and 
XMD::XBone::GetChildren() can be used to then query which transform nodes are the parent's or children respectively. \n\n

As a complete example, the following code hierarchically descends through the XMD::XModel starting at it's root bones
working all the way down through it's children.

\code
void DumpBone(XMD::XBone* bone,int indent=0)
{
  for(int i=0;i!=indent;++i)
    std::cout << "\t";

  std::cout << bone->GetName() << std::endl;

  XMD::XBoneList children;
  bone->GetChildren( children );

  XMD::XBoneList::iterator it = children.begin();
  XMD::XBoneList::iterator end = children.end();
  for(; it != end; ++it )
  {
    XMD::XBone* child = *it;
    DumpBone(child,indent+1);
  }
}

void DumpBoneHierarchy(XMD::XModel& model)
{
  XMD::XBoneList root_bones;
  model.GetRootBones(root_bones);

  XMD::XBoneList::iterator it = root_bones.begin();
  XMD::XBoneList::iterator end = root_bones.end();

  for(; it != end; ++it )
  {
    XMD::XBone* root = *it;
    DumpBone(root);
  }
}
\endcode\n\n

\section xshape Accessing the Shape Instances

Three functions exist to allow you to query shape instances within XMD, depending on how you wish to approach
it. So, \n\n

* XMD::XBone::GetObjectInstances() - to access the instanced shapes underneath a bone. \n
* XMD::XShape::GetObjectInstances() - to access all instances of a given shape. \n
* XMD::XModel::GetObjectInstances() - to access a flattened list of all shape instances within a scene. \n\n

One method to access the instance information would be to list all XMD::XShapes and get the instance info for 
each one, i.e.

\code
void DumpShapeInstances(XMD::XModel& model)
{
  XMD::XList shapes;
  model.List(shapes,XMD::XFn::Shape); // list all shape nodes

  XMD::XList::iterator it = shapes.begin();
  XMD::XList::iterator end = shapes.end();
  for( ; it != end; ++it )
  {
    XMD::XShape* shape = (*it)->HasFn<XMD::XShape>();

    std::cout << shape->GetName() << "\n";
    
    // now retrieve a list of instances
    XMD::XInstanceList instances;
    shape->GetObjectInstances(instances);

    //
    // iterate over each shape instance and write the parent transform
    //
    XMD::XInstanceList::iterator instance_it = instances.begin();
    XMD::XInstanceList::iterator instance_end = instances.begin();
    for( ; instance_it != instance_end; ++instance_it )
    {
      XMD::XInstance* instance = *instance_it;
      
      // grab the parent
      XMD::XBone* parent = instance->GetParent();

      std::cout << "\t" << parent->GetName() << "\n";
    }
  }
}
\endcode
\n\n
Alternatively we could just list all shapes parented to a specific bone transform
\n\n
\code
void DumpBoneInstances(XMD::XModel& model)
{
  XMD::XList transforms;
  model.List(transforms,XMD::XFn::Bone); // list all shape nodes

  XMD::XList::iterator it = transforms.begin();
  XMD::XList::iterator end = transforms.end();
  for( ; it != end; ++it )
  {
    XMD::XBone* transform = (*it)->HasFn<XMD::XBone>();

    std::cout << transform->GetName() << "\n";
    
    // now retrieve a list of instances
    XMD::XInstanceList instances;
    transform->GetObjectInstances(instances);

    //
    // iterate over each shape instance and write the name of the shape
    //
    XMD::XInstanceList::iterator instance_it = instances.begin();
    XMD::XInstanceList::iterator instance_end = instances.begin();
    for( ; instance_it != instance_end; ++instance_it )
    {
      XMD::XInstance* instance = *instance_it;
      
      // grab the shape
      XMD::XShape* shape = instance->GetShape();

      std::cout << "\t" << shape->GetName() << "\n";
    }
  }
}
\endcode



*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 4
/*! \page _03_tutorial 4. Accessing Mesh Data

\section _03_0 Vertex Data

To access the vertex Data from a mesh (or any other XMD::XGeometry derived class) use the XMD::XGeometry::Points
or XMD::XGeometry::GetPoints() methods. All vertex Data is stored in Local Space, so if you need to get hold of 
world space vertex data, access the World Space Matrix of the specific XMD::XBone parent of the mesh.\n\n

\code
void DumpVertices(XMD::XMesh* mesh)
{
  std::cout << "NumPoints " << mesh->GetNumPoints() << std::endl;

  XPointList::const_iterator it = mesh->Points().begin();
  XPointList::const_iterator end = mesh->Points().end();
  for(; it != end; ++it )
  {
    const XPoint& p = *it;
    std::cout << "\t" << p.x << " " << p.y << " " << p.z << std::endl;
  }
}
\endcode \n\n


\section _03_1 Normal Data

A mesh May or may not have vertex normals assigned to it. If it does not, then you can use the function 
XMD::XMesh::CalculateNormals() to generate them for you. The functions XMD::XMesh::GetNormals() and
XMD::XMesh::GetNumNormals() provide access to the local space normal vectors. As with the vertex data,
you will have to transform the normals into world space yourself using the parent bones world space matrix
(Make sure you use the Inverse Transpose World Matrix to transform vertex normals!). \n\n

\code
void DumpNormals(XMD::XMesh* mesh)
{
  // if no normals are found, generate them
  if(mesh->GetNumNormals()==0)
    mesh->CalculateNormals(false);

  std::cout << "NumNormals " << mesh->GetNumNormals() << std::endl;

  XNormalList::const_iterator it = mesh->GetNormals().begin();
  XNormalList::const_iterator end = mesh->GetNormals().end();
  for(; it != end; ++it )
  {
    const XNormal& n = *it;
    std::cout << "\t" << n.x << " " << n.y << " " << n.z << std::endl;
  }
}
\endcode \n\n


\section _03_2 Texture Coordinate Data

\code
void DumpUvCoordinates(XMD::XMesh* mesh)
{
  std::cout << "NumUvSets " << mesh->GetNumUvSets() << std::endl;

  for(XMD::XU32 i=0;i!=mesh->GetNumUvSets();++i)
  {
    std::cout << "\tUvSet" << i << std::endl;

    std::cout << "\t\tNumUvs " << mesh->GetNumTexCoords(i) << std::endl;
    XTexCoordList::const_iterator it = mesh->GetTexCoords(i).begin();
    XTexCoordList::const_iterator end = mesh->GetTexCoords(i).end();
    for(; it != end; ++it )
    {
      const XTexCoord& t = *it;
      std::cout << "\t" << t.u << " " << t.v << std::endl;
    }
  }
}
\endcode \n\n


\section _03_3 Accessing Face Data

The polygonal face data within the XMD is stored within the XMD::XFace class. The XMD::XMesh
simply stores a list of these faces that can be accessed using the function, XMD::XMesh::GetFaces().
Each XMD::XFace simply stores a list of indices that can be retrieved using the XMD::XFace::GetVertexIndex, 
XMD::XFace::GetNormalIndex and XMD::XFace::GetUvIndex functions. \n\n

\code
void DumpFaceData(XMD::XMesh* mesh)
{
  std::cout << "NumFaces " << mesh->GetNumFaces() << std::endl;

  // we need to know how many uv sets exist to allow us to output the 
  // UV coordinate indices for each UV set.
  unsigned int num_uv_sets = mesh->GetNumUvSets();

  // grab a reference to the face data
  const XMD::XFaceList& faces = mesh->GetFaces();

  // output each face
  for(XMD::XU32 i=0;i!=mesh->GetNumFaces();++i)
  {
    const XMD::XFace& face = faces[i];

    // output the face number, and the number of points on the face
    std::cout << "\tFace: " << i << std::endl;
    std::cout << "\t\tNumPoints " << face.NumPoints() << std::endl;

    for(XMD::XU32 j=0;j!=face.NumPoints();++j)
    {
      // output the j'th vertex index
      std::cout << "\t\t\tv: " << face.GetVertexIndex(j);

      // output the j'th normal index
      std::cout << "\tn: " << face.GetNormalIndex(j);

      // output the uv set indices
      for(XMD::XU32 k=0;k!=num_uv_sets;++k)
      {
        std::cout << "\tt" << k << ": " << face.GetUvIndex(j,k);
      }
      std::cout << std::endl;
    }
  }
}
\endcode \n\n


\section _03_4 VertexArray / VertexBuffer data

The polygonal data provided by the XMD::XMesh class is provided in the same format as most 3D authoring packages.
This however normally needs to be converted to a single indexed format for use with vertex arrays or vertex buffers
in OpenGL or Direct3D. For this you have two options, either provide your own conversion routines, or you can make use 
of the XMU::XVertexArray class to perform the conversion for you. 


\code

#include "XMU/XVertexArray.h"

void WriteMeshAsVertexArray(XMD::XMesh* mesh)
{
  // initialise the vertex array data
  XMU::XVertexArray va(mesh);

  std::cout << "num_verts " << va.GetNumVertices() << "\n";
  {
    unsigned vertex_idx=0;
    unsigned normal_idx=0;
    unsigned uv_idx=0;

    // write the vertex data
    for(unsigned i=0;i!=va.GetNumVertices();++i,vertex_idx+=3,normal_idx+=3,uv_idx+=2)
    {
      std::cout << "\t" << va.Vertices()[vertex_idx]
                << ", " << va.Vertices()[vertex_idx+1]
                << ", " << va.Vertices()[vertex_idx+2];
                
      std::cout << "\t" << va.Normals()[normal_idx]
                << ", " << va.Normals()[normal_idx+1]
                << ", " << va.Normals()[normal_idx+2];
                
      if(va.GetNumUvSets()>0)
      {
        std::cout << "\t" << va.Uvs0()[uv_idx]
                  << ", " << va.Uvs0()[uv_idx+1];
      }         
      
      std::cout << std::endl;
    }
  }

  std::cout << "num_indices " << va.GetNumIndices() << "\n";
  {
    XMD::XIndexList::const_iterator it = va.Indices().begin();
    for(unsigned int i=0;it != va.Indices().end(); ++it, ++i )
    {
      if( i%3 == 0 ) std::cout << "\n\t";
      std::cout << *it << " ";
    }
    std::cout << "\n";
  }
}

\endcode


*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 5
/*! \page _04_tutorial 5. Accessing Material Data

Material data for XMD::XGeometry derived classes cannot be accessed from the geometry data themselves (this
basically just concerns meshes and parametric surfaces). Instead, the data must be accessed from the XInstance
classes. This is to allow the ability for XMD to store multiple instances of a piece of geometry, with a different
material assigned to each instance. In the case of parametric surfaces, only one material will be assigned to each 
instance, so this is a fairly easy situation to deal with : \n\n

\code
void DumpMaterialsAssigneToNurbs(XMD::XModel& model)
{
  XMD::XList nurbs;
  model.List(nurbs,XMD::XFn::ParametricSurface);

  // iterate over all surfaces
  XMD::XList::iterator it = nurbs.begin();
  XMD::XList::iterator end = nurbs.end();
  for( ; it != end; ++it )
  {
    XMD::XParametricSurface* ps = (*it)->HasFn<XMD::XParametricSurface>();

    std::cout << "surface " << ps->GetName() << std::endl;

    // get the object instances from the surface
    XMD::XInstanceList instances;
    ps->GetObjectInstances(instances);

    XMD::XInstanceList::iterator it_inst = instances.begn();
    XMD::XInstanceList::iterator end_inst = instances.end();
    for( int i=0; it_inst!=end_inst; ++it_inst, ++i)
    {
      XMD::XInstance* instance = *it_inst;

      // get the material assigned to the surface (may be NULL)
      XMD::XMaterial* material = instance->GetMaterial();

      // output the material applied to the instance
      std::cout << "\tinst" << i;
      if(material)
        std::cout << "\tmaterial " << material->GetName() << std::endl;
      else
        std::cout << "\tmaterial none" << std::endl;
    }
  }
}
\endcode

\n\n
Accessing the materials on a mesh is done in a very similar way, however with a mesh you may have more than 
one material assigned to each instance. If the mesh instance has more than one material, then it will also contain
a set of face indices for each material. As an example : \n\n

\code

void DumpMaterialsAssigneToMeshes(XMD::XModel& model)
{
  XMD::XList meshes;
  model.List(meshes,XMD::XFn::Mesh);

  // iterate over all surfaces
  XMD::XList::iterator it = meshes.begin();
  XMD::XList::iterator end = meshes.end();
  for( ; it != end; ++it )
  {
    XMD::XMesh* mesh = (*it)->HasFn<XMD::XMesh>();

    std::cout << "surface " << mesh->GetName() << std::endl;

    // get the object instances from the surface
    XMD::XInstanceList instances;
    ps->GetObjectInstances(instances);

    XMD::XInstanceList::iterator it_inst = instances.begn();
    XMD::XInstanceList::iterator end_inst = instances.end();
    for( int i=0; it_inst!=end_inst; ++it_inst, ++i)
    {
      XMD::XInstance* instance = *it_inst;

      // if the instance has material groups, then there is more than
      // one material assigned to it. 
      // 
      if(instance->HasMaterialGroups())
      {
        std::cout << "\tinst" << i << "\n";
        for(unsigned j=0;j!=instance->GetNumMaterialGroups();++j)
        {
          // get the j'th material assigned to the mesh
          XMD::XMaterial* material = instance->GetMaterial(j);

          // now get the face indices to which the material is assigned
          XMD::XIndexList face_indices;
          instance->GetFaces(j,face_indices);

          // dump the material name
          std::cout << "\t\tmaterial " << material->GetName() << " [";

          // output the indices

          XMD::XIndexList::iterator itf = face_indices.begin();
          XMD::XIndexList::iterator endf = face_indices.end();
          for( ; itf != endf; ++itf )
          {
            std::cout << *itf << " ";
          }
          std::cout << "]\n";
        }
      }
      else
      //
      // otherwise, there is only one material assigned to it.
      //
      {
        // get the material assigned to the mesh (may be NULL)
        XMD::XMaterial* material = instance->GetMaterial();

        // output the material applied to the instance
        std::cout << "\tinst" << i;
        if(material)
          std::cout << "\tmaterial " << material->GetName() << std::endl;
        else
          std::cout << "\tmaterial none" << std::endl;
      }

    }
  }
}


\endcode




*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 6
/*! \page _05_tutorial 6. Accessing Geometry Deformers

The class XMD::XGeometry derives directly from XMD::XShape and extends it with the inclusion of a set of
deformable points (XMD::XGeometry::GetPoints). Lattices, meshes, and all NURBS surfaces and curves all derive 
from this geometry type. \n\n

The geometry points contained within the class can be modified by a sequence of deformers, which can be 
accessed via the function XMD::XGeometry::GetDeformerQueue().


\code
void DumpGeometryDeformers(XMD::XModel& model)
{
  XMD::XList geometry;
  model.List(geometry,XMD::XFn::Geometry); // list all geometry nodes

  XMD::XList::iterator it = geometry.begin();
  XMD::XList::iterator end = geometry.end();
  for( ; it != end; ++it )
  {
    XMD::XGeometry* geom = (*it)->HasFn<XMD::XGeometry>();

    std::cout << geom->GetName() << "\n";
    
    // now retrieve the deformer queue for the geometry
    XMD::XDeformerList deformer_queue;
    geom->GetDeformerQueue(deformer_queue);

    //
    // iterate over each deformer and write
    //
    XMD::XDeformerList::iterator def_it = deformer_queue.begin();
    XMD::XDeformerList::iterator def_end = deformer_queue.begin();
    for( ; def_it != def_end; ++def_it )
    {
      XMD::XDeformer* deformer = *def_it;      

      std::cout << "\t" << deformer->GetName() << " " << deformer->ApiTypeString() << "\n";
    }
  }
}
\endcode

*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 7
/*! \page _06_tutorial 7. Accessing Animation Cycle Data

All animation cycle data within XMD is stored within the XMD::XAnimCycle class. The functions XMD::XModel::NumAnimCycles
and XMD::XModel::GetAnimCycle allow you to query the animation cycles present. Currently XMD stores a sampled
set of keyframes for all XMD::XBone's that get written to the file. This may seem a bit wasteful, however it 
ensures that animations get correctly interpretted (in comparison to exporting the original animation curves,
which may fail if animations have been authored using constraints, expressions of IK. In that case, it is common
for someone to 'bake' the animation curve data down to a key-frame for all bones on all frames, which ultimately
results in exactly the same data that XMD generates). \n\n

The XMD::XAnimCycle stores a set of XMD::XSampledKeys structures for each animated bone. You can therefore query
the local space animation data using the following code.... \n\n

\code
void DumpAnimCycleInfo(XMD::XModel& model)
{
  for(unsigned i=0;i!=model.NumAnimCycles();++i)
  {
    XMD::XAnimCycle* cycle = model.GetAnimCycle(i);

    // dump some general info about the animation cycle
    std::cout << "anim: " << cycle->GetName() << "\n";
    std::cout << "\tstart_frame " << cycle->GetStartTime() << "\n";
    std::cout << "\tend_frame " << cycle->GetEndTime() << "\n";
    std::cout << "\tnum_frames " << cycle->GetNumFrames() << "\n";
    std::cout << "\tfps " << cycle->GetFramesPerSecond() << "\n";

    // get a list of the bones in the model... 
    XMD::XBoneList bones;
    model.GetBones(bones);

    // iterate over the bones to see if there are any keys for them...
    XMD::XBoneList::iterator it = bones.begin();
    XMD::XBoneList::iterator end = bones.end();
    for(;it != end;++it)
    {
      XMD::XBone* bone = *it;

      // see if there are keys for this bone
      const XMD::XSampledKeys* keys = cycle->GetBoneKeys(bone);

      if(keys)
      {
        std::cout << "\tkeys for " << bone->GetName() << "\n";
        for(unsigned j=0;j!=cycle->GetNumFrames();++j)
        {
          XM::XQuaternion rotation = keys.RotationKeys()[j];
          XM::XVector3 translate = keys.TranslationKeys()[j];
          XM::XVector3 scale = keys.ScaleKeys()[j];
          std::cout << "T: " << translate << "   "
                    << "R: " << rotation << "   "
                    << "S: " << scale << "\n";
        }
      }
      else
      {
        std::cout << "\tno keys present for " << bone->GetName() << "\n";
      }
    }
  }
}
\endcode

It's very important that you realise that the rotation keys returned may also require the Pre and Post rotation
values applied to the rotational keys to access the correct data. (You can query these using the XMD::XBone::GetJointOrient()
and XMD::XBone::GetRotationOrient() functions respectivly). It is however possible to bypass this stage completely and
simply get XMD to compute matrices for you and use the matrices to decompose into TRS values yourself. i.e.

\code
void DumpAnimCycleInfo(XMD::XModel& model)
{
  // get a list of the bones in the model... 
  XMD::XBoneList bones;
  model.GetBones(bones);


  for(unsigned i=0;i!=model.NumAnimCycles();++i)
  {
    XMD::XAnimCycle* cycle = model.GetAnimCycle(i);

    for(unsigned j=0;j!=cycle->GetNumFrames();++j)
    {
      // sets the entire model to the specified frame
      cycle->SetToFrame(j);


      // iterate over the bones to see if there are any keys for them...
      XMD::XBoneList::iterator it = bones.begin();
      XMD::XBoneList::iterator end = bones.end();
      for(;it != end;++it)
      {
        XMD::XBone* bone = *it;

        // world and local space matrices for this bone at this frame in the cycle... 
        const XMatrix& local_space = bone->GetLocal();
        const XMatrix& world_space = bone->GetWorld();
      }
    }
  }
}
\endcode





*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 8
/*! \page _07_tutorial 8. Accessing Custom Attributes

XMD has the ability to store additional custom attributes on a node. Sometimes these attributes are those 
specified by users (via the extra attributes mechanism in Maya, or via user properties in Max); or they 
can occur to hold additional attributes on a node that are not supported by the core chunk definitions 
within XMD. 

*/

//-------------------------------------------------------------------------------------------------------------    TUTORIAL 9
/*! \page _08_tutorial 9. Accessing Custom Node Data

XMD has the ability to add your own custom data nodes via a simple lua script within Maya (currently this feature
is unsupported in Max and XSI). These custom nodes can be used to export proprietary plug-in data into the format
without requiring a re-compile of the XMD plug-ins 

*/

