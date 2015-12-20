#include "r3dPCH.h"
#include "r3d.h"

#include "UI/UIimEdit.h"
#include "UI/UIimEdit2.h"
#include "GameObjects/ObjManag.h"

#include "GameLevel.h"
#include "LevelEditor_Decorators.h"

extern	r3dPoint3D	UI_TargetPos;		// snapped to object position (object center)
extern r3dCamera gCam;

void r3dDrawLine3DT(const r3dPoint3D& p1, const r3dPoint3D& p2, r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex=NULL);
void r3dDrawCircle3DT(const r3dPoint3D& P1, const float Radius, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex=NULL);

float terra_GetH(const r3dPoint3D &vPos);

void GetRandomPos(r3dPoint3D& Pos, float Radius)
{
	r3dPoint3D dir;
	dir.X = r3dRandomFloat();
	dir.Z = r3dRandomFloat();
	dir.Y = 0.0f;
	dir.Normalize();

	Pos.X += dir.X * Radius*0.5f;
	Pos.Z += dir.Z * Radius*0.5f;
	Pos.Y = terra_GetH(Pos);	
}

void GetRandomScale(r3dPoint3D& Scale, float value)
{
	float v = 1.0f + r3dRandomFloat() * value;
	Scale.X = v;
	Scale.Z = v;
	Scale.Y = v;
}

void GetRandomRot(r3dPoint3D& Rot, float value)
{
	Rot.X = r3dRandomFloat() * 90;
	Rot.Z = 0;
	Rot.Y = 0;
}

BrushParams::BrushParams()
{
	radius = 2.0f;
}

//
//
//
BrushGrid::BrushGrid()
{

}

BrushGrid::Cell* BrushGrid::find(int x, int y)
{
	for (unsigned int i = 0; i < data.size(); ++i)
	{
		if(data[i].x == x && data[i].y == y)
			return &data[i];
	}

	return 0;
}

BrushGrid::Cell& BrushGrid::cell(int x, int y)
{
	BrushGrid::Cell* c = find(x, y);

	if(!c)
	{
		data.push_back(Cell());
		c = &data.back();
		c->x = x;
		c->y = y;
	}
	
	return *c;
}

BrushGrid::Cell& BrushGrid::cell(const r3dPoint3D& pos)
{
	return cell( (int)pos.x, (int)pos.z );
}

void BrushGrid::eraseCells(const r3dPoint3D& pos, float radius)
{
	int baseX = (int)pos.x;
	int baseY = (int)pos.z;

	int ir = (int)ceilf(radius);
	int minX = baseX - ir;
	int minY = baseY - ir;
	int maxX = baseX + ir;
	int maxY = baseY + ir;

	for (int i = minX; i <= maxX; ++i)
	{
		for (int j = minY; j <= maxY; ++j)
		{
			cell(i, j).eraseObjects();
		}
	}
}

void BrushGrid::Cell::eraseObjects()
{
	for (unsigned int i = 0; i < size(); ++i)
	{
		GameObject* gobj = GameWorld().GetObject((*this)[i]);
		if(gobj)
		{
			GameWorld().DeleteObject(gobj);
		}		
	}

	clear();
	filled = false;
}

void BrushGrid::Cell::save(FILE* f) const
{
	r3dgameVector(uint32_t) id;
	
	id.resize(size());
	for (unsigned int i = 0; i < size(); ++i)
	{
		GameObject* gobj = GameWorld().GetObject((*this)[i]);
		if(gobj)
		{
			id[i] = gobj->GetHashID();
		}	
	}

	int count = (int)id.size();
	fwrite(&count, sizeof(count), 1, f);
	if(count > 0)
	{
		fwrite(&id[0], sizeof(uint32_t), count, f);
	}
}

void BrushGrid::Cell::load(r3dFile* f)
{
	r3dgameVector(uint32_t) id;
	int count;
	fread(&count, sizeof(count), 1, f);

	if(count > 0)
	{
		id.resize(count);
		fread(&id[0], sizeof(uint32_t), count, f);

		resize(count);
		for (unsigned int i = 0; i < id.size(); ++i)
		{
			GameObject* gobj = GameWorld().GetObjectByHash(id[i]);
			if(gobj)
			{
				(*this)[i] = gobj->GetSafeID();
			}		
		}
	}	
}

