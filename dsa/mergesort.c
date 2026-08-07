#include <stdio.h>
void merge(int *arr,int *tmp, int start, int mid, int end) {
    int i = start;
    int j = mid+1;
    int k = start;

    printf("Merging %d-%d\n", start, end);

    while(i <= mid && j <= end) {
        if(arr[i] <= arr[j]) {
            tmp[k++] = arr[i++];
        }
        
        else {
            tmp[k++] = arr[j++];
        }
    }

    while(i <= mid) {
        tmp[k++] = arr[i++];
    }
    
    while(j <= end) {
        tmp[k++] = arr[j++];
    }

    for(int idx=start; idx<=end; idx++) {
        arr[idx] = tmp[idx];
    }
}

void mergeSort(int *arr, int*tmp, int start, int end) {
    if(start<end) {
        int mid = start + (end-start)/2;
        mergeSort(arr, tmp, start, mid);
        mergeSort(arr, tmp, mid+1, end);

        merge(arr, tmp, start, mid, end);
    }
}

void print(int *arr, int size){
    for(int i=0; i<size; i++) {
        printf("%d\n", arr[i]);
    }
}

// int main() {
//     int arr[] = {
//         4,1,3,2
//     };

//     int size = sizeof(arr)/sizeof(arr[0]);

//     int *tmp = malloc(size*sizeof(int));

//     mergeSort(arr, tmp, 0, size-1);
//     print(arr, size);

//     return 0;
// }

int main() {

    int arr[] = {
        42, -7, 13, 13, 0, 99, -100, 58, 58, 3,
        77, -42, 5, 5, 5, 81, -1, 64, 32, -50,
        19, 19, 2, 73, -9, 88, 1, 47, 47, -30,
        60, 17, 17, 90, -3, 6, 72, 72, -80, 25,
        11, 11, 54, -60, 8, 8, 39, 100, -2, 66
    };

    int size = sizeof(arr) / sizeof(arr[0]);

    int tmp[size];

    printf("Original array:\n");
    print(arr, size);

    mergeSort(arr, tmp, 0, size - 1);

    printf("\nSorted array:\n");
    print(arr, size);

    return 0;
}