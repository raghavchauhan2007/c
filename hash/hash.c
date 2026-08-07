#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *key;
    int val;
    struct Node *next; 
} Node;

typedef struct {
    int n;
    int N;
    Node **buckets;
} HashMap;

HashMap* initHashMap(int initial_N) {
    HashMap* map = malloc(sizeof(HashMap));
    if (!map) {
        exit(1);
    }

    map->N = initial_N;
    map->n = 0;

    map->buckets = calloc(map->N, sizeof(Node*));
    if (!map->buckets) {
        free(map);
        exit(1);
    }

    return map;
}

int hash(char* key, HashMap *map) {
    int sum = 0;
    for (char *ptr = key; *ptr; ++ptr) {
        sum += *ptr;
    }

    return abs(sum) % map->N;
}

Node* search(HashMap *map, char *key) {
    int index = hash(key, map);

    Node *curr = map->buckets[index];

    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

void rehash(HashMap *map) {
    Node **oldBuckets = map->buckets;
    int old_N = map->N;

    map->N *= 2;
    map->n = 0;

    map->buckets = calloc(map->N, sizeof(Node*));
    if (!map->buckets) {
        exit(1);
    }

    for (int i = 0; i < old_N; ++i) {
        Node *curr = oldBuckets[i];

        while (curr) {
            Node *next = curr->next;

            int index = hash(curr->key, map);

            curr->next = map->buckets[index];
            map->buckets[index] = curr;

            map->n++;

            curr = next;
        }
    }

    free(oldBuckets);
}

void insert(HashMap *map, char *key, int val) {
    Node *newNode = search(map, key);
    if (newNode) {
        newNode->val = val;
        return;
    }

    int index = hash(key, map);

    newNode = malloc(sizeof(Node));
    if (!newNode) {
        exit(1);
    }

    newNode->val = val;
    newNode->key = strdup(key);
    newNode->next = map->buckets[index];
    map->buckets[index] = newNode;
    map->n++;

    if (((double) map->n / (double) map->N) > 0.75) {
        rehash(map);
    }
}

int delete(HashMap *map, char *key) {
    int index = hash(key, map);

    Node *curr = map->buckets[index];

    if (!curr) {
        return INT_MIN;
    }

    if (strcmp(curr->key, key) == 0) {
        map->buckets[index] = curr->next;
        int val = curr->val;
        free(curr->key);
        free(curr);
        map->n--;
        return val;
    }

    while (curr && curr->next) {
        if (strcmp(curr->next->key, key) == 0) {
            int val = curr->next->val;
            Node *tmp = curr->next;
            curr->next = curr->next->next;
            free(tmp->key);
            free(tmp);
            map->n--;
            return val;
        }

        curr = curr->next;
    }

    return INT_MIN;
}

void freeMap(HashMap *map) {
    if (!map) {
        return;
    }

    for (int i = 0; i < map->N; ++i) {
        Node *curr = map->buckets[i];

        while (curr) {
            Node *tmp = curr->next;
            free(curr->key);
            free(curr);
            curr = tmp;
        }
    }

    free(map->buckets);
    free(map);
}

void printMap(HashMap *map) {
    if (map) {
        printf("N: %d | n: %d | alpha: %lf\n", map->N, map->n, ((double) map->n / map->N));
        for (int i = 0; i < map->N; ++i) {
            printf("%d: ", i);

            Node *curr = map->buckets[i];

            while (curr) {
                printf("{%s, %d}", curr->key, curr->val);
                if (curr->next) {
                    printf(", ");
                }

                curr = curr->next;
            }

            printf("\n");
        }
    }
}

int main() {
    HashMap *map = initHashMap(1);

    insert(map, "raghav", 15);
    printMap(map);
    printf("\n");
    insert(map, "shreya", 24);
    printMap(map);
    printf("\n");
    insert(map, "shreya", 12);
    printMap(map);
    printf("\n");
    insert(map, "shreenaa", 77);
    printMap(map);
    printf("\n");
    insert(map, "sara", 100);
    printMap(map);
    printf("\n");
    insert(map, "shrauuu", 5);
    printMap(map);
    printf("\n");
    insert(map, "mango", 111);
    printMap(map);
    printf("\n");
    insert(map, "orange", 23);
    printMap(map);
    printf("\n");
    insert(map, "peach", 69);
    printMap(map);
    printf("\n");
    insert(map, "lips", 100000);
    printMap(map);
    printf("\n");

    delete(map, "raghav");
    printMap(map);

    freeMap(map);
    return 0;
}