#pragma once

#include "classes.h"
#include "easings.h"
#include <string.h>

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
#define MAX_NAME_LENGTH     20


bool isMovingFast = false;

Camera2D camera = { 0 };
int generationFramesCounter = 0;
int generationPerSeconds = 1;

// neighbours
// 0 - moore
// 1 - von Neumann
// 2 - vertical
// 3 - horizontal
int neighbourType = 0;
const char *neighbours[MAX_NEIGHBOUR_TYPES] = 
{ "Moore", "Von Neumann", "Vertical", "Horizontal"};
int selectedNeighbourType = 0;
Color selectedColor = BLACK;
char newCellName[MAX_NAME_LENGTH];
int cellNameCount = 0;
bool mouseOnText = false;
bool selectedCellNameText = false;
int selectedIndex = -1;
Rectangle cellNameTextBox = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 50};
struct TargetCellRelationship newTargetRelationShips[MAX_RELATIONSHIPS];
int newTargetCellType = 0;
int newRelationShipType = 0;
int newResultCellType = 0;
int newAmountCellType = 0;

Font font;

// UI
int framesCounter = 0;
Rectangle leftUIBackground = {- (0.2 * SCREEN_WIDTH), 0, 0.2 * SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY};
Vector2 mousePosition;
bool isShowingUI = false;
bool isUIAnimationFinished = false;
bool isShowingCreateCellTypeDialog = false;
float uiTransparency = 0.8;

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
    
    for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
        newTargetRelationShips[i].index = -1;
    }

    cellTypes[0].color = WHITE;
    cellTypes[0].index = 0;

    // cellTypes[1].color = BLUE;
    // cellTypes[1].index = 1;


    // cellTypes[1].targetCellRelationship[0] = &relationshipSecond;
    // cellTypes[1].targetCellRelationship[0]->amount = 2;
    // cellTypes[1].targetCellRelationship[0]->relationshipType = "more";

    // for(int x = 0; x < MAX_CELLS_X; x++) {
    //     for(int y = 0; y < MAX_CELLS_Y; y++) {
    //         int num = rand() % 100 + 1;
    //         cells[x][y] = num <= 40 ? 1 : 0;
    //         renderCells[x][y] = cells[x][y];
    //     }
    // }

    

   // (*cellTypes[0]).index = 1; 
    SetTargetFPS(60);
}

