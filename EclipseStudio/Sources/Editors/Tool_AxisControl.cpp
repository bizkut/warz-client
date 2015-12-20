#include "r3dPCH.h"
#include "r3d.h"

#include "gamecommon.h"

#include "Tool_AxisControl.h"

Tool_AxisControl* Tool_AxisControl::curAxisControl = NULL;
//float Tool_AxisControl::distance_ = 0.0f;

const float Tool_AxisControl::width = 0.25f;
const float Tool_AxisControl::extendAxis = 5.0f;
const float Tool_AxisControl::extendPlane = 1.0f;
const float ToolAxisScaleInv = 100.0f;

Tool_AxisControl::Tool_AxisControl()
{
    size_ =0;
    isDraggable_ = false;
    xFree_ =
    yFree_ = 
    zFree_ =
    0;

    sensitivity_ = 0.1f;

  selected_ = NULL;
  dragging_ = false;
}

bool Tool_AxisControl::isDraggable()
{
    return isDraggable_;
}

void Tool_AxisControl::Draw()
{
  if(dragging_)
    return;

    r3dBoundBox xAxis, yAxis, zAxis;
    r3dBoundBox xyPlane, xzPlane, yzPlane;

	float fScale = r3dTL::Max ( 1.0f, ( gCam - position_ ).Length () / ToolAxisScaleInv );

    xAxis.Org = 
    yAxis.Org = 
    zAxis.Org = 
    xyPlane.Org = 
    xzPlane.Org = 
    yzPlane.Org =
    position_;

    zAxis.Org.z -= fScale * extendAxis;

    xAxis.Size = fScale*r3dVector(extendAxis, width, width);
    yAxis.Size = fScale*r3dVector(width, extendAxis, width);
    zAxis.Size = fScale*r3dVector(width, width, extendAxis*2.0f);

    xyPlane.Size = fScale*r3dVector(extendPlane, extendPlane, width);
    xzPlane.Size = fScale*r3dVector(extendPlane, width, extendPlane);
    yzPlane.Size = fScale*r3dVector(width, extendPlane, extendPlane);
    
    //r3dColor24 xTargetColor( 210,   0,   0, 200);
    //r3dColor24 yTargetColor(   0, 210,   0, 200);
    //r3dColor24 zColor(   0,   0, 210, 200);

    r3dColor24 xDefaultColor( 85, 0, 0);
    r3dColor24 yDefaultColor(   0, 85, 0);
    r3dColor24 zDefaultColor( 0, 0,  85);
    
    r3dColor24 addColor(85,85,85);

    if( xFree_ )
    {
        xDefaultColor = xDefaultColor + addColor;

        if( isDraggable() )
            xDefaultColor = xDefaultColor + addColor;
    }

    r3dDrawLine3D(  fScale*r3dVector(  0.0,  0.0, width/2.0f ) + position_, position_ + fScale*r3dVector(extendAxis, 0.0,0.0), gCam, fScale*width, xDefaultColor);

    //r3dDrawBoundBox( xAxis, gCam, xDefaultColor);
   
    if( zFree_ )
    {
        zDefaultColor = zDefaultColor + addColor;

        if( isDraggable() )
            zDefaultColor = zDefaultColor + addColor;
    }
    //r3dDrawBoundBox( zAxis, gCam, zDefaultColor);
    //r3dDrawLine3D(  r3dVector( width/2.0 , 0.0, 0.0  ) + position_, position_ + r3dVector( 0.0,0.0, extendAxis), gCam, width, zDefaultColor);
    r3dDrawLine3D(  fScale*r3dVector( width/2.0f , 0.0, 0.0  ) + zAxis.Org, zAxis.Org + zAxis.Size , gCam, fScale*width, zDefaultColor);
	
    if( yFree_ )
    {
        yDefaultColor = yDefaultColor + addColor;

        if( isDraggable() )
            yDefaultColor = yDefaultColor + addColor;
    }

    r3dDrawLine3D( fScale*r3dVector( width/2.0f, 0.0,  0.0) + position_, position_ + fScale*r3dVector( 0.0,extendAxis,0.0), gCam, fScale*width, yDefaultColor);
    //r3dDrawBoundBox( yAxis, gCam, yDefaultColor );
    
    


}


void Tool_AxisControl::selectDirectMovement(GameObject* obj)
{
  assert(selected_ == NULL);
  
  r3dVector direct;
  r3dScreenTo3D( ( float )imgui_mx, ( float ) imgui_my, &direct);

  if(!selected_) {
    float dist;
    if(obj->GetBBoxWorld().ContainsRay(gCam, direct, 10000.0f, &dist) && imgui_lbp) {
      selected_   = obj;
      mx1_        = imgui_mx;
      my1_        = imgui_my;
      savedFlags_ = obj->ObjFlags;
    }
    return; 
  }
  
}

