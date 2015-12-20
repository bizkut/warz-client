#ifndef LEVEL_DECORATORS_H
#define LEVEL_DECORATORS_H

struct DecoratorParams 
{
	r3dSTLString className;
	r3dSTLString fileName;
	r3dSTLString uiName;
	float spacing;
	float rotateVar;	

	DecoratorParams();

	void save(FILE* f) const;
	void load(r3dFile* f);
};

struct BrushParams 
{
	BrushParams();
	float radius;
};

struct BrushGrid
{
	struct Cell : public r3dgameVector(gobjid_t)
	{
		int x, y;

		Cell():filled(false), x(0), y(0) {}

		bool filled;
		void eraseObjects();

		void save(FILE* f) const;
		void load(r3dFile* f);
	};
	
	BrushGrid();
	Cell& cell(int x, int y);
	Cell& cell(const r3dPoint3D& pos);

	Cell* find(int x, int y);

	void eraseCells(const r3dPoint3D& pos, float radius);

	void save(FILE* f) const;
	void load(r3dFile* f);
	
	r3dgameVector(Cell) data;
};

struct Brush 
{
	Brush();
	DecoratorParams& addDecorator();
	void deleteDecorator(int index);
	void draw(const BrushParams& params) const;
	void paint(const BrushParams& params);
	void erase(const BrushParams& params);

	void save(FILE* f) const;
	void load(r3dFile* f);

	r3dSTLString name;
	r3dgameVector(DecoratorParams) decorators;

	BrushGrid grid;
};

struct BrushCollection : public r3dgameVector(Brush)
{
	Brush& addBrush();
	void deleteBrush(int index);
	void save(FILE* f) const;
	void load(r3dFile* f);
};

struct DecoratorsEditor 
{
	DecoratorsEditor();

	void draw() const;
	void paint();
	void erase();
	bool active() const;

	void selectBrush(int index);

	void save(const char* fileName) const;
	void load(const char* fileName);
	void clear();

	Brush* currentBrush;
	BrushCollection brushes;
	BrushParams params;
};

void DrawDecoratorsUI();
void HandleDecoratorsDraw();

#endif