void Loop() {
    while(WindowShouldClose() == false) {
        float deltaTime = GetFrameTime();
        generationFramesCounter++;

        char newTargetCellTypeText[6];
        sprintf(newTargetCellTypeText, "%d", generationFramesCounter);

        SetWindowTitle(newTargetCellTypeText);

        if(IsKeyPressed(KEY_LEFT_SHIFT)) {
            isMovingFast = true;
        }
        if(IsKeyReleased(KEY_LEFT_SHIFT)) {
            isMovingFast = false;
        }
        if(IsKeyPressed(KEY_P)) {
            isShowingUI = !isShowingUI;
            // Hiding other UI elements
            if(isShowingUI == false) {
                isShowingCreateCellTypeDialog = false;
            }
        }
        if(selectedCellType > -1 && isShowingUI == false && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX() / CELL_SIZE / 2 + 1;
            int mouseY = GetMouseY() / CELL_SIZE / 2 + 1;
            if(mouseX >= 0 && mouseX < MAX_CELLS_X) {
                if(mouseY >= 0 && mouseY < MAX_CELLS_Y) {
                    cells[mouseX][mouseY] = selectedCellType;
                    renderCells[mouseX][mouseY] = cells[mouseX][mouseY];
                }
            }
        }

        //if((generationFramesCounter / (TARGET_FPS * generationPerSeconds) % generationPerSeconds) == 1) {
        if(generationFramesCounter - (TARGET_FPS * generationPerSeconds) == 0) {
            generationFramesCounter = 0;
            if(isShowingUI == false) {
                for(int x = 0; x < MAX_CELLS_X; x++) {
                    for(int y = 0; y < MAX_CELLS_Y; y++) {
                        for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                            int cellIndex = cells[x][y];
                            if(cellTypes[cellIndex].targetCellRelationship[i] != 0) {
                                int neighbours = getGridNeighbours(x, y, renderCells, cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex);
                                bool relationshipFullfilled = false; 
                                if(cellTypes[cellIndex].targetCellRelationship[i]->relationshipType != 0) {
                                    if( strcmp(cellTypes[cellIndex].targetCellRelationship[i]->relationshipType, "equal") == 0)
                                    {
                                        // if(neighbours == cellTypes[cellIndex].targetCellRelationship[i]->amount && cellTypes[cellIndex].targetCellRelationship[i]->resultCellType != 0)
                                        // {
                                        //     cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                        // }
                                        if(neighbours == cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                        {
                                            cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                            relationshipFullfilled = true;
                                        }
                                    } else if(strcmp(cellTypes[cellIndex].targetCellRelationship[i]->relationshipType, "notequal") == 0)
                                    {
                                        if(neighbours != cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                        {
                                            cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                            relationshipFullfilled = true;
                                        }
                                    }else if(strcmp(cellTypes[cellIndex].targetCellRelationship[i]->relationshipType, "less") == 0)
                                    {
                                        if(neighbours < cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                        {
                                            cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                            relationshipFullfilled = true;
                                        }
                                    }else if(strcmp(cellTypes[cellIndex].targetCellRelationship[i]->relationshipType, "lessequal") == 0)
                                    {
                                        if(neighbours <= cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                        {
                                            cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                            relationshipFullfilled = true;
                                        }
                                    }else if(strcmp(cellTypes[cellIndex].targetCellRelationship[i]->relationshipType, "more") == 0)
                                    {
                                        if(neighbours > cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                        {
                                            cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                            relationshipFullfilled = true;
                                        }
                                    }else if(strcmp(cellTypes[cellIndex].targetCellRelationship[i]->relationshipType, "moreequal") == 0)
                                    {
                                        if(neighbours >= cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                        {
                                            cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                            relationshipFullfilled = true;
                                        }
                                    } else if(strcmp(cellTypes[cellIndex].targetCellRelationship[i]->relationshipType, "self") == 0) {
                                        cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                        relationshipFullfilled = true;
                                    }

                                    if(relationshipFullfilled == true) {
                                        break;
                                    }
                                }
                                
                            }
                        }
                    }
                }
            }
            
        }

        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                renderCells[x][y] = cells[x][y];
            }
        }

        mousePosition = GetMousePosition();

        UpdateMyCamera();
        
        
        BeginDrawing();
        ClearBackground(GRAY);
        BeginMode2D(camera);

        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                DrawRectangleV((Vector2) {CELL_SIZE / 2 + CELL_SEPARATION * x + CELL_SIZE * x, CELL_SIZE / 2 + CELL_SEPARATION * y + CELL_SIZE * y}, (Vector2) {CELL_SIZE, CELL_SIZE}, cellTypes[cells[x][y]].color);
            }
        }

        EndMode2D();

        HandleUI();

        EndDrawing();
    }

    CloseWindow();
}

// neighbours
// 0 - moore
// 1 - von Neumann
// 2 - vertical
// 3 - horizontal
int getGridNeighbours(int gridX, int gridY, int cells[MAX_CELLS_X] [MAX_CELLS_Y], int targetNeighbourIndex) {
    int result = 0;
    if(selectedNeighbourType == 0) {
        result += gridX - 1 >= 0 && cellTypes[cells[gridX - 1][gridY]].index == targetNeighbourIndex ? 1 : 0;
        result += gridX + 1 < MAX_CELLS_X && cellTypes[cells[gridX + 1][gridY]].index == targetNeighbourIndex ? 1 : 0;
        result += gridY - 1 >= 0 && cellTypes[cells[gridX][gridY - 1]].index == targetNeighbourIndex ? 1 : 0;
        result += gridY + 1 < MAX_CELLS_Y && cellTypes[cells[gridX][gridY + 1]].index == targetNeighbourIndex ? 1 : 0;
    } else if(selectedNeighbourType == 1) {
        for(int neighbourX = gridX - 1; neighbourX <= gridX + 1; neighbourX++) {
            for(int neighbourY = gridY - 1; neighbourY <= gridY + 1; neighbourY++) {
                if(neighbourX >= 0 && neighbourX < MAX_CELLS_X && neighbourY >= 0 && neighbourY < MAX_CELLS_Y) {
                    if((neighbourX != gridX || neighbourY != gridY) && cellTypes[cells[neighbourX][neighbourY]].index == targetNeighbourIndex) {
                        result += 1;
                    }
                }
            }
        }
    } else if(selectedNeighbourType == 2) {
        result += gridY - 1 >= 0 && cellTypes[cells[gridX][gridY - 1]].index == targetNeighbourIndex ? 1 : 0;
        result += gridY + 1 < MAX_CELLS_Y && cellTypes[cells[gridX][gridY + 1]].index == targetNeighbourIndex ? 1 : 0;
    } else if(selectedNeighbourType == 3) {
        result += gridX - 1 >= 0 && cellTypes[cells[gridX - 1][gridY]].index == targetNeighbourIndex ? 1 : 0;
        result += gridX + 1 < MAX_CELLS_X && cellTypes[cells[gridX + 1][gridY]].index == targetNeighbourIndex ? 1 : 0;
    }

    return result;
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
    if(isShowingUI == true) {
        if(isUIAnimationFinished == false) {
            framesCounter++;

            leftUIBackground.x = EaseLinearNone(framesCounter, -(0.5 * leftUIBackground.width), leftUIBackground.width, 6);

            if(framesCounter >= 6) {
                framesCounter = 0;
                isUIAnimationFinished = true;
            }
            DrawRectanglePro(leftUIBackground, (Vector2) {leftUIBackground.width / 2, 0}, 0, Fade(LIGHTGRAY, uiTransparency));
        } else {
            int backgroundPositionX = leftUIBackground.width / 2;
            DrawRectanglePro(leftUIBackground, (Vector2) {backgroundPositionX, 0}, 0, Fade(LIGHTGRAY, uiTransparency));
            // I need to change the rest to use the backgroundPositionX in order to achieve a relative position
            
            if(GuiButton((Rectangle) {0.085 * SCREEN_WIDTH, 0.52 * SCREEN_HEIGHT, 100, 20}, "Add Cell Type")) {
                isShowingCreateCellTypeDialog = true;
                selectedIndex = -1;
            }
        }
        

        //DrawTextEx(font, "Von Neumann", (Vector2) {0.07 * SCREEN_WIDTH, 0.62 * SCREEN_HEIGHT}, 20, 0, BLACK);
        //if(isUIAnimationFinished == true) {
            //DrawRectangle(0, 0, 0.2 * SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
            //GuiCheckBox((Rectangle){ 0.07 * SCREEN_WIDTH, SCREEN_HEIGHT / 2, 20, 20 }, "Draw Ring", false);
            
        //}
        // new cell type UI
        if(isShowingCreateCellTypeDialog) {
            int newCellWidth = 0.25 * SCREEN_WIDTH;
            int newCellHeight = 0.25 * SCREEN_HEIGHT;
            int positionX = 0.5 * SCREEN_WIDTH - newCellWidth / 2;
            int positionY = 0.5 * SCREEN_HEIGHT - newCellHeight / 2;


            GuiScrollPanel((Rectangle) {positionX, positionY, newCellWidth, newCellHeight}, )
            

            DrawRectangle(positionX, positionY, newCellWidth, newCellHeight, Fade(LIGHTGRAY, uiTransparency));

            if(GuiButton((Rectangle) {positionX + newCellWidth - 25, positionY + 5, 20, 20}, "x")) {
                isShowingCreateCellTypeDialog = false;
            }

            DrawText("Neighbours:", positionX + 10, positionY + 20, 8, BLACK);
            if(GuiButton((Rectangle) {positionX + 10, positionY + 35, 100, 20}, neighbours[selectedNeighbourType])) {
                if(selectedNeighbourType >= MAX_NEIGHBOUR_TYPES - 1) {
                    selectedNeighbourType = 0;
                } else {
                    selectedNeighbourType ++;
                }
            }
            int cellColorPickerSize = 75;
            
            DrawText("Color:", positionX + newCellWidth / 2, positionY + 20, 8, selectedColor);
            selectedColor = GuiColorPicker((Rectangle) {positionX + newCellWidth / 2, positionY + 35, cellColorPickerSize, cellColorPickerSize}, selectedColor);
            
            DrawText("Relationships:", positionX + 10, positionY + 75, 8, BLACK);
            if(GuiButton((Rectangle) {positionX + 10, positionY + 90, 100, 20}, "Add Relationship")) {
                
                for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                    if(newTargetRelationShips[i].index == -1) {
                        newTargetRelationShips[i].index = i;
                        newTargetRelationShips[i].amount = newAmountCellType;
                        newAmountCellType = 0;
                        // char newRelationShipTypeText[1];
                        // sprintf(newRelationShipTypeText, "%d", newRelationShipType);
                        // newTargetRelationShips[i].relationshipType = newRelationShipTypeText;

                        if(newRelationShipType == 0) {
                            newTargetRelationShips[i].relationshipType = "equal";
                        }
                        if(newRelationShipType == 1) {
                            newTargetRelationShips[i].relationshipType = "notequal";
                        }
                        if(newRelationShipType == 2) {
                            newTargetRelationShips[i].relationshipType = "less";
                        }
                        if(newRelationShipType == 3) {
                            newTargetRelationShips[i].relationshipType = "lessequal";
                        }
                        if(newRelationShipType == 4) {
                            newTargetRelationShips[i].relationshipType = "more";
                        }
                        if(newRelationShipType == 5) {
                            newTargetRelationShips[i].relationshipType = "moreequal";
                        }
                        if(newRelationShipType == 6) {
                            newTargetRelationShips[i].relationshipType = "self";
                        }

                        newTargetRelationShips[i].resultCellTypeIndex = newResultCellType;
                        newTargetRelationShips[i].targetCellTypeIndex = newTargetCellType;

                        break;
                    }
                }
            }

            char newTargetCellTypeText[2];
            sprintf(newTargetCellTypeText, "%d", newTargetCellType);
            //printf("%c", c);

            if(GuiButton((Rectangle) {positionX + 10, positionY + 115, 20, 20}, newTargetCellTypeText)) {
                if(newTargetCellType >= MAX_CELLTYPES - 1) {
                    newTargetCellType = 0;
                } else {
                    newTargetCellType ++;
                }
            }

            sprintf(newTargetCellTypeText, "%d", newRelationShipType);
            if(GuiButton((Rectangle) {positionX + 35, positionY + 115, 20, 20}, newTargetCellTypeText)) {
                if(newRelationShipType >= 7 - 1) {
                    newRelationShipType = 0;
                } else {
                    newRelationShipType ++;
                }
            }

            sprintf(newTargetCellTypeText, "%d", newResultCellType);
            if(GuiButton((Rectangle) {positionX + 60, positionY + 115, 20, 20}, newTargetCellTypeText)) {
                if(newResultCellType >= MAX_CELLTYPES - 1) {
                    newResultCellType = 0;
                } else {
                    newResultCellType ++;
                }
            }

            sprintf(newTargetCellTypeText, "%d", newAmountCellType);
            if(GuiButton((Rectangle) {positionX + 85, positionY + 115, 20, 20}, newTargetCellTypeText)) {
                if(newAmountCellType >= 9 - 1) {
                    newAmountCellType = 0;
                } else {
                    newAmountCellType ++;
                }
            }

            for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                if(newTargetRelationShips[i].index > -1) {
                   // DrawText("Rel-" + i, positionX+10 + i * 45, positionY + 115, 8, BLACK);
                    if(GuiButton((Rectangle) {positionX + 10 + i * 25, positionY + 140, 20, 20}, "R")) {
                        newTargetRelationShips[i].index = -1;
                    }
                }
            }

            if(GuiButton((Rectangle) {positionX + 10, positionY + newCellHeight - 30, 100, 20}, "Save")) {
                if(selectedIndex == -1) {
                    for(int i = 0; i < MAX_CELLTYPES; i++) {
                        if(cellTypes[i].index == -1) {
                            cellTypes[i].color = selectedColor;
                            cellTypes[i].name = "TST " + i;
                            cellTypes[i].index = i;
                            cellTypes[i].neighbourType = selectedNeighbourType;
                            for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
                                cellTypes[i].targetCellRelationship[x] = malloc(sizeof(T_TargetCellRelationship));
                                cellTypes[i].targetCellRelationship[x]->amount = newTargetRelationShips[x].amount;
                                cellTypes[i].targetCellRelationship[x]->index = newTargetRelationShips[x].index;
                                cellTypes[i].targetCellRelationship[x]->relationshipType = newTargetRelationShips[x].relationshipType;
                                                            
                                cellTypes[i].targetCellRelationship[x]->resultCellTypeIndex = newTargetRelationShips[x].resultCellTypeIndex;
                                cellTypes[i].targetCellRelationship[x]->targetCellTypeIndex = newTargetRelationShips[x].targetCellTypeIndex;
                            }

                            break;
                        }
                    }
                } else {
                    cellTypes[selectedIndex].color = selectedColor;
                    cellTypes[selectedIndex].name = "TST " + selectedIndex;
                    cellTypes[selectedIndex].neighbourType = selectedNeighbourType;
                    for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
                        cellTypes[selectedIndex].targetCellRelationship[x] = malloc(sizeof(T_TargetCellRelationship));
                        cellTypes[selectedIndex].targetCellRelationship[x]->amount = newTargetRelationShips[x].amount;
                        cellTypes[selectedIndex].targetCellRelationship[x]->index = newTargetRelationShips[x].index;
                        cellTypes[selectedIndex].targetCellRelationship[x]->relationshipType = newTargetRelationShips[x].relationshipType;
                                                    
                        cellTypes[selectedIndex].targetCellRelationship[x]->resultCellTypeIndex = newTargetRelationShips[x].resultCellTypeIndex;
                        cellTypes[selectedIndex].targetCellRelationship[x]->targetCellTypeIndex = newTargetRelationShips[x].targetCellTypeIndex;
                    }
                    selectedIndex = -1;
                }
                
                isShowingCreateCellTypeDialog = false;
            }
            
            // cellNameTextBox.x = positionX + cellNameTextBox.width / 2;
            // cellNameTextBox.y = positionY + cellNameTextBox.height / 2;
            // if(CheckCollisionPointRec(GetMousePosition(), cellNameTextBox)) {
            //     mouseOnText = true;
            // } else {
            //     mouseOnText = false;
            // }
            // if(mouseOnText == true) {
            //     if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            //         selectedCellNameText = true;
            //     }

            // } else if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            //     selectedCellNameText = false;
            // }

        }
        
    } else if(isUIAnimationFinished == true) {
        framesCounter++;
        
        leftUIBackground.x = EaseLinearNone(framesCounter, 0.5 * leftUIBackground.width, -(leftUIBackground.width), 6);

        if(framesCounter >= 6) {
            framesCounter = 0;
            isUIAnimationFinished = false;
        }
        DrawRectanglePro(leftUIBackground, (Vector2) {leftUIBackground.width / 2, 0}, 0, Fade(LIGHTGRAY, uiTransparency));
    }

    // I can use this when creating cellTypes if I want total control over colors
    //GuiColorPicker((Rectangle) {200, 200, 300, 300}, );
    
    DrawRectangle(SCREEN_WIDTH * 0.94, 0, SCREEN_WIDTH * 0.06, SCREEN_HEIGHT, Fade(LIGHTGRAY, uiTransparency));

    // Cells Menu
    for(int i = 0; i < MAX_CELLTYPES; i++) {
        if(cellTypes[i].index > -1) {
            int cellPositionX = 0.97 * SCREEN_WIDTH;
            int cellPositionY = 0.13 * SCREEN_HEIGHT + 0.05*SCREEN_HEIGHT * i;
            int cellWidth = 25;
            int cellHeight = 25;
            if(CheckCollisionPointRec(mousePosition,
            (Rectangle) {cellPositionX, cellPositionY, cellWidth, cellHeight})) {
                hoveredCellType = cellTypes[i].index;
            } else {
                hoveredCellType = -1;
            }
            
            if(GuiButton((Rectangle) {cellPositionX - (cellWidth * 1.25), cellPositionY, cellWidth, cellHeight}, "...")) {
                
                selectedIndex = i;
                selectedColor = cellTypes[selectedIndex].color;
                selectedNeighbourType = cellTypes[selectedIndex].neighbourType;
                for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
                    if(cellTypes[selectedIndex].targetCellRelationship[x] != 0) {
                        newTargetRelationShips[x].amount = cellTypes[selectedIndex].targetCellRelationship[x]->amount;
                        newTargetRelationShips[x].index = cellTypes[selectedIndex].targetCellRelationship[x]->index;
                        newTargetRelationShips[x].relationshipType = cellTypes[selectedIndex].targetCellRelationship[x]->relationshipType;
                                                    
                        newTargetRelationShips[x].resultCellTypeIndex = cellTypes[selectedIndex].targetCellRelationship[x]->resultCellTypeIndex;
                        newTargetRelationShips[x].targetCellTypeIndex = cellTypes[selectedIndex].targetCellRelationship[x]->targetCellTypeIndex;
                    }
                    
                }
                isShowingUI = true;
                isShowingCreateCellTypeDialog = true;
            }

            DrawRectangle(cellPositionX, cellPositionY, cellWidth, cellHeight, cellTypes[i].color);
            if(hoveredCellType > -1) {
                DrawRectangle(cellPositionX, cellPositionY, cellWidth, cellHeight, Fade(WHITE, uiTransparency));
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    
                    selectedCellType = cellTypes[i].index;
                }
            }
            if(selectedCellType == cellTypes[i].index) {
                DrawRectangleLines(cellPositionX, cellPositionY, cellWidth + 1, cellHeight + 1, BLACK);
            }
        }
    }
    DrawText("Cells", 0.97 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT, 10, BLACK);
}
