#pragma once

#include "ui.h"


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

void Setup() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Cellular Fun - by Ted");

    fileDialogState = InitGuiFileDialog(420, 310, GetWorkingDirectory(), false);
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

    SetExitKey(0); // This was I set the exit key. Atm, on Escape, it exits
    //DisableCursor();
    while(WindowShouldClose() == false) {
        float deltaTime = GetFrameTime();
        generationFramesCounter++;

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

        if(fileDialogState.SelectFilePressed) {
            if(IsFileExtension(fileDialogState.fileNameText, ".json")) {
                strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));

            }
        }

        // Handle Cell Interaction
        Vector2 mouseToWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
        
        if(selectedCellType > -1 && isShowingUI == false && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            int mouseX = (mouseToWorldPosition.x) / (CELL_SIZE + CELL_SEPARATION) + 1;
            int mouseY = (mouseToWorldPosition.y) / (CELL_SIZE + CELL_SEPARATION) + 1;
            if(mouseX >= 0 && mouseX < MAX_CELLS_X) {
                if(mouseY >= 0 && mouseY < MAX_CELLS_Y) {
                    cells[mouseX][mouseY] = selectedCellType;
                    finalCells[mouseX][mouseY] = cells[mouseX][mouseY];
                }
            }
        }

        

        //if((generationFramesCounter / (TARGET_FPS * generationPerSeconds) % generationPerSeconds) == 1) {
        if(generationFramesCounter - (TARGET_FPS * generationPerSeconds) == 0) {
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
            if(GuiButton((Rectangle) {contentPositionX, 0.52 * SCREEN_HEIGHT, 100, 20}, "Add Cell Type")) {
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