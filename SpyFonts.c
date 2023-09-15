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
	const int screenWidth = 800;
	const int screenHeight = 450;


	font.w = UNO;
	font.h = 8;

	int spybuffersize = (font.w * font.h);
	spyBuffer = malloc(sizeof(unsigned char) * spybuffersize);

	file_t *file = (file_t *)malloc(sizeof(file_t));
	if (!file) return -2;
	(*file).position = 0;
	strcpy(file->name, "C:/Users/Tolkien/source/repos/Pruebas/RayGuiFonts/x64/Debug/waxworksfont.raw");


	
	//file->fileHandle = fopen("C:/Users/Tolkien/source/repos/Pruebas/RayGuiFonts/x64/Debug/font2.raw", "rb"); //DOBLE FONT
	file->fileHandle = fopen(file->name, "rb");
	if (!file->fileHandle)
	{
		if(spyBuffer) free(spyBuffer);
		if (file) free(file);
		CloseWindow();
		return - 1;
	}
	fseek(file->fileHandle, 0L, SEEK_END);
	file->size = ftell(file->fileHandle);
	rewind(file->fileHandle);

	fread(spyBuffer, sizeof(char)*spybuffersize, 1, file->fileHandle);
	//printf("Caracter: %x\n", spyBuffer[0]);

	// layout_name: controls initialization
	//----------------------------------------------------------------------------------
	bool MainWindowActive = true;
	bool AnchoEditMode = false;
	int AnchoValue = 0;
	bool AltoEditMode = false;
	int AltoValue = 0;
	bool FileEditMode = false;
	//----------------------------------------------------------------------------------
	char positionbuffer[8];
	sprintf(positionbuffer, "0x%x", 0);

	InitWindow(screenWidth, screenHeight, "Amiga SpyFonts");
	SetTargetFPS(60);
	//--------------------------------------------------------------------------------------
	// Main loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{



		if (IsKeyPressed(KEY_LEFT) && file->position >= spybuffersize)
		{
			file->position-=spybuffersize;
			fseek(file->fileHandle, file->position, SEEK_SET);
			fread(spyBuffer, sizeof(char) * spybuffersize, 1, file->fileHandle);
			printf("Position: 0x%x\n", file->position);
			sprintf(positionbuffer, "0x%x", file->position);
		}
		else if (IsKeyPressed(KEY_RIGHT) && file->position < ((file->size) - spybuffersize))
		{
			file->position+= spybuffersize;
			fseek(file->fileHandle, file->position, SEEK_SET);
			fread(spyBuffer, sizeof(char) * spybuffersize, 1, file->fileHandle);
			printf("Position: 0x%x\n", file->position);
			sprintf(positionbuffer, "0x%x", file->position);
		}
		
		//printf("Ancho Value: %d\n", AnchoValue);

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
			MainWindowActive = !GuiWindowBox((Rectangle) { 402, 26, 201, 272 }, "Amiga SpyFonts");
			GuiGroupBox((Rectangle) { 407, 69, 189, 99 }, "Font size");
			if (GuiSpinner((Rectangle) { 461, 86, 119, 25 }, "Width", & AnchoValue, 0, 100, AnchoEditMode)) AnchoEditMode = !AnchoEditMode;
			if (GuiSpinner((Rectangle) { 461, 124, 119, 25 }, "Height", & AltoValue, 0, 100, AltoEditMode)) AltoEditMode = !AltoEditMode;
			if (GuiTextBox((Rectangle) { 461, 200, 119, 25 }, file->name, 127, FileEditMode)) FileEditMode = !FileEditMode;
			GuiDrawText("Position: ", (Rectangle) { 410, 180, 80, 10 }, 0, BLACK);
			GuiDrawText(positionbuffer, (Rectangle) { 460, 180, 80, 10 }, 0, BLACK);
	
		}
		//----------------------------------------------------------------------------------

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	if(spyBuffer) free(spyBuffer);
	if(file->fileHandle) fclose(file->fileHandle);
	if(file) free(file);
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}


//pinta un rectangulo segun los bits que estan activos
void drawChar(unsigned char *charfont)
{
	int x = 10;
	int y = 10;
	unsigned char mask = 128; //1000 0000 en binario

	for (int j = 1; j <= font.h; j++)
	{
		for (int i = 0; i < (font.w * 8); i++)
		{
			if((*charfont & mask) == mask)
				DrawRectangle(x*i, y*j, 10, 10, BLACK);
			//mask /= 2;
			mask = mask >> 1; 
		}
		charfont++;
		mask = 128;
		//A LO EXTREMADAMENTE CUTRE DE MOMENTO PARA CUANDO EL FONT.W = DOS
		if (font.w == DOS)
		{
			for (int i = 0; i < (font.w * 8); i++)
			{
				if ((*charfont & mask) == mask)
					DrawRectangle(x+80 * i, y * j, 10, 10, WHITE);
				//mask /= 2;
				mask = mask >> 1;
			}
			charfont++;
			mask = 128;
		}
	}

}