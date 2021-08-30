#include "cellular.h"
#include "easings.h"

#define SCREEN_WIDTH                    1480
#define SCREEN_HEIGHT                   800

#define MAX_NEIGHBOUR_TYPES             4

#define DIALOG_VIEW_PERCENT             0.35f
#define DIALOG_CONTENT_PERC             0.341f

#define MIN_GENERATION_SPEED_MULTIPLIER 0
#define MAX_GENERATION_SPEED_MULTIPLIER 4

#define MIN_BRUSH_SIZE                  0
#define MAX_BRUSH_SIZE                  2

int active_width = SCREEN_WIDTH;
int active_height = SCREEN_HEIGHT;

// UI
int framesCounter = 0;
Rectangle leftUIBackground = {- (0.2 * SCREEN_WIDTH), 0, 0.2 * SCREEN_WIDTH, SCREEN_HEIGHT};
Vector2 mousePosition;
bool isMouseOverUI = false;
bool isShowingUI = false;
bool isCinematic = false;
bool isUIAnimationFinished = false;
bool isShowingCreateCellTypeDialog = false;
float uiTransparency = 0.8;

// cellTypes

int selectedCellType = -1;
int hoveredCellType = -1;

Color selectedColor = BLACK;
char newCellName[MAX_NAME_LENGTH];
int cellNameCount = 0;
bool mouseOnText = false;
int selectedIndex = -1;
struct TargetCellRelationship newTargetRelationShips[MAX_RELATIONSHIPS];

int neighbourType = 0;

Rectangle panelRec = { 0.5 * SCREEN_WIDTH - (DIALOG_VIEW_PERCENT * SCREEN_WIDTH) / 2, 0.6 * SCREEN_HEIGHT - (DIALOG_VIEW_PERCENT * SCREEN_HEIGHT) / 2, (DIALOG_VIEW_PERCENT * SCREEN_WIDTH), (DIALOG_VIEW_PERCENT * SCREEN_HEIGHT) };
Rectangle panelContentRec = {0, 0, (DIALOG_CONTENT_PERC * SCREEN_WIDTH), (0.65 * SCREEN_HEIGHT) };
Vector2 panelScroll = { 99, 0 };
Rectangle panelView;

Rectangle sidePanel = {SCREEN_WIDTH * 0.94, 0, SCREEN_WIDTH * 0.06, SCREEN_HEIGHT};
// TEDO Not used
Rectangle editCellPanelHolder = {(0.5 * SCREEN_WIDTH - (DIALOG_VIEW_PERCENT * SCREEN_WIDTH) / 2) - 40 / 2,
(0.6 * SCREEN_HEIGHT - (DIALOG_VIEW_PERCENT * SCREEN_HEIGHT) / 2) - ((DIALOG_VIEW_PERCENT + 0.30f) * SCREEN_HEIGHT) * 0.45f,
(DIALOG_VIEW_PERCENT * SCREEN_WIDTH) + 40,
((DIALOG_VIEW_PERCENT + 0.30f) * SCREEN_HEIGHT)
};

char cellName[MAX_NAME_LENGTH + 1] = "\0";
int letterCount = 0;
bool textBoxSelected = false;
bool isDefault = false;
bool isImmovable = false;
bool isMaintainingVelocity = false;

int generationSpeedMultiplier = 0;
const char *generationSpeedMultiplierNames[MAX_GENERATION_SPEED_MULTIPLIER + 1] = {"Stop", "Slowest", "Slow", "Fast", "Fastest"};
int generationFramesCounter = 0;
int generationPerSeconds = 8;

int globalFrameCounter = 0;

int brushSize = 0;
const char *brushSizeNames[MAX_BRUSH_SIZE + 1] = {"Small", "Medium", "Large"};

