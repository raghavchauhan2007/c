#include <stdlib.h>
typedef struct EdgeNode{
    int destination;
    int weight;
    struct EdgeNode *next;
} EdgeNode;

typedef struct {
    int numVertices;
    EdgeNode **heads;
} AdjList;

AdjList* createAdjList(int n) {
    AdjList *al = malloc(sizeof(AdjList));
    if (!al) {
        return NULL;
    }

    al->numVertices = n;
    al->heads = malloc(al->numVertices * sizeof(EdgeNode*));
    if (!al->heads) {
        free(al);
        return NULL;
    }

    for (int i = 0; i < al->numVertices; ++i) {
        al->heads[i] = NULL;
    }

    return al;
}

