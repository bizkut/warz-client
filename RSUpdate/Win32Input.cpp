#include "r3dPCH.h"
#include "r3d.h"

#include "Win32Input.h"

#define INPUT_KBD_STACK	32
volatile static  int   input_ScanStack[INPUT_KBD_STACK + 2];
volatile static  int   * volatile input_StackHead = input_ScanStack;
volatile static  int   * volatile input_StackTail = input_ScanStack;


void win32_OnWMChar(WPARAM wParam)
{
  int ch = (unsigned char)wParam;
  *(input_StackHead++) = ch;
  if(input_StackHead >= input_ScanStack + INPUT_KBD_STACK)
    input_StackHead = input_ScanStack;
}

int win32_input_Flush()
{
  input_StackHead = input_ScanStack;
  input_StackTail = input_ScanStack;

  return 1;
}

int win32_kbhit()
{
  if(input_StackHead == input_StackTail)
    return 0;
  else
    return 1;
}

int win32_getch()
{
	int	ch;

  if(!win32_kbhit())
    return 0;

  ch = *input_StackTail;
  input_StackTail++;

  if(input_StackTail >= input_ScanStack + INPUT_KBD_STACK)
    input_StackTail = input_ScanStack;

  return ch;
}
