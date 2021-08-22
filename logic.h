#pragma once

#include "ui.h"
#include "cJSON/cJSON.h"
#include "cJSON/cJSON.c"


#define MOVE_SPEED          10
#define FAST_MOVE_SPEED     20

#define CELL_SIZE               4
#define CELL_SEPARATION         1
#define TARGET_FPS              60

#define MAX_CAMERA_ZOOM         3.5f // zoom in
#define MIN_CAMERA_ZOOM         0.9f // zoom out


GuiFileDialogState fileDialogState;
char fileNameToLoad[512] = { 0 };
FILE *fp;
bool isSavingFile = false;

bool isMovingFast = false;
bool isMousePressed = false;
int lastMouseX = -1;
int lastMouseY = -1;

Camera2D camera = { 0 };

Font font;

int testSize = 0;

// PROTOTYPES
void UpdateMyCamera();
void HandleUI();
void Loop();
void ApplyMouseClick(int mouseX, int mouseY);

int ParseJson(char *string) {
    int status = 0;
    cJSON *parent = cJSON_Parse(string);
    cJSON *jsonDefaultCell = NULL;
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
    cJSON *jsonTargetRelationship = NULL;
    cJSON *bottomLeft = NULL;
    cJSON *bottom = NULL;
    cJSON *bottomRight = NULL;
    cJSON *right = NULL;
    cJSON *topRight = NULL;
    cJSON *top = NULL;
    cJSON *topLeft = NULL;
    cJSON *left = NULL;
    cJSON *jsonTargetIndex = NULL;
    cJSON *jsonRelationshipType = NULL;
    cJSON *jsonAmount = NULL;
    cJSON *jsonToAmount = NULL;
    cJSON *jsonRelationshipIndex = NULL;
    cJSON *jsonResultIndex = NULL;

    if(parent == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 1;
        goto end;
    }

    
    jsonDefaultCell = cJSON_GetObjectItemCaseSensitive(parent, "defaultcell");
    if(jsonDefaultCell != NULL) {
        defaultCell = jsonDefaultCell->valueint;
        selectedCellType = defaultCell;
        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                finalCells[x][y] = defaultCell;
                cells[x][y] = defaultCell;
            }
        }
    }
    

    for(int i = 0; i < MAX_CELLTYPES; i++) {
        cellTypes[i].index = -1;
        for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
            cellTypes[i].targetCellRelationship[x] = NULL;
        }
    }

    jsonCellTypes = cJSON_GetObjectItemCaseSensitive(parent, "celltypes");
    // MAYBE
    // TEDO I need to change it so that instead of having relationship array or cellType array uninitiated, to have them all with no indexes
    //int cellTypeIndex = 0;
    cJSON_ArrayForEach(jsonCellType, jsonCellTypes) {
        jsonIndex = cJSON_GetObjectItemCaseSensitive(jsonCellType, "index");
        jsonNeighbourType = cJSON_GetObjectItemCaseSensitive(jsonCellType, "neighbourtype");
        jsonColor = cJSON_GetObjectItemCaseSensitive(jsonCellType, "color");

        if(jsonColor == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                fprintf(stderr, "Error before: %s\n", error_ptr);
            }
            status = 2;
            goto end;
        }

        jsonR = cJSON_GetObjectItemCaseSensitive(jsonColor, "r");
        jsonG = cJSON_GetObjectItemCaseSensitive(jsonColor, "g");
        jsonB = cJSON_GetObjectItemCaseSensitive(jsonColor, "b");
        jsonA = cJSON_GetObjectItemCaseSensitive(jsonColor, "a");

        jsonName = cJSON_GetObjectItemCaseSensitive(jsonCellType, "name");

        cellTypes[jsonIndex->valueint].index = jsonIndex->valueint;
        cellTypes[jsonIndex->valueint].color = CLITERAL(Color){ jsonR->valueint, jsonG->valueint, jsonB->valueint, jsonA->valueint };
        
        strcpy(cellTypes[jsonIndex->valueint].name, jsonName->valuestring);



        jsonTargetRelationships = cJSON_GetObjectItemCaseSensitive(jsonCellType, "targetcellrelationships");

        cJSON_ArrayForEach(jsonTargetRelationship, jsonTargetRelationships) {
            jsonTargetIndex = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "targetindex");
            jsonRelationshipType = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "relationshiptype");
            jsonAmount = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "amount");
            jsonToAmount = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "toamount");
            jsonRelationshipIndex = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "index");
            jsonResultIndex = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "resultindex");
            
            if(jsonRelationshipIndex->valueint > -1) {
                if(cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint] == 0) {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint] = malloc(sizeof(T_TargetCellRelationship));
                }
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->amount = jsonAmount->valueint;
                if(jsonToAmount != NULL) {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->toAmount = jsonToAmount->valueint;
                } else {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->toAmount = 0;
                }
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->index = jsonIndex->valueint;
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->relationshipType = jsonRelationshipType->valueint;
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->resultCellTypeIndex = jsonResultIndex->valueint;
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->targetCellTypeIndex = jsonTargetIndex->valueint;

                if(jsonNeighbourType != NULL) {
                    if(jsonNeighbourType->valueint == 0) {
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = 1;
                    } else if(jsonNeighbourType->valueint == 1) {
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = 1;
                    } else if(jsonNeighbourType->valueint == 2) {
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = 0;
                    } else if(jsonNeighbourType->valueint == 3) {
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = 1;
                    }
                    
                } else {
                    bottomLeft = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "bottomleft");
                    bottom = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "bottom");
                    bottomRight = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "bottomright");
                    right = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "right");
                    topRight = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "topright");
                    top = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "top");
                    topLeft = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "topleft");
                    left = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "left");

                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = bottomLeft->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = bottom->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = bottomRight->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = right->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = topRight->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = top->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = topLeft->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = left->valueint;
                }
            }
            
        }


        //cellTypeIndex++;
    }

    end:
        cJSON_Delete(parent);
        return status;
}

