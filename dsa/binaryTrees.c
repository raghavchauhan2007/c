#include <stdio.h>
#include <stdlib.h>
typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

Node* createNode(int data) {
    Node *newNode = (Node*) calloc(1, sizeof(Node));

    if(!newNode) {
        printf("Memory Allocation Failed\n");
        return NULL;
    }

    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

Node* insert(Node* root, int data) {
    if(root == NULL) {
        return createNode(data);
    }

    if(data < root->data) {
        root->left = insert(root->left, data);
    }

    else {
        root->right = insert(root->right, data);
    }

    return root;
}

Node* buildBST(int arr[], int size) {
    Node *root = NULL;

    for(int i=0; i<size; i++) {
        root = insert(root, arr[i]);
    }

    return root;
}

void inorder(Node *root) {
    if(root == NULL) {
        return;
    }

    inorder(root->left);
    printf("%d", root->data);
    inorder(root->right);
}

char search(Node *root, int key) {
    if(root == NULL) {
        return 0;
    }

    if(root->data == key) {
        return 1;
    }

    if(key < root->data) {
        return search(root->left, key);
    }
    
    else {
        return search(root->right, key);
    }
}

Node* getInorderSuccessor(Node *root) {
    while(root && root->left) {
        root = root->left;
    }

    return root;
}

Node* deleteNode(Node *root, int key) {
    if(root == NULL) {
        return NULL;
    }

    if(key < root->data) {
        root->left = deleteNode(root->left, key);
    }

    else if(key > root->data) {
        root->right = deleteNode(root->right, key);
    }

    else {
        if(!root->left) {
            Node *tmp = root->right;
            free(root);
            return tmp;
        }
        
        else if(!root->right) {
            Node *tmp = root->left;
            free(root);
            return tmp;
        }

        else {
            Node *IS = getInorderSuccessor(root->right);
            root->data = IS->data;
            deleteNode(root->right, root->data);
        }
    }

    return root;
}

void printTree(Node *root, int space) {
    if (root == NULL)
        return;

    space += 5;   // distance between levels

    // print right subtree first
    printTree(root->right, space);

    // print current node
    printf("\n");
    for (int i = 5; i < space; i++)
        printf(" ");
    printf("%d\n", root->data);

    // print left subtree
    printTree(root->left, space);
}

int main() {
    int arr[] = {3,2,1,5,6,4};
    
    Node *root = buildBST(arr, 6);
    printTree(root, 0);
    root = insert(root, 45);
    printTree(root, 0);
    // inorder(root);
    // printf("\n");
    // printf("%d\n", search(root, 4));
    deleteNode(root, 3);
    // inorder(root);
    // printf("\n");
    // printf("%d\n", root->left->right->data);

    printTree(root, 0);

    return 0;
}