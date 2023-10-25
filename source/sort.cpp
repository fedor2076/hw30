#include "sort.h"

bool Sort::multiThreads = true;
ThreadPool Sort::thrPool;

void Sort::quickSort(int *arr, long size)
{
    if (Sort::multiThreads)
    {
        Sort::thrPool.start();
        quicksort(arr, 0, size);
        Sort::thrPool.stop();
        return;
    }
    quicksort(arr, 0, size);
}

void Sort::quicksort(int *array, long left, long right)
{
    if (left >= right)
        return;
    long left_bound = left;
    long right_bound = right;

    long middle = array[(left_bound + right_bound) / 2];

    do
    {
        while (array[left_bound] < middle)
        {
            left_bound++;
        }
        while (array[right_bound] > middle)
        {
            right_bound--;
        }

        if (left_bound <= right_bound)
        {
            std::swap(array[left_bound], array[right_bound]);
            left_bound++;
            right_bound--;
        }
    } while (left_bound <= right_bound);

    if (right_bound - left > 100000 && Sort::multiThreads)
    {

        auto fL = thrPool.push_task(quicksort, array, left, right_bound);
        quicksort(array, left_bound, right);
        while (fL.wait_for(std::chrono::seconds(0)) == std::future_status::timeout)
        {
            thrPool.run_pending_task();
        }
    }
    else
    {
        quicksort(array, left, right_bound);
        quicksort(array, left_bound, right);
    }
}
