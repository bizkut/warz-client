#pragma once

typedef r3dgameVector(r3dSTLString) stringlist_t;

class r3dTimeGradient2;

        bool            imgui2_DrawHSlider(float x, float y, float width, float minval, float maxval, float* edit_val, bool bUseDesktop = true);
        bool            imgui2_DrawVSlider(float x, float y, float height, float minval, float maxval, float* edit_val, bool bUseDesktop = true);
        
        void            imgui2_StartArea(const char* name, float x, float y, float w, float h, bool drawBorder = false);
        void            imgui2_EndArea();
        void            imgui2_AdvanceY(float dy);
        void            imgui2_Indent(float idx = 0);

    // float/int value editing
        bool            imgui2_Value(const char* name, float* edit_val, float minval, float maxval, const char* fmt, int doslider = true, bool bUseDesktop = true);
        bool            imgui2_Value(const char* name, int* edit_val, float minval, float maxval, const char* fmt, int doslider = true, bool bUseDesktop = true);
        bool             imgui2_ValueEx(float x, float y, float w, const char* name, void* edit_val, bool isFloat, float minval, float maxval, const char* fmt, int doslider = true, bool bUseDesktop = true);
    // integer choise 
        bool            imgui2_ListValue(const char* name, int* edit_val, const char* list[], int numlist, bool bUseDesktop = true);
        bool             imgui2_ListValueEx(float x, float y, float w, const char* name, int* edit_val, const char* list[], int numlist, bool bUseDesktop = true);
        // string editing

        bool	        imgui2_StringValue(const char* name, char* value, bool allowEdit = true, bool bUseDesktop = true );
        bool	         imgui2_StringValueEx(float x, float y, float w, const char* name, char* value, bool allowEdit = true, bool bUseDesktop = true );


        bool            imgui2_Label(const char* name, ...);
        bool            imgui2_Static(const char* name, ...);
		bool            imgui2_Static_Checked(const char* name, ...);
        bool             imgui2_StaticEx(float x, float y, float w, bool bChecked, bool bUseDesktop, const char* name, ...);
        bool            imgui2_Checkbox(const char* name, int* edit_val, const DWORD flag = 0x1, bool bUseDesktop = true );
        bool             imgui2_CheckboxEx(float x, float y, float w, const char* name, int* edit_val, const DWORD flag = 0x1, bool bUseDesktop = true);
        bool            imgui2_Button(const char* name, int selected = false, bool bUseDesktop = true);
        bool             imgui2_ButtonEx(float sx, float sy, float sw, float sh, const char* name, int selected = false, bool bUseDesktop = true);

        bool            imgui2_Toolbar(int* edit_val, int MinIdx, const char* list[], int numlist, bool bUseDesktop = true);
        bool             imgui2_ToolbarEx(float sx, float sy, float sw, float sh, int *edit_val, int MinIdx, const char* list[], int numlist, bool bUseDesktop = true);

        bool            imgui2_DrawList( float sx, float sy, float sw, float sh, const stringlist_t& names, int CurSelection, float* arr_offset, int* edit_val, bool bUseDesktop = true);
        bool            imgui2_DirList(float sw, float sh, const char* Dir, char* edit_val, bool bUseDesktop = true);
		bool            imgui2_FileList(float sw, float sh, const char* Dir, char* edit_val, bool bUseDesktop = true);

        bool            imgui2_DrawColorPicker(float x, float y, const char* name, r3dColor* edit_val, float width, bool withAlpha, bool bUseDesktop = true) ;
        bool            imgui2_DrawColorGradient(const char* name, r3dTimeGradient2* edit_val, bool bUseDesktop = true);
        bool             imgui2_DrawColorGradientEx(float x, float y, const char* name, r3dTimeGradient2* edit_val, float width, bool bUseDesktop = true);
        bool            imgui2_DrawFloatGradient(const char* name, r3dTimeGradient2* edit_val, float* pVertScale, float minVal, float maxVal, bool bUseDesktop = true);
        bool             imgui2_DrawFloatGradientEx(float x, float y, const char* name, r3dTimeGradient2* edit_val, float* pVertScale, float in_w, float in_h, float minVal, float maxVal, bool bUseDesktop = true);

        void            imgui2_Update();

		bool			imgui2_IsCursorOver2d();