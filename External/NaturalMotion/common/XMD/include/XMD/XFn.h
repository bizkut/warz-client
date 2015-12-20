//----------------------------------------------------------------------------------------------------------------------
/// \file   XFn.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This defines the enum for all node types
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <set>
#include <string>
#include "XM2/MathCommon.h"
//----------------------------------------------------------------------------------------------------------------------
#ifdef WIN32
  #ifndef XMD_STATIC
    #ifdef XMD_EXPORTS
      /// DLL export define
      #define XMD_EXPORT __declspec(dllexport)
    #else
      /// DLL export define
      #define XMD_EXPORT __declspec(dllimport)
    #endif
  #else
    /// DLL export define
    #define XMD_EXPORT
  #endif
#else
  /// DLL export define
  #define XMD_EXPORT
#endif

#ifdef _DEBUG
  #ifdef _MSC_VER
    #define XMD_ASSERT(X) _ASSERT(X)
  #else
    #include<assert.h>
    #define XMD_ASSERT(X) assert(X)
  #endif
#else
  #define XMD_ASSERT(X)
#endif
#include "XM2/pod_vector.h"
#include "XM2/vector.h"
namespace XM2
{
  class XM2EXPORT XBoundingBox;
  class XM2EXPORT XColour;
  class XM2EXPORT XMatrix;
  class XM2EXPORT XPlane;
  class XM2EXPORT XQuaternion;
  class XM2EXPORT XQuaternionSOA;
  class XM2EXPORT XRay;
  class XM2EXPORT XSphere;
  class XM2EXPORT XTransform;
  class XM2EXPORT XVector2;
  class XM2EXPORT XVector2SOA;
  class XM2EXPORT XVector3;
  class XM2EXPORT XVector3Packed;
  class XM2EXPORT XVector3SOA;
  class XM2EXPORT XVector4;
  class XM2EXPORT XVector4SOA;
  class XM2EXPORT XColourArray;
  class XM2EXPORT XMatrixArray;
  class XM2EXPORT XQuaternionArray;
  class XM2EXPORT XQuaternionSOAArray;
  class XM2EXPORT XRealArray;
  class XM2EXPORT XTransformArray;
  class XM2EXPORT XVector2Array;
  class XM2EXPORT XVector3Array;
  class XM2EXPORT XVector3PackedArray;
  class XM2EXPORT XVector3SOAArray;
  class XM2EXPORT XVector4Array;
  class XM2EXPORT XVector4SOAArray;
}

namespace XMD
{
  /// \brief  The XMD version 
  #define XMD_VERSION_NUMBER 4
  /// \brief  The XMD version 
  #define XMD_VERSION "XMD_004"
  /// \brief  The ASCII identifier
  #define XMD_VERSION_ASCII XMD_VERSION "A"
  /// \brief  The Binary identifier
  #define XMD_VERSION_BINARY XMD_VERSION "B"

  #ifdef _MSC_VER
    /// \todo remove this comment. 
    /// being disabled for sanities sake. I wanna see how many of these I can actually kill 
    /// off in the API
    #pragma warning( disable: 4251 )
  #endif

  using XM2::XReal;
  using XM2::XU8;
  using XM2::XU16;
  using XM2::XU32;
  using XM2::XS8;
  using XM2::XS16;
  using XM2::XS32;
  using XM2::XBoundingBox; 
  using XM2::XColour;
  using XM2::XMatrix;
  using XM2::XPlane;
  using XM2::XQuaternion;
  using XM2::XQuaternionSOA;
  using XM2::XRay;
  using XM2::XSphere;
  using XM2::XTransform;
  using XM2::XVector2;
  using XM2::XVector2SOA;
  using XM2::XVector3;
  using XM2::XVector3Packed;
  using XM2::XVector3SOA;
  using XM2::XVector4;
  using XM2::XVector4SOA;
  using XM2::XColourArray;
  using XM2::XMatrixArray;
  using XM2::XQuaternionArray;
  using XM2::XQuaternionSOAArray;
  using XM2::XRealArray;
  using XM2::XTransformArray;
  using XM2::XVector2Array;
  using XM2::XVector3Array;
  using XM2::XVector3PackedArray;
  using XM2::XVector3SOAArray;
  using XM2::XVector4Array;
  using XM2::XVector4SOAArray;

