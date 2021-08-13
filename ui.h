#include "cellular.h"
#include "easings.h"

#define SCREEN_WIDTH                    1480
#define SCREEN_HEIGHT                   800

#define MAX_NEIGHBOUR_TYPES             4

#define DIALOG_VIEW_PERCENT             0.25f
#define DIALOG_CONTENT_PERC             0.241f

#define MIN_GENERATION_SPEED_MULTIPLIER 1
#define MAX_GENERATION_SPEED_MULTIPLIER 3

#define MIN_BRUSH_SIZE                  0
#define MAX_BRUSH_SIZE                  2

// UI
int framesCounter = 0;
Rectangle leftUIBackground = {- (0.2 * SCREEN_WIDTH), 0, 0.2 * SCREEN_WIDTH, SCREEN_HEIGHT, LIGHTGRAY};
Vector2 mousePosition;
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
bool selectedCellNameText = false;
int selectedIndex = -1;
Rectangle cellNameTextBox = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 50};
struct TargetCellRelationship newTargetRelationShips[MAX_RELATIONSHIPS];

int neighbourType = 0;
const char *neighbours[MAX_NEIGHBOUR_TYPES] = 
{ "Moore", "Von Neumann", "Vertical", "Horizontal"};
int selectedNeighbourType = 0;

Rectangle panelRec = { 0.5 * SCREEN_WIDTH - (DIALOG_VIEW_PERCENT * SCREEN_WIDTH) / 2, 0.6 * SCREEN_HEIGHT - (DIALOG_VIEW_PERCENT * SCREEN_HEIGHT) / 2, (DIALOG_VIEW_PERCENT * SCREEN_WIDTH), (DIALOG_VIEW_PERCENT * SCREEN_HEIGHT) };
Rectangle panelContentRec = {0, 0, (DIALOG_CONTENT_PERC * SCREEN_WIDTH), (0.65 * SCREEN_HEIGHT) };
Vector2 panelScroll = { 99, 0 };
Rectangle panelView;

char cellName[MAX_NAME_LENGTH + 1] = "\0";
int letterCount = 0;
bool textBoxSelected = false;
bool isDefault = false;
int defaultCell = 0;

int generationSpeedMultiplier = 1;
const char *generationSpeedMultiplierNames[MAX_GENERATION_SPEED_MULTIPLIER] = {"Slow", "Fast", "Fastest"};

int brushSize = 0;
const char *brushSizeNames[MAX_BRUSH_SIZE + 1] = {"Small", "Medium", "Large"};

// PROTOTYPES
void HandleEditCellUI();
void HandleRunningMenuUI();

void HandleRunningMenuUI() {
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
                        newTargetRelationShips[x].index = cellTypes[selectedIndex].targetCellRelationship[x]->index;
                        newTargetRelationShips[x].relationshipType = cellTypes[selectedIndex].targetCellRelationship[x]->relationshipType;
                                                    
                        newTargetRelationShips[x].resultCellTypeIndex = cellTypes[selectedIndex].targetCellRelationship[x]->resultCellTypeIndex;
                        newTargetRelationShips[x].targetCellTypeIndex = cellTypes[selectedIndex].targetCellRelationship[x]->targetCellTypeIndex;
                    } else {
                        newTargetRelationShips[x].amount = 0;
                        newTargetRelationShips[x].index = -1;
                        newTargetRelationShips[x].relationshipType = 0;
                                                    
                        newTargetRelationShips[x].resultCellTypeIndex = 0;
                        newTargetRelationShips[x].targetCellTypeIndex = 0;
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

    if(GuiButton((Rectangle) {0.95 * SCREEN_WIDTH, 0.88 * SCREEN_HEIGHT, 60, 20}, brushSizeNames[brushSize])) {
        if(brushSize >= MAX_BRUSH_SIZE) {
            brushSize = MIN_BRUSH_SIZE;
        } else {
            brushSize++;
        }
    }

    if(GuiButton((Rectangle) {0.95 * SCREEN_WIDTH, 0.92 * SCREEN_HEIGHT, 60, 20}, generationSpeedMultiplierNames[generationSpeedMultiplier - 1])) {
        if(generationSpeedMultiplier >= MAX_GENERATION_SPEED_MULTIPLIER) {
            generationSpeedMultiplier = MIN_GENERATION_SPEED_MULTIPLIER;
        } else {
            generationSpeedMultiplier++;
        }
    }

    if(GuiButton((Rectangle) {0.95 * SCREEN_WIDTH, 0.96 * SCREEN_HEIGHT, 60, 20}, "Clear")) {
        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                finalCells[x][y] = defaultCell;
                cells[x][y] = defaultCell;
            }
        }
    }
}

