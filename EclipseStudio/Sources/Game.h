
enum EnumRenderSceneType
{
 Render_None		= 0,
 Render_SceneSimple	= (1 << 1 ),
 Render_SceneAdvanced	= (1 << 2 ),
 Render_FOV		= (1 << 3 ),
 Render_Bloom		= (1 << 4 ),
 Render_NVG		= (1 << 5 ),
 Render_Thermal		= (1 << 6 ),
 Render_FogFilter	= (1 << 7 ),
 Render_FilmGrain	= (1 << 8 ),

};

extern int GameRenderFlags;



void menu_MessageBox(char *Text);


int Splat_Init(char *FName, int NumHoles);
int Splat_Destroy();
int Splat_Add(r3dFace *face, r3dVector &Normal, r3dPoint3D &Pos, float Size, int Type, r3dColor24 C);
void Splat_Draw(r3dCamera &Cam);

int BHole_Init(char *FName, int NumHoles);
int BHole_Destroy();
void BHole_Draw(r3dCamera &Cam);
int BHole_Add(r3dFace *face, r3dVector &Normal, r3dPoint3D &Pos, float Size, int Type, r3dColor24 C);



void LM_Set(float F, char *Str);
void LM_Add(float F, char *Str);
void LM_Init();
void LM_Unload();
void LM_DrawStart();


extern char CurLoadString[100];

#endif __GAME_H
