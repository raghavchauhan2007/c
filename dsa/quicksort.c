#include <stdio.h>
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int partition(int *arr, int start, int end) {
    int idx = start-1;
    int pivot = arr[end];

    for(int j=start; j<end; j++) {
        if(arr[j] <= pivot) {
            swap(&arr[++idx], &arr[j]);
        }
    }

    swap(&arr[++idx], &arr[end]);

    return idx;
}

void quickSort(int *arr, int start, int end) {
    if(start < end) {
        int pivIDX = partition(arr, start, end);

        quickSort(arr, start, pivIDX-1);
        quickSort(arr, pivIDX+1, end);
    }
}

void print(int *arr, int size){
    for(int i=0; i<size; i++) {
        printf("%d\n", arr[i]);
    }
}

int main() {
    int arr[] = {
        1,2,3,4,5
    };

    quickSort(arr, 0, 4);
    print(arr, 5 );

    return 0;
}