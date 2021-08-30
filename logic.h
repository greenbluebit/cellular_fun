#pragma once

#include "ui.h"
#include "json.h"


#define MOVE_SPEED          15
#define FAST_MOVE_SPEED     25
#define ZOOM_SPEED          0.1f

#define CELL_SIZE               4
#define CELL_SEPARATION         1
#define TARGET_FPS              60

#define MAX_CAMERA_ZOOM         4.5f // zoom in
#define MIN_CAMERA_ZOOM         0.8f // zoom out


GuiFileDialogState fileDialogState;
char fileNameToLoad[512] = { 0 };
char **droppedFiles = { 0 };
int droppedFilesCount = 0;
FILE *fp;
bool isSavingFile = false;
bool isFileImage = false;
Image droppedImage;

bool isMovingFast = false;
bool isMousePressed = false;
int lastMouseX = -1;
int lastMouseY = -1;

Camera2D camera = { 0 };

//Font font;

int testSize = 0;

// PROTOTYPES
void UpdateMyCamera();
void HandleUI();
void Loop();
void ApplyMouseClick(int mouseX, int mouseY);
int GetDistanceBetweenColors(Color c1, Color c2) {
    float point1 = (c1.r - c2.r) * (c1.r - c2.r);
    float point2 = (c1.g - c2.g) * (c1.g - c2.g);
    float point3 = (c1.b - c2.b) * (c1.b - c2.b);
    float point4 = (c1.a - c2.a) * (c1.a - c2.a);
    return sqrtf(point1 + point2 + point3 + point4);


}
void HandleOpeningFile() {
    fp = fopen(fileNameToLoad, "r");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);


    char *buf = malloc(fsize + 1);
    //size_t nread;

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
void HandleOpeningImage() {
    for(int y = 0; y < droppedImage.height; y++) {
        if(y >= MAX_CELLS_Y) {
            break;
        }
        for(int x = 0; x < droppedImage.width; x++) {
            if(x >= MAX_CELLS_X) {
                continue;
            }
            float prevDistance = -1;
            int currentIndex = 0;
            for(int i = 0; i < MAX_CELLTYPES; i++) {
                float distance = GetDistanceBetweenColors(((Color *)droppedImage.data)[y * droppedImage.width + x], cellTypes[i].color);
                if(prevDistance == -1 || distance < prevDistance) {
                    prevDistance = distance;
                    currentIndex = i;
                }
            }
            cells[x][y].cellTypeIndex = currentIndex;
            finalCells[x][y].cellTypeIndex = currentIndex;
        }
    }
    strcpy(fileDialogState.fileNameText, "\0");
    strcpy(fileDialogState.fileNameTextCopy, fileDialogState.fileNameText);
    fileDialogState.filesListActive = -1;
    fileDialogState.prevFilesListActive = fileDialogState.filesListActive;
    UnloadImage(droppedImage);    
}

