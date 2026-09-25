#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <typeinfo>
#include <iomanip>

//#include "nlohmann_json.h"
//#include "vega_lite_plot.h"

// Base structure of sorts adapted from GeeksForGeeks

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
    if (l < n) {
        comparisons++;
        if (arr[l] > arr[largest]) largest = l;
    }

    // If right child is larger than largest so far
    if (r < n) {
        comparisons++;
        if (arr[r] > arr[largest]) largest = r;
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
    if (arr.empty()) { output = arr; return; }
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
    std::vector<int> output(n);
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
    if (arr.empty()) { output = arr; return; }
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

// -------------- TEST CASES GENERATION --------------
float Mapper(float inputStart, float inputEnd, float outputStart, float outputEnd, float input) {
    return outputStart + ((outputEnd - outputStart) / (inputEnd - inputStart)) * (input - inputStart);
}

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

void TestCasesVariableStrings(std::vector<std::string>& arr, int num = 100, int minLen = 1, int maxLen = 10) {
    for (int i = 0; i < num; i++) {
        int len = (int)Mapper(-1.0, 1.0, minLen, maxLen, distribution(generator));
        std::string s;
        for (int j = 0; j < len; j++) {
            int charCode = (int)Mapper(-1.0, 1.0, (int)'a', (int)'z', distribution(generator));
            s += (char)charCode;
        }
        arr.push_back(s);
    }
}

void TestCasesSameInteger(std::vector<int>& arr, int num = 100, int lowerRange = -10, int upperRange = 10) {
    int number = (int)Mapper(-1.0, 1.0, lowerRange, upperRange, distribution(generator));
    for (int i = 0; i < num; i++) {
        arr.push_back(number);
    }
}

void TestCasesSameString(std::vector<std::string>& arr, int num = 100, int minLen = 1, int maxLen = 10) {
    int len = (int)Mapper(-1.0, 1.0, minLen, maxLen, distribution(generator));
    std::string s;
    for (int j = 0; j < len; j++) {
        int charCode = (int)Mapper(-1.0, 1.0, (int)'a', (int)'z', distribution(generator));
        s += (char)charCode;
    }

    for (int i = 0; i < num; i++) {
        arr.push_back(s);
    }
}

// -------------- VECTOR-RELATED HELPERS --------------
template <typename T>
void ClearArray(std::vector<T>& arr) {
    arr.clear();
}

template <typename T>
void DisplayArray(std::vector<T>& arr, std::string header = " -- CURRENT ARRAY CONTENTS --\n") {
    std::cout << header;
    for (T& var : arr) {
        std::cout << var << " ";
    }
    std::cout << std::endl;
}

// -------------- TEST CASES --------------
struct TestResult {
    int n = 0;
    int comparisons = 0;
    int passes = 0;
    float nk_ratio = 0;
    float nlogn_ratio = 0;
    float n_ratio = 0;
    int chosen_int = 0;
};

float FindNK(int n) {
    return (float)comparisons / ((float)n * passes);
}

float FindNLogN(int n) {
    return (float)comparisons / ((float)n * std::log2f((float)n));
}

float FindN(int n) {
    return (float)comparisons / ((float)n);
}

void TestOne() {
    std::vector<int> sizes = {100, 1000, 10000, 100000, 1000000, 10000000};
    std::vector<TestResult> resultsHeap;
    std::vector<TestResult> resultsRadix;

    std::cout << "TEST 1: HEAP VS RADIX BENCHMARKING: N = 100 to 10000000\n";
    for (int n : sizes) {
        comparisons = passes = 0; // reset

        std::vector<int> benchmarkArr;
        TestCasesIntegers(benchmarkArr, n, 100000, 999999);

        std::vector<int> sortedHeap;
        std::vector<int> sortedRadix;

        HeapSort(benchmarkArr, sortedHeap);
        resultsHeap.push_back(TestResult{n, comparisons, passes, FindNK(n), FindNLogN(n)});

        comparisons = passes = 0; // reset
        RadixSort(benchmarkArr, sortedRadix);
        resultsRadix.push_back({n, comparisons, passes, FindNK(n), FindNLogN(n)});
    }

    std::cout << "======== HEAP SORT ========\n";
    std::cout << std::left << std::setw(10) << "n"
              << std::setw(15) << "comparisons"
              << std::setw(15) << "comp/(n*k)"
              << std::setw(15) << "comp/(n log n)" << std::endl;
    for (TestResult tr : resultsHeap) {
        std::cout << std::left << std::setw(10) << tr.n
              << std::setw(15) << tr.comparisons
              << std::setw(15) << tr.nk_ratio
              << std::setw(15) << tr.nlogn_ratio << std::endl;
    }

    std::cout << std::endl;
    std::cout << "======== RADIX SORT ========\n";
    std::cout << std::left << std::setw(10) << "n"
              << std::setw(15) << "comparisons"
              << std::setw(15) << "comp/(n*k)"
              << std::setw(15) << "comp/(n log n)" << std::endl;
    for (TestResult tr : resultsRadix) {
        std::cout << std::left << std::setw(10) << tr.n
              << std::setw(15) << tr.comparisons
              << std::setw(15) << tr.nk_ratio
              << std::setw(15) << tr.nlogn_ratio << std::endl;
    }
}

void TestTwo() {
    std::vector<int> sizes = {100, 1000, 10000, 100000, 1000000};
    std::vector<TestResult> resultsHeap;
    std::vector<TestResult> resultsRadix;

    std::cout << "TEST 2: HEAP VS RADIX BENCHMARKING: N = 100 to 1000000\n";
    for (int n : sizes) {
        comparisons = passes = 0; // reset

        std::vector<std::string> benchmarkArr;
        TestCasesVariableStrings(benchmarkArr, n, 6, 6);

        std::vector<std::string> sortedHeap;
        std::vector<std::string> sortedRadix;

        HeapSort(benchmarkArr, sortedHeap);
        resultsHeap.push_back(TestResult{n, comparisons, passes, FindNK(n), FindNLogN(n)});

        comparisons = passes = 0; // reset
        RadixSort(benchmarkArr, sortedRadix);
        resultsRadix.push_back({n, comparisons, passes, FindNK(n), FindNLogN(n)});
    }

    std::cout << "======== HEAP SORT ========\n";
    std::cout << std::left << std::setw(10) << "n"
              << std::setw(15) << "comparisons"
              << std::setw(15) << "comp/(n*k)"
              << std::setw(15) << "comp/(n log n)" << std::endl;
    for (TestResult tr : resultsHeap) {
        std::cout << std::left << std::setw(10) << tr.n
              << std::setw(15) << tr.comparisons
              << std::setw(15) << tr.nk_ratio
              << std::setw(15) << tr.nlogn_ratio << std::endl;
    }

    std::cout << std::endl;
    std::cout << "======== RADIX SORT ========\n";
    std::cout << std::left << std::setw(10) << "n"
              << std::setw(15) << "comparisons"
              << std::setw(15) << "comp/(n*k)"
              << std::setw(15) << "comp/(n log n)" << std::endl;
    for (TestResult tr : resultsRadix) {
        std::cout << std::left << std::setw(10) << tr.n
              << std::setw(15) << tr.comparisons
              << std::setw(15) << tr.nk_ratio
              << std::setw(15) << tr.nlogn_ratio << std::endl;
    }
}

void TestThree() {
    std::cout << "TEST 3: CORRECTNESS\n";

    // empty, single. sorted, reversed, dupes, negatives
    std::vector<int> tests = {0, 1, 2, 3, 4, 5};
    for (int test : tests) {
        std::vector<int> testArray;
        switch (test) {
            case 0:
                // nothing to do. it is empty
                std::cout << "== 3A - EMPTY ARRAY ==\n";
                break;
            case 1:
                std::cout << "== 3B - SINGLE ELEMENT ==\n";
                TestCasesIntegers(testArray, 1, 0, 100);
                break;
            case 2:
                std::cout << "== 3C - SORTED ==\n";
                TestCasesIntegersOrder(testArray, 30, 0, 100);
                break;
            case 3:
                std::cout << "== 3D - SORTED REVERSED ==\n";
                TestCasesIntegersOrderReversed(testArray, 30, 0, 100);
                break;
            case 4:
                std::cout << "== 3E - DUPLICATES ==\n";
                TestCasesSameInteger(testArray, 30, 0, 100);
                break;
            case 5:
                std::cout << "== 3F - NEGATIVES ==\n";
                TestCasesIntegers(testArray, 30, -100, -1);
                break;
            default: break;
        }

        DisplayArray(testArray, "INITIAL ARRAY: ");

        std::vector<int> heap;
        std::vector<int> radix;

        HeapSort(testArray, heap);
        RadixSort(testArray, radix);

        DisplayArray(heap, "HEAP: ");
        DisplayArray(radix, "RADIX: ");
        std::cout << std::endl;
    }
}

void TestFour() {
    std::vector<int> sizes = {100, 1000, 10000, 100000, 1000000, 10000000};
    std::vector<TestResult> resultsRadixConst;
    std::vector<TestResult> resultsRadixVariable;

    std::cout << "TEST 4: RADIX SORT DEGRADATION\n";
    for (int n : sizes) {
        comparisons = passes = 0; // reset

        std::vector<int> benchmarkArr1;
        TestCasesIntegers(benchmarkArr1, n, 100000, 999999);

        std::vector<int> benchmarkArr2;
        TestCasesIntegers(benchmarkArr2, n, 0, n - 1);

        std::vector<int> sortedRadix1;
        std::vector<int> sortedRadix2;

        RadixSort(benchmarkArr1, sortedRadix1);
        resultsRadixConst.push_back(TestResult{n, comparisons, passes, FindNK(n), FindNLogN(n)});

        comparisons = passes = 0; // reset
        RadixSort(benchmarkArr2, sortedRadix2);
        resultsRadixVariable.push_back({n, comparisons, passes, FindNK(n), FindNLogN(n)});
    }

    std::cout << "======== RADIX SORT (K = 6, CONSTANT) ========\n";
    std::cout << std::left << std::setw(10) << "n"
              << std::setw(15) << "k"
              << std::setw(15) << "comparisons"
              << std::setw(15) << "comp/(n*k)"
              << std::setw(15) << "comp/(n log n)" << std::endl;
    for (TestResult tr : resultsRadixConst) {
        std::cout << std::left << std::setw(10) << tr.n
              << std::setw(15) << tr.passes
              << std::setw(15) << tr.comparisons
              << std::setw(15) << tr.nk_ratio
              << std::setw(15) << tr.nlogn_ratio << std::endl;
    }

    std::cout << std::endl;
    std::cout << "======== RADIX SORT (K VARIABLE) ========\n";
    std::cout << std::left << std::setw(10) << "n"
              << std::setw(15) << "k"
              << std::setw(15) << "comparisons"
              << std::setw(15) << "comp/(n*k)"
              << std::setw(15) << "comp/(n log n)" << std::endl;
    for (TestResult tr : resultsRadixVariable) {
        std::cout << std::left << std::setw(10) << tr.n
              << std::setw(15) << tr.passes
              << std::setw(15) << tr.comparisons
              << std::setw(15) << tr.nk_ratio
              << std::setw(15) << tr.nlogn_ratio << std::endl;
    }
}

void TestFive() {
    std::cout << "TEST 5: HEAP SORT SPECIAL CASE (EQUAL KEYS)\n";

    std::vector<int> sizes = {100, 1000, 10000, 100000};
    std::vector<TestResult> resultsHeap;

    for (int n : sizes) {
        comparisons = passes = 0; // reset

        std::vector<int> benchmarkArr;
        TestCasesSameInteger(benchmarkArr, n, 1, 100);

        std::vector<int> sortedHeap;

        HeapSort(benchmarkArr, sortedHeap);
        resultsHeap.push_back(TestResult{n, comparisons, passes, FindNK(n), FindNLogN(n), FindN(n), sortedHeap[0]});
    }

    std::cout << "======== HEAP SORT ========\n";
    std::cout << std::left << std::setw(15) << "chosen int"
              << std::setw(15) << "n"
              << std::setw(15) << "comparisons"
              << std::setw(15) << "comp/(n log n)"
              << std::setw(15) << "comp/(n)" << std::endl;
    for (TestResult tr : resultsHeap) {
        std::cout << std::left << std::setw(15)  << tr.chosen_int
              << std::setw(15) << tr.n
              << std::setw(15) << tr.comparisons
              << std::setw(15) << tr.nlogn_ratio
              << std::setw(15) << tr.n_ratio << std::endl;
    }
}

// -------------- MAIN --------------
int main() {
    std::string userInput;

    bool runProgram = true;
    while (runProgram) {
        // reset user input, comparison, testdataarray at every loop start
        userInput.clear();
        comparisons = 0;
        passes = 0;

        std::cout << "\n========= CSD3131 ASSIGNMENT 1: Q3B =========\n";
        std::cout << "OPTIONS:\n";
        std::cout << " 1 - Heap vs Radix Benchmark (6-digit Integers), n = 100 to 10000000\n";
        std::cout << " 2 - Heap vs Radix Benchmark (6-letter Strings), n = 100 to 1000000\n";
        std::cout << " 3 - Correctness Tests (empty, single, sorted, reversed, duplicates, negatives)\n";
        std::cout << " 4 - Radix Degradation Demo (key width scales with n)\n";
        std::cout << " 5 - Heap Sort special case (equal keys)\n";
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
            runProgram = false;
        // CASE 1: Benchmarking Radix and Heap (INT), n = 100 - 10000000
        } else if (userInput == "1") {
            TestOne();
        // CASE 2: Benchmarking Radix and Heap (STR), n = 100 - 10000
        } else if (userInput == "2") {
            TestTwo();
        // CASE 3: Correctness for both sorts
        } else if (userInput == "3") {
            TestThree();
        // CASE 4: Radix degradation
        } else if (userInput == "4") {
            TestFour();
        // CASE 5: Heap special case
        } else if (userInput == "5") {
            TestFive();
        }
    }

    return 0;
}

