#pragma once

#include <stdio.h>
#include "raylib.h"

#define MAX_RELATIONSHIPS       10

struct CellType;
struct TargetCellRelationship;

struct CellType {
    int index;
    int neighbourType;
    Color color;
    char *name;
    struct TargetCellRelationship *targetCellRelationship[MAX_RELATIONSHIPS];
} T_CellType;

// relationshipType
// equal
// notequal
// less
// lessequal
// more
// moreequal
// self
struct TargetCellRelationship {
    //struct CellType *targetCellType;
    int targetCellTypeIndex;
    char *relationshipType;
    int amount;
    int index;
    int resultCellTypeIndex;
    //struct CellType *resultCellType;
} T_TargetCellRelationship;