void Setup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(active_width, active_height, "Cellular Fun");

    fileDialogState = InitGuiFileDialog(active_width / 2, active_height / 2, GetWorkingDirectory(), false);

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
        isShowingCreateCellTypeDialog = false;
        if(fileDialogState.fileDialogActive) {
            fileDialogState.fileDialogActive = false;
            CloseDialog(&fileDialogState);
        }
    }

    if(IsKeyPressed(KEY_KP_ADD)) {
        if(generationSpeedMultiplier >= MAX_GENERATION_SPEED_MULTIPLIER) {
            generationSpeedMultiplier = MIN_GENERATION_SPEED_MULTIPLIER;
        } else {
            generationSpeedMultiplier++;
        }
    }
    if(IsKeyPressed(KEY_KP_SUBTRACT)) {
        if(generationSpeedMultiplier <= 0) {
            generationSpeedMultiplier = MAX_GENERATION_SPEED_MULTIPLIER;
        } else {
            generationSpeedMultiplier--;
        }
    }
    if(IsKeyPressed(KEY_PAGE_UP)) {
        if(brushSize >= MAX_BRUSH_SIZE) {
            brushSize = MIN_BRUSH_SIZE;
        } else {
            brushSize++;
        }
    }
    if(IsKeyPressed(KEY_PAGE_DOWN)) {
        if(brushSize <= 0) {
            brushSize = MAX_BRUSH_SIZE;
        } else {
            brushSize--;
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
            SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
            active_width = GetMonitorWidth(display);
            active_height = GetMonitorHeight(display);
            
        }
        else
        {
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
            active_width = GetMonitorWidth(display);
            active_height = GetMonitorHeight(display);
            
        }

        ToggleFullscreen();
    }

    if(IsWindowResized()) {
        camera.target = (Vector2) { (active_width / 2) , (active_height / 2)};
        camera.offset = camera.target;
        camera.target = (Vector2) {((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_X) / 2, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_Y) / 2};
        
    }

    if(fileDialogState.SelectFilePressed) {
        if(isSavingFile) {
            if(isFileImage == false && IsFileExtension(fileDialogState.fileNameText, ".json")) {
                strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                fp = fopen(fileNameToLoad, "wb");

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
            } else if (isFileImage == true && (IsFileExtension(fileDialogState.fileNameText, ".png")
            || IsFileExtension(fileDialogState.fileNameText, ".jpg")
            || IsFileExtension(fileDialogState.fileNameText, ".jpeg")
            || IsFileExtension(fileDialogState.fileNameText, ".bmp"))) {
                strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                //fp = fopen(fileNameToLoad, "wb");
                Color *pixels = (Color *) malloc(MAX_CELLS_X * MAX_CELLS_Y * sizeof(Color));

                for(int y = 0; y < MAX_CELLS_Y; y++) {
                    
                    for(int x = 0; x < MAX_CELLS_X; x++) {
                        pixels[MAX_CELLS_X * y + x].r = cellTypes[cells[x][y].cellTypeIndex].color.r;
                        pixels[MAX_CELLS_X * y + x].g = cellTypes[cells[x][y].cellTypeIndex].color.g;
                        pixels[MAX_CELLS_X * y + x].b = cellTypes[cells[x][y].cellTypeIndex].color.b;
                        pixels[MAX_CELLS_X * y + x].a = cellTypes[cells[x][y].cellTypeIndex].color.a;
                        
                    }
                }
                Image checkedIm = {
                    .data = pixels,             // We can assign pixels directly to data
                    .width = MAX_CELLS_X,
                    .height = MAX_CELLS_Y,
                    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
                    .mipmaps = 1
                };
                ExportImage(checkedIm, fileNameToLoad);
                strcpy(fileDialogState.fileNameText, "\0");
                strcpy(fileDialogState.fileNameTextCopy, fileDialogState.fileNameText);
                fileDialogState.filesListActive = -1;
                fileDialogState.prevFilesListActive = fileDialogState.filesListActive;
                free(pixels);
                UnloadImage(checkedIm);
                //fclose(fp);
            }
            
        } else {
            if(isFileImage == false && IsFileExtension(fileDialogState.fileNameText, ".json")) {
                strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                HandleOpeningFile();
            } else if (isFileImage == true) {
                strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                droppedImage = LoadImage(fileNameToLoad);
                HandleOpeningImage();               
            }
            
        }
        fileDialogState.SelectFilePressed = false;
    }
    if(IsFileDropped()) {
        droppedFiles = GetDroppedFiles(&droppedFilesCount); // TEDO Count is irrelevant now, but I want to open multiple files at once, I'd bother with it.
        strcpy(fileNameToLoad, droppedFiles[0]);

        if(IsFileExtension(droppedFiles[0], ".json")) {
            HandleOpeningFile();
        } else if (IsFileExtension(droppedFiles[0], ".png")
            || IsFileExtension(droppedFiles[0], ".jpg")
            || IsFileExtension(droppedFiles[0], ".jpeg")
            || IsFileExtension(droppedFiles[0], ".bmp")) {
            droppedImage = LoadImage(droppedFiles[0]);
            
            HandleOpeningImage();

            
        }
        
        ClearDroppedFiles();
    }

    // Handle Cell Interaction
    Vector2 mouseToWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
    
    if(selectedCellType > -1 && isShowingUI == false && isMouseOverUI == false && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {


        int mouseX = (mouseToWorldPosition.x) / (CELL_SIZE + CELL_SEPARATION);
        int mouseY = (mouseToWorldPosition.y) / (CELL_SIZE + CELL_SEPARATION);

        // lastMouseX ; lastMouseY

        if(mouseX >= 0 && mouseX < MAX_CELLS_X && mouseY >= 0 && mouseY < MAX_CELLS_Y) {
            ApplyMouseClick(mouseX, mouseY);
        } else {
            lastMouseX = -1;
            lastMouseY = -1;
        }
        isMousePressed = true;
    } else {
        int mouseX = (mouseToWorldPosition.x) / (CELL_SIZE + CELL_SEPARATION);
        int mouseY = (mouseToWorldPosition.y) / (CELL_SIZE + CELL_SEPARATION);
        if(isMousePressed == true && mouseX >= 0 && mouseX < MAX_CELLS_X) {
            if(mouseY >= 0 && mouseY < MAX_CELLS_Y) {
                isMousePressed = false;
                ApplyMouseClick(mouseX, mouseY);
                
            }
        }
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
            finalCells[x][y].cellTypeIndex = cells[x][y].cellTypeIndex;
        }
    }

    mousePosition = GetMousePosition();

    UpdateMyCamera();
    
    
    BeginDrawing();
    
    ClearBackground((Color) {46, 55, 71, 255});
    

    BeginMode2D(camera);
    DrawRectangle(-10,-10, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_X) + 20, ((CELL_SIZE + CELL_SEPARATION) * MAX_CELLS_Y) + 20, BLACK);
    
    for(int x = 0; x < MAX_CELLS_X; x++) {
        for(int y = 0; y < MAX_CELLS_Y; y++) {
            DrawRectangleV((Vector2) {CELL_SIZE / 2 + CELL_SEPARATION * x + CELL_SIZE * x, CELL_SIZE / 2 + CELL_SEPARATION * y + CELL_SIZE * y}, (Vector2) {CELL_SIZE, CELL_SIZE}, cellTypes[cells[x][y].cellTypeIndex].color);
            testSize += CELL_SIZE;
        }
    }
    

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
    if(cellX >= 0 && cellX < MAX_CELLS_X) {
        if(cellY >= 0 && cellY < MAX_CELLS_Y) {
            cells[cellX][cellY].cellTypeIndex = selectedCellType;
            finalCells[cellX][cellY].cellTypeIndex = cells[cellX][cellY].cellTypeIndex;

            for(int neighbourX = cellX - brushSize; neighbourX <= cellX + brushSize; neighbourX++) {
                for(int neighbourY = cellY - brushSize; neighbourY <= cellY + brushSize; neighbourY++) {
                    if(neighbourX >= 0 && neighbourX < MAX_CELLS_X && neighbourY >= 0 && neighbourY < MAX_CELLS_Y) {
                        if((neighbourX != cellX || neighbourY != cellY)) {
                            cells[neighbourX][neighbourY].cellTypeIndex = selectedCellType;
                            finalCells[neighbourX][neighbourY].cellTypeIndex = cells[neighbourX][neighbourY].cellTypeIndex;
                        }
                    }
                }
            }
        }
    }    
}

