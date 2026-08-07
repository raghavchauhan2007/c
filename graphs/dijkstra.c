#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define INF INT_MAX

typedef struct Edge {
    int dest;
    int weight;
    struct Edge *next;
} Edge;

typedef struct {
    int NumVertices;
    Edge **edges;
} Graph;

Edge* createEdge(int destination, int weight) {
    Edge *newEdge = malloc(sizeof(Edge));
    if (!newEdge) {
        exit(1);
    }

    newEdge->dest = destination;
    newEdge->weight  = weight;
    newEdge->next = NULL;

    return newEdge;
}

void addEdge(Graph *graph, int source, int destination, int weight) {
    Edge *newEdge = createEdge(destination, weight);
    newEdge->next = graph->edges[source];
    graph->edges[source] = newEdge;

    // if (!graph->edges[source]) {
    //     graph->edges[source] = newEdge;
    //     return;
    // }

    // Edge *curr = graph->edges[source];

    // while (curr->next) {
    //     curr = curr->next;
    // }

    // curr->next = newEdge;
}

Graph* createGraph(int NumVertices) {
    Graph *graph = malloc(sizeof(Graph));
    if (!graph) {
        exit(1);
    }

    graph->NumVertices = NumVertices;
    graph->edges = malloc(graph->NumVertices * sizeof(Edge*));
    if (!graph->edges) {
        free(graph);
        exit(1);
    }

    for (int i = 0; i < NumVertices; ++i) {
        graph->edges[i] = NULL;
    }

    return graph;
}

void freeGraph(Graph *graph) {
    if (graph) {
        if (graph->edges) {
            for (int i = 0; i < graph->NumVertices; ++i) {
                Edge *curr = graph->edges[i];
                while (curr) {
                    Edge *tmp = curr->next;
                    free(curr);
                    curr = tmp;
                }
            }

            free(graph->edges);
        }

        free(graph);
    }
}

typedef struct {
    int distanceFromSource;
    int vertex;
} HeapNode;

typedef struct {
    int count;
    HeapNode *nodes;
} MinHeap;

HeapNode* initHeapNode(int capacity) {
    HeapNode *arr = malloc(capacity * sizeof(HeapNode));
    if (!arr) {
        exit(1);
    }

    for (int i = 0; i < capacity; ++i) {
        arr[i] = (HeapNode) {INF, 0};
    }

    return arr;
}

void freeHeapNode(HeapNode *arr) {
    if (arr) {
        free(arr);
    }
}

MinHeap* initHeap(int capacity) {
    MinHeap *heap = malloc(sizeof(MinHeap));
    if (!heap) {
        exit(1);
    }

    heap->count = 0;
    heap->nodes = malloc(capacity * sizeof(HeapNode));
    if (!heap->nodes) {
        free(heap);
        exit(1);
    }

    return heap;
}

void swapHeapNodes(HeapNode *a, HeapNode *b) {
    HeapNode tmp = *a;
    *a = *b;
    *b = tmp;
}

char isHeapEmpty(MinHeap *heap) {
    return !heap->count;
}

void heapifyDown(MinHeap *heap , int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->count && heap->nodes[left].distanceFromSource < heap->nodes[smallest].distanceFromSource) {
        smallest = left;
    }

    if (right < heap->count && heap->nodes[right].distanceFromSource < heap->nodes[smallest].distanceFromSource) {
        smallest = right;
    }

    if (smallest != idx) {
        swapHeapNodes(&(heap->nodes[smallest]), &(heap->nodes[idx]));
        heapifyDown(heap, smallest);
    }
}

void heapifyUp(MinHeap *heap, int idx) {
    if (idx > 0) {
        int parent = (idx - 1)/2;

        if (heap->nodes[parent].distanceFromSource > heap->nodes[idx].distanceFromSource) {
            swapHeapNodes(&heap->nodes[parent], &heap->nodes[idx]);
            heapifyUp(heap, parent);
        }
    }
}

void push(MinHeap *heap, int distance, int vertex) {
    heap->nodes[heap->count++] = (HeapNode) {distance, vertex};
    heapifyUp(heap, heap->count - 1);
}

HeapNode pop(MinHeap *heap) {
    if (!heap->count) {
        return (HeapNode) {0, 0};
    }

    HeapNode min = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->count - 1];
    heap->count--;
    heapifyDown(heap, 0);
    return min;
}

void freeHeap(MinHeap *heap) {
    if (heap) {
        free(heap->nodes);
    }
}

void djikstra(int source, Graph *graph) {
    HeapNode *arr = initHeapNode(graph->NumVertices);
    MinHeap *heap = initHeap(graph->NumVertices);

    arr[source] = (HeapNode) {};
    push(heap, 0, source);

    while (!isHeapEmpty(heap)) {
        int u = pop(heap).vertex;
        
        Edge *curr = graph->edges[u];
        while (curr) {
            int v = curr->dest;

            if (arr[v].distanceFromSource > arr[u].distanceFromSource + curr->weight) {
                arr[v].distanceFromSource = arr[u].distanceFromSource + curr->weight;
                push(heap, arr[v].distanceFromSource, v);
            }

            curr = curr->next;
        }
    }

    for (int i = 0; i < graph->NumVertices; ++i) {
        if (arr[i].distanceFromSource == INF) {
            printf("INF ");
        }
        else {
            printf("%d ", arr[i].distanceFromSource);
        }
    }

    freeHeapNode(arr);
    freeHeap(heap);
}

int main() {
    int n = 6;
    Graph *graph = createGraph(n);

    addEdge(graph, 0, 1, 2);
    addEdge(graph, 0, 2, 4);
    addEdge(graph, 1, 2, 1);
    addEdge(graph, 1, 3, 7);
    addEdge(graph, 2, 4, 3);
    addEdge(graph, 3, 5, 1);
    addEdge(graph, 4, 3, 2);
    addEdge(graph, 4, 5, 5);

    djikstra(0, graph);

    return 0;
}