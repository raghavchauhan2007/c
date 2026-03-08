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

int idx = -1;
Node* buildTree(int nodes[]) {
    idx++;
    if(nodes[idx] == -1) {
        return NULL;
    }

    Node *newNode = createNode(nodes[idx]);
    newNode->left = buildTree(nodes);
    newNode->right = buildTree(nodes);

    return newNode;
}

void inorder(Node *root) {
    if (root) {
        inorder(root->left);
        printf("%d", root->data);
        inorder(root->right);
    }
}




#define MAX 100

typedef struct Stack {
    Node* arr[MAX];
    int top;
} Stack;

void initStack(Stack *s) {
    s->top = -1;
}

int isEmptyStack(Stack *s) {
    return s->top == -1;
}

void push(Stack *s, Node *node) {
    s->arr[++(s->top)] = node;
}

Node* pop(Stack *s) {
    return s->arr[(s->top)--];
}



typedef struct Queue {
    Node *arr[MAX];
    int front;
    int rear;
} Queue;

void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
}

int isEmptyQueue(Queue *q) {
    return (q->front > q->rear);
}

void enqueue(Queue *q, Node *node) {
    q->arr[++(q->rear)] = node;
}

Node* dequeue(Queue *q) {
    return (q->arr[(q->front)++]);
}


void preorderIterative(Node *root) {
    Stack s;
    initStack(&s);

    push(&s, root);

    while (!isEmptyStack(&s)) {
        Node *node = pop(&s);
        printf("%d", node->data);

        if (node->right) {
            push(&s, node->right);
        }

        if (node->left) {
            push(&s, node->left);
        }
    }
}

void inorderIterative(Node *root) {
    Stack s;
    initStack(&s);

    Node *current = root;
    
    while(current || !isEmptyStack(&s)) {
        while(current) {
            push(&s, current);
            current = current->left;
        }

        current = pop(&s);
        printf("%d", current->data);
        current = current->right;
    }
}

void postorder(Node *root) {
    if(root) {
        postorder(root->left);
        postorder(root->right);
        printf("%d", root->data);
    }
}

void postorderIterative(Node *root) {
    Stack s1, s2;
    initStack(&s1);
    initStack(&s2);

    push(&s1, root);

    while(!isEmptyStack(&s1)) {
        Node *node = pop(&s1);
        push(&s2, node);

        if(node->left) {
            push(&s1, node->left);
        }
        
        if(node->right) {
            push(&s1, node->right);
        }
    }

    while(!isEmptyStack(&s2)) {
        printf("%d", pop(&s2)->data);
    }
}

void levelOrder(Node *root) {
    Queue q;
    initQueue(&q);

    enqueue(&q, root);
    enqueue(&q, NULL);

    while(!isEmptyQueue(&q)) {
        Node *current = dequeue(&q);
        if(!current) {
            if(!isEmptyQueue(&q)) {
                printf("\n");
                enqueue(&q, NULL);
                continue;
            }
            
            else {
                break;
            }

        }
        printf("%d ", current->data);

        if(current->left) {
            enqueue(&q, current->left);
        }

        if(current->right) {
            enqueue(&q, current->right);
        }
    }
}


int main() {
    int nodes[] = {1,2,4,-1,-1,5,-1,-1,3,-1,6,-1,-1};

    Node *root = buildTree(nodes);
    inorder(root);
    printf("\n");
    
    postorder(root);
    printf("\n");
    postorderIterative(root);
    printf("\n");
    levelOrder(root);
    printf("\n");
    
    return 0;
}