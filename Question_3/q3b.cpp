#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>

// References:
// https://www.geeksforgeeks.org/cpp/how-to-make-a-countdown-timer-in-cpp/
// https://www.geeksforgeeks.org/dsa/understanding-time-complexity-simple-examples/
// https://www.geeksforgeeks.org/cpp/measure-execution-time-function-cpp/
// https://www.geeksforgeeks.org/dsa/analysis-algorithms-big-o-analysis/
// https://github.com/Cheejyg/Integration-of-Mergesort-and-Insertion-Sort

// https://en.wikipedia.org/wiki/Heapsort
// https://www.geeksforgeeks.org/dsa/heap-sort/

// Radix Sort
// O(n * k), number of digits in largest number
// sorts nums digit by digit using stable subroutine like Counting Sort
// beats O(n log n) when k is small or constant (linear time)
// limitation: only works efficiently on integers or fixed length strings

// test cases: integer only, maybe float as a bonus limitation for radix

// -------------- ALGORITHM 1: A O(n log n) algorithm - Heap Sort --------------
template <typename T>
void Heapify(std::vector<T>& arr) {
    
}

template <typename T>
void HeapSort(std::vector<T>& arr) {

}

// -------------- ALGORITHM 2: Radix Sort --------------
template <typename T>
void RadixSort(std::vector<T>& arr) {
    
}

// -------------- TEST CASES HELPERS --------------


// -------------- VECTOR-RELATED HELPERS --------------
template <typename T>
bool PopulateArray(std::vector<T>& arr, std::string filepath) {
    
}

template <typename T>
void ClearArray(std::vector<T>& arr) {
    arr.clear();
}

// -------------- MAIN --------------
int main() {
    std::vector<int> testDataArray; // only initialise float to demonstrate limitation case

    std::string userInput;

    bool runProgram = true;
    while (runProgram) {
        userInput.clear();

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
        }
    }

    return 0;
}