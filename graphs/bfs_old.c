#include <alloca.h>
#include <limits.h>
#include <stdlib.h>
#include "stdio.h"

#define WHITE 0
#define GRAY  1
#define BLACK 2
#define INF INT_MAX

typedef struct Node {
    int dest;
    struct Node *next;
} Node;

typedef struct {
    int color;
    int dist;
    int parent;
} BFS_Meta;

void addEdge(Node **adj, int u, int v) {
    Node *newNode = malloc(sizeof(Node));
    newNode->dest = v;
    newNode->next = adj[u];
    adj[u] = newNode;

    newNode = malloc(sizeof(Node));
    newNode->dest = u;
    newNode->next = adj[v];
    adj[v] = newNode;
}

void runBFS(int n, Node **adj, int startNode, BFS_Meta *meta) {
    for (int i = 0; i < n; ++i) {
        meta[i].color = WHITE;
        meta[i].dist = INF;
        meta[i].parent = -1;
    }

    meta[startNode].color = GRAY;
    meta[startNode].dist = 0;
    meta[startNode].parent = -1;

    int *q = malloc(n * sizeof(int));
    int front = 0, rear = 0;

    q[rear++] = startNode;

    while (front < rear) {
        int u = q[front++];
        printf("%d ", u);
        Node *curr = adj[u];

        while (curr) {
            int v = curr->dest;

            if (meta[v].color == WHITE) {
                meta[v].color = GRAY;
                meta[v].dist = meta[u].dist + 1;
                meta[v].parent = u;

                q[rear++] = v;
            }

            curr = curr->next;
        }

        meta[u].color = BLACK;
    }

    free(q);
}

int main() {
    int n = 6;

    Node **al = calloc(n, sizeof(Node*));

    addEdge(al, 0, 1);
    addEdge(al, 0, 2);
    addEdge(al, 1, 3);
    addEdge(al, 2, 4);
    addEdge(al, 3, 4);
    addEdge(al, 3, 5);
    addEdge(al, 4, 5);

    BFS_Meta *meta = malloc(n * sizeof(BFS_Meta));

    int source = 0;

    runBFS(n, al, source, meta);

    free(meta);
    free(al);

    return 0;
}