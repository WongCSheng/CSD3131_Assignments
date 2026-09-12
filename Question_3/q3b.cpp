#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

// References:
// https://www.geeksforgeeks.org/cpp/how-to-make-a-countdown-timer-in-cpp/
// https://www.geeksforgeeks.org/dsa/understanding-time-complexity-simple-examples/
// https://www.geeksforgeeks.org/cpp/measure-execution-time-function-cpp/
// https://www.geeksforgeeks.org/dsa/analysis-algorithms-big-o-analysis/
// https://github.com/Cheejyg/Integration-of-Mergesort-and-Insertion-Sort

// https://en.wikipedia.org/wiki/Heapsort
// https://www.geeksforgeeks.org/dsa/heap-sort/
// https://stackoverflow.com/questions/26491137/number-of-comparisons-for-heap-sort

// https://stackoverflow.com/questions/5731863/mapping-a-numeric-range-onto-another

// Radix Sort
// O(n * k), number of digits in largest number
// sorts nums digit by digit using stable subroutine like Counting Sort
// beats O(n log n) when k is small or constant (linear time)
// limitation: only works efficiently on integers or fixed length strings

// test cases: integer only, maybe float as a bonus limitation for radix

// -------------- GLOBALS --------------
int comparisons = 0;

std::random_device rng;
std::mt19937 generator(rng()); // generator
std::uniform_real_distribution<double> distribution(-1.0, 1.0);

// -------------- ALGORITHM 1: A O(n log n) algorithm - Heap Sort --------------
template <typename T>
void Heapify(std::vector<T>& arr, int n, int i){

    // Initialize largest as root
    int largest = i;

    // left index = 2*i + 1
    int l = 2 * i + 1;

    // right index = 2*i + 2
    int r = 2 * i + 2;

    // If left child is larger than root
    if (l < n && arr[l] > arr[largest]) {
        comparisons++;
        largest = l;
    }

    // If right child is larger than largest so far
    if (r < n && arr[r] > arr[largest]) {
        comparisons++;
        largest = r;
    }

    // If largest is not root
    if (largest != i) {
        std::swap(arr[i], arr[largest]);

        // Recursively heapify the affected sub-tree
        Heapify(arr, n, largest);
    }
}

template <typename T>
void HeapSort(std::vector<T>& arr) {
    int n = arr.size();

    // Build heap, rearrange vector
    for (int i = n / 2 - 1; i >= 0; i--)
        Heapify(arr, n, i);

    // Extract an element from heap
    for (int i = n - 1; i > 0; i--) {

        // Move current root to end
        std::swap(arr[0], arr[i]);

        // Call max heapify on the reduced heap
        Heapify(arr, i, 0);
    }
}

// -------------- ALGORITHM 2: Radix Sort --------------
template <typename T>
void RadixSort(std::vector<T>& arr) {
    
}

// -------------- TEST CASES HELPERS --------------
float Mapper(float inputStart, float inputEnd, float outputStart, float outputEnd, float input) {
    return outputStart + ((outputEnd - outputStart) / (inputEnd - inputStart)) * (input - inputStart);
}

// probably would just generate test cases on the fly...?
void TestCasesIntegers(std::vector<int>& arr, int num = 100, int lowerRange = -10, int upperRange = 10) {
    for (int i = 0; i < num; i++) {
        arr.push_back((int)Mapper(-1.0, 1.0, lowerRange, upperRange, distribution(generator)));
    }
}

void TestCasesIntegersOrder(std::vector<int>& arr, int num = 100, int lowerRange = -10, int upperRange = 10) {
    for (int i = lowerRange, count = 0; i < upperRange && count < num; i++, count++) {
        arr.push_back(i);
    }
}

void TestCasesIntegersOrderReversed(std::vector<int>& arr, int num = 100, int lowerRange = -10, int upperRange = 10) {
    for (int i = upperRange, count = 0; i > lowerRange && count < num; i--, count++) {
        arr.push_back(i);
    }
}

// -------------- VECTOR-RELATED HELPERS --------------
template <typename T>
bool PopulateArray(std::vector<T>& arr, std::string filepath) {
    
}

template <typename T>
void ClearArray(std::vector<T>& arr) {
    arr.clear();
}

template <typename T>
void DisplayArray(std::vector<T>& arr) {
    std::cout << " -- CURRENT ARRAY CONTENTS --\n";
    for (T& var : arr) {
        std::cout << var << " ";
    }
    std::cout << std::endl;
}

// -------------- OUTPUT-RELATED HELPERS --------------
template <typename T>
float DisplayComparisonOne(std::vector<T>& arr, int sortType) {
    int n = arr.size();
    // 0 - n log n, 1 - n * k
    float bigO = (sortType == 0 ? n * std::log2f(n) : n); // n * k

    float compBigORatio = (float)comparisons / bigO;

    std::cout << "Ratio: " << compBigORatio << std::endl;

    return compBigORatio; // just in case
}

// -------------- MAIN --------------
int main() {
    std::vector<int> testDataArray; // only initialise float to demonstrate limitation case

    std::string userInput;

    bool runProgram = true;
    while (runProgram) {
        // reset user input, comparison, testdataarray at every loop start
        userInput.clear();
        comparisons = 0;
        ClearArray(testDataArray);

        std::cout << "\n========= INSERT TITLE HERE =========\n";
        std::cout << "OPTIONS:\n";
        std::cout << " 1 - \n";
        std::cout << " Q - Quit Application\n\n";

        // Get user's input
        std::cin >> userInput;

        // std::cin fails somehow
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        // Process user input here - convert all to lower case, remove trailing and leading whitespaces
        std::transform(userInput.begin(), userInput.end(), userInput.begin(), [](unsigned char c){ return std::tolower(c); });

        // Perform actions based on input
        // CASE Q: QUIT APPLICATION
        if (userInput == "q") {
            std::cout << "Quitting application!\n";

            ClearArray(testDataArray);
            runProgram = false;
        } else if (userInput == "1") {
            //testDataArray = std::vector<int>{10, 56, 3, 23, 90, 22, 34, 18, 90};
            //testDataArray = std::vector<int>{90, 80, 70, 60, 50, 40, 30, 12, 1};
            //TestCasesIntegers(testDataArray, 42, 0, 100);
            //TestCasesIntegersOrder(testDataArray, 42, 0, 100);
            TestCasesIntegersOrderReversed(testDataArray, 42, 0, 100);

            DisplayArray(testDataArray);
            HeapSort(testDataArray);
            DisplayArray(testDataArray);
            DisplayComparisonOne(testDataArray, 0);
        }
    }

    return 0;
}