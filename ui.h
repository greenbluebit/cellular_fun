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
const char *neighbours[MAX_NEIGHBOUR_TYPES] = 
{ "Moore", "Von Neumann", "Vertical", "Horizontal"};
int selectedNeighbourType = 0;

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
int defaultCell = 0;

int generationSpeedMultiplier = 0;
const char *generationSpeedMultiplierNames[MAX_GENERATION_SPEED_MULTIPLIER + 1] = {"Stop", "Slowest", "Slow", "Fast", "Fastest"};
int generationFramesCounter = 0;
int generationPerSeconds = 8;

int brushSize = 0;
const char *brushSizeNames[MAX_BRUSH_SIZE + 1] = {"Small", "Medium", "Large"};

// PROTOTYPES
void HandleEditCellUI();
void HandleRunningMenuUI();

void HandleRunningMenuUI() {
    sidePanel.x = active_width * 0.94;
    sidePanel.width = active_width * 0.06;
    sidePanel.height = active_height;

    DrawRectangleRec(sidePanel, Fade(LIGHTGRAY, uiTransparency));

    if(CheckCollisionPointRec(GetMousePosition(), sidePanel)) {
        isMouseOverUI = true;
    } else {
        isMouseOverUI = false;
    }

    int cellIndex = 0;
    // Cells Menu
    for(int i = 0; i < MAX_CELLTYPES; i++) {
        if(cellTypes[i].index > -1) {
            int cellPositionX = 0.97 * active_width;
            int cellPositionY = 0.13 * active_height + 0.05 * active_height * cellIndex;
            int cellWidth = 25;
            int cellHeight = 25;
            if(CheckCollisionPointRec(mousePosition,
            (Rectangle) {cellPositionX, cellPositionY, cellWidth, cellHeight})) {
                hoveredCellType = cellTypes[i].index;
            } else {
                hoveredCellType = -1;
            }
            
            if(GuiButton((Rectangle) {cellPositionX - (cellWidth * 1.25), cellPositionY, cellWidth, cellHeight}, "...")) {
                textBoxSelected = false;
                selectedIndex = i;
                selectedColor = cellTypes[selectedIndex].color;
                selectedNeighbourType = cellTypes[selectedIndex].neighbourType;
                letterCount = 0;
                if(defaultCell == selectedIndex) {
                    isDefault = true;
                } else {
                    isDefault = false;
                }
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
                        newTargetRelationShips[x].relationshipType = 0;
                                                    
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

            cellIndex++;
        }
    }
    DrawText("Cells", 0.97 * active_width, 0.1 * active_height, 10, BLACK);

    isWrapping = GuiCheckBox((Rectangle) {0.955 * active_width, 0.72 * active_height, 0.015 * active_width, 20}, "Wrap", isWrapping);

    if(GuiButton((Rectangle) {0.95 * active_width, 0.76 * active_height, 0.04 * active_width, 20}, "Generate")) {
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
                finalCells[x][y] = generatingCellTypes[GetRandomValue(0, types_amount - 1)];
                cells[x][y] = finalCells[x][y];
            }
        }
    }

    if(GuiButton((Rectangle) {0.95 * active_width, 0.80 * active_height, 0.04 * active_width, 20}, brushSizeNames[brushSize])) {
        if(brushSize >= MAX_BRUSH_SIZE) {
            brushSize = MIN_BRUSH_SIZE;
        } else {
            brushSize++;
        }
    }

    if(GuiButton((Rectangle) {0.95 * active_width, 0.84 * active_height, 0.04 * active_width, 20}, "Step")) {
        generationFramesCounter = 0;
        LoopCells();
    }

    if(GuiButton((Rectangle) {0.95 * active_width, 0.88 * active_height, 0.04 * active_width, 20}, generationSpeedMultiplierNames[generationSpeedMultiplier])) {
        if(generationSpeedMultiplier >= MAX_GENERATION_SPEED_MULTIPLIER) {
            generationSpeedMultiplier = MIN_GENERATION_SPEED_MULTIPLIER;
        } else {
            generationSpeedMultiplier++;
        }
    }

    if(GuiButton((Rectangle) {0.95 * active_width, 0.92 * active_height, 0.04 * active_width, 20}, "Clear")) {
        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                finalCells[x][y] = defaultCell;
                cells[x][y] = defaultCell;
            }
        }
    }

    if(GuiButton((Rectangle) {0.95 * active_width, 0.96 * active_height, 0.04 * active_width, 20}, isShowingUI == false ? "Pause" : "Resume")) {
        isShowingUI = !isShowingUI;
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

    if(textBoxSelected == true) {
        DrawRectangleLines(textBox.x, textBox.y, textBox.width, textBox.height, BLUE);
        
    } else {
        DrawRectangleLines(textBox.x, textBox.y, textBox.width, textBox.height, GRAY);
        
    }

    DrawText(letterCount <= 0 ? "Cell Name" : cellName, textBox.x + 5, textBox.y + 4, 8, BLACK);

    // DrawText("Neighbours:", dialogHolderPosX + 20, dialogHolderPosY + 75, 8, BLACK);
    // if(GuiButton((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 90, 100, 20}, neighbours[selectedNeighbourType])) {
    //     if(selectedNeighbourType >= MAX_NEIGHBOUR_TYPES - 1) {
    //         selectedNeighbourType = 0;
    //     } else {
    //         selectedNeighbourType ++;
    //     }
    // }

    DrawText("Relationships:", dialogHolderPosX + 20, dialogHolderPosY + 75, 8, BLACK);

    if(GuiButton((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 90, 100, 20}, "Add Relationship")) {
        for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
            if(newTargetRelationShips[i].index == -1) {
                newTargetRelationShips[i].index = i;
                newTargetRelationShips[i].amount = 0;
                newTargetRelationShips[i].toAmount = 0;
                newTargetRelationShips[i].relationshipType = 0;
                newTargetRelationShips[i].resultCellTypeIndex = 0;
                newTargetRelationShips[i].targetCellTypeIndex = 0;

                break;
            }
        }
    }

    isDefault = GuiCheckBox((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 120, 20, 20}, "Is Default", isDefault);
    if(isDefault) {
        defaultCell = selectedIndex;
    }

    int cellColorPickerSize = 100;
        
    DrawText("Color:", dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - (cellColorPickerSize + 100), dialogHolderPosY + 30, 8, BLACK);

    DrawRectangle(dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - (cellColorPickerSize + 100), dialogHolderPosY + 45, 30, 30, selectedColor);

    selectedColor = GuiColorPicker((Rectangle) {dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - (cellColorPickerSize + 60), dialogHolderPosY + 45, cellColorPickerSize, cellColorPickerSize}, selectedColor);
    
    DrawText("Target", panelRec.x + panelRec.width * 0.055, panelRec.y - 20, 8, BLACK);
    DrawText("Type", panelRec.x + panelRec.width * 0.195, panelRec.y - 20, 8, BLACK);
    DrawText("Result", panelRec.x + panelRec.width * 0.325, panelRec.y - 20, 8, BLACK);
    DrawText("Amount", panelRec.x + panelRec.width * 0.475, panelRec.y - 20, 8, BLACK);
    DrawText("Neighbours", panelRec.x + panelRec.width * 0.655, panelRec.y - 20, 8, BLACK);
    DrawText("Delete", panelRec.x + panelRec.width * 0.870, panelRec.y - 20, 8, BLACK);

    panelView = GuiScrollPanel(panelRec, panelContentRec, &panelScroll);

    BeginScissorMode(panelView.x, panelView.y, panelView.width, panelView.height);

    int relationShipSpaceSeparator = 20;
    int topSpace = 10;
    int relationshipCounter = 0;
    for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
        if(newTargetRelationShips[i].index > -1) {
            if(GuiButton((Rectangle) {panelRec.x + panelRec.width * 0.035, panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)), 60, 20}, cellTypes[newTargetRelationShips[i].targetCellTypeIndex].name )) {
                int tempIndex = newTargetRelationShips[i].targetCellTypeIndex;
                while(true) {
                    if(tempIndex >= MAX_CELLTYPES - 1) {
                        tempIndex = 0;
                    } else {
                        tempIndex++;
                    }
                    if(cellTypes[tempIndex].index != -1) {
                        newTargetRelationShips[i].targetCellTypeIndex = tempIndex;
                        break;
                    }                    
                }
            }

            

            if(GuiButton((Rectangle) {panelRec.x + panelRec.width * 0.195, panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)), 25, 20}, relationshipTypes[newTargetRelationShips[i].relationshipType])) {
                if(newTargetRelationShips[i].relationshipType >= MAX_RELATIONSHIP_TYPES - 1) {
                    newTargetRelationShips[i].relationshipType = 0;
                } else {
                    newTargetRelationShips[i].relationshipType++;
                }
            }

            

            if(GuiButton((Rectangle) {panelRec.x + panelRec.width * 0.3, panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)), 60, 20}, cellTypes[newTargetRelationShips[i].resultCellTypeIndex].name)) {
                int tempIndex = newTargetRelationShips[i].resultCellTypeIndex;
                while(true) {
                    if(tempIndex >= MAX_CELLTYPES - 1) {
                        tempIndex = 0;
                    } else {
                        tempIndex++;
                    }
                    if(cellTypes[tempIndex].index != -1) {
                        newTargetRelationShips[i].resultCellTypeIndex = tempIndex;
                        break;
                    }                    
                }
            }

            
            
            
            if(GuiButton((Rectangle) {panelRec.x + panelRec.width * (newTargetRelationShips[i].relationshipType != 7 ? 0.49 : 0.465), panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)), 20, 20}, TextFormat("%i", newTargetRelationShips[i].amount))) {
                if(newTargetRelationShips[i].amount >= 9 -1) {
                    newTargetRelationShips[i].amount = 0;
                } else {
                    newTargetRelationShips[i].amount++;
                }
            }
            if(newTargetRelationShips[i].relationshipType == 7) {
                if(GuiButton((Rectangle) {panelRec.x + panelRec.width * 0.515, panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)), 20, 20}, TextFormat("%i", newTargetRelationShips[i].toAmount))) {
                    if(newTargetRelationShips[i].toAmount >= 9 -1) {
                        newTargetRelationShips[i].toAmount = 0;
                    } else {
                        newTargetRelationShips[i].toAmount++;
                    }
                }
            }

            Rectangle center = { panelRec.x + panelRec.width * 0.695
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01))
            , panelRec.width * 0.01 + 5, panelRec.width * 0.01 + 5};
            DrawRectangleRec(center, LIGHTGRAY);
            DrawRectangleLinesEx(center, 1, BLACK);
            
            Rectangle left = { panelRec.x + panelRec.width * 0.695 - (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01))
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

            Rectangle bottomLeft = { panelRec.x + panelRec.width * 0.695 - (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + (2.1 * (panelRec.width * 0.01))
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

            Rectangle bottom = { panelRec.x + panelRec.width * 0.695
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + (2.1 * (panelRec.width * 0.01))
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

            Rectangle bottomRight = { panelRec.x + panelRec.width * 0.695 + (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) + (2.1 * (panelRec.width * 0.01))
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

            Rectangle right = { panelRec.x + panelRec.width * 0.695 + (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01))
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
            
            Rectangle topRight = { panelRec.x + panelRec.width * 0.695 + (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) - (2.1 * (panelRec.width * 0.01))
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

            Rectangle top = { panelRec.x + panelRec.width * 0.695
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) - (2.1 * (panelRec.width * 0.01))
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

            Rectangle topLeft = { panelRec.x + panelRec.width * 0.695 - (2.1 * (panelRec.width * 0.01))
            , panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)) - (2.1 * (panelRec.width * 0.01))
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

            
            
            if(GuiButton((Rectangle) {panelRec.x + panelRec.width * 0.88, panelRec.y + panelScroll.y + (topSpace * (panelRec.height * 0.01)) + relationshipCounter * (relationShipSpaceSeparator * (panelRec.height * 0.01)), 20, 20}, "x")) {
                newTargetRelationShips[i].index = -1;
            }

            relationshipCounter++;
        }
    }
    
        
    EndScissorMode();

    panelContentRec.height = (relationShipSpaceSeparator * (panelRec.height * 0.01)) * relationshipCounter;

    if(GuiButton((Rectangle) {dialogHolderPosX + 17, dialogHolderPosY + dialogHolderHeight - ( 100 * DIALOG_VIEW_PERCENT + 0.40f), 100, 20}, "Save")) {
        cellTypes[selectedIndex].color = selectedColor;
        cellTypes[selectedIndex].index = selectedIndex;
        strcpy(cellTypes[selectedIndex].name, cellName);
        cellTypes[selectedIndex].neighbourType = selectedNeighbourType;
        
        for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
            cellTypes[selectedIndex].targetCellRelationship[x] = malloc(sizeof(T_TargetCellRelationship));
            cellTypes[selectedIndex].targetCellRelationship[x]->amount = newTargetRelationShips[x].amount;
            cellTypes[selectedIndex].targetCellRelationship[x]->toAmount = newTargetRelationShips[x].toAmount;
            cellTypes[selectedIndex].targetCellRelationship[x]->index = newTargetRelationShips[x].index;
            cellTypes[selectedIndex].targetCellRelationship[x]->relationshipType = newTargetRelationShips[x].relationshipType;
                                        
            cellTypes[selectedIndex].targetCellRelationship[x]->resultCellTypeIndex = newTargetRelationShips[x].resultCellTypeIndex;
            cellTypes[selectedIndex].targetCellRelationship[x]->targetCellTypeIndex = newTargetRelationShips[x].targetCellTypeIndex;
            cellTypes[selectedIndex].targetCellRelationship[x]->bottomLeft = newTargetRelationShips[x].bottomLeft;
            cellTypes[selectedIndex].targetCellRelationship[x]->bottom = newTargetRelationShips[x].bottom;
            cellTypes[selectedIndex].targetCellRelationship[x]->bottomRight = newTargetRelationShips[x].bottomRight;
            cellTypes[selectedIndex].targetCellRelationship[x]->right = newTargetRelationShips[x].right;
            cellTypes[selectedIndex].targetCellRelationship[x]->topRight = newTargetRelationShips[x].topRight;
            cellTypes[selectedIndex].targetCellRelationship[x]->top = newTargetRelationShips[x].top;
            cellTypes[selectedIndex].targetCellRelationship[x]->topLeft = newTargetRelationShips[x].topLeft;
            cellTypes[selectedIndex].targetCellRelationship[x]->left = newTargetRelationShips[x].left;
        }
        selectedIndex = -1;
        
        
        isShowingCreateCellTypeDialog = false;
    }

    if(GuiButton((Rectangle) {dialogHolderPosX + panelRec.width + dialogHolderWidthExtra - 120, dialogHolderPosY + dialogHolderHeight - ( 100 * DIALOG_VIEW_PERCENT + 0.40f), 100, 20}, "Delete")) {
        if(isDefault == false) {
            cellTypes[selectedIndex].color = WHITE;
            cellTypes[selectedIndex].index = -1;
            strcpy(cellTypes[selectedIndex].name, "UNSET");
            cellTypes[selectedIndex].neighbourType = 0;
            
            for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
                free(cellTypes[selectedIndex].targetCellRelationship[x]);
            }
            selectedIndex = -1;

            for(int x = 0; x < MAX_CELLS_X; x++) {
                for(int y = 0; y < MAX_CELLS_Y; y++) {
                    finalCells[x][y] = defaultCell;
                    cells[x][y] = defaultCell;
                }
            }
                        
            isShowingCreateCellTypeDialog = false;
        }
    }
}