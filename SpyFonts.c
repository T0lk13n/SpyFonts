#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "SpyFont.h"

//TODO: 
//			REFACTORIZAR ESTE LIO URGENTEMENTE
//			CHECK INPUT ES ABERRANTE

int main(void)
{
	SetTraceLogLevel(LOG_NONE);

	// Initialization
	//--------------------------------------------------------------------------------------
	font.w = UNO;
	font.h = 8;	

	nextscansize = (font.w * font.h);
	editMode = false;

	//loadFile("C:/Users/Tolkien/source/repos/Pruebas/SpyFonts/x64/Debug/WAxWORKSFONT.raw");
	//loadFile("C:/Users/Tolkien/source/repos/Pruebas/SpyFonts/x64/Debug/font3.raw", &file);
	loadFile("C:/Users/amiten/Downloads/TRADUCCIONES/SimonTheSorcererAGA/data/runit");

	// layout_name: controls initialization
	//----------------------------------------------------------------------------------

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

		if (checkInput())		//CHECK INPUT ES ABERRANTE
		{
			sprintf(positionbuffer, "0x%x", file.position);
			sprintf(TextFilename, "%s", file.name);

		}
		if (editMode)
			rawEdit();


		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		///ClearBackground(BLACK);
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		drawMap(file.position, file.size);
		


		// raygui: controls drawing
		//----------------------------------------------------------------------------------
		float currentW = (float)GetScreenWidth();
		float currentH = (float)GetScreenHeight();
		if (MainWindowActive)
		{
			MainWindowActive = !GuiWindowBox((Rectangle) { currentW-200, currentH-275, 200, 272 }, "F1 toggle window");
			
			GuiGroupBox((Rectangle) { currentW-195, currentH-240, 190, 99 }, "Font size");
			
			GuiToggleGroup((Rectangle) { currentW-142, currentH-230, 60, 25 }, "8;16", &AnchoValue);			
			if (GuiSpinner((Rectangle) { currentW-140, currentH-190, 119, 25 }, "Height", & AltoValue, 1, 100, AltoEditMode)) AltoEditMode = !AltoEditMode;
			
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
		if (HelpWindowActive)
		{
			HelpWindowActive = !GuiWindowBox((Rectangle) { currentW - 400, currentH - 275, 200, 272 }, "F2 - Help window");
			GuiDrawText("F1  - toggle main window", (Rectangle) { currentW - 390, currentH - 250, 180, 10 }, 0, BLACK);
			GuiDrawText("F2 - toggle help window", (Rectangle) { currentW - 390, currentH - 240, 180, 10 }, 0, BLACK);
			GuiDrawText("Left Cursor - 1 char less", (Rectangle) { currentW - 390, currentH - 230, 180, 10 }, 0, BLACK);
			GuiDrawText("Right Cursor - 1 char more", (Rectangle) { currentW - 390, currentH - 220, 180, 10 }, 0, BLACK);
			GuiDrawText("Up Cursor - less lot chars", (Rectangle) { currentW - 390, currentH - 210, 180, 10 }, 0, BLACK);
			GuiDrawText("Down Cursor - more lot chars", (Rectangle) { currentW - 390, currentH - 200, 180, 10 }, 0, BLACK);
			GuiDrawText("Home key - go to start", (Rectangle) { currentW - 390, currentH - 190, 180, 10 }, 0, BLACK);
			GuiDrawText("End key - go to finish", (Rectangle) { currentW - 390, currentH - 180, 180, 10 }, 0, BLACK);
			GuiDrawText("Lshift + Lmouse - go to that char", (Rectangle) { currentW - 390, currentH - 170, 180, 10 }, 0, BLACK);
			GuiDrawText("z key - Zoom out", (Rectangle) { currentW - 390, currentH - 160, 180, 10 }, 0, BLACK);
			GuiDrawText("x key - Zoom in", (Rectangle) { currentW - 390, currentH - 150, 180, 10 }, 0, BLACK);
			GuiDrawText("Drop file to open", (Rectangle) { currentW - 390, currentH - 140, 180, 10 }, 0, BLACK);
			GuiDrawText("Lctrl + s - Save file", (Rectangle) { currentW - 390, currentH - 130, 180, 10 }, 0, BLACK);
			GuiDrawText("Alt + Lmouse - Edit raw", (Rectangle) { currentW - 390, currentH - 120, 180, 10 }, 0, BLACK);
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
	char winTitle[100];
	strcpy(winTitle, "Amiga SpyFonts - ");
	strcat(winTitle, GetFileName(file.name));
	SetWindowTitle(winTitle);

	file.size = GetFileLength(file.name);

	//Requerimos nuestra memoria
	spyBuffer = malloc(sizeof(unsigned char) * file.size);
	if (!spyBuffer)
		puts("Cant allocate spyBuffer");
	else
		spyBuffer = LoadFileData(file.name, &file.size);

	return 0;
}



int saveFile()
{

	return 0;
}


/// <summary>
/// 
/// </summary>
/// <returns></returns>
bool checkInput()
{
	static bool upScroll   = false;
	static bool downScroll = false;
	//// TODO ESTO ES MUY REPETIDO...MEJORAR YA
	//// se deberia llamar a una funcion update state o algo asi...esta guarro

	switch (GetKeyPressed())
	{
		case KEY_F1:
			MainWindowActive = !MainWindowActive;
			return false;

		case KEY_F2:
			HelpWindowActive = !HelpWindowActive;
			return false;

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
			upScroll = true;
			downScroll = false;
			break;

		case KEY_DOWN:
			upScroll = false;
			downScroll = true;
			break;

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

	if (IsKeyDown(KEY_LEFT_ALT))
		editMode = true;
	else
		editMode = false;

	if(IsKeyReleased(KEY_UP))
	{
		upScroll = false;
		downScroll = false;
		
	}
	else if (IsKeyReleased(KEY_DOWN))
	{
		upScroll = false;
		downScroll = false;
	}

	if (upScroll)
	{
		file.position -= (nextscansize * 40);
		if (file.position < 0)
			file.position = 0;
		return true;
	}
	else if (downScroll)
	{
		file.position += (nextscansize * 40);
		if (file.position > file.size - 8)
			file.position = file.size - 8;
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




/// <summary>
/// AUN FALLA EN ALGUNA POSICION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///     PUEDE QUE SEA GFXTOBUFFER QUE NO SIEMPRE DEVUELVE BIEN
/// </summary>
void rawEdit()
{
	int x = (GetMouseX() / pixelSize) * (font.w * pixelSize);
	int y = (GetMouseY()  / pixelSize) * pixelSize;
	DrawRectangle(x, y, pixelSize, pixelSize, RED);

	int bufferPosition = file.position + gfxToBuffer();

	int pixelShift = x / (font.w * pixelSize) % 8;
	unsigned byte = 0b10000000 >> pixelShift;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		spyBuffer[bufferPosition] ^= byte;
}


//devuelve la direccion en el buffer correspondiente a donde tenemos el cursor en pantalla
int gfxToBuffer()
{
	int currentW = GetScreenWidth();

	int charsperLargo = currentW / (font.w * 8 * pixelSize);
	int x = ((GetMouseX() * charsperLargo) / currentW);
	x *= font.w * font.h;

	int y = (GetMouseY() / pixelSize);  //% font.h; // *pixelSize;
	int yAbs = (y / (font.h)) *(charsperLargo * font.h);
	y = y % font.h;

	//printf("x: %d  y: %d  yAbs: %d\n", x, y, yAbs);


	return x+yAbs+y; // *= font.w * font.h;
}