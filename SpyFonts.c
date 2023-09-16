#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "SpyFont.h"


int main(void)
{
	SetTraceLogLevel(LOG_NONE);

	// Initialization
	//--------------------------------------------------------------------------------------
	font.w = UNO;
	font.h = 8;

	//El buffersize puede ser mas grande siempre y nosotros mostrar solo 
	//la ventana que corresponda a font.w y font.h
	int spybuffersize = (font.w * font.h)*2;
	int nextscansize = (font.w * font.h);
	spyBuffer = malloc(sizeof(unsigned char) * spybuffersize);

	file_t file;
	loadFile("C:/Users/Tolkien/source/repos/Pruebas/SpyFonts/x64/Debug/font3.raw", &file);
	fread(spyBuffer, sizeof(char) * spybuffersize, 1, file.fileHandle);


	const int screenWidth = 800;
	const int screenHeight = 450;
	// layout_name: controls initialization
	//----------------------------------------------------------------------------------
	bool MainWindowActive = true;
	//bool AnchoEditMode = false;
	int AnchoValue = font.w-1;
	bool AltoEditMode = false;
	int AltoValue = font.h;
	bool FileEditMode = false;
	bool GetTextFilename = false;
	char TextFilename[128];
	//----------------------------------------------------------------------------------
	char positionbuffer[8];
	sprintf(positionbuffer, "0x%x", 0);
	strcpy(TextFilename, file.name);

	InitWindow(screenWidth, screenHeight, "Amiga SpyFonts");
	SetTargetFPS(60);


	//--------------------------------------------------------------------------------------
	// Main loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		font.w = AnchoValue + 1;
		font.h = AltoValue;
		nextscansize = (font.w * font.h);

		if(IsKeyPressed(KEY_F1))
			MainWindowActive = !MainWindowActive;

		if (IsKeyPressed(KEY_LEFT))
		{
			file.position-=nextscansize;
			if (file.position < 0) file.position = 0;
			fseek(file.fileHandle, file.position, SEEK_SET);
			fread(spyBuffer, sizeof(char) * spybuffersize, 1, file.fileHandle);
			sprintf(positionbuffer, "0x%x", file.position);
		}
		else if (IsKeyPressed(KEY_RIGHT) && file.position < ((file.size) - nextscansize))
		{
			file.position+= nextscansize;
			fseek(file.fileHandle, file.position, SEEK_SET);
			fread(spyBuffer, sizeof(char) * spybuffersize, 1, file.fileHandle);
			sprintf(positionbuffer, "0x%x", file.position);
		}
		
		if (GetTextFilename)
		{
			if (_stricmp(file.name, TextFilename) != 0)
			{
				strcpy(file.name, TextFilename);
				if (file.fileHandle) fclose(file.fileHandle);		//por si tenemos otro file abierto antes
				loadFile(file.name, &file);
				//printf("Edit file %s\n", file.name);
			}
			GetTextFilename = false;
		}


		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		///ClearBackground(BLACK);
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		drawChar(spyBuffer);

		
		// raygui: controls drawing
		//----------------------------------------------------------------------------------
		if (MainWindowActive)
		{
			MainWindowActive = !GuiWindowBox((Rectangle) { 542, 26, 201, 272 }, "Amiga SpyFonts - F1 toggle window");
			
			GuiGroupBox((Rectangle) { 547, 69, 190, 99 }, "Font size");
			GuiDrawText("Width", (Rectangle) { 570, 95, 80, 10 }, 0, BLACK);
			GuiToggleGroup((Rectangle) { 600, 86, 60, 25 }, "8;16", &AnchoValue);			
			if (GuiSpinner((Rectangle) { 600, 124, 119, 25 }, "Height", & AltoValue, 0, 100, AltoEditMode)) AltoEditMode = !AltoEditMode;
			
			GuiDrawText("Position: ", (Rectangle) { 550, 180, 80, 10 }, 0, BLACK);
			GuiDrawText(positionbuffer, (Rectangle) { 600, 180, 80, 10 }, 0, BLACK);

			if (GuiTextBox((Rectangle) { 550, 270, 185, 25 }, TextFilename, 127, FileEditMode))
			{
				GetTextFilename = true;
				FileEditMode = !FileEditMode;
			}
	
		}
		//----------------------------------------------------------------------------------

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	if(spyBuffer) free(spyBuffer);
	if(file.fileHandle) fclose(file.fileHandle);
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}


//pinta un rectangulo segun los bits que estan activos
//si el ancho del font es doble hay que leer una linea de cada
//char y luego la de abajo etc etc..

void drawChar(unsigned char *charfont)
{
	int x = 10;
	int y = 10;
	unsigned int mask = 128; //1000 0000 en binario

	for (int j = 1; j <= font.h; j++)
	{
		for (int i = 0; i < font.w*8; i++)
		{
			int index = (i / 8);
			if((charfont[index] & mask) == mask)
				DrawRectangle(x*i, y*j, 10, 10, BLACK);
			mask = mask >> 1;
			if (mask == 0) mask = 128;
		}
		charfont = charfont + font.w;
		//mask = 128;

	}

}

int loadFile(const char* filename, file_t *file)
{
	
	//Abrimos Fichero
	file->position = 0;
	strcpy(file->name, filename);
	file->fileHandle = fopen(file->name, "rb");
	if (!file->fileHandle)
	{
		printf("Couldnt open file: %s\n", file->name);
		if (spyBuffer) free(spyBuffer);
		CloseWindow();
		return -1;
	}
	fseek(file->fileHandle, 0L, SEEK_END);
	file->size = ftell(file->fileHandle);
	rewind(file->fileHandle);
	return 0;
}