void BrushGrid::save(FILE* f) const
{
	int count = (int)data.size();
	fwrite(&count, sizeof(count), 1, f);
	for(unsigned int i = 0; i < data.size(); ++i)
	{
		data[i].save(f);
	}
}

void BrushGrid::load(r3dFile* f)
{
	int count;
	fread(&count, sizeof(count), 1, f);
	data.resize(count);
	for(unsigned int i = 0; i < data.size(); ++i)
	{
		data[i].load(f);
	}
}
//
//
//
DecoratorParams::DecoratorParams()
{
	rotateVar = 90.0f;
	spacing = 10.f;
}

void DecoratorParams::save(FILE* f) const
{
	{
		int len = (int)className.size();
		fwrite(&len, sizeof(len), 1, f);
		fwrite(&className[0], sizeof(char), className.size(), f);
	}

	{
		int len = (int)fileName.size();
		fwrite(&len, sizeof(len), 1, f);
		fwrite(&fileName[0], sizeof(char), fileName.size(), f);
	}	

	fwrite(&spacing, sizeof(spacing), 1, f);
	fwrite(&rotateVar, sizeof(rotateVar), 1, f);
	//fwrite(&scaleVar, sizeof(scaleVar), 1, f);
}

void DecoratorParams::load(r3dFile* f)
{
	char buf[256];
	buf[0] = 0;
	int len;
	fread(&len,sizeof(len), 1, f);
	if(len > 0)
	{
		fread(&buf[0], sizeof(char), len, f);
	}
	buf[len] = 0;
	className = buf;

	buf[0] = 0;
	fread(&len,sizeof(len), 1, f);
	if(len > 0)
	{
		fread(&buf[0], sizeof(char), len, f);
	}
	buf[len] = 0;
	fileName = buf;

	fread(&spacing, sizeof(spacing), 1, f);
	fread(&rotateVar, sizeof(rotateVar), 1, f);
	//fread(&scaleVar, sizeof(scaleVar), 1, f);
}


//
//
//
Brush::Brush()
{
	name = "default";

// 	{
// 		DecoratorParams& decorator = addDecorator();
// 		decorator.className = "obj_Building";
// 		decorator.fileName = "Data\\ObjectsDepot\\Desert_Plants\\CR_Plant_Bush_01.sco";
// 		decorator.uiName = "Desert_Plants\\CR_Plant_Bush_01.sco";
// 	}
	
	{
		DecoratorParams& decorator = addDecorator();
		decorator.className = "obj_Building";
		decorator.fileName = "Data\\ObjectsDepot\\Desert_Plants\\CR_Des_Rock_02.sco";
		decorator.uiName = "Desert_Plants\\CR_Des_Rock_02.sco";
	}
}

void Brush::save(FILE* f) const
{
	grid.save(f);

	int count = (int)decorators.size();
	fwrite(&count, sizeof(count), 1, f);
	for(unsigned int i = 0; i < decorators.size(); ++i)
	{
		decorators[i].save(f);
	}
}

void Brush::load(r3dFile* f)
{
	grid.load(f);

	int count;
	fread(&count, sizeof(count), 1, f);
	decorators.resize(count);
	for(unsigned int i = 0; i < decorators.size(); ++i)
	{
		decorators[i].load(f);
	}
}

DecoratorParams& Brush::addDecorator()
{
	decorators.push_back(DecoratorParams());
	return decorators.back();
}

void Brush::deleteDecorator(int index)
{
	int s = (int)decorators.size();

	if(index<0 || index >= s)
		return;

	decorators.erase( decorators.begin() + index ); 
}

void Brush::draw(const BrushParams& params) const
{
#ifndef FINAL_BUILD
	// draw circle and line height of human
	r3dDrawCircle3DT(UI_TargetPos, params.radius, gCam, 0.5f, r3dColor(100,255,100));
	r3dDrawLine3D(UI_TargetPos, UI_TargetPos+r3dPoint3D(0,2,0), gCam, 0.2f, r3dColor(255,155,0));
#endif
}

