#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef enum {
    SUCCESS,
    ERR_UNDERFLOW,
    ERR_MEMORY,
    ERR_INVALID_POS
} Status;

typedef struct Node{
    struct Node *next;
    int data;
} Node;

// CRUD

Node* createList() {
    return NULL;
}

bool isEmpty(Node *head) {
    return head == NULL;
}

int length(Node *head) {
    int count = 0;
    Node *curr = head;
    while (curr) {
        curr = curr->next;
        count++;
    }

    return count;
}

Status insert(Node **head, int idx, int val) {
    if(idx < 0) {
        return ERR_INVALID_POS;
    }

    Node *newNode = (Node*) malloc(sizeof(Node));
    if(!newNode) {
        return ERR_MEMORY;
    }
    newNode->data = val;

    if (idx == 0) {
        newNode->next = *head;
        *head = newNode;
        return SUCCESS;
    }

    Node *curr = *head;
    for (int i=0; curr && i<idx-1; i++) {
        curr = curr->next;
    }

    if(!curr) {
        free(newNode);
        return ERR_INVALID_POS;
    }

    newNode->next = curr->next;
    curr->next = newNode;
    return SUCCESS;    
}

Status delete(Node **head, int idx) {
    if (isEmpty(*head)) {
        return ERR_UNDERFLOW;
    }

    if (idx < 0) {
        return ERR_INVALID_POS;
    }

    Node *target;
    if(idx == 0) {
        target = *head;
        *head = (*head)->next;
        free(target);
        return SUCCESS;
    }

    Node *prev = *head;
    for (int i=0; prev->next && i<idx-1; i++) {
        prev = prev->next;
    }

    if(!prev->next) {
        return ERR_INVALID_POS;
    }

    target = prev->next;
    prev->next = prev->next->next;
    free(target);
    return SUCCESS;
}

int get(Node *head, int idx, Status *s) {
    Node *curr = head;
    for (int i=0; curr; i++) {
        if(i == idx) {
            *s = SUCCESS;
            return curr->data;
        }
        curr = curr->next;
    }
    *s = ERR_INVALID_POS;
    return -1;
}

int find(Node *head, int val) {
    Node *curr = head;
    for(int i=0; curr; i++) {
        if(curr->data == val) {
            return i;
        }
        curr = curr->next;
    }
    return -1;
}

void destroyList(Node **head) {
    Node *curr = *head;
    while (curr) {
        Node *next = curr->next;
        free(curr);
        curr = next;
    }
    *head = NULL;
}

// TESTING AND VERIFICATION

void display(Node *head) {
    printf("List: ");
    if(isEmpty(head)) {
        printf("[EMPTY]\n");
        return;
    }

    Node *curr = head;
    while (curr) {
        printf("%d → ", curr->data);
        curr = curr->next;
    }
    printf("NULL\n");
}

void report(const char *testName, Status s) {
    printf("%-30s: ", testName);
    switch (s) {
        case SUCCESS: printf("[SUCCESS]\n");
        break;
        case ERR_UNDERFLOW: printf("[ERR_UNDERFLOW]\n");
        break;
        case ERR_MEMORY: printf("[ERR_MEMORY]\n");
        break;
        case ERR_INVALID_POS: printf("[ERR_INVALID_POS]\n");
        break;
    }
}

Node* findMid(Node *head) {
    Node *hare = head;
    Node *turtle = head;

    while (hare->next && hare->next->next) {
        turtle = turtle->next;
        hare = hare->next->next;
    }

    return turtle;
}

Node* merge(Node **h1, Node **h2) {
    if (!(*h1)) {
        return *h2;
    }
    
    if (!(*h2)) {
        return *h1;
    }
    if ((*h1)->data <= (*h2)->data) {
        (*h1)->next = merge(&(*h1)->next, h2);
        return *h1;
    }

    else {
        (*h2)->next = merge(&(*h2)->next, h1);
        return *h2;
    }
}

Node* mergeSort(Node **head) {
    if(head == NULL || (*head)->next == NULL) {
        return *head;
    }

    Node *mid = findMid(*head);
    Node *left = *head;
    Node *right = mid->next;
    mid->next = NULL;

    left = mergeSort(&left);
    right = mergeSort(&right);

    return merge(&left, &right);
}

int main() {
    Node *head = createList();
    insert(&head, 0, 5);
    insert(&head, 0, 3);
    insert(&head, 0, 4);
    insert(&head, 0, 2);
    insert(&head, 0, 1);
    // printf("%d\n", isEmpty(head));
    display(head);

    head = mergeSort(&head);

    display(head);

    destroyList(&head);
    return 0;
}