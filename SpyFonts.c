#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "SpyFonts.h"

//#include <windows.h>		//for resources

//DONE:
//			NO CARGAR PROYECTO POR DEFECTO
//			QUITAR CONSOLE EN RELEASE
//			INCREMENTAR/DECREMENTAR POR BYTES (para ajuste fino de algunos ficheros)
//			CAMBIAR PUNTERO EN MODO EDICION
//			FOCUS WINDOW AFTER DRAG AND DROP
//			AUMENTAR TAMA�O DEL FONT
//			UNDO
//			NO FUNCIONAN LAS OPCIONES DEL GUI AL HABER CAMBIADO A DRAWGUI()
//			HACER EL SAVE FILE (half done - notificar de alguna manera si se ha grabado?)
//			SOLO SALVAR SI SE HA MODIFICADO ALGO
//			AVISAR SI SALIMOS Y HEMOS MODIFICADO EL FICHERO
//			AUTOMATIC FONT SEARCH (PRIMERA APROXIMACION) 
//			EL AUTOSEARCH SE QUEDA EN LO ENCONTRADO Y HAY QUE MOVERSE ADELANTE PARA BUSCR MAS. CAMBIAR ESO.
//			USAR MOUSE WHEEL PARA DESPLAZARSE O HACER ZOOM



//TODO: 
//			REFACTORIZAR ESTE LIO de CHECKINPUT URGENTEMENTE
//			CHECK INPUT ES ABERRANTE
//			CAMBIAR ICONO (falta icono ventana)
//			USAR OTRO FONT MAS LEGIBLE
//			USAR SHORTCUTS COHERENTES CON EL STANDARD
//			SOLO DIBUJAR SI SE ACTUALIZA ALGO !!!
//			FIX EDIT MODE CON FONT DE 16 DE ANCHO
//			ALGUN REQUESTER EN AUTOMATIC FONT SEARCH ?
//			COPY SELECTED ZONE / PASTE TO POSITION

int AnchoValue = 0;
int AltoValue = 8;

