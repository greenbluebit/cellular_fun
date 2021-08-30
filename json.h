#include "cJSON/cJSON.h"
#include "cJSON/cJSON.c"

int ParseJson(char *string) {
    int status = 0;
    cJSON *parent = cJSON_Parse(string);
    cJSON *jsonDefaultCell = NULL;
    cJSON *jsonIsImmovable = NULL;
    cJSON *jsonIsMaintainingVelocity = NULL;
    cJSON *jsonCellTypes = NULL;
    cJSON *jsonCellType = NULL;
    cJSON *jsonIndex = NULL;
    cJSON *jsonNeighbourType = NULL;
    cJSON *jsonColor = NULL;
    cJSON *jsonR = NULL;
    cJSON *jsonG = NULL;
    cJSON *jsonB = NULL;
    cJSON *jsonA = NULL;
    cJSON *jsonName = NULL;
    cJSON *jsonTargetRelationships = NULL;
    cJSON *jsonTargetRelationship = NULL;
    cJSON *jsonChance = NULL;
    cJSON *jsonRelationshipType = NULL;
    cJSON *bottomLeft = NULL;
    cJSON *bottom = NULL;
    cJSON *bottomRight = NULL;
    cJSON *right = NULL;
    cJSON *topRight = NULL;
    cJSON *top = NULL;
    cJSON *topLeft = NULL;
    cJSON *left = NULL;
    cJSON *jsonTargetIndex = NULL;
    cJSON *jsonComparisonType = NULL;
    cJSON *jsonAmount = NULL;
    cJSON *jsonToAmount = NULL;
    cJSON *jsonRelationshipIndex = NULL;
    cJSON *jsonResultIndex = NULL;

    if(parent == 0) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 1;
        goto end;
    }

    
    jsonDefaultCell = cJSON_GetObjectItemCaseSensitive(parent, "defaultcell");
    if(jsonDefaultCell != 0) {
        defaultCell = jsonDefaultCell->valueint;
        selectedCellType = defaultCell;
        for(int x = 0; x < MAX_CELLS_X; x++) {
            for(int y = 0; y < MAX_CELLS_Y; y++) {
                finalCells[x][y].cellTypeIndex = defaultCell;
                cells[x][y].cellTypeIndex = defaultCell;
            }
        }
    }
    
    

    for(int i = 0; i < MAX_CELLTYPES; i++) {
        cellTypes[i].index = -1;
        for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
            cellTypes[i].targetCellRelationship[x] = NULL;
        }
    }

    jsonCellTypes = cJSON_GetObjectItemCaseSensitive(parent, "celltypes");
    // MAYBE
    // TEDO I need to change it so that instead of having relationship array or cellType array uninitiated, to have them all with no indexes
    //int cellTypeIndex = 0;
    cJSON_ArrayForEach(jsonCellType, jsonCellTypes) {
        jsonIndex = cJSON_GetObjectItemCaseSensitive(jsonCellType, "index");
        jsonNeighbourType = cJSON_GetObjectItemCaseSensitive(jsonCellType, "neighbourtype");
        jsonColor = cJSON_GetObjectItemCaseSensitive(jsonCellType, "color");
        jsonIsImmovable = cJSON_GetObjectItemCaseSensitive(jsonCellType, "isimmovable");
        if(jsonIsImmovable != 0) {
            cellTypes[jsonIndex->valueint].isImmovable = jsonIsImmovable->valueint;
        } else {
            cellTypes[jsonIndex->valueint].isImmovable = 0; // TEDO I might not need this
        }
        jsonIsMaintainingVelocity = cJSON_GetObjectItemCaseSensitive(jsonCellType, "ismaintainingvelocity");
        if(jsonIsMaintainingVelocity != 0) {
            cellTypes[jsonIndex->valueint].isMaintainingVelocity = jsonIsMaintainingVelocity->valueint;
        } else {
            cellTypes[jsonIndex->valueint].isMaintainingVelocity = 0;
        }

        jsonIsMaintainingVelocity = NULL;

        if(jsonColor == 0) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                fprintf(stderr, "Error before: %s\n", error_ptr);
            }
            status = 2;
            goto end;
        }

        jsonR = cJSON_GetObjectItemCaseSensitive(jsonColor, "r");
        jsonG = cJSON_GetObjectItemCaseSensitive(jsonColor, "g");
        jsonB = cJSON_GetObjectItemCaseSensitive(jsonColor, "b");
        jsonA = cJSON_GetObjectItemCaseSensitive(jsonColor, "a");

        jsonName = cJSON_GetObjectItemCaseSensitive(jsonCellType, "name");

        cellTypes[jsonIndex->valueint].index = jsonIndex->valueint;
        cellTypes[jsonIndex->valueint].color = CLITERAL(Color){ jsonR->valueint, jsonG->valueint, jsonB->valueint, jsonA->valueint };
        
        for(int x = 0; x < MAX_NAME_LENGTH; x++) {
            if(cellTypes[jsonIndex->valueint].name[x] != '\0') {
                cellTypes[jsonIndex->valueint].name[x] = '\0';
            } else {
                break;
            }
        }
        strcpy(cellTypes[jsonIndex->valueint].name, jsonName->valuestring);



        jsonTargetRelationships = cJSON_GetObjectItemCaseSensitive(jsonCellType, "targetcellrelationships");

        cJSON_ArrayForEach(jsonTargetRelationship, jsonTargetRelationships) {
            jsonTargetIndex = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "targetindex");
            jsonComparisonType = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "relationshiptype");
            if(jsonComparisonType == 0) {
                jsonComparisonType = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "comparisontype");
            }
            
            jsonAmount = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "amount");
            jsonToAmount = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "toamount");
            jsonRelationshipIndex = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "index");
            jsonResultIndex = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "resultindex");
            jsonChance = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "chance");
            jsonRelationshipType = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "relationshiptype_");
            
            if(jsonRelationshipIndex->valueint > -1) {
                if(cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint] == 0) {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint] = malloc(sizeof(T_TargetCellRelationship));
                }
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->amount = jsonAmount->valueint;
                if(jsonToAmount != 0) {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->toAmount = jsonToAmount->valueint;
                } else {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->toAmount = 0;
                }
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->index = jsonIndex->valueint;
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->comparisonType = jsonComparisonType->valueint;
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->resultCellTypeIndex = jsonResultIndex->valueint;
                cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->targetCellTypeIndex = jsonTargetIndex->valueint;
                if(jsonChance != 0) {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->chance = jsonChance->valueint;
                } else {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->chance = MAX_CHANCE;
                }
                
                if(jsonRelationshipType != 0) {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->relationshipType = jsonRelationshipType->valueint;
                } else {
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->relationshipType = 0;
                }
                
                
                if(jsonNeighbourType != 0) {
                    if(jsonNeighbourType->valueint == 0) {
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = 1;
                    } else if(jsonNeighbourType->valueint == 1) {
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = 1;
                    } else if(jsonNeighbourType->valueint == 2) {
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = 0;
                    } else if(jsonNeighbourType->valueint == 3) {
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = 1;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = 0;
                        cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = 1;
                    }
                    
                } else {
                    bottomLeft = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "bottomleft");
                    bottom = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "bottom");
                    bottomRight = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "bottomright");
                    right = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "right");
                    topRight = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "topright");
                    top = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "top");
                    topLeft = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "topleft");
                    left = cJSON_GetObjectItemCaseSensitive(jsonTargetRelationship, "left");

                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomLeft = bottomLeft->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottom = bottom->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->bottomRight = bottomRight->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->right = right->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topRight = topRight->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->top = top->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->topLeft = topLeft->valueint;
                    cellTypes[jsonIndex->valueint].targetCellRelationship[jsonRelationshipIndex->valueint]->left = left->valueint;
                }
            }
            
        }


        //cellTypeIndex++;
    }

    end:
        cJSON_Delete(parent);
        return status;
}

