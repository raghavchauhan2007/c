#include <stdio.h>
#include <string.h>

#define MAX 11

typedef struct {
    int key;
    char value[50];
    int active;
} Entry;

Entry table[MAX];

int hash(int key) {
    return key % MAX;
}

void insert(int key, char *value) {
    int idx = hash(key);

    table[idx].key = key;
    strcpy(table[idx].value, value);
    table[idx].active = 1;

    printf("inserted key %d at idx %d\n", key, idx);
}

char* search(int key) {
    int idx = hash(key);

    if(table[idx].active && table[idx].key == key) return table[idx].value;

    else return "NOT_FOUND";
}

int main() {
    for(int i=0; i<MAX; i++) {
        table[i].active = 0;
    }

    insert(101, "RAGHAV");
    insert(206, "SHREYA");
    insert(689, "SARA");

    printf("Search for 206: %s\n", search(101));

    return 0;

}
