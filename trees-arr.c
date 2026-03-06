#include <stdio.h>
#define MAX 100

int tree[MAX];
int size = 0;

void insert(int data) {
    if(size >= MAX-1) {
        printf("TREE FULL\n");
        return;
    }

    tree[++size] = data;
}

int leftChild(int idx) {
    if((2*idx) <= size) {
        return tree[2*idx];
    }

    return -1;
}

int rightChild(int idx) {
    if((2*idx)+1 <= size) {
        return tree[(2*idx)+1];
    }

    return -1;
}

int parent(int idx) {
    if(idx > 1) {
        return tree[idx/2];
    }

    return -1;
}

int main() {
    insert(3);
    insert(4);
    insert(6);
    insert(5);
    insert(24);
    insert(23);
    insert(22);

    printf("%d\n", leftChild(3));
    // printf("%d\n", size);

    return 0;
}