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
};

struct TargetCellRelationship {
    struct CellType *targetCellType;
    int amount;
    struct CellType *resultCellType;
};