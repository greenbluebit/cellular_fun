#pragma once

#include "ui.h"
#include "cJSON/cJSON.h"


#define MOVE_SPEED          10
#define FAST_MOVE_SPEED     20

#define CELL_SIZE               4
#define CELL_SEPARATION         2
#define TARGET_FPS              60

#define MAX_CAMERA_ZOOM         3.5f // zoom in
#define MIN_CAMERA_ZOOM         0.9f // zoom out


GuiFileDialogState fileDialogState;
char fileNameToLoad[512] = { 0 };

bool isMovingFast = false;

Camera2D camera = { 0 };
int generationFramesCounter = 0;
int generationPerSeconds = 1;

Font font;

int testSize = 0;

// PROTOTYPES
void UpdateMyCamera();
void HandleUI();

char *PrintJson(void) {
    // TEDO I can reuse variables in next version
    char *string = NULL;
    cJSON *jsonCellTypes = NULL;
    cJSON *jsonCellType = NULL;
    cJSON *jsonIndex = NULL;
    cJSON *jsonNeighbourType = NULL;
    cJSON *jsonColor = NULL;
    cJSON *jsonR = NULL;
    cJSON *jsonG = NULL;
    cJSON *jsonB = NULL;
    cJSON *jsonA = NULL;
    cJSON *jsonName = NULL;
    cJSON *jsonTargetRelationships = NULL;
    cJSON *jsonTargetIndex = NULL;
    cJSON *jsonRelationshipType = NULL;
    cJSON *jsonAmount = NULL;
    cJSON *jsonRelationshipIndex = NULL;
    cJSON *jsonResultIndex = NULL;
    cJSON *jsonCells = NULL;
    cJSON *jsonCellIndex = NULL;

    cJSON *parent = cJSON_CreateObject();
    if(parent == NULL) {
        goto end;
    }

    jsonCellTypes = cJSON_CreateArray();
    cJSON_AddItemToObject(parent, "celltypes", jsonCellTypes);

    for(int i = 0; i < MAX_CELLTYPES; i++) {
        jsonCellType = cJSON_CreateObject();
        if(jsonCellType == NULL) { // TEDO I Need to do this check at the rest of the json initializers
            goto end;
        }
        cJSON_AddItemToArray(jsonCellTypes, jsonCellType);

        jsonIndex = cJSON_CreateNumber(cellTypes[i].index);
        cJSON_AddItemToObject(jsonCellType, "index", jsonIndex);

        jsonNeighbourType = cJSON_CreateNumber(cellTypes[i].neighbourType);
        cJSON_AddItemToObject(jsonCellType, "neighbourtype", jsonNeighbourType);

        jsonColor = cJSON_CreateObject();
        cJSON_AddItemToObject(jsonCellType, "color", jsonColor);

        jsonR = cJSON_CreateNumber(cellTypes[i].color.r);
        cJSON_AddItemToObject(jsonColor, "r", jsonR);

        jsonG = cJSON_CreateNumber(cellTypes[i].color.g);
        cJSON_AddItemToObject(jsonColor, "g", jsonG);

        jsonB = cJSON_CreateNumber(cellTypes[i].color.b);
        cJSON_AddItemToObject(jsonColor, "b", jsonB);

        jsonName = cJSON_CreateString(cellTypes[i].name);
        cJSON_AddItemToObject(jsonCellType, "name", jsonName);

        jsonTargetRelationships = cJSON_CreateArray();
        cJSON_AddItemToObject(jsonCellType, "targetcellrelationship", jsonTargetRelationships);

        for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
            cJSON *jsonTargetRelationship = cJSON_CreateObject();
            cJSON_AddItemToArray(jsonTargetRelationships, jsonTargetRelationship);

            jsonTargetIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->targetCellTypeIndex);
            cJSON_AddItemToObject(jsonTargetRelationship, "targetindex", jsonTargetIndex);

            jsonRelationshipType = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->relationshipType);
            cJSON_AddItemToObject(jsonTargetRelationship, "relationshiptype", jsonRelationshipType);

            jsonAmount = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->amount);
            cJSON_AddItemToObject(jsonTargetRelationship, "amount", jsonAmount);

            jsonRelationshipIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->index);
            cJSON_AddItemToObject(jsonTargetRelationship, "index", jsonRelationshipIndex);

            jsonResultIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->resultCellTypeIndex);
            cJSON_AddItemToObject(jsonResultIndex, "resultindex", jsonResultIndex);
        }
    }

    string = cJSON_Print(parent);
    if(string == NULL) {
        fprintf(stderr, "Failed to print. \n");
    }

    end:
        cJSON_Delete(parent);
        return string;
}

