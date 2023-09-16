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

	//El buffersize va a ser de todo el tamaño del fichero.
	// los ficheros de Amiga son pequeños para la memoria que hay.
	//la ventana que corresponda a font.w y font.h
	
	nextscansize = (font.w * font.h);

	file_t file;
	loadFile("C:/Users/Tolkien/source/repos/Pruebas/SpyFonts/x64/Debug/WAxWORKSFONT.raw", &file);
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
			if (file.position < 0)
				file.position = 0;

			sprintf(positionbuffer, "0x%x", file.position);
		}
		else if (IsKeyPressed(KEY_RIGHT) && file.position < ((file.size) - nextscansize))
		{
			if (file.position > file.size)
					file.position = file.size;
			file.position+= nextscansize;
			sprintf(positionbuffer, "0x%x", file.position);
		}
		
		if (GetTextFilename)
		{
			if (_stricmp(file.name, TextFilename) != 0)
			{
				if (file.fileHandle) fclose(file.fileHandle);		//por si tenemos otro file abierto antes
				loadFile(TextFilename, &file);
			}
			GetTextFilename = false;
		}


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


//pinta todos los font que puede en la ventana
//dependiendo de sus caracteristicas
void drawMap(int position, int size)
{
	//los 5 son el tamaño de los pixeles hechos en drawchar con DrawRectangule
	int nextchar = font.w * font.h;
	int spaceX = 8 * 5;
	int spaceY = font.h * 5;
	int maxfonts = (800 / spaceX) * (450 / spaceY);

	for (int y = 1; y < (450/spaceY); y++)
	{
		for (int x = 1; x < (800/spaceX); x++)			//800 = screenWidht hardcodeado
		{
			drawChar(&spyBuffer[position], x*spaceX, y*spaceY);
			position += nextchar;
			//Habria que calcular el numero maximo de fonts que se pueden pintar
			//ver tamaño del buffer y hacer bufferfinal-maxfonts o algo asi
			if ((position-maxfonts) > size) break;
		}

	}
	//drawChar(&spyBuffer[position+nextchar], posX+spaceX, 0);
}

//pinta un rectangulo segun los bits que estan activos
//si el ancho del font es doble hay que leer una linea de cada
//char y luego la de abajo etc etc..

void drawChar(unsigned char *drawfont, int posx, int posy)
{
	unsigned char* charfont = drawfont;
	int x = 5;
	int y = 5;
	unsigned int mask = 128; //1000 0000 en binario

	for (int j = 1; j <= font.h; j++)
	{
		for (int i = 0; i < font.w*8; i++)
		{
			int index = (i / 8);
			if((charfont[index] & mask) == mask)
				DrawRectangle(posx+x*i, posy+y*j, 5, 5, BLACK);
			mask = mask >> 1;
			if (mask == 0) mask = 128;
		}
		charfont = charfont + font.w;
		//mask = 128;

	}

}

int loadFile(const char* filename, file_t *file)
{
	//si tenemos un buffer del fichero anterior lo cerramos
	if (spyBuffer) free(spyBuffer);
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

	//Requerimos nuestra memoria
	spyBuffer = malloc(sizeof(unsigned char) * file->size);
	if (!spyBuffer)
		puts("Cant allocate spyBuffer");
	fread(spyBuffer, sizeof(char) * file->size, 1, file->fileHandle);

	//Realmente ya no necesitariamos el fichero abierto
	//y podriamos cerrorlo aqui mismo

	return 0;
}