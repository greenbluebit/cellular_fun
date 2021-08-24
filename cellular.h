#include "classes.h"
#include <string.h>
#include "vector.h"

#define MAX_CELLS_X             260
#define MAX_CELLS_Y             125

//#define MAX_CELLS_X             500
//#define MAX_CELLS_Y             500

// #define PHYSICS_MAX_CELLS_X     210
//#define PHYSICS_MAX_CELLS_Y     110

#define MAX_CELLTYPES           10
#define MAX_COMPARISON_TYPES    8
#define MAX_RELATIONSHIP_TYPES  2

#define MAX_CHANCE 100
#define MIN_CHANCE 1

struct CellType cellTypes[MAX_CELLTYPES];
const char *comparisonTypes[MAX_COMPARISON_TYPES] =
{"=", "!=", "<", "<=", ">", ">=", ".", ">=<="};

const char *relationshipTypes[MAX_RELATIONSHIP_TYPES] = {"Self", "Neighbour"};

int cells[MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };
int finalCells [MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };

//int physicsCells[MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };
//int physicsFinalCells [MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };

int defaultCell = 0;

bool isWrapping = true;

vector changes;

// PERFORMANCE DEBUG
bool isDebug = false;
clock_t start_clk;
double cells_performance_timer = 0;
double changes_performance_timer = 0;
double pre_changes_performance_timer = 0;

void SetupCells() {
    for(int i = 0; i < MAX_CELLTYPES; i++) {
        cellTypes[i].index = -1;
        strcpy(cellTypes[i].name, "Unset");
    }

    cellTypes[0].color = WHITE;
    cellTypes[0].index = 0;
    strcpy(cellTypes[0].name, "Default");

    vector_init(&changes);
}

