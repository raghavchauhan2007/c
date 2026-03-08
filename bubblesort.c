#include <stdio.h>
#define len(arr) sizeof(arr)/sizeof(arr[0])

void swap(int *a, int *b) {
    *a = *a^*b;
    *b = *a^*b;
    *a = *a^*b;
}

void bubblesort(int *arr, int size) {
    for(int i=0; i<size-1; i++) {
        for(int j=0; j<size-i-1; j++) {
            if(arr[j] > arr[j+1]) {
                swap(&arr[j], &arr[j+1]);
            }
        }
    }
}

void print(int *arr, int size){
    for(int i=0; i<size; i++) {
        printf("%d\n", arr[i]);
    }
}

int main() {
    int arr[] = {
        4,1,3,2
    };

    bubblesort(arr, len(arr));
    print(arr, len(arr));

    return 0;
}