void HandleEditCellUI() {
    float dialogHolderHeight = ((DIALOG_VIEW_PERCENT + 0.30f) * SCREEN_HEIGHT);
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

    DrawText("Neighbours:", dialogHolderPosX + 20, dialogHolderPosY + 75, 8, BLACK);
    if(GuiButton((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 90, 100, 20}, neighbours[selectedNeighbourType])) {
        if(selectedNeighbourType >= MAX_NEIGHBOUR_TYPES - 1) {
            selectedNeighbourType = 0;
        } else {
            selectedNeighbourType ++;
        }
    }

    isDefault = GuiCheckBox((Rectangle) {dialogHolderPosX + 20, dialogHolderPosY + 120, 20, 20}, "Is Default", isDefault);
    if(isDefault) {
        defaultCell = selectedIndex;
    }

    int cellColorPickerSize = 100;
        
    DrawText("Color:", dialogHolderPosX + 260, dialogHolderPosY + 30, 8, BLACK);
    selectedColor = GuiColorPicker((Rectangle) {dialogHolderPosX + 260, dialogHolderPosY + 45, cellColorPickerSize, cellColorPickerSize}, selectedColor);
    
    DrawText("Relationships:", dialogHolderPosX + 20, dialogHolderPosY + 165, 8, BLACK);

    if(GuiButton((Rectangle) {dialogHolderPosX + 290, dialogHolderPosY + 160, 100, 20}, "Add Relationship")) {
        for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
            if(newTargetRelationShips[i].index == -1) {
                newTargetRelationShips[i].index = i;
                newTargetRelationShips[i].amount = 0;
                newTargetRelationShips[i].relationshipType = 0;
                newTargetRelationShips[i].resultCellTypeIndex = 0;
                newTargetRelationShips[i].targetCellTypeIndex = 0;

                break;
            }
        }
    }

    panelView = GuiScrollPanel(panelRec, panelContentRec, &panelScroll);

    BeginScissorMode(panelView.x, panelView.y, panelView.width, panelView.height);

    int relationShipSpaceSeparator = 45;
    int relationshipCounter = 0;
    for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
        if(newTargetRelationShips[i].index > -1) {
            DrawText("Target:", panelRec.x + panelScroll.x + 10, panelRec.y + panelScroll.y + 10 + relationshipCounter * relationShipSpaceSeparator, 8, BLACK);

            if(GuiButton((Rectangle) {panelRec.x + panelScroll.x + 10, panelRec.y + panelScroll.y + 25 + relationshipCounter * relationShipSpaceSeparator, 60, 20}, cellTypes[newTargetRelationShips[i].targetCellTypeIndex].name )) {
                if(newTargetRelationShips[i].targetCellTypeIndex >= MAX_CELLTYPES - 1) {
                    newTargetRelationShips[i].targetCellTypeIndex = 0;
                } else {
                    newTargetRelationShips[i].targetCellTypeIndex++;
                }
            }

            DrawText("Relationship:", panelRec.x + panelScroll.x + 80, panelRec.y + panelScroll.y + 10 + relationshipCounter * relationShipSpaceSeparator, 8, BLACK);

            if(GuiButton((Rectangle) {panelRec.x + panelScroll.x + 80, panelRec.y + panelScroll.y + 25 + relationshipCounter * relationShipSpaceSeparator, 60, 20}, relationshipTypes[newTargetRelationShips[i].relationshipType])) {
                if(newTargetRelationShips[i].relationshipType >= MAX_RELATIONSHIP_TYPES - 1) {
                    newTargetRelationShips[i].relationshipType = 0;
                } else {
                    newTargetRelationShips[i].relationshipType++;
                }
            }

            DrawText("Result:", panelRec.x + panelScroll.x + 160, panelRec.y + panelScroll.y + 10 + relationshipCounter * relationShipSpaceSeparator, 8, BLACK);

            if(GuiButton((Rectangle) {panelRec.x + panelScroll.x + 160, panelRec.y + panelScroll.y + 25 + relationshipCounter * relationShipSpaceSeparator, 60, 20}, cellTypes[newTargetRelationShips[i].resultCellTypeIndex].name)) {
                if(newTargetRelationShips[i].resultCellTypeIndex >= MAX_CELLTYPES - 1) {
                    newTargetRelationShips[i].resultCellTypeIndex = 0;
                } else {
                    newTargetRelationShips[i].resultCellTypeIndex++;
                }
            }

            DrawText("Amount:", panelRec.x + panelScroll.x + 240, panelRec.y + panelScroll.y + 10 + relationshipCounter * relationShipSpaceSeparator, 8, BLACK);
            if(GuiButton((Rectangle) {panelRec.x + panelScroll.x + 245, panelRec.y + panelScroll.y + 25 + relationshipCounter * relationShipSpaceSeparator, 20, 20}, TextFormat("%i", newTargetRelationShips[i].amount))) {
                if(newTargetRelationShips[i].amount >= 9 -1) {
                    newTargetRelationShips[i].amount = 0;
                } else {
                    newTargetRelationShips[i].amount++;
                }
            }

            DrawText("Delete:", panelRec.x + panelScroll.x + 300, panelRec.y + panelScroll.y + 10 + relationshipCounter * relationShipSpaceSeparator, 8, BLACK);
            if(GuiButton((Rectangle) {panelRec.x + panelScroll.x + 305, panelRec.y + panelScroll.y + 25 + relationshipCounter * relationShipSpaceSeparator, 20, 20}, "x")) {
                newTargetRelationShips[i].index = -1;
            }
            relationshipCounter++;
        }
    }
        
    EndScissorMode();

    if(GuiButton((Rectangle) {dialogHolderPosX + 17, dialogHolderPosY + dialogHolderHeight - 30, 100, 20}, "Save")) {
        cellTypes[selectedIndex].color = selectedColor;
        cellTypes[selectedIndex].index = selectedIndex;
        strcpy(cellTypes[selectedIndex].name, cellName);
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
        
        
        isShowingCreateCellTypeDialog = false;
    }
}