void LoopCells() {
    
    if(isDebug) {
        start_clk = clock();
    }
    
    for(int x = 0; x < MAX_CELLS_X; x++) {
        for(int y = 0; y < MAX_CELLS_Y; y++) {
            for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                int cellIndex = cells[x][y];
                if(cellTypes[cellIndex].targetCellRelationship[i] != 0 && cellTypes[cellIndex].targetCellRelationship[i]->index > -1 && (cellTypes[cellIndex].immovable == false || (cellTypes[cellIndex].immovable == true && cellTypes[cellIndex].targetCellRelationship[i]->relationshipType == 0 ))) {
                    bool relationshipFullfilled = false; 
                    int chanceValue = GetRandomValue(0, MAX_CHANCE);
                    int chance = cellTypes[cellIndex].targetCellRelationship[i]->chance == MAX_CHANCE || chanceValue <= cellTypes[cellIndex].targetCellRelationship[i]->chance;
                    if(chance && cellTypes[cellIndex].targetCellRelationship[i]->relationshipType == 0) {
                        
                         if(chance) {
                            int neighbours = 0;

                            if(cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1) {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }

                            if(cellTypes[cellIndex].targetCellRelationship[i]->bottomLeft) {
                                int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                                int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : 0;
                                neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && currentCellY > 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->bottom) {
                                int currentCellX = x;
                                int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : ( isWrapping == true ? 0 : y + 1 );
                                neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && currentCellY > 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->bottomRight) {
                                int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                                int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : ( isWrapping == true ? 0 : y + 1 );
                                neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && currentCellY > 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->right) {
                                int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                                int currentCellY = y;
                                neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && currentCellY > 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->topRight) {
                                int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                                int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1);
                                neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && currentCellY > 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->top) {
                                int currentCellX = x;
                                int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1 );
                                neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && currentCellY > 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->topLeft) {
                                int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                                int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1 );
                                neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && currentCellY > 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->left) {
                                int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                                int currentCellY = y;
                                neighbours += (currentCellX > 0 && currentCellX < MAX_CELLS_X && currentCellY > 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }

                            if( strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], "=") == 0)
                            {
                                if(neighbours == cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            } else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], "!=") == 0)
                            {
                                if(neighbours != cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], "<") == 0)
                            {
                                if(neighbours < cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], "<=") == 0)
                            {
                                if(neighbours <= cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], ">") == 0)
                            {
                                if(neighbours > cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], ">=") == 0)
                            {
                                if(neighbours >= cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            } else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], ".") == 0) {
                                cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            } else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], ">=<=") == 0) {
                                if(neighbours >= cellTypes[cellIndex].targetCellRelationship[i]->amount && neighbours <= cellTypes[cellIndex].targetCellRelationship[i]->toAmount) {
                                    cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }

                            if(relationshipFullfilled == true) {
                                //changes.pfVectorFree(&changes);
                                break;
                            }
                        }
                    } else if(chance && cellTypes[cellIndex].targetCellRelationship[i]->relationshipType == 1 && cellTypes[cellIndex].immovable == false) {
                        struct FromTo neighbours [8];
                        int neighbourCounter = 0;

                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottomLeft) {
                            int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                            int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : (isWrapping == true ? 0 : y + 1);

                            if(currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && (cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                //neighbours[neighbourCounter] = currentCellX;
                                //neighbours[neighbourCounter + 1] = currentCellY;
                                neighbours[neighbourCounter].cellX = x;
                                neighbours[neighbourCounter].cellY = y;
                                neighbours[neighbourCounter].toCellX = currentCellX;
                                neighbours[neighbourCounter].toCellY = currentCellY;
                                neighbours[neighbourCounter].resultId = cells[currentCellX][currentCellY];
                                neighbourCounter++;
                            }
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottom) {
                            int currentCellX = x;
                            cellTypes[cellIndex].lookDistance;
                            int 
                            int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : ( isWrapping == true ? 0 : y + 1 );
                            if(currentCellX >= 0 
                            && currentCellX < MAX_CELLS_X 
                            && currentCellY >= 0 
                            && currentCellY < MAX_CELLS_Y 
                            && (cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                //neighbours[neighbourCounter] = currentCellX;
                                //neighbours[neighbourCounter + 1] = currentCellY;
                                neighbours[neighbourCounter].cellX = x;
                                neighbours[neighbourCounter].cellY = y;
                                neighbours[neighbourCounter].toCellX = currentCellX;
                                neighbours[neighbourCounter].toCellY = currentCellY;
                                neighbours[neighbourCounter].resultId = cells[currentCellX][currentCellY];
                                neighbourCounter++;
                            }
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottomRight) {
                            int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                            int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : ( isWrapping == true ? 0 : y + 1 );
                            if(currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && (cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                //neighbours[neighbourCounter] = currentCellX;
                                //neighbours[neighbourCounter + 1] = currentCellY;
                                neighbours[neighbourCounter].cellX = x;
                                neighbours[neighbourCounter].cellY = y;
                                neighbours[neighbourCounter].toCellX = currentCellX;
                                neighbours[neighbourCounter].toCellY = currentCellY;
                                neighbours[neighbourCounter].resultId = cells[currentCellX][currentCellY];
                                neighbourCounter++;
                            }
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->right) {
                            int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                            int currentCellY = y;
                            if(currentCellX >= 0 && currentCellX < MAX_CELLS_X && (cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                //neighbours[neighbourCounter] = currentCellX;
                                //neighbours[neighbourCounter + 1] = currentCellY;
                                neighbours[neighbourCounter].cellX = x;
                                neighbours[neighbourCounter].cellY = y;
                                neighbours[neighbourCounter].toCellX = currentCellX;
                                neighbours[neighbourCounter].toCellY = currentCellY;
                                neighbours[neighbourCounter].resultId = cells[currentCellX][currentCellY];
                                neighbourCounter++;
                            }
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->topRight) {
                            int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                            int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1);
                            if(currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && (cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                //neighbours[neighbourCounter] = currentCellX;
                                //neighbours[neighbourCounter + 1] = currentCellY;
                                neighbours[neighbourCounter].cellX = x;
                                neighbours[neighbourCounter].cellY = y;
                                neighbours[neighbourCounter].toCellX = currentCellX;
                                neighbours[neighbourCounter].toCellY = currentCellY;
                                neighbours[neighbourCounter].resultId = cells[currentCellX][currentCellY];
                                neighbourCounter++;
                            }
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->top) {
                            int currentCellX = x;
                            int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1 );
                            if(currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && (cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                //neighbours[neighbourCounter] = currentCellX;
                                //neighbours[neighbourCounter + 1] = currentCellY;
                                neighbours[neighbourCounter].cellX = x;
                                neighbours[neighbourCounter].cellY = y;
                                neighbours[neighbourCounter].toCellX = currentCellX;
                                neighbours[neighbourCounter].toCellY = currentCellY;
                                neighbours[neighbourCounter].resultId = cells[currentCellX][currentCellY];
                                neighbourCounter++;

                            }
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->topLeft) {
                            int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                            int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1 );
                            if(currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && (cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                //neighbours[neighbourCounter] = currentCellX;
                                //neighbours[neighbourCounter + 1] = currentCellY;
                                neighbours[neighbourCounter].cellX = x;
                                neighbours[neighbourCounter].cellY = y;
                                neighbours[neighbourCounter].toCellX = currentCellX;
                                neighbours[neighbourCounter].toCellY = currentCellY;
                                neighbours[neighbourCounter].resultId = cells[currentCellX][currentCellY];
                                neighbourCounter++;
                            }
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->left) {
                            int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                            int currentCellY = y;
                            if(currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && (cellTypes[finalCells[currentCellX][currentCellY]].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                //neighbours[neighbourCounter] = currentCellX;
                                //neighbours[neighbourCounter + 1] = currentCellY;
                                neighbours[neighbourCounter].cellX = x;
                                neighbours[neighbourCounter].cellY = y;
                                neighbours[neighbourCounter].toCellX = currentCellX;
                                neighbours[neighbourCounter].toCellY = currentCellY;
                                neighbours[neighbourCounter].resultId = cells[currentCellX][currentCellY];
                                neighbourCounter++;
                            }
                        }

                        if(neighbourCounter > 0) {
                            //neighbourCounter /= 2;
                            int randomNeighbour = GetRandomValue(0, neighbourCounter > 0 ? neighbourCounter - 1 : 0);

                            int index = (MAX_CELLS_X) * y + x;
                            if(index == 249999) {
                                index = index;
                            }
                            struct FromTo *fromTo = changes.pfVectorGet(&changes, index);
                            // if(changes.pfVectorGet(&changes, changes.pfVectorTotal(&changes) - 1)) {
                            //     fromTo = changes.pfVectorGet(&changes, changes.pfVectorTotal(&changes) - 1);
                            // } else {
                            //     fromTo = (struct FromTo*) malloc(sizeof(struct FromTo));
                            // }
                            //fromTo = (struct FromTo*) malloc(sizeof(struct FromTo));
                            if(fromTo == 0) {
                                index = 0;
                            }

                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            fromTo->toCellX = neighbours[randomNeighbour].toCellX;
                            fromTo->toCellY = neighbours[randomNeighbour].toCellY;
                            fromTo->resultId = neighbours[randomNeighbour].resultId;

                            //changes.vectorSet(&changes, x + y * (MAX_CELLS_X), fromTo);

                            // 
                            // //cells[neighbours[randomNeighbour]][neighbours[randomNeighbour + 1]] = cellIndex;
                            // finalCells[neighbours[randomNeighbour]][neighbours[randomNeighbour + 1]] = cellIndex;
                            // cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex;
                            // finalCells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex;
                            break;
                        }
                    }
                }
            }
        }
    }
    cells_performance_timer = (clock() - start_clk) / (long double) CLOCKS_PER_SEC;
    if(changes.pfVectorTotal(&changes) > 0) {
        //changes.pfVectorSort(&changes);

        int iPrev = 0;
        // struct FromTo *fromo;
        // fromo = (struct FromTo*) malloc(sizeof(struct FromTo));
        // fromo->cellX = -1;
        // fromo->cellY = -1;
        // fromo->toCellX = -1;
        // fromo->toCellY = -1;
        // fromo->resultId = -1;

        // changes.pfVectorAdd(&changes, fromo);

        // struct FromTo *fromToto = changes.pfVectorGet(&changes, 0);

        
        for(int i = 0; i < changes.pfVectorTotal(&changes) - 1; i++) {

            //int random = iPrev + GetRandomValue(0, i - iPrev);

            struct FromTo *fromTo = changes.pfVectorGet(&changes, i);

            if(fromTo != 0) {
                // TEDO I'd prefer to have only the active changes in memory, but when freeing A LOT of them, it slows down, this is the best solution, keeping all possible changes in memory and only executing the active ones.
                if(fromTo->cellX >= 0 && fromTo->cellX <= MAX_CELLS_X - 1) { 
                    int destX = fromTo->toCellX;
                    int destY = fromTo->toCellY;
                    int srcX = fromTo->cellX;
                    int srcY = fromTo->cellY;
                    
                    int otherCell = cells[destX] [destY];
                    cells[destX] [destY] = cells[srcX] [srcY];
                    cells[srcX] [srcY] = cellTypes[otherCell].immovable == false ? otherCell : fromTo->resultId;
                    fromTo->cellX = -1;
                    //iPrev = i + 1;  
                }
                
            }



            // int random = iPrev + GetRandomValue(0, i - iPrev);

            // struct FromTo *fromTo = changes.pfVectorGet(&changes, random);

            // if(fromTo != 0) {
            //     if(fromTo->cellX >= 0 && fromTo->cellX <= MAX_CELLS_X - 1) {
            //         int destX = fromTo->toCellX;
            //         int destY = fromTo->toCellY;
            //         int srcX = fromTo->cellX;
            //         int srcY = fromTo->cellY;
                    
            //         int otherCell = cells[destX] [destY];
            //         cells[destX] [destY] = cells[srcX] [srcY];
            //         cells[srcX] [srcY] = cellTypes[otherCell].immovable == false ? otherCell : fromTo->resultId;

            //         iPrev = i + 1;  
            //     }
                
            // }
            
            
            //free(changes.pfVectorGet(&changes, random));
            // if(changes.pfVectorGet(&changes, i) != 0 && changes.pfVectorGet(&changes, i + 1) != 0 && changes.pfVectorGet(&changes, i + 1)->toCellX != changes.pfVectorGet(&changes, i)->cellX &&
            //     changes.pfVectorGet(&changes, i + 1)->toCellY != changes.pfVectorGet(&changes, i)->toCellY) {
            //         int random = iPrev + GetRandomValue(0, i - iPrev);

            //         int destX = changes.pfVectorGet(&changes, random)->toCellX;
            //         int destY = changes.pfVectorGet(&changes, random)->toCellY;
            //         int srcX = changes.pfVectorGet(&changes, random)->cellX;
            //         int srcY = changes.pfVectorGet(&changes, random)->cellY;
                    
            //         int otherCell = cells[destX] [destY];
            //         cells[destX] [destY] = cells[srcX] [srcY];
            //         cells[srcX] [srcY] = cellTypes[otherCell].immovable == false ? otherCell : changes.pfVectorGet(&changes, random)->resultId;

            //         iPrev = i + 1;
            // }
        }
        pre_changes_performance_timer = (clock() - start_clk) / (long double) CLOCKS_PER_SEC;            
        //changes.pfVectorFree(&changes);
    }
    changes_performance_timer = (clock() - start_clk) / (long double) CLOCKS_PER_SEC;
    
}

// neighbours
// 0 - moore
// 1 - von Neumann
// 2 - vertical
// 3 - horizontal
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