void Brush::paint(const BrushParams& params)
{
	r3dPoint3D pos = UI_TargetPos;
	BrushGrid::Cell& cell = grid.cell(pos);

	if(!cell.filled)
	{
		r3dBoundBox brushBox;
		brushBox.Size.x = brushBox.Size.y = brushBox.Size.z = params.radius*2.5f;
		brushBox.Org = pos - brushBox.Size * 0.5f;
		

		int objectsCount = 0;
		GameObject** objects = 0;
		GameWorld().GetObjectsInCube(brushBox, objects, objectsCount);

		for (unsigned int i = 0; i < decorators.size(); ++i)
		{
			DecoratorParams& decorator = decorators[i];

			GetRandomPos(pos, params.radius);
			GameObject* obj = srv_CreateGameObject(decorator.className.c_str(), decorator.fileName.c_str(), pos);

			r3dPoint3D p3;
			GetRandomRot(p3, R3D_PI);
			obj->SetRotationVector(p3);

			r3dBoundBox spacingBox;
			spacingBox.Size.x = spacingBox.Size.y = spacingBox.Size.z = decorator.spacing*2.0f;
			spacingBox.Org = pos - spacingBox.Size * 0.5f;

			int objectsCountForSpacing = 0;
			GameObject** objectsForSpacing = 0;

			GameWorld().GetObjectsInCube(spacingBox, objectsForSpacing, objectsCountForSpacing);

			float nearestDistSq = FLT_MAX;
			for (int k = 0; k < objectsCountForSpacing; ++k)
			{
				GameObject* o = objectsForSpacing[k];
				if(o->Name == obj->Name)
				{
					r3dPoint3D d = pos - o->GetPosition();
					float lenSq = d.LengthSq();
					if(lenSq < nearestDistSq)
					{
						nearestDistSq = lenSq;
						if(nearestDistSq < decorator.spacing * decorator.spacing)
							break;
					}
				}				
			}
					
			bool isBad = nearestDistSq < decorator.spacing * decorator.spacing;
/*
			if(!isBad)
			{
// 				for (int j = 0; j < 100; ++j)
 				{
					
// 					GetRandomPos(pos, params.radius);
// 					obj->SetPosition(pos); 				

					const r3dBoundBox& testBox = obj->GetBBoxWorld();
					isBad = false;
					for (int k = 0; k < objectsCount; ++k)
					{
						const r3dBoundBox& bbox = objects[k]->GetBBoxWorld();
						isBad |= bbox.Intersect(testBox) || testBox.ContainsBox(bbox) || bbox.ContainsBox(testBox);
					}

					if(!isBad)
						break;
				}
			}*/


			if(isBad)
			{
				GameWorld().DeleteObject(obj);
			}
			else
			{
				cell.push_back(obj->GetSafeID());
			}			
		}	

		cell.filled = true;
	}
}

void Brush::erase(const BrushParams& params)
{
	r3dPoint3D pos = UI_TargetPos;
	grid.eraseCells(pos, params.radius);
}

typedef r3dgameVector(r3dSTLString) stringlist_t;

extern int				ObjCatInit;
extern int				NumCategories;

struct  CategoryStruct
{
	int			Type;
	float		Offset;
	int			NumObjects;
	stringlist_t	ObjectsClasses;
	stringlist_t	ObjectsNames;
};

typedef r3dgameVector(CategoryStruct)  catlist;
extern catlist  ObjectCategories;
extern stringlist_t 	CatNames;
extern float			CatOffset;

static DecoratorsEditor editor;

void InitObjCategories();


void GetBrushesList(const BrushCollection& bc, stringlist_t& brushesList)
{
	brushesList.clear();
	brushesList.reserve(bc.size());
	for (unsigned int i = 0; i < bc.size(); ++i)
	{
		brushesList.push_back(bc[i].name);
	}
}

void GetDecoratorsList(const Brush& brush, stringlist_t& decList)
{
	decList.clear();
	decList.reserve(brush.decorators.size());
	for (unsigned int i = 0; i < brush.decorators.size(); ++i)
	{
		decList.push_back(brush.decorators[i].uiName);
	}
}

void DrawBrushSettings(float& SliderX, float& SliderY)
{
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Brush Radius", &editor.params.radius, 1.0f, 100.0f, "%.2f"  );
}

void DrawDecoratorsChoose(bool flag);