// PROTOTYPES
void HandleEditCellUI();
void HandleRunningMenuUI();
void AddCellType() {
    isShowingUI = true;
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
    selectedColor.r = 204;
    selectedColor.g = 210;
    selectedColor.b = 221;
    //selectedNeighbourType = 0;
    letterCount = 0;
    isDefault = 0;
    isImmovable = 0;
    isMaintainingVelocity = 0;
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

void HandleRunningMenuUI() {
    GuiControlState state = guiState;
    //bool pressed = false;

    if(selectedCellType != -1) {

        int topHeaderWidth = MeasureText(cellTypes[selectedCellType].name, 10) + active_width * 0.01;
        int topHeaderHeight = 20;
        int topHeaderSep = 10;

        Rectangle bg = (Rectangle) {active_width / 2 - (active_width * 0.01 + topHeaderWidth) / 2,
            active_height * 0.01 + topHeaderSep,
            active_width * 0.01 + topHeaderWidth,
            active_height * 0.01 + topHeaderHeight};
            
        GuiDrawRectangle(bg, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(BLACK, guiAlpha), Fade(LIGHTGRAY, guiAlpha));
        GuiDrawText(cellTypes[selectedCellType].name, GetTextBounds(BUTTON, bg), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));

        state = GUI_STATE_NORMAL;

    }
    
    sidePanel.x = active_width * 0.94;
    sidePanel.width = active_width * 0.06;
    sidePanel.height = active_height;

    DrawRectangleRec(sidePanel, Fade(LIGHTGRAY, uiTransparency));

    if(CheckCollisionPointRec(GetMousePosition(), sidePanel)) {
        isMouseOverUI = true;
    } else {
        isMouseOverUI = false;
    }

    DrawText("Cells", 0.97 * active_width, 0.1 * active_height, 10, BLACK);

    int cellIndex = 0;
    int cellSeparator = 4 * (0.01 * active_height);
    // Cells Menu
    for(int i = 0; i < MAX_CELLTYPES; i++) {
        if(cellTypes[i].index > -1) {
            int cellPositionX = 0.97 * active_width;
            int cellPositionY = 0.13 * active_height + 0.01 * active_height + cellIndex * cellSeparator;
            int cellWidth = 18;
            int cellHeight = 18;

            Rectangle editCellRec = (Rectangle) {cellPositionX - (0.01 * active_width + cellWidth),
                cellPositionY,
                0.01 * active_height + cellWidth,
                0.01 * active_height + cellHeight};
            
            if ((state != GUI_STATE_DISABLED) && !guiLocked)
            {
                Vector2 mousePoint = GetMousePosition();

                if (CheckCollisionPointRec(mousePoint, editCellRec))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                    else state = GUI_STATE_FOCUSED;

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        textBoxSelected = false;
                        selectedIndex = i;
                        selectedColor = cellTypes[selectedIndex].color;
                        //selectedNeighbourType = cellTypes[selectedIndex].neighbourType;
                        letterCount = 0;
                        isImmovable = cellTypes[selectedIndex].isImmovable;
                        isMaintainingVelocity = cellTypes[selectedIndex].isMaintainingVelocity;
                        if(defaultCell == selectedIndex) {
                            isDefault = true;
                        } else {
                            isDefault = false;
                        }
                        isImmovable = cellTypes[selectedIndex].isImmovable;
                        for(int x = 0; x < MAX_NAME_LENGTH; x++) {
                            if(cellTypes[selectedIndex].name[x] != '\0') {
                                letterCount++;
                            } else {
                                break;
                            }
                        }
                        strcpy(cellName, cellTypes[selectedIndex].name);
                        for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
                            if(cellTypes[selectedIndex].targetCellRelationship[x] != 0 && cellTypes[selectedIndex].targetCellRelationship[x]->index > -1) {
                                newTargetRelationShips[x].amount = cellTypes[selectedIndex].targetCellRelationship[x]->amount;
                                newTargetRelationShips[x].toAmount = cellTypes[selectedIndex].targetCellRelationship[x]->toAmount;
                                newTargetRelationShips[x].index = cellTypes[selectedIndex].targetCellRelationship[x]->index;
                                newTargetRelationShips[x].comparisonType = cellTypes[selectedIndex].targetCellRelationship[x]->comparisonType;
                                newTargetRelationShips[x].chance = cellTypes[selectedIndex].targetCellRelationship[x]->chance;
                                newTargetRelationShips[x].relationshipType = cellTypes[selectedIndex].targetCellRelationship[x]->relationshipType;
                                                            
                                newTargetRelationShips[x].resultCellTypeIndex = cellTypes[selectedIndex].targetCellRelationship[x]->resultCellTypeIndex;
                                newTargetRelationShips[x].targetCellTypeIndex = cellTypes[selectedIndex].targetCellRelationship[x]->targetCellTypeIndex;
                                
                                newTargetRelationShips[x].bottomLeft = cellTypes[selectedIndex].targetCellRelationship[x]->bottomLeft;
                                newTargetRelationShips[x].bottom = cellTypes[selectedIndex].targetCellRelationship[x]->bottom;
                                newTargetRelationShips[x].bottomRight = cellTypes[selectedIndex].targetCellRelationship[x]->bottomRight;
                                newTargetRelationShips[x].right = cellTypes[selectedIndex].targetCellRelationship[x]->right;
                                newTargetRelationShips[x].topRight = cellTypes[selectedIndex].targetCellRelationship[x]->topRight;
                                newTargetRelationShips[x].top = cellTypes[selectedIndex].targetCellRelationship[x]->top;
                                newTargetRelationShips[x].topLeft = cellTypes[selectedIndex].targetCellRelationship[x]->topLeft;
                                newTargetRelationShips[x].left = cellTypes[selectedIndex].targetCellRelationship[x]->left;
                            } else {
                                newTargetRelationShips[x].amount = 0;
                                newTargetRelationShips[x].toAmount = 0;
                                newTargetRelationShips[x].index = -1;
                                newTargetRelationShips[x].comparisonType = 0;
                                                            
                                newTargetRelationShips[x].resultCellTypeIndex = 0;
                                newTargetRelationShips[x].targetCellTypeIndex = 0;

                                newTargetRelationShips[x].bottomLeft = 0;
                                newTargetRelationShips[x].bottom = 0;
                                newTargetRelationShips[x].bottomRight = 0;
                                newTargetRelationShips[x].right = 0;
                                newTargetRelationShips[x].topRight = 0;
                                newTargetRelationShips[x].top = 0;
                                newTargetRelationShips[x].topLeft = 0;
                                newTargetRelationShips[x].left = 0;
                            }                    
                        }
                        isShowingUI = true;
                        isShowingCreateCellTypeDialog = true;
                    }
                }
            }
            GuiDrawRectangle(editCellRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
            GuiDrawText("...", GetTextBounds(BUTTON, editCellRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
            state = GUI_STATE_NORMAL;

            Rectangle selectCellRec = (Rectangle) {cellPositionX,
                cellPositionY,
                0.01 * active_height + cellWidth,
                0.01 * active_height + cellHeight};
            
            if ((state != GUI_STATE_DISABLED) && !guiLocked)
            {
                Vector2 mousePoint = GetMousePosition();

                if (CheckCollisionPointRec(mousePoint, selectCellRec))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                    else state = GUI_STATE_FOCUSED;

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        selectedCellType = cellTypes[i].index;
                    }
                }
            }
            GuiDrawRectangle(selectCellRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(cellTypes[i].color, guiAlpha), Fade(cellTypes[i].color, guiAlpha));
            if(state == GUI_STATE_FOCUSED) {
                GuiDrawRectangle(selectCellRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), uiTransparency), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), uiTransparency));
            }
            state = GUI_STATE_NORMAL;

            if(selectedCellType == cellTypes[i].index) {
                DrawRectangleLines(cellPositionX,
                cellPositionY,
                0.01 * active_height + (cellWidth),
                0.01 * active_height + (cellHeight), BLACK);
            }

            cellIndex++;
        }
    }

    if(cellIndex != MAX_CELLTYPES - 1) {
        Rectangle addCellRec = (Rectangle) {0.97 * active_width,
            0.13 * active_height + 0.01 * active_height + cellIndex * cellSeparator,
            0.01 * active_height + 18,
            0.01 * active_height + 18};
        
        if ((state != GUI_STATE_DISABLED) && !guiLocked)
        {
            Vector2 mousePoint = GetMousePosition();

            if (CheckCollisionPointRec(mousePoint, addCellRec))
            {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                else state = GUI_STATE_FOCUSED;

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    AddCellType();
                }
            }
        }
        GuiDrawRectangle(addCellRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
        GuiDrawText("+", GetTextBounds(BUTTON, addCellRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
        state = GUI_STATE_NORMAL;
    }

    isWrapping = GuiCheckBox((Rectangle) {0.955 * active_width, 0.70 * active_height, 0.015 * active_width, 0.01 * active_height + 15}, "Wrap", isWrapping);

    if(GuiButton((Rectangle) {0.95 * active_width, 0.75 * active_height, 0.04 * active_width, 0.01 * active_height + 20}, "Generate")) {
        int types_amount = 0;
        for(int i = 0; i < MAX_CELLTYPES; i++) {
            if(cellTypes[i].index > -1) {
                types_amount++;
            }
        }
        int generatingCellTypes[types_amount];
        int generatingTypeIndex = 0;
        for(int i = 0; i < MAX_CELLTYPES; i++) {
            if(cellTypes[i].index > -1) {
                generatingCellTypes[generatingTypeIndex] = cellTypes[i].index;
                generatingTypeIndex++;
            }
        }
        
        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                finalCells[x][y].cellTypeIndex = generatingCellTypes[GetRandomValue(0, types_amount - 1)];
                cells[x][y].cellTypeIndex = finalCells[x][y].cellTypeIndex;
            }
        }
    }

    Rectangle brushSizeRec = (Rectangle) {0.95 * active_width, 0.79 * active_height, 0.04 * active_width, 0.01 * active_height + 20};

    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, brushSizeRec))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                if(brushSize >= MAX_BRUSH_SIZE) {
                    brushSize = MIN_BRUSH_SIZE;
                } else {
                    brushSize++;
                }
            }
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                if(brushSize <= 0) {
                    brushSize = MAX_BRUSH_SIZE;
                } else {
                    brushSize--;
                }
            }
        }
    }
    GuiDrawRectangle(brushSizeRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
    GuiDrawText(brushSizeNames[brushSize],
    GetTextBounds(BUTTON, brushSizeRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
    state = GUI_STATE_NORMAL;

    if(GuiButton((Rectangle) {0.95 * active_width, 0.83 * active_height, 0.04 * active_width, 0.01 * active_height + 20}, "Step")) {
        generationFramesCounter = 0;
        LoopCells();
    }

    Rectangle generationSpeedRec = (Rectangle) {0.95 * active_width, 0.87 * active_height, 0.04 * active_width, 0.01 * active_height + 20};

    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, generationSpeedRec))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                if(generationSpeedMultiplier >= MAX_GENERATION_SPEED_MULTIPLIER) {
                    generationSpeedMultiplier = MIN_GENERATION_SPEED_MULTIPLIER;
                } else {
                    generationSpeedMultiplier++;
                }
            }
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                if(generationSpeedMultiplier <= 0) {
                    generationSpeedMultiplier = MAX_GENERATION_SPEED_MULTIPLIER;
                } else {
                    generationSpeedMultiplier--;
                }
            }
        }
    }
    GuiDrawRectangle(generationSpeedRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
    GuiDrawText(generationSpeedMultiplierNames[generationSpeedMultiplier],
    GetTextBounds(BUTTON, generationSpeedRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
    state = GUI_STATE_NORMAL;

    if(GuiButton((Rectangle) {0.95 * active_width, 0.91 * active_height, 0.04 * active_width, 0.01 * active_height + 20}, "Clear")) {
        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                finalCells[x][y].cellTypeIndex = defaultCell;
                cells[x][y].cellTypeIndex = defaultCell;
            }
        }
    }

    if(GuiButton((Rectangle) {0.95 * active_width, 0.95 * active_height, 0.04 * active_width, 0.01 * active_height + 20}, isShowingUI == false ? "Pause" : "Resume")) {
        isShowingUI = !isShowingUI;
        isShowingCreateCellTypeDialog = false;
    }
}

