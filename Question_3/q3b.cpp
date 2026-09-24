#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <typeinfo>

#include "nlohmann_json.h"
#include "vega_lite_plot.h"

// Base structure of sorts adapted from GeeksForGeeks
// nlohmann json and vega lite plot implementation taken from csd2183 example

// References:
// https://www.geeksforgeeks.org/cpp/how-to-make-a-countdown-timer-in-cpp/
// https://www.geeksforgeeks.org/dsa/understanding-time-complexity-simple-examples/
// https://www.geeksforgeeks.org/cpp/measure-execution-time-function-cpp/
// https://www.geeksforgeeks.org/dsa/analysis-algorithms-big-o-analysis/
// https://github.com/Cheejyg/Integration-of-Mergesort-and-Insertion-Sort

// https://en.wikipedia.org/wiki/Heapsort
// https://www.geeksforgeeks.org/dsa/heap-sort/
// https://stackoverflow.com/questions/26491137/number-of-comparisons-for-heap-sort

// https://www.geeksforgeeks.org/dsa/radix-sort/

// https://stackoverflow.com/questions/5731863/mapping-a-numeric-range-onto-another
// https://dev.to/nitinsonicoder/radix-sort-1igp
// https://stackoverflow.com/questions/32543683/radix-sort-array-of-strings

// Radix Sort
// O(n * k), number of digits in largest number
// sorts nums digit by digit using stable subroutine like Counting Sort
// beats O(n log n) when k is small or constant (linear time)
// limitation: only works efficiently on integers or fixed length strings

// test cases: integer only, maybe float as a bonus limitation for radix

// to do:
// heap - show diff data types, like float, int, string
// radix - show string, int only

// -------------- GLOBALS --------------
int comparisons = 0;
int passes = 0;

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
void HeapSort(std::vector<T> arr, std::vector<T>& output) {
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

    output = arr;
}

// -------------- ALGORITHM 2: Radix Sort --------------
int GetMaxInArray(std::vector<int>& arr) {
    int greatestFound = arr[0];
    for (int i = 1; i < (int)arr.size(); i++) {
        if (arr[i] > greatestFound) greatestFound = arr[i];
    }
    return greatestFound;
}

int GetMaxInArray(std::vector<std::string>& arr) {
    int greatestFound = (int)arr[0].length();
    for (int i = 1; i < (int)arr.size(); i++) {
        if ((int)arr[i].length() > greatestFound) greatestFound = (int)arr[i].length();
    }
    return greatestFound;
}

template <typename T>
void CountSort(std::vector<T>& arr, int exp) {
    int n = (int)arr.size();

    // Output array
    int output[n];
    int i, count[10] = { 0 };

    // Store count of occurrences
    // in count[]
    for (i = 0; i < n; i++) {
        count[(arr[i] / exp) % 10]++;
        comparisons++;
    }

    // Change count[i] so that count[i]
    // now contains actual position
    // of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Build the output array
    for (i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
        comparisons++;
    }

    // Copy the output array to arr[],
    // so that arr[] now contains sorted
    // numbers according to current digit
    for (i = 0; i < n; i++)
        arr[i] = output[i];
}

template<>
void CountSort(std::vector<std::string>& arr, int exp) {
    int size = (int)arr.size();

    std::vector<std::string> arrCopy(size);
    std::vector<int> characterFrequency(257, 0);

    // count occurrences (using +1 offset so "no char at position k" maps to index 0)
    for (size_t j = 0; j < size; j++) {
        size_t idx = (exp < arr[j].size()) ? (size_t)(unsigned char)arr[j][exp] + 1 : 0;
        characterFrequency[idx]++;
        comparisons++;
    }

    // prefix sums
    for (int f = 1; f < 257; f++) {
        characterFrequency[f] += characterFrequency[f - 1];
    }

    // build sorted output (stable, iterate backwards)
    for (int r = (int)size - 1; r >= 0; r--) {
        size_t idx = (exp < arr[r].size()) ? (size_t)(unsigned char)arr[r][ exp] + 1 : 0;
        arrCopy[characterFrequency[idx] - 1] = arr[r];
        characterFrequency[idx]--;
        comparisons++;
    }

    // copy back
    arr = arrCopy;
}

