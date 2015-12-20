#include "r3dPCH.h"
#include "r3d.h"

#include "UI\UIElements.h"

void UIElm::DrawWH(float x, float y, float w, float h, float px, float py, const r3dColor& col) const
{
  float tw = 1.0f / pTex->GetWidth();
  float th = 1.0f / pTex->GetHeight();
  
  float x2 = sx + sw * px;
  float y2 = sy + sh * py;

  float TC[16];
  TC[0] = sx * tw; TC[1] = sy * th;
  TC[2] = x2 * tw; TC[3] = sy * th;
  TC[4] = x2 * tw; TC[5] = y2 * th;
  TC[6] = sx * tw; TC[7] = y2 * th;

  r3dDrawBox2D(x, y, w, h, col, pTex, TC);
}

UIElements::UIElements(int size)
{
  elmCurSize = 0;
  elmMaxSize = size;
  elmData    = game_new UIElm[size];
}

void UIElements::AddData1(r3dTexture* baseTex, const char** data, int size)
{
  assert(elmCurSize + size < elmMaxSize);
  
  char buf[1024];
  int gx, gy;
  int sx, sy;
  int ssx, ssy;
  
  for(int i=0; i<size; i++) {
    r3dscpy(buf, data[i]);

    char* name;
    char* token;
    name = strtok(buf, ";");

    token = strtok(NULL, ";");
    sscanf(token, "%dx%d", &gx, &gy);
    token = strtok(NULL, ";");
    sscanf(token, "%d,%d", &sy, &sx);	// note: sx/sy was swapped there.
    token = strtok(NULL, ";");
    sscanf(token, "%dx%d", &ssx, &ssy);
    
    UIElm& e = elmData[elmCurSize++]; 
    e.name = name;
    e.pTex = baseTex;
    e.sx   = float(gx * sx);
    e.sy   = float(gy * sy);
    e.sw   = float(gx);
    e.sh   = float(gy);
  }
  
  return;
}


void UIElements::AddData2(r3dTexture* baseTex, const char** data, int size)
{
  assert(elmCurSize + size <= elmMaxSize);
  
  char name[1024];
  int  x, y, w, h;

  for(int i=0; i<size; i++) {

    //"<elm name=mainpane id=0 x=0 y=577 w=1024 h=191 />",
    int scanned = sscanf(data[i], "<elm name=%s id=0 x=%d y=%d w=%d h=%d", name, &x, &y, &w, &h);
    assert(scanned == 5);

    UIElm& e = elmData[elmCurSize++]; 
    e.name = name;
    e.pTex = baseTex;
    e.sx   = float(x);
    e.sy   = float(y);
    e.sw   = float(w);
    e.sh   = float(h);
  }
  
  return;
}

UIElements::~UIElements()
{
  delete[] elmData;
}


UIElm* UIElements::Find(const char* name)
{
  for(int i=0; i<elmCurSize; i++) {
    if(stricmp(elmData[i].name.c_str(), name) == NULL)
      return &elmData[i];
  }
  
  r3dOutToLog("no element '%s'\n", name);
  assert(0);
  
  return &elmData[0];
}

void UIElements::Draw(const char* name, float x, float y, const r3dColor& col)
{
  const UIElm* e = Find(name);
  e->Draw(x, y, col);
}


void uiDrawFrame(UIElements* ui_elm, float in_x, float in_y, float in_w, float in_h, const char* base)
{
  //r3dDrawBox2D((float)x, (float)y, (float)w, (float)h, clr, NULL);
  //return;

  // 9 elements (index as number in numeric keyboard layout)
  char elms[10][256];
  sprintf(elms[7], "%s UL", base);
  sprintf(elms[8], "%s Top", base);
  sprintf(elms[9], "%s UR", base);
  sprintf(elms[4], "%s Left", base);
  sprintf(elms[5], "%s Center", base);
  sprintf(elms[6], "%s Right", base);
  sprintf(elms[1], "%s BL", base);
  sprintf(elms[2], "%s Bottom", base);
  sprintf(elms[3], "%s BR", base);
  
  const UIElm* sq[10];
  for(int i=1; i<10; i++) {
    sq[i] = ui_elm->Find(elms[i]);
  }
  
  float w  = sq[5]->sw;
  float h  = sq[5]->sh;
  float x0 = in_x;
  float y0 = in_y;
  float x1 = in_x + w;
  float y1 = in_y + h;
  float x2 = in_x + in_w - w;
  float y2 = in_y + in_h - h;
  
  // top line
  sq[7]->Draw(x0, y0);
  sq[8]->DrawWH(x1, y0, x2-x1, h);
  sq[9]->Draw(x2, y0); 
  
  // left/center/right
  sq[4]->DrawWH(x0, y1, w, y2-y1);
  sq[5]->DrawWH(x1, y1, x2-x1, y2-y1);
  sq[6]->DrawWH(x2, y1, w, y2-y1);

  // bottom line
  sq[1]->Draw(x0, y2);
  sq[2]->DrawWH(x1, y2, x2-x1, h);
  sq[3]->Draw(x2, y2);
                              
  return;
}

void uiDrawButton(UIElements* ui_elm, float x, float y, const char* btnname, bool hovering, bool selected)
{
  const r3dColor24 normal_col = r3dColor24(200, 200, 200);
  const r3dColor24 sel_col    = r3dColor24(255, 255, 255);
  const r3dColor24 hover_col  = r3dColor24(255, 255, 255);

  r3dColor col = normal_col;

  if(hovering) {
    // hovering
    //y += 1;
    col = hover_col;
  }

  if(selected) {
    char name[256];
    sprintf(name, "HL_%s", btnname);
    ui_elm->Draw(name, x, y, sel_col);
    
    return;
  }
  
  ui_elm->Draw(btnname, x, y, col);
  return;
}
