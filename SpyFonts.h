#pragma once
#include <stdio.h>


#define UNO 1		//Solo lee un byte de ancho cada font
#define DOS 2		//lee dos bytes de ancho
//define PIXELSIZE 2		//tamaño del pixel representado
#define UNDO_MAX 10

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

typedef struct
{
	int position[UNDO_MAX];
	unsigned char byte[UNDO_MAX];
	int index;
}undo_t;

fontattr_t font;
file_t file;
undo_t undo;

unsigned char *spyBuffer = NULL;
int nextscansize = 0;
bool editMode = false;
bool fileLoaded = false;


bool loadFile(const char* filename);
int  saveFile();
void drawMap(int position, int size);
void drawChar(unsigned char *drawfont, int posx, int posy);
void checkInput();
int  getRelativePos();
void rawEdit();
int  gfxToBuffer();
void newPosition(long newPosition);
void addUndo(int position, unsigned char byte);
void unDo();
void undoDisplace(undo_t* undo);
void drawGui();
