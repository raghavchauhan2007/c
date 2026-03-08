#include <stdio.h>
#define len(arr) sizeof(arr)/sizeof(arr[0])

void swap(int *a, int *b) {
    *a = *a^*b;
    *b = *a^*b;
    *a = *a^*b;
}

void print(int *arr, int size){
    for(int i=0; i<size; i++) {
        printf("%d\n", arr[i]);
    }
}

void insertionsort(int *arr, int size) {
    for(int i=0; i<size; i++) {
        int curr = arr[i];
        int prev = i-1;

        while(prev >= 0 && curr < arr[prev]) {
            arr[prev+1] = arr[prev];
            prev--;
        }

        arr[prev+1] = curr;
    }
}

int main() {
    int arr[] = {
        4,1,3,2
    };

    insertionsort(arr, len(arr));
    print(arr, len(arr));

    return 0;
}