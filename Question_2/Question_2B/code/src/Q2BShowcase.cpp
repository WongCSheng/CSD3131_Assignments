/**
 * @file Question2BShowcase.cpp
 * @author Wong Chong Sheng (w.chongsheng@digipen.edu)
 * @brief Program to showcase the implementation of Question 2B.
 *      The following program shows the usage of the substitution method, recursive method, and master method
 *      which satisfies the time complexity T(n) = 3T(n/3) + lg(n), to solve a complex problem.
 * @date 23-09-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <filesystem>

#include "../header/recursiveFunc.h"
#include "../header/Livability_Score_Calculator.hpp"
#include "../header/GSTVoucher.hpp"

#define data_path std::filesystem::current_path().string() + "/data/"

/*
* From T(n) = 3·T(n/3) + lg(n), we can tell that the recursive method splits into 3 subproblems of size n/3,
* and the time complexity for each function call is lg(n). For each function call to be lg(n), there has to be a
* split 
*/

/**
 * @brief An example of a real life scenerio where T(n) = 3·T(n/3) + lg(n), would be 
 * 
 */

void RecursiveTest()
{
    std::cout << "Recursive Test choosen.\nEnter number of recursive calls: ";
    int n;
    std::cin >> n;
    auto start_time = std::chrono::high_resolution_clock::now();
    recursiveExample(n);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
}

/**
 * @brief An example of a merge sort with 3-way split, which satisfies the recurrence relation T(n) = 3·T(n/3) + lg(n)
 * 
 */
void threeWaySortExample()
{
    std::cout << "Three-way Merge Sort choosen.\nEnter number of elements to sort: ";
    int n;
    std::vector<int> arr;
    std::cin >> n;
    std::cout << "Enter " << n << " elements to sort:\n";
    for(int i = 0; i < n; ++i)
    {
        int element;
        std::cin >> element;
        arr.push_back(element);
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    threeWayMergeSort(arr, 0, n - 1);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    printArray(arr);
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
}

/**
 * @brief List out all the files in the data folder.
 * 
 * @return std::vector<std::string> 
 */
std::vector<std::string> dataFileList()
{
    std::vector<std::string> files;
    for(const auto& entry : std::filesystem::directory_iterator(data_path))
    {
        if(entry.is_regular_file())
        {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;
}

/**
 * @brief Filter and sort eligible individuals for GST Voucher based on their age, job, income, and property ownership.
 * 
 */
void GSTVoucherExample()
{
    std::cout << "Choose which data file to process for GST Voucher Eligibility:\n";
    std::vector<std::string> files = dataFileList();
    size_t selectedFileIndex = 0;

    for(size_t i = 0; i < files.size(); ++i)
    {
        std::cout << i + 1 << ". " << files[i] << std::endl;
    }

    std::cin >> selectedFileIndex;

    if (selectedFileIndex > 0 && selectedFileIndex <= files.size())
    {
        std::string filename = files[selectedFileIndex - 1];
        std::vector<Person> people = readCSV(data_path + filename);
        auto start_time = std::chrono::high_resolution_clock::now();
        processSegmentRecursive(people, 0, people.size() - 1);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
    }
    else
    {
        std::cout << "Invalid file selection." << std::endl;
    }
}

void livibilityScoreExample()
{
    std::cout << "Livability Score Calculator choosen.\nEnter number of land parcels: ";
    int n;
    std::cin >> n;
    std::vector<int> land_parcels(n);
    std::cout << "Enter the livability scores for each land parcel:\n";
    for(int i = 0; i < n; ++i)
    {
        std::cin >> land_parcels[i];
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    int score = calculateLivabilityScore(land_parcels, 0, n);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Total Livability Score: " << score << std::endl;
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
}

int main()
{    
    int selectedMethod = 0;
    
    std::cout << "Enter which method you want to view:\n1. Recursive Test\n2. Three way Merge Sort\n3. GST Voucher Eligibility Checker\n4. Livability Score Calculator\n9. Exit\n";

    while(std::cin >> selectedMethod)
    {
        // Exit the program if the user inputs 4
        if(selectedMethod == 9)
        {
            std::cout << "Exiting program." << std::endl;
            break;
        }

        switch(selectedMethod)
        {
            case 1:
                RecursiveTest();
                break;
            case 2:
                threeWaySortExample();
                break;
            case 3:
                GSTVoucherExample();
                break;
            case 4:
                livibilityScoreExample();
                break;
            default:
                std::cout << "Invalid input. Please try again." << std::endl;
        }

        selectedMethod = 0;
    
        std::cout << "Enter which method you want to view:\n1. Substitution Method\n2. Three-way Merge Sort\n3. Master Method\n9. Exit\n";
    }

    return 0;
}