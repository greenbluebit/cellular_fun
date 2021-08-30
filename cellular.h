#include "classes.h"
#include <string.h>


#define MAX_CELLS_X             360
#define MAX_CELLS_Y             190

#include "vector.h"

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

struct Cell cells[MAX_CELLS_X] [MAX_CELLS_Y];
struct Cell finalCells [MAX_CELLS_X] [MAX_CELLS_Y];

// int cells[MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };
// int finalCells [MAX_CELLS_X] [MAX_CELLS_Y] = { 0 };

int defaultCell = 0;

bool isWrapping = true;

vector changes;
int changeIndexes[MAX_CELLS_X*MAX_CELLS_Y] = {0};

struct FromTo neighbours [8];
int neighbourCounter = 0;

// PERFORMANCE DEBUG
bool isDebug = false;
clock_t start_clk;
double cells_performance_timer = 0;
double changes_performance_timer = 0;
double pre_changes_performance_timer = 0;

void handleFromTo(int x, int y, int neighbourIndex) {
    int index = (MAX_CELLS_X) * y + x;
    
    struct FromTo *fromTo = changes.pfVectorGet(&changes, index);
    // if(changes.pfVectorGet(&changes, changes.pfVectorTotal(&changes) - 1)) {
    //     fromTo = changes.pfVectorGet(&changes, changes.pfVectorTotal(&changes) - 1);
    // } else {
    //     fromTo = (struct FromTo*) malloc(sizeof(struct FromTo));
    // }
    //fromTo = (struct FromTo*) malloc(sizeof(struct FromTo));
    if(fromTo == 0) {
        index = 0; // TEDO Investigate this
    }

    fromTo->cellX = x;
    fromTo->cellY = y;
    fromTo->toCellX = neighbours[neighbourIndex].toCellX;
    fromTo->toCellY = neighbours[neighbourIndex].toCellY;
    fromTo->resultId = neighbours[neighbourIndex].resultId;

}

void findNextNeighbour(struct FromTo *fromTo, int targetCellIndex, int amountX, int amountY) {
    int currentCellX = fromTo->cellX;
    int currentCellY = fromTo->cellY;

    int xDiff = fromTo->cellX - (currentCellX + amountX);
    int yDiff = fromTo->cellY - (currentCellY + amountY);

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

        int currentX = fromTo->cellX + (xIncrease * xModifier);
        int currentY = fromTo->cellY + (yIncrease * yModifier);

        if(currentX > MAX_CELLS_X - 1 && isWrapping) {
            currentX = 0;
        }
        if(currentY > MAX_CELLS_Y - 1 && isWrapping) {
            currentY = 0;
        }

        if(currentX >= 0 && currentX < MAX_CELLS_X && currentY >= 0 && currentY < MAX_CELLS_Y) {
            if(targetCellIndex == -1 || cellTypes[finalCells[currentX][currentY].cellTypeIndex].index == targetCellIndex) {
                currentCellX = currentX;
                currentCellY = currentY;
            } else {
                break;
            }
        } else {
            break;
        }
    }

    fromTo->toCellX = currentCellX;
    fromTo->toCellY = currentCellY;

    
}