char *PrintJson() {
    // TEDO I can reuse variables in next version
    char *string = NULL;
    cJSON *jsonDefaultCell = NULL;
    cJSON *jsonCellTypes = NULL;
    cJSON *jsonCellType = NULL;
    cJSON *jsonIndex = NULL;
    //cJSON *jsonNeighbourType = NULL;
    cJSON *jsonColor = NULL;
    cJSON *jsonR = NULL;
    cJSON *jsonG = NULL;
    cJSON *jsonB = NULL;
    cJSON *jsonA = NULL;
    cJSON *jsonName = NULL;
    cJSON *jsonTargetRelationships = NULL;
    cJSON *bottomLeft = NULL;
    cJSON *bottom = NULL;
    cJSON *bottomRight = NULL;
    cJSON *right = NULL;
    cJSON *topRight = NULL;
    cJSON *top = NULL;
    cJSON *topLeft = NULL;
    cJSON *left = NULL;
    cJSON *jsonTargetIndex = NULL;
    cJSON *jsonRelationshipType = NULL;
    cJSON *jsonAmount = NULL;
    cJSON *jsonToAmount = NULL;
    cJSON *jsonRelationshipIndex = NULL;
    cJSON *jsonResultIndex = NULL;
    cJSON *jsonCells = NULL;
    // cJSON *jsonCell = NULL;
    // cJSON *jsonCellIndex = NULL;
    // cJSON *jsonCellTypeIndex = NULL;

    cJSON *parent = cJSON_CreateObject();
    if(parent == NULL) {
        goto end;
    }

    jsonDefaultCell = cJSON_CreateNumber(defaultCell);
    cJSON_AddItemToObject(parent, "defaultcell",jsonDefaultCell);

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

        //jsonNeighbourType = cJSON_CreateNumber(cellTypes[i].neighbourType);
        //cJSON_AddItemToObject(jsonCellType, "neighbourtype", jsonNeighbourType);

        jsonColor = cJSON_CreateObject();
        cJSON_AddItemToObject(jsonCellType, "color", jsonColor);

        jsonR = cJSON_CreateNumber(cellTypes[i].color.r);
        cJSON_AddItemToObject(jsonColor, "r", jsonR);

        jsonG = cJSON_CreateNumber(cellTypes[i].color.g);
        cJSON_AddItemToObject(jsonColor, "g", jsonG);

        jsonB = cJSON_CreateNumber(cellTypes[i].color.b);
        cJSON_AddItemToObject(jsonColor, "b", jsonB);

        jsonA = cJSON_CreateNumber(cellTypes[i].color.a);
        cJSON_AddItemToObject(jsonColor, "a", jsonA);

        jsonName = cJSON_CreateString(cellTypes[i].name);
        cJSON_AddItemToObject(jsonCellType, "name", jsonName);

        jsonTargetRelationships = cJSON_CreateArray();
        cJSON_AddItemToObject(jsonCellType, "targetcellrelationships", jsonTargetRelationships);

        for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
            if(cellTypes[i].targetCellRelationship[x] != 0) {
                cJSON *jsonTargetRelationship = cJSON_CreateObject();
                cJSON_AddItemToArray(jsonTargetRelationships, jsonTargetRelationship);

                jsonTargetIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->targetCellTypeIndex);
                cJSON_AddItemToObject(jsonTargetRelationship, "targetindex", jsonTargetIndex);

                jsonRelationshipType = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->relationshipType);
                cJSON_AddItemToObject(jsonTargetRelationship, "relationshiptype", jsonRelationshipType);

                jsonAmount = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->amount);
                cJSON_AddItemToObject(jsonTargetRelationship, "amount", jsonAmount);

                jsonToAmount = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->toAmount);
                cJSON_AddItemToObject(jsonTargetRelationship, "toamount", jsonToAmount);

                jsonRelationshipIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->index);
                cJSON_AddItemToObject(jsonTargetRelationship, "index", jsonRelationshipIndex);

                jsonResultIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->resultCellTypeIndex);
                cJSON_AddItemToObject(jsonTargetRelationship, "resultindex", jsonResultIndex);

                bottomLeft = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->bottomLeft);
                cJSON_AddItemToObject(jsonTargetRelationship, "bottomleft", bottomLeft);

                bottom = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->bottom);
                cJSON_AddItemToObject(jsonTargetRelationship, "bottom", bottom);

                bottomRight = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->bottomRight);
                cJSON_AddItemToObject(jsonTargetRelationship, "bottomright", bottomRight);

                right = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->right);
                cJSON_AddItemToObject(jsonTargetRelationship, "right", right);

                topRight = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->topRight);
                cJSON_AddItemToObject(jsonTargetRelationship, "topright", topRight);

                top = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->top);
                cJSON_AddItemToObject(jsonTargetRelationship, "top", top);

                topLeft = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->topLeft);
                cJSON_AddItemToObject(jsonTargetRelationship, "topleft", topLeft);

                left = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->left);
                cJSON_AddItemToObject(jsonTargetRelationship, "left", left);

                // if(jsonNeighbourType == NULL) {
                //     cJSON *bottomLeft = NULL;
                //     cJSON *bottom = NULL;
                //     cJSON *bottomRight = NULL;
                //     cJSON *right = NULL;
                //     cJSON *topRight = NULL;
                //     cJSON *top = NULL;
                //     cJSON *topLeft = NULL;
                //     cJSON *left = NULL;
                // } else {

                // }
            }
        }
    }

    // TEDO Make Cells saving as a boolean
    //jsonCells = cJSON_CreateArray();
    //cJSON_AddItemToObject(parent, "cells", jsonCells);

    // int usedIndex = 0;

    // for(int x = 0; x < MAX_CELLS_X; x++) {
    //     for(int y = 0; y < MAX_CELLS_Y; y++) {
    //         jsonCell = cJSON_CreateObject();
    //         if(jsonCell == NULL) { // TEDO I Need to do this check at the rest of the json initializers
    //             goto end;
    //         }
    //         cJSON_AddItemToArray(jsonCells, jsonCell);

    //         jsonCellIndex = cJSON_CreateNumber(usedIndex);
    //         cJSON_AddItemToObject(jsonCell, "cellindex", jsonCellIndex);
            
    //         jsonCellTypeIndex = cJSON_CreateNumber(cells[x][y]);
    //         cJSON_AddItemToObject(jsonCell, "celltypeindex", jsonCellTypeIndex);

    //         usedIndex++;

    //         SetWindowTitle(TextFormat("%i", usedIndex));

    //     }
    // }

    string = cJSON_Print(parent);
    if(string == NULL) {
        fprintf(stderr, "Failed to print. \n");
    }

    end:
        cJSON_Delete(parent);
        return string;
}

