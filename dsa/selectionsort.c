#include <stdio.h>
#define len(arr) sizeof(arr)/sizeof(arr[0])

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void print(int *arr, int size){
    for(int i=0; i<size; i++) {
        printf("%d\n", arr[i]);
    }
}

void selectionsort(int *arr, int size) {
    for(int i=0; i<size; i++) {
        int smallest = i;

        for(int j=i+1; j<size; j++) {
            if(arr[smallest] > arr[j]) {
                smallest = j;
            }
        }

        swap(&arr[i], &arr[smallest]);
    }
}

int main() {
    int arr[] = {
        4,1,3,2
    };

    selectionsort(arr, len(arr));
    print(arr, len(arr));

    return 0;
}