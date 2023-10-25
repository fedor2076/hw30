#pragma once
#include "threadpool.h"

class Sort
{
public:
  static bool multiThreads;
  static void quickSort(int *arr, long size);
private:
  static ThreadPool thrPool;
  static void quicksort(int *array, long left, long right);
};
