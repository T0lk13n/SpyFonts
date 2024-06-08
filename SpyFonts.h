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

fontattr_t font = { UNO, 8 };
file_t file;
undo_t undo;

unsigned char *spyBuffer = NULL;
int	 nextscansize = 0;
bool editMode = false;
bool fileLoaded = false;
bool saveRequester = false;
bool modifiedFile = false;
bool checkQuit = false;
bool quit = false;



#define SEARCH(index, font, LSR)	fontSearch[font][0] >> LSR == spyBuffer[index]   &&\
									fontSearch[font][1] >> LSR == spyBuffer[index+1] &&\
									fontSearch[font][2] >> LSR == spyBuffer[index+2] &&\
									fontSearch[font][3] >> LSR == spyBuffer[index+3] &&\
									fontSearch[font][4] >> LSR == spyBuffer[index+4] &&\
									fontSearch[font][5] >> LSR == spyBuffer[index+5] &&\
									fontSearch[font][6] >> LSR == spyBuffer[index+6]
					 

// Datos del font EXCLAMACION! 
// Puede tener muchas variantes...
#define NUMFONTS 4
const unsigned char fontSearch[NUMFONTS][7] =
{
	{
		{0b01000000},
		{0b11100000},
		{0b11100000},
		{0b01000000},
		{0b01000000},
		{0b00000000},
		{0b01000000}
	},
	{
		{0b01000000},
		{0b01000000},
		{0b01000000},
		{0b01000000},
		{0b01000000},
		{0b00000000},
		{0b01000000}

	},
	{
		{0b01100000},
		{0b01100000},
		{0b01100000},
		{0b01100000},
		{0b01100000},
		{0b00000000},
		{0b01100000}
	},
	{
		{0b01100000},
		{0b01100000},
		{0b01100000},
		{0b01100000},
		{0b00000000},
		{0b00000000},
		{0b01100000}
	}
};




bool loadFile(const char* filename);
int  saveFile();
void drawMap(int position, int size);
void drawChar(unsigned char *drawfont, int posx, int posy);
void checkInput();
int  getRelativePos();
void rawEdit();
int  gfxToBuffer(int mx, int my);
void newPosition(long newPosition);
void addUndo(int position, unsigned char byte);
void unDo();
void undoDisplace(undo_t* undo);
void drawGui();
bool autoSearch();
void limitPixelSize();