#ifdef _WIN32
int WinMain(void)
#else
int main()
#endif
{
	SetTraceLogLevel(LOG_NONE);

	// Initialization
	InitWindow(screenWidth, screenHeight, "Amiga SpyFonts - tolkien 2024");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetWindowMinSize(400, 200);               // Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
	GuiSetStyle(DEFAULT, TEXT_SIZE, 10);
	SetTargetFPS(30);

	//--------------------------------------------------------------------------------------
	// Main loop
	
	while (!quit)   // Detect window close button or ESC key
	{
		if (WindowShouldClose() )
		{
			if (modifiedFile)
				checkQuit = true;
			else
				quit = true;
		}


		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		if(fileLoaded)
			drawMap(file.position, file.size);	//PORQUE LO DIBUJAMOS SIEMPRE?

		drawGui();
		checkInput();
		EndDrawing();
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
bool loadFile(const char* filename)
{
	//No estamos comprobando que se haya podido cargar el fichero

	//si tenemos un buffer del fichero anterior lo cerramos
	if (spyBuffer) free(spyBuffer);

	//Abrimos Fichero
	file.position = 0;
	//strcpy(file.name, filename);
	TextCopy(file.name, filename);
	char winTitle[100];
	strcpy(winTitle, "Amiga SpyFonts - tolkien 2024 - ");
	strcat(winTitle, GetFileName(file.name));
	SetWindowTitle(winTitle);

	file.size = GetFileLength(file.name);

	//Requerimos nuestra memoria
	spyBuffer = malloc(sizeof(unsigned char) * file.size);
	if (!spyBuffer)
	{
		puts("Cant allocate spyBuffer");
		return false;
	}
	else
		spyBuffer = LoadFileData(file.name, &file.size);

	modifiedFile = false;
	return true;
}



/// INPUT 
void checkInput()
{
	static bool upScroll   = false;
	static bool downScroll = false;
	nextscansize = (font.w * font.h);

	//// TODO ESTO ES MUY REPETIDO...MEJORAR YA
	//// se deberia llamar a una funcion update state o algo asi...esta guarro

	if (fileLoaded)
	{
		switch (GetKeyPressed())
		{
		case KEY_F1:
			MainWindowActive = !MainWindowActive;
			break;

		case KEY_F2:
			HelpWindowActive = !HelpWindowActive;
			break;

		case KEY_LEFT:
			newPosition(-nextscansize);
			break;

		case KEY_RIGHT:
			newPosition(nextscansize);
			break;

		case KEY_UP:
			upScroll = true;
			downScroll = false;
			break;

		case KEY_DOWN:
			upScroll = false;
			downScroll = true;
			break;

			//Move Byte by byte
		case KEY_N:
			newPosition(-1);
			break;

		case KEY_M:
			newPosition(1);
			break;

		case KEY_HOME:
			newPosition(-file.position);
			break;

		case KEY_END:
			newPosition(file.size - file.position);
			break;

		case KEY_Z:
			pixelSize--;
			limitPixelSize();
			break;

		case KEY_X:
			pixelSize++;
			limitPixelSize();
			break;

		case (KEY_LEFT_CONTROL & KEY_A):
			if(modifiedFile) saveRequester = true;
			break;

		case (KEY_LEFT_CONTROL & KEY_U):
			unDo();
			break;

		case KEY_F:
			autoSearch();
			break;

		}//switch


		//MouseWheel
		if(!IsKeyDown(KEY_LEFT_SHIFT))
			newPosition(-GetMouseWheelMove() * nextscansize*40);
		else
		{
			pixelSize += -GetMouseWheelMove();
			limitPixelSize();
		}

		//Edit Mode
		if (IsKeyDown(KEY_LEFT_ALT))
		{
			SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
			rawEdit();
		}
		else
			SetMouseCursor(MOUSE_CURSOR_DEFAULT);

		//Fast Scroll
		if (IsKeyReleased(KEY_UP))
		{
			upScroll = false;
			downScroll = false;

		}
		else if (IsKeyReleased(KEY_DOWN))
		{
			upScroll = false;
			downScroll = false;
		}

		//Esto debe de estar aqui?
		//Fast Scroll
		if (upScroll)
			newPosition(-nextscansize * 40);		//NO MAGIC NUMBERS AGAIN!
		else if (downScroll)
			newPosition(nextscansize * 40);			//* 40 VELOCIDAD SCROLL


		//Move to mouse pointer click
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsKeyDown(KEY_LEFT_SHIFT))
			newPosition(getRelativePos() * font.w * 8);	

	}//if fileloaded

	if (IsFileDropped())
	{
		FilePathList dropedFiles = LoadDroppedFiles();
		if (loadFile(dropedFiles.paths[0]))
			fileLoaded = true;
		UnloadDroppedFiles(dropedFiles);
		SetWindowFocused();
	}

}



void limitPixelSize()
{
	if (pixelSize < 1)
		pixelSize = 1;
	else if (pixelSize > 24)
		pixelSize = 24;
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


void drawGrid(int space, Color col )
{
	int	currentW = GetScreenWidth();
	int currentH = GetScreenHeight();
	for (size_t x = 0; x < currentW; x += space )
	{
		DrawLine(x, 0, x, currentH, col);
	}
	for (size_t y = 0; y < currentH; y += space)
	{
		DrawLine(0, y, currentW, y, col);
	}
}

void rawEdit()
{
	editMode = true;
	int charSize = font.w * 8 * pixelSize;
	int rightLimit = (GetScreenWidth() / charSize) * charSize;
	if (GetMouseX() > rightLimit) return;

	int x = (GetMouseX() / pixelSize) * pixelSize;
	int y = (GetMouseY() / pixelSize) * pixelSize;
	DrawRectangle(x, y, pixelSize, pixelSize, RED);

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		//int bufferPosition = file.position + gfxToBuffer(GetMouseX(), GetMouseY());
		int bufferPosition = file.position + gfxToBuffer((x/pixelSize), (y/pixelSize));

		int pixelShift = x / (font.w * pixelSize) % 8;
		unsigned char byte = 0b10000000 >> pixelShift;

		spyBuffer[bufferPosition] ^= byte;
		addUndo(bufferPosition, byte);
		if (!modifiedFile)
		{
			modifiedFile = true;
			char winTitle[100];
			strcpy(winTitle, "Amiga SpyFonts - tolkien 2024 - ");
			strcat(winTitle, GetFileName(file.name));
			strcat(winTitle, " - Modified");
			SetWindowTitle(winTitle);
		}
	}
}


//devuelve la direccion en el buffer correspondiente a donde tenemos el cursor en pantalla
//No funciona cuando el font.w es a 16 pixels
int gfxToBuffer(int mx, int my)
{
	int currentW = GetScreenWidth();
	int charSize = font.w * 8 * pixelSize;
	int charsperLine = (currentW / charSize);

	int xbyte = mx / (font.w * 8);
	int x = xbyte;
	x *= (font.w * font.h);

	int yAbs = (my / font.h) * (charsperLine * font.h);
	int y = my % font.h;

	//printf("x: %d  y: %d  yAbs: %d\n", x, y, yAbs);

	return x+yAbs+y; // *= font.w * font.h;
}



//Actualiza la posicion del buffer en pantalla
void newPosition(long newPosition)
{
	file.position += newPosition;
	if (file.position > file.size - 8)
		file.position = file.size - 8;
	if (file.position < 0)
		file.position = 0;
}

void addUndo(int position, unsigned char byte)
{
	if (undo.index >= UNDO_MAX)
	{
		undo.index = UNDO_MAX - 1;
		undoDisplace(&undo);
	}

	undo.position[undo.index] = position;
	undo.byte[undo.index] = byte;

	undo.index++;
}

void unDo()
{
	undo.index--;
	if (undo.index >= 0)
	{
		spyBuffer[undo.position[undo.index]] ^= undo.byte[undo.index];
	}
	else
		undo.index = 0;
}

//Si llega al maximo de niveles de undo elimina el primero y desplaza para tener el ultimo libre
void undoDisplace(undo_t *undo)
{
	for (int i = 0; i < UNDO_MAX-1; i++)
	{
		undo->position[i] = undo->position[i + 1];
		undo->byte[i] = undo->byte[i + 1];
	}
}


void drawGui()
{
	bool FileEditMode = false;
	char positionbuffer[8];

	// raygui: controls drawing
//----------------------------------------------------------------------------------
	float currentW = (float)GetScreenWidth();
	float currentH = (float)GetScreenHeight();

	// Draw GRIDs
	if (editMode)
	{
		drawGrid(pixelSize, WHITE);
		editMode = false;
	}
	drawGrid(font.w * 8 * pixelSize, RED);

	//PREFS WINDOW
	if (MainWindowActive)
	{
		sprintf(positionbuffer, "0x%x", file.position);			//QUISIERA QUE SOLO SE ACTUALIZARA AL MOVERNOS EN EL BUFFER pero no es grave

		MainWindowActive = !GuiWindowBox((Rectangle) { currentW - 200, currentH - 275, 200, 272 }, "F1 toggle window");

		GuiGroupBox((Rectangle) { currentW - 195, currentH - 240, 190, 99 }, "Font size");

		GuiToggleGroup((Rectangle) { currentW - 142, currentH - 230, 60, 25 }, "8;16", & AnchoValue);
		GuiSpinner((Rectangle) { currentW - 140, currentH - 190, 119, 25 }, "Height ", & AltoValue, 1, 100, false);

		GuiDrawText("Position: ", (Rectangle) { currentW - 190, currentH - 100, 80, 10 }, 0, BLACK);
		GuiDrawText(positionbuffer, (Rectangle) { currentW - 140, currentH - 100, 80, 10 }, 0, BLACK);

		char mx[6] = "";
		sprintf(mx, "%d", GetMouseX() / pixelSize);
		char my[6] = "";
		sprintf(my, "%d", GetMouseY() / pixelSize);
		GuiDrawText(mx, (Rectangle) { currentW - 190, currentH - 80, 80, 10 }, 0, BLACK);
		GuiDrawText(my, (Rectangle) { currentW - 190, currentH - 70, 80, 10 }, 0, BLACK);

		font.h = AltoValue;
		font.w = AnchoValue + 1;

		//GuiSliderBar()
		//GuiMessageBox()
		//GuiDrawIcon();
	}


	//HELP WINDOW
	if (HelpWindowActive)
	{
		HelpWindowActive = !GuiWindowBox(			(Rectangle) { currentW - 400, currentH - 275, 200, 272}, "F2 - Help window");
		GuiDrawText("F1  - toggle main window",		(Rectangle) { currentW - 390, currentH - 250, 180, 10 }, 0, BLACK);
		GuiDrawText("F2 - toggle help window",		(Rectangle) { currentW - 390, currentH - 235, 180, 10 }, 0, BLACK);
		GuiDrawText("Left Cursor - 1 char less",	(Rectangle) { currentW - 390, currentH - 220, 180, 10 }, 0, BLACK);
		GuiDrawText("Right Cursor - 1 char more",	(Rectangle) { currentW - 390, currentH - 205, 180, 10 }, 0, BLACK);
		GuiDrawText("Up Cursor - less lot chars",	(Rectangle) { currentW - 390, currentH - 190, 180, 10 }, 0, BLACK);
		GuiDrawText("Down Cursor - more lot chars", (Rectangle) { currentW - 390, currentH - 175, 180, 10 }, 0, BLACK);
		GuiDrawText("Home key - go to start",		(Rectangle) { currentW - 390, currentH - 160, 180, 10 }, 0, BLACK);
		GuiDrawText("End key - go to finish",		(Rectangle) { currentW - 390, currentH - 145, 180, 10 }, 0, BLACK);
		GuiDrawText("Lshift + Lmouse - go to that char", (Rectangle) { currentW - 390, currentH - 130, 180, 10 }, 0, BLACK);
		GuiDrawText("z key - Zoom out",				(Rectangle) { currentW - 390, currentH - 115, 180, 10 }, 0, BLACK);
		GuiDrawText("x key - Zoom in",				(Rectangle) { currentW - 390, currentH - 100, 180, 10 }, 0, BLACK);
		GuiDrawText("Drop file to open",			(Rectangle) { currentW - 390, currentH -  85, 180, 10 }, 0, BLACK);
		GuiDrawText("Lctrl + a - Save file",		(Rectangle) { currentW - 390, currentH -  70, 180, 10 }, 0, BLACK);
		GuiDrawText("Alt + Lmouse - Edit raw",		(Rectangle) { currentW - 390, currentH -  55, 180, 10 }, 0, BLACK);
		GuiDrawText("N <-> M - byte displacement",	(Rectangle) { currentW - 390, currentH -  40, 180, 10 }, 0, BLACK);
		GuiDrawText("Ctrl U - Undo",				(Rectangle) { currentW - 390, currentH -  25, 180, 10 }, 0, BLACK);
		GuiDrawText("F - Auto Search Font",			(Rectangle) { currentW - 390, currentH -  10, 180, 10 }, 0, BLACK);
	}

	if (saveRequester)
	{
		int result = GuiMessageBox((Rectangle) { 300, 200, 300, 200 }, "Save", "Overwrite?", "Yes;No");
		
		if (result == 0 || result == 2)
			saveRequester = false;
		else if(result ==1)
		{
			saveFile();
			saveRequester = false;
		}
	}

	if (checkQuit)
	{
		int result = GuiMessageBox((Rectangle) { 300, 200, 300, 200 }, "File has been modified", "Quit?", "Yes;No");
		if (result == 0 || result == 2)
		{
			checkQuit = false;
		}
		else if (result == 1)
			quit = true;
	}

}



int saveFile()
{
	//Esto se tiene que dibujar en el loop de BeginDrwaing() ??
	
	//int result = GuiMessageBox((Rectangle) { 300, 200, 300, 200 }, "Save", "Overwrite?", "Yes;No");

	if (!SaveFileData(file.name, spyBuffer, file.size))
	return -1; //printf("No save file");

	//check errors
	return 0;
}


void setTitle()
{

}



// Hacemos busqueda de un caracter que esta siempre y es sencillo
// aunque puede tener variantes que hay que tener en cuenta.

//ES UN MEGA BUCLE BASTANTE FEO. MIRAR SI SE PUEDE OPTIMIZAR.
bool autoSearch()
{
	for (int pos = file.position+1; pos < file.size-8; pos++)
	{
		for (int font = 0; font < NUMFONTS; font++)
		{
			for (int shift = 0; shift <= 5; shift++)
			{
				if (SEARCH(pos, font, shift))
				{
					file.position = pos;
					return true;
				}
			}
		}
	}
	return false;
}