// -------------- OUTPUT-RELATED HELPERS --------------
// template <typename T>
// float DisplayComparisonOne(std::vector<T>& arr, int sortType) {
//     int n = arr.size();
//     // 0 - n log n, 1 - n * k
//     float bigO = (sortType == 0 ? n * std::log2f(n) : n * passes); // n * k

//     float compBigORatio = (float)comparisons / bigO;

//     std::cout << "Ratio: " << compBigORatio << std::endl;

//     return compBigORatio; // just in case
// }

//testDataArray = std::vector<int>{10, 56, 3, 23, 90, 22, 34, 18, 90};
//testDataArray = std::vector<int>{90, 80, 70, 60, 50, 40, 30, 12, 1};
//TestCasesIntegers(testDataArray, 42, 0, 100);
//TestCasesIntegersOrder(testDataArray, 42, 0, 100);
// TestCasesIntegersOrderReversed(testDataArray, 42, 0, 100);

// DisplayArray(testDataArray);
// HeapSort(testDataArray, outputArrayInt);
// DisplayArray(outputArrayInt);
// DisplayComparisonOne(outputArrayInt, 0);

// std::vector<std::string> testDataArray2 = {"book", "tree", "ship", "roll", "poll", "awes"};
// std::vector<std::string> outputArrayInt2;
// DisplayArray(testDataArray2);
// HeapSort(testDataArray2, outputArrayInt2);
// DisplayArray(outputArrayInt2);
// DisplayComparisonOne(outputArrayInt2, 0);

//TestCasesIntegersOrderReversed(testDataArray, 42, 0, 100);
// TestCasesIntegers(testDataArray, 42, 0, 100);

// DisplayArray(testDataArray);
// RadixSort(testDataArray, outputArrayInt);
// DisplayArray(outputArrayInt);
// DisplayComparisonOne(outputArrayInt, 1);

// std::vector<std::string> testDataArray2 = {"book", "tree", "ship", "roll", "poll", "awes"};
// std::vector<std::string> outputArrayInt2;
// DisplayArray(testDataArray2);
// RadixSort(testDataArray2, outputArrayInt2);
// DisplayArray(outputArrayInt2);
// DisplayComparisonOne(outputArrayInt2, 1);