void Setup() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Cellular Fun");

    fileDialogState = InitGuiFileDialog(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, GetWorkingDirectory(), false);
    char fileNameToLoad[512] = { 0 };
    
    font = LoadFont("resources/fonts/setback.png");
    camera.target = (Vector2) { (SCREEN_WIDTH / 2) , (SCREEN_HEIGHT / 2)};
    camera.offset = camera.target;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetupCells(); 

    for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
        newTargetRelationShips[i].index = -1;
    }

    SetTargetFPS(60);
}

void Loop() {

    SetExitKey(0); // This way I set the exit key. Atm, on Escape, it exits
    //DisableCursor();
    while(WindowShouldClose() == false) {
        generationFramesCounter+= generationSpeedMultiplier;

        if(IsKeyPressed(KEY_LEFT_SHIFT)) {
            isMovingFast = true;
        }
        if(IsKeyReleased(KEY_LEFT_SHIFT)) {
            isMovingFast = false;
        }
        if(IsKeyPressed(KEY_ESCAPE)) {
            isShowingUI = !isShowingUI;
            // Hiding other UI elements
            if(isShowingUI == false) {
                isShowingCreateCellTypeDialog = false;
            }
        }

        if(fileDialogState.SelectFilePressed) {
            if(IsFileExtension(fileDialogState.fileNameText, ".json")) {
                strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                PrintJson();
            }
        }

        // Handle Cell Interaction
        Vector2 mouseToWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
        
        if(selectedCellType > -1 && isShowingUI == false && isMouseOverUI == false && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            int mouseX = (mouseToWorldPosition.x) / (CELL_SIZE + CELL_SEPARATION) + 1;
            int mouseY = (mouseToWorldPosition.y) / (CELL_SIZE + CELL_SEPARATION) + 1;

            if(mouseX >= 0 && mouseX < MAX_CELLS_X) {
                if(mouseY >= 0 && mouseY < MAX_CELLS_Y) {
                    cells[mouseX][mouseY] = selectedCellType;
                    finalCells[mouseX][mouseY] = cells[mouseX][mouseY];

                    for(int neighbourX = mouseX - brushSize; neighbourX <= mouseX + brushSize; neighbourX++) {
                        for(int neighbourY = mouseY - brushSize; neighbourY <= mouseY + brushSize; neighbourY++) {
                            if(neighbourX >= 0 && neighbourX < MAX_CELLS_X && neighbourY >= 0 && neighbourY < MAX_CELLS_Y) {
                                if((neighbourX != mouseX || neighbourY != mouseY)) {
                                    cells[neighbourX][neighbourY] = selectedCellType;
                                    finalCells[neighbourX][neighbourY] = cells[neighbourX][neighbourY];
                                }
                            }
                        }
                    }
                }
            }
        }

        if(generationFramesCounter >= TARGET_FPS) {
            generationFramesCounter = 0;
            if(isShowingUI == false) {
                LoopCells();
            }
        }

        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                finalCells[x][y] = cells[x][y];
            }
        }

        mousePosition = GetMousePosition();

        UpdateMyCamera();
        
        
        BeginDrawing();
        
        ClearBackground(GRAY);
        

        BeginMode2D(camera);
        DrawRectangle(-20,-20, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_X) + 40, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_Y) + 40, BLACK);
        
        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                DrawRectangleV((Vector2) {CELL_SIZE / 2 + CELL_SEPARATION * x + CELL_SIZE * x, CELL_SIZE / 2 + CELL_SEPARATION * y + CELL_SIZE * y}, (Vector2) {CELL_SIZE, CELL_SIZE}, cellTypes[cells[x][y]].color);
                testSize += CELL_SIZE;
            }
        }

        if(isShowingUI == false) {
            mouseToWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
            DrawRectangle(mouseToWorldPosition.x, mouseToWorldPosition.y, 5, 5, BLACK);
        }
        
        EndMode2D();

        HandleUI();

        if (fileDialogState.fileDialogActive) GuiLock();
        GuiUnlock();

        GuiFileDialog(&fileDialogState);

        EndDrawing();
    }

    CloseWindow();
}

