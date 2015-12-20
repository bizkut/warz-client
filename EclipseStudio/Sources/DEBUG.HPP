namespace debug
{
	// screen output
	r3dPoint3D	scr_StartPos(0, 30, 0);
	int		scr_Lines;
	float		scr_LastFrame;
	r3dColor	scr_Clr1(255, 255, 0);
	r3dColor	scr_Clr2(255, 255, 255);

	int		scr_OutEnabled = 1;

	void  		Out(char *fmt, ...);

	// keyboard
	typedef struct
	{
	  char		Key;
	  int		kbsCode;
	  int		Used;
	} keytable_t;
extern	keytable_t	keys_Table[];
extern	int		keys_Entries;
	float		keys_LastFrame;

	void		SetKeyAsUsed(int kbsCode);
	int		GetFreeKey(int kbsWantedCode = 0);
	char		KbsToChar(int kbsCode);

	enum EControlTypes
	{
	  CNTRL_Bool,
	  CNTRL_Add,
	  CNTRL_AddCont,
	  CNTRL_Mul,
	  CNTRL_MulCont,
	};
	void		ControlVariable(int Type, int *var, int kbsKey1, int kbsKey2 = 0, int delta = 0);
	void		ControlVariable(int Type, float *var, int kbsKey1, int kbsKey2 = 0, float delta = 0);

	// debug
};


	debug::keytable_t debug::keys_Table[] = {
	  '1', kbs1, 0, 
	  '2', kbs2, 0, 
	  '3', kbs3, 0, 
	  '4', kbs4, 0, 
	  '5', kbs5, 0, 
	  '6', kbs6, 0, 
	  '7', kbs7, 0, 
	  '8', kbs8, 0, 
	  '9', kbs9, 0, 
	  '0', kbs0, 0, 
	  '-', kbsMinus, 0,
	  '+', kbsEquals, 0,
	  'R', kbsR, 0, 
	  'T', kbsT, 0, 
	  'Y', kbsY, 0, 
	  'U', kbsU, 0, 
	  'I', kbsI, 0, 
	  'O', kbsO, 0, 
	  'P', kbsP, 0, 
	  'F', kbsF, 0, 
	  'G', kbsG, 0, 
	  'H', kbsH, 0, 
	  'J', kbsJ, 0, 
	  'K', kbsK, 0, 
	  'L', kbsL, 0, 
	  'V', kbsV, 0, 
	  'B', kbsB, 0, 
	  'N', kbsN, 0, 
	  'M', kbsM, 0, 
	};
	int	debug::keys_Entries = sizeof(debug::keys_Table) / sizeof(debug::keys_Table[0]);

void debug::SetKeyAsUsed(int kbsCode)
{
	int	i;

  for(i=0; i<keys_Entries; i++) {
    if(keys_Table[i].kbsCode == kbsCode) {
      keys_Table[i].Used |= 0x2;
      return;
    }
  }

  return;
}

int debug::GetFreeKey(int kbsWantedCode)
{
	int	i;

  // if we are on new frame, reset free keys table
  if(keys_LastFrame != timeGetTime()) {
    for(i=0; i<keys_Entries; i++) 
      keys_Table[i].Used &= ~1;

    keys_LastFrame = timeGetTime();
  }

  // check for wanted code
  for(i=0; i<keys_Entries; i++) {
    if(keys_Table[i].kbsCode == kbsWantedCode && keys_Table[i].Used == 0)
      return keys_Table[i].kbsCode;
  }

  // check for free code
  for(i=0; i<keys_Entries; i++) {
    if(keys_Table[i].Used == 0) {
      keys_Table[i].Used |= 1;
      return keys_Table[i].kbsCode;
    }
  }
  return 0;
}

char debug::KbsToChar(int kbsCode)
{
	int	i;

  for(i=0; i<keys_Entries; i++) {
    if(keys_Table[i].kbsCode == kbsCode) {
      return keys_Table[i].Key;
    }
  }

  return '?';
}