void DrawBrushesList(float& SliderX, float& SliderY)
{
#ifndef FINAL_BUILD
	const float DEFAULT_CONTROLS_WIDTH = 360.0f;
	const float DEFAULT_CONTROLS_HEIGHT = 22.0f;

	DrawBrushSettings(SliderX, SliderY);

	stringlist_t brushes;
	GetBrushesList(editor.brushes, brushes);

	if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Add Brush"))
	{
		editor.brushes.addBrush();
	}
	SliderY += DEFAULT_CONTROLS_HEIGHT;

	if(!brushes.empty())
	{
		static float groupOffset, groupOffset2;
		static int brushIdx = -1;

		if(brushIdx >= 0)
		{
			imgui2_StartArea("area222", SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT);
			char name[128];
			r3dscpy(name, brushes[brushIdx].c_str());
			imgui2_StringValue("Name:", name);
			brushes[brushIdx] = name;
			editor.brushes[brushIdx].name = name;
			SliderY += 22;
			imgui2_EndArea();
			SliderY += 30;
		}
		

		SliderY += imgui_Static(SliderX, SliderY, "Brushes", 200);
		imgui_DrawList( SliderX, SliderY, 360, 100, brushes, &groupOffset, &brushIdx);
		SliderY += 105;

		if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Delete Brush"))
		{
			editor.brushes.deleteBrush(brushIdx);
			GetBrushesList(editor.brushes, brushes);
			int ns = (int)brushes.size() - 1;
			if(brushIdx > ns)
				brushIdx = ns;
		}
		SliderY += DEFAULT_CONTROLS_HEIGHT*2;

		static int decIdx = 0;
		if(brushIdx != -1)
		{
			editor.selectBrush(brushIdx);

			static int choose = 0;
			SliderY += imgui_Checkbox(SliderX, SliderY, "Available Decorators", &choose, 1);
			DrawDecoratorsChoose(choose!=0);

			Brush& brush = editor.brushes[brushIdx];
			stringlist_t decorators;
			GetDecoratorsList(brush, decorators);
			SliderY += imgui_Static(SliderX, SliderY, "Decorators", 200);
			imgui_DrawList( SliderX, SliderY, 360, 100, decorators, &groupOffset2, &decIdx);
			SliderY += 105;

			if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Delete Decorator"))
			{
				brush.deleteDecorator(decIdx);
				GetDecoratorsList(brush, decorators);
				int ns = (int)decorators.size() - 1;
				if(decIdx > ns)
					decIdx = ns;
			}
			SliderY += DEFAULT_CONTROLS_HEIGHT;

			if(decIdx != -1)
			{
				Brush& brush = editor.brushes[brushIdx];
				DecoratorParams& decorator = brush.decorators[decIdx];
				//SliderY += imgui_Value_Slider( SliderX, SliderY, "Density", &decorator.density, 0.0f, 100.0f, "%.2f"  );
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Rotation Var", &decorator.rotateVar, 0.0f, 180.0f, "%.2f" );
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Spacing Var", &decorator.spacing, 0.0f, 100.0f, "%.2f" );
				SliderY += 5;
			}
		} 
	}

	char path[512];
	sprintf(path, "%s%s", r3dGameLevel::GetHomeDir(), "/test.brushes");

	if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Save"))
	{
		editor.save(path);
	}
	SliderY += DEFAULT_CONTROLS_HEIGHT;

	if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Load"))
	{
		editor.load(path);
	}
	SliderY += DEFAULT_CONTROLS_HEIGHT;

	if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Clear"))
	{
		editor.clear();
	}
	SliderY += DEFAULT_CONTROLS_HEIGHT;
#endif
}

