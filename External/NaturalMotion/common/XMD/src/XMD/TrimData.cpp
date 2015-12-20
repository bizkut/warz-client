//----------------------------------------------------------------------------------------------------------------------
/// \file TrimData.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/TrimData.h"
#include "XMD/FileIO.h"
#include "XM2/Vector3Array.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XU32 Curve2D::GetDataSize() const 
{
  return static_cast<XU32>(
    sizeof(XReal)*(Knots.size()+uCoords.size()+vCoords.size()) +
    sizeof(XS32) + 
    sizeof(XU32)*3
    );
}

//----------------------------------------------------------------------------------------------------------------------
bool Curve2D::DoData(XFileIO& io) 
{
  DUMPER(Curve2D);

  IO_CHECK( io.DoDataVector(uCoords) );
  DAPARAM( uCoords );

  IO_CHECK( io.DoDataVector(vCoords) );
  DAPARAM( vCoords );

  IO_CHECK( io.DoDataVector(Knots) );
  DAPARAM( Knots );

  IO_CHECK( io.DoData(&Degree) );
  DPARAM( Degree );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 Curve3D::GetDataSize() const 
{
  return static_cast<XU32>(
    sizeof(XReal)*Knots.size() +
    CVs.size()*3*sizeof(XReal) +
    sizeof(XS32) + 
    sizeof(XU32)*2
    );
}

//----------------------------------------------------------------------------------------------------------------------
bool Curve3D::DoData(XFileIO& io) 
{
  DUMPER(Curve3D);

  IO_CHECK( io.DoDataVector(CVs) );
  DAPARAM( CVs );

  IO_CHECK( io.DoDataVector(Knots) );
  DAPARAM( Knots );

  IO_CHECK( io.DoData(&Degree) );
  DPARAM( Degree );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::Region::Region() 
  : m_Boundaries() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::Region::~Region() 
{
  XM2::pod_vector<Boundary*>::iterator it = m_Boundaries.begin();
  for (; it != m_Boundaries.end(); ++it)
  {
    delete *it;
  }
  m_Boundaries.clear();
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::Boundary::Boundary() 
  : m_Curves(),m_Type(XTrimData::kInner) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::Boundary::~Boundary() 
{
  XM2::pod_vector<TrimCurve*>::iterator it = m_Curves.begin();
  for (; it != m_Curves.end(); ++it)
  {
    delete *it;
  }
  m_Curves.clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::TrimCurve::DoData(XFileIO& io)  
{
  IO_CHECK( m_2D.DoData(io) );
  IO_CHECK( m_3D.DoData(io) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTrimData::TrimCurve::GetDataSize() const 
{
  return m_2D.GetDataSize() + m_3D.GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::Boundary::DoData(XFileIO& io)  
{
  IO_CHECK( io.DoDataVector(m_Curves) );
  IO_CHECK( io.DoData(&m_Type) );
  DPARAM( m_Type );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTrimData::Boundary::GetDataSize() const 
{
  XU32 _size_ = sizeof(XU32) + sizeof(BoundaryType);
  XM2::pod_vector<TrimCurve*>::const_iterator it = m_Curves.begin();
  for(;it != m_Curves.end();++it)
    _size_ += (*it)->GetDataSize();
  return _size_;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::Region::DoData(XFileIO& io)  
{
  IO_CHECK( io.DoDataVector(m_Boundaries) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTrimData::Region::GetDataSize() const 
{
  XU32 _size_ = sizeof(XU32);
  XM2::pod_vector<Boundary*>::const_iterator it = m_Boundaries.begin();
  for(;it != m_Boundaries.end();++it)
    _size_ += (*it)->GetDataSize();
  return _size_;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::DoData(XFileIO& io) 
{
  DUMPER(XAnimCurveSet);

  IO_CHECK( io.DoDataVector(m_Regions) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::XTrimData() : m_Regions()
{
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::XTrimData(const XTrimData& td)
{
  m_Regions.resize(td.m_Regions.size());
  for (XU32 i=0;i!=m_Regions.size();++i)
  {
    const Region* rc = td.m_Regions[i];
    Region* r = new Region;
    m_Regions[i] = r;

    r->m_Boundaries.resize(rc->m_Boundaries.size());
    for (XU32 j=0;j!=rc->m_Boundaries.size();++j)
    {
      const Boundary* bc = rc->m_Boundaries[j];
      Boundary* b = new Boundary;
      r->m_Boundaries[j] = b;

      b->m_Type = bc->m_Type;
      b->m_Curves.resize( bc->m_Curves.size() );

      for (XU32 k=0;k!=b->m_Curves.size();++k)
      {
        const TrimCurve* tc = bc->m_Curves[k];
        TrimCurve* t = new TrimCurve;
        b->m_Curves[k] = t;

        t->m_2D.Degree = tc->m_2D.Degree;
        t->m_2D.Knots = tc->m_2D.Knots;
        t->m_2D.uCoords = tc->m_2D.uCoords;
        t->m_2D.vCoords = tc->m_2D.vCoords;

        t->m_3D.Degree = tc->m_3D.Degree;
        t->m_3D.Knots = tc->m_3D.Knots;
        t->m_3D.CVs = tc->m_3D.CVs;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::~XTrimData()
{
  for (XU32 i=0;i!=m_Regions.size();++i)
  {
    delete m_Regions[i];
  }
  m_Regions.clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::SetBoundaryType(XS32 region,XS32 boundary,const BoundaryType bt) 
{
  if( static_cast<size_t>(region) < m_Regions.size() )
  {
    if( static_cast<XU32>(boundary) < m_Regions[region]->m_Boundaries.size() )
    {
      m_Regions[region]->m_Boundaries[boundary]->m_Type = bt;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::SetNumRegions(const XU32 num)
{
  if( num < m_Regions.size() )
  {
    for(XU32 i=num;i<m_Regions.size();++i)
    {
      delete m_Regions[i];
      m_Regions[i]=0;
    }
  }
  m_Regions.resize(num);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::SetNumBoundaries(XS32 region,const XU32 num)
{
  if( static_cast<size_t>(region) < m_Regions.size() )
  {
    XU32 old_size= static_cast<XU32>(m_Regions[region]->m_Boundaries.size());
    if( num < old_size )
    {
      for(XU32 i=num;i<old_size;++i)
      {
        delete m_Regions[region]->m_Boundaries[i];
        m_Regions[region]->m_Boundaries[i]=0;
      }
    }
    m_Regions[region]->m_Boundaries.clear();
    m_Regions[region]->m_Boundaries.resize(num);
    if( num > old_size )
    {
      for(XU32 i=old_size;i<num;++i) 
      {
        m_Regions[region]->m_Boundaries[i] = new Boundary;
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::SetNumCurves(XS32 region,XS32 boundary,const XU32 num)
{
  if( static_cast<XU32>(region) < m_Regions.size() )
  {
    if( static_cast<XU32>(boundary) < m_Regions[region]->m_Boundaries.size() )
    {
      XU32 old_size = static_cast<XU32>(m_Regions[region]->m_Boundaries[boundary]->m_Curves.size());
      if( num < old_size )
      {
        for(XU32 i=num;i<old_size;++i) 
        {
          delete m_Regions[region]->m_Boundaries[boundary]->m_Curves[i];
          m_Regions[region]->m_Boundaries[boundary]->m_Curves[i]=0;
        }
      }
      m_Regions[region]->m_Boundaries.clear();
      m_Regions[region]->m_Boundaries.resize(num);
      if( num > old_size )
      {
        for(XU32 i=old_size;i<num;++i)
        {
          m_Regions[region]->m_Boundaries[boundary]->m_Curves[i] = new TrimCurve;
        }
      }
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::Set2DCurve(XS32 region,XS32 boundary,XS32 curve,const Curve2D& c)
{
  if( static_cast<XU32>(region) < m_Regions.size() )
  {
    if( static_cast<XU32>(boundary) < m_Regions[region]->m_Boundaries.size() )
    {
      if( static_cast<XU32>(curve) < m_Regions[region]->m_Boundaries[boundary]->m_Curves.size() )
      {
        m_Regions[region]->m_Boundaries[boundary]->m_Curves[curve]->m_2D = c;
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTrimData::Set3DCurve(XS32 region,XS32 boundary,XS32 curve,const Curve3D& c)
{
  if( static_cast<XU32>(region) < m_Regions.size() )
  {
    if( static_cast<XU32>(boundary) < m_Regions[region]->m_Boundaries.size() )
    {
      if( static_cast<XU32>(curve) < m_Regions[region]->m_Boundaries[boundary]->m_Curves.size() )
      {
        m_Regions[region]->m_Boundaries[boundary]->m_Curves[curve]->m_3D = c;
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTrimData::GetDataSize() const
{
  XU32 _size_= sizeof(XU32);

  XM2::pod_vector<Region*>::const_iterator it = m_Regions.begin();
  for (;it != m_Regions.end();++it)
  {
    _size_ += (*it)->GetDataSize();
  }
  return _size_;    
}

//----------------------------------------------------------------------------------------------------------------------
XTrimData::BoundaryType XTrimData::GetBoundaryType(const XS32 region,const XS32 boundary) const 
{
  if (GetNumBoundaries(region)>boundary)
  {
    return m_Regions[region]->m_Boundaries[boundary]->m_Type;
  }
  return XTrimData::kLast;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XTrimData::GetNumRegions() const
{
  return static_cast<XS32>(m_Regions.size());
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XTrimData::GetNumBoundaries(const XS32 region) const 
{
  if( static_cast<XS32>(region) < GetNumRegions() )
    return static_cast<XS32>( m_Regions[region]->m_Boundaries.size() );
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XTrimData::GetNumCurves(const XS32 region,const XS32 boundary) const 
{
  if( static_cast<XS32>(boundary) < GetNumBoundaries(region) )
    return static_cast<XS32>( m_Regions[region]->m_Boundaries[boundary]->m_Curves.size() );
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const Curve2D* XTrimData::Get2DCurve(const XS32 region,const XS32 boundary,const XS32 curve) const
{
  if (GetNumCurves(region,boundary)>curve)
  {
    return &(m_Regions[region]->m_Boundaries[boundary]->m_Curves[curve]->m_2D);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const Curve3D* XTrimData::Get3DCurve(const XS32 region,const XS32 boundary,const XS32 curve) const 
{
  if (GetNumCurves(region,boundary)>curve)
  {
    return &(m_Regions[region]->m_Boundaries[boundary]->m_Curves[curve]->m_3D);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
std::istream &operator>>(std::istream& ifs,XTrimData& td)
{
  XS32 num_regions;
  XString buffer;
  SREAD_CHECK("num_regions",ifs);
  ifs >> num_regions;

  td.m_Regions.resize(num_regions);

  for(XS32 i=0;i!=num_regions;++i)
  {
    XS32 num_boundaries;
    SREAD_CHECK("region",ifs);
    SREAD_CHECK("{",ifs);
    SREAD_CHECK("num_boundaries",ifs);
    ifs >> num_boundaries;

    XTrimData::Region * pRegion = new XTrimData::Region;
    XMD_ASSERT(pRegion);
    td.m_Regions[i] = pRegion;

    pRegion->m_Boundaries.resize(num_boundaries);
    for (XS32 j=0;j!=num_boundaries;++j)
    {
      XTrimData::Boundary * p = new XTrimData::Boundary;
      XMD_ASSERT(p);
      pRegion->m_Boundaries[j] = p;

      SREAD_CHECK("boundary",ifs);
      SREAD_CHECK("{",ifs);
      SREAD_CHECK("type",ifs);
      ifs >> buffer;

      if (strcmp(buffer.c_str(),"InnerBoundary_ClockWise")==0)
        p->m_Type = XTrimData::kInner;
      else
      if (strcmp(buffer.c_str(),"OuterBoundary_AntiClockWise")==0)
        p->m_Type = XTrimData::kOuter ;
      else
      if (strcmp(buffer.c_str(),"BoundarySegment_CurveOnFace")==0)
        p->m_Type = XTrimData::kSegment;
      else
      if (strcmp(buffer.c_str(),"ClosedBoundarySegment_ClosedCurveOnFace")==0)
        p->m_Type = XTrimData::kClosedSegment;

      XS32 num_curves;
      SREAD_CHECK("num_curves",ifs);
      ifs >> num_curves;
      p->m_Curves.resize(num_curves);

      for (XS32 k=0;k!=num_curves;++k)
      {
        XTrimData::TrimCurve *tr = new XTrimData::TrimCurve;
        XMD_ASSERT(tr);
        p->m_Curves[k] = tr;

        XS32 num_knots,num_cvs;
        SREAD_CHECK("trim_curve",ifs);
        SREAD_CHECK("{",ifs);
        SREAD_CHECK("2d_parametric_curve",ifs);
        SREAD_CHECK("degree",ifs);
        ifs >> tr->m_2D.Degree;
        SREAD_CHECK("knots",ifs);
        ifs >> num_knots;

        tr->m_2D.Knots.resize(num_knots);
        for (XS32 l=0;l!=num_knots;++l)
        {
          ifs >> tr->m_2D.Knots[l];
        }

        SREAD_CHECK("cvs",ifs);
        ifs >> num_cvs;

        tr->m_2D.uCoords.resize(num_cvs);
        tr->m_2D.vCoords.resize(num_cvs);

        XRealArray::iterator itu = tr->m_2D.uCoords.begin();
        XRealArray::iterator itv = tr->m_2D.vCoords.begin();

        for (XS32 l=0;l!=num_cvs;++l,++itu,++itv)
        {
          ifs >> *itu >> *itv;
        }

        SREAD_CHECK("3d_local_curve",ifs);
        SREAD_CHECK("degree",ifs);
        ifs >> tr->m_3D.Degree;
        SREAD_CHECK("knots",ifs);
        ifs >> num_knots;

        tr->m_3D.Knots.resize(num_knots);
        for (XS32 l=0;l!=num_knots;++l)
        {
          ifs >> tr->m_3D.Knots[l];
        }

        SREAD_CHECK("cvs",ifs);
        ifs >> num_cvs;

        tr->m_3D.CVs.resize(num_cvs);

        XVector3Array::iterator itcv = tr->m_3D.CVs.begin();

        for (XS32 l=0;l!=num_cvs;++l,++itcv)
        {
          ifs >> *itcv ;
        }
        SREAD_CHECK("}",ifs);
      }

      SREAD_CHECK("}",ifs);
    }
    SREAD_CHECK("}",ifs);
  }
//  ifs >> buffer;  //  }  ?
  return ifs;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream& ofs,const XTrimData& td)
{
  ofs << "\tnum_regions " <<  static_cast<XU32>(td.m_Regions.size()) << "\n";

  for(XU32 i=0;i!=td.m_Regions.size();++i)
  {
    XTrimData::Region * pRegion = td.m_Regions[i];

    ofs << "\tREGION {\n"  //  region
      << "\t\tnum_boundaries " //  {
      << static_cast<XU32>(pRegion->m_Boundaries.size())  //    num_boundaries
      << "\n";

    XU32 nBoundaries =  static_cast<XU32>(pRegion->m_Boundaries.size());

    for (XU32 j=0;j!=nBoundaries;++j)
    {
      XTrimData::Boundary * p = pRegion->m_Boundaries[j];

      ofs << "\t\tBOUNDARY {\n"  //  Boundary
        << "\t\t\ttype ";
      
      switch(p->m_Type) 
      {
      case XTrimData::kInner:
        ofs << "InnerBoundary_ClockWise\n";
        break;
      case XTrimData::kOuter:
        ofs << "OuterBoundary_AntiClockWise\n";
        break;
      case XTrimData::kSegment:
        ofs << "BoundarySegment_CurveOnFace\n";
        break;
      case XTrimData::kClosedSegment:
        ofs << "ClosedBoundarySegment_ClosedCurveOnFace\n";
        break;
      default:
        break;
      }

      ofs << "\t\t\tnum_curves "  // numCurves
        << static_cast<XU32>(p->m_Curves.size()) << "\n";

      XU32 nCurves =  static_cast<XU32>(p->m_Curves.size());

      for (XU32 k=0;k!=nCurves;++k)
      {
        XTrimData::TrimCurve *tr = p->m_Curves[k];
        XMD_ASSERT(tr);

        ofs << "\t\t\tTRIM_CURVE {\n";

        {
          {
            ofs  << "\t\t\t\t2d_parametric_curve\n"
              << "\t\t\t\tdegree "  //    Degree
              << tr->m_2D.Degree
              << "\n\t\t\t\tknots "  //  Knots 
              << static_cast<XU32>(tr->m_2D.Knots.size())
              << "\n\t\t\t\t";

            for (XU32 l=0;l!=tr->m_2D.Knots.size();++l)
            {
              ofs << tr->m_2D.Knots[l] << " ";
            }

            ofs << "\n\t\t\t\tcvs "  // CVS
              << static_cast<XU32>(tr->m_2D.uCoords.size()) 
              << "\n";

            XRealArray::iterator itu = tr->m_2D.uCoords.begin();
            XRealArray::iterator itv = tr->m_2D.vCoords.begin();

            for ( ;itu != tr->m_2D.uCoords.end();++itu,++itv)
            {
              ofs << "\t\t\t\t\t" << *itu << " " << *itv << "\n";
            }

          }

          {
            ofs << "\t\t\t\t3d_local_curve\n"
              << "\t\t\t\tdegree "  //    Degree
              << tr->m_3D.Degree
              << "\n\t\t\t\tknots "  //  Knots 
              << static_cast<XU32>(tr->m_3D.Knots.size())
              << "\n\t\t\t\t";

            for (XU32 l=0;l!=tr->m_3D.Knots.size();++l)
            {
              ofs << tr->m_3D.Knots[l] << " ";
            }

            ofs << "\n\t\t\t\tcvs "  // CVS
              << static_cast<XU32>(tr->m_3D.CVs.size()) 
              << "\n";

            XVector3Array::iterator itcv = tr->m_3D.CVs.begin();

            for ( ;itcv!=tr->m_3D.CVs.end();++itcv)
            {
              ofs << "\t\t\t\t\t" << *itcv << "\n";
            }

          }
        }

        ofs << "\t\t\t}\n";  //  }
      }

      ofs << "\t\t}\n";  //  }
    }
    ofs << "\t}\n";  //  }
  }
  return ofs;
}
}