void Setup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(active_width, active_height, "Cellular Fun");

    fileDialogState = InitGuiFileDialog(active_width / 2, active_height / 2, GetWorkingDirectory(), false);
    char fileNameToLoad[512] = { 0 };
    
    font = LoadFont("resources/fonts/setback.png");
    camera.target = (Vector2) { (active_width / 2) , (active_height / 2)};
    camera.offset = camera.target;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;


    SetupCells(); 

    for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
        newTargetRelationShips[i].index = -1;
    }

    SetExitKey(0); // This way I set the exit key. Atm, on Escape, it exits
    //DisableCursor();

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(Loop, 0, 1);
    #else
        SetTargetFPS(60);   // Set our game to run at 60 frames-per-second

        while(WindowShouldClose() == false) {
            Loop();
        }

        CloseWindow();
    #endif
}

void Loop() {
    int display = GetCurrentMonitor();
    active_width = GetScreenWidth();
    active_height = GetScreenHeight();
    generationFramesCounter+= generationSpeedMultiplier * generationPerSeconds;

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

    if(IsKeyPressed(KEY_SPACE)) {
        if(isShowingUI == false) {
            generationFramesCounter = 0;
            LoopCells();
        }
    }

    if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
    {
        // see what display we are on right now
        

        
        if (IsWindowFullscreen())
        {
            // if we are full screen, then go back to the windowed size
            SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
            active_width = GetMonitorWidth(display);
            active_height = GetMonitorHeight(display);
            
        }
        else
        {
            // if we are not full screen, set the window size to match the monitor we are on
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
            active_width = GetMonitorWidth(display);
            active_height = GetMonitorHeight(display);
        }

        // toggle the state
        ToggleFullscreen();
    }

    if(fileDialogState.SelectFilePressed) {
        if(IsFileExtension(fileDialogState.fileNameText, ".json")) {
            if(isSavingFile) {
                strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                fp = fopen(fileNameToLoad, "w");

                if(fp == NULL) {
                    fprintf(stderr, "Failed to open file. \n"); // TEDO Add file name
                    exit(1);
                }

                fprintf(fp, PrintJson());
                strcpy(fileDialogState.fileNameText, "\0");
                strcpy(fileDialogState.fileNameTextCopy, fileDialogState.fileNameText);
                fileDialogState.filesListActive = -1;
                fileDialogState.prevFilesListActive = fileDialogState.filesListActive;
                fclose(fp);
            } else {
                //TEDO Load file and clear the cells with the new default cell
                strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                fp = fopen(fileNameToLoad, "r");

                fseek(fp, 0, SEEK_END);
                long fsize = ftell(fp);
                fseek(fp, 0, SEEK_SET);


                char *buf = malloc(fsize + 1);
                size_t nread;

                if(buf == NULL) {
                    // TEDO throw error for malloc fail
                }

                fread(buf, 1, fsize, fp);
                ParseJson(buf);
                
                strcpy(fileDialogState.fileNameText, "\0");
                strcpy(fileDialogState.fileNameTextCopy, fileDialogState.fileNameText);
                fileDialogState.filesListActive = -1;
                fileDialogState.prevFilesListActive = fileDialogState.filesListActive;
                fclose(fp);
            }
        }
    }

    // Handle Cell Interaction
    Vector2 mouseToWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
    
    if(selectedCellType > -1 && isShowingUI == false && isMouseOverUI == false && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {


        int mouseX = (mouseToWorldPosition.x) / (CELL_SIZE + CELL_SEPARATION);
        int mouseY = (mouseToWorldPosition.y) / (CELL_SIZE + CELL_SEPARATION);

        // lastMouseX ; lastMouseY

        if(mouseX >= 0 && mouseX < MAX_CELLS_X) {
            if(mouseY >= 0 && mouseY < MAX_CELLS_Y) {
                ApplyMouseClick(mouseX, mouseY);
                
            }
        }
        isMousePressed = true;
    } else {
        isMousePressed = false;
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
    DrawRectangle(-10,-10, ((CELL_SIZE + CELL_SEPARATION) * (selfActualizing ? MAX_CELLS_X : PHYSICS_MAX_CELLS_X)) + 20, ((CELL_SIZE + CELL_SEPARATION) * (selfActualizing ? MAX_CELLS_Y : PHYSICS_MAX_CELLS_Y)) + 20, BLACK);
    
    if(selfActualizing == true) {
        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                DrawRectangleV((Vector2) {CELL_SIZE / 2 + CELL_SEPARATION * x + CELL_SIZE * x, CELL_SIZE / 2 + CELL_SEPARATION * y + CELL_SIZE * y}, (Vector2) {CELL_SIZE, CELL_SIZE}, cellTypes[cells[x][y]].color);
                testSize += CELL_SIZE;
            }
        }
    } else {
        for(int x = 0; x < PHYSICS_MAX_CELLS_X; x++) {
            for(int y = 0; y < PHYSICS_MAX_CELLS_Y; y++) {
                DrawRectangleV((Vector2) {CELL_SIZE / 2 + CELL_SEPARATION * x + CELL_SIZE * x, CELL_SIZE / 2 + CELL_SEPARATION * y + CELL_SIZE * y}, (Vector2) {CELL_SIZE, CELL_SIZE}, cellTypes[cells[x][y]].color);
                testSize += CELL_SIZE;
            }
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

void DoCellChange(int cellX, int cellY) {
    cells[cellX][cellY] = selectedCellType;
    finalCells[cellX][cellY] = cells[cellX][cellY];

    for(int neighbourX = cellX - brushSize; neighbourX <= cellX + brushSize; neighbourX++) {
        for(int neighbourY = cellY - brushSize; neighbourY <= cellY + brushSize; neighbourY++) {
            if(neighbourX >= 0 && neighbourX < MAX_CELLS_X && neighbourY >= 0 && neighbourY < MAX_CELLS_Y) {
                if((neighbourX != cellX || neighbourY != cellY)) {
                    cells[neighbourX][neighbourY] = selectedCellType;
                    finalCells[neighbourX][neighbourY] = cells[neighbourX][neighbourY];
                }
            }
        }
    }
}

void ApplyMouseClick(int mouseX, int mouseY) {
    if(isMousePressed == false) {
        DoCellChange(mouseX, mouseY);
    } else {
        if(lastMouseX == mouseX && lastMouseY == mouseY) {
            cells[mouseX][mouseY] = selectedCellType;
            finalCells[mouseX][lastMouseY] = cells[mouseX][mouseY];
        }
        int xDiff = lastMouseX - mouseX;
        int yDiff = lastMouseY - mouseY;

        bool xDiffIsLarger = (xDiff < 0 ? -xDiff : xDiff) > (yDiff < 0 ? -yDiff : yDiff);
        int xModifier = xDiff < 0 ? 1 : -1;
        int yModifier = yDiff < 0 ? 1 : -1;

        bool xDiffSmaller = (xDiff < 0 ? -xDiff : xDiff) < (yDiff < 0 ? -yDiff : yDiff);
        int upperBound = xDiffSmaller ? (yDiff < 0 ? -yDiff : yDiff) : (xDiff < 0 ? -xDiff : xDiff);
        int min = xDiffSmaller ? (xDiff < 0 ? -xDiff : xDiff) : (yDiff < 0 ? -yDiff : yDiff);
        float slope = ( min == 0 || upperBound == 0) ? 0 : ((float) (min + 1) / (upperBound + 1));

        int smallerCount;
        for(int i = 1; i <= upperBound; i++) {
            smallerCount = (int) floor(i * slope);
            int yIncrease, xIncrease;
            if(xDiffIsLarger) {
                xIncrease = i;
                yIncrease = smallerCount;
            } else {
                yIncrease = i;
                xIncrease = smallerCount;
            }

            int currentX = lastMouseX + (xIncrease * xModifier);
            int currentY = lastMouseY + (yIncrease * yModifier);

            if(currentX >= 0 && currentX < MAX_CELLS_X) {
                if(currentY >= 0 && currentY < MAX_CELLS_Y) {
                    DoCellChange(currentX, currentY);
                }
            }
        }
        

        
    }

    lastMouseX = mouseX;
    lastMouseY = mouseY;
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

    int zoomXTo = active_width * camera.zoom;
    int zoomYTo = active_height * camera.zoom;
    int temp_cam_w = (zoomXTo - active_width) / 25;
    int temp_cam_h = (zoomYTo - active_height) / 25;

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

    //leftUIBackground.x = - (0.2 * active_width);
    //leftUIBackground.y = 0;
    leftUIBackground.width = 0.2 * active_width;
    leftUIBackground.height = active_height;

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
                //if(selectedIndex == -1) { // TEDO remove this if
                    for(int i = 0; i < MAX_CELLTYPES; i++) {
                        if(cellTypes[i].index == -1) {
                            selectedIndex = i;
                            break;
                        }
                    }
                //}
                selectedColor = WHITE;
                selectedNeighbourType = 0;
                letterCount = 0;
                isDefault = 0;
                isImmovable = 0;
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
            if(GuiButton((Rectangle) {contentPositionX, 0.66 * SCREEN_HEIGHT, 100, 20}, "Save Rule")) {
                isSavingFile = true;
                fileDialogState.fileDialogActive = true;
            }
            if(GuiButton((Rectangle) {contentPositionX, 0.70 * SCREEN_HEIGHT, 100, 20}, "Load Rule")) {
                isSavingFile = false;
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