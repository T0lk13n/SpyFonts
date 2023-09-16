#include <stdlib.h>
#include "raylib.h"

#define NUMBALLS 100

double gravedad = 0.9;

typedef struct bola
{
    double x;
    double y;
    double vx;
    double vy;
    int   vida;
    //float rebote;
}bola_t;


int main(void)
{
    SetTraceLogLevel(LOG_NONE);

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Prueba fisicas gravedad y mierdas");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    
    //iniciamos las bolas
    bola_t bolas[NUMBALLS];
    for (int i = 0; i < NUMBALLS; i++)
    {
        bolas[i].x  = 400 + (rand() % 4) - 2;
        bolas[i].y  = 100;
        bolas[i].vx = ((rand() % 3) + 1);
        bolas[i].vy = -((rand() % 5) + 8);
        bolas[i].vida = (rand() % 30) + 30;
    }


    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        for (int i = 0; i < NUMBALLS; i++)
        {
            if(bolas[i].vida >= 0)
            { 
                bolas[i].vida -= 1;

                bolas[i].x += bolas[i].vx;
                bolas[i].y += bolas[i].vy;

                bolas[i].vy += gravedad;

                //Limitar la velocidad?
                if (bolas[i].vy > 8)
                    bolas[i].vy = 8;

                if (bolas[i].y > 300)
                    bolas[i].vy = -(bolas[i].vy * 0.9);
                
            }
            else
            {
                bolas[i].x = 400 + (rand() % 4)-2;
                bolas[i].y = 100;
                bolas[i].vx = ((rand() % 3)+1);
                bolas[i].vy = -((rand() % 5)+8);
                bolas[i].vida = (rand() % 30)+30;
            }
        }


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        DrawLine(0, 102, 410, 102, WHITE);
        DrawLine(410, 102, 410, 310, WHITE);
        DrawLine(410, 310, 800, 310, WHITE);

        for (int i = 0; i < NUMBALLS; i++)
                DrawPixel((int)bolas[i].x, (int)bolas[i].y, WHITE);

        //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}