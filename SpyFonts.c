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

	//El buffersize puede ser mas grande siempre y nosotros mostrar solo 
	//la ventana que corresponda a font.w y font.h
	int spybuffersize = (font.w * font.h)*2;
	int nextscansize = (font.w * font.h);
	spyBuffer = malloc(sizeof(unsigned char) * spybuffersize);


	//ESTO A OTRA FUNCION YA!
	//Abrimos Fichero
	file_t file = { .fileHandle = NULL, .name ='\0', .position = 0, .size = 0};
	//strcpy(file->name, "C:/Users/Tolkien/source/repos/Pruebas/RayGuiFonts/x64/Debug/waxworksfont.raw");
	strcpy(file.name, "C:/Users/Tolkien/source/repos/Pruebas/RayGuiFonts/x64/Debug/font3.raw");
	file.fileHandle = fopen(file.name, "rb");
	if (!file.fileHandle)
	{
		if(spyBuffer) free(spyBuffer);
		//if (file) free(file);
		CloseWindow();
		return - 1;
	}
	fseek(file.fileHandle, 0L, SEEK_END);
	file.size = ftell(file.fileHandle);
	rewind(file.fileHandle);
	fread(spyBuffer, sizeof(char)*spybuffersize, 1, file.fileHandle);


	// layout_name: controls initialization
	//----------------------------------------------------------------------------------
	bool MainWindowActive = true;
	//bool AnchoEditMode = false;
	int AnchoValue = font.w-1;
	bool AltoEditMode = false;
	int AltoValue = font.h;
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
		font.w = AnchoValue + 1;
		font.h = AltoValue;
		nextscansize = (font.w * font.h);

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
			//if (GuiSpinner((Rectangle) { 461, 86, 119, 25 }, "Width", & AnchoValue, 0, 100, AnchoEditMode)) AnchoEditMode = !AnchoEditMode;
			GuiDrawText("Width", (Rectangle) { 430, 95, 80, 10 }, 0, BLACK);
			GuiToggleGroup((Rectangle) { 460, 86, 60, 25 }, "8;16", &AnchoValue);			

			if (GuiSpinner((Rectangle) { 461, 124, 119, 25 }, "Height", & AltoValue, 0, 100, AltoEditMode)) AltoEditMode = !AltoEditMode;
			if (GuiTextBox((Rectangle) { 461, 200, 119, 25 }, file.name, 127, FileEditMode)) FileEditMode = !FileEditMode;
			GuiDrawText("Position: ", (Rectangle) { 410, 180, 80, 10 }, 0, BLACK);
			GuiDrawText(positionbuffer, (Rectangle) { 460, 180, 80, 10 }, 0, BLACK);
	
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

