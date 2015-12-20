//----------------------------------------------------------------------------------------------------------------------
/// \file ExtraAttributes.cpp  (v2.0)
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ExtraAttributes.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
void XExtraAttributes::NodeDeath(XModel* model,XId node)
{
  DUMPER(XExtraAttributes__NodeDeath);
  iterator it = begin();
  for( ; it != end(); ++it ) 
  {
    XAttribute* attr = *it;
    attr->NodeDeath(model,node);
  }    
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XExtraAttributes::GetDataSize() const
{
  XU32 sz = 4;
  const_iterator it = begin();
  for( ; it != end(); ++it ) 
  {
    sz += (*it)->GetDataSize();
  }
  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
XExtraAttributes::XExtraAttributes(void) 
  : XAttributeList()
{
}

//----------------------------------------------------------------------------------------------------------------------
XExtraAttributes::XExtraAttributes(const XExtraAttributes& ea)
{
  resize(ea.size());
  iterator it = begin();
  const_iterator oit = ea.begin();
  for (;it != end(); ++it, ++oit)
  {
    *it = new XAttribute( *(*oit) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
XExtraAttributes::~XExtraAttributes()
{
  iterator it = begin();
  for(; it != end(); ++it )
    delete *it;
}

//----------------------------------------------------------------------------------------------------------------------
bool XExtraAttributes::DoData(XFileIO& io,XModel* model)
{
  DUMPER(XExtraAttributes);

  XU32 num = static_cast<XU32>(size());
  IO_CHECK( io.DoData(&num) );

  if(io.IsReading())
  {
    resize(num);
    iterator it = begin();
    for (;it != end();++it) 
    {
      XAttribute* attr = new XAttribute;
      if(!attr->DoData(io,model))
        return false;
      *it = attr;
    }
  }
  else
  {
    iterator it = begin();
    for (;it != end();++it) 
    {
      if(!(*it)->DoData(io,model))
        return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const XAttribute* XExtraAttributes::GetAttr(const XString& name) const
{
  DUMPER(XExtraAttributes__GetAttr);
  const_iterator it = begin(); 
  for( ; it != end() ; ++it )
  {
    if ((*it)->Name() == name) 
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XExtraAttributes::GetAttr(const XString& name) 
{
  DUMPER(XExtraAttributes__GetAttr);
  iterator it = begin(); 
  for( ; it != end() ; ++it )
  {
    if ((*it)->Name() == name) 
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XExtraAttributes::ListAttr(XStringList& attr_names) const
{
  DUMPER(XExtraAttributes__ListAttr);
  attr_names.resize(size());
  XStringList::iterator its = attr_names.begin();
  const_iterator it = begin();
  for( ; it != end(); ++it,++its )
  {  
    *its = (*it)->Name();
  }
}

//----------------------------------------------------------------------------------------------------------------------
XString stripString(std::istream& ifs) 
{
  XString s;
  XString buffer;
  while (buffer.size()==0 || !buffer[0]) 
  {
    if (!ifs.good()) 
    {
      return "";
    } 
    buffer.clear();
    XChar c;
    while((c = (XChar)ifs.get())!='\n' && (c!='\r'))
    {
      buffer += c;
    } 
  }

  XU32 j=0;
  while (j<buffer.size() && buffer[j] != '\"') 
  {
    ++j;
  }

  // skip opening "
  ++j;

  bool done=false;
  while (!done)
  {
    XChar c = buffer[j];
    switch(c)
    {
    case '\0':
      break;

    case '\\':
      {
        switch(buffer[++j]) 
        {
          case 't':
            s.push_back('\t');
            break;
          case 'n':
            s.push_back('\n');
            break;
          case 'a':
            s.push_back('\a');
            break;
          case 'r':
            s.push_back('\r');
            break;
          case '\"':
            s.push_back('\"');
            break;
          case '\\':
            s.push_back('\\');
            break;
          case '0':
            s.push_back('\0');
            break;
          default:
            break;
        }
      }
      break;

    case '\"':
      done = true;
      break;

    default:
      s.push_back(c);
      break;
    }
    ++j;
  }
  return s;
}

//----------------------------------------------------------------------------------------------------------------------
std::istream &operator>>(std::istream& ifs,XExtraAttributes& ea)
{
  DUMPER(XExtraAttributes__operator_file_in);
  XString buffer;
  SREAD_CHECK("{",ifs);

  while (!ifs.eof()) 
  {
    // check for attribute name, or }
    ifs >> buffer;
    if (buffer[0] == '}')
    {
      break;
    }
    XAttribute* ptr = new XAttribute;
    ea.push_back(ptr);

    ptr->SetName(buffer.c_str());

    // read attribute type
    ifs >> buffer;

    XFn::Type type = XFn::Attribute;

    if (buffer == "bool")
    {
      type = XFn::BoolAttribute;
    }
    else
    if (buffer == "float") 
    {
      type = XFn::FloatAttribute;
    }
    else
    if (buffer == "float2")
    {
      type = XFn::Float2Attribute;
    }
    else
    if (buffer == "float3") 
    {
      type = XFn::Float3Attribute;
    }
    else
    if (buffer == "float4") 
    {
      type = XFn::Float4Attribute;
    }
    else
    if (buffer == "int") 
    {
      type = XFn::IntAttribute;
    }
    else
    if (buffer == "int2") 
    {
      type = XFn::Int2Attribute;
    }
    else
    if (buffer == "int3")
    {
      type = XFn::Int3Attribute;
    }
    else
    if (buffer == "int4")
    {
      type = XFn::Int4Attribute;
    }
    else
    if (buffer == "string") 
    {
      type = XFn::StringAttribute;
    }
    else
    if (buffer == "colour")
    {
      type = XFn::ColourAttribute;
    }
    else
    if (buffer == "matrix") 
    {
      type = XFn::MatrixAttribute;
    }
    else
    if (buffer == "message")
    {
      type = XFn::MessageAttribute;
    }
    else


    if (buffer == "vector<bool>")
    {
      type = XFn::BoolArrayAttribute;
    }
    else
    if (buffer == "vector<float>") 
    {
      type = XFn::FloatArrayAttribute;
    }
    else
    if (buffer == "vector<float2>") 
    {
      type = XFn::Float2ArrayAttribute;
    }
    else
    if (buffer == "vector<float3>") 
    {
      type = XFn::Float3ArrayAttribute;
    }
    else
    if (buffer == "vector<float4>")
    {
      type = XFn::Float4ArrayAttribute;
    }
    else
    if (buffer == "vector<colour>") 
    {
      type = XFn::ColourArrayAttribute;
    }
    else
    if (buffer == "vector<int>")
    {
      type = XFn::IntArrayAttribute;
    }
    else
    if (buffer == "vector<int2>") 
    {
      type = XFn::Int2ArrayAttribute;
    }
    else
    if (buffer == "vector<int3>")
    {
      type = XFn::Int3ArrayAttribute;
    }
    else
    if (buffer == "vector<int4>") 
    {
      type = XFn::Int4ArrayAttribute;
    }
    else
    if (buffer == "vector<string>")
    {
      type = XFn::StringArrayAttribute;
    }
    else
    if (buffer == "vector<matrix>") 
    {
      type = XFn::MatrixArrayAttribute;
    }
    else
    if (buffer == "vector<message>")
    {
      type = XFn::MessageArrayAttribute;
    }
    else 
      // if you assert here, the attribute type specified is invalid and unknown
      XMD_ASSERT(0);

    ptr->SetType(type);

    XU32 sz;
    switch(type)
    {      
    case XFn::BoolAttribute:
      ifs >> ptr->b;
      break;
    
    case XFn::FloatAttribute:
      ifs >> ptr->f;
      break;

    case XFn::Float2Attribute:
      ifs >> ptr->f2->x >> ptr->f2->y;
      break;

    case XFn::Float3Attribute:
      ifs >> ptr->f3->x >> ptr->f3->y >> ptr->f3->z;
      break;

    case XFn::Float4Attribute:
      ifs >> ptr->f4->x >> ptr->f4->y >> ptr->f4->z >> ptr->f4->w;
      break;
    
    case XFn::IntAttribute:
      ifs >> ptr->i;
      break;

    case XFn::Int2Attribute:
      ifs >> ptr->i2->x >> ptr->i2->y;
      break;

    case XFn::Int3Attribute:
      ifs >> ptr->i3->x >> ptr->i3->y >> ptr->i3->z;
      break;

    case XFn::Int4Attribute:
      ifs >> ptr->i4->x >> ptr->i4->y >> ptr->i4->z >> ptr->i4->w;
      break;
    
    case XFn::StringAttribute:
      *ptr->s = stripString(ifs);
      break;

    case XFn::ColourAttribute:
      ifs >> (ptr->col)->r >> (ptr->col)->g >> (ptr->col)->b ;
      break;

    case XFn::MatrixAttribute:
      ifs >> *(ptr->m);
      break;

    case XFn::MessageAttribute:
      // nothing to read/write for messages. All handled by the connections
      break;

    case XFn::BoolArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->ab->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        XS32 val;
        ifs >> val;
        (ptr->ab)->at(i) = val==0?false:true;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::FloatArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->af->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (*(ptr->af))[i];;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::Float2ArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->af2->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (*(ptr->af2))[i].x >> (*(ptr->af2))[i].y;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::Float3ArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->af3->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (*(ptr->af3))[i].x >> (*(ptr->af3))[i].y >> (*(ptr->af3))[i].z ;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::Float4ArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->af4->resize(sz);
      for(XU32 i=0;i<sz;++i)
      {
        ifs >> (*(ptr->af4))[i].x >> (*(ptr->af4))[i].y >> (*(ptr->af4))[i].z >> (*(ptr->af4))[i].w;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::IntArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->ai->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (*(ptr->ai))[i];;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::Int2ArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->ai2->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (*(ptr->ai2))[i].x >> (*(ptr->ai2))[i].y;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::Int3ArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->ai3->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (*(ptr->ai3))[i].x >> (*(ptr->ai3))[i].y >> (*(ptr->ai3))[i].z;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::Int4ArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->ai4->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (*(ptr->ai4))[i].x >> (*(ptr->ai4))[i].y >> (*(ptr->ai4))[i].z >> (*(ptr->ai4))[i].w;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::StringArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->as->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        (*(ptr->as))[i] = stripString(ifs);
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::ColourArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->acol->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (*(ptr->acol))[i].r >> (*(ptr->acol))[i].g >> (*(ptr->acol))[i].b;
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::MatrixArrayAttribute:
      ifs >> sz;
      SREAD_CHECK("{",ifs);
      ptr->am->resize(sz);
      for(XU32 i=0;i<sz;++i) 
      {
        ifs >> (ptr->am)->at(i);
      }
      SREAD_CHECK("}",ifs);
      break;

    case XFn::MessageArrayAttribute:
      ifs >> ptr->i ;
      // nothing to read/write for messages. All handled by the connections
      break;

    default:
      break;
    }
  }
  return ifs;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream& ofs,const XExtraAttributes& ea)
{
  if (ea.empty()) 
  {
    return ofs;
  }
  DUMPER(XExtraAttributes__operator_file_out);

  ofs << "\tEXTRA_ATTRIBUTES\n\t{\n";

  XExtraAttributes::const_iterator it = ea.begin();
  for( ; it != ea.end(); ++it )
  {
    const XAttribute* ptr = *it;

    ofs << "\t\t" << ptr->Name() << " ";
    
    switch(ptr->Type()) 
    {
    case XFn::BoolAttribute:
      ofs <<"bool "
        << ptr->b
        << "\n";
      break;

    case XFn::FloatAttribute:
      ofs <<"float "
        << ptr->f
        << "\n";
      break;

    case XFn::Float2Attribute:
      ofs <<"float2 "
          << ptr->f2->x << " "
          << ptr->f2->y << "\n";
      break;

    case XFn::Float3Attribute:
      ofs <<"float3 "
          << ptr->f3->x << " "
          << ptr->f3->y << " "
          << ptr->f3->z << "\n";
      break;

    case XFn::Float4Attribute:
      ofs <<"float4 "
        << ptr->f4->x << " "
        << ptr->f4->y << " "
        << ptr->f4->z << " "
        << ptr->f4->w << "\n";
      break;

    case XFn::IntAttribute:
      ofs <<"int "
          << ptr->i
          << "\n";
      break;

    case XFn::Int2Attribute:
      ofs <<"int2 "
          << ptr->i2->x << " "
          << ptr->i2->y << "\n";
      break;

    case XFn::Int3Attribute:
      ofs <<"int3 "
          << ptr->i3->x << " "
          << ptr->i3->y << " "
          << ptr->i3->z << "\n";
      break;

    case XFn::Int4Attribute:
      ofs <<"int4 "
          << ptr->i4->x << " "
          << ptr->i4->y << " "
          << ptr->i4->z << " "
          << ptr->i4->w << "\n";
      break;

    case XFn::StringAttribute:
      ofs <<"string ";
      {
        ofs << "\"";
        for(XU32 i=0;i<ptr->s->size();++i) 
        {
          XChar c = (*ptr->s)[i];

          switch(c) 
          {
          case '\n':
            ofs << "\\n";
            break;
          case '\t':
            ofs << "\\t";
            break;
          case '\a':
            ofs << "\\a";
            break;
          case '\r':
            ofs << "\\r";
            break;
          case '\"':
            ofs << "\\\"";
            break;
          case '\0':
            ofs << "\\0";
            break;
          default:
            ofs << c;
            break;
          }
        }
        ofs << "\"\n";
      }
      break;

    case XFn::ColourAttribute:
      ofs <<"colour ";
      ofs << ptr->col->r << " " 
        << ptr->col->g << " "
        << ptr->col->b << "\n";
      break;

    case XFn::MessageAttribute:
      ofs <<"message\n";
      break;

    case XFn::MatrixAttribute:
      ofs <<"matrix ";
      for(XU32 i=0;i!=16;++i)
        ofs << ptr->m->data[i] << " ";
      ofs << "\n";
      break;

    case XFn::BoolArrayAttribute:
      ofs << "vector<bool> "
        << static_cast<XU32>(ptr->ab->size()) 
        << " { ";

      for(XU32 i=0;i<ptr->ab->size();++i) 
      {
        ofs << (*ptr->ab)[i] << " ";
      }
      ofs << " }\n";
      break;

    case XFn::FloatArrayAttribute:
      ofs <<"vector<float> "
        << static_cast<XU32>(ptr->af->size()) 
        << " { ";

      for(XU32 i=0;i<ptr->af->size();++i)
      {
        ofs << (*ptr->af)[i] << " ";
      }
      ofs << " }\n";
      break;

    case XFn::Float2ArrayAttribute:
      ofs <<"vector<float2> "
          << static_cast<XU32>(ptr->af2->size()) 
          << "\n\t\t{\n";

      for(XU32 i=0;i<ptr->af2->size();++i) 
      {
        ofs << (*ptr->af2)[i].x  << "  " << (*ptr->af2)[i].y << "\n";
      }
      ofs << "\t\t}\n";
      break;

    case XFn::Float3ArrayAttribute:
      ofs <<"vector<float3> "
          << static_cast<XU32>(ptr->af3->size()) 
          << "\n\t\t{\n";

      for(XU32 i=0;i<ptr->af3->size();++i) 
      {
        ofs << "\t\t\t" << (*ptr->af3)[i].x  << "  " << (*ptr->af3)[i].y << "  " << (*ptr->af3)[i].z << "\n";
      }
      ofs << "\t\t}\n";
      break;

    case XFn::Float4ArrayAttribute:
      ofs <<"vector<float4> "
          << static_cast<XU32>(ptr->af4->size()) 
          << "\n\t\t{\n";

      for(XU32 i=0;i<ptr->af4->size();++i) 
      {
        ofs << "\t\t\t" << (*ptr->af4)[i].x  << "  " << (*ptr->af4)[i].y << "  " << (*ptr->af4)[i].z << "  " << (*ptr->af4)[i].w << "\n";
      }
      ofs << "\t\t}\n";
      break;

    case XFn::IntArrayAttribute:
      ofs <<"vector<int> "
          << static_cast<XU32>(ptr->ai->size()) 
          << " { ";

      for(XU32 i=0;i<ptr->ai->size();++i) 
      {
        ofs << (*ptr->ai)[i] << " ";
      }
      ofs << " }\n";
      break;

    case XFn::Int2ArrayAttribute:
      ofs <<"vector<int2> "
          << static_cast<XU32>(ptr->ai2->size()) 
          << "\n\t\t{\n";

      for(XU32 i=0;i<ptr->ai2->size();++i) 
      {
        ofs << "\t\t\t" << (*ptr->ai2)[i].x  << "  " << (*ptr->ai2)[i].y << "\n";
      }
      ofs << "\t\t}\n";
      break;

    case XFn::Int3ArrayAttribute:
      ofs <<"vector<int3> "
          << static_cast<XU32>(ptr->ai3->size()) 
          << "\n\t\t{\n";

      for(XU32 i=0;i<ptr->ai3->size();++i)
      {
        ofs << "\t\t\t" << (*ptr->ai3)[i].x  << "  " << (*ptr->ai3)[i].y << "  " << (*ptr->ai3)[i].z << "\n";
      }
      ofs << "\t\t}\n";
      break;

    case XFn::Int4ArrayAttribute:
      ofs <<"vector<int4> "
          << static_cast<XU32>(ptr->ai4->size()) 
          << "\n\t\t{\n";

      for(XU32 i=0;i<ptr->af4->size();++i) 
      {
        ofs << "\t\t\t" << (*ptr->ai4)[i].x  << "  " << (*ptr->ai4)[i].y << "  " << (*ptr->ai4)[i].z << "  " << (*ptr->ai4)[i].w << "\n";
      }
      ofs << "\t\t}\n";
      break;

    case XFn::StringArrayAttribute:
      ofs <<"vector<string> "
        << static_cast<XU32>(ptr->as->size())
        << "\n\t\t{\n";

      for(XU32 i=0;i<ptr->as->size();++i)
      {
        XString& so = (*ptr->as)[i];

        ofs << "\t\t\t\"";
        for(XU32 j=0;j<so.size();++j) 
        {
          XChar c = so[j];

          switch(c)
          {
          case '\n':
            ofs << "\\n";
            break;
          case '\t':
            ofs << "\\t";
            break;
          case '\a':
            ofs << "\\a";
            break;
          case '\r':
            ofs << "\\r";
            break;
          case '\"':
            ofs << "\\\"";
            break;
          case '\0':
            ofs << "\\0";
            break;
          default:
            ofs << c;
            break;
          }
        }
        ofs << "\"\n";
      }
      ofs << "\t\t}\n";
      break;

    case XFn::ColourArrayAttribute:
      ofs <<"vector<colour> "
          << static_cast<XU32>(ptr->acol->size()) 
          << " {\n";

      for(XU32 i=0;i<ptr->acol->size();++i)
      {
        ofs << "\t\t\t" << (*ptr->acol)[i].r << " " 
            << (*ptr->acol)[i].g << " "
            << (*ptr->acol)[i].b << "\n";
      }
      ofs << " }\n";
      break;

    case XFn::MessageArrayAttribute:
      ofs <<"vector<message> ";
      ofs << ptr->i << "\n";
      break;

    case XFn::MatrixArrayAttribute:
      ofs <<"vector<matrix> "
          << static_cast<XU32>(ptr->am->size()) 
          << "\t\t{\n";

      for(XU32 ii=0;ii<ptr->am->size();++ii) 
      {
        const XMatrix& m = (*(ptr->am))[ii];
        ofs << "\t\t\t";
        for(XU32 i=0;i!=16;++i)
          ofs << m.data[i] << " ";
        ofs << "\n";
      }

      ofs << "\t\t}\n";
      break;

    default:
      // if you assert here, the attribute type specified is invalid and unknown
      XMD_ASSERT(0);
      break;
    }

  }
  ofs << "\t}\n";
  return ofs;
}
}

