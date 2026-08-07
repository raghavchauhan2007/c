#include <stdio.h>
#include <stdlib.h>
#define swap(x,y) {int tmp = x; x=y; y=tmp;}


typedef struct Heap {
    int *arr;
    int size;
} Heap;

Heap* initHeap() {
    Heap *h = (Heap*) malloc(sizeof(Heap));
    
    if(!h) {
        printf("Memory Allocation Failed");
        return NULL;
    }
    
    h->arr = (int*) calloc(100, sizeof(int));
    
    if(!h->arr) {
        printf("Memory Allocation Failed");
        return NULL;
    }
    
    h->size = 0;
    return h;
}

void freeHeap(Heap *h) {
    free(h->arr);
    free(h);
}

void insert(Heap *h, int data) {
    int idx = h->size++;
    h->arr[idx] = data;
    
    while(idx > 0) {
        int parent = (idx-1)/2;
    
        if(h->arr[parent] < h->arr[idx]) {
            swap(h->arr[parent], h->arr[idx]);
            idx = parent;
        } 
        
        else {
            break;
        }
    }
}

void heapify(int arr[], int size, int idx) {
    int largest = idx;
    int leftChild = 2*idx + 1;
    int rightChild = 2*idx + 2;

    if(leftChild<size && arr[leftChild] > arr[largest]) {
        largest = leftChild;
    }

    if(rightChild<size && arr[rightChild] > arr[largest]) {
        largest = rightChild;
    }

    if(largest != idx) {
        swap(arr[largest], arr[idx]);
        heapify(arr, size, largest);
    }
}

int extractMax(Heap *h) {
    if(h->size < 1) {
        printf("Heap Underflow\n");
        return -1;
    }

    int max = h->arr[0];
    h->arr[0] = h->arr[--h->size];
    heapify(h->arr, h->size, 0);
    return max;
}

void buildHeap(int arr[], int size) {    
    for(int i=(size/2)-1; i>=0; i--) {
        heapify(arr, size, i);
    }
}

void heapSort(int arr[], int size) {
    buildHeap(arr, size);
    for(int i=size-1; i>0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

void printHeap(Heap *h) {
    for(int i=0; i<h->size; i++) {
        printf("%d  ", h->arr[i]);
    }
    printf("\n");
}

void printArray(int *arr, int size) {
    for(int i=0; i<size; i++) {
        printf("%d  ", arr[i]);
    }

    printf("\n");
}

int main() {
    int arr[] = {8,6,5,4,7,2,1,3,5,9,6,3,1,4,8,6,3,2,4,5,2,2,2,1,};
    int size = sizeof(arr)/sizeof(arr[0]);

    printArray(arr, size);
    heapSort(arr, size);
    printArray(arr, size);
      
    return 0;
}

// int main() {

//     Heap *h = initHeap();
//     if(h == NULL) {
//         return 1;
//     }

//     printf("=== Inserting Elements ===\n");

//     int arr[] = {10, 4, 15, 20, 1, 8, 12};
//     int n = sizeof(arr)/sizeof(arr[0]);

//     for(int i = 0; i < n; i++) {
//         printf("Insert %d\n", arr[i]);
//         insert(h, arr[i]);
//         printHeap(h);
//     }

//     printf("\n=== Extracting Max Elements ===\n");

//     while(h->size > 0) {
//         int max = extractMax(h);
//         printf("Extracted: %d\n", max);
//         printHeap(h);
//     }

//     printf("\n=== Testing Underflow ===\n");
//     extractMax(h);   // should trigger underflow

//     freeHeap(h);

//     return 0;
// }