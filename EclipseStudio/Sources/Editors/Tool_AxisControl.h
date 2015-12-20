#pragma once 
#include "r3dPCH.h"
#include "r3d.h"
#include "r3dPoint.h"


class Tool_AxisControl;
class AxisControlGroup;


class Tool_AxisControl
{
public:

    

    class AxisControlGroup
    {
    public:
        AxisControlGroup()
        {
            size_ = 0;
            active_ = NULL;
        };

        ~AxisControlGroup()
        {
            Clear();
        };

        Tool_AxisControl* getActive()
        {
            return active_;
        };

        void Add(Tool_AxisControl *tool);

        void Clear();

        void Draw();

        bool Process(int MouseX, int MouseY, bool shouldSelect);



        static const int MAX = 50;
        friend class Tool_AxisControl;
    private:

        Tool_AxisControl* items_[MAX];
        int size_;
        Tool_AxisControl* active_;



    };

    Tool_AxisControl();
    void Draw();
    bool Picked(GameObject* obj, int MouseX, int MouseY, bool shouldSelect, bool AssignAsActive = true, r3dPoint3D* objDragPos = NULL, bool drawBBoxIfMoved = false );
    
    void setPosition(r3dPoint3D startPoint);
    void setSensitivity(float sen);

    r3dPoint3D getPosition();
    r3dPoint3D getCurrentDifference();
    r3dPoint3D getFinalDifference();
    float getSensitivity();
    float getDistance();

    void attachPoint(r3dPoint3D* point);
    void clearPoints();

    
    bool isDraggable();


private:
    void updateMovement(int MouseY, int MouseX);
    void updatePoints();

    float distance_;

    bool isDraggable_;
    bool xFree_, yFree_, zFree_;
    int oldMouseX_, oldMouseY_;

    r3dPoint3D position_;

    r3dPoint3D oldFinalPosition_;
    r3dPoint3D oldCurrentPosition_;

    static Tool_AxisControl* curAxisControl;
    
    static const int POINT_MAX = 100;
    r3dPoint3D* points_[POINT_MAX];
    int size_;

    static const float width;
    static const float extendAxis;// = 100.0f;
    static const float extendPlane;// = 10.0f;
    float sensitivity_; 


    static Tool_AxisControl::AxisControlGroup* groups_[300];
    static void addGroupContainer(AxisControlGroup*);

	GameObject*	selected_;		// world drag: pointer to object
	bool		dragging_;		// world drag: if dragging now
	float		mx1_, my1_;
	DWORD		savedFlags_;
    
	bool		processDirectMovement(GameObject* obj, r3dPoint3D* dragPosition, bool drawBBox );
	void		selectDirectMovement(GameObject* obj);
};