void shuffle(int *array, size_t n) {
    if (n > 1) {
        size_t i;
        for(int i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}
// get_closest_particle_from_color


void SetupCells() {

    for(int i = 0; i < MAX_CELLTYPES; i++) {
        cellTypes[i].index = -1;
        cellTypes[i].color = WHITE;
        cellTypes[i].color.r = 204;
        cellTypes[i].color.g = 210;
        cellTypes[i].color.b = 221;
        strcpy(cellTypes[i].name, "Unset");
    }
    // for(int i = 0; i < MAX_CELLS_X * MAX_CELLS_Y; i++) {
    //     changeIndexes[i] = i;
    // }

    //shuffle(changeIndexes, MAX_CELLS_X * MAX_CELLS_Y);
    // RAYGUI_CLITERAL(Color){ (unsigned char)(204.0f*204),
                                //  (unsigned char)(210.0f*210),
                                //  (unsigned char)(221.0f*221),ddddd
                                //  (unsigned char)(255.0f*(float)255/255.0f) };
    
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
            int cellIndex = cells[x][y].cellTypeIndex;
            neighbourCounter = 0;  
            for(int i = 0; i < MAX_RELATIONSHIPS; i++) {
                bool relationshipFullfilled = false;
                if(cellTypes[cellIndex].targetCellRelationship[i] != 0 && cellTypes[cellIndex].targetCellRelationship[i]->index > -1 && (cellTypes[cellIndex].isImmovable == false || (cellTypes[cellIndex].isImmovable == true && cellTypes[cellIndex].targetCellRelationship[i]->relationshipType == 0 ))) {
                    // bool relationshipFullfilled = false; 
                    int chanceValue = GetRandomValue(0, MAX_CHANCE);
                    int chance = cellTypes[cellIndex].targetCellRelationship[i]->chance == MAX_CHANCE || chanceValue <= cellTypes[cellIndex].targetCellRelationship[i]->chance;
                    if(chance && cellTypes[cellIndex].targetCellRelationship[i]->relationshipType == 0) {
                        
                         if(chance) {
                            int neighbours = 0;

                            if(cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1) {
                                cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            }

                            if(cellTypes[cellIndex].targetCellRelationship[i]->bottomLeft) {
                                int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                                int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : 0;
                                neighbours += (currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->bottom) {
                                int currentCellX = x;
                                int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : ( isWrapping == true ? 0 : y + 1 );
                                neighbours += (currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->bottomRight) {
                                int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                                int currentCellY = y + 1 <= MAX_CELLS_Y - 1 ? y + 1 : ( isWrapping == true ? 0 : y + 1 );
                                neighbours += (currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->right) {
                                int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                                int currentCellY = y;
                                neighbours += (currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->topRight) {
                                int currentCellX = x + 1 <= MAX_CELLS_X - 1 ? x + 1 : ( isWrapping == true ? 0 : x + 1 );
                                int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1);
                                neighbours += (currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->top) {
                                int currentCellX = x;
                                int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1 );
                                neighbours += (currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->topLeft) {
                                int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                                int currentCellY = y - 1 >= 0 ? y - 1 : ( isWrapping == true ? MAX_CELLS_Y - 1 : y - 1 );
                                neighbours += (currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }
                            if(cellTypes[cellIndex].targetCellRelationship[i]->left) {
                                int currentCellX = x - 1 >= 0 ? x - 1 : ( isWrapping == true ? MAX_CELLS_X - 1 : x - 1 );
                                int currentCellY = y;
                                neighbours += (currentCellX >= 0 && currentCellX < MAX_CELLS_X && currentCellY >= 0 && currentCellY < MAX_CELLS_Y && cellTypes[finalCells[currentCellX][currentCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex) ? 1 : 0;
                            }

                            if( strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], "=") == 0)
                            {
                                if(neighbours == cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            } else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], "!=") == 0)
                            {
                                if(neighbours != cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], "<") == 0)
                            {
                                if(neighbours < cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], "<=") == 0)
                            {
                                if(neighbours <= cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], ">") == 0)
                            {
                                if(neighbours > cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], ">=") == 0)
                            {
                                if(neighbours >= cellTypes[cellIndex].targetCellRelationship[i]->amount)
                                {
                                    cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            } else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], ".") == 0) {
                                cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                relationshipFullfilled = true;
                            } else if(strcmp(comparisonTypes[cellTypes[cellIndex].targetCellRelationship[i]->comparisonType], ">=<=") == 0) {
                                if(neighbours >= cellTypes[cellIndex].targetCellRelationship[i]->amount && neighbours <= cellTypes[cellIndex].targetCellRelationship[i]->toAmount) {
                                    cells[x][y].cellTypeIndex = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex;
                                    relationshipFullfilled = true;
                                }
                            }

                            
                        }
                    } else if(chance && cellTypes[cellIndex].targetCellRelationship[i]->relationshipType == 1 && cellTypes[cellIndex].isImmovable == false) {
                              

                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottomLeft) {
                            struct FromTo *fromTo = malloc(sizeof(struct FromTo));
                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            //int targetCellIndex, int amountX, int amountY)
                            findNextNeighbour(fromTo, cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex
                            , -cellTypes[cellIndex].targetCellRelationship[i]->amount
                            , cellTypes[cellIndex].targetCellRelationship[i]->amount);

                            //TEDO can remove the check fortargetCellTypeIndex here
                            if(fromTo->toCellX >= 0 && fromTo->toCellX < MAX_CELLS_X) {
                                if(fromTo->toCellY >= 0 && fromTo->toCellY < MAX_CELLS_Y) {
                                    if(fromTo->toCellY != y && fromTo->toCellX != x
                                    && (cellTypes[finalCells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                        || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                        neighbours[neighbourCounter].cellX = x;
                                        neighbours[neighbourCounter].cellY = y;
                                        neighbours[neighbourCounter].toCellX = fromTo->toCellX;
                                        neighbours[neighbourCounter].toCellY = fromTo->toCellY;
                                        neighbours[neighbourCounter].resultId = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex; //cells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex; // TEDO Chance this to use result cell id

                                        if(cells[fromTo->cellX][fromTo->cellY].velX < 0 && cells[fromTo->cellX][fromTo->cellY].velY > 0 && (cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true || neighbourCounter == 0)) {//&& neighbourCounter == 0) {
                                            handleFromTo(x, y, neighbourCounter);
                                            neighbourCounter = -1;
                                            free(fromTo);
                                            break;
                                        }
                                        neighbourCounter++;
                                    }
                                }
                            }
                            free(fromTo);
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottom) {
                            struct FromTo *fromTo = malloc(sizeof(struct FromTo));
                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            //int targetCellIndex, int amountX, int amountY)
                            findNextNeighbour(fromTo, cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex
                            , 0
                            , cellTypes[cellIndex].targetCellRelationship[i]->amount);

                            //TEDO can remove the check fortargetCellTypeIndex here
                            if(fromTo->toCellX >= 0 && fromTo->toCellX < MAX_CELLS_X) {
                                if(fromTo->toCellY >= 0 && fromTo->toCellY < MAX_CELLS_Y) {
                                    if(fromTo->toCellY != y
                                    && (cellTypes[finalCells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                        || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                        neighbours[neighbourCounter].cellX = x;
                                        neighbours[neighbourCounter].cellY = y;
                                        neighbours[neighbourCounter].toCellX = fromTo->toCellX;
                                        neighbours[neighbourCounter].toCellY = fromTo->toCellY;
                                        neighbours[neighbourCounter].resultId = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex; // TEDO Chance this to use result cell id
                                        
                                        if(cells[fromTo->cellX][fromTo->cellY].velX == 0 && cells[fromTo->cellX][fromTo->cellY].velY > 0 && (cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true || neighbourCounter == 0)) {//&& neighbourCounter == 0) {
                                            handleFromTo(x, y, neighbourCounter);
                                            neighbourCounter = -1;
                                            free(fromTo);
                                            break;
                                        }
                                        neighbourCounter++;
                                    }
                                }
                            }
                            free(fromTo);
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->bottomRight) {
                            struct FromTo *fromTo = malloc(sizeof(struct FromTo));
                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            //int targetCellIndex, int amountX, int amountY)
                            findNextNeighbour(fromTo
                            , cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex
                            , cellTypes[cellIndex].targetCellRelationship[i]->amount
                            , cellTypes[cellIndex].targetCellRelationship[i]->amount);

                            //TEDO can remove the check fortargetCellTypeIndex here
                            if(fromTo->toCellX >= 0 && fromTo->toCellX < MAX_CELLS_X) {
                                if(fromTo->toCellY >= 0 && fromTo->toCellY < MAX_CELLS_Y) {
                                    if(fromTo->toCellY != y && fromTo->toCellX != x
                                    && (cellTypes[finalCells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                        || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                        neighbours[neighbourCounter].cellX = x;
                                        neighbours[neighbourCounter].cellY = y;
                                        neighbours[neighbourCounter].toCellX = fromTo->toCellX;
                                        neighbours[neighbourCounter].toCellY = fromTo->toCellY;
                                        neighbours[neighbourCounter].resultId = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex; // TEDO Chance this to use result cell id

                                        if(cells[fromTo->cellX][fromTo->cellY].velX > 0 && cells[fromTo->cellX][fromTo->cellY].velY > 0 && (cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true || neighbourCounter == 0)) {//&& neighbourCounter == 0) {
                                            handleFromTo(x, y, neighbourCounter);
                                            neighbourCounter = -1;
                                            free(fromTo);
                                            break;
                                        }
                                        neighbourCounter++;
                                    }
                                }
                            }

                            free(fromTo);
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->right) {
                            struct FromTo *fromTo = malloc(sizeof(struct FromTo));
                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            //int targetCellIndex, int amountX, int amountY)
                            findNextNeighbour(fromTo, cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex
                            , cellTypes[cellIndex].targetCellRelationship[i]->amount
                            , 0);

                            //TEDO can remove the check fortargetCellTypeIndex here
                            if(fromTo->toCellX >= 0 && fromTo->toCellX < MAX_CELLS_X) {
                                if(fromTo->toCellY >= 0 && fromTo->toCellY < MAX_CELLS_Y) {
                                    if(fromTo->toCellX != x
                                    && (cellTypes[finalCells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                        || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                        neighbours[neighbourCounter].cellX = x;
                                        neighbours[neighbourCounter].cellY = y;
                                        neighbours[neighbourCounter].toCellX = fromTo->toCellX;
                                        neighbours[neighbourCounter].toCellY = fromTo->toCellY;
                                        neighbours[neighbourCounter].resultId = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex; // TEDO Chance this to use result cell id

                                        if(cells[fromTo->cellX][fromTo->cellY].velX > 0 && cells[fromTo->cellX][fromTo->cellY].velY == 0 && (cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true || neighbourCounter == 0)) {//&& neighbourCounter == 0) {
                                            handleFromTo(x, y, neighbourCounter);
                                            neighbourCounter = -1;
                                            free(fromTo);
                                            break;
                                        }
                                        neighbourCounter++;
                                    }
                                }
                            }
                            free(fromTo);
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->topRight) {
                            struct FromTo *fromTo = malloc(sizeof(struct FromTo));
                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            //int targetCellIndex, int amountX, int amountY)
                            findNextNeighbour(fromTo
                            , cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex
                            , cellTypes[cellIndex].targetCellRelationship[i]->amount
                            , -cellTypes[cellIndex].targetCellRelationship[i]->amount);

                            //TEDO can remove the check fortargetCellTypeIndex here
                            if(fromTo->toCellX >= 0 && fromTo->toCellX < MAX_CELLS_X) {
                                if(fromTo->toCellY >= 0 && fromTo->toCellY < MAX_CELLS_Y) {
                                    if(fromTo->toCellY != y && fromTo->toCellX != x
                                    && (cellTypes[finalCells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                        || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                        neighbours[neighbourCounter].cellX = x;
                                        neighbours[neighbourCounter].cellY = y;
                                        neighbours[neighbourCounter].toCellX = fromTo->toCellX;
                                        neighbours[neighbourCounter].toCellY = fromTo->toCellY;
                                        neighbours[neighbourCounter].resultId = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex; // TEDO Chance this to use result cell id

                                        if(cells[fromTo->cellX][fromTo->cellY].velX > 0 && cells[fromTo->cellX][fromTo->cellY].velY < 0 && (cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true || neighbourCounter == 0)) {//&& neighbourCounter == 0) {
                                            handleFromTo(x, y, neighbourCounter);
                                            neighbourCounter = -1;
                                            free(fromTo);
                                            break;
                                        }
                                        neighbourCounter++;
                                    }
                                }
                            }
                            free(fromTo);
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->top) {
                            struct FromTo *fromTo = malloc(sizeof(struct FromTo));
                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            //int targetCellIndex, int amountX, int amountY)
                            findNextNeighbour(fromTo
                            , cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex
                            , 0
                            , -cellTypes[cellIndex].targetCellRelationship[i]->amount);

                            //TEDO can remove the check fortargetCellTypeIndex here
                            if(fromTo->toCellX >= 0 && fromTo->toCellX < MAX_CELLS_X) {
                                if(fromTo->toCellY >= 0 && fromTo->toCellY < MAX_CELLS_Y) {
                                    if(fromTo->toCellY != y
                                    && (cellTypes[finalCells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                        || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                        neighbours[neighbourCounter].cellX = x;
                                        neighbours[neighbourCounter].cellY = y;
                                        neighbours[neighbourCounter].toCellX = fromTo->toCellX;
                                        neighbours[neighbourCounter].toCellY = fromTo->toCellY;
                                        neighbours[neighbourCounter].resultId = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex; // TEDO Chance this to use result cell id

                                        if(cells[fromTo->cellX][fromTo->cellY].velX == 0 && cells[fromTo->cellX][fromTo->cellY].velY < 0 && (cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true || neighbourCounter == 0)) {//&& neighbourCounter == 0) {
                                            handleFromTo(x, y, neighbourCounter);
                                            neighbourCounter = -1;
                                            free(fromTo);
                                            break;
                                        }
                                        neighbourCounter++;
                                    }
                                }
                            }
                            free(fromTo);
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->topLeft) {
                            struct FromTo *fromTo = malloc(sizeof(struct FromTo));
                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            //int targetCellIndex, int amountX, int amountY)
                            findNextNeighbour(fromTo
                            , cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex
                            , -cellTypes[cellIndex].targetCellRelationship[i]->amount
                            , -cellTypes[cellIndex].targetCellRelationship[i]->amount);

                            //TEDO can remove the check fortargetCellTypeIndex here
                            if(fromTo->toCellX >= 0 && fromTo->toCellX < MAX_CELLS_X) {
                                if(fromTo->toCellY >= 0 && fromTo->toCellY < MAX_CELLS_Y) {
                                    if(fromTo->toCellY != y && fromTo->toCellX != x
                                    && (cellTypes[finalCells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                        || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                        neighbours[neighbourCounter].cellX = x;
                                        neighbours[neighbourCounter].cellY = y;
                                        neighbours[neighbourCounter].toCellX = fromTo->toCellX;
                                        neighbours[neighbourCounter].toCellY = fromTo->toCellY;
                                        neighbours[neighbourCounter].resultId = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex; // TEDO Chance this to use result cell id

                                        if(cells[fromTo->cellX][fromTo->cellY].velX < 0 && cells[fromTo->cellX][fromTo->cellY].velY < 0 && (cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true || neighbourCounter == 0)) {//&& neighbourCounter == 0) {
                                            handleFromTo(x, y, neighbourCounter);
                                            neighbourCounter = -1;
                                            free(fromTo);
                                            break;
                                        }
                                        neighbourCounter++;
                                    }
                                }
                            }
                            free(fromTo);
                        }
                        if(cellTypes[cellIndex].targetCellRelationship[i]->left) {
                            struct FromTo *fromTo = malloc(sizeof(struct FromTo));
                            fromTo->cellX = x;
                            fromTo->cellY = y;
                            //int targetCellIndex, int amountX, int amountY)
                            findNextNeighbour(fromTo
                            , cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex
                            , -cellTypes[cellIndex].targetCellRelationship[i]->amount
                            , 0);

                            //TEDO can remove the check fortargetCellTypeIndex here
                            if(fromTo->toCellX >= 0 && fromTo->toCellX < MAX_CELLS_X) {
                                if(fromTo->toCellY >= 0 && fromTo->toCellY < MAX_CELLS_Y) {
                                    if(fromTo->toCellX != x
                                    && (cellTypes[finalCells[fromTo->toCellX][fromTo->toCellY].cellTypeIndex].index == cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex 
                                        || cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex == -1)) {
                                        neighbours[neighbourCounter].cellX = x;
                                        neighbours[neighbourCounter].cellY = y;
                                        neighbours[neighbourCounter].toCellX = fromTo->toCellX;
                                        neighbours[neighbourCounter].toCellY = fromTo->toCellY;
                                        neighbours[neighbourCounter].resultId = cellTypes[cellIndex].targetCellRelationship[i]->resultCellTypeIndex; // TEDO Chance this to use result cell id

                                        // I might to change cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true to use finalCells
                                        if(cells[fromTo->cellX][fromTo->cellY].velX < 0 && cells[fromTo->cellX][fromTo->cellY].velY == 0 && (cellTypes[cells[x][y].cellTypeIndex].isMaintainingVelocity == true || neighbourCounter == 0)) {//&& neighbourCounter == 0) {
                                            handleFromTo(x, y, neighbourCounter);
                                            neighbourCounter = -1;
                                            free(fromTo);
                                            break;
                                        }
                                        neighbourCounter++;
                                    }
                                }
                            }
                            free(fromTo);
                        }
                        if(neighbourCounter > 0) {
                            relationshipFullfilled = true;
                        }
                    }
                }
                if(relationshipFullfilled == true) {
                    //changes.pfVectorFree(&changes);
                    break;
                }
            }
            if(neighbourCounter > 0) {
                //neighbourCounter /= 2;
                int randomNeighbour = GetRandomValue(0, neighbourCounter > 0 ? neighbourCounter - 1 : 0);
                int index = (MAX_CELLS_X) * y + x;
                if(index == 249999) {
                    index = index;
                }
                // struct FromTo *fromTo = changes.pfVectorGet(&changes, index);
                // // if(changes.pfVectorGet(&changes, changes.pfVectorTotal(&changes) - 1)) {
                // //     fromTo = changes.pfVectorGet(&changes, changes.pfVectorTotal(&changes) - 1);
                // // } else {
                // //     fromTo = (struct FromTo*) malloc(sizeof(struct FromTo));
                // // }
                // //fromTo = (struct FromTo*) malloc(sizeof(struct FromTo));
                // if(fromTo == 0) {
                //     index = 0;
                // }

                // fromTo->cellX = x;
                // fromTo->cellY = y;
                // fromTo->toCellX = neighbours[randomNeighbour].toCellX;
                // fromTo->toCellY = neighbours[randomNeighbour].toCellY;
                // fromTo->resultId = neighbours[randomNeighbour].resultId;


                handleFromTo(x, y, randomNeighbour);
                neighbourCounter = 0;

                //changes.vectorSet(&changes, x + y * (MAX_CELLS_X), fromTo);

                // 
                // //cells[neighbours[randomNeighbour]][neighbours[randomNeighbour + 1]] = cellIndex;
                // finalCells[neighbours[randomNeighbour]][neighbours[randomNeighbour + 1]] = cellIndex;
                // cells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex;
                // finalCells[x][y] = cellTypes[cellIndex].targetCellRelationship[i]->targetCellTypeIndex;
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

        //shuffle(changeIndexes, MAX_CELLS_X * MAX_CELLS_Y);
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
                    Vector2 direction = (Vector2) {fromTo->toCellX - fromTo->cellX, fromTo->toCellY - fromTo->cellY};
                    
                    cells[srcX][srcY].velX = direction.x < 0 ? -1 : direction.x > 0 ? 1 : 0;
                    cells[srcX][srcY].velY = direction.y < 0 ? -1 : direction.y > 0 ? 1 : 0;
                    int otherCell = cells[destX] [destY].cellTypeIndex;
                    int velX = cells[destX][destY].velX;
                    int velY = cells[destX][destY].velY;

                    cells[destX] [destY].cellTypeIndex = cells[srcX] [srcY].cellTypeIndex;
                    cells[destX] [destY].velX = cells[srcX] [srcY].velX;
                    cells[destX] [destY].velY = cells[srcX] [srcY].velY;
                    cells[srcX] [srcY].cellTypeIndex = cellTypes[otherCell].isImmovable == false ? otherCell : fromTo->resultId;
                    cells[srcX] [srcY].velX = 0;
                    cells[srcX] [srcY].velY = 0;
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