void UpdateMyCamera() {
    if(isShowingUI == true) {
        return;
    }
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

    if (camera.zoom > MAX_CAMERA_ZOOM) camera.zoom = MAX_CAMERA_ZOOM;
    else if (camera.zoom < MIN_CAMERA_ZOOM) camera.zoom = MIN_CAMERA_ZOOM;

    int zoomXTo = SCREEN_WIDTH * camera.zoom;
    int zoomYTo = SCREEN_HEIGHT * camera.zoom;
    int temp_cam_w = (zoomXTo - SCREEN_WIDTH) / 25;
    int temp_cam_h = (zoomYTo - SCREEN_HEIGHT) / 25;

    int temp_cam_w_half = temp_cam_w / 2;
    int temp_cam_h_half = temp_cam_h / 2;

    int worldWidth = ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_X);
    int worldHeight = ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_Y);
 
    Vector2 cameraClampPosition = camera.target;
    cameraClampPosition.x = clamp(cameraClampPosition.x, temp_cam_w_half, worldWidth - temp_cam_w_half);
    cameraClampPosition.y = clamp(cameraClampPosition.y, temp_cam_h_half, worldHeight - temp_cam_h_half);
    camera.target = cameraClampPosition;

}

void AnimateOpeningMenu() {
    framesCounter++;
    leftUIBackground.x = EaseLinearNone(framesCounter, -(0.5 * leftUIBackground.width), leftUIBackground.width, 6);

    if(framesCounter >= 6) {
        framesCounter = 0;
        isUIAnimationFinished = true;
    }
    DrawRectanglePro(leftUIBackground, (Vector2) {leftUIBackground.width / 2, 0}, 0, Fade(LIGHTGRAY, uiTransparency));
}

void HandleUI() {  
    if(isShowingUI == true) {
        if(isUIAnimationFinished == false) {
            AnimateOpeningMenu();
        } else {
            int backgroundPositionX = leftUIBackground.width / 2;
            int contentPositionX = backgroundPositionX - 55;
            
            DrawRectanglePro(leftUIBackground, (Vector2) {backgroundPositionX, 0}, 0, Fade(LIGHTGRAY, uiTransparency));
            DrawRectangle(contentPositionX - 15, 20, 125, 125, WHITE);
            DrawRectangleLines(contentPositionX - 15, 20, 125, 125, BLACK);
            
            DrawRectangle(contentPositionX - 7, 150, 110, 25, WHITE);
            DrawRectangleLines(contentPositionX - 7, 150, 110, 25, BLACK);
            
            DrawText("Cellular Fun", contentPositionX + 11, 156, 12, BLACK);

            DrawRectangle(contentPositionX + 10, 180, 70, 25, WHITE);
            DrawRectangleLines(contentPositionX + 10, 180, 70, 25, BLACK);
            
            DrawText("by Ted", contentPositionX + 26, 186, 12, BLACK);
            if(GuiButton((Rectangle) {contentPositionX, 0.62 * SCREEN_HEIGHT, 100, 20}, "Add Cell Type")) {
                textBoxSelected = false;
                isShowingCreateCellTypeDialog = true;
                if(selectedIndex == -1) { // TEDO remove this if
                    for(int i = 0; i < MAX_CELLTYPES; i++) {
                        if(cellTypes[i].index == -1) {
                            selectedIndex = i;
                            break;
                        }
                    }
                }
                selectedColor = WHITE;
                selectedNeighbourType = 0;
                letterCount = 0;
                isDefault = 0;
                for(int i = 0; i < MAX_NAME_LENGTH; i++) {
                    cellName[i] = '\0';
                }
                for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                    newTargetRelationShips[i].index = -1;
                    newTargetRelationShips[i].amount = 0;
                    newTargetRelationShips[i].relationshipType = 0;
                    newTargetRelationShips[i].resultCellTypeIndex = 0;
                    newTargetRelationShips[i].targetCellTypeIndex = 0;
                }
            }
            if(GuiButton((Rectangle) {contentPositionX, 0.66 * SCREEN_HEIGHT, 100, 20}, "Save File")) {
                exit(0);
            }
            if(GuiButton((Rectangle) {contentPositionX, 0.70 * SCREEN_HEIGHT, 100, 20}, "Load File")) {
                fileDialogState.fileDialogActive = true;
            }
            if(GuiButton((Rectangle) {contentPositionX, 0.74 * SCREEN_HEIGHT, 100, 20}, "Exit Application")) {
                exit(0);
            }
        }
        
        // new cell type UI
        if(isShowingCreateCellTypeDialog) {
            HandleEditCellUI();
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

    HandleRunningMenuUI();
}