  typedef XU32 XId;                          ///< a type used as an ID for a node
  typedef XM2::pod_vector<XU32> XIndexList;  ///< an array of node ID's
  typedef std::string           XString;     ///< the common string type
  typedef char                  XChar;       ///< an 8bit ascii character
  typedef XM2::pod_vector<bool> XBoolArray;  ///< an array of booleans
  typedef XM2::pod_vector<int>  XIntArray;   ///< an array of integers
  typedef XM2::pod_vector<XS8>  XS8Array;    ///< an array of signed 8bit integers
  typedef XM2::pod_vector<XS16> XS16Array;   ///< an array of signed 16bit integers
  typedef XM2::pod_vector<XS32> XS32Array;   ///< an array of signed 32bit integers
  typedef XM2::pod_vector<XU8>  XU8Array;    ///< an array of unsigned 8bit integers
  typedef XM2::pod_vector<XU16> XU16Array;   ///< an array of unsigned 16bit integers
  typedef XM2::pod_vector<XU32> XU32Array;   ///< an array of unsigned 32bit integers
  typedef XM2::vector<XString>  XStringList; ///< an array of strings
};

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief This namespace holds the enumerations of the various supported Node types. Some of these may as 
///        yet be un-supported, they will all work by the end of the series though ;)
/// \note  I've tried wherever possible to try and retain a similarity to the way maya stores it's data, this 
///        is reflected in the way the objects are derived. ie, formating denotes the inheritance tree.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFn {
public:

  /// dtor
  virtual ~XFn() {};
  
  /// \brief  an enum containing all supported types in XMD
  enum Type {

    /// base of everything
    Base=0,

      /// a plug-in node type
      PxNode=1,

      /// a shading node
      ShadingNode = 168,

        /// a texture
        Texture=2,

          /// a texture file containing one or more textures
          BumpMap=3,

          /// a base environment map type
          EnvMap=4,

            /// a base cube map type
            CubeMap=5,

            /// a sphere map type
            SphereMap=6,

          /// defines a texture composed of a set of other textures
          LayeredTexture=170,

        /// the base material type
        Material=7,

          /// lambert material
          Lambert=10,

            /// material with specular component
            Specular=169,

              /// anisotropic surface material type
              Anisotropic=8,

              /// blinn material
              Blinn=9,

              /// phong material
              Phong=11,

              /// phong extended material
              PhongE=12,

          /// a surface shader
          SurfaceShader=13,

          /// a layered surface shader
          LayeredShader=14,

          /// a shader defined over a ramp
          RampShader=15,

          /// the predator effect shader - leaves background intact, adds specular only
          UseBackgroundShader=16,

          /// a plugin material node
          PxMaterial=17,

      /// the base hardware shader type
      HwShader=18,

        /// a plugin hardware shader node
        PxHwShader=19,

      /// base class of all sets and layers
      NodeCollection=167,

        /// a collection of visible objects
        DisplayLayer=20,

        /// a collection of renderable objects that are rendered in a single pass.
        RenderLayer=21,

        /// a set of objects (component sets not supported)
        ObjectSet=22,

      /// an IK chain comprising of a handle, effector, pole vector and joint chain
      IkChain=23,

      /// an object instance
      Instance=24,

      /// base class of all shape nodes that can be parented in the scene (ie, mesh, locators etc)
      Shape=25,

        /// the base type for all shapes
        Geometry=26,

          /// a deformed lattice shape - see lattice.h
          Lattice=27,
          
          /// a mesh
          Mesh=28,

          /// polygonal mesh data
          PolygonMesh = 166,

          /// common stuph for dynamic LOD surfaces
          ParametricBase=30,

            /// base parametric curve type
            ParametricCurve=31,

              /// cubic bezier curve
              BezierCurve=32,

              /// cubic hermite curve
              HermiteCurve=33,

              /// NURBS curve
              NurbsCurve=34,

            /// base parametric surface type
            ParametricSurface=36,

              /// cubic bezier patch
              BezierPatch=37,

              /// cubic hermite patch
              HermitePatch=38,

              /// NURBS surface
              NurbsSurface=39,

          /// particle shape
          ParticleShape=40,

          /// a plugin shape node
          PxGeometry=42,

        /// a perspective camera
        Camera=43,

        /// base class of volume primitive locator types
        VolumeObject=44,

          /// volume sphere
          VolumeSphere=45,

          /// volume box
          VolumeBox=46,

          /// volume cone
          VolumeCone=47,

          /// volume cylinder
          VolumeCylinder=164,

        /// a base lattice shape - see lattice.h
        BaseLattice=48,

        /// locator node
        Locator=49,

          /// base class for rigid bodies
          CollisionObject=52,

            /// a box shaped rigid body
            CollisionBox=53,

            /// a capsule shaped rigid body
            CollisionCapsule=54,

            /// a cone shaped rigid body
            CollisionCone=55,

            /// a cylinder shaped rigid body
            CollisionCylinder=56,

            /// a geometry shaped rigid body
            CollisionMesh=57,

            /// a plane shaped rigid body
            CollisionPlane=58,

            /// a sphere shaped rigid body
            CollisionSphere=59,
      
        /// the base light type
        Light=76,

          /// a simple ambient light
          AmbientLight=77,

          /// a light that has shadow info
          ExtendedLight=78,

            /// a simple area light
            AreaLight=79,

            /// a point light
            PointLight=80,

            /// directional light (sun for Xsi peeps)
            DirectionalLight=81,

            /// a spot light
            SpotLight=82,
              
            /// a spot light
            VolumeLight=83,

          /// a plugin light type
          PxLight=84,

        /// a plugin shape/locator type
        PxShape=85,

      /// the scene graph transform type
      Bone=86,  
        
        /// a joint - ie, the joint orientation becomes important
        Joint=87,

        /// a plugin shape node
        PxTransform=88,

        /// a 2D texture placement node
        TexturePlacement2D=89,

        /// a 3D texture placement node
        TexturePlacement3D=90,

        /// a constraint node
        Constraint=91,

          /// constrain a transform to aim at a second
          AimConstraint=92,

            /// look-at-constraint
            LookAtConstraint=171,

          /// constrain a transform to some geometry
          GeometryConstraint=93,

          /// constrain a transform to the normal of a surface
          NormalConstraint=94,

          /// constrain a transform to orient the same as a second transform
          OrientConstraint=95,

          /// constrain a transform to act as a child of another transform
          ParentConstraint=96,

          /// constrain a transform to the same position as a second transform
          PointConstraint=97,
          
            /// pole vector constraint
            PoleVectorConstraint=172,

          /// constrain a transform to the same scale as a second transform
          ScaleConstraint=98,

          /// constrain a transform to follow the tangent of a curve
          TangentConstraint=99,

          /// a plugin constraint node
          PxConstraint=100,

        /// a particle emitter
        Emitter=101,  

          /// a plugin particle emitter node
          PxEmitter=102,
    
        /// the base Field node
        Field=103,

          /// a plugin dynamics field node
          PxField=104,

          /// Air field
          Air=105,

          /// Drag field
          Drag=106,

          /// Gravity field
          Gravity=107,

          /// Newton field
          Newton=108,

          /// Radial field
          Radial=109,

          /// Turbulence field
          Turbulence=110,

          /// uniform field
          Uniform=111,

          /// volume axis field
          VolumeAxis=112,

          /// vortex field
          Vortex=113,

      /// the base deformer type
      Deformer=114,

        /// a plugin surface deformer node
        PxDeformer=115,

        /// blend shape deformer
        BlendShape=116,

        /// a cluster deformation
        Cluster=117,
      
        /// Free-Form-Deformation
        FFd=118,

        /// Soft skinning (aka enveloping Xsi)
        SkinCluster=119,

        /// a jiggle deformer
        JiggleDeformer=120,

        /// rigid skinning
        JointCluster=121,

        /// non-linear deformer type
        NonLinearDeformer=122,

          /// non linear bend deformation
          NonLinearBend=123,

          /// non linear flare deformation
          NonLinearFlare=124,

          /// non linear sine deformation
          NonLinearSine=125,

          /// non linear squash deformation
          NonLinearSquash=126,

          /// non linear twist deformation
          NonLinearTwist=127,

          /// non linear wave deformation
          NonLinearWave=128,

        /// a wire deformer
        WireDeformer=129,

        /// a surface wrap deformation
        WrapDeformer=130,

        /// a sculpt deformation
        SculptDeformer=131,

      /// contains an animation take
      AnimationTake = 165,

      /// \b [DEPRECATED] All animation data associated with a specific animation cycle/loop. 
      /// Please use XFn::AnimationTake instead.
      AnimCycle=133,

      /// a plugin animation cycle node
      PxAnim=134,

      /// A generic attribute
      Attribute=135,

        /// a boolean attribute
        BoolAttribute=136,

        /// a colour attribute
        ColourAttribute=137,

        /// a floating point attribute type
        FloatAttribute=138,

        /// a 2D floating point attribute type (u,v)
        Float2Attribute=139,

        /// a 3D floating point attribute type (x,y,z)
        Float3Attribute=140,

        /// a 3D vector attribute
        VectorAttribute=Float3Attribute,

        /// a 4D floating point attribute type (x,y,z,w)
        Float4Attribute=141,

        /// an integer attribute type
        IntAttribute=142,

        /// an 2D integer attribute type (u,v)
        Int2Attribute=143,

        /// an 3D integer attribute type (x,y,z)
        Int3Attribute=144,

        /// an 4D integer attribute type (x,y,z,w)
        Int4Attribute=145,

        /// a string attribute 
        StringAttribute=146,

        /// a connected output attribute
        MessageAttribute=147,

        /// a matrix attribute
        MatrixAttribute=148,

        /// a boolean array attribute
        BoolArrayAttribute=149,

        /// a string array attribute
        ColourArrayAttribute=150,

        /// a floating point array attribute type
        FloatArrayAttribute=151,

        /// a 2D floating point array attribute type
        Float2ArrayAttribute=152,

        /// a 3D floating point array attribute type
        Float3ArrayAttribute=153,

        /// a 3D vector array attribute
        VectorArrayAttribute=Float3ArrayAttribute,

        /// a 4D floating point array attribute type
        Float4ArrayAttribute=154,

        /// an integer array attribute type
        IntArrayAttribute=155,

        /// a 2D integer array attribute type
        Int2ArrayAttribute=156,

        /// a 3D integer array attribute type
        Int3ArrayAttribute=157,

        /// a 4D integer array attribute type
        Int4ArrayAttribute=158,

        /// a string array attribute
        StringArrayAttribute=159,

        /// an array of outputs
        MessageArrayAttribute=160,

        /// a matrix attribute
        MatrixArrayAttribute=161,

    /// a file info structure (header)
    Info=162,

    /// container for vertex colours
    MeshVertexColours=163,

    Last=173,

    /// a flag for an inherited node description
    InheritedFileFlag = 0x1000,

    /// a flag for the new file descriptions within the binary file. 
    NewFileFlag     = 0x1001,

    /// used specifically to flag nodes within the binary file as to what type they are.
    /// this is used so that new node types can always be resolved independantly of
    /// different Apps using XMD. ie, if the node being read in was exported from maya,
    /// however you want a custom node rather than the base Px nodes, the NewNode type
    /// can be resolved from the info at the start of the xld file, and then be re-mapped
    /// to your custom hard coded description.
    ///
    CustomFileStart = 0x2000,

    /// This flag is used to define new node ID's for C++ registered types. The file flags
    /// can then be re-mapped to these types if defined.
    ///
    CustomStart = 0x8000,

    /// This flag indicates the start of the inherited node ID's. The ID's held in the file
    /// we be re-mapped to what is currently in use within any XMD files loaded. This way
    /// nodes from different XMD or XMD files *should* be re-mapped correctly.
    ///
    InheritedStart = 0xD000,

    Max=0xFFFF      
  };
};
#ifndef DOXYGEN_PROCESSING
bool XMD_EXPORT operator==(const XFn::Type& t, const XU16& et);
bool XMD_EXPORT operator==(const XU16& et, const XFn::Type& t);
bool XMD_EXPORT operator!=(const XFn::Type& t, const XU16& et);
bool XMD_EXPORT operator!=(const XU16& et, const XFn::Type& t);
bool XMD_EXPORT operator<(const XFn::Type& t, const XU16& et);
bool XMD_EXPORT operator<(const XU16& et, const XFn::Type& t);
bool XMD_EXPORT operator>(const XFn::Type& t, const XU16& et);
bool XMD_EXPORT operator>(const XU16& et, const XFn::Type& t);
#endif
}
