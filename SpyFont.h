#pragma once
#include <stdio.h>


#define UNO 1		//Solo lee un byte de ancho cada font
#define DOS 2		//lee dos bytes de ancho

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
unsigned char *spyBuffer;

void loadFile(const char* filename, file_t *file);
void drawChar(unsigned char *charfont);