void DrawDecoratorsChoose(bool flag)
{
#ifndef FINAL_BUILD
	if(!flag)
		return;

	float SliderX = r3dRenderer->ScreenW-375-375;
	float SliderY = 50;

	const float DEFAULT_CONTROLS_WIDTH = 360.0f;
	const float DEFAULT_CONTROLS_HEIGHT = 22.0f;

	//
	static char CategoryName[64] = "";
	static char ClassName[64] = "";
	static char FileName[64] = "";

	char Str[256];

	// 	float SliderX = r3dRenderer->ScreenW-375;
	// 	float SliderY = 50;

	InitObjCategories();

	stringlist_t ClassNames;
	ClassNames.push_back( "default" );
	ClassNames.push_back( "obj_Building" );

	static float groupOffset;
	static int classNameID = 0;
	imgui_DrawList( SliderX, SliderY, 360, 100, ClassNames, &groupOffset, &classNameID );
	SliderY += 100;

	if ( classNameID <= 1 )
	{
		SliderY += imgui_Static(SliderX, SliderY, CategoryName, 200);
		static int idx = -1;
		if (imgui_DrawList(SliderX, SliderY, 360, 200, CatNames, &CatOffset, &idx))
		{
			sprintf(CategoryName, "%s", CatNames.at(idx).c_str());

			sprintf(ClassName,"");
			sprintf(FileName, "");
			if(ObjectCategories.at(idx).ObjectsClasses.size() > 0)
			{
				sprintf(ClassName, "%s", ObjectCategories.at(idx).ObjectsClasses.at(0).c_str());
				sprintf(FileName, "%s", ObjectCategories.at(idx).ObjectsNames.at(0).c_str());
			}
		}

		SliderY += 210;

		SliderY += imgui_Static(SliderX, SliderY, FileName);

		static int idx1 = 0;
		if (idx != -1)
		{
			if (imgui_DrawList(SliderX, SliderY, 360, 200, ObjectCategories.at(idx).ObjectsNames , &ObjectCategories.at(idx).Offset, &idx1))
			{
				sprintf (ClassName,"%s", ObjectCategories.at(idx).ObjectsClasses.at(idx1).c_str());
				sprintf (FileName,"%s", ObjectCategories.at(idx).ObjectsNames.at(idx1).c_str());
			}
			SliderY += 205;

			if(ClassName[0] == 0 || FileName[0] == 0)
				return;

			sprintf (Str,"Data\\ObjectsDepot\\%s\\%s", CategoryName, FileName);
			r3dSTLString fileName = Str;

			sprintf (Str,"%s\\%s", CategoryName, FileName);
			r3dSTLString uiName = Str;

			if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Add Decorator"))
			{
				DecoratorParams& decorator = editor.currentBrush->addDecorator();
				decorator.className = ClassName;
				decorator.fileName = fileName;
				decorator.uiName = uiName;
			}
			SliderY += DEFAULT_CONTROLS_HEIGHT;
		}
	}
#endif
}

void DrawDecoratorsUI()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 50;

	DrawBrushesList(SliderX, SliderY);
	return;
}

//
//
//
Brush& BrushCollection::addBrush()
{
	push_back(Brush());
	return back();
}

void BrushCollection::deleteBrush(int index)
{
	int s = (int)size();

	if(index<0 || index >= s)
		return;

	erase( begin() + index );
}

//
//
//
void BrushCollection::save(FILE* f) const
{
	int count = (int)size();
	fwrite(&count, sizeof(count), 1, f);
	for (const_iterator it = begin(); it != end(); ++it)
	{
		it->save(f);
	}
}

void BrushCollection::load(r3dFile* f)
{
	int count;
	fread(&count, sizeof(count), 1, f);
	resize(count);
	for (iterator it = begin(); it != end(); ++it)
	{
		it->load(f);
	}
}


//
//
//
DecoratorsEditor::DecoratorsEditor()
:currentBrush(0)
{
	Brush& brush = brushes.addBrush();
	currentBrush = &brush;
}

void DecoratorsEditor::draw() const
{
	if(currentBrush)
		currentBrush->draw(params);
}

void DecoratorsEditor::selectBrush(int index)
{
	if(index >=0 && index < (int)brushes.size())
		currentBrush = &brushes[index];
}

void DecoratorsEditor::paint()
{
	if(currentBrush)
		currentBrush->paint(params);
}

void DecoratorsEditor::erase()
{
	if(currentBrush)
		currentBrush->erase(params);
}

bool DecoratorsEditor::active() const
{
	return currentBrush != 0; 
}

void DecoratorsEditor::save(const char* fileName) const
{
	FILE* f = fopen(fileName, "wb");
	brushes.save(f);
	fclose(f);
}

void DecoratorsEditor::load(const char* fileName)
{
	r3dFile* f = r3d_open(fileName, "rb");
	brushes.load(f);
	fclose(f);
}

void DecoratorsEditor::clear()
{
	brushes.clear();
}


void HandleDecoratorsDraw()
{
	if(editor.active())
	{
		int iEditMode = 0;

		if (Keyboard->IsPressed(kbsLeftControl)) 
			iEditMode = 1;

		if(iEditMode)
		{
			editor.draw();
			
			bool eraseMode = Keyboard->IsPressed(kbsLeftAlt) && imgui_lbp;

			if (imgui_lbp && !eraseMode)
			{
				editor.paint();
			}
			else if(eraseMode)
			{
				editor.erase();
			}
		}
	}
}