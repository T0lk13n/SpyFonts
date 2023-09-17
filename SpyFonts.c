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
	nextscansize = (font.w * font.h);

	loadFile("C:/Users/Tolkien/source/repos/Pruebas/SpyFonts/x64/Debug/WAxWORKSFONT.raw");
	//loadFile("C:/Users/Tolkien/source/repos/Pruebas/SpyFonts/x64/Debug/font3.raw", &file);


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
	char TextFilename[128];
	//----------------------------------------------------------------------------------
	char positionbuffer[8];
	sprintf(positionbuffer, "0x%x", 0);
	strcpy(TextFilename, file.name);

	InitWindow(screenWidth, screenHeight, "Amiga SpyFonts");
	SetTargetFPS(30);


	//--------------------------------------------------------------------------------------
	// Main loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		font.w = AnchoValue + 1;
		font.h = AltoValue;
		nextscansize = (font.w * font.h);

		//este caso de F1 lo tengo que intentar meter en checkKeyboard
		if (IsKeyPressed(KEY_F1))
			MainWindowActive = !MainWindowActive;
		if (checkInput())
			sprintf(positionbuffer, "0x%x", file.position);
		


		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		///ClearBackground(BLACK);
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		drawMap(file.position, file.size);
		
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

			char mx[4] = "";
			sprintf(mx, "%d", GetMouseX());
			char my[4] = "";
			sprintf(my, "%d", GetMouseY());
			GuiDrawText(mx, (Rectangle) { 550, 220, 80, 10 }, 0, BLACK);
			GuiDrawText(my, (Rectangle) { 550, 230, 80, 10 }, 0, BLACK);

			if (GuiTextBox((Rectangle) { 550, 270, 185, 25 }, TextFilename, 127, FileEditMode))
			{
				if (_stricmp(file.name, TextFilename) != 0)
				{
					loadFile(TextFilename);
					sprintf(positionbuffer, "0x%x", file.position);
				}
				FileEditMode = !FileEditMode;
			}
	
		}
		//----------------------------------------------------------------------------------

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	if(spyBuffer) free(spyBuffer);

	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}


//pinta todos los font que puede en la ventana
//dependiendo de sus caracteristicas
void drawMap(int position, int size)
{
	//los 5 son el tamaño de los pixeles hechos en drawchar con DrawRectangule
	int nextchar = font.w * font.h;
	int spaceX = 8 * PIXELSIZE;
	int spaceY = font.h * PIXELSIZE;
	int maxfonts = (size / font.h);

	for (int y = 0; y < (450/spaceY); y++)
	{
		for (int x = 0; x < (800/spaceX); x++)			//800 = screenWidht hardcodeado
		{
			drawChar(&spyBuffer[position], x*spaceX, y*spaceY);
			position += nextchar;

			if ((position/font.h) >= maxfonts) return;
		}
	}
}

//pinta un rectangulo segun los bits que estan activos
//si el ancho del font es doble hay que leer una linea de cada
//char y luego la de abajo etc etc..
void drawChar(unsigned char *drawfont, int posx, int posy)
{
	unsigned char* charfont = drawfont;
	//const int x = 5;
	//const int y = 5;
	unsigned int mask = 128; //1000 0000 en binario

	for (int j = 0; j < font.h; j++)
	{
		for (int i = 0; i < font.w*8; i++)
		{
			int index = (i / 8);
			if((charfont[index] & mask) == mask)
				DrawRectangle(posx+(PIXELSIZE*i), posy+(PIXELSIZE*j), PIXELSIZE, PIXELSIZE, BLACK);
			mask = mask >> 1;
			if (mask == 0) mask = 128;
		}
		charfont = charfont + font.w;
		//mask = 128;
	}
}

//carga fichero y reserva memoria para el buffer
int loadFile(const char* filename)
{
	//si tenemos un buffer del fichero anterior lo cerramos
	if (spyBuffer) free(spyBuffer);

	//Abrimos Fichero
	file.position = 0;
	strcpy(file.name, filename);
	file.fileHandle = fopen(file.name, "rb");
	if (!file.fileHandle)
	{
		printf("Couldnt open file: %s\n", file.name);
		if (spyBuffer) free(spyBuffer);
		CloseWindow();
		return -1;
	}
	//Get size for buffer
	fseek(file.fileHandle, 0L, SEEK_END);
	file.size = ftell(file.fileHandle);
	rewind(file.fileHandle);

	//Requerimos nuestra memoria
	spyBuffer = malloc(sizeof(unsigned char) * file.size);
	if (!spyBuffer)
		puts("Cant allocate spyBuffer");
	else
		fread(spyBuffer, sizeof(char) * file.size, 1, file.fileHandle);

	//Realmente ya no necesitariamos el fichero abierto
	//y podriamos cerrarlo aqui mismo
	fclose(file.fileHandle);

	return 0;
}

bool checkInput()
{
	//// TODO ESTO ES MUY REPETIDO...MEJORAR YA
	//// se deberia llamar a una funcion update state o algo asi...esta guarro
	switch (GetKeyPressed())
	{
		case KEY_LEFT:
			file.position -= nextscansize;
			if (file.position < 0)
				file.position = 0;
			return true;

		case KEY_RIGHT:
			file.position += nextscansize;
			if (file.position > file.size)
				file.position = file.size;
			return true;

		case KEY_UP:
			file.position -= (nextscansize * 20);
			if (file.position < 0)
				file.position = 0;
			return true;

		case KEY_DOWN:
			file.position += (nextscansize * 20);
			if (file.position > file.size - 8)
				file.position = file.size - 8;
			return true;

		case KEY_HOME:
			file.position = 0;
			return true;

		case KEY_END:
			file.position = file.size - 8;
			return true;
	}

	return false;
}