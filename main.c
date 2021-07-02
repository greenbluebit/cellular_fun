#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define RECS_WIDTH              5
#define RECS_HEIGHT             5

#define MAX_RECS_X              1480/RECS_WIDTH
#define MAX_RECS_Y              800/RECS_HEIGHT
#define DELAY                   200

#define PLAY_TIME_IN_FRAMES     240                 // At 60 fps = 4 seconds
#define RANDOM_CHANCE           50
typedef struct Cell {
    float x;
    float y;
    float width;
    float height;
    bool occupied;
} Cell;

int getGridNeighbous(int gridX, int gridY, int recs[MAX_RECS_X][MAX_RECS_Y]) {
    int result = 0;
    for(int neighbourX = gridX - 1; neighbourX <= gridX + 1; neighbourX++) {
        for(int neighbourY = gridY - 1; neighbourY <= gridY + 1; neighbourY++) {
            if(neighbourX >= 0 && neighbourX < MAX_RECS_X && neighbourY >= 0 && neighbourY < MAX_RECS_Y) {
                if(neighbourX != gridX || neighbourY != gridY) {
                    result += recs[neighbourX] [neighbourY];
                }
            }
        }
    }

    return result;
}

void LogCustom(int msgType, const char *text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", timeStr);

    switch (msgType)
    {
        case LOG_INFO: printf("[INFO] : "); break;
        case LOG_ERROR: printf("[ERROR]: "); break;
        case LOG_WARNING: printf("[WARN] : "); break;
        case LOG_DEBUG: printf("[DEBUG]: "); break;
        default: break;
    }

    vprintf(text, args);
    printf("\n");
}

bool CheckMouseCollision(int gridX, int gridY, int mouseX, int mouseY)
{
    bool collision = false;

    if ((gridX < (mouseX + RECS_WIDTH / 2) && (gridX + RECS_WIDTH) > mouseX) &&
        (gridY < (mouseY + RECS_HEIGHT / 2) && (gridY + RECS_HEIGHT) > mouseY)) collision = true;

    return collision;
}


int main(void)
{
    // First thing we do is setting our custom logger to ensure everything raylib logs
    // will use our own logger instead of its internal one
    SetTraceLogCallback(LogCustom);

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1480;
    const int screenHeight = 800;
    int range = 100 - 1 + 1;
    

    InitWindow(screenWidth, screenHeight, "Ted - Cellurar fun");

    //Cell recs[MAX_RECS_X*MAX_RECS_Y] = { 0 };
    int recs[MAX_RECS_X] [MAX_RECS_Y] = { 0 };
    int recs_final[MAX_RECS_X] [MAX_RECS_Y] = { 0 };

    for (int x = 0; x < MAX_RECS_X; x++)
    {
        for (int y = 0; y < MAX_RECS_Y; y++)
        {
            int num = rand() % range + 1;
            recs[x][y] = num <= RANDOM_CHANCE ? 1 : 0;
            recs_final[x][y] = recs[x][y];
        }
    }

    int state = 0;

    SetTargetFPS(15);
    //--------------------------------------------------------------------------------------

    int max_run = 5;
    int count = 0;
    bool should_limit = false;
    bool should_run = false;
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // UPDATE
        // --------------------------------------------------------------------

        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX() / RECS_WIDTH - RECS_WIDTH / 2 + 1;
            int mouseY = GetMouseY() / RECS_HEIGHT - RECS_HEIGHT / 2 + 1;
            if(mouseY >= 0 && mouseY < MAX_RECS_Y){
                if(mouseX>= 0 && mouseX< MAX_RECS_X) {
                    recs[mouseX] [mouseY] = 1;
                    recs_final[mouseX] [mouseY] = 1;
                }
            }
        }
        if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            int mouseX = GetMouseX() / RECS_WIDTH - RECS_WIDTH / 2 + 1;
            int mouseY = GetMouseY() / RECS_HEIGHT - RECS_HEIGHT / 2 + 1;
            if(mouseY >= 0 && mouseY < MAX_RECS_Y){
                if(mouseX>= 0 && mouseX< MAX_RECS_X) {
                    recs[mouseX] [mouseY] = 0;
                    recs_final[mouseX] [mouseY] = 0;
                }
            }
        }
        if(IsKeyPressed(KEY_SPACE)) {
            should_run = !should_run;
        }
        if(IsKeyPressed(KEY_R)) {
            for(int y = 0; y < MAX_RECS_Y; y++) {
                for (int x = 0; x < MAX_RECS_X; x++) {
                    recs[x][y] = 0;
                    recs_final[x][y] = 0;                      
                }
            }
        }
        // --------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

        if (state == 0)
        {
            for(int y = 0; y < MAX_RECS_Y; y++) {
                for (int x = 0; x < MAX_RECS_X; x++) {
                    if(recs_final[x][y] == 1) {
                        DrawRectangleV((Vector2) {RECS_WIDTH/2 + RECS_WIDTH*x, RECS_HEIGHT/2 + RECS_HEIGHT*y}, (Vector2) {RECS_WIDTH, RECS_HEIGHT}, BLACK);
                    }                        
                }
            }
        }
        else if (state == 1) DrawText("PRESS [SPACE] TO PLAY AGAIN!", 240, 200, 20, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------

        // Post-Update
        //----------------------------------------------------------------------------------
    
        if(should_run == true && (should_limit == false || count < max_run) ) {
            for (int x = 0; x < MAX_RECS_X; x++)
            {
                for (int y = 0; y < MAX_RECS_Y; y++)
                {
                    int neighbours = getGridNeighbous(x,y, recs_final);

                    if((recs[x][y] == 0 && neighbours == 3) || (recs[x][y] == 1 && (neighbours == 2 || neighbours == 3))) {
                        recs[x][y] = 1;
                    } else if(neighbours <= 1 || neighbours >= 4) {
                        recs[x][y] = 0;
                    }
                }
            }

            for (int y = 0; y < MAX_RECS_Y; y++)
            {
                for (int x = 0; x < MAX_RECS_X; x++)
                {
                    recs_final[x][y] = recs[x][y];
                }
            }
            count++;
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}