bool Tool_AxisControl::processDirectMovement(GameObject* obj, r3dPoint3D* dragPosition, bool drawBBox )
{
  if(!selected_)
    return false;

  if(!imgui_lbp) {
    obj->ObjFlags = savedFlags_;
    selected_     = NULL;
    dragging_     = false;
    return true;
  }

  if( drawBBox )
  {
	r3dDrawBoundBox(obj->GetBBoxWorld(), gCam, r3dColor(0,255,0));
  }

  if(!dragging_) {
    float dist1 = (mx1_ - imgui_mx) * (mx1_ - imgui_mx) + (my1_ - imgui_my) * (my1_ - imgui_my);
    if(dist1 > 2.0f) {
      dragging_ = true;
      obj->ObjFlags |= OBJFLAG_SkipCastRay;
      
      r3dPoint3D scrv;
      r3dProjectToScreen(obj->GetPosition(), &scrv);
      ::SetCursorPos((int)scrv.x, (int)scrv.y);
    }

    return true;
  }
  
  //obj->SetPosition(UI_TargetPos);

  if( dragPosition )
  {
	  position_ = *dragPosition;
  }
  else
  {
	  extern r3dPoint3D UI_TargetPos;
	  position_ = UI_TargetPos;
  }
  
  return true;
}

bool Tool_AxisControl::Picked(GameObject* obj, int MouseX, int MouseY, bool shouldSelect, bool AssignAsActive , r3dPoint3D* objDragPos /*= NULL*/, bool drawBBoxIfMoved /*= false*/ )
{
  if(processDirectMovement(obj, objDragPos, drawBBoxIfMoved )) {
    return true;
  }

    r3dVector direct;
    r3dScreenTo3D( ( float ) MouseX, ( float ) MouseY, &direct);

	float fScale = r3dTL::Max ( 1.0f, ( gCam - position_ ).Length () / ToolAxisScaleInv );

    r3dBoundBox xAxis, yAxis, zAxis;
    r3dBoundBox xyPlane, xzPlane, yzPlane;
    xAxis.Org = 
    yAxis.Org = 
    zAxis.Org = 
    xyPlane.Org = 
    xzPlane.Org = 
    yzPlane.Org =
    position_;

    zAxis.Org.z -= fScale*extendAxis;

    xAxis.Size = fScale*r3dVector(extendAxis, width, width);
    yAxis.Size = fScale*r3dVector(width, extendAxis, width);
    zAxis.Size = fScale*r3dVector(width, width, extendAxis * 2.0f );

    xyPlane.Size = fScale*r3dVector(extendPlane, extendPlane, width);
    xzPlane.Size = fScale*r3dVector(extendPlane, width, extendPlane);
    yzPlane.Size = fScale*r3dVector(width, extendPlane, extendPlane);
    
    
    bool isSelected = false;
    if(!isDraggable())
    {
        oldMouseX_ = MouseX;
        oldMouseY_ = MouseY;
        
        oldCurrentPosition_ = position_;

        float distanceOut;
        xFree_ = (xAxis.ContainsRay(gCam, direct, 1000000.0f, &distanceOut))?true:false;
        distance_ = distanceOut;
        
        yFree_ = !xFree_ && yAxis.ContainsRay(gCam, direct, 1000000.0f, &distanceOut);
        distance_ = distanceOut;

        zFree_ = !yFree_ && zAxis.ContainsRay(gCam, direct, 1000000.0f, &distanceOut);
        distance_ = distanceOut;

        if(xFree_ || yFree_ || zFree_)
        {
            oldFinalPosition_ = position_;
            isDraggable_ = shouldSelect;
            isSelected = true;
        }
    }
    else
    {
        isDraggable_ = shouldSelect;
        updateMovement(MouseX, MouseY);
    }

    if(!xFree_ && !yFree_ && !zFree_) {
      selectDirectMovement(obj);
    }    
    
    return isSelected;
}