void HandleEditCellUI() {
    panelRec.x = 0.5 * active_width - (DIALOG_VIEW_PERCENT * active_width) / 2;
    panelRec.y = 0.6 * active_height - (DIALOG_VIEW_PERCENT * active_height) / 2;
    panelRec.width = DIALOG_VIEW_PERCENT * active_width;
    panelRec.height = DIALOG_VIEW_PERCENT * active_height;
    
    panelContentRec.width = DIALOG_CONTENT_PERC * active_width;
    //panelContentRec.height = 0.65 * active_height;
    
    float dialogHolderHeight = ((DIALOG_VIEW_PERCENT + 0.40f) * active_height);
    float dialogHolderWidthExtra = 40;
    int dialogHolderPosX = panelRec.x - dialogHolderWidthExtra / 2;
    int dialogHolderPosY = panelRec.y - dialogHolderHeight * 0.45f;
    Rectangle textBox = {dialogHolderPosX + 20, dialogHolderPosY + 45, 100, 20};

    
    if(CheckCollisionPointRec(GetMousePosition(), textBox)) {
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            textBoxSelected = true;
        }
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    } else {
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            textBoxSelected = false;
        }
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
    

    if(textBoxSelected == true) {
        int key = GetCharPressed();
    
        while(key > 0) {
            if((key >= 32) && (key <= 125) && (letterCount < MAX_NAME_LENGTH)) {
                cellName[letterCount] = (char)key;
                cellName[letterCount+1] = '\0';
                letterCount++;
            }

            key = GetCharPressed();
        }

        if(IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if(letterCount < 0 ) {
                letterCount = 0;
            }
            cellName[letterCount] = '\0';
        }
    }
    

    DrawRectangle(dialogHolderPosX, dialogHolderPosY, panelRec.width + dialogHolderWidthExtra, dialogHolderHeight, LIGHTGRAY);

    DrawText("Cell Editor", dialogHolderPosX + 5, dialogHolderPosY + 5, 8, BLACK);
    if(GuiButton((Rectangle) {dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - 20, dialogHolderPosY, 20, 20}, "x")) {
        isShowingCreateCellTypeDialog = false;
    }
    
    DrawLine(dialogHolderPosX, dialogHolderPosY + 20, dialogHolderPosX + panelRec.width + dialogHolderWidthExtra, dialogHolderPosY + 20, BLACK);

    DrawText("Name:", dialogHolderPosX + 20, dialogHolderPosY + 30, 8, BLACK);
    DrawRectangleRec(textBox, WHITE);
    DrawText(letterCount <= 0 ? (textBoxSelected ? "" : "UNSET") : cellName, textBox.x + 5, textBox.y + 4, 8, BLACK);

    if(textBoxSelected == true) {
        globalFrameCounter++;
        DrawRectangleLines(textBox.x, textBox.y, textBox.width, textBox.height, BLUE);
        if (((globalFrameCounter/20)%2) == 0) DrawText("_", textBox.x + 5 + MeasureText(cellName, 8), textBox.y + 4, 8, BLUE);
        
    } else {
        globalFrameCounter = 0;
        DrawRectangleLines(textBox.x, textBox.y, textBox.width, textBox.height, GRAY);
        
    }

    isDefault = GuiCheckBox((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 75, 20, 20}, "Is Default", isDefault);

    isImmovable = GuiCheckBox((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 105, 20, 20}, "Is Immovable", isImmovable);
    

    isMaintainingVelocity = GuiCheckBox((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 135, 20, 20}, "Is Maintaining Velocity", isMaintainingVelocity);    

    DrawText("Relationships:", dialogHolderPosX + 20, dialogHolderPosY + 165, 8, BLACK);

    if(GuiButton((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 180, 100, 20}, "Add Relationship")) {
        for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
            if(newTargetRelationShips[i].index == -1) {
                newTargetRelationShips[i].index = i;
                newTargetRelationShips[i].amount = 0;
                newTargetRelationShips[i].toAmount = 0;
                newTargetRelationShips[i].comparisonType = 0;
                newTargetRelationShips[i].resultCellTypeIndex = 0;
                newTargetRelationShips[i].targetCellTypeIndex = 0;

                break;
            }
        }
    }

    

    int cellColorPickerSize = 100;
        
    DrawText("Color:", dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - (cellColorPickerSize + 100), dialogHolderPosY + 30, 8, BLACK);

    DrawRectangle(dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - (cellColorPickerSize + 100), dialogHolderPosY + 45, 30, 30, selectedColor);

    selectedColor = GuiColorPicker((Rectangle) {dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - (cellColorPickerSize + 60), dialogHolderPosY + 45, cellColorPickerSize, cellColorPickerSize}, selectedColor);
    
    DrawText("Type", panelRec.x + panelRec.width * 0.045, panelRec.y - 20, 8, BLACK);
    DrawText("Target", panelRec.x + panelRec.width * 0.155, panelRec.y - 20, 8, BLACK);
    DrawText("Result", panelRec.x + panelRec.width * 0.285, panelRec.y - 20, 8, BLACK);
    DrawText("Compare", panelRec.x + panelRec.width * 0.395, panelRec.y - 20, 8, BLACK);
    DrawText("Amount", panelRec.x + panelRec.width * 0.530, panelRec.y - 20, 8, BLACK);
    DrawText("Neighbours", panelRec.x + panelRec.width * 0.650, panelRec.y - 20, 8, BLACK);
    DrawText("Chance", panelRec.x + panelRec.width * 0.8, panelRec.y - 20, 8, BLACK);
    DrawText("Delete", panelRec.x + panelRec.width * 0.915, panelRec.y - 20, 8, BLACK);

    panelView = GuiScrollPanel(panelRec, panelContentRec, &panelScroll);

    BeginScissorMode(panelView.x, panelView.y, panelView.width, panelView.height);

    int relationShipSpaceSeparator = 15;
    int topSpace = 5;
    int relationshipCounter = 0;
    for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
        if(newTargetRelationShips[i].index > -1) {
            GuiControlState state = guiState;
            //bool pressed = false;

            Rectangle relationshipTypesRec = (Rectangle) {panelRec.x + panelRec.width * 0.027,
                panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)),
                panelRec.width * 0.01 + 52,
                panelRec.width * 0.01 + 20};
            
            if ((state != GUI_STATE_DISABLED) && !guiLocked)
            {
                Vector2 mousePoint = GetMousePosition();

                if (CheckCollisionPointRec(mousePoint, relationshipTypesRec))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                    else state = GUI_STATE_FOCUSED;

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        if(newTargetRelationShips[i].relationshipType >= MAX_RELATIONSHIP_TYPES - 1) {
                            newTargetRelationShips[i].relationshipType = 0;
                        } else {
                            newTargetRelationShips[i].relationshipType++;
                        }
                        newTargetRelationShips[i].comparisonType = 0;
                    }
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        if(newTargetRelationShips[i].relationshipType <= 0) {
                            newTargetRelationShips[i].relationshipType = MAX_RELATIONSHIP_TYPES - 1;
                        } else {
                            newTargetRelationShips[i].relationshipType--;
                        }
                        newTargetRelationShips[i].comparisonType = 0;
                    }
                }
            }
            GuiDrawRectangle(relationshipTypesRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
            GuiDrawText(relationshipTypes[newTargetRelationShips[i].relationshipType], GetTextBounds(BUTTON, relationshipTypesRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
            state = GUI_STATE_NORMAL;

            Rectangle targetCellTypeRec = (Rectangle) {panelRec.x + panelRec.width * 0.145,
            panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)),
            panelRec.width * 0.01 + 45,
            panelRec.width * 0.01 + 20};

            if ((state != GUI_STATE_DISABLED) && !guiLocked)
            {
                Vector2 mousePoint = GetMousePosition();

                if (CheckCollisionPointRec(mousePoint, targetCellTypeRec))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                    else state = GUI_STATE_FOCUSED;

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        int tempIndex = newTargetRelationShips[i].targetCellTypeIndex;
                        while(true) {
                            if(tempIndex >= MAX_CELLTYPES - 1) {
                                tempIndex = -1;
                            } else {
                                tempIndex++;
                            }
                            if(cellTypes[tempIndex].index != -1 || tempIndex == selectedIndex) {
                                newTargetRelationShips[i].targetCellTypeIndex = tempIndex;
                                break;
                            }                    
                        }
                    }
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        int tempIndex = newTargetRelationShips[i].targetCellTypeIndex;
                        while(true) {
                            if(tempIndex <= -1) {
                                tempIndex = MAX_CELLTYPES - 1;
                            } else {
                                tempIndex--;
                            }
                            if(cellTypes[tempIndex].index != -1 || tempIndex == selectedIndex) {
                                newTargetRelationShips[i].targetCellTypeIndex = tempIndex;
                                break;
                            }                    
                        }
                    }
                }
            }
            GuiDrawRectangle(targetCellTypeRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
            GuiDrawText(newTargetRelationShips[i].targetCellTypeIndex == selectedIndex ? (letterCount > 0 ? cellName : "UNSET") : (newTargetRelationShips[i].targetCellTypeIndex > -1 ? cellTypes[newTargetRelationShips[i].targetCellTypeIndex].name : "Any"), GetTextBounds(BUTTON, targetCellTypeRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
            state = GUI_STATE_NORMAL;

            Rectangle resultCellTypeRec = (Rectangle) {panelRec.x + panelRec.width * 0.270,
             panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)),
             panelRec.width * 0.01 + 45,
             panelRec.width * 0.01 + 20};

            if ((state != GUI_STATE_DISABLED) && !guiLocked)
            {
                Vector2 mousePoint = GetMousePosition();

                if (CheckCollisionPointRec(mousePoint, resultCellTypeRec))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                    else state = GUI_STATE_FOCUSED;

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        int tempIndex = newTargetRelationShips[i].resultCellTypeIndex;
                        while(true) {
                            if(tempIndex >= MAX_CELLTYPES - 1) {
                                tempIndex = 0;
                            } else {
                                tempIndex++;
                            }
                            if(cellTypes[tempIndex].index != -1 || tempIndex == selectedIndex) {
                                newTargetRelationShips[i].resultCellTypeIndex = tempIndex;
                                break;
                            }                    
                        }
                    }
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        int tempIndex = newTargetRelationShips[i].resultCellTypeIndex;
                        while(true) {
                            if(tempIndex <= 0) {
                                tempIndex = MAX_CELLTYPES - 1;
                            } else {
                                tempIndex--;
                            }
                            if(cellTypes[tempIndex].index != -1 || tempIndex == selectedIndex) {
                                newTargetRelationShips[i].resultCellTypeIndex = tempIndex;
                                break;
                            }                    
                        }
                    }
                }
            }
            GuiDrawRectangle(resultCellTypeRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
            GuiDrawText(newTargetRelationShips[i].resultCellTypeIndex == selectedIndex ? (letterCount > 0 ? cellName : "UNSET") : cellTypes[newTargetRelationShips[i].resultCellTypeIndex].name,
             GetTextBounds(BUTTON, resultCellTypeRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
            state = newTargetRelationShips[i].relationshipType == 0 ? GUI_STATE_NORMAL : GUI_STATE_DISABLED;

            Rectangle comparisonRec = (Rectangle) {panelRec.x + panelRec.width * 0.3995,
             panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)),
             panelRec.width * 0.01 + 25, 
             panelRec.width * 0.01 + 20};

            if ((state != GUI_STATE_DISABLED) && !guiLocked)
            {
                Vector2 mousePoint = GetMousePosition();

                if (CheckCollisionPointRec(mousePoint, comparisonRec))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                    else state = GUI_STATE_FOCUSED;

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        if(newTargetRelationShips[i].comparisonType >= MAX_COMPARISON_TYPES - 1) {
                            newTargetRelationShips[i].comparisonType = 0;
                        } else {
                            newTargetRelationShips[i].comparisonType++;
                        }
                    }
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        if(newTargetRelationShips[i].comparisonType <= 0) {
                            newTargetRelationShips[i].comparisonType = MAX_COMPARISON_TYPES - 1;
                        } else {
                            newTargetRelationShips[i].comparisonType--;;
                        }
                    }
                }
            }
            GuiDrawRectangle(comparisonRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
            GuiDrawText(comparisonTypes[newTargetRelationShips[i].comparisonType],
             GetTextBounds(BUTTON, comparisonRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
            state = GUI_STATE_NORMAL;

            Rectangle amountRec = (Rectangle) {panelRec.x + panelRec.width * (newTargetRelationShips[i].comparisonType != 7 ? 0.532 : 0.509),
             panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)),
              panelRec.width * 0.01 + 20, 
              panelRec.width * 0.01 + 20};

            if ((state != GUI_STATE_DISABLED) && !guiLocked)
            {
                Vector2 mousePoint = GetMousePosition();

                if (CheckCollisionPointRec(mousePoint, amountRec))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                    else state = GUI_STATE_FOCUSED;

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        if(newTargetRelationShips[i].amount >= 8) {
                            newTargetRelationShips[i].amount = 0;
                        } else {
                            newTargetRelationShips[i].amount++;
                        }
                    }
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        if(newTargetRelationShips[i].amount <= 0) {
                            newTargetRelationShips[i].amount = 8;
                        } else {
                            newTargetRelationShips[i].amount--;
                        }
                    }
                }
            }
            GuiDrawRectangle(amountRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
            GuiDrawText(TextFormat("%i", newTargetRelationShips[i].amount),
             GetTextBounds(BUTTON, amountRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
            state = GUI_STATE_NORMAL;

            if(newTargetRelationShips[i].comparisonType == 7) {
                Rectangle toAmountRec = (Rectangle) {panelRec.x + panelRec.width * 0.5595,
                 panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)),
                 panelRec.width * 0.01 + 20,
                 panelRec.width * 0.01 + 20};

                if ((state != GUI_STATE_DISABLED) && !guiLocked)
                {
                    Vector2 mousePoint = GetMousePosition();

                    if (CheckCollisionPointRec(mousePoint, toAmountRec))
                    {
                        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                        else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                        else state = GUI_STATE_FOCUSED;

                        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                            if(newTargetRelationShips[i].toAmount >= 8) {
                                newTargetRelationShips[i].toAmount = 0;
                            } else {
                                newTargetRelationShips[i].toAmount++;
                            }
                        }
                        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                            if(newTargetRelationShips[i].toAmount <= 0) {
                                newTargetRelationShips[i].toAmount = 8;
                            } else {
                                newTargetRelationShips[i].toAmount--;
                            }
                        }
                    }
                }
                GuiDrawRectangle(toAmountRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
                GuiDrawText(TextFormat("%i", newTargetRelationShips[i].toAmount),
                GetTextBounds(BUTTON, toAmountRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
                state = GUI_STATE_NORMAL;
            }

            Rectangle center = { panelRec.x + panelRec.width * 0.684
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + 5
            , panelRec.width * 0.01 + 5,
            panelRec.width * 0.01 + 5};

            DrawRectangleRec(center, LIGHTGRAY);
            DrawRectangleLinesEx(center, 1, BLACK);
            
            Rectangle left = { panelRec.x + panelRec.width * 0.684 - (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + 5
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            if(newTargetRelationShips[i].left) {
                DrawRectangleRec(left, BLACK);
            }
            DrawRectangleLinesEx(left, 1, BLACK);
            
            if(CheckCollisionPointRec(mousePosition,
            left)) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    newTargetRelationShips[i].left = !newTargetRelationShips[i].left;
                }
            }

            Rectangle bottomLeft = { panelRec.x + panelRec.width * 0.684 - (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + (2.1 * (panelRec.width * 0.01)) + 5
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            if(newTargetRelationShips[i].bottomLeft) {
                DrawRectangleRec(bottomLeft, BLACK);
            }
            DrawRectangleLinesEx(bottomLeft, 1, BLACK);

            if(CheckCollisionPointRec(mousePosition,
            bottomLeft)) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    newTargetRelationShips[i].bottomLeft = !newTargetRelationShips[i].bottomLeft;
                }
            }

            Rectangle bottom = { panelRec.x + panelRec.width * 0.684
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + (2.1 * (panelRec.width * 0.01)) + 5
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            if(newTargetRelationShips[i].bottom) {
                DrawRectangleRec(bottom, BLACK);
            }
            DrawRectangleLinesEx(bottom, 1, BLACK);

            if(CheckCollisionPointRec(mousePosition,
            bottom)) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    newTargetRelationShips[i].bottom = !newTargetRelationShips[i].bottom;
                }
            }

            Rectangle bottomRight = { panelRec.x + panelRec.width * 0.684 + (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + (2.1 * (panelRec.width * 0.01)) + 5
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            if(newTargetRelationShips[i].bottomRight) {
                DrawRectangleRec(bottomRight, BLACK);
            }
            DrawRectangleLinesEx(bottomRight, 1, BLACK);

            if(CheckCollisionPointRec(mousePosition,
            bottomRight)) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    newTargetRelationShips[i].bottomRight = !newTargetRelationShips[i].bottomRight;
                }
            }

            Rectangle right = { panelRec.x + panelRec.width * 0.684 + (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + 5
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            if(newTargetRelationShips[i].right) {
                DrawRectangleRec(right, BLACK);
            }
            DrawRectangleLinesEx(right, 1, BLACK);

            if(CheckCollisionPointRec(mousePosition,
            right)) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    newTargetRelationShips[i].right = !newTargetRelationShips[i].right;
                }
            }
            
            Rectangle topRight = { panelRec.x + panelRec.width * 0.684 + (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) - (2.1 * (panelRec.width * 0.01)) + 5
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            if(newTargetRelationShips[i].topRight) {
                DrawRectangleRec(topRight, BLACK);
            }
            DrawRectangleLinesEx(topRight, 1, BLACK);

            if(CheckCollisionPointRec(mousePosition,
            topRight)) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    newTargetRelationShips[i].topRight = !newTargetRelationShips[i].topRight;
                }
            }

            Rectangle top = { panelRec.x + panelRec.width * 0.684
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) - (2.1 * (panelRec.width * 0.01)) + 5
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            if(newTargetRelationShips[i].top) {
                DrawRectangleRec(top, BLACK);
            }
            DrawRectangleLinesEx(top, 1, BLACK);

            if(CheckCollisionPointRec(mousePosition,
            top)) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    newTargetRelationShips[i].top = !newTargetRelationShips[i].top;
                }
            }

            Rectangle topLeft = { panelRec.x + panelRec.width * 0.684- (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) - (2.1 * (panelRec.width * 0.01)) + 5
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            if(newTargetRelationShips[i].topLeft) {
                DrawRectangleRec(topLeft, BLACK);
            }
            DrawRectangleLinesEx(topLeft, 1, BLACK);

            if(CheckCollisionPointRec(mousePosition,
            topLeft)) {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    newTargetRelationShips[i].topLeft = !newTargetRelationShips[i].topLeft;
                }
            }

            Rectangle chanceRec = (Rectangle) {panelRec.x + panelRec.width * 0.81,
            panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)),
            panelRec.width * 0.01 + 20, panelRec.width * 0.01 + 20};

            if ((state != GUI_STATE_DISABLED) && !guiLocked)
            {
                Vector2 mousePoint = GetMousePosition();

                if (CheckCollisionPointRec(mousePoint, chanceRec))
                {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
                    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) state = GUI_STATE_PRESSED;
                    else state = GUI_STATE_FOCUSED;

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        int chanceAmount = 1;
                        if(IsKeyDown(KEY_LEFT_SHIFT)) {
                            chanceAmount = 10;
                        }
                        newTargetRelationShips[i].chance += chanceAmount;
                        if(newTargetRelationShips[i].chance > MAX_CHANCE) {
                            newTargetRelationShips[i].chance = MIN_CHANCE;
                        }
                    }
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        int chanceAmount = 1;
                        if(IsKeyDown(KEY_LEFT_SHIFT)) {
                            chanceAmount = 10;
                        }
                        newTargetRelationShips[i].chance -= chanceAmount;
                        if(newTargetRelationShips[i].chance <= 0) {
                            newTargetRelationShips[i].chance = MAX_CHANCE;
                        }
                    }
                }
            }
            GuiDrawRectangle(chanceRec, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));
            GuiDrawText(TextFormat("%i", newTargetRelationShips[i].chance),
            GetTextBounds(BUTTON, chanceRec), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
            state = GUI_STATE_NORMAL;
            
            if(GuiButton((Rectangle) {panelRec.x + panelRec.width * 0.915,
             panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)),
             panelRec.width * 0.01 + 20, panelRec.width * 0.01 + 20}, "x")) {
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

            relationshipCounter++;
        }
    }
    
        
    EndScissorMode();

    panelContentRec.height = (relationShipSpaceSeparator * (panelRec.height * 0.01)) * relationshipCounter;

    if(GuiButton((Rectangle) {dialogHolderPosX + 17, dialogHolderPosY + dialogHolderHeight - ( 100 * DIALOG_VIEW_PERCENT + 0.40f), 100, 20}, "Save")) {
        
        int nameCounter = 0;
        for(int i = 0; i < MAX_NAME_LENGTH - 1; i++) {
            if(cellName[i] != '\000') {
                nameCounter++;
            } else {
                break;
            }
        }
        if(nameCounter > 0) {
            cellTypes[selectedIndex].color = selectedColor;
            cellTypes[selectedIndex].index = selectedIndex;
            strcpy(cellTypes[selectedIndex].name, cellName);
            //cellTypes[selectedIndex].neighbourType = selectedNeighbourType;
            if(isDefault) {
                defaultCell = selectedIndex;
            }
            cellTypes[selectedIndex].isMaintainingVelocity = isMaintainingVelocity;
            cellTypes[selectedIndex].isImmovable = isImmovable;
            cellTypes[selectedIndex].lookDistance = 5;
            
            for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
                cellTypes[selectedIndex].targetCellRelationship[x] = malloc(sizeof(T_TargetCellRelationship));
                cellTypes[selectedIndex].targetCellRelationship[x]->chance = newTargetRelationShips[x].chance;
                cellTypes[selectedIndex].targetCellRelationship[x]->relationshipType = newTargetRelationShips[x].relationshipType;
                cellTypes[selectedIndex].targetCellRelationship[x]->amount = newTargetRelationShips[x].amount;
                cellTypes[selectedIndex].targetCellRelationship[x]->toAmount = newTargetRelationShips[x].toAmount;
                cellTypes[selectedIndex].targetCellRelationship[x]->index = newTargetRelationShips[x].index;
                cellTypes[selectedIndex].targetCellRelationship[x]->comparisonType = newTargetRelationShips[x].comparisonType;
                                            
                cellTypes[selectedIndex].targetCellRelationship[x]->resultCellTypeIndex = newTargetRelationShips[x].resultCellTypeIndex;
                cellTypes[selectedIndex].targetCellRelationship[x]->targetCellTypeIndex = newTargetRelationShips[x].targetCellTypeIndex == MAX_CELLTYPES ? selectedIndex : newTargetRelationShips[x].targetCellTypeIndex;
                cellTypes[selectedIndex].targetCellRelationship[x]->bottomLeft = newTargetRelationShips[x].bottomLeft;
                cellTypes[selectedIndex].targetCellRelationship[x]->bottom = newTargetRelationShips[x].bottom;
                cellTypes[selectedIndex].targetCellRelationship[x]->bottomRight = newTargetRelationShips[x].bottomRight;
                cellTypes[selectedIndex].targetCellRelationship[x]->right = newTargetRelationShips[x].right;
                cellTypes[selectedIndex].targetCellRelationship[x]->topRight = newTargetRelationShips[x].topRight;
                cellTypes[selectedIndex].targetCellRelationship[x]->top = newTargetRelationShips[x].top;
                cellTypes[selectedIndex].targetCellRelationship[x]->topLeft = newTargetRelationShips[x].topLeft;
                cellTypes[selectedIndex].targetCellRelationship[x]->left = newTargetRelationShips[x].left;
            }
            selectedCellType = selectedIndex;

            selectedIndex = -1;
            
            
            isShowingCreateCellTypeDialog = false;
            isShowingUI = !isShowingUI;
        }
        
    }

    if(GuiButton((Rectangle) {dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - 120, dialogHolderPosY + dialogHolderHeight - ( 100 * DIALOG_VIEW_PERCENT + 0.40f), 100, 20}, "Delete")) {
        if(isDefault == false) {
            bool found = false;
            for(int m = 0; m < MAX_CELLTYPES; m++) {
                for(int y = 0; y < MAX_RELATIONSHIPS; y++) {
                    if(m != selectedIndex && cellTypes[m].targetCellRelationship[y] != 0 && (cellTypes[m].targetCellRelationship[y]->targetCellTypeIndex == selectedIndex
                        || cellTypes[m].targetCellRelationship[y]->resultCellTypeIndex == selectedIndex)) {
                            found = true;
                    }
                }
            }

            if(found == false) {
                cellTypes[selectedIndex].color.r = 204;
                cellTypes[selectedIndex].color.g = 210;
                cellTypes[selectedIndex].color.b = 221;
                cellTypes[selectedIndex].index = -1;
                strcpy(cellTypes[selectedIndex].name, "UNSET");
                cellTypes[selectedIndex].neighbourType = 0;
                isImmovable = false;
                
                for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
                    free(cellTypes[selectedIndex].targetCellRelationship[x]);
                }
                selectedIndex = -1;

                for(int x = 0; x < MAX_CELLS_X; x++) {
                    for(int y = 0; y < MAX_CELLS_Y; y++) {
                        finalCells[x][y].cellTypeIndex = defaultCell;
                        cells[x][y].cellTypeIndex = defaultCell;
                    }
                }
                            
                isShowingCreateCellTypeDialog = false;
            }
        }
    }
}