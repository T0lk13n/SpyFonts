#pragma once
#include <stdio.h>


#define UNO 1		//Solo lee un byte de ancho cada font
#define DOS 2		//lee dos bytes de ancho
//define PIXELSIZE 2		//tamaño del pixel representado

int pixelSize = 2;
const int screenWidth = 1024;
const int screenHeight = 600;

bool MainWindowActive = true;
bool HelpWindowActive = true;

typedef struct
{
	int w;		//ancho en bytes
	int h;		//alto
}fontattr_t;

typedef struct
{
	long position;
	long size;
	FILE* fileHandle;
	char name[128];
}file_t;

fontattr_t font;
file_t file;
unsigned char *spyBuffer = NULL;
int nextscansize = 0;
bool editMode = false;

int loadFile(const char* filename);
int saveFile();
void drawMap(int position, int size);
void drawChar(unsigned char *drawfont, int posx, int posy);
void checkInput();
int getRelativePos();
void rawEdit();
int gfxToBuffer();