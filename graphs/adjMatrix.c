#include <stdlib.h>
typedef struct {
    int n;
    int isDirected;
    int **arr;
} AdjMatrix;

AdjMatrix* initMatrix(int n, int isDirected) {
    AdjMatrix* am = malloc(sizeof(AdjMatrix));
    if (!am) {
        return NULL;
    }

    am->n = n;
    am->isDirected = isDirected;

    am->arr = malloc(n * sizeof(int*));
    if (!am->arr) {
        free(am);
        return NULL;
    }

    for (int i=0; i<n; ++i) {
        am->arr[i] = calloc(n, sizeof(int));
        if (!am->arr[i]) {
            free(am->arr);
            free(am);
            return NULL;
        }
    }

    return am;
}

void setEdge(AdjMatrix *am, int u, int v, int w) {
    if (u >= 0 && u < am->n && v >= 0 && v < am->n) {
        am->arr[u][v] = w;
        if (!am->isDirected) {
            am->arr[v][u] = w;
        }
    }
}

void freeMatrix(AdjMatrix *am) {
    if (am) {
        for (int i = 0; i < am->n; ++i) {
            free(am->arr[i]);            
        }

        free(am->arr);
        free(am);
    }
}