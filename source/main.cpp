#include "sort.h"

int main()
{
    srand(0);
    long size = 100000000;
    int *array = new int[size];
    for (long i = 0; i < size; i++)
    {
        array[i] = rand() % 500000;
    }

    time_t start, end;
    Sort::multiThreads = true;
    time(&start);
    Sort::quickSort(array, size);
    time(&end);

    double seconds = difftime(end, start);
    std::cout<<"Multi threads the time:  "<< seconds<<" seconds"<<"\n";

//проверка
    for (long i = 0; i < size - 1; i++)
      {
        if (array[i] > array[i + 1])
        {
          std::cout<<"incorrect sorting"<<"\n";
          break;
        }
      }


    for (long i = 0; i < size; i++)
    {
        array[i] = rand() % 500000;
    }
    Sort::multiThreads = false;
    // однопоточный запуск
    time(&start);
    Sort::quickSort(array, size);
    time(&end);
    seconds = difftime(end, start);
    std::cout << "One thread the time:  " << seconds << " seconds" <<"\n";

    delete[] array;

}
