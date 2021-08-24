#define VECTOR_INIT_CAPACITY 6
#define UNDEFINE  -1
#define SUCCESS 0

int vectorCapacity = VECTOR_INIT_CAPACITY;
typedef struct sVectorList
{
    struct FromTo **items;
    int capacity;
    int total;
} sVectorList;
typedef struct sVector vector;
struct sVector
{
    sVectorList vectorList;
    int (*pfVectorTotal)(vector *);
    int (*pfVectorResize)(vector *, int);
    int (*pfVectorAdd)(vector *, struct FromTo *);
    int (*pfVectorSet)(vector *, int, struct FromTo *);
    struct FromTo *(*pfVectorGet)(vector *, int);
    int (*pfVectorDelete)(vector *, int);
    int (*pfVectorFree)(vector *);
    int (*pfVectorSort)(vector *);
};
void vector_init(vector *v);
int compare(const struct FromTo * elem1, const struct FromTo * elem2) {
    if(elem1->toCellX < elem2->toCellX) {
        return 1;
    } else if(elem2->toCellX < elem1->toCellX){
        return -1;
    }
    return 0;
}
void vectorSort(vector *v){
    for(int i = 0; i < v->vectorList.total; ++i) {
        for(int j = i + 1; j < v->vectorList.total; j++) {
            if(v->vectorList.items[i]->toCellX < v->vectorList.items[j]->toCellX &&
                v->vectorList.items[i]->toCellY < v->vectorList.items[j]->toCellY) {
                    struct FromTo *fromTo;
                    fromTo = (struct FromTo*) malloc(sizeof(struct FromTo));
                    fromTo->cellX = v->vectorList.items[i]->cellX;
                    fromTo->cellY = v->vectorList.items[i]->cellY;
                    fromTo->toCellX = v->vectorList.items[i]->toCellX;
                    fromTo->toCellY = v->vectorList.items[i]->toCellY;
                    free(v->vectorList.items[i]);
                    v->vectorList.items[i] = v->vectorList.items[j];
                    
                    v->vectorList.items[j] = fromTo;
            }
        }
    }
}
int vectorTotal(vector *v)
{
    int totalCount = UNDEFINE;
    if(v)
    {
        totalCount = v->vectorList.total;
    }
    return totalCount;
}
int vectorResize(vector *v, int capacity)
{
    int  status = UNDEFINE;
    if(v)
    {
        struct FromTo **items = realloc(v->vectorList.items, sizeof(struct FromTo *) * capacity);
        if (items)
        {
            v->vectorList.items = items;
            v->vectorList.capacity = capacity;
            vectorCapacity = capacity; // TEDO I Might want to remove this
            status = SUCCESS;
        }
    }
    return status;
}
int vectorPushBack(vector *v, struct FromTo *item)
{
    int  status = UNDEFINE;
    if(v)
    {
        if (v->vectorList.capacity == v->vectorList.total)
        {
            status = vectorResize(v, v->vectorList.capacity * 2);
            if(status != UNDEFINE)
            {
                v->vectorList.items[v->vectorList.total++] = item;
            }
        }
        else
        {
            // TEDO I might need to remove this
            if(v->vectorList.items == NULL) {
                vector_init(v);
            }
            v->vectorList.items[v->vectorList.total++] = item;
            status = SUCCESS;
        }
    }
    return status;
}
int vectorSet(vector *v, int index, struct FromTo *item)
{
    int  status = UNDEFINE;
    if(v)
    {
        if ((index >= 0) && (index < v->vectorList.total))
        {
            v->vectorList.items[index] = item;
            status = SUCCESS;
        }
    }
    return status;
}
struct FromTo *vectorGet(vector *v, int index)
{
    struct FromTo *readData = NULL;
    if(v)
    {
        if ((index >= 0) && (index < v->vectorList.total))
        {
            readData = v->vectorList.items[index];
        }
    }
    return readData;
}
int vectorDelete(vector *v, int index)
{
    int  status = UNDEFINE;
    int i = 0;
    if(v)
    {
        if ((index < 0) || (index >= v->vectorList.total))
            return status;
        v->vectorList.items[index] = NULL;
        for (i = index; (i < v->vectorList.total - 1); ++i)
        {
            v->vectorList.items[i] = v->vectorList.items[i + 1];
            v->vectorList.items[i + 1] = NULL;
        }
        v->vectorList.total--;
        if ((v->vectorList.total > 0) && ((v->vectorList.total) == (v->vectorList.capacity / 4)))
        {
            vectorResize(v, v->vectorList.capacity / 2);
        }
        status = SUCCESS;
    }
    return status;
}
int vectorFree(vector *v)
{
    int  status = UNDEFINE;
    if(v)
    {
        for (int i = 0; (i < v->vectorList.total - 1); i++)
        {
            if(v->vectorList.items[i] != 0) {
                free(v->vectorList.items[i]);
            }
            
        }
        free(v->vectorList.items);
        v->vectorList.items = NULL;
        status = SUCCESS;
    }
    return status;
}
void vector_init(vector *v)
{
    v->pfVectorTotal = vectorTotal;
    v->pfVectorResize = vectorResize;
    v->pfVectorAdd = vectorPushBack;
    v->pfVectorSet = vectorSet;
    v->pfVectorGet = vectorGet;
    v->pfVectorFree = vectorFree;
    v->pfVectorSort = vectorSort;
    v->pfVectorDelete = vectorDelete;


    v->vectorList.capacity = vectorCapacity;
    v->vectorList.total = 0;
    v->vectorList.items = malloc(sizeof(struct FromTo *) * v->vectorList.capacity);
}