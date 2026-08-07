#include <stdio.h>
#include <stdlib.h>
typedef struct Node {
    int dest;
    struct Node *next;
} Node;

Node* createNode(int v) {
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "ERR");
        exit(EXIT_FAILURE);
    }

    newNode->dest = v;
    newNode->next = NULL;

    return newNode;
}

void addEdge(Node **al, int u, int v) {
    Node *newNode = createNode(v);
    newNode->next = al[u];
    al[u] = newNode;

    newNode = createNode(u);
    newNode->next = al[v];
    al[v] = newNode;
}

void freeGraph(Node **al, int n) {
    if (al) {
        for (int i = 0; i < n; ++i) {
            Node *curr = al[i];

            while (curr) {
                Node *tmp = curr->next;
                free(curr);
                curr = tmp;
            }
        }

        free(al);
    }
}

char dfs(int n, Node **al, int parent, int u, char *visited) {
    visited[u] = 1;

    Node *curr = al[u];

    while (curr) {
        int v = curr->dest;

        if (!visited[v]) {
            if (dfs(n, al, u, v, visited))
                return 1;
        }
        else if (v != parent) {
            return 1;
        }

        curr = curr->next;
    }

    return 0;
}

char dfsRun(int n, Node **al, int parent, int u, char *visited) {
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            return dfs(n, al, -1, i, visited);
        }
    }

    return 0;
}

int main() {
    int n = 5;
    Node **al = calloc(n,  sizeof(Node*));
    if (!al) {
        fprintf(stderr, "ERR");
        exit(EXIT_FAILURE);
    }

    addEdge(al, 0, 1);
    addEdge(al, 1, 2);
    addEdge(al, 1, 3);
    addEdge(al, 2, 3);
    addEdge(al, 2, 4);

    char *visited = calloc(n, sizeof(char));
    if (!visited) {
        fprintf(stderr, "ERR");
        exit(EXIT_FAILURE);
    }

    printf("%d", dfsRun(n, al, -1, 0, visited));

    free(visited);
    freeGraph(al, n);
    return 0;
}