void debug::Out(char *fmt, ...)
{
	char 		buf[512];
	r3dColor24	scr_Clr;

  if(!scr_OutEnabled)
    return;

  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);

  if(buf[0] == '-') {
    scr_Lines++;
    scr_StartPos.X = 0;
    scr_Clr = scr_Clr2;
  } else {
    scr_StartPos.X = 20;
    scr_Clr = scr_Clr1;
  }

  if(scr_LastFrame != timeGetTime()) {
    scr_LastFrame = timeGetTime();
    scr_Lines     = 0;
  }

  Font_Label->PrintF(scr_StartPos.X, scr_StartPos.Y + (++scr_Lines) * 20, scr_Clr, buf);
}


void debug::ControlVariable(int Type, int *var, int kbsKey1, int kbsKey2, int delta)
{
	int	bShift = Keyboard->IsPressed(kbsLeftShift) || Keyboard->IsPressed(kbsRightShift);

  switch(Type) {
   case CNTRL_Bool:
     if(Keyboard->WasPressed(kbsKey1)) {
       if(*var == 0) *var = 1;
       else          *var = 0;
     }
     break;
   case CNTRL_Add:
     if(kbsKey2) {
       if(Keyboard->WasPressed(kbsKey2)) *var += delta;
       if(Keyboard->WasPressed(kbsKey1)) *var -= delta;
     } else {
       if(Keyboard->WasPressed(kbsKey1)) bShift ? *var -= delta : *var += delta;
     }
     break;
   case CNTRL_AddCont:
     if(kbsKey2) {
       if(Keyboard->IsPressed(kbsKey2)) *var += delta;
       if(Keyboard->IsPressed(kbsKey1)) *var -= delta;
     } else {
       if(Keyboard->IsPressed(kbsKey1)) bShift ? *var -= delta : *var += delta;
     }
     break;
   case CNTRL_Mul:
     if(kbsKey2) {
       if(Keyboard->WasPressed(kbsKey2)) *var += delta;
       if(Keyboard->WasPressed(kbsKey1)) *var -= delta;
     } else {
       if(Keyboard->WasPressed(kbsKey1)) bShift ? *var /= delta : *var *= delta;
     }
     break;
   case CNTRL_MulCont:
     if(kbsKey2) {
       if(Keyboard->IsPressed(kbsKey2)) *var += delta;
       if(Keyboard->IsPressed(kbsKey1)) *var -= delta;
     } else {
       if(Keyboard->IsPressed(kbsKey1)) bShift ? *var /= delta : *var *= delta;
     }
     break;
  }
  return;
}


void debug::ControlVariable(int Type, float *var, int kbsKey1, int kbsKey2, float delta)
{
	int	bShift = Keyboard->IsPressed(kbsLeftShift) || Keyboard->IsPressed(kbsRightShift);

  switch(Type) {
   case CNTRL_Bool:
     if(Keyboard->WasPressed(kbsKey1)) {
       if(*var == 0) *var = 1;
       else          *var = 0;
     }
     break;
   case CNTRL_Add:
     if(kbsKey2) {
       if(Keyboard->WasPressed(kbsKey2)) *var += delta;
       if(Keyboard->WasPressed(kbsKey1)) *var -= delta;
     } else {
       if(Keyboard->WasPressed(kbsKey1)) bShift ? *var -= delta : *var += delta;
     }
     break;
   case CNTRL_AddCont:
     if(kbsKey2) {
       if(Keyboard->IsPressed(kbsKey2)) *var += delta;
       if(Keyboard->IsPressed(kbsKey1)) *var -= delta;
     } else {
       if(Keyboard->IsPressed(kbsKey1)) bShift ? *var -= delta : *var += delta;
     }
     break;
   case CNTRL_Mul:
     if(kbsKey2) {
       if(Keyboard->WasPressed(kbsKey2)) *var += delta;
       if(Keyboard->WasPressed(kbsKey1)) *var -= delta;
     } else {
       if(Keyboard->WasPressed(kbsKey1)) bShift ? *var /= delta : *var *= delta;
     }
     break;
   case CNTRL_MulCont:
     if(kbsKey2) {
       if(Keyboard->IsPressed(kbsKey2)) *var += delta;
       if(Keyboard->IsPressed(kbsKey1)) *var -= delta;
     } else {
       if(Keyboard->IsPressed(kbsKey1)) bShift ? *var /= delta : *var *= delta;
     }
     break;
  }
  return;
}
