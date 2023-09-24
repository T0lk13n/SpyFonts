#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "SpyFont.h"

//TODO: 
//			HACER COPIA DE CARACTERES SELECCIONADOS PARA PODER EDITARLOS FUERA

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
	//strcpy(TextFilename, file.name);
	TextCopy(TextFilename, file.name);

	InitWindow(screenWidth, screenHeight, "Amiga SpyFonts");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetWindowMinSize(400, 200);               // Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
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
		{
			sprintf(positionbuffer, "0x%x", file.position);
			sprintf(TextFilename, "%s", file.name);

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
			
			float currentW = (float)GetScreenWidth();
			float currentH = (float)GetScreenHeight();

			MainWindowActive = !GuiWindowBox((Rectangle) { currentW-200, currentH-275, 200, 272 }, "F1 toggle window");
			
			GuiGroupBox((Rectangle) { currentW-195, currentH-240, 190, 99 }, "Font size");
			GuiDrawText("Width", (Rectangle) { currentW-170, currentH-220, 80, 10 }, 0, BLACK);
			GuiToggleGroup((Rectangle) { currentW-142, currentH-230, 60, 25 }, "8;16", &AnchoValue);			
			if (GuiSpinner((Rectangle) { currentW-140, currentH-190, 119, 25 }, "Height", & AltoValue, 0, 100, AltoEditMode)) AltoEditMode = !AltoEditMode;
			
			GuiDrawText("Position: ", (Rectangle) { currentW-190, currentH-100, 80, 10 }, 0, BLACK);
			GuiDrawText(positionbuffer, (Rectangle) { currentW-140, currentH-100, 80, 10 }, 0, BLACK);

			char mx[6] = "";
			sprintf(mx, "%d", GetMouseX()/pixelSize);
			char my[6] = "";
			sprintf(my, "%d", GetMouseY()/pixelSize);
			GuiDrawText(mx, (Rectangle) { currentW -190, currentH-80, 80, 10 }, 0, BLACK);
			GuiDrawText(my, (Rectangle) { currentW-190, currentH-70, 80, 10 }, 0, BLACK);

			if (GuiTextBox((Rectangle) { currentW-190, currentH-30, 185, 25 }, TextFilename, 127, FileEditMode))
			{
				//if (_stricmp(file.name, TextFilename) != 0)
				if(!TextIsEqual(file.name, TextFilename))
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
	int currentW = GetScreenWidth();
	int currentH = GetScreenHeight();

	//los 8 es por que cada byte tiene ocho pixels a pintar
	int fontSize = font.w * font.h;
	int spaceX = font.w * 8 * pixelSize;
	int spaceY = font.h * pixelSize;
	int maxfonts = (size / fontSize);

	for (int y = 0; y < (currentH/spaceY); y++)
	{
		for (int x = 0; x < (currentW/spaceX); x++)			
		{
			drawChar(&spyBuffer[position], x*spaceX, y*spaceY);
			position += fontSize;

			if ((position/fontSize) >= maxfonts) return;  
		}
	}
}

//pinta un rectangulo segun los bits que estan activos
//si el ancho del font es doble hay que leer una linea de cada
//char y luego la de abajo etc etc..
void drawChar(unsigned char *drawfont, int posx, int posy)
{
	unsigned char* charfont = drawfont;

	unsigned int mask = 128; //1000 0000 en binario

	for (int j = 0; j < font.h; j++)
	{
		for (int i = 0; i < font.w*8; i++)
		{
			int index = (i / 8);
			if((charfont[index] & mask) == mask)
				DrawRectangle(posx+(pixelSize*i), posy+(pixelSize*j), pixelSize, pixelSize, BLACK);
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
	//strcpy(file.name, filename);
	TextCopy(file.name, filename);

	file.size = GetFileLength(file.name);

	//Requerimos nuestra memoria
	spyBuffer = malloc(sizeof(unsigned char) * file.size);
	if (!spyBuffer)
		puts("Cant allocate spyBuffer");
	else
		spyBuffer = LoadFileData(file.name, &file.size);

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
			if (file.position > file.size-8)
				file.position = file.size-8;
			return true;

		case KEY_UP:
			file.position -= (nextscansize * 40);
			if (file.position < 0)
				file.position = 0;
			return true;

		case KEY_DOWN:
			file.position += (nextscansize * 40);
			if (file.position > file.size - 8)
				file.position = file.size - 8;
			return true;

		case KEY_HOME:
			file.position = 0;
			return true;

		case KEY_END:
			file.position = file.size - 8;
			return true;

		case KEY_Z:
			pixelSize--;
			if (pixelSize < 1)
				pixelSize = 1;
			return true;

		case KEY_X:
			pixelSize++;
			if (pixelSize > 8)
				pixelSize = 8;
			return true;
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsKeyDown(KEY_LEFT_SHIFT))
	{
		file.position += getRelativePos()*font.w*8;
		if (file.position > file.size -8)
			file.position = file.size -8;
		return true;
	}

	if (IsFileDropped())
	{
		FilePathList dropedFiles = LoadDroppedFiles();
		loadFile(dropedFiles.paths[0]);
		UnloadDroppedFiles(dropedFiles);
		return true;
	}

	return false;
}

int getRelativePos()
{
	int currentW = GetScreenWidth();
	int currentH = GetScreenHeight();
	Vector2 mouseposition;
	mouseposition = GetMousePosition();
	int charsperLargo = currentW / (font.w * 8 * pixelSize);
	int charsperAlto = currentH / (font.h * pixelSize); 
	int relX = (((int)mouseposition.x * charsperLargo) / currentW);
	int RelY = (((int)mouseposition.y * charsperAlto) / currentH) * charsperLargo;
	int relativepos = relX + RelY;

	return relativepos;
}