void Tool_AxisControl::updateMovement(int MouseX, int MouseY)
{
	float fScale = r3dTL::Max ( 1.0f, ( gCam - position_ ).Length () / ToolAxisScaleInv );

    r3dVector screenPos;
    r3dProjectToScreen( position_ , &screenPos);
    screenPos.z = 0.0f;

    r3dVector xDir = position_ + r3dVector(2.0,0.0,0.0);
    r3dProjectToScreen( xDir, &xDir);
    xDir.z = 0;
    xDir = xDir - screenPos;
    xDir.Normalize();
    
    r3dVector yDir = position_ + r3dVector(0.0,2.0,0.0);
    r3dProjectToScreen( yDir, &yDir);
    yDir.z = 0;
    yDir = yDir - screenPos;
    yDir.Normalize();

    r3dVector zDir = position_ + r3dVector(0.0,0.0,2.0);
    r3dProjectToScreen( zDir, &zDir);
    zDir.z = 0;
    zDir = zDir - screenPos;
    zDir.Normalize();

    float xDif = fScale * sensitivity_ * (MouseX - oldMouseX_);
    float yDif = fScale * sensitivity_ * (MouseY - oldMouseY_);

    oldMouseX_ = MouseX;
    oldMouseY_ = MouseY;

    //Font_Editor->PrintF( 200,200, r3dColor24::white, "%f = %d - %d \n %f = %d - %d", xDif, MouseX, oldMouseX_, yDif, MouseY, oldMouseY_);

    oldCurrentPosition_ = position_;

    float result;
    if( xFree_ )
    {
        result = xDir.x * xDif + xDir.y * yDif ;
        position_.x = position_.x + result;
    }

    if( yFree_ )
    {
        position_.y += (yDir.x *xDif  + yDir.y * yDif);
    }

    if( zFree_ )
    {
        position_.z += (zDir.x * xDif  + zDir.y * yDif );
    }

    updatePoints();

    
}

void Tool_AxisControl::updatePoints()
{
    for( int i = 0; i < size_; i++)
    {
        if(points_[i] != NULL) 
            *points_[i] = *points_[i] + getCurrentDifference();
    }
}

void Tool_AxisControl::setPosition(r3dPoint3D startPoint)
{
    position_ = startPoint;
}

void Tool_AxisControl::setSensitivity(float sen)
{
    sensitivity_ = sen;
}

float Tool_AxisControl::getSensitivity()
{
    return sensitivity_;
}

r3dPoint3D Tool_AxisControl::getPosition()
{
    return position_;
}

r3dPoint3D Tool_AxisControl::getCurrentDifference()
{
    return position_ - oldCurrentPosition_;
}

r3dPoint3D Tool_AxisControl::getFinalDifference()
{
    return position_ - oldFinalPosition_;
}

void Tool_AxisControl::attachPoint(r3dPoint3D *point)
{
    if(size_ < POINT_MAX )
    {
        points_[size_];
        size_++;
    }
}

float Tool_AxisControl::getDistance()
{
    return distance_;
}

void Tool_AxisControl::clearPoints()
{
    for(int i = 0; i < size_ ; i++)
    {
        points_[size_] = NULL;
    }
}

void Tool_AxisControl::AxisControlGroup::Add(Tool_AxisControl *tool)
{
    if( size_ < MAX)
    {
        items_[size_] = tool;
        size_++;
    }
}

void Tool_AxisControl::AxisControlGroup::Clear()
{

    for( int i = 0 ; i < size_; i++)
    {
        delete items_[i] ;
        items_[i] = NULL;
    };

    size_ = 0;


}

/*
void Tool_AxisControl::ProcessGroup(Tool_AxisControl::AxisControlGroup *Group, int MouseX, int MouseY, bool shouldSelect)
{
    float closestDistance = 100000.0f;
    
    for( int i = 0; i < Group->size_ ; i++)
    {  
        if( Group->items_[i]->Picked(MouseX, MouseY, shouldSelect))
        {
            //this is kinda messy, i'll fix it up later
            if(closestDistance > Group->items_[i]->getDistance() )
            {
                closestDistance > Group->items_[i]->getDistance();
                Group->active_ = Group->items_[i];
            }
        }   
    }
}
*/

bool Tool_AxisControl::AxisControlGroup::Process(int MouseX, int MouseY, bool shouldSelect)
{

    float closestDistance = 100000.0f;
    active_ = NULL;
    for( int i = 0; i < size_ ; i++)
    {  
        if( items_[i]->Picked(NULL, MouseX, MouseY, shouldSelect))
        {
            //this is kinda messy, i'll fix it up later
            if(closestDistance > items_[i]->getDistance() )
            {
                closestDistance = items_[i]->getDistance();
                active_ = items_[i];
            }
        }   
    }
    return active_ != NULL;
}

void Tool_AxisControl::AxisControlGroup::Draw()
{
    for(int i = 0;  i < size_ ; i++)
    {
        items_[i]->Draw();
    }
}
