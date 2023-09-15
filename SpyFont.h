#pragma once
#include <stdio.h>

void drawChar(unsigned char *charfont);

#define UNO 1		//Solo lee un byte de ancho cada font
#define DOS 2		//lee dos bytes de ancho

typedef struct
{
	int w;		//ancho en bytes
	int h;		//alto
}fontattr_t;

unsigned char *spyBuffer;

typedef struct
{
	long position;
	long size;
	FILE* fileHandle;
	char name[128];
}file_t;

fontattr_t font;
