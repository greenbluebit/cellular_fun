#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "utils.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui/src/raygui.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#define GUI_FILE_DIALOG_IMPLEMENTATION
#include "gui_file_dialog.h"

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