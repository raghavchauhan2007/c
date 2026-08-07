#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    int height;
    struct Node *left;
    struct Node *right;
} Node;

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(Node *node) {
    if (!node) {
        return -1;
    }

    return node->height;
}

Node* createNode(int key) {
    Node *newNode = malloc(sizeof(Node));

    if (!newNode) {
        fprintf(stderr, "ERR_MEM\n");
        exit(1);
    }

    newNode->key = key;
    newNode->height = 0;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

void updateHeight(Node *node) {
    node->height = 1 + max(height(node->left), height(node->right));
}

int getBalance(Node *node) {
    if (!node) {
        return 0;
    }

    return height(node->left) - height(node->right);
}

void printTree(Node *root, int space) {
    if (!root) {
        return;
    }

    space += 8;

    printTree(root->right, space);

    printf("\n");

    for (int i = 8; i < space; ++i) {
        printf(" ");
    }

    printf("%d(h=%d,b=%d)\n",
           root->key,
           root->height,
           getBalance(root));

    printTree(root->left, space);
}

Node* rightRotate(Node *z) {
    printf("\n>>> RIGHT ROTATE on %d <<<\n", z->key);

    Node *y = z->left;
    Node *tmp = y->right;

    y->right = z;
    z->left = tmp;

    updateHeight(z);
    updateHeight(y);

    return y;
}

Node* leftRotate(Node *z) {
    printf("\n>>> LEFT ROTATE on %d <<<\n", z->key);

    Node *y = z->right;
    Node *tmp = y->left;

    y->left = z;
    z->right = tmp;

    updateHeight(z);
    updateHeight(y);

    return y;
}

Node* insert(Node *root, int key) {

    /*
        BST INSERTION
    */

    if (!root) {
        printf("Inserted %d\n", key);
        return createNode(key);
    }

    if (key < root->key) {
        root->left = insert(root->left, key);
    }

    else if (key > root->key) {
        root->right = insert(root->right, key);
    }

    else {
        printf("Duplicate Key Ignored: %d\n", key);
        return root;
    }

    /*
        UPDATE HEIGHT
    */

    updateHeight(root);

    /*
        CHECK BALANCE
    */

    int balance = getBalance(root);

    printf("Node=%d Height=%d Balance=%d After inserting %d\n",
           root->key,
           root->height,
           balance,
           key);

    /*
        ROTATIONS
    */

    // LL
    if (balance > 1 && key < root->left->key) {
        printf("LL CASE DETECTED\n");
        return rightRotate(root);
    }

    // RR
    if (balance < -1 && key > root->right->key) {
        printf("RR CASE DETECTED\n");
        return leftRotate(root);
    }

    // LR
    if (balance > 1 && key > root->left->key) {
        printf("LR CASE DETECTED\n");

        root->left = leftRotate(root->left);

        return rightRotate(root);
    }

    // RL
    if (balance < -1 && key < root->right->key) {
        printf("RL CASE DETECTED\n");

        root->right = rightRotate(root->right);

        return leftRotate(root);
    }

    return root;
}

void inorder(Node *root) {
    if (root) {
        inorder(root->left);
        printf("%d ", root->key);
        inorder(root->right);
    }
}

void freeTree(Node *root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

int main() {

    /*
        SEQUENCE DESIGNED TO TRIGGER:
        - LL
        - RR
        - LR
        - RL
    */

    int arr[] = {
        30,20,10,   // LL
        40,50,      // RR
        25,         // LR
        45          // RL
    };

    int n = sizeof(arr) / sizeof(arr[0]);

    Node *root = NULL;

    for (int i = 0; i < n; ++i) {

        printf("\n");
        printf("========================================\n");
        printf("INSERTING %d\n", arr[i]);
        printf("========================================\n");

        root = insert(root, arr[i]);

        printf("\nCURRENT AVL TREE:\n");

        printTree(root, 0);

        printf("\n");
        printf("----------------------------------------\n");
    }

    printf("\nFINAL INORDER:\n");
    inorder(root);

    printf("\n");

    freeTree(root);

    return 0;
}