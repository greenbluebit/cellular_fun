#pragma once

#include "classes.h"
#include "easings.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui/src/raygui.h"


#define SCREEN_WIDTH        1480
#define SCREEN_HEIGHT       800
#define MOVE_SPEED          10
#define FAST_MOVE_SPEED     20
#define MAX_CELLTYPES       10
#define MAX_NEIGHBOUR_TYPES 4
#define MAX_CELLS_X         500
#define MAX_CELLS_Y         500
#define CELL_SIZE           4
#define CELL_SEPARATION     2
#define TARGET_FPS          60


bool isMovingFast = false;
bool isShowingUI = false;
bool isUIAnimationFinished = false;
Camera2D camera = { 0 };
int generationFramesCounter = 0;
int generationPerSeconds = 2;

// neighbours
// 0 - moore
// 1 - von Neumann
// 2 - vertical
// 3 - horizontal
int neighbourType = 0;
const char *neighbours[MAX_NEIGHBOUR_TYPES] = { "Moore", "Von Neumann", "Vertical", "Horizontal"};
int currentNeighbour = 0;

Font font;

// UI
int framesCounter = 0;
Rectangle leftUIBackground = {- (0.2 * SCREEN_WIDTH), 0, 0.2 * SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY};
Vector2 mousePosition;


// cellTypes
struct CellType cellTypes[MAX_CELLTYPES];
int selectedCellType = -1;
int hoveredCellType = -1;

// cells
int cells[MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };
int renderCells [MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };

// PROTOTYPES
void UpdateMyCamera();
void UpdateUI();

void Setup() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Cellular Fun - by Ted");

    font = LoadFont("resources/fonts/setback.png");
    camera.target = (Vector2) { (SCREEN_WIDTH / 2) , (SCREEN_HEIGHT / 2)};
    camera.offset = camera.target;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    for(int i = 0; i < MAX_CELLTYPES; i++) {
        cellTypes[i].index = -1;           
    }

    cellTypes[0].color = RED;
    cellTypes[0].index = 0;
    struct TargetCellRelationship relationship = { &cellTypes[1], 2, &cellTypes[1] };
    cellTypes[0].targetCellRelationship[0] = &relationship;
    cellTypes[0].targetCellRelationship[0]->amount = 100;
    cellTypes[1].color = BLUE;
    cellTypes[1].index = 1;
    cellTypes[2].color = YELLOW;
    cellTypes[2].index = 2;
    cellTypes[3].color = ORANGE;
    cellTypes[3].index = 3;
    cellTypes[4].color = BROWN;
    cellTypes[4].index = 4;

    
   // (*cellTypes[0]).index = 1; 
    SetTargetFPS(60);
}

void Loop() {
    while(WindowShouldClose() == false) {
        float deltaTime = GetFrameTime();
        generationFramesCounter++;

        if(IsKeyPressed(KEY_LEFT_SHIFT)) {
            isMovingFast = true;
        }
        if(IsKeyReleased(KEY_LEFT_SHIFT)) {
            isMovingFast = false;
        }

        if((generationFramesCounter / (TARGET_FPS * generationPerSeconds) % generationPerSeconds) == 1) {

            generationFramesCounter = 0;
        }

        mousePosition = GetMousePosition();

        UpdateMyCamera();
        
        
        
        BeginDrawing();
        ClearBackground(GRAY);
        BeginMode2D(camera);

        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                DrawRectangleV((Vector2) {CELL_SIZE / 2 + CELL_SEPARATION * x + CELL_SIZE * x, CELL_SIZE / 2 + CELL_SEPARATION * y + CELL_SIZE * y}, (Vector2) {CELL_SIZE, CELL_SIZE}, cellTypes[0].color);
            }
        }

        EndMode2D();

        HandleUI();

        EndDrawing();
    }

    CloseWindow();
}

void UpdateMyCamera() {
    if(IsKeyDown(KEY_A)) {
        camera.target.x -= (isMovingFast) ? FAST_MOVE_SPEED : MOVE_SPEED;
    }
    if(IsKeyDown(KEY_D)) {
        camera.target.x += (isMovingFast) ? FAST_MOVE_SPEED : MOVE_SPEED;
    }

    if(IsKeyDown(KEY_W)) {
        camera.target.y -= (isMovingFast) ? FAST_MOVE_SPEED : MOVE_SPEED;
    }
    if(IsKeyDown(KEY_S)) {
        camera.target.y += (isMovingFast) ? FAST_MOVE_SPEED : MOVE_SPEED;
    }

    camera.zoom += ((float)GetMouseWheelMove()*0.05f);

    if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    else if (camera.zoom < 0.25f) camera.zoom = 0.25f;
}

