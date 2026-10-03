#include <iostream>
#include <vector>

double check = 0;

/**
 * @brief Calling 3 recursive functions with n/3 as the argument until the base case is reached.
 * T(n) = 3T(n/3)
 * 
 * @param n 
 */
void recursiveExample(int n)
{
    if (n <= 1)
    {
        //std::cout << "Base case reached with n = " << n << std::endl;
        return;
    }

    for(int i = 0; i < n; ++i)
    {
        ++check;
    }
    //std::cout << "Recursive call with n = " << n << std::endl;
    recursiveExample(n / 3);
    recursiveExample(n / 3);
    recursiveExample(n / 3);
}

/**
 * @brief Merge 3 sorted subarrays into a single sorted array.
 * The subarrays are defined by the indices left, mid1, mid2, and right.
 * 
 * @param arr 
 * @param left 
 * @param mid1 
 * @param mid2 
 * @param right 
 */
void merge(std::vector<int>& arr, int left, int mid1, int mid2, int right)
{
    std::vector<int> temp(right - left + 1);
    int i = left, j = mid1 + 1, k = mid2 + 1, t = 0;

    while (i <= mid1 && j <= mid2 && k <= right)
    {
        if (arr[i] <= arr[j] && arr[i] <= arr[k])
            temp[t++] = arr[i++];
        else if (arr[j] <= arr[i] && arr[j] <= arr[k])
            temp[t++] = arr[j++];
        else
            temp[t++] = arr[k++];
    }

    while (i <= mid1)
        temp[t++] = arr[i++];
    while (j <= mid2)
        temp[t++] = arr[j++];
    while (k <= right)
        temp[t++] = arr[k++];

    for (int m = 0; m < t; ++m)
        arr[left + m] = temp[m];
}

/**
 * @brief 3 way version of merge sort. The array is divided into 3 parts and each part is sorted recursively.
 * 
 * @param arr 
 * @param left 
 * @param right 
 */
void threeWayMergeSort(std::vector<int>& arr, int left, int right)
{
    if (left >= right)
        return;

    int third = (right - left) / 3;
    int mid1 = left + third;
    int mid2 = left + 2 * third;

    threeWayMergeSort(arr, left, mid1);
    threeWayMergeSort(arr, mid1 + 1, mid2);
    threeWayMergeSort(arr, mid2 + 1, right);

    // Merge the three sorted subarrays
    merge(arr, left, mid1, mid2, right);
}

/**
 * @brief Print out the elements of the array.
 * 
 * @param arr 
 */
void printArray(const std::vector<int>& arr)
{
    for (const auto& elem : arr)
        std::cout << elem << " ";
    std::cout << std::endl;
}