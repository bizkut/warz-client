#include "r3dPCH.h"
#include "r3d.h"
#include "r3dSpline.h"

r3dSpline3D::r3dSpline3D()
{
  iNumPt  = 0;
  bLoaded = 0;
  
  pV      = NULL;
  pIn     = NULL;
  pOut    = NULL;
}

r3dSpline3D::~r3dSpline3D()
{
  Unload();
}

void r3dSpline3D::Unload()
{
  if(!bLoaded) return;
  
  delete[] pV;
  delete[] pSeg;

  if(bBezier) {
    delete[] pIn;
    delete[] pOut;
  }
  
  iNumPt  = 0;
  bLoaded = 0;
}

void r3dSpline3D::LoadBinary(char *fname)
{
  if(bLoaded) Unload();

  //FIXME: implement resource leak fix if exception is thrown.
  r3dFile *f = r3d_open(fname, "rb");
  if(!f) r3dError("can't open %s", fname);
  
  binhdr_s hdr;
  if(fread(&hdr, sizeof(hdr), 1, f) != 1) r3dError("%s - can't read header from", fname);
  if(hdr.r3dID != R3D_BINARY_FILE_ID)     r3dError("%s - invalid header", fname);
  if(hdr.ID != R3D_SPLINE3D_BINARY_ID)	  r3dError("%s - not an animation file", fname);
  
  switch(hdr.Version) {
    default:
      r3dError("%s - unsupported version", fname);
      break;
    
    case 0x00000001:
    // reading V1
    {
      DWORD dw1;
      //in: number of splines - and ignore it!
      fread(&dw1, sizeof(DWORD), 1, f);

      //in: count
      fread(&dw1, sizeof(DWORD), 1, f);
      iNumPt = dw1;
      //in: flags
      DWORD dwFlags;
      fread(&dwFlags, sizeof(DWORD), 1, f);
      if(dwFlags & (1L<<0)) bBezier = 1;
      else                  bBezier = 0;
      //in: spline length
      fread(&fSplineLength, sizeof(float), 1, f);
      
      // alloc
      pV   = gfx_new r3dPoint3D[iNumPt];
      pSeg = gfx_new seg_s[iNumPt-1];
      if(bBezier) {
        pIn  = gfx_new r3dPoint3D[iNumPt];
        pOut = gfx_new r3dPoint3D[iNumPt];
      }
      
      //in: vertices
      float fl[3];
      for(int i=0; i<iNumPt; i++) {
        fread(&fl[0], sizeof(float), 3, f);
        pV[i].Assign(fl[0], fl[1], fl[2]);
        
        if(bBezier) {
          fread(&fl[0], sizeof(float), 3, f);
          pIn[i].Assign(fl[0], fl[1], fl[2]);
          fread(&fl[0], sizeof(float), 3, f);
          pOut[i].Assign(fl[0], fl[1], fl[2]);
        }
      }
      
      //in: segments
      float fStart = 0;
      for(int i=0; i<iNumPt-1; i++) {
        float fLen;
        fread(&fLen, sizeof(float), 1, f);
        pSeg[i].fPos1       = fStart;
        pSeg[i].fPos2       = (fStart + fLen);

        fStart += fLen;
      }
      
      bLoaded = 1;
      break;
    }
  }
  
  fclose(f);
  return;
}

void r3dSpline3D::Scale(float fScale)
{
  if(!bLoaded) return;
  for(int i=0; i<iNumPt; i++) {
    pV[i]  *= fScale;
    if(bBezier) {
      pIn[i]  *= fScale;
      pOut[i] *= fScale;
    }
  }

  fSplineLength *= fScale;
  for(int i=0; i<iNumPt-1; i++) {
    pSeg[i].fPos1 *= fScale;
    pSeg[i].fPos2 *= fScale;
  }

  return;  
}


// Three control point Bezier interpolation
// mu ranges from 0 to 1, start to end of curve
r3dPoint3D r3dSpline3D::Bezier3(const r3dPoint3D& p1, const r3dPoint3D& p2, const r3dPoint3D& p3, float mu)
{
	return (1 - mu)*(1 - mu)*p1 + 2*mu*(1 - mu)*p2 + mu * mu*p3;
}

// Four control point Bezier interpolation
// mu ranges from 0 to 1, start to end of curve
r3dPoint3D r3dSpline3D::Bezier4(const r3dPoint3D& p1, const r3dPoint3D& p2, const r3dPoint3D& p3, const r3dPoint3D& p4, float mu)
{
   float	mum1,mum13, mu3;
   r3dPoint3D	p;

   mum1  = 1 - mu;
   mum13 = mum1 * mum1 * mum1;
   mu3   = mu * mu * mu;

   p.X = mum13*p1.X + 3*mu*mum1*mum1*p2.X + 3*mu*mu*mum1*p3.X + mu3*p4.X;
   p.Y = mum13*p1.Y + 3*mu*mum1*mum1*p2.Y + 3*mu*mu*mum1*p3.Y + mu3*p4.Y;
   p.Z = mum13*p1.Z + 3*mu*mum1*mum1*p2.Z + 3*mu*mu*mum1*p3.Z + mu3*p4.Z;

   return p;
}

