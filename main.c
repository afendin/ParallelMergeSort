/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#define ARRAY_SIZE 1000000
#define THREAD_COUNT 5

const int elementsPerThread = ARRAY_SIZE / THREAD_COUNT;
const int remainedElements = ARRAY_SIZE % THREAD_COUNT;

int numbers[ARRAY_SIZE];

void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);
void assertArray(int arr[], int size);

void* parallel_mergeSort(void *arg);
void  mergeParts();

int main()
{
    struct timeval  start, end;
    double timeSpent;

    // Init array with some random unsorted numbers
    for (int i = 0; i < ARRAY_SIZE; ++i)
        numbers[i] = rand() % 1000000;

    pthread_t threads[THREAD_COUNT];
    gettimeofday(&start, NULL);

//    mergeSort(numbers, 0, ARRAY_SIZE - 1);

    for (int i = 0; i < THREAD_COUNT; ++i)
        pthread_create(&threads[i], NULL, parallel_mergeSort, (void*)i);

    for (int i = 0; i < THREAD_COUNT; ++i)
        pthread_join(threads[i], NULL);

    mergeParts();

    gettimeofday(&end, NULL);
    timeSpent = ((double) ((double) (end.tv_usec - start.tv_usec) / 1000000 +
                            (double) (end.tv_sec - start.tv_sec)));
    printf("Job finished in: %lf\n", timeSpent);
    assertArray(numbers, ARRAY_SIZE);

    return 0;
}


void mergeSort(int arr[], int l, int r)
{
    if (l >= r)
        return;

    int m = l + (r - l) / 2;

    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
}

void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temp arrays
    int L[n1], R[n2];

    // Copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Merge the temp arrays back into arr[l..r]
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elemts of L[], if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    //Copy the remaining elemetns of R[], if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// test if all elements are in ascending order
void assertArray(int arr[], int size)
{
    for (int i = 0; i < size; ++i) {
        if (i == size - 1)
            return;

        assert(arr[i] <= arr[i+1]);
    }
}

void *parallel_mergeSort(void *arg)
{
    int threadIndex = (int)arg;

    int l = threadIndex * elementsPerThread;
    int r = (threadIndex + 1) * elementsPerThread -1;
    if (threadIndex == THREAD_COUNT - 1)
        r += remainedElements;

    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(numbers, l, r);
        merge(numbers, l, m, r);
    }

    return NULL;
}

void  mergeParts()
{
    // Initially our partCount is equal to thread count
    int partCount = THREAD_COUNT;

    // Factor of part size, will be multiplied by two as we merge
    int partFactor = 1;

    // Until parts left to merge
    while (partCount > 0) {
        int partIndex = 0;

        while (partIndex < partCount) {
            int l = partIndex * elementsPerThread * partFactor;
            int m = l + elementsPerThread * partFactor - 1;
            int r = (partIndex += 2) * elementsPerThread * partFactor - 1;

            if (r >= ARRAY_SIZE)
                r = ARRAY_SIZE - 1;

            merge(numbers, l, m, r);
        }

        // After merging, two pieces became one, so we increase the factor of size
        partFactor *= 2;

        // and decrease count of parts
        partCount /= 2;
    }

    // We merge even with partCount = 1, because of the possible remainder.
    // Btw merging with odd number of parts looks like this
    // [|||] [|||] [|||] [|||] [|||]         l r  l r  l r  l r   r
    //   [||||||]   [||||||]   [|||]          l    r    l    r    r
    //      [||||||||||||]     [|||]             l         r      r
    //          [|||||||||||||||]                     l           r
    // As you can see reaminder merged at the last iteration where partCount = 1
}