void HandleUI() {
    if(IsKeyPressed(KEY_P)) {
        isShowingUI = !isShowingUI;
    }
    
    if(isShowingUI == true) {
        if(isUIAnimationFinished == false) {
            framesCounter++;

            leftUIBackground.x = EaseLinearNone(framesCounter, -(0.5 * leftUIBackground.width), leftUIBackground.width, 6);

            if(framesCounter >= 6) {
                framesCounter = 0;
                isUIAnimationFinished = true;
            }
            DrawRectanglePro(leftUIBackground, (Vector2) {leftUIBackground.width / 2, 0}, 0, LIGHTGRAY);
        } else {
            DrawRectanglePro(leftUIBackground, (Vector2) {leftUIBackground.width / 2, 0}, 0, LIGHTGRAY);
            DrawText("Neighbours:", 0.035 * SCREEN_WIDTH, 0.625 * SCREEN_HEIGHT, 8, BLACK);
            if(GuiButton((Rectangle) {0.085 * SCREEN_WIDTH, 0.62 * SCREEN_HEIGHT, 100, 20}, neighbours[currentNeighbour])) {
                if(currentNeighbour >= MAX_NEIGHBOUR_TYPES - 1) {
                    currentNeighbour = 0;
                } else {
                    currentNeighbour ++;
                }
            }
        }
        

        //DrawTextEx(font, "Von Neumann", (Vector2) {0.07 * SCREEN_WIDTH, 0.62 * SCREEN_HEIGHT}, 20, 0, BLACK);
        //if(isUIAnimationFinished == true) {
            //DrawRectangle(0, 0, 0.2 * SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
            //GuiCheckBox((Rectangle){ 0.07 * SCREEN_WIDTH, SCREEN_HEIGHT / 2, 20, 20 }, "Draw Ring", false);
            
        //}
        
    } else if(isUIAnimationFinished == true) {
        framesCounter++;
        
        leftUIBackground.x = EaseLinearNone(framesCounter, 0.5 * leftUIBackground.width, -(leftUIBackground.width), 6);

        if(framesCounter >= 6) {
            framesCounter = 0;
            isUIAnimationFinished = false;
        }
        DrawRectanglePro(leftUIBackground, (Vector2) {leftUIBackground.width / 2, 0}, 0, LIGHTGRAY);
    }

    // I can use this when creating cellTypes if I want total control over colors
    //GuiColorPicker((Rectangle) {200, 200, 300, 300}, );
    

    // Cells Menu
    for(int i = 0; i < MAX_CELLTYPES; i++) {
        if(cellTypes[i].index > -1) {
            if(CheckCollisionPointRec(mousePosition,
            (Rectangle) {0.97 * SCREEN_WIDTH, 0.13 * SCREEN_HEIGHT + 0.05*SCREEN_HEIGHT * i, 25, 25})) {
                hoveredCellType = cellTypes[i].index;
            } else {
                hoveredCellType = -1;
            }
            
            DrawRectangle(0.97 * SCREEN_WIDTH, 0.13 * SCREEN_HEIGHT + 0.05*SCREEN_HEIGHT * i, 25, 25, cellTypes[i].color);
            if(hoveredCellType > -1) {
                DrawRectangle(0.97 * SCREEN_WIDTH, 0.13 * SCREEN_HEIGHT + 0.05*SCREEN_HEIGHT * i, 25, 25, Fade(WHITE, 0.6f));
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    
                    selectedCellType = cellTypes[i].index;
                }
            }
            if(selectedCellType == cellTypes[i].index) {
                DrawRectangleLines(0.97 * SCREEN_WIDTH, 0.13 * SCREEN_HEIGHT + 0.05*SCREEN_HEIGHT * i, 26, 26, BLACK);
            }
        }
    }
    DrawText("Cells", 0.97 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT, 10, BLACK);
}