//   General Bezier curve
//   Number of control points is n+1 (IMPORTANT: the last point is not computed)
r3dPoint3D r3dSpline3D::Bezier(r3dPoint3D *p, int n, float mu)
{
   int k,kn,nn,nkn;
   float blend,muk,munk;
   r3dPoint3D b(0.0f, 0.0f, 0.0f);

   muk  = 1;
   munk = pow(1.0f - mu, n);

   for (k=0;k<=n;k++) {
      nn = n;
      kn = k;
      nkn = n - k;
      blend = muk * munk;
      muk *= mu;
	  if ( fabsf(1-mu) > 0.00001f )
		munk /= (1-mu);
      while (nn >= 1) {
         blend *= nn;
         nn--;
         if (kn > 1) {
            blend /= (float)kn;
            kn--;
         }
         if (nkn > 1) {
            blend /= (float)nkn;
            nkn--;
         }
      }
      b.X += p[k].X * blend;
      b.Y += p[k].Y * blend;
      b.Z += p[k].Z * blend;
   }

   return(b);
}


r3dPoint3D r3dSpline3D::GetPos(float fLen, int *iSeg, r3dPoint3D *vForw)
{
  if(!bLoaded)    
    return r3dPoint3D(0, 0, 0);
    
  r3dPoint3D v1, v2;

  // special case for both out-of-range versions
  if(fLen <= 0.0f) {
    v1 = pV[0];
    
    if(iSeg)   *iSeg = 0;
    if(vForw)  {
      v2 = GetPos(0.01f, NULL, NULL);
      *vForw = (v2 - v1).NormalizeTo();
    }
    return v1;
  }
  if(fLen >= fSplineLength) {
    v1 = pV[iNumPt-1];
    
    if(iSeg) *iSeg = iNumPt-2;
    if(vForw)  {
      int tmpseg = iNumPt-2;
      r3dPoint3D v2 = GetPos(fSplineLength-0.01f, &tmpseg, NULL);
      *vForw = -(v2 - v1).NormalizeTo();
    }
    
    return v1;
  }
  
  // search for segment
  int k;
  if(iSeg) {
    // if specified, search from that point
    k = *iSeg;
    if(fLen > pSeg[k].fPos2) {
      for(; fLen > pSeg[k].fPos2; k++) ;
    } else {
      for(; fLen < pSeg[k].fPos1; k--) ;
    }
    *iSeg = k;
  } else {
    // linearry search from beginning
    for(k=0; fLen > pSeg[k].fPos2; k++) ;
  }

  // calc  
  float fSegLen = pSeg[k].fPos2-pSeg[k].fPos1;
  float mu = (fLen - pSeg[k].fPos1) / fSegLen;

  if(bBezier) {
    v1 = Bezier4(pV[k], pOut[k], pIn[k+1], pV[k+1], mu);
    if(vForw)  {
      // use this segment (k) to set segment lookup
      r3dPoint3D v2 = GetPos(fLen+fSegLen/20.0f, &k, NULL);
      *vForw = (v2 - v1).NormalizeTo();
    }
  } else  {
    v1 = pV[k] + (pV[k+1] - pV[k]) * mu;
    if(vForw)  {
      *vForw = (pV[k+1] - v1).NormalizeTo();
    }
  }

  return v1;
}


void r3dSpline3D::Draw(const r3dCamera& Cam)
{
  R3DPROFILE_FUNCTION("r3dSpline3D::Draw");

  if(!iNumPt) return;
  //r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW);
  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
  
  r3dPoint3D v1;
  
  float w1 = 0.1f;
  float w2 = 0.05f;
  float w3 = 0.2f;
  
  // render vertices & paths
  if(0) {
    v1 = pV[0];
    for(int i=1; i<iNumPt; i++) {
      r3dDrawLine3D(v1, pV[i], Cam, w1, r3dColor(255, 255, 255));
      r3dDrawLine3D(v1, v1+r3dPoint3D(1, 0, 0), Cam, w2, r3dColor(255, 255, 255));
      r3dDrawLine3D(v1, v1+r3dPoint3D(0, 1, 0), Cam, w2, r3dColor(255, 255, 255));
      r3dDrawLine3D(v1, v1+r3dPoint3D(0, 0, 1), Cam, w2, r3dColor(255, 255, 255));

      v1 = pV[i];
    }
    if(0 && bBezier) {
      for(int i=0; i<iNumPt; i++) {
        r3dDrawLine3D(pV[i], pIn[i],  Cam, w2, r3dColor(255, 0, 0));
        r3dDrawLine3D(pV[i], pOut[i], Cam, w2, r3dColor(0, 255, 0));
      }
    }
  }

  if(1 && bBezier) {
    // show interpolated path

    v1 = pV[0];
    for(int i=0; i<iNumPt-1; i++) {
      for(float mu = 0.0f; mu <= 1.0001f; mu += 0.1f) {
        r3dPoint3D v2 = Bezier4(pV[i], pOut[i], pIn[i+1], pV[i+1], mu);
        r3dDrawLine3D(v1, v2, Cam, w3, r3dColor(255, 0, 255));

        //r3dDrawLine3D(v1, v1+r3dPoint3D(1, 0, 0), Cam, w2, r3dColor(255, 255, 255));
        //r3dDrawLine3D(v1, v1+r3dPoint3D(0, 1, 0), Cam, w2, r3dColor(255, 255, 255));
        //r3dDrawLine3D(v1, v1+r3dPoint3D(0, 0, 1), Cam, w2, r3dColor(255, 255, 255));


        v1 = v2;
      }
    }
  }

  return;
}