template <typename T>
void RadixSort(std::vector<T> arr, std::vector<T>& output) {
    int max = GetMaxInArray(arr);

    if (typeid(T) == typeid(std::string)) {
        for (int digit = max; digit > 0; digit--){
            CountSort(arr, digit - 1);
            passes++;
        }
    } else {
        for (int exp = 1; max / exp > 0; exp *= 10) {
            CountSort(arr, exp);
            passes++;
        }
    }

    output = arr;
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

    // int actualMin = *std::min_element(arr.begin(), arr.end());
    // int actualMax = *std::max_element(arr.begin(), arr.end());
    // std::cout << "n=" << arr.size() << " actual range: [" << actualMin << ", " << actualMax << "]\n";
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

void TestCasesString(std::vector<std::string>& arr) {

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
    float bigO = (sortType == 0 ? n * std::log2f(n) : n * passes); // n * k

    float compBigORatio = (float)comparisons / bigO;

    std::cout << "Ratio: " << compBigORatio << std::endl;

    return compBigORatio; // just in case
}

// -------------- BENCHMARKING --------------
struct BenchResult {
    int n;
    int k;              // number of passes for radix
    long long comparisons;
};

void RunBenchmarks() {
    std::vector<int> sizes = {100, 1000, 10000, 100000, 1000000};

    std::cout << std::left << std::setw(10) << "n"
              << std::setw(15) << "comparisons"
              << std::setw(15) << "comp/(n*k)"
              << std::setw(15) << "comp/(n log n)" << std::endl;

    for (int n : sizes) {
        comparisons = passes = 0;

        std::vector<int> benchmarkArr;
        TestCasesIntegers(benchmarkArr, n, 100000, 999999);

        std::vector<int> sorted;

        RadixSort(benchmarkArr, sorted);

        float nk_ratio  = (float)comparisons / ((float)n * passes);
        float nlogn_ratio = (float)comparisons / ((float)n * std::log2f((float)n));

        std::cout << std::setw(10) << n
                  << std::setw(15) << comparisons
                  << std::setw(15) << nk_ratio
                  << std::setw(15) << nlogn_ratio << std::endl;
    }
}

// -------------- MAIN --------------
int main() {
    std::vector<int> testDataArray; // only initialise float to demonstrate limitation case
    std::vector<int> outputArrayInt;

    std::string userInput;

    bool runProgram = true;
    while (runProgram) {
        // reset user input, comparison, testdataarray at every loop start
        userInput.clear();
        comparisons = 0;
        passes = 0;
        ClearArray(testDataArray);
        ClearArray(outputArrayInt);

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
        // CASE 1: Heap vs Radix, integer, N
        } else if (userInput == "1") {
            TestCasesIntegers(testDataArray, 100, 0, 100);
        } else if (userInput == "2") {
            //testDataArray = std::vector<int>{10, 56, 3, 23, 90, 22, 34, 18, 90};
            //testDataArray = std::vector<int>{90, 80, 70, 60, 50, 40, 30, 12, 1};
            //TestCasesIntegers(testDataArray, 42, 0, 100);
            //TestCasesIntegersOrder(testDataArray, 42, 0, 100);
            TestCasesIntegersOrderReversed(testDataArray, 42, 0, 100);

            DisplayArray(testDataArray);
            HeapSort(testDataArray, outputArrayInt);
            DisplayArray(outputArrayInt);
            DisplayComparisonOne(outputArrayInt, 0);

            std::vector<std::string> testDataArray2 = {"book", "tree", "ship", "roll", "poll", "awes"};
            std::vector<std::string> outputArrayInt2;
            DisplayArray(testDataArray2);
            HeapSort(testDataArray2, outputArrayInt2);
            DisplayArray(outputArrayInt2);
            DisplayComparisonOne(outputArrayInt2, 0);
        } else if (userInput == "3") {
            //TestCasesIntegersOrderReversed(testDataArray, 42, 0, 100);
            TestCasesIntegers(testDataArray, 42, 0, 100);

            DisplayArray(testDataArray);
            RadixSort(testDataArray, outputArrayInt);
            DisplayArray(outputArrayInt);
            DisplayComparisonOne(outputArrayInt, 1);

            std::vector<std::string> testDataArray2 = {"book", "tree", "ship", "roll", "poll", "awes"};
            std::vector<std::string> outputArrayInt2;
            DisplayArray(testDataArray2);
            RadixSort(testDataArray2, outputArrayInt2);
            DisplayArray(outputArrayInt2);
            DisplayComparisonOne(outputArrayInt2, 1);
        } else if (userInput == "4") {
            RunBenchmarks();
        }

        // Generate Vega-Lite JSON specification
        //nlohmann::json spec = generate_vega_lite_spec(sizes, times1, times2, times3);

        // Generate HTML file
        //generate_html(spec, "bubble_sort_timings.html");
    }

    return 0;
}