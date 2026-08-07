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
        fprintf(stderr, "ERR_MEM");
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
    return height(node->left) - height(node->right);
}

Node* rightRotate(Node *z) {
    Node *y = z->left;
    Node *tmp = y->right;

    y->right = z;
    z->left = tmp;

    updateHeight(z);
    updateHeight(y);

    return y;
}

Node* leftRotate(Node *z) {
    Node *y = z->right;
    Node *tmp = y->left;

    y->left = z;
    z->right = tmp;

    updateHeight(z);
    updateHeight(y);

    return y;
}

Node* insert(Node *root, int key) {
    if (!root) {
        return createNode(key);
    }

    if (key < root->key) {
        root->left = insert(root->left, key);
    }

    else if (key > root->key) {
        root->right = insert(root->right, key);
    }

    else {
        printf("Duplicate Key. Ignored\n");
    }

    updateHeight(root);

    int balance = getBalance(root);

    // LL
    if (balance > 1 && key < root->left->key) {
        return rightRotate(root);
    }

    // RR
    if (balance < -1 && key > root->right->key) {
        return leftRotate(root);
    }

    // LR
    if (balance > 1 && key > root->left->key) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // RL
    if (balance < -1 && key < root->right->key) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

Node* findMin(Node *node) {
    Node *curr = node;

    while (curr->left) {
        curr = curr->left;
    }

    return curr;
}

Node* delete(Node* root, int key) {
    if (!root) {
        return root;
    }

    if (key < root->key) {
        root->left = delete(root->left, key);
    }

    else if (key > root->key) {
        root->right = delete(root->right, key);
    }

    else {
        if (!root->left || !root->right) {
            Node *tmp = root->left ? root->left : root->right;

            if (tmp) {
                *root = *tmp;
            }

            else {
                tmp = root;
                root = NULL;
            }

            free(tmp);
        }

        else {
            Node *min = findMin(root->right);
            root->key = min->key;
            root->right = delete(root->right, min->key);
        }
    }

    if (!root) {
        return root;
    }

    updateHeight(root);

    int balance = getBalance(root);

    // LL
    if (balance > 1 && getBalance(root->left) >= 0) {
        return rightRotate(root);
    }

    // LR
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // RR
    if (balance < -1 && getBalance(root->right) <= 0) {
        return leftRotate(root);
    }

    // RL
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

Node* search(Node *root, int key) {
    if (!root || root->key == key) {
        return root;
    }

    if (key < root->key) {
        return search(root->left, key);
    }

    else {
        return search(root->right, key);
    }
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

void levelOrder(Node *root) {
    Node **q = calloc(20, sizeof(Node*));
    int front = 0, rear = 0;

    q[rear++] = root;
    q[rear++] = NULL;

    while (front < rear) {
        Node *curr = q[front++];

        if (curr) {
            printf("%c ", curr->key);

            if (curr->left) {
                q[rear++] = curr->left;
            }

            if (curr->right) {
                q[rear++] = curr->right;
            }
        }

        else {
            printf("\n");

            if (front < rear) {
                q[rear++] = NULL;
            }
        }
    }

    free(q);
}

int main() {
    Node *root = NULL;

    printf("=====================================\n");
    printf("AVL TREE COMPREHENSIVE TEST SUITE\n");
    printf("=====================================\n\n");

    /*
        -------------------------------------------------
        TEST 1: Basic Insertions
        -------------------------------------------------
    */
    printf("TEST 1: Sequential Insertions\n");

    int arr1[] = {1,2,3,4,5,6,7,8,9,10};
    int n1 = sizeof(arr1)/sizeof(arr1[0]);

    for (int i = 0; i < n1; ++i) {
        root = insert(root, arr1[i]);
    }

    printf("Inorder Traversal:\n");
    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 2: Search Existing Keys
        -------------------------------------------------
    */
    printf("TEST 2: Search Existing Keys\n");

    int searchKeys1[] = {1,5,10};
    int s1 = sizeof(searchKeys1)/sizeof(searchKeys1[0]);

    for (int i = 0; i < s1; ++i) {
        Node *res = search(root, searchKeys1[i]);

        if (res) {
            printf("Found: %d\n", res->key);
        }

        else {
            printf("NOT FOUND\n");
        }
    }

    printf("\n");

    /*
        -------------------------------------------------
        TEST 3: Search Missing Keys
        -------------------------------------------------
    */
    printf("TEST 3: Search Missing Keys\n");

    int searchKeys2[] = {-1,100,42};

    for (int i = 0; i < 3; ++i) {
        Node *res = search(root, searchKeys2[i]);

        if (res) {
            printf("Unexpectedly Found: %d\n", res->key);
        }

        else {
            printf("%d NOT FOUND\n", searchKeys2[i]);
        }
    }

    printf("\n");

    /*
        -------------------------------------------------
        TEST 4: Duplicate Insertions
        -------------------------------------------------
    */
    printf("TEST 4: Duplicate Insertions\n");

    root = insert(root, 5);
    root = insert(root, 7);
    root = insert(root, 10);

    printf("Inorder After Duplicate Attempts:\n");
    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 5: Delete Leaf Nodes
        -------------------------------------------------
    */
    printf("TEST 5: Delete Leaf Nodes\n");

    root = delete(root, 10);
    root = delete(root, 9);

    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 6: Delete Node With One Child
        -------------------------------------------------
    */
    printf("TEST 6: Delete Node With One Child\n");

    root = delete(root, 8);

    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 7: Delete Node With Two Children
        -------------------------------------------------
    */
    printf("TEST 7: Delete Node With Two Children\n");

    root = delete(root, 4);

    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 8: Delete Nonexistent Key
        -------------------------------------------------
    */
    printf("TEST 8: Delete Nonexistent Key\n");

    root = delete(root, 999);

    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 9: Trigger LL Rotation
        -------------------------------------------------
    */
    printf("TEST 9: LL Rotation\n");

    freeTree(root);
    root = NULL;

    int ll[] = {30,20,10};

    for (int i = 0; i < 3; ++i) {
        root = insert(root, ll[i]);
    }

    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 10: Trigger RR Rotation
        -------------------------------------------------
    */
    printf("TEST 10: RR Rotation\n");

    freeTree(root);
    root = NULL;

    int rr[] = {10,20,30};

    for (int i = 0; i < 3; ++i) {
        root = insert(root, rr[i]);
    }

    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 11: Trigger LR Rotation
        -------------------------------------------------
    */
    printf("TEST 11: LR Rotation\n");

    freeTree(root);
    root = NULL;

    int lr[] = {30,10,20};

    for (int i = 0; i < 3; ++i) {
        root = insert(root, lr[i]);
    }

    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 12: Trigger RL Rotation
        -------------------------------------------------
    */
    printf("TEST 12: RL Rotation\n");

    freeTree(root);
    root = NULL;

    int rl[] = {10,30,20};

    for (int i = 0; i < 3; ++i) {
        root = insert(root, rl[i]);
    }

    inorder(root);
    printf("\n\n");

    /*
        -------------------------------------------------
        TEST 13: Large Mixed Operations
        -------------------------------------------------
    */
    printf("TEST 13: Large Mixed Operations\n");

    freeTree(root);
    root = NULL;

    int big[] = {
        50,20,70,10,30,60,80,
        5,15,25,35,55,65,75,85,
        1,6,14,16,24,26,34,36
    };

    int bigSize = sizeof(big)/sizeof(big[0]);

    for (int i = 0; i < bigSize; ++i) {
        root = insert(root, big[i]);
    }

    printf("Initial Tree:\n");
    inorder(root);
    printf("\n");

    int dels[] = {70,20,50,5,85};

    for (int i = 0; i < 5; ++i) {
        printf("Deleting %d\n", dels[i]);
        root = delete(root, dels[i]);

        inorder(root);
        printf("\n");
    }

    printf("\n");

    /*
        -------------------------------------------------
        TEST 14: Delete Entire Tree
        -------------------------------------------------
    */
    printf("TEST 14: Delete Entire Tree\n");

    while (root) {
        printf("Deleting Root: %d\n", root->key);
        root = delete(root, root->key);

        inorder(root);
        printf("\n");
    }

    printf("\nTree Fully Deleted.\n");

    freeTree(root);

    return 0;
}


// int main() {
//     int arr[] = {'a', 'z', 'b', 'y', 'c', 'x', 'd', 'w', 'e', 'v', 'f'};
//     int size = sizeof(arr) / sizeof(arr[0]);
//
//     Node *root = NULL;
//
//     for (int i = 0; i < size; ++i) {
//         root = insert(root, arr[i]);    
//     }
//
//     levelOrder(root);
//     freeTree(root);
//
//     return 0;
// }
