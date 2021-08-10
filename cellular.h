#include "classes.h"
#include <string.h>

#define MAX_CELLS_X         500
#define MAX_CELLS_Y         500
#define MAX_CELLTYPES       10


struct CellType cellTypes[MAX_CELLTYPES];

int cells[MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };
int finalCells [MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };

void SetupCells() {
    for(int i = 0; i < MAX_CELLTYPES; i++) {
        cellTypes[i].index = -1;           
    }

    cellTypes[0].color = WHITE;
    cellTypes[0].index = 0;
}

void LoopCells() {
    for(int x = 0; x < MAX_CELLS_X; x++) {
        for(int y = 0; y < MAX_CELLS_Y; y++) {
            for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                int cellIndex = cells[x][y];
                if(cellTypes[cellIndex].targetCellRelationship[i] != 0) {
                    int neighbours = getGridNeighbours(x, y, finalCells, cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex);
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

// neighbours
// 0 - moore
// 1 - von Neumann
// 2 - vertical
// 3 - horizontal
int getGridNeighbours(int gridX, int gridY, int cells[MAX_CELLS_X] [MAX_CELLS_Y], int targetNeighbourIndex) {
    int result = 0;
    if(cellTypes[cells[gridX][gridY]].neighbourType == 0) {
        result += gridX - 1 >= 0 && cellTypes[cells[gridX - 1][gridY]].index == targetNeighbourIndex ? 1 : 0;
        result += gridX + 1 < MAX_CELLS_X && cellTypes[cells[gridX + 1][gridY]].index == targetNeighbourIndex ? 1 : 0;
        result += gridY - 1 >= 0 && cellTypes[cells[gridX][gridY - 1]].index == targetNeighbourIndex ? 1 : 0;
        result += gridY + 1 < MAX_CELLS_Y && cellTypes[cells[gridX][gridY + 1]].index == targetNeighbourIndex ? 1 : 0;
    } else if(cellTypes[cells[gridX][gridY]].neighbourType  == 1) {
        for(int neighbourX = gridX - 1; neighbourX <= gridX + 1; neighbourX++) {
            for(int neighbourY = gridY - 1; neighbourY <= gridY + 1; neighbourY++) {
                if(neighbourX >= 0 && neighbourX < MAX_CELLS_X && neighbourY >= 0 && neighbourY < MAX_CELLS_Y) {
                    if((neighbourX != gridX || neighbourY != gridY) && cellTypes[cells[neighbourX][neighbourY]].index == targetNeighbourIndex) {
                        result += 1;
                    }
                }
            }
        }
    } else if(cellTypes[cells[gridX][gridY]].neighbourType  == 2) {
        result += gridY - 1 >= 0 && cellTypes[cells[gridX][gridY - 1]].index == targetNeighbourIndex ? 1 : 0;
        result += gridY + 1 < MAX_CELLS_Y && cellTypes[cells[gridX][gridY + 1]].index == targetNeighbourIndex ? 1 : 0;
    } else if(cellTypes[cells[gridX][gridY]].neighbourType  == 3) {
        result += gridX - 1 >= 0 && cellTypes[cells[gridX - 1][gridY]].index == targetNeighbourIndex ? 1 : 0;
        result += gridX + 1 < MAX_CELLS_X && cellTypes[cells[gridX + 1][gridY]].index == targetNeighbourIndex ? 1 : 0;
    }

    return result;
}