char *PrintJson() {
    // TEDO I can reuse variables in next version
    char *string = NULL;
    cJSON *jsonDefaultCell = NULL;
    cJSON *jsonCellTypes = NULL;
    cJSON *jsonCellType = NULL;
    cJSON *jsonIsImmovable = NULL;
    cJSON *jsonIsMaintainingVelocity = NULL;
    cJSON *jsonIndex = NULL;
    cJSON *jsonColor = NULL;
    cJSON *jsonR = NULL;
    cJSON *jsonG = NULL;
    cJSON *jsonB = NULL;
    cJSON *jsonA = NULL;
    cJSON *jsonName = NULL;
    cJSON *jsonTargetRelationships = NULL;
    cJSON *jsonChance = NULL;
    cJSON *jsonRelationshipType = NULL;
    cJSON *bottomLeft = NULL;
    cJSON *bottom = NULL;
    cJSON *bottomRight = NULL;
    cJSON *right = NULL;
    cJSON *topRight = NULL;
    cJSON *top = NULL;
    cJSON *topLeft = NULL;
    cJSON *left = NULL;
    cJSON *jsonTargetIndex = NULL;
    cJSON *jsonComparisonType = NULL;
    cJSON *jsonAmount = NULL;
    cJSON *jsonToAmount = NULL;
    cJSON *jsonRelationshipIndex = NULL;
    cJSON *jsonResultIndex = NULL;
    //cJSON *jsonCells = NULL;

    cJSON *parent = cJSON_CreateObject();
    if(parent == 0) {
        goto end;
    }

    jsonDefaultCell = cJSON_CreateNumber(defaultCell);
    cJSON_AddItemToObject(parent, "defaultcell",jsonDefaultCell);

    jsonCellTypes = cJSON_CreateArray();
    cJSON_AddItemToObject(parent, "celltypes", jsonCellTypes);

    for(int i = 0; i < MAX_CELLTYPES; i++) {
        jsonCellType = cJSON_CreateObject();
        if(jsonCellType == 0) { // TEDO I Need to do this check at the rest of the json initializers
            goto end;
        }
        cJSON_AddItemToArray(jsonCellTypes, jsonCellType);

        jsonIsImmovable = cJSON_CreateNumber(cellTypes[i].isImmovable);
        cJSON_AddItemToObject(jsonCellType, "isimmovable", jsonIsImmovable);

        jsonIsMaintainingVelocity = cJSON_CreateNumber(cellTypes[i].isMaintainingVelocity);
        cJSON_AddItemToObject(jsonCellType, "ismaintainingvelocity", jsonIsMaintainingVelocity);

        jsonIndex = cJSON_CreateNumber(cellTypes[i].index);
        cJSON_AddItemToObject(jsonCellType, "index", jsonIndex);

        jsonColor = cJSON_CreateObject();
        cJSON_AddItemToObject(jsonCellType, "color", jsonColor);

        jsonR = cJSON_CreateNumber(cellTypes[i].color.r);
        cJSON_AddItemToObject(jsonColor, "r", jsonR);

        jsonG = cJSON_CreateNumber(cellTypes[i].color.g);
        cJSON_AddItemToObject(jsonColor, "g", jsonG);

        jsonB = cJSON_CreateNumber(cellTypes[i].color.b);
        cJSON_AddItemToObject(jsonColor, "b", jsonB);

        jsonA = cJSON_CreateNumber(cellTypes[i].color.a);
        cJSON_AddItemToObject(jsonColor, "a", jsonA);

        jsonName = cJSON_CreateString(cellTypes[i].name);
        cJSON_AddItemToObject(jsonCellType, "name", jsonName);

        jsonTargetRelationships = cJSON_CreateArray();
        cJSON_AddItemToObject(jsonCellType, "targetcellrelationships", jsonTargetRelationships);

        for(int x = 0; x < MAX_RELATIONSHIPS; x++) {
            if(cellTypes[i].targetCellRelationship[x] != 0) {
                cJSON *jsonTargetRelationship = cJSON_CreateObject();
                cJSON_AddItemToArray(jsonTargetRelationships, jsonTargetRelationship);

                jsonTargetIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->targetCellTypeIndex);
                cJSON_AddItemToObject(jsonTargetRelationship, "targetindex", jsonTargetIndex);

                jsonComparisonType = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->comparisonType);
                cJSON_AddItemToObject(jsonTargetRelationship, "comparisontype", jsonComparisonType);

                jsonChance = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->chance);
                cJSON_AddItemToObject(jsonTargetRelationship, "chance", jsonChance);

                jsonRelationshipType = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->relationshipType);
                cJSON_AddItemToObject(jsonTargetRelationship, "relationshiptype_", jsonRelationshipType);

                jsonAmount = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->amount);
                cJSON_AddItemToObject(jsonTargetRelationship, "amount", jsonAmount);

                jsonToAmount = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->toAmount);
                cJSON_AddItemToObject(jsonTargetRelationship, "toamount", jsonToAmount);

                jsonRelationshipIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->index);
                cJSON_AddItemToObject(jsonTargetRelationship, "index", jsonRelationshipIndex);

                jsonResultIndex = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->resultCellTypeIndex);
                cJSON_AddItemToObject(jsonTargetRelationship, "resultindex", jsonResultIndex);

                bottomLeft = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->bottomLeft);
                cJSON_AddItemToObject(jsonTargetRelationship, "bottomleft", bottomLeft);

                bottom = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->bottom);
                cJSON_AddItemToObject(jsonTargetRelationship, "bottom", bottom);

                bottomRight = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->bottomRight);
                cJSON_AddItemToObject(jsonTargetRelationship, "bottomright", bottomRight);

                right = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->right);
                cJSON_AddItemToObject(jsonTargetRelationship, "right", right);

                topRight = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->topRight);
                cJSON_AddItemToObject(jsonTargetRelationship, "topright", topRight);

                top = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->top);
                cJSON_AddItemToObject(jsonTargetRelationship, "top", top);

                topLeft = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->topLeft);
                cJSON_AddItemToObject(jsonTargetRelationship, "topleft", topLeft);

                left = cJSON_CreateNumber(cellTypes[i].targetCellRelationship[x]->left);
                cJSON_AddItemToObject(jsonTargetRelationship, "left", left);
            }
        }
    }

    string = cJSON_Print(parent);
    if(string == NULL) {
        fprintf(stderr, "Failed to print. \n");
    }

    end:
        cJSON_Delete(parent);
        return string;
}