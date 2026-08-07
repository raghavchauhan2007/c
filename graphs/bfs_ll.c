#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define WHITE 0
#define GRAY  1
#define BLACK 2
#define INF INT_MAX

typedef struct GraphNode {
    int dest;
    struct GraphNode *next;
} GraphNode;

typedef struct QueueNode {
    int vertex;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int count;
} Queue;

typedef struct {
    int countVertices;
    GraphNode **heads;
} AdjList;

typedef struct {
    int color;
    int parent;
    int dist;
} Bfs_Meta;

int isEmptyQueue(Queue *q) {
    return q->front == NULL && q->rear == NULL;
}

Queue* initQueue() {
    Queue *q = malloc(sizeof(Queue));
    if (!q) {
        fprintf(stderr, "ERR_MEM");
        exit(EXIT_FAILURE);
    }

    q->front = NULL;
    q->rear = NULL;
    q->count = 0;

    return q;
}

QueueNode* createQueueNode(int vertex) {
    QueueNode *newQueueNode = malloc(sizeof(QueueNode));
    if (!newQueueNode) {
        fprintf(stderr, "ERR_MEM");
    }

    newQueueNode->vertex = vertex;
    newQueueNode->next = NULL;

    return newQueueNode;
}

void enqueue(Queue *q, int vertex) {
    QueueNode *newQueueNode = createQueueNode(vertex);

    if (isEmptyQueue(q)) {
        q->front = q->rear = newQueueNode;
        return;
    }

    (*(*q).rear).next = newQueueNode;
    (*q).rear = newQueueNode;
}

int dequeue(Queue *q) {
    if (isEmptyQueue(q)) {
        return -1;
    }

    QueueNode *tmp = q->front;
    int vertex = tmp->vertex;
    q->front = q->front->next;

    if (!q->front) {
        q->rear = NULL;
    }
    
    free(tmp);
    return vertex;
}

void freeQueue(Queue *q) {
    if (q) {
        while (!isEmptyQueue(q)) {
            dequeue(q);
        }

        free(q);
    }
}

AdjList* initAdjList(int numVertices) {
    AdjList *al = malloc(sizeof(AdjList));
    if (!al) {
        fprintf(stderr, "ERR_MEM");
        exit(EXIT_FAILURE);
    }

    al->countVertices = numVertices;

    al->heads = malloc(al->countVertices * sizeof(GraphNode*));
    if (!al->heads) {
        fprintf(stderr, "ERR_MEM");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < al->countVertices; ++i) {
        al->heads[i] = NULL;
    }

    return al;
}

GraphNode* createGraphNode(int v) {
    GraphNode *newGraphNode = malloc(sizeof(GraphNode));
    if (!newGraphNode) {
        fprintf(stderr, "ERR_MEM");
        exit(EXIT_FAILURE);
    }

    newGraphNode->dest = v;
    newGraphNode->next = NULL;

    return newGraphNode;
}

void addEdge(AdjList *al, int u, int v) {
    GraphNode *newGraphNode = createGraphNode(v);
    newGraphNode->next = al->heads[u];
    al->heads[u] = newGraphNode;

    newGraphNode = createGraphNode(u);
    newGraphNode->next = al->heads[v];
    al->heads[v] = newGraphNode;
}

void freeGraph(AdjList *al) {
    if (!al) {
        return;
    }

    for (int i = 0; i < al->countVertices; ++i) {

        GraphNode *curr = al->heads[i];

        while (curr) {
            GraphNode *tmp = curr;
            curr = curr->next;
            free(tmp);
        }
    }

    free(al->heads);
    free(al);
}

Bfs_Meta* initMetaData(AdjList *al) {
    Bfs_Meta *meta = malloc(al->countVertices * sizeof(Bfs_Meta));
    if (!meta) {
        fprintf(stderr, "ERR_MEM");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < al->countVertices; ++i) {
        meta[i].color = WHITE;
        meta[i].dist = INF;
        meta[i].parent = -1;
    }

    return meta;
}

void freeMeta(Bfs_Meta *meta) {
    if (meta) {
        free(meta);
    }
}

void runBFS(AdjList *al, int startNode, Bfs_Meta *meta) {
    meta[startNode].color = GRAY;
    meta[startNode].dist = 0;
    meta[startNode].parent = -1;

    Queue *q = initQueue();

    enqueue(q, startNode);

    while (!isEmptyQueue(q)) {
        int u = dequeue(q);

        GraphNode *curr = al->heads[u];
        
        while (curr) {
            int v = curr->dest;

            if (meta[v].color == WHITE) {
                meta[v].color = GRAY;
                meta[v].dist = meta[u].dist + 1;
                meta[v].parent = u;

                enqueue(q, v);
            }

            curr = curr->next;
        }

        meta[u].color = BLACK;
    }

    freeQueue(q);
}

int main() {
    int n = 6;

    AdjList *al = initAdjList(n);

    addEdge(al, 0, 1);
    addEdge(al, 0, 2);
    addEdge(al, 1, 3);
    addEdge(al, 2, 4);
    addEdge(al, 3, 4);
    addEdge(al, 3, 5);
    addEdge(al, 4, 5);

    Bfs_Meta *meta = initMetaData(al);

    int source = 0;

    runBFS(al, source, meta);

    printf("BFS Traversal starting from Vertex %d:\n", source);
    printf("--------------------------------------------\n");
    printf("Target | Distance | Shortest Path\n");
    printf("--------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        printf("%d|%d| ", i, meta[i].dist);

        if (meta[i].dist == INF) {
            printf("Unreachable");
        }

        printf("\n");
    }

    freeMeta(meta);
    freeGraph(al);

    return 0;
}