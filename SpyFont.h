#pragma once
#include <stdio.h>


#define UNO 1		//Solo lee un byte de ancho cada font
#define DOS 2		//lee dos bytes de ancho
//define PIXELSIZE 2		//tama�o del pixel representado

int pixelSize = 2;
const int screenWidth = 1000;
const int screenHeight = 600;

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

int loadFile(const char* filename);
void drawMap(int position, int size);
void drawChar(unsigned char *drawfont, int posx, int posy);
bool checkInput();
int getRelativePos();