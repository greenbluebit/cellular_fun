#include "classes.h"
#include <string.h>

#define MAX_CELLS_X             500
#define MAX_CELLS_Y             500
#define MAX_CELLTYPES           10
#define MAX_RELATIONSHIP_TYPES  8


struct CellType cellTypes[MAX_CELLTYPES];
const char *relationshipTypes[MAX_RELATIONSHIP_TYPES] =
{"=", "!=", "<", "<=", ">", ">=", ".", ">=<="};

int cells[MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };
int finalCells [MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };

bool isWrapping = true;
bool selfActualizing = true;

//int getGridNeighbours(int gridX, int gridY, int cells[MAX_CELLS_X] [MAX_CELLS_Y], int targetNeighbourIndex, int relationshipTy);

void SetupCells() {
    for(int i = 0; i < MAX_CELLTYPES; i++) {
        cellTypes[i].index = -1;
        strcpy(cellTypes[i].name, "Unset");
    }

    cellTypes[0].color = WHITE;
    cellTypes[0].index = 0;
    strcpy(cellTypes[0].name, "Default");
}

void LoopCells() {
    for(int x = 0; x < MAX_CELLS_X; x++) {
        for(int y = 0; y < MAX_CELLS_Y; y++) {
            for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                int cellIndex = cells[x][y];
                if(cellTypes[cellIndex].targetCellRelationship[i] != 0 && cellTypes[cellIndex].targetCellRelationship[i]->index > -1) {
                    if(selfActualizing) {
                        int neighbours = 0;
                        
                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottomLeft) {
                            int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                            int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : 0;
                            neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && cellTypes[cells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottom) {
                            int currentCellX = x;
                            int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : ( isWrapping == true ? 0 : y + 1 );
                            neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && cellTypes[cells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottomRight) {
                            int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                            int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : ( isWrapping == true ? 0 : y + 1 );
                            neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && cellTypes[cells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->right) {
                            int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                            int currentCellY = y;
                            neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && cellTypes[cells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->topRight) {
                            int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                            int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1);
                            neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && cellTypes[cells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->top) {
                            int currentCellX = x;
                            int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1 );
                            neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && cellTypes[cells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->topLeft) {
                            int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                            int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1 );
                            neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && cellTypes[cells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                        }

                        bool relationshipFullfilled = false; 
                        if( strcmp(relationshipTypes[cellTypes[cellIndex].targetCellRelationship[i]->relationshipType], "=") == 0)
                        {
                            if(neighbours == cellTypes[cellIndex].targetCellRelationship[i]->amount)
                            {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }
                        } else if(strcmp(relationshipTypes[cellTypes[cellIndex].targetCellRelationship[i]->relationshipType], "!=") == 0)
                        {
                            if(neighbours != cellTypes[cellIndex].targetCellRelationship[i]->amount)
                            {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }
                        }else if(strcmp(relationshipTypes[cellTypes[cellIndex].targetCellRelationship[i]->relationshipType], "<") == 0)
                        {
                            if(neighbours < cellTypes[cellIndex].targetCellRelationship[i]->amount)
                            {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }
                        }else if(strcmp(relationshipTypes[cellTypes[cellIndex].targetCellRelationship[i]->relationshipType], "<=") == 0)
                        {
                            if(neighbours <= cellTypes[cellIndex].targetCellRelationship[i]->amount)
                            {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }
                        }else if(strcmp(relationshipTypes[cellTypes[cellIndex].targetCellRelationship[i]->relationshipType], ">") == 0)
                        {
                            if(neighbours > cellTypes[cellIndex].targetCellRelationship[i]->amount)
                            {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }
                        }else if(strcmp(relationshipTypes[cellTypes[cellIndex].targetCellRelationship[i]->relationshipType], ">=") == 0)
                        {
                            if(neighbours >= cellTypes[cellIndex].targetCellRelationship[i]->amount)
                            {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }
                        } else if(strcmp(relationshipTypes[cellTypes[cellIndex].targetCellRelationship[i]->relationshipType], ".") == 0) {
                            cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                            relationshipFullfilled = true;
                        } else if(strcmp(relationshipTypes[cellTypes[cellIndex].targetCellRelationship[i]->relationshipType], ">=<=") == 0) {
                            if(neighbours >= cellTypes[cellIndex].targetCellRelationship[i]->amount && neighbours <= cellTypes[cellIndex].targetCellRelationship[i]->toAmount) {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }
                        }

                        if(relationshipFullfilled == true) {
                            break;
                        }     
                    } else {

                    }
                                   
                }
            }
        }
    }
}

// neighbours
// 0 - moore
// 1 - von Neumann
// 2 - vertical
// 3 - horizontal
// TEDO
// 4 - diagonal
// 

// int getGridNeighbours(int gridX, int gridY, int cells[MAX_CELLS_X] [MAX_CELLS_Y], int targetNeighbourIndex, int neighbourType) {
//     int result = 0;
//     if(cellTypes[cells[gridX][gridY]].neighbourType == 0) {
//         for(int neighbourX = gridX - 1; neighbourX <= gridX + 1; neighbourX++) {
//             for(int neighbourY = gridY - 1; neighbourY <= gridY + 1; neighbourY++) {
//                 int tempNeighbourX = neighbourX >= 0 && neighbourX < MAX_CELLS_X ? neighbourX : neighbourX <= 0 && isWrapping ? MAX_CELLS_X - 1 : neighbourX >= MAX_CELLS_X && isWrapping ? 0 : -1;
//                 int tempNeighbourY = neighbourY >= 0 && neighbourY < MAX_CELLS_Y ? neighbourY : neighbourY <= 0 && isWrapping ? MAX_CELLS_Y - 1 : neighbourY >= MAX_CELLS_Y && isWrapping ? 0 : -1;
                
//                 if(( (tempNeighbourX != gridX || tempNeighbourY != gridY) && tempNeighbourX >= 0 && tempNeighbourX < MAX_CELLS_X 
//                         && tempNeighbourY >= 0 && tempNeighbourY < MAX_CELLS_Y && cellTypes[cells[tempNeighbourX][tempNeighbourY]].index == targetNeighbourIndex) ) {
//                     result += 1;
//                 }
//             }
//         }
        
//     } else if(cellTypes[cells[gridX][gridY]].neighbourType  == 1) {
//         result += (gridX - 1 >= 0 && cellTypes[cells[gridX - 1][gridY]].index == targetNeighbourIndex) || (isWrapping == true && cellTypes[cells[MAX_CELLS_X - 1] [gridY]].index == targetNeighbourIndex) ? 1 : 0;
//         result += (gridX + 1 < MAX_CELLS_X && cellTypes[cells[gridX + 1][gridY]].index == targetNeighbourIndex) || (isWrapping == true && cellTypes[cells[0][gridY]].index == targetNeighbourIndex) ? 1 : 0;
//         result += (gridY - 1 >= 0 && cellTypes[cells[gridX][gridY - 1]].index == targetNeighbourIndex) || (isWrapping == true && cellTypes[cells[gridX][MAX_CELLS_Y - 1]].index == targetNeighbourIndex) ? 1 : 0;
//         result += (gridY + 1 < MAX_CELLS_Y && cellTypes[cells[gridX][gridY + 1]].index == targetNeighbourIndex) || (isWrapping == true && cellTypes[cells[gridX][0]].index == targetNeighbourIndex) ? 1 : 0;
//     } else if(cellTypes[cells[gridX][gridY]].neighbourType  == 2) {
//         result += (gridY - 1 >= 0 && cellTypes[cells[gridX][gridY - 1]].index == targetNeighbourIndex) || (isWrapping == true && cellTypes[cells[gridX][MAX_CELLS_Y - 1]].index == targetNeighbourIndex) ? 1 : 0;
//         result += (gridY + 1 < MAX_CELLS_Y && cellTypes[cells[gridX][gridY + 1]].index == targetNeighbourIndex) || (isWrapping == true && cellTypes[cells[gridX][0]].index == targetNeighbourIndex) ? 1 : 0;
//     } else if(cellTypes[cells[gridX][gridY]].neighbourType  == 3) {
//         result += (gridX - 1 >= 0 && cellTypes[cells[gridX - 1][gridY]].index == targetNeighbourIndex) || (isWrapping == true && cellTypes[cells[MAX_CELLS_X - 1] [gridY]].index == targetNeighbourIndex) ? 1 : 0;
//         result += (gridX + 1 < MAX_CELLS_X && cellTypes[cells[gridX + 1][gridY]].index == targetNeighbourIndex) || (isWrapping == true && cellTypes[cells[0][gridY]].index == targetNeighbourIndex) ? 1 : 0;
//     }

//     return result;
// }