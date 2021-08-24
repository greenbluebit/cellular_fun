#pragma once

#include "ui.h"
#include "json.h"


#define MOVE_SPEED          10
#define FAST_MOVE_SPEED     20

#define CELL_SIZE               4
#define CELL_SEPARATION         1
#define TARGET_FPS              60

#define MAX_CAMERA_ZOOM         4.5f // zoom in
#define MIN_CAMERA_ZOOM         1.1f // zoom out


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



void Setup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(active_width, active_height, "Cellular Fun");

    fileDialogState = InitGuiFileDialog(active_width / 2, active_height / 2, GetWorkingDirectory(), false);
    char fileNameToLoad[512] = { 0 };
    
    font = LoadFont("resources/fonts/setback.png"); // TEDO Not using this at all

    camera.target = (Vector2) { (active_width / 2) , (active_height / 2)};
    camera.offset = camera.target;
    camera.target = (Vector2) {((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_X) / 2, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_Y) / 2};
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
        isCinematic = false;
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

    if(IsKeyPressed(KEY_C)) {
        if(isShowingUI == false) {
            isCinematic = !isCinematic;
        }
    }

    if(IsKeyPressed(KEY_ONE)) {
        if(cellTypes[0].index != -1 ) {
            selectedCellType = cellTypes[0].index;
        }
    }
    if(IsKeyPressed(KEY_TWO)) {
        if(cellTypes[1].index != -1 ) {
            selectedCellType = cellTypes[1].index;
        }
    }
    if(IsKeyPressed(KEY_THREE)) {
        if(cellTypes[2].index != -1 ) {
            selectedCellType = cellTypes[2].index;
        }
    }
    if(IsKeyPressed(KEY_FOUR)) {
        if(cellTypes[3].index != -1 ) {
            selectedCellType = cellTypes[3].index;
        }
    }
    if(IsKeyPressed(KEY_FIVE)) {
        if(cellTypes[4].index != -1 ) {
            selectedCellType = cellTypes[4].index;
        }
    }
    if(IsKeyPressed(KEY_SIX)) {
        if(cellTypes[5].index != -1 ) {
            selectedCellType = cellTypes[5].index;
        }
    }
    if(IsKeyPressed(KEY_SEVEN)) {
        if(cellTypes[6].index != -1 ) {
            selectedCellType = cellTypes[6].index;
        }
    }
    if(IsKeyPressed(KEY_EIGHT)) {
        if(cellTypes[7].index != -1 ) {
            selectedCellType = cellTypes[7].index;
        }
    }
    if(IsKeyPressed(KEY_NINE)) {
        if(cellTypes[8].index != -1 ) {
            selectedCellType = cellTypes[8].index;
        }
    }
    if(IsKeyPressed(KEY_ZERO)) {
        if(cellTypes[9].index != -1 ) {
            selectedCellType = cellTypes[9].index;
        }
    }

    

    if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
    {        
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

    if(IsWindowResized()) {
        camera.target = (Vector2) { (active_width / 2) , (active_height / 2)};
        camera.offset = camera.target;
        camera.target = (Vector2) {((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_X) / 2, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_Y) / 2};
        
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
    //DrawRectangle(-10,-10, ((CELL_SIZE + CELL_SEPARATION) * (selfActualizing ? MAX_CELLS_X : PHYSICS_MAX_CELLS_X)) + 20, ((CELL_SIZE + CELL_SEPARATION) * (selfActualizing ? MAX_CELLS_Y : PHYSICS_MAX_CELLS_Y)) + 20, BLACK);
    DrawRectangle(-10,-10, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_X) + 20, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_Y) + 20, BLACK);
    
    for(int x = 0; x < MAX_CELLS_X; x++) {
        for(int y = 0; y < MAX_CELLS_Y; y++) {
            DrawRectangleV((Vector2) {CELL_SIZE / 2 + CELL_SEPARATION * x + CELL_SIZE * x, CELL_SIZE / 2 + CELL_SEPARATION * y + CELL_SIZE * y}, (Vector2) {CELL_SIZE, CELL_SIZE}, cellTypes[cells[x][y]].color);
            testSize += CELL_SIZE;
        }
    }
    // if(selfActualizing == true) {
        
    // } else {
    //     // for(int x = 0; x < PHYSICS_MAX_CELLS_X; x++) {
    //     //     for(int y = 0; y < PHYSICS_MAX_CELLS_Y; y++) {
    //     //         DrawRectangleV((Vector2) {CELL_SIZE / 2 + CELL_SEPARATION * x + CELL_SIZE * x, CELL_SIZE / 2 + CELL_SEPARATION * y + CELL_SIZE * y}, (Vector2) {CELL_SIZE, CELL_SIZE}, cellTypes[cells[x][y]].color);
    //     //         testSize += CELL_SIZE;
    //     //     }
    //     // }
    // }
    

    if(isShowingUI == false) {
        mouseToWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
        DrawRectangle(mouseToWorldPosition.x, mouseToWorldPosition.y, 5, 5, BLACK);
    }
    
    EndMode2D();

    if(isCinematic == false) {
        HandleUI();
    }

    if(isDebug) {
        DrawText(TextFormat("Processor time used by CELLS: %lg sec.\n", \
        cells_performance_timer), 50, 100, 10, BLACK);
        DrawText(TextFormat("Processor time used by CHANGES: %lg sec.\n", \
        changes_performance_timer), 50, 120, 10, BLACK);
        DrawText(TextFormat("Processor time used by PRE-CHANGES: %lg sec.\n", \
        pre_changes_performance_timer), 50, 140, 10, BLACK);
    }

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
                //selectedNeighbourType = 0;
                letterCount = 0;
                isDefault = 0;
                isImmovable = 0;
                for(int i = 0; i < MAX_NAME_LENGTH; i++) {
                    cellName[i] = '\0';
                }
                for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                    newTargetRelationShips[i].index = -1;
                    newTargetRelationShips[i].amount = 0;
                    newTargetRelationShips[i].comparisonType = 0;
                    newTargetRelationShips[i].resultCellTypeIndex = 0;
                    newTargetRelationShips[i].targetCellTypeIndex = 0;
                    newTargetRelationShips[i].chance = MIN_CHANCE;
                    newTargetRelationShips[i].relationshipType = 0;

                    newTargetRelationShips[i].bottomLeft = 0;
                    newTargetRelationShips[i].bottom = 0;
                    newTargetRelationShips[i].bottomRight = 0;
                    newTargetRelationShips[i].right = 0;
                    newTargetRelationShips[i].topRight = 0;
                    newTargetRelationShips[i].top = 0;
                    newTargetRelationShips[i].topLeft = 0;
                    newTargetRelationShips[i].left = 0;
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