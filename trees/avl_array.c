#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX 1000

typedef struct Node {
    int key;
    int height;
    int left;
    int right;
} Node;

Node tree[MAX] = {0};
int root = -1;
int count = 0;

int getHeight(int idx) {
    if (idx == -1) {
        return -1;
    }

    return tree[idx].height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

void updateHeight(int idx) {
    tree[idx].height = 1 + max(getHeight(tree[idx].left), getHeight(tree[idx].right));
}

int getBalance(int idx) {
    if (idx == -1) {
        return 0;
    }

    return getHeight(tree[idx].left) - getHeight(tree[idx].right);
}

int createNode(int key) {
    if (count >= MAX) {
        printf("TREE_FULL\n");
        exit(1);
    }

    tree[count].key = key;
    tree[count].height = 0;
    tree[count].left = -1;
    tree[count].right = -1;

    return count++;
}

int rotateRight(int z) {
    int y = tree[z].left;
    int tmp = tree[y].right;
    tree[y].right = z;
    tree[z].left = tmp;

    updateHeight(z);
    updateHeight(y);
    return y;
}

int rotateLeft(int z) {
    int y = tree[z].right;
    int tmp = tree[y].left;
    tree[y].left = z;
    tree[z].right = tmp;

    updateHeight(z);
    updateHeight(y);

    return y;
}

int insert(int root, int key) {
    if (root == -1) {
        return createNode(key);
    }

    if (key < tree[root].key) {
        tree[root].left = insert(tree[root].left, key);
    }

    else if (key > tree[root].key) {
        tree[root].right = insert(tree[root].right, key);
    }

    else {
      printf("Duplicate Key. Ignored.\n");
      return root;
    }

    updateHeight(root);

    int balance = getBalance(root);

    // LL
    if (balance > 1 && key < tree[tree[root].left].key) {
        return rotateRight(root);
    }

    // RR
    if (balance < -1 && key > tree[tree[root].right].key) {
        return rotateLeft(root);
    }

    // LR
    if (balance > 1 && key > tree[tree[root].left].key) {
        tree[root].left = rotateLeft(tree[root].left);
        return rotateRight(root);
    }

    // RL
    if (balance < -1 && key < tree[tree[root].right].key) {
        tree[root].right = rotateRight(tree[root].right);
        return rotateLeft(root);
    }

    return root;
}

void inorder(int root) {
    if (root != -1) {
        inorder(tree[root].left);
        printf("%c ", tree[root].key);
        inorder(root[tree].right);
    }
}

void levelOrder(int root) {
    int *q = calloc(200, sizeof(int));
    int front = 0, rear = 0;

    q[rear++] = root;
    q[rear++] = INT_MIN;
    while (front < rear) {
        int curr = q[front++];

        if (curr != INT_MIN) {
            printf("%c ", tree[curr].key);

            if (tree[curr].left != -1) {
                q[rear++] = tree[curr].left;
            }

            if (tree[curr].right != -1) {
                q[rear++] = tree[curr].right;
            }
        }

        else {
            printf("\n");
            front++;
        }

        if (front < rear) {
            q[rear++] = INT_MIN;
        }
    }

    free(q);
}

int main() {
    // int arr[] = {10,20,30,40,50,60,70,80,90,100};
    int arr[] = {'a', 'z', 'b', 'y', 'c', 'x', 'd', 'w', 'e', 'v', 'f'};
    int n = sizeof(arr)/sizeof(arr[0]);

    for (int i=0; i<n; ++i) {
        root = insert(root, arr[i]);
    }

    levelOrder(root);

    return 0;
}