void ApplyMouseClick(int mouseX, int mouseY) {
    if(isMousePressed == false || (lastMouseX == -1 || lastMouseY == -1)) {
        DoCellChange(mouseX, mouseY);
    } else {
        if(lastMouseX == mouseX && lastMouseY == mouseY) {
            if(mouseX >= 0 && mouseX < MAX_CELLS_X) {
                if(mouseY >= 0 && mouseY < MAX_CELLS_Y) {
                    DoCellChange(mouseX, mouseY);
                }
            }
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

    #if defined(PLATFORM_WEB)
        camera.zoom -= ((float)GetMouseWheelMove()*ZOOM_SPEED);
    #else
        camera.zoom += ((float)GetMouseWheelMove()*ZOOM_SPEED);
    #endif
    

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
            leftUIBackground.x = leftUIBackground.width / 2;
            int backgroundPositionX = leftUIBackground.width / 2;
            int contentPositionX = backgroundPositionX - 55;
            GuiControlState state = guiState;
            //bool pressed = false;
            
            DrawRectanglePro(leftUIBackground, (Vector2) {backgroundPositionX, 0}, 0, Fade(LIGHTGRAY, uiTransparency));
            DrawRectangle(contentPositionX - 17,
             (0.01 * active_height + 20),
              0.01 * active_width + 125,
               0.01 * active_height + 125, WHITE);
            DrawRectangleLines(contentPositionX - 17,
             (0.01 * active_height + 20),
              0.01 * active_width + 125,
               0.01 * active_height + 125, BLACK);
            
            DrawRectangle(contentPositionX - 8,
             (0.01 * active_height + 160),
              0.01 * active_width + 110,
               0.01 * active_height + 25, WHITE);
            DrawRectangleLines(contentPositionX - 8,
             (0.01 * active_height + 160),
              0.01 * active_width + 110,
               0.01 * active_height + 25, BLACK);
            
            DrawText("Cellular Fun", contentPositionX + 17, (0.01 * active_height + 172), 14, BLACK);

            DrawRectangle(contentPositionX,
             (0.01 * active_height + 200),
              0.01 * active_width + 95,
               0.01 * active_height + 25, WHITE);
            DrawRectangleLines(contentPositionX,
             (0.01 * active_height + 200),
              0.01 * active_width + 95,
               0.01 * active_height + 25, BLACK);
            
            DrawText("by Ted", contentPositionX + 33,
             (0.01 * active_height + 212),
              12, BLACK);

            DrawText("Rules", contentPositionX,
             (0.01 * active_height + 330),
              12, BLACK);
            if(GuiButton((Rectangle) {contentPositionX, (0.01 * active_height + 345), 0.01 * active_width + 100, 0.01 * active_height + 20}, "Save")) {
                isSavingFile = true;
                isFileImage = false;
                fileDialogState.fileDialogActive = true;
            }
            if(GuiButton((Rectangle) {contentPositionX, (0.01 * active_height + 380), 0.01 * active_width + 100, 0.01 * active_height + 20}, "Load")) {
                isSavingFile = false;
                isFileImage = false;
                fileDialogState.fileDialogActive = true;
            }

            DrawText("Images", contentPositionX, (0.01 * active_height + 430), 12, BLACK);
            if(GuiButton((Rectangle) {contentPositionX, (0.01 * active_height + 445), 0.01 * active_width + 100, 0.01 * active_height + 20}, "Save")) {
                isSavingFile = true;
                isFileImage = true;
                fileDialogState.fileDialogActive = true;
            }
            if(GuiButton((Rectangle) {contentPositionX, (0.01 * active_height + 480), 0.01 * active_width + 100, 0.01 * active_height + 20}, "Load")) {
                isSavingFile = false;
                isFileImage = true;
                fileDialogState.fileDialogActive = true;
            }

            #if defined(PLATFORM_DESKTOP)
                Rectangle exitApplicationRec = (Rectangle) {contentPositionX, (0.01 * active_height + 535), 0.01 * active_width + 100, 0.01 * active_height + 20};
                
                if ((state != GUI_STATE_DISABLED) && !guiLocked)
                {
                    Vector2 mousePoint = GetMousePosition();
                    if (CheckCollisionPointRec(mousePoint, exitApplicationRec))
                    {
                        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                        else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                        else state = GUI_STATE_FOCUSED;

                        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                            exit(0);
                        }
                    }
                }
                
                GuiDrawRectangle(exitApplicationRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GRAY, guiAlpha), Fade(state == GUI_STATE_FOCUSED ? (Color) {193, 92, 92, 255} : (Color) {168, 62, 62, 255}, guiAlpha));
                GuiDrawText("Exit", GetTextBounds(BUTTON, exitApplicationRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(WHITE, guiAlpha));
                state = GUI_STATE